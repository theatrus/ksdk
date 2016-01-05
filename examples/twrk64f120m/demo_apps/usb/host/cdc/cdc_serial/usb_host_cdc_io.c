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
 * $FileName: usb_host_cdc_io.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file Contains the implementation of IO driver for CDC devices.
 *
 *END************************************************************************/
#include "adapter.h"
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_cdc.h"
#include "usb_host_cdc_io.h"

/* Global variables */
file_cdc_t * f_usb;

volatile static _cdc_io_xfer_sts_t s_cdc_xfer_sts = {0};

/* Global functions declaration */
extern void usb_class_cdc_in_data_callback(usb_pipe_handle tr_ptr, void * param, uint8_t *buffer, uint32_t len, uint32_t status);
extern void usb_class_cdc_out_data_callback(usb_pipe_handle tr_ptr, void *param, uint8_t *buffer, uint32_t len, uint32_t status);
extern uint32_t usb_class_cdc_os_event_wait(os_event_handle handle, uint32_t bitmask, uint32_t flag, uint32_t timeout);
static void _io_cdc_serial_write_cb(void * param);
static void _io_cdc_serial_read_cb(void * param);

/*FUNCTION****************************************************************
 *
 * Function Name    : _io_cdc_serial_open
 * Returned Value   : int_32 error code
 * Comments         :
 *    This routine initializes a USB serial I/O channel. It acquires
 *    memory, then stores information into it about the channel.
 *    It then calls the hardware interface function to initialize the channel.
 *
 *END**********************************************************************/
int32_t _io_cdc_serial_open
(
    /* [IN] the file handle for the device being opened */
    file_cdc_t * fd_ptr,

    /* [IN] the remaining portion of the name of the device */
    char * open_name_ptr,

    /* [IN] the flags to be used during operation:
     ** echo, translation, xon/xoff, encoded into a pointer.
     */
    const char * flags
)
{ /* Body */
    if (NULL == (void *)(fd_ptr->flags = (uint32_t)flags))
    {
        fd_ptr->error = USB_INVALID_PARAMETER;
        return IO_ERROR;
    }

    if (((cdc_serial_init_t *)fd_ptr->flags)->flags & USB_UART_HW_FLOW)
    {
        /* HW flow control? If yes, then from our side we don't have any problem to send
         ** data any time. Inform device that we are prepared to send.
         */
        if (USB_OK != (fd_ptr->error = usb_class_cdc_set_acm_ctrl_state(
            (cdc_class_call_struct_t *)fd_ptr->dev_ptr->driver_init_ptr,
            1, /* DTR set on */
            1 /* RTS set on */
            )))
        {
            return IO_ERROR;
        }
    }

    return IO_OK;
} /* Endbody */

/*FUNCTION****************************************************************
 *
 * Function Name    : _io_cdc_serial_close
 * Returned Value   : int_32 error code
 * Comments         :
 *    This routine closes the USB serial I/O channel.
 *
 *END**********************************************************************/

int32_t _io_cdc_serial_close
(
    /* [IN] the file handle for the device being closed */
    file_cdc_t * fd_ptr
)
{ /* Body */
    return IO_OK;
}

/*FUNCTION****************************************************************
 *
 * Function Name    : _io_cdc_serial_read
 * Returned Value   : int_32 number of characters read
 * Comments         :
 *    This routine reads characters from the USB serial I/O channel
 *    device, converting carriage return ('\r') characters to newlines,
 *    and then echoing the input characters.
 *
 *END*********************************************************************/

