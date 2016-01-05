/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 - 2015 Freescale Semiconductor;
 * All Rights Reserved
 *
 * Copyright (c) 1989-2008 ARC International;
 * All Rights Reserved
 *
 *************************************************************************** 
 *
 * THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
 * IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************
 *
 * $FileName: cdc_serial.c$
 * $Version : 3.8.28.0$
 * $Date    : Sep-13-2012$
 *
 * Comments:
 *
 *   This file is an example of device drivers for the CDC class. This example
 *   demonstrates the virtual serial port capability with abstract control model.
 *   Redirects the communication from CDC device, which is connected to the board,
 *   to the serial port ttyB.
 *
 *END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_hub_sm.h"
#include "usb_host_cdc.h"
#include "usb_host_cdc_io.h"
#include "cdc_serial.h"


//#include "MK70F12_port.h"
//#include "fsl_usb_features.h"
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
//#include "board.h"
#include "fsl_debug_console.h"

#include "fsl_port_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "usb_uart_drv.h"

/***************************************
 **
 ** Macros
 */
#define FILE_PTR file_device_struct_t *

#define CDC_SERIAL_DELIMITER_CR '\r'
#define CDC_SERIAL_DELIMITER_LF '\n'

/* CDC work mode configure */
#define CDC_MAX_PKT_SIZE 64
#define CDC_BUF_MAX 800
#define CDC_UART_RX_MAX 256
#define CDC_USB_BUF_MAX 200
#define CDC_USB_RETRY_MAX 2

/* CDC IO lock/unlock */
#define CDC_SERIAL_IO_lock()                OS_Lock()
#define CDC_SERIAL_IO_unlock()              OS_Unlock()
/***************************************
 **
 ** Globals
 */
extern void Main_Task(uint32_t param);
#if defined(__cplusplus)
extern "C"
{
#endif
    extern usb_host_handle usb_host_dev_mng_get_host(usb_device_instance_handle dev_handle);
#if defined(__cplusplus)
}
#endif
extern void USB_UART_DRV_IRQHandler(uint32_t instance);
/* Table of driver capabilities this application wants to use */
static usb_host_driver_info_t DriverInfoTable[] =
{
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_COMMUNICATION,      /* Class code                       */
      USB_SUBCLASS_COM_ABSTRACT,    /* Sub-Class code                   */
      0xFF,                         /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_cdc_acm_event        /* Application call back function   */
   },
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_DATA,               /* Class code                       */
      0xFF,                         /* Sub-Class code                   */
      0xFF,                         /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_cdc_data_event           /* Application call back function   */
   },
   /* USB 1.1 hub */
   {

      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_HUB,                /* Class code                       */
      USB_SUBCLASS_HUB_NONE,        /* Sub-Class code                   */
      USB_PROTOCOL_HUB_ALL,          /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_hub_device_event     /* Application call back function   */
   },
   {
      {0x00,0x00},                  /* All-zero entry terminates        */
      {0x00,0x00},                  /* driver info list.                */
      0,
      0,
      0,
      0,
      NULL
   }
};

char * device_name = "tty0:";
const cdc_serial_init_t usb_open_param = {
    #if CDC_EXAMPLE_USE_HW_FLOW
    USB_UART_NO_BLOCKING | USB_UART_HW_FLOW
#else
    USB_UART_NO_BLOCKING
#endif
};
usb_device_instance_handle reg_device = 0;

usb_interface_descriptor_handle acm_intf_handle = NULL;
usb_interface_descriptor_handle data_intf_handle = NULL;

extern file_cdc_t * f_usb;

/***************************************
 **
 ** Locals
 */