int32_t _io_cdc_serial_read
(
    /* [IN] the file handle for the device */
    file_cdc_t * fd_ptr,

    /* [IN] where the characters are to be stored */
    char * data_ptr,

    /* [IN] the number of characters to input */
    int32_t num
)
{ /* Body */
    cdc_class_call_struct_t * data_instance;
    usb_data_class_intf_struct_t * if_data_ptr;
    usb_status status;
    int32_t num_left, block_len;
    os_event_handle event;
    tr_struct_t* tr_ptr;
    if (0 == (num_left = num))
    {
        return 0;
    }

    data_instance = (cdc_class_call_struct_t *)fd_ptr->dev_ptr->driver_init_ptr;

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_class_cdc_intf_validate(data_instance))
    {
        fd_ptr->error = USBERR_NO_INTERFACE;
        return IO_ERROR;
    }
    if_data_ptr = (usb_data_class_intf_struct_t *)data_instance->class_intf_handle;
    USB_CDC_DATA_lock();
    /* Does the device use IN pipe? */
    if (if_data_ptr->in_pipe == NULL)
    {
        USB_CDC_DATA_unlock();
        fd_ptr->error = USBERR_OPEN_PIPE_FAILED;
        return IO_ERROR;
    }

    block_len = if_data_ptr->RX_BUFFER_SIZE;
    event = if_data_ptr->data_event;

    /* If we have USB_UART_NO_BLOCKING, we want to be sure that we will not be waiting for the pipe.
     ** We just check if the pipe is busy (event USB_DATA_READ_PIPE_FREE is not set). If it is, we should return,
     ** There is no other way to check the status of event without waiting than to access its structure.
     ** Known issue:
     ** This approach takes the risk that the 2 tasks access USB_UART_NO_BLOCKING file, then both can read "free pipe"
     ** and next on waiting for USB_DATA_READ_PIPE_FREE one of them takes the event and the other one waits blocked
     ** until the pipe is free (and waiting is not accepted with USB_UART_NO_BLOCKING).
     */
    if ((((cdc_serial_init_t *)fd_ptr->flags)->flags & USB_UART_NO_BLOCKING) && (!OS_Event_check_bit(event, USB_DATA_READ_PIPE_FREE)))
    {
        USB_CDC_DATA_unlock();
        return 0;
    }

    if (event != NULL)
    {
        usb_class_cdc_os_event_wait(event, USB_DATA_READ_PIPE_FREE | USB_DATA_DETACH, FALSE, 0);
        if (OS_Event_check_bit(event, USB_DATA_READ_PIPE_FREE))
        {
            OS_Event_clear(event, USB_DATA_READ_PIPE_FREE);
        }
        if (OS_Event_check_bit(event, USB_DATA_DETACH))
        {
            OS_Event_clear(event, USB_DATA_DETACH);
        }
    }

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_class_cdc_intf_validate(data_instance))
    {
        USB_CDC_DATA_unlock();
        fd_ptr->error = USBERR_NO_INTERFACE;
        return IO_ERROR;
    }

    while (if_data_ptr->RX_BUFFER_APP < if_data_ptr->RX_BUFFER_DRV)
    {
        *data_ptr++ = *if_data_ptr->RX_BUFFER_APP++;
        num_left--;
        if (!num_left)
        {
            break;
        }
    }

    while (num_left)
    {
        if (usb_host_get_tr(if_data_ptr->host_handle, usb_class_cdc_in_data_callback, (void *)fd_ptr, &tr_ptr) != USB_OK)
        {
            USB_PRINTF("_io_cdc_serial_read: error to get tr\n");
            return USBERR_ERROR;
        }

        if (num_left > block_len)
        {
            tr_ptr->rx_buffer = (uint8_t *)data_ptr;
            tr_ptr->rx_length = block_len * (num_left / block_len);
            if_data_ptr->RX_BUFFER_DRV = NULL;
        }
        else
        {
            /* last packet must be always written to the local buffer, because device can send more bytes than we need to read */
            if_data_ptr->RX_BUFFER_APP = if_data_ptr->rx_buffer;
            if_data_ptr->RX_BUFFER_DRV = if_data_ptr->rx_buffer;
            tr_ptr->rx_buffer = (uint8_t *)if_data_ptr->rx_buffer;
            tr_ptr->rx_length = block_len;
        }

        if (event != NULL)
        {
            OS_Event_clear(event, USB_DATA_READ_COMPLETE);
        }
        status = usb_host_recv_data(if_data_ptr->host_handle, if_data_ptr->in_pipe, tr_ptr);
        if (status == USB_OK)
        {

            if (event != NULL)
            {
                /* serialize requests by waiting for pipe */
                /* wait while data will be available */
                usb_class_cdc_os_event_wait(event, USB_DATA_READ_COMPLETE | USB_DATA_DETACH, FALSE, 0);
                if (OS_Event_check_bit(event, USB_DATA_READ_COMPLETE))
                {
                    OS_Event_clear(event, USB_DATA_READ_COMPLETE);
                }
                if (OS_Event_check_bit(event, USB_DATA_DETACH))
                {
                    OS_Event_clear(event, USB_DATA_DETACH);
                }
            }

            /* Validity checking, always needed when passing data to lower API */
            if (!usb_class_cdc_intf_validate(data_instance))
            {
                USB_CDC_DATA_unlock();
                fd_ptr->error = USBERR_NO_INTERFACE;
                return IO_ERROR;
            }

            if (tr_ptr->rx_buffer != (uint8_t *)data_ptr)
            {
                while ((if_data_ptr->RX_BUFFER_APP < if_data_ptr->RX_BUFFER_DRV) && num_left)
                {
                    *data_ptr++ = *if_data_ptr->RX_BUFFER_APP++;
                    num_left--;
                }
            }
            else
            {
                data_ptr += if_data_ptr->RX_READ;
                num_left -= if_data_ptr->RX_READ;
            }
            if (usb_host_release_tr(if_data_ptr->host_handle, tr_ptr) != USB_OK)
            {
                USB_PRINTF("_io_cdc_serial_read: _usb_host_release_tr failed\n");
            }
            if (num_left && (!(((cdc_serial_init_t *)fd_ptr->flags)->flags & USB_UART_NO_BLOCKING)))
            {
                if (event != NULL)
                {
                    /* wait 10 ticks to next read */
                    OS_Event_wait(event, USB_DATA_DETACH, FALSE, 10);
                    if (OS_Event_check_bit(event, USB_DATA_DETACH))
                    {
                        OS_Event_clear(event, USB_DATA_DETACH);
                    }
                }
                /* Validity checking, always needed when passing data to lower API */
                if (!usb_class_cdc_intf_validate(data_instance))
                {
                    USB_CDC_DATA_unlock();
                    fd_ptr->error = USBERR_NO_INTERFACE;
                    return IO_ERROR;
                }
            }
            else
            {
                status = USBERR_BAD_STATUS;
            }
        }
        else
        {
            USB_PRINTF("\nError in _io_cdc_serial_read: 0x%x", status);
            usb_host_release_tr(if_data_ptr->host_handle, tr_ptr);
            status = USBERR_BAD_STATUS;
        }
        if (status != USB_OK)
        {
            break;
        }
    }

//read_done:
    if (event != NULL)
    {
        OS_Event_set(event, USB_DATA_READ_PIPE_FREE); /* mark we are not using input pipe */
    }

    USB_CDC_DATA_unlock();

    return num - num_left;

} /* Endbody */

/*FUNCTION****************************************************************
 *
 * Function Name    : _io_cdc_serial_read_async
 * Returned Value   : error or status of the transfer
 * Comments         :
 *    
 *END*********************************************************************/

int32_t _io_cdc_serial_read_async
(
    /* [IN] the file handle for the device */
    file_cdc_t * fd_ptr,

    /* [IN] where the characters are to be stored */
    char * data_ptr,

    /* [IN] the number of characters to input */
    int32_t num,

    /* [IN] the callback when read complete */
    _usb_cdc_callback cb_ptr
)
{ /* Body */
    cdc_class_call_struct_t * data_instance;
    usb_data_class_intf_struct_t * if_data_ptr;
    usb_status status;
    int32_t num_left, block_len;
    os_event_handle event;
    tr_struct_t * tr_ptr;
    int32_t ret = IO_OK;
    if (0 == (num_left = num))
    {
        return 0;
    }

    data_instance = (cdc_class_call_struct_t *)fd_ptr->dev_ptr->driver_init_ptr;

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_class_cdc_intf_validate(data_instance))
    {
        fd_ptr->error = USBERR_NO_INTERFACE;
        return IO_ERROR;
    }
    if_data_ptr = (usb_data_class_intf_struct_t *)data_instance->class_intf_handle;
    USB_CDC_DATA_lock();
    if (TRUE == if_data_ptr->is_rx_xferring)
    {
        USB_CDC_DATA_unlock();
        return IO_ERROR;
    }
    if_data_ptr->is_rx_xferring = TRUE;
    USB_CDC_DATA_unlock();
    /* Does the device use IN pipe? */
    if (if_data_ptr->in_pipe == NULL)
    {
        if_data_ptr->is_rx_xferring = FALSE;
        fd_ptr->error = USBERR_OPEN_PIPE_FAILED;
        return IO_ERROR;
    }

    block_len = if_data_ptr->RX_BUFFER_SIZE;
    event = if_data_ptr->data_event;

    /* If we have USB_UART_NO_BLOCKING, we want to be sure that we will not be waiting for the pipe.
     ** We just check if the pipe is busy (event USB_DATA_READ_PIPE_FREE is not set). If it is, we should return,
     ** There is no other way to check the status of event without waiting than to access its structure.
     ** Known issue:
     ** This approach takes the risk that the 2 tasks access USB_UART_NO_BLOCKING file, then both can read "free pipe"
     ** and next on waiting for USB_DATA_READ_PIPE_FREE one of them takes the event and the other one waits blocked
     ** until the pipe is free (and waiting is not accepted with USB_UART_NO_BLOCKING).
     */
    if ((((cdc_serial_init_t *)fd_ptr->flags)->flags & USB_UART_NO_BLOCKING) && (!OS_Event_check_bit(event, USB_DATA_READ_PIPE_FREE)))
    {
        if_data_ptr->is_rx_xferring = FALSE;
        return 0;
    }

    if (event != NULL)
    {
        usb_class_cdc_os_event_wait(event, USB_DATA_READ_PIPE_FREE | USB_DATA_DETACH, FALSE, 0);
        if (OS_Event_check_bit(event, USB_DATA_READ_PIPE_FREE))
        {
            OS_Event_clear(event, USB_DATA_READ_PIPE_FREE);
        }
        if (OS_Event_check_bit(event, USB_DATA_DETACH))
        {
            OS_Event_clear(event, USB_DATA_DETACH);
        }
    }

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_class_cdc_intf_validate(data_instance))
    {
        if_data_ptr->is_rx_xferring = FALSE;
        fd_ptr->error = USBERR_NO_INTERFACE;
        return IO_ERROR;
    }

    if (usb_host_get_tr(if_data_ptr->host_handle, usb_class_cdc_in_data_callback, (void *)fd_ptr, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("_io_cdc_serial_read: error to get tr\n");
        return USBERR_ERROR;
    }

    if (num_left > block_len)
    {
        tr_ptr->rx_buffer = (uint8_t *)data_ptr;
        tr_ptr->rx_length = block_len * (num_left / block_len);
        if_data_ptr->RX_BUFFER_DRV = NULL;
    }
    else
    {
        /* last packet must be always written to the local buffer, because device can send more bytes than we need to read */
        if_data_ptr->RX_BUFFER_APP = if_data_ptr->rx_buffer;
        if_data_ptr->RX_BUFFER_DRV = if_data_ptr->rx_buffer;
        tr_ptr->rx_buffer = (uint8_t *)if_data_ptr->rx_buffer;
        tr_ptr->rx_length = block_len;
    }
    s_cdc_xfer_sts.data_ptr_rx = data_ptr;
    s_cdc_xfer_sts.num_left_rx = num_left;
    s_cdc_xfer_sts.num_rx = num;
    s_cdc_xfer_sts.tr_ptr_rx = tr_ptr;
    s_cdc_xfer_sts.rx_cb = cb_ptr;
    if_data_ptr->data_rx_cb = _io_cdc_serial_read_cb;

    if (event != NULL)
    {
        OS_Event_clear(event, USB_DATA_READ_COMPLETE);
    }
    status = usb_host_recv_data(if_data_ptr->host_handle, if_data_ptr->in_pipe, tr_ptr);

    if (status == USB_OK)
    {
        ret = IO_OK;
    }
    else
    {
        //USB_PRINTF("\nError in _io_cdc_serial_read_async: 0x%x", status);
        if (usb_host_release_tr(if_data_ptr->host_handle, tr_ptr) != USB_OK)
        {
            USB_PRINTF("_io_cdc_serial_read_async: _usb_host_release_tr failed\n");
        }
        if (event != NULL)
        {
            OS_Event_set(event, USB_DATA_READ_PIPE_FREE); /* mark we are not using input pipe */
        }

        if_data_ptr->is_rx_xferring = FALSE;
        ret = IO_ERROR;
    }

    return ret;
}

/*FUNCTION****************************************************************
 *
 * Function Name    : _io_cdc_serial_read_cb
 * Returned Value   : None
 * Comments         :
 *    This routine read characters to the USB serial I/O channel in async mode.
 *
 *END**********************************************************************/