static f_usb_info_t s_f_usb_info = {NULL, 0, CDC_SERIAL_DETACHED};
static char usb2uart_buffer[CDC_BUF_MAX];
static char uart2usb_buffer[CDC_BUF_MAX];
static char uart_rx_buf[CDC_UART_RX_MAX];
static char usb_tx_buf[CDC_USB_BUF_MAX];
static char usb_rx_buf[CDC_USB_BUF_MAX];
static int usb_tx_remain_cnt = 0;
static int usb_tx_done = 0;
static char * uart2usb_buf_ptr_in = NULL;
static char * uart2usb_buf_ptr_out = NULL;
static char * usb2uart_buf_ptr_in = NULL;
static char * usb2uart_buf_ptr_out = NULL;
static acm_device_struct_t acm_device;
static data_device_struct_t data_device;
static uint32_t s_feed = 0;
static uint8_t s_retry = CDC_USB_RETRY_MAX;
static os_mutex_handle s_cdc_serial_io_mutex = NULL;
static usb_uart_state_t s_uartState;
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : _cdc_serial_set_state
 * Returned Value : USB_OK if set status successfully
 * Comments       :
 *     This operate in block mode.
 *
 *END*--------------------------------------------------------------------*/
static usb_status _cdc_serial_set_state(CDC_SERIAL_STS_E sts)
{
    usb_status ret;
    if (sts < CDC_SERIAL_STS_INVALID)
    {
        s_f_usb_info.state = sts;
        ret = USB_OK;
    }
    else
    {
        ret = USBERR_ERROR;
    }
    return ret;
}
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : _cdc_serial_os_task_suspend
 * Returned Value : OS_TASK_OK if block successfully
 * Comments       :
 *    
 *
 *END*--------------------------------------------------------------------*/
static uint32_t _cdc_serial_os_task_suspend(void)
{
    uint32_t ret;
    ret = 1;
    return ret;
}

#if CDC_ASYNC
volatile static cdc_xfer_t s_cdc_xfer = {CDC_TX_IDLE, CDC_RX_IDLE, 0, 0};

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : _cdc_serial_tx_callback
 * Returned Value : callback when tx done
 * Comments       :
 *
 *END*--------------------------------------------------------------------*/