static void _io_cdc_serial_read_cb
(
    /* [IN] number of characters been sent  */
    void * param
)
{
    file_cdc_t * fd_ptr = f_usb;
    cdc_class_call_struct_t * data_instance;
    usb_data_class_intf_struct_t * if_data_ptr;
    os_event_handle event;
    tr_struct_t * tr_ptr;
    int32_t num_left;
    char * data_ptr;
    int32_t ret = IO_OK;
    uint32_t num_rx_temp, num_left_rx_temp;

    data_instance = (cdc_class_call_struct_t *)fd_ptr->dev_ptr->driver_init_ptr;

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_class_cdc_intf_validate(data_instance))
    {
        fd_ptr->error = USBERR_NO_INTERFACE;
        ret = IO_ERROR;
    }
    if_data_ptr = (usb_data_class_intf_struct_t *)data_instance->class_intf_handle;
    event = if_data_ptr->data_event;

    if (OS_Event_check_bit(event, USB_DATA_READ_COMPLETE))
    {
        OS_Event_clear(event, USB_DATA_READ_COMPLETE);
    }
    if (OS_Event_check_bit(event, USB_DATA_DETACH))
    {
        OS_Event_clear(event, USB_DATA_DETACH);
    }

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_class_cdc_intf_validate(data_instance))
    {
        if_data_ptr->is_rx_xferring = FALSE;
        fd_ptr->error = USBERR_NO_INTERFACE;
        ret = IO_ERROR;
    }
    tr_ptr = s_cdc_xfer_sts.tr_ptr_rx;
    data_ptr = s_cdc_xfer_sts.data_ptr_rx;
    num_left = s_cdc_xfer_sts.num_left_rx;
    if (tr_ptr->rx_buffer != (uint8_t *)data_ptr)
    {
        while ((if_data_ptr->RX_BUFFER_APP < if_data_ptr->RX_BUFFER_DRV) && num_left)
        {
            *data_ptr++ = *if_data_ptr->RX_BUFFER_APP++;
            num_left--;
        }
    }
    else
    {
        data_ptr += if_data_ptr->RX_READ;
        num_left -= if_data_ptr->RX_READ;
    }
    s_cdc_xfer_sts.num_left_rx = num_left;

    if (usb_host_release_tr(if_data_ptr->host_handle, tr_ptr) != USB_OK)
    {
        ret = IO_ERROR;
    }

    if (event != NULL)
    {
        OS_Event_set(event, USB_DATA_READ_PIPE_FREE); /* mark we are not using input pipe */
    }

    num_rx_temp = s_cdc_xfer_sts.num_rx;
    num_left_rx_temp = s_cdc_xfer_sts.num_left_rx;
    if_data_ptr->is_rx_xferring = FALSE;
    if (num_left && (!(((cdc_serial_init_t *)fd_ptr->flags)->flags & USB_UART_NO_BLOCKING)))
    {
        _io_cdc_serial_read_async(fd_ptr, data_ptr, num_left, s_cdc_xfer_sts.rx_cb);
    }
    else
    {
        if (ret == IO_OK)
        {
            ret = num_rx_temp - num_left_rx_temp;
        }
        if (s_cdc_xfer_sts.rx_cb)
        {
            s_cdc_xfer_sts.rx_cb((void*)&ret);
        }
    }
}

/*FUNCTION****************************************************************
 *
 * Function Name    : _io_cdc_serial_char_avail
 * Returned Value   : TRUE if char is available, FALSE if not
 * Comments         :
 *    This routine reads characters from the USB serial I/O channel
 *    device, converting carriage return ('\r') characters to newlines,
 *    and then echoing the input characters.
 *
 *END*********************************************************************/

int32_t _io_cdc_serial_char_avail
(
    /* [IN] the file handle for the device */
    file_cdc_t * fd_ptr
)
{ /* Body */
    cdc_class_call_struct_t * data_instance;
    usb_data_class_intf_struct_t * if_data_ptr;
    usb_status status;
    os_event_handle event;
    tr_struct_t* tr_ptr;
    data_instance = (cdc_class_call_struct_t *)fd_ptr->dev_ptr->driver_init_ptr;

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_class_cdc_intf_validate(data_instance))
    {
        return USBERR_NO_INTERFACE;
    }
    if_data_ptr = (usb_data_class_intf_struct_t *)data_instance->class_intf_handle;
    USB_CDC_DATA_lock();

    if (if_data_ptr->in_pipe == NULL)
    {
        USB_CDC_DATA_unlock();
        fd_ptr->error = USBERR_OPEN_PIPE_FAILED;
        return IO_ERROR;
    }

    event = if_data_ptr->data_event;

    if ((event != NULL) && (!OS_Event_check_bit(event, USB_DATA_READ_PIPE_FREE)))
    {
        /* any other task is reading => no character available for us now */
        USB_CDC_DATA_unlock();
        return FALSE;
    }
    if (if_data_ptr->RX_BUFFER_APP != if_data_ptr->RX_BUFFER_DRV)
    {
        /* cache is not empty => char is available */
        USB_CDC_DATA_unlock();
        return TRUE;
    }

    if (event != NULL)
    {
        /* following command never waits as USB_DATA_READ_PIPE_FREE is set (tested above) */
        usb_class_cdc_os_event_wait(event, USB_DATA_READ_PIPE_FREE, FALSE, 0);
        if (OS_Event_check_bit(event, USB_DATA_READ_PIPE_FREE))
        {
            OS_Event_clear(event, USB_DATA_READ_PIPE_FREE);
        }
    }

    /* We are going to try reading data. We will see if some data is available or not. */

    if (usb_host_get_tr(if_data_ptr->host_handle, usb_class_cdc_in_data_callback, (void *)fd_ptr, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("_io_cdc_serial_char_avail: error to get tr\n");
        return USBERR_ERROR;
    }
    if_data_ptr->RX_BUFFER_APP = if_data_ptr->rx_buffer;
    if_data_ptr->RX_BUFFER_DRV = if_data_ptr->rx_buffer;
    tr_ptr->rx_buffer = (uint8_t *)if_data_ptr->rx_buffer;
    tr_ptr->rx_length = if_data_ptr->RX_BUFFER_SIZE;

    if (event != NULL)
    {
        OS_Event_clear(event, USB_DATA_READ_COMPLETE);
    }
    status = usb_host_recv_data(if_data_ptr->host_handle, if_data_ptr->in_pipe, tr_ptr);

    if (status == USB_OK)
    {
        if (event != NULL)
        {
            /* serialize requests by waiting for pipe */
            /* wait while data will be available */
            usb_class_cdc_os_event_wait(event, USB_DATA_READ_COMPLETE | USB_DATA_DETACH, FALSE, 0);
            if (OS_Event_check_bit(event, USB_DATA_READ_COMPLETE))
            {
                OS_Event_clear(event, USB_DATA_READ_COMPLETE);
            }
            if (OS_Event_check_bit(event, USB_DATA_DETACH))
            {
                OS_Event_clear(event, USB_DATA_DETACH);
            }
        }
        /* Validity checking, always needed when passing data to lower API */
        if (!usb_class_cdc_intf_validate(data_instance))
        {
            USB_CDC_DATA_unlock();
            return FALSE;
        }

        status = (if_data_ptr->RX_READ != 0 ? TRUE : FALSE);
    }
    else
    {
        status = FALSE;
    }

    if (event != NULL)
    {
        OS_Event_set(event, USB_DATA_READ_PIPE_FREE); /* mark we are not using input pipe */
    }

    USB_CDC_DATA_unlock();

    return status;

} /* Endbody */

/*FUNCTION****************************************************************
 *
 * Function Name    : _io_cdc_serial_write
 * Returned Value   : int_32 number of characters written
 * Comments         :
 *    This routine writes characters to the USB serial I/O channel.
 *
 *END**********************************************************************/

int32_t _io_cdc_serial_write
(
    /* [IN] the file handle for the device */
    file_cdc_t * fd_ptr,

    /* [IN] where the characters are */
    char * data_ptr,

    /* [IN] the number of characters to output */
    int32_t num
)
{ /* Body */
    cdc_class_call_struct_t * data_instance;
    usb_data_class_intf_struct_t * if_data_ptr;
    usb_status status = USBERR_NO_INTERFACE;
    uint32_t num_left;
    os_event_handle event;
    tr_struct_t* tr_ptr;
    if (0 == (num_left = num))
    {
        return 0;
    }

    data_instance = (cdc_class_call_struct_t *)fd_ptr->dev_ptr->driver_init_ptr;

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_class_cdc_intf_validate(data_instance))
    {
        fd_ptr->error = USBERR_NO_INTERFACE;
        return IO_ERROR;
    }
    if_data_ptr = (usb_data_class_intf_struct_t *)data_instance->class_intf_handle;
    USB_CDC_DATA_lock();
    /* Do the device use OUT pipe? */
    if (if_data_ptr->out_pipe == NULL)
    {
        USB_CDC_DATA_unlock();
        return USBERR_OPEN_PIPE_FAILED;
    }

    event = if_data_ptr->data_event;

    if ((((cdc_serial_init_t *)fd_ptr->flags)->flags & USB_UART_NO_BLOCKING) && (!OS_Event_check_bit(event, USB_DATA_SEND_PIPE_FREE)))
    {
        USB_CDC_DATA_unlock();
        return 0;
    }

    usb_class_cdc_os_event_wait(event, USB_DATA_SEND_PIPE_FREE | USB_DATA_DETACH, FALSE, 0);
    if (OS_Event_check_bit(event, USB_DATA_SEND_PIPE_FREE))
    {
        OS_Event_clear(event, USB_DATA_SEND_PIPE_FREE);
    }
    if (OS_Event_check_bit(event, USB_DATA_DETACH))
    {
        OS_Event_clear(event, USB_DATA_DETACH);
    }

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_class_cdc_intf_validate(data_instance))
    {
        USB_CDC_DATA_unlock();
        fd_ptr->error = USBERR_NO_INTERFACE;
        return IO_ERROR;
    }

    while (num_left)
    {
        if (usb_host_get_tr(if_data_ptr->host_handle, usb_class_cdc_out_data_callback, (void *)fd_ptr, &tr_ptr) != USB_OK)
        {
            USB_PRINTF("_io_cdc_serial_write: error to get tr\n");
            return USBERR_ERROR;
        }
        tr_ptr->tx_buffer = (uint8_t *)data_ptr;
        tr_ptr->tx_length = num_left;

        OS_Event_clear(event, USB_DATA_SEND_COMPLETE);
        status = usb_host_send_data(if_data_ptr->host_handle, if_data_ptr->out_pipe, tr_ptr);
        if (status == USB_OK)
        {

            /* serialize requests by waiting for pipe */
            /* wait while data will be sent */
            usb_class_cdc_os_event_wait(event, USB_DATA_SEND_COMPLETE | USB_DATA_DETACH, FALSE, 0);
            if (OS_Event_check_bit(event, USB_DATA_SEND_COMPLETE))
            {
                OS_Event_clear(event, USB_DATA_SEND_COMPLETE);
            }
            if (OS_Event_check_bit(event, USB_DATA_DETACH))
            {
                OS_Event_clear(event, USB_DATA_DETACH);
            }

            /* Validity checking, always needed when passing data to lower API */
            if (!usb_class_cdc_intf_validate(data_instance))
            {
                USB_CDC_DATA_unlock();
                fd_ptr->error = USBERR_NO_INTERFACE;
                return IO_ERROR;
            }

            data_ptr += if_data_ptr->TX_SENT;
            num_left -= if_data_ptr->TX_SENT;

            if (usb_host_release_tr(if_data_ptr->host_handle, tr_ptr) != USB_OK)
            {
                USB_PRINTF("_io_cdc_serial_write: _usb_host_release_tr failed\n");
            }

            if ((num_left) && (!(((cdc_serial_init_t *)fd_ptr->flags)->flags & USB_UART_NO_BLOCKING)))
            {
                /* wait 10 ticks to next write */
                OS_Event_wait(event, USB_DATA_DETACH, FALSE, 10);
                if (OS_Event_check_bit(event, USB_DATA_DETACH))
                {
                    OS_Event_clear(event, USB_DATA_DETACH);
                }
                /* Validity checking, always needed when passing data to lower API */
                if (!usb_class_cdc_intf_validate(data_instance))
                {
                    USB_CDC_DATA_unlock();
                    fd_ptr->error = USBERR_NO_INTERFACE;
                    return IO_ERROR;
                }
            }
            else
            {
                status = USBERR_BAD_STATUS;
            }
        }
        else
        {
            USB_PRINTF("\nError in _io_cdc_serial_write: 0x%x", status);
            usb_host_release_tr(if_data_ptr->host_handle, tr_ptr);
            status = USBERR_BAD_STATUS;
        }
        if (status != USB_OK)
        {
            break;
        }
    }

    OS_Event_set(event, USB_DATA_SEND_PIPE_FREE); /* mark we are not using input pipe */

    USB_CDC_DATA_unlock();

    return num - num_left;
} /* Endbody */