void _cdc_serial_tx_callback(void *param)
{
    static uint32_t tx_cb_i = 0;
    tx_cb_i++;
    s_cdc_xfer.tx_sts = CDC_TX_DONE;
    s_cdc_xfer.tx_num_done = *((int32_t*)param);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : _cdc_serial_rx_callback
 * Returned Value : callback when tx done
 * Comments       :
 *
 *END*--------------------------------------------------------------------*/
void _cdc_serial_rx_callback(void *param)
{
    s_cdc_xfer.rx_sts = CDC_RX_DONE;
    s_cdc_xfer.rx_num_done = *((int32_t*)param);
}
#endif

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : _cdc_serial_send_element
 * Returned Value : CDC_ERROR or number of characters sent
 * Comments         :
 *      Execution starts here
 *
 *END*--------------------------------------------------------------------*/
static int32_t _cdc_serial_send_element(void)
{
    usb_uart_status_t ret;
    uint32_t sent_num = 0;
    uint32_t remained = 0;

    while (usb2uart_buf_ptr_out != usb2uart_buf_ptr_in)
    {
        if (usb2uart_buf_ptr_out >= &usb2uart_buffer[CDC_BUF_MAX])
        {
            usb2uart_buf_ptr_out = &usb2uart_buffer[0];
        }
        sent_num = (usb2uart_buf_ptr_in > usb2uart_buf_ptr_out ? (usb2uart_buf_ptr_in - usb2uart_buf_ptr_out) : (&usb2uart_buffer[CDC_BUF_MAX] - usb2uart_buf_ptr_out));
        //sent_num = 1;
        if (kStatus_USB_UART_TxBusy != USB_UART_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, &remained))
        {
            ret = USB_UART_DRV_SendData(BOARD_DEBUG_UART_INSTANCE, (uint8_t *)usb2uart_buf_ptr_out, sent_num);
            if (ret == kStatus_USB_UART_Success)
            {
                /* send out successfully */
                usb2uart_buf_ptr_out += sent_num;
            }
        }
        else
        {
            break;
        }
    }
    return ret;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : _copy_to_usb2uart_buf
 * Returned Value : none
 * Comments         :
 *      Execution starts here
 *
 *END*--------------------------------------------------------------------*/
static void _copy_to_usb2uart_buf(uint32_t num)
{
    uint32_t i = 0;
    while (num)
    {
        if (usb2uart_buf_ptr_in >= &usb2uart_buffer[CDC_BUF_MAX])
        {
            usb2uart_buf_ptr_in = &usb2uart_buffer[0];
        }
        *(usb2uart_buf_ptr_in++) = usb_rx_buf[i++];
        if (usb2uart_buf_ptr_in == usb2uart_buf_ptr_out)
        {
            USB_PRINTF("\nusb2uart_buf overflow");
        }
        num--;
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb2uart_task
 * Returned Value : none
 * Comments       :
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/
void usb2uart_task(uint32_t param)
{ /* Body */
    int num_done = 0;
    uint32_t remained = 0;
    num_done = 0;
    /* read data from USB */
#if CDC_ASYNC
    switch(s_cdc_xfer.rx_sts)
    {
        case CDC_RX_WAIT:
        return;
        break;

        case CDC_RX_IDLE:
        if(s_feed > 0)
        {
            s_cdc_xfer.rx_sts = CDC_RX_WAIT;
            if(IO_OK != _io_cdc_serial_read_async(s_f_usb_info.f_usb, usb_rx_buf, CDC_USB_BUF_MAX, _cdc_serial_rx_callback))
            {
                s_cdc_xfer.rx_sts = CDC_RX_IDLE;
                while(kStatus_USB_UART_TxBusy == USB_UART_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, &remained))
                {
                    ;
                }
                USB_PRINTF("\n_io_cdc_serial_read_async error");
            }
        }
        break;
        case CDC_RX_DONE:
        num_done = s_cdc_xfer.rx_num_done;
        if((num_done > 0) || (!s_retry--))
        {
            /* 
             *  There are two cases that would cause num_done becomes zero: [1] Host receives a
             *  zero-length package, [2] Some error occurs during last IN pipe, e.g. bmStates isn't
             *  set properly due to HW_FLOW. For case [2], a retry mechanism shall be taken to
             *  avoid infinite loop in RX cycle. 
             */
            s_retry = CDC_USB_RETRY_MAX;
            s_feed--;
        }
        s_cdc_xfer.rx_sts = CDC_RX_IDLE;
        break;
        default:
        break;
    }
#else
    if (s_feed > 0)
    {
        num_done = _io_cdc_serial_read(s_f_usb_info.f_usb, usb_rx_buf, CDC_USB_BUF_MAX);
        if (num_done > 0)
            s_feed--;
    }
#endif
    if (num_done != IO_ERROR)
    {
        _copy_to_usb2uart_buf(num_done);
    }
    /* send data to UART */
    _cdc_serial_send_element();
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : _copy_to_uart2usb_buf
 * Returned Value : none
 * Comments         :
 *      Execution starts here
 *
 *END*--------------------------------------------------------------------*/
static void _copy_to_uart2usb_buf(uint32_t num)
{
    uint32_t i = 0;
    while (num)
    {
        if (uart2usb_buf_ptr_in >= &uart2usb_buffer[CDC_BUF_MAX])
        {
            uart2usb_buf_ptr_in = &uart2usb_buffer[0];
        }
        *(uart2usb_buf_ptr_in++) = uart_rx_buf[i++];
        if (uart2usb_buf_ptr_in == uart2usb_buf_ptr_out)
        {
            //USB_PRINTF("\nuart2usb_buf overflow");
        }
        num--;
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : _cdc_serial_recv_element
 * Returned Value : CDC_ERROR or number of characters ready to be sent
 * Comments         :
 *      Execution starts here
 *
 *END*--------------------------------------------------------------------*/
volatile static int32_t s_usb_tx_buf_i = 0;
volatile static int32_t s_last_usb_tx_buf_i = 0;
static int32_t _cdc_serial_recv_element(void)
{
    int32_t ret = 0;
    int32_t last_tx_i_temp, tx_i_temp = 0;
    while (uart2usb_buf_ptr_out != uart2usb_buf_ptr_in)
    {
        if (uart2usb_buf_ptr_out >= &uart2usb_buffer[CDC_BUF_MAX])
        {
            uart2usb_buf_ptr_out = &uart2usb_buffer[0];
        }
#if CDC_SERIAL_ECHO_BACK
        USB_PRINTF("%c", *uart2usb_buf_ptr_out);
#endif
        usb_tx_buf[s_usb_tx_buf_i++] = *(uart2usb_buf_ptr_out++);
        if (CDC_MAX_PKT_SIZE == s_usb_tx_buf_i)
        {
            ret = s_usb_tx_buf_i;
            s_usb_tx_buf_i = 0;
            break;
        }
    }
    /* No more data received and CR or LF encountered */
    last_tx_i_temp = s_last_usb_tx_buf_i;
    tx_i_temp = s_usb_tx_buf_i;
    if ((last_tx_i_temp == tx_i_temp) && (tx_i_temp > 0))
    {
        if ((CDC_SERIAL_DELIMITER_CR == usb_tx_buf[s_usb_tx_buf_i - 1]) ||
        (CDC_SERIAL_DELIMITER_LF == usb_tx_buf[s_usb_tx_buf_i - 1]))
        {
            ret = s_usb_tx_buf_i;
            s_usb_tx_buf_i = 0;
        }
    }

    s_last_usb_tx_buf_i = s_usb_tx_buf_i;
    if (ret == 0)
    {
        ret = CDC_ERROR;
    }
    return ret;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : uart_rx_cb
 * Returned Value : none
 * Comments       :
 *     Callback functioni for UART RX channel.
 *
 *END*--------------------------------------------------------------------*/
void uart_rx_cb(uint32_t instance, void * uartState)
{
    if (CDC_SERIAL_FUSB_OPEND == s_f_usb_info.state)
    {
        _copy_to_uart2usb_buf(1);
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : uart2usb_task
 * Returned Value : none
 * Comments       :
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/
void uart2usb_task(uint32_t param)
{ /* Body */
    int num_done = 0;
    int send_cnt = 0;
    uint32_t remained = 0;

    /* 
     ** The virtual com example on device side won't prime next OUT packet if it hasn't finished sending
     ** its packet just received. So we try a max packet size here to avoid consecutive OUT packets.
     */
#if CDC_ASYNC
    switch(s_cdc_xfer.tx_sts)
    {
        case CDC_TX_WAIT:
        return;
        break;
        case CDC_TX_IDLE:
        if(0 == s_feed)
        {
            if(usb_tx_remain_cnt == 0)
            {
                send_cnt = _cdc_serial_recv_element();
                if(CDC_ERROR != send_cnt)
                {
                    usb_tx_remain_cnt = send_cnt;
                    usb_tx_done = 0;
                }
            }
            else
            {
                send_cnt = usb_tx_remain_cnt;
            }
            if(CDC_ERROR != send_cnt)
            {
                s_cdc_xfer.tx_sts = CDC_TX_WAIT;
                if(IO_OK != _io_cdc_serial_write_async(s_f_usb_info.f_usb, &usb_tx_buf[usb_tx_done], (send_cnt > CDC_MAX_PKT_SIZE? CDC_MAX_PKT_SIZE : send_cnt), _cdc_serial_tx_callback))
                {
                    s_cdc_xfer.tx_sts = CDC_TX_IDLE;
                    while(kStatus_USB_UART_TxBusy == USB_UART_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, &remained))
                    {
                        ;
                    }
                    USB_PRINTF("\n_io_cdc_serial_write_async error");
                }
                num_done = 0;
            }
        }
        break;
        case CDC_TX_DONE:
        s_feed++;
        num_done = s_cdc_xfer.tx_num_done;
        usb_tx_done += num_done;
        usb_tx_remain_cnt -= num_done;
        s_cdc_xfer.tx_sts = CDC_TX_IDLE;
        break;
        default:
        break;
    }
#else
    if (usb_tx_remain_cnt == 0)
    {
        send_cnt = _cdc_serial_recv_element();
        if (CDC_ERROR != send_cnt)
        {
            usb_tx_remain_cnt = send_cnt;
            usb_tx_done = 0;
        }
    }
    else
    {
        send_cnt = usb_tx_remain_cnt;
    }
    if (CDC_ERROR != send_cnt)
    {
        num_done = _io_cdc_serial_write(s_f_usb_info.f_usb, &usb_tx_buf[usb_tx_done], (send_cnt > CDC_MAX_PKT_SIZE ? CDC_MAX_PKT_SIZE : send_cnt));
        s_feed++;
        usb_tx_done += num_done;
        usb_tx_remain_cnt -= num_done;
    }
#endif
    if (IO_ERROR == num_done)
    {
        USB_PRINTF("\nuart2usb_task: IO_ERROR");
    }

}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_cdc_acm_event
 * Returned Value : usb_status
 * Comments       :
 *     Called when acm interface has been attached, detached, etc.
 *END*--------------------------------------------------------------------*/

usb_status usb_host_cdc_acm_event
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,

    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,

    /* [IN] code number for event causing callback */
    uint32_t event_code
    )
{ /* Body */
    switch(event_code)
    {
    case USB_CONFIG_EVENT:
        acm_intf_handle = intf_handle;
        break;
    case USB_ATTACH_EVENT:
    {
//         fflush(stdout);
//         USB_PRINTF("State = attached");
//         USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
//         USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
//         USB_PRINTF("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
//         fflush(stdout);

        break;
    }
    case USB_INTF_OPENED_EVENT:
        {
        cdc_class_call_struct_t * acm_parser;
        usb_status status;
        usb_cdc_desc_acm_t * acm_desc = NULL;
        usb_cdc_desc_cm_t * cm_desc = NULL;
        usb_cdc_desc_header_t * header_desc = NULL;
        usb_cdc_desc_union_t * union_desc = NULL;
        /* finds all the descriptors in the configuration */
        if (USB_OK != usb_class_cdc_get_acm_descriptors(dev_handle,
            intf_handle,
            &acm_desc,
            &cm_desc,
            &header_desc,
            &union_desc))
        {
            break;
        }

        /* set all info got from descriptors to the class interface struct */
        usb_class_cdc_set_acm_descriptors((cdc_class_call_struct_t *)&acm_device.CLASS_INTF,
            acm_desc, cm_desc, header_desc, union_desc);
        /* link all already registered data interfaces to this ACM control, if needed */
        if (USB_OK != usb_class_cdc_bind_data_interfaces(dev_handle, (cdc_class_call_struct_t *)&acm_device.CLASS_INTF))
        {
            break;
        }
        if (NULL == (acm_parser = usb_class_cdc_get_ctrl_interface(intf_handle)))
        {
            break;
        }
        if (USB_OK != usb_class_cdc_acm_use_lwevent(acm_parser, acm_device.acm_event))
        {
            break;
        }
        status = usb_class_cdc_init_ipipe(acm_parser);
        if ((status != USB_OK) && (status != USBERR_OPEN_PIPE_FAILED))
        {
            break;
        }

//         USB_PRINTF("----- CDC control interface selected -----\n");

        break;
    }

    case USB_DETACH_EVENT:
        {
        _cdc_serial_set_state (CDC_SERIAL_DETACHED);
        break;
    }

    default:
        //         USB_PRINTF("CDC device: unknown control event\n");
        //fflush(stdout);
        break;
    } /* EndSwitch */

    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_cdc_data_event
 * Returned Value : usb_status
 * Comments       :
 *     Called when data interface has been attached, detached, etc.
 *END*--------------------------------------------------------------------*/

usb_status usb_host_cdc_data_event
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,

    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,

    /* [IN] code number for event causing callback */
    uint32_t event_code
    )
{ /* Body */
    switch(event_code)
    {
    case USB_CONFIG_EVENT:
        if (reg_device == 0)
        {
            reg_device = dev_handle;
            data_intf_handle = intf_handle;
            _cdc_serial_set_state (CDC_SERIAL_ATTACHED);
        }
        break;
    case USB_ATTACH_EVENT:
    {

//         fflush(stdout);
//         USB_PRINTF("State = attached");
//         USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
//         USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
//         USB_PRINTF("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
//         fflush(stdout);

        break;
    }
    case USB_INTF_OPENED_EVENT:
        {
        cdc_class_call_struct_t * data_parser;

//         fflush(stdout);

        /* This data interface could be controlled by some control interface,
         * which could be already initialized (or not). We have to find 
         * that interface. Then we need to bind this interface with
         * found control interface. */
        interface_descriptor_t* if_desc;

        if (USB_OK != usb_class_cdc_get_ctrl_descriptor(dev_handle,
            intf_handle,
            &if_desc))
        {
            break;
        }
        /* binds this data interface with its control interface, if possible */
        if (USB_OK != usb_class_cdc_bind_acm_interface((cdc_class_call_struct_t *)&data_device.CLASS_INTF,
            if_desc))
        {
            break;
        }
        if (NULL == (data_parser = usb_class_cdc_get_data_interface(intf_handle)))
        {
            break;
        }
        if (USB_OK != usb_class_cdc_data_use_lwevent(data_parser, data_device.data_event))
        {
            break;
        }
        if (USB_OK == usb_class_cdc_install_driver(data_parser, device_name))
        {
            if (((usb_data_class_intf_struct_t *)(data_parser->class_intf_handle))->BOUND_CONTROL_INTERFACE != NULL)
            {
            }
//             USB_PRINTF("----- Device installed -----\n");
        }
//         USB_PRINTF("----- CDC data interface selected -----\n");
        break;
    }

    case USB_DETACH_EVENT:
        {
        break;
    }

    default:
        //         USB_PRINTF("CDC device: unknown data event\n");
//         fflush(stdout);
        break;
    } /* EndSwitch */

    return USB_OK;
} /* Endbody */
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : UART_RX_TX_IRQHandler
 * Returned Value : none
 * Comments       :
 *     Implementation of UART handler.
 *
 *END*--------------------------------------------------------------------*/
/*  */
static void UART_RX_TX_IRQHandler(void)
{
    USB_UART_DRV_IRQHandler (BOARD_DEBUG_UART_INSTANCE);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : APP_init
 * Returned Value : none
 * Comments       :
 *     All app initialization should start here.
 *
 *END*--------------------------------------------------------------------*/
void APP_init(void)
{
    usb_status status = USB_OK;
    usb_host_handle host_handle;
    usb_uart_user_config_t uartConfig;

    uartConfig.bitCountPerChar = USB_UART_8BitsPerChar;
    uartConfig.parityMode = USB_UART_ParityDisabled;
    uartConfig.stopBitCount = USB_UART_OneStopBit;
    uartConfig.clockSource = USB_UART_DEFAULT_CLOCK_SOURCE;
    uartConfig.baudRate = 115200;
    USB_UART_DRV_Init(BOARD_DEBUG_UART_INSTANCE, &s_uartState, &uartConfig);

    OSA_InstallIntHandler(USB_UART_Get_USB_iRQ_Num(BOARD_DEBUG_UART_INSTANCE), UART_RX_TX_IRQHandler);
#if defined (FSL_RTOS_FREE_RTOS)
    NVIC_SetPriority(USB_UART_Get_USB_iRQ_Num(BOARD_DEBUG_UART_INSTANCE),3);
#endif
    USB_UART_DRV_InstallRxCallback(BOARD_DEBUG_UART_INSTANCE, (usb_uart_rx_callback_t)uart_rx_cb,
    (uint8_t *)(&uart_rx_buf[0]), NULL, TRUE);
    uart2usb_buf_ptr_in = uart2usb_buf_ptr_out = uart2usb_buffer;
    usb2uart_buf_ptr_in = usb2uart_buf_ptr_out = usb2uart_buffer;
    acm_device.acm_event = OS_Event_create(0);
    data_device.data_event = OS_Event_create(0);
    s_f_usb_info.f_usb = (file_cdc_t *)OS_Mem_alloc_zero(sizeof(file_cdc_t));
    s_f_usb_info.f_usb->dev_ptr = (cdc_io_device_struct_t *)OS_Mem_alloc(sizeof(cdc_io_device_struct_t));
    f_usb = s_f_usb_info.f_usb;
    s_cdc_serial_io_mutex = OS_Mutex_create();
    if (NULL == s_cdc_serial_io_mutex)
    {
        USB_PRINTF("\nCDC serial OS_Mutex_create fail");
        _cdc_serial_os_task_suspend();
    }
    //_int_install_unexpected_isr();
    status = usb_host_init(CONTROLLER_ID, usb_host_board_init, &host_handle);

    /*
     ** It means that we are going to act like host, so we initialize the
     ** host stack. This call will allow USB system to allocate memory for
     ** data structures, it uses later (e.g pipes etc.).
     */

    if (status != USB_OK)
    {
        USB_PRINTF("\nUSB Host Initialization failed. STATUS: %x", status);
        _cdc_serial_os_task_suspend();
    }

    /*
     ** since we are going to act as the host driver, register the driver
     ** information for wanted class/subclass/protocols
     */
    status = usb_host_register_driver_info(
        host_handle,
        DriverInfoTable
        );
    if (status != USB_OK)
    {
        USB_PRINTF("\nDriver Registration failed. STATUS: %x", status);
        _cdc_serial_os_task_suspend();
    }

    /* We suppose that the standard output is interrupt driven uart device */
    USB_PRINTF("\n\rNOTE: Please ensure that stdin is interrupt driven uart device.");
    USB_PRINTF("\nInitialization passed. Plug-in CDC device to USB port first.\r\n");
    USB_PRINTF("This example requires that the CDC device uses HW flow.\r\n");
    USB_PRINTF("If your device does not support HW flow, then set \r\n");
    USB_PRINTF("CDC_EXAMPLE_USE_HW_FLOW in cdc_serial.h to zero and rebuild example project.\r\n");
    USB_PRINTF("\nTry typing some string... Press ENTER to send to CDC device, then you will\n\r");
    USB_PRINTF("see them echoed back from the device.\r\n");
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : APP_task
 * Returned Value : none
 * Comments       :
 *     App task
 *
 *END*--------------------------------------------------------------------*/
void APP_task(void)
{
    uint32_t remained;
    usb_status status = USB_OK;
    /* process the state of cdc serial */
    switch(s_f_usb_info.state)
    {
    case CDC_SERIAL_DETACHED:
        {
        uart2usb_buf_ptr_in = uart2usb_buf_ptr_out = uart2usb_buffer;
        usb2uart_buf_ptr_in = usb2uart_buf_ptr_out = usb2uart_buffer;
        s_feed = 0;
        s_usb_tx_buf_i = 0;
        usb_tx_remain_cnt = 0;
#if CDC_ASYNC
        s_cdc_xfer.tx_sts = CDC_TX_IDLE;
        s_cdc_xfer.rx_sts = CDC_RX_IDLE;
#endif
        if (s_f_usb_info.cnt)
        {
            if (IO_OK != _io_cdc_serial_close(s_f_usb_info.f_usb))
            {
                USB_PRINTF("\nInternal error occurred");
                _cdc_serial_os_task_suspend(); /* internal error occurred */
            }
            s_f_usb_info.cnt--;

            /* 
             * ACM interface detach 
             */
            cdc_class_call_struct_t * acm_parser;
            if (USB_OK != usb_host_close_dev_interface((usb_host_handle)usb_host_dev_mng_get_host(reg_device),
                reg_device, acm_intf_handle, (usb_class_handle)acm_device.CLASS_INTF.class_intf_handle))
            {
                USB_PRINTF("\nerror in _usb_hostdev_close_interface");
            }
            if (NULL == (acm_parser = usb_class_cdc_get_ctrl_interface(acm_intf_handle)))
            {
                break;
            }

            usb_class_cdc_unbind_data_interfaces(acm_parser);

            /* Use only the interface with desired protocol */
            /* Flush out data on UART TX */
            while (kStatus_USB_UART_TxBusy == USB_UART_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, &remained))
            {
                ;
            }

            USB_PRINTF("----- CDC control interface detach event -----\r\n");

            /*
             * Data interface detach
             */
            cdc_class_call_struct_t * data_parser;
            if (USB_OK != usb_host_close_dev_interface((usb_host_handle)usb_host_dev_mng_get_host(reg_device),
                reg_device, data_intf_handle, (usb_class_handle)data_device.CLASS_INTF.class_intf_handle))
            {
                USB_PRINTF("\nerror in _usb_hostdev_close_interface");
            }
            if (NULL == (data_parser = usb_class_cdc_get_data_interface(data_intf_handle)))
            {
                break;
            }

            /* unbind data interface */
            if (USB_OK != usb_class_cdc_unbind_acm_interface(data_parser))
            {
                break;
            }
            if (USB_OK != usb_class_cdc_uninstall_driver(data_parser))
            {
                break;
            }

            reg_device = 0;

            USB_PRINTF("----- CDC data interface detach Event -----\r\n");
        }
        break;
    }
    case CDC_SERIAL_ATTACHED:
        /* initialize new interface members and select this interface */
        status = usb_host_open_dev_interface((usb_host_handle)usb_host_dev_mng_get_host(reg_device),
            reg_device, acm_intf_handle, (usb_class_handle*)&acm_device.CLASS_INTF.class_intf_handle);
        if (status != USB_OK)
        {
            USB_PRINTF("\nError in _usb_hostdev_open_interface: %x\n", status);
            return;
        } /* Endif */

        USB_PRINTF("----- CDC control interface attach Event -----\r\n");

        /* initializes interface members and selects it */
        status = usb_host_open_dev_interface((usb_host_handle)usb_host_dev_mng_get_host(reg_device),
            reg_device, data_intf_handle, (usb_class_handle*)&data_device.CLASS_INTF.class_intf_handle);
        if (status != USB_OK)
        {
            USB_PRINTF("\nError in _usb_hostdev_open_interface: %x\n", status);
            return;
        } /* Endif */

        USB_PRINTF("----- CDC data interface attach event -----\r\n");

        /*
         * The following describes the scenario that the usb device is detached but still with power on.
         *
         * Once fopen() for usb is called, usb_class_cdc_set_acm_ctrl_state() will result in 'producer' of ep1
         * in USB_Class_CDC_Send_Data() being increased by 1. So the original fopen() in usb2uart_task
         * and uart2usb_task will get the 'producer' added by 2. However, there is only one interrupt pipe,
         * created in usb_class_cdc_init_ipipe(), which will cause the 'consumer' in USB_Service_Dic_Bulk_In()
         * being synced with 'producer'. So as the interrupt pipe finish receiving the serial status, the producer
         * is NOT equal to(one more greater than) consumer.    Next time on a re-attach event, the device will
         * refuse to send any data since the 'producer-consumer' pair of ep1 doesn't match any more. Thus, host
         * can't get the correct serial status, though ep2 for normal read/write is alive.
         */
        _io_cdc_serial_open(s_f_usb_info.f_usb, device_name, (const char *)&usb_open_param);
        s_f_usb_info.cnt++;
        _cdc_serial_set_state (CDC_SERIAL_FUSB_OPEND);
        break;
    case CDC_SERIAL_FUSB_OPEND:
        uart2usb_task(0);
        usb2uart_task(0);
        break;
    default:
        break;
    }

}



#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    {   1L, Main_Task, 2500L, MQX_MAIN_TASK_PRIORITY, "Main", MQX_AUTO_START_TASK},
    {   0L, 0L, 0L, 0L, 0L, 0L}
};
#endif

static void Task_Start(void *arg)
{
#if (USE_RTOS)
    APP_init();

    for (;; )
    {
#endif
    APP_task();
#if (USE_RTOS)
} /* Endfor */
#endif
}

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param)
#else
int main(void)
#endif
{
    hardware_init();
    OSA_Init();
    dbg_uart_init();

#if !(USE_RTOS)
    APP_init();
#endif

    OS_Task_create(Task_Start, NULL, 4L, 3000L, "task_start", NULL);
    OSA_Start();
#if !defined(FSL_RTOS_MQX)
    return 1;
#endif
}