/*FUNCTION****************************************************************
 *
 * Function Name    : _io_cdc_serial_write_async
 * Returned Value   : error or status of the transfer
 * Comments         :
 *    This routine writes characters to the USB serial I/O channel in async mode.
 *
 *END**********************************************************************/

int32_t _io_cdc_serial_write_async
(
    /* [IN] the file handle for the device */
    file_cdc_t * fd_ptr,

    /* [IN] where the characters are */
    char * data_ptr,

    /* [IN] the number of characters to output */
    int32_t num,

    /* [IN] the callback when write complete */
    _usb_cdc_callback cb_ptr
)
{ /* Body */
    cdc_class_call_struct_t * data_instance;
    usb_data_class_intf_struct_t * if_data_ptr;
    usb_status status = USBERR_NO_INTERFACE;
    uint32_t num_left;
    os_event_handle event;
    tr_struct_t * tr_ptr;
    int32_t ret = IO_OK;
    if (0 == (num_left = num))
    {
        return 0;
    }
    //USB_PRINTF("\n_io_cdc_serial_write_async: num: 0x%x", num);
    data_instance = (cdc_class_call_struct_t *)fd_ptr->dev_ptr->driver_init_ptr;

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_class_cdc_intf_validate(data_instance))
    {
        fd_ptr->error = USBERR_NO_INTERFACE;
        return IO_ERROR;
    }
    if_data_ptr = (usb_data_class_intf_struct_t *)data_instance->class_intf_handle;
    USB_CDC_DATA_lock();
    if (TRUE == if_data_ptr->is_tx_xferring)
    {
        USB_CDC_DATA_unlock();
        return IO_ERROR;
    }
    if_data_ptr->is_tx_xferring = TRUE;
    USB_CDC_DATA_unlock();
    /* Do the device use OUT pipe? */
    if (if_data_ptr->out_pipe == NULL)
    {
        if_data_ptr->is_tx_xferring = FALSE;
        return USBERR_OPEN_PIPE_FAILED;
    }

    event = if_data_ptr->data_event;

    if ((((cdc_serial_init_t *)fd_ptr->flags)->flags & USB_UART_NO_BLOCKING) && (!OS_Event_check_bit(event, USB_DATA_SEND_PIPE_FREE)))
    {
        if_data_ptr->is_tx_xferring = FALSE;
        return 0;
    }

    usb_class_cdc_os_event_wait(event, USB_DATA_SEND_PIPE_FREE | USB_DATA_DETACH, FALSE, 0);
    if (OS_Event_check_bit(event, USB_DATA_SEND_PIPE_FREE))
    {
        OS_Event_clear(event, USB_DATA_SEND_PIPE_FREE);
    }
    if (OS_Event_check_bit(event, USB_DATA_DETACH))
    {
        OS_Event_clear(event, USB_DATA_DETACH);
    }

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_class_cdc_intf_validate(data_instance))
    {
        if_data_ptr->is_tx_xferring = FALSE;
        fd_ptr->error = USBERR_NO_INTERFACE;
        return IO_ERROR;
    }

    if (usb_host_get_tr(if_data_ptr->host_handle, usb_class_cdc_out_data_callback, (void *)fd_ptr, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("_io_cdc_serial_write: error to get tr\n");
        return USBERR_ERROR;
    }
    tr_ptr->tx_buffer = (uint8_t *)data_ptr;
    tr_ptr->tx_length = num_left;
    s_cdc_xfer_sts.tr_ptr_tx = tr_ptr;
    s_cdc_xfer_sts.num_left_tx = tr_ptr->tx_length;
    s_cdc_xfer_sts.num_tx = num;
    s_cdc_xfer_sts.data_ptr_tx = (char *)tr_ptr->tx_buffer;
    s_cdc_xfer_sts.tx_cb = cb_ptr;
    if_data_ptr->data_tx_cb = _io_cdc_serial_write_cb;

    OS_Event_clear(event, USB_DATA_SEND_COMPLETE);
    status = usb_host_send_data(if_data_ptr->host_handle, if_data_ptr->out_pipe, tr_ptr);

    if (status == USB_OK)
    {
        ret = IO_OK;
    }
    else
    {
        if (usb_host_release_tr(if_data_ptr->host_handle, tr_ptr) != USB_OK)
        {
            USB_PRINTF("_io_cdc_serial_write_async: _usb_host_release_tr failed\n");
        }
        OS_Event_set(event, USB_DATA_SEND_PIPE_FREE); /* mark we are not using input pipe */

        if_data_ptr->is_tx_xferring = FALSE;
        ret = IO_ERROR;
    }

    return ret;
}

/*FUNCTION****************************************************************
 *
 * Function Name    : _io_cdc_serial_write_cb
 * Returned Value   : None
 * Comments         :
 *    This routine writes characters to the USB serial I/O channel in async mode.
 *
 *END**********************************************************************/
static void _io_cdc_serial_write_cb
(
    /* [IN] number of characters been sent  */
    void * param
)
{
    file_cdc_t * fd_ptr = f_usb;
    cdc_class_call_struct_t * data_instance;
    usb_data_class_intf_struct_t * if_data_ptr;
    os_event_handle event;
    int32_t ret = IO_OK;
    char *data_ptr_tx_temp;
    uint32_t num_tx_temp, num_left_tx_temp;
    data_instance = (cdc_class_call_struct_t *)fd_ptr->dev_ptr->driver_init_ptr;

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_class_cdc_intf_validate(data_instance))
    {
        fd_ptr->error = USBERR_NO_INTERFACE;
        ret = IO_ERROR;
    }
    if_data_ptr = (usb_data_class_intf_struct_t *)data_instance->class_intf_handle;
    event = if_data_ptr->data_event;
    if (OS_Event_check_bit(event, USB_DATA_SEND_COMPLETE))
    {
        OS_Event_clear(event, USB_DATA_SEND_COMPLETE);
    }
    if (OS_Event_check_bit(event, USB_DATA_DETACH))
    {
        OS_Event_clear(event, USB_DATA_DETACH);
    }

    s_cdc_xfer_sts.data_ptr_tx += if_data_ptr->TX_SENT;
    s_cdc_xfer_sts.num_left_tx -= if_data_ptr->TX_SENT;

    if (usb_host_release_tr(if_data_ptr->host_handle, s_cdc_xfer_sts.tr_ptr_tx) != USB_OK)
    {
        ret = IO_ERROR;
    }
    OS_Event_set(event, USB_DATA_SEND_PIPE_FREE); /* mark we are not using input pipe */
    if_data_ptr->is_tx_xferring = FALSE;
    data_ptr_tx_temp = s_cdc_xfer_sts.data_ptr_tx;
    num_left_tx_temp = s_cdc_xfer_sts.num_left_tx;
    num_tx_temp = s_cdc_xfer_sts.num_tx;
    if ((s_cdc_xfer_sts.num_left_tx) && (!(((cdc_serial_init_t *)fd_ptr->flags)->flags & USB_UART_NO_BLOCKING)))
    {
        _io_cdc_serial_write_async(fd_ptr, data_ptr_tx_temp, num_left_tx_temp, s_cdc_xfer_sts.tx_cb);
    }
    else
    {
        if (ret == IO_OK)
        {
            ret = num_tx_temp - num_left_tx_temp;
        }
        if (s_cdc_xfer_sts.tx_cb)
        {
            s_cdc_xfer_sts.tx_cb((void*)&ret);
        }

    }

}

/*FUNCTION*****************************************************************
 *
 * Function Name    : _io_cdc_serial_ioctl
 * Returned Value   : int_32
 * Comments         :
 *    Returns result of ioctl operation.
 *
 *END*********************************************************************/

int32_t _io_cdc_serial_ioctl
(
    /* [IN] the file handle for the device */
    file_cdc_t * fd_ptr,

    /* [IN] the ioctl command */
    uint32_t cmd,

    /* [IN] the ioctl parameters */
    void * param_ptr
)
{ /* Body */
    int32_t * bparam_ptr;
    uint32_t * pparam_ptr;
    usb_cdc_uart_coding_t uart_coding;

    switch(cmd)
    {
    case CDC_IOCTL_CHAR_AVAIL:
        bparam_ptr = (int32_t *)param_ptr;
        fd_ptr->error = _io_cdc_serial_char_avail(fd_ptr);
        if ((fd_ptr->error != (int32_t)TRUE) && (fd_ptr->error != (int32_t)FALSE))
        {
            return IO_ERROR;
        }
        *bparam_ptr = fd_ptr->error;
        break;

    case CDC_IOCTL_SERIAL_GET_FLAGS:
        pparam_ptr = (uint32_t *)param_ptr;
        *pparam_ptr = fd_ptr->flags;
        break;

    case CDC_IOCTL_SERIAL_SET_FLAGS:
        pparam_ptr = (uint32_t *)param_ptr;
        fd_ptr->flags = *pparam_ptr;
        break;

    case CDC_IOCTL_SERIAL_GET_STOP_BITS:
        if (USB_OK != (fd_ptr->error = usb_class_cdc_get_acm_line_coding((cdc_class_call_struct_t *)fd_ptr->dev_data_ptr, &uart_coding)))
        {
            return IO_ERROR;
        }
        pparam_ptr = (uint32_t *)param_ptr;
        switch(uart_coding.stopbits)
        {
        case 1:
            *pparam_ptr = CDC_IO_SERIAL_STOP_BITS_1;
            break;
        case 2:
            *pparam_ptr = CDC_IO_SERIAL_STOP_BITS_2;
            break;
        case 3:
            *pparam_ptr = CDC_IO_SERIAL_STOP_BITS_1_5;
            break;
        default:
            fd_ptr->error = USB_INVALID_PARAMETER;
            return IO_ERROR;
        }
        break;

    case CDC_IOCTL_SERIAL_SET_STOP_BITS:
        /* Tell user to change terminal settings */
        pparam_ptr = (uint32_t *)param_ptr;
        switch(*pparam_ptr)
        {
        case CDC_IO_SERIAL_STOP_BITS_1:
            if (USB_OK != (fd_ptr->error = usb_class_cdc_get_acm_line_coding((cdc_class_call_struct_t *)fd_ptr->dev_data_ptr, &uart_coding)))
            {
                return IO_ERROR;
            }
            uart_coding.stopbits = 1;
            if (USB_OK != (fd_ptr->error = usb_class_cdc_get_acm_line_coding((cdc_class_call_struct_t *)fd_ptr->dev_data_ptr, &uart_coding)))
            {
                return IO_ERROR;
            }
            break;

        case CDC_IO_SERIAL_STOP_BITS_1_5:
            if (USB_OK != (fd_ptr->error = usb_class_cdc_get_acm_line_coding((cdc_class_call_struct_t *)fd_ptr->dev_data_ptr, &uart_coding)))
            {
                return IO_ERROR;
            }
            uart_coding.stopbits = 3;
            if (USB_OK != (fd_ptr->error = usb_class_cdc_get_acm_line_coding((cdc_class_call_struct_t *)fd_ptr->dev_data_ptr, &uart_coding)))
            {
                return IO_ERROR;
            }
            break;

        case CDC_IO_SERIAL_STOP_BITS_2:
            if (USB_OK != (fd_ptr->error = usb_class_cdc_get_acm_line_coding((cdc_class_call_struct_t *)fd_ptr->dev_data_ptr, &uart_coding)))
            {
                return IO_ERROR;
            }
            uart_coding.stopbits = 2;
            if (USB_OK != (fd_ptr->error = usb_class_cdc_get_acm_line_coding((cdc_class_call_struct_t *)fd_ptr->dev_data_ptr, &uart_coding)))
            {
                return IO_ERROR;
            }
            break;

        default:
            {
            break;
        }

        } /* Endswitch */
        return USB_INVALID_PARAMETER;
        break;

    case CDC_IOCTL_SERIAL_GET_DATA_BITS:
        if (USB_OK != (fd_ptr->error = usb_class_cdc_get_acm_line_coding((cdc_class_call_struct_t *)fd_ptr->dev_data_ptr, &uart_coding)))
        {
            return IO_ERROR;
        }
        pparam_ptr = (uint32_t *)param_ptr;
        *pparam_ptr = uart_coding.databits;
        break;

    case CDC_IOCTL_SERIAL_SET_DATA_BITS:
        if (USB_OK != (fd_ptr->error = usb_class_cdc_get_acm_line_coding((cdc_class_call_struct_t *)fd_ptr->dev_data_ptr, &uart_coding)))
        {
            return IO_ERROR;
        }
        pparam_ptr = (uint32_t *)param_ptr;
        if (*pparam_ptr > 16)
        {
            return IO_ERROR;
        }
        uart_coding.databits = *pparam_ptr;
        if (USB_OK != (fd_ptr->error = usb_class_cdc_set_acm_line_coding((cdc_class_call_struct_t *)fd_ptr->dev_data_ptr, &uart_coding)))
        {
            return IO_ERROR;
        }
        break;

    case CDC_IOCTL_SERIAL_GET_BAUD:
        if (USB_OK != (fd_ptr->error = usb_class_cdc_get_acm_line_coding((cdc_class_call_struct_t *)fd_ptr->dev_data_ptr, &uart_coding)))
        {
            return IO_ERROR;
        }
        pparam_ptr = (uint32_t *)param_ptr;
        *pparam_ptr = uart_coding.baudrate;
        break;

    case CDC_IOCTL_SERIAL_SET_BAUD:
        if (USB_OK != (fd_ptr->error = usb_class_cdc_get_acm_line_coding((cdc_class_call_struct_t *)fd_ptr->dev_data_ptr, &uart_coding)))
        {
            return IO_ERROR;
        }
        pparam_ptr = (uint32_t *)param_ptr;
        uart_coding.baudrate = *pparam_ptr;
        if (USB_OK != (fd_ptr->error = usb_class_cdc_set_acm_line_coding((cdc_class_call_struct_t *)fd_ptr->dev_data_ptr, &uart_coding)))
        {
            return IO_ERROR;
        }
        break;

    default:
        return IO_ERROR;
    } /* Endswitch */

    return IO_OK;

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_cdc_install_driver
 * Returned Value : Success as USB_OK
 * Comments       :
 *     Adds / installs USB serial device driver to the MQX drivers
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_install_driver
(
    cdc_class_call_struct_t * data_instance,
    char * device_name
)
{
    usb_data_class_intf_struct_t * if_data_ptr;
    usb_status status = USB_OK;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_install_driver");
#endif

    /* Validity checking, always needed when passing data to lower API */
    if (usb_class_cdc_intf_validate(data_instance))
    {
        if_data_ptr = (usb_data_class_intf_struct_t *)data_instance->class_intf_handle;
        USB_CDC_DATA_lock();
        f_usb->dev_ptr->driver_init_ptr = data_instance;
        f_usb->dev_ptr->identifier = device_name;
        f_usb->dev_ptr->io_open = _io_cdc_serial_open;
        f_usb->dev_ptr->io_close = _io_cdc_serial_close;
        f_usb->dev_ptr->io_read = _io_cdc_serial_read;
        f_usb->dev_ptr->io_write = _io_cdc_serial_write;
        f_usb->dev_ptr->io_ioctl = _io_cdc_serial_ioctl;
        f_usb->dev_ptr->io_uninstall = NULL;
        f_usb->dev_ptr->driver_type = 0;
        if (status == IO_OK)
        {
            if_data_ptr->device_name = device_name;
        }
        USB_CDC_DATA_unlock();
    }

#ifdef _HOST_DEBUG_
    if (!status)
    {
        DEBUG_LOG_TRACE("usb_class_cdc_install_driver, SUCCESSFUL");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_class_cdc_install_driver, FAILED");
    }
#endif

    return status;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_acm_uninstall_driver
 * Returned Value : Success as USB_OK
 * Comments       :
 *     Removes USB serial device driver from the MQX drivers
 *END*--------------------------------------------------------------------*/

usb_status usb_class_cdc_uninstall_driver
(
    cdc_class_call_struct_t * data_instance
)
{
    //usb_data_class_intf_struct_t *  if_data_ptr;
    usb_status status = USB_OK;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_cdc_uninstall_driver");
#endif

    /* Validity checking, always needed when passing data to lower API */
    if (usb_class_cdc_intf_validate(data_instance))
    {
        //if_data_ptr = (usb_data_class_intf_struct_t *) data_instance->class_intf_handle;
        //USB_CDC_DATA_lock();
        //status = _io_dev_uninstall(if_ptr->device_name);
        //USB_CDC_DATA_unlock();
    }

#ifdef _HOST_DEBUG_
    if (!status)
    {
        DEBUG_LOG_TRACE("usb_class_acm_uninstall_driver, SUCCESSFUL");
    }
    else
    {
        DEBUG_LOG_TRACE("usb_class_acm_uninstall_driver, FAILED");
    }
#endif

    return status;
}
