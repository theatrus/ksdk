/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 - 2015 Freescale Semiconductor;
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
 * $FileName: khci.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file contains KHCI-specific implementations of USB interfaces
 *
 *END************************************************************************/
#include "usb_host_config.h"
#if USBCFG_HOST_KHCI
#include "usb.h"
#include "usb_host_stack_interface.h"

#include "usb_host_common.h"
#include "usb_host.h"

#include "khci.h"
#include "khci_prv.h"

#include "usb_host_dev_mng.h"
#include "fsl_usb_khci_hal.h"
#define USB_ASYNC_MODE 0

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) && USB_ASYNC_MODE 
#define TIMER_CALLBACK_ARG
#define USED_PIT1
#define USED_PIT0
#include "rtc_kinetis.h"
#endif

#ifdef KHCI_DEBUG
struct debug_messaging
{
    char inout;
    tr_msg_type_t type;
    uint8_t ep;
    uint16_t size;
};
volatile static uint16_t dm_index = 0;
volatile static struct debug_messaging dm[1024] = { 0 }; /* note, the array is for 1024 records only */
#   define KHCI_DEBUG_LOG(a, b, c, d) \
  { \
    dm[dm_index].inout = a; \
      dm[dm_index].type = b; \
      dm[dm_index].ep = c; \
      dm[dm_index].size = d; \
      dm_index++; \
  }
#else
#   define KHCI_DEBUG_LOG(a, b, c, d) {}
#endif

// KHCI task parameters
#define USB_KHCI_TASK_NUM_MESSAGES         16
#define USB_KHCI_TASK_TEMPLATE_INDEX       0
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)            /* USB stack running on MQX */
#define USB_KHCI_TASK_ADDRESS              _usb_khci_task_stun

#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)        /* USB stack running on BM  */
#define USB_KHCI_TASK_ADDRESS              _usb_khci_task

#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)            /* USB stack running on uCOS */
#if !(USE_RTOS)
#define USB_KHCI_TASK_ADDRESS               _usb_khci_task
#else
#define USB_KHCI_TASK_ADDRESS              _usb_khci_task_stun
#endif
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)  || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && USE_RTOS) /* USB stack running on MQX */
#define USB_NONBLOCKING_MODE 0

#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && (!USE_RTOS)) /* USB stack running on BM */
#define USB_NONBLOCKING_MODE 1
#endif

#define USB_KHCI_TASK_STACKSIZE            3500
#define USB_KHCI_TASK_NAME                 "KHCI Task"
#define USB_KHCI_TASK_ATTRIBUTES           0
#define USB_KHCI_TASK_CREATION_PARAMETER   0
#define USB_KHCI_TASK_DEFAULT_TIME_SLICE   0
#define USB_KHCI_MAX_SPEED_DETECTION_COUNT 3

// atom transaction error results
#define KHCI_ATOM_TR_PID_ERROR   (-1)
#define KHCI_ATOM_TR_EOF_ERROR   (-2)
#define KHCI_ATOM_TR_CRC_ERROR   (-4)
#define KHCI_ATOM_TR_TO          (-16)
#define KHCI_ATOM_TR_DMA_ERROR   (-32)
#define KHCI_ATOM_TR_BTS_ERROR   (-128)
#define KHCI_ATOM_TR_NAK         (-256)
#define KHCI_ATOM_TR_DATA_ERROR  (-512)
#define KHCI_ATOM_TR_STALL       (-1024)
#define KHCI_ATOM_TR_RESET       (-2048)
#define KHCI_ATOM_TR_BUS_TIMEOUT (-4096)
#define KHCI_ATOM_TR_INVALID     (-8192)
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK))
#if defined( __ICCCF__ ) || defined( __ICCARM__ )
    #pragma segment="USB_BDT_Z"
    #pragma data_alignment=512
    __no_init static uint8_t bdt[512] @ "USB_BDT_Z";
#elif defined(__GNUC__)
    __attribute__((aligned(512))) static uint8_t bdt[512];
#elif defined (__CC_ARM) 
    __align(512) uint8_t bdt[512]; 
#else
     #error Unsupported compiler, please use IAR, Keil or arm gcc compiler and rebuild the project.
#endif  
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
     static uint8_t *bdt;
#endif

#if USBCFG_KHCI_4BYTE_ALIGN_FIX
static uint8_t *_usb_khci_swap_buf_ptr = NULL;
#endif

#define MSG_SIZE_IN_MAX_TYPE (1 + (sizeof(tr_msg_struct_t) - 1) / sizeof(uint32_t))

usb_khci_host_state_struct_t* usb_host_global_handler;

/* Prototypes of functions */
usb_status _usb_khci_host_close_interface(usb_host_handle handle);
//static usb_status _usb_khci_host_close_pipe(usb_host_handle handle, pipe_struct_t* pipe_ptr);
static void _usb_khci_process_tr_complete(pipe_struct_t* pipe_desc_ptr, tr_struct_t* pipe_tr_ptr, uint32_t required, uint32_t remaining, int32_t err);
static int32_t _usb_khci_tr_done(usb_khci_host_state_struct_t* usb_host_ptr, tr_msg_struct_t msg);
static int32_t _usb_khci_atom_noblocking_tr(usb_khci_host_state_struct_t* usb_host_ptr,uint32_t type, pipe_struct_t* pipe_desc_ptr,uint8_t *buf_ptr,uint32_t len);

//extern uint8_t usb_host_dev_mng_get_address(usb_device_instance_handle dev_handle);
//extern uint8_t usb_host_dev_mng_get_speed(usb_device_instance_handle dev_handle);
//extern uint8_t usb_host_dev_mng_get_level(usb_device_instance_handle dev_handle);
//extern usb_status usb_host_dev_mng_attach(usb_host_handle handle, uint8_t speed, uint8_t hub_no, uint8_t port_no, uint8_t level, usb_device_instance_handle* dev_handle_ptr);
//extern usb_status usb_host_dev_mng_detach(usb_host_handle handle, uint8_t hub_no, uint8_t port_no);
//extern usb_status USB_log_error(char* file, uint32_t line, usb_status error);
//extern uint8_t usb_host_dev_mng_get_attach_state(usb_device_instance_handle dev_handle);
extern uint32_t OS_MsgQ_Is_Empty(os_msgq_handle msgq, void* msg);
extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);
extern uint32_t soc_get_usb_base_address(uint8_t controller_id);

// KHCI event bits
#define KHCI_EVENT_ATTACH       0x01
#define KHCI_EVENT_RESET        0x02
#define KHCI_EVENT_TOK_DONE     0x04
#define KHCI_EVENT_SOF_TOK      0x08
#define KHCI_EVENT_DETACH       0x10
#define KHCI_EVENT_MSG          0x20
#define KHCI_EVENT_ISO_MSG      0x40
#define KHCI_EVENT_NAK_MSG      0x80

#define KHCI_EVENT_MASK         0xff

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_get_hot_int_tr
 *  Returned Value : 0 successful
 *  Comments       :
 *        Make a message copy for transaction which need evaluation
 *END*-----------------------------------------------------------------*/
static uint32_t _usb_khci_get_total_frame_count(usb_khci_host_state_struct_t* usb_host_ptr)
{
    static uint32_t total_frame_number = 0;
    static uint16_t old_frame_number = 0;
    uint16_t frame_number = 0xFFFF;

    //frame_number = ((usb_ptr->FRMNUMH) << 8) | (usb_ptr->FRMNUML);
    frame_number = usb_hal_khci_get_frame_number(usb_host_ptr->usbRegBase);

    if(frame_number < old_frame_number)
    {
        total_frame_number += 2048;
    }

    old_frame_number = frame_number;

    //USB_PRINTF("t %d\n", frame_number + total_frame_number);

    return (frame_number + total_frame_number);
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_isr
 *  Returned Value : None
 *  Comments       :
 *        Service all the interrupts in the kirin usb hardware
 *END*-----------------------------------------------------------------*/
#ifdef USBCFG_OTG
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM))             /* USB stack running on MQX */
    void _usb_host_khci_isr(usb_host_handle handle)
#endif
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
    void _usb_host_khci_isr(void)
#endif
#else
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM))             /* USB stack running on MQX */
    void _usb_khci_isr(usb_host_handle handle)
#endif
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
    void _usb_khci_isr(void)
#endif
#endif
{
    usb_khci_host_state_struct_t* usb_host_ptr = (usb_khci_host_state_struct_t*)usb_host_global_handler;

    uint8_t status;
    //static uint32_t index = 0;

    while (1)
    {
        status = (uint8_t)(usb_hal_khci_get_interrupt_status(usb_host_ptr->usbRegBase) &
        usb_hal_khci_get_interrupt_enable_status(usb_host_ptr->usbRegBase));

        if (!status)
        {
            break;
        }

        usb_hal_khci_clr_interrupt(usb_host_ptr->usbRegBase,status);
        //usb_ptr->ISTAT = status;
        //USB_PRINTF("0x%x\n", usb_ptr->ISTAT);

        if (status & INTR_SOFTOK)
        {

            OS_Event_set(usb_host_ptr->khci_event_ptr, KHCI_EVENT_SOF_TOK);
        }

        if (status & INTR_ATTACH)
        {
            /* USB device is (being) attached */
            usb_hal_khci_disable_interrupts(usb_host_ptr->usbRegBase, INTR_ATTACH);
            //USB_PRINTF("0x%x\n", usb_ptr->INTEN);
            OS_Event_set(usb_host_ptr->khci_event_ptr, KHCI_EVENT_ATTACH);
        }

        if (status & INTR_TOKDNE)
        {
            // atom transaction done - token done
            //USB_PRINTF("k\n");
            /*if (usb_ptr->CTL & USB_CTL_TXSUSPENDTOKENBUSY_MASK)
             {
             USB_PRINTF("!!!!ERROR !!!!!! in ISR\n");
             }*/
            OS_Event_set(usb_host_ptr->khci_event_ptr, KHCI_EVENT_TOK_DONE);
        }

        if (status & INTR_USBRST)
        {
            // usb reset
            //USB_PRINTF("r\n");
            OS_Event_set(usb_host_ptr->khci_event_ptr, KHCI_EVENT_RESET);
        }
    }
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_init_int_tr
 *  Returned Value : None
 *  Comments       :
 *        Initialize interrupt transaction queue
 *END*-----------------------------------------------------------------*/
static void _usb_khci_init_tr_que(usb_khci_host_state_struct_t* usb_host_ptr)
{
    int32_t i;
    tr_int_que_itm_struct_t* tr = usb_host_ptr->tr_int_que;

    for (i = 0; i < USBCFG_HOST_KHCI_MAX_INT_TR; i++)
    {
        tr->msg.type = TR_MSG_UNKNOWN;
        tr++;
    }
    tr = usb_host_ptr->tr_nak_que;

    for (i = 0; i < USBCFG_HOST_KHCI_MAX_INT_TR; i++)
    {
        tr->msg.type = TR_MSG_UNKNOWN;
        tr++;
    }
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_add_int_tr
 *  Returned Value : -1 queue is full
 *  Comments       :
 *        Add new interrupt transaction to queue
 *END*-----------------------------------------------------------------*/
static int32_t _usb_khci_add_tr(usb_khci_host_state_struct_t* usb_host_ptr, tr_msg_struct_t *msg, uint32_t period,que_type_t type)
{
    int32_t i = 0;
    tr_int_que_itm_struct_t* tr = NULL;

    if (type == TYPE_INT)
    {
        tr= usb_host_ptr->tr_int_que;
    }
    else if(type == TYPE_NAK)
    {
        tr= usb_host_ptr->tr_nak_que;
    }
    //TIME_STRUCT tm;

    // find free position
    for (i = 0; i < USBCFG_HOST_KHCI_MAX_INT_TR; i++)
    {
        if (tr->msg.type == TR_MSG_UNKNOWN)
        {
            tr->period = period;
            tr->frame = _usb_khci_get_total_frame_count(usb_host_ptr) + period;

            OS_Mem_copy(msg, &tr->msg, sizeof(tr_msg_struct_t));
            //USB_PRINTF("TR 0x%x added target frame is %d\n", msg->pipe_tr, tr->frame);
            break;
        }
        tr++;
    }

    return (i < USBCFG_HOST_KHCI_MAX_INT_TR) ? i : -1;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_rm_int_tr
 *  Returned Value : 0 successful
 *  Comments       :
 *        Remove interrupt transaction from queue
 *END*-----------------------------------------------------------------*/
static int32_t _usb_khci_rm_tr(usb_khci_host_state_struct_t* usb_host_ptr, tr_msg_struct_t *msg, que_type_t type)
{
    int32_t i = 0;
    tr_int_que_itm_struct_t* tr = NULL;

    if (type == TYPE_INT)
    {
        tr= usb_host_ptr->tr_int_que;
    }
    else if (type == TYPE_NAK)
    {
        tr= usb_host_ptr->tr_nak_que;
    }
    // find record
    for (i = 0; i < USBCFG_HOST_KHCI_MAX_INT_TR; i++)
    {
        if ((tr->msg.pipe_desc == msg->pipe_desc) && (tr->msg.pipe_tr == msg->pipe_tr))
        {
            //OS_Mem_zero(tr, sizeof(tr_int_que_itm_struct_t));
            tr->msg.type = TR_MSG_UNKNOWN;
            //USB_PRINTF("TR 0x%x removed\n", tr->msg.pipe_tr);
            return 0;
        }
        tr++;
    }

    return -1;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_get_hot_int_tr
 *  Returned Value : 0 successful
 *  Comments       :
 *        Make a message copy for transaction which need evaluation
 *END*-----------------------------------------------------------------*/
static int32_t _usb_khci_get_hot_tr(usb_khci_host_state_struct_t* usb_host_ptr, tr_msg_struct_t *msg, que_type_t type)
{
    int32_t i, res = -1;
    //register TIME_STRUCT hot_time;
    uint32_t frame_number = 0xFFFFFFFF;

    tr_int_que_itm_struct_t* tr = NULL;
    tr_int_que_itm_struct_t* hot_tr = NULL;

    if(type == TYPE_INT)
    {
        tr= usb_host_ptr->tr_int_que;
    }
    else if(type == TYPE_NAK)
    {
        tr= usb_host_ptr->tr_nak_que;
    }
    //TIME_STRUCT tm;

    for (i = 0; i < USBCFG_HOST_KHCI_MAX_INT_TR; i++)
    {
        if (tr->msg.type != TR_MSG_UNKNOWN)
        {
            if (tr->frame < frame_number)
            {
                hot_tr = tr;
                frame_number = hot_tr->frame;
            }
        }
        tr++;
    }

    if (hot_tr)
    {
        /* test if hottest transaction was the last one with timeout - if yes, don't allow to block USB transfers with this interrupt */
        if (usb_host_ptr->last_to_pipe == hot_tr->msg.pipe_desc)
        {
            usb_host_ptr->last_to_pipe = NULL;  //it is allowed to perform this interrupt next time, but not now
            return res;
        }

        if (usb_host_ptr->device_attach_phy > 0)
        {
            frame_number = _usb_khci_get_total_frame_count(usb_host_ptr);
        }
        //USB_PRINTF("%d, %d\n", frame_number, hot_tr->frame);

        if (frame_number >= hot_tr->frame)
        {
            OS_Mem_copy(&hot_tr->msg, msg, sizeof(tr_msg_struct_t));
            res = 0;
            hot_tr->frame += hot_tr->period;
        }

    }

    return res;
}
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) && USB_ASYNC_MODE
/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_attach
 *  Returned Value : none
 *  Comments       :
 *        KHCI attach event
 *END*-----------------------------------------------------------------*/
static void _usb_khci_attach_timer_callback(void* arg)
{
    usb_khci_host_state_struct_t* usb_host_ptr = (usb_khci_host_state_struct_t*)arg;

    switch (usb_host_ptr->device_attach_state)
    {
        case USB_ATTACH_STATE_BEGIN:
            usb_host_ptr->device_attach_state = USB_ATTACH_STATE_SPEED_DETECTION;
            break;

        case USB_ATTACH_STATE_SPEED_DETECTION_BEGIN:
            usb_host_ptr->device_attach_state = USB_ATTACH_STATE_SPEED_DETECTION_TIMEOUT;
        break;

        case USB_ATTACH_STATE_RESET:
            usb_host_ptr->device_attach_state = USB_ATTACH_STATE_RESET_DONE;
            break;

        case USB_ATTACH_STATE_ENABLE_SOF:
            usb_host_ptr->device_attach_state = USB_ATTACH_STATE_SOF_ENABLED;
            break;

        default:
            break;
    }
    return;
}
#endif

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_attach
 *  Returned Value : none
 *  Comments       :
 *        KHCI attach event
 *END*-----------------------------------------------------------------*/
static void _usb_khci_attach(usb_khci_host_state_struct_t* usb_host_ptr)
{
    uint8_t speed;
    uint8_t temp;
    usb_device_instance_handle dev_handle;

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) && USB_ASYNC_MODE
    static uint8_t index = 0;
    timer_object_t timer;

    switch (usb_host_ptr->device_attach_state)
    {
        case USB_ATTACH_STATE_IDLE:
        {
            TimerQInitialize(0);
            usb_hal_khci_set_device_addr(usb_host_ptr->usbRegBase, 0);
            timer.ms_count = 150;
            timer.pfn_timer_callback = _usb_khci_attach_timer_callback;
            timer.arg = usb_host_ptr;
            usb_host_ptr->device_attach_state = USB_ATTACH_STATE_BEGIN;
            AddTimerQ(&timer);
            break;
        }

        case USB_ATTACH_STATE_SPEED_DETECTION:
        {
            // speed check, set
            usb_host_ptr->speed = usb_hal_khci_get_line_status(usb_host_ptr->usbRegBase);
            timer.ms_count = 5;
            timer.pfn_timer_callback = _usb_khci_attach_timer_callback;
            timer.arg = usb_host_ptr;
            usb_host_ptr->device_attach_state = USB_ATTACH_STATE_SPEED_DETECTION_BEGIN;
            AddTimerQ(&timer);
            break;
        }

        case USB_ATTACH_STATE_SPEED_DETECTION_TIMEOUT:
        {
            temp = usb_hal_khci_get_line_status(usb_host_ptr->usbRegBase);
            if (temp != usb_host_ptr->speed)
            {
                if (index < USB_KHCI_MAX_SPEED_DETECTION_COUNT)
                {
                    index++;
                }
                else
                {
                    index = 0;
                    usb_host_ptr->device_attach_state = USB_ATTACH_STATE_IDLE;
                    break;
                }
                usb_host_ptr->speed = temp;
                timer.ms_count = 5;
                timer.pfn_timer_callback = _usb_khci_attach_timer_callback;
                timer.arg = usb_host_ptr;
                usb_host_ptr->device_attach_state = USB_ATTACH_STATE_SPEED_DETECTION_BEGIN;
                AddTimerQ(&timer);
                break;
            }

            if (temp == USB_SPEED_FULL)
            {
                usb_hal_khci_disable_low_speed_support(usb_host_ptr->usbRegBase);
            }
            usb_hal_khci_clr_all_interrupts(usb_host_ptr->usbRegBase);   // clean each int flags
            usb_hal_khci_disable_interrupts(usb_host_ptr->usbRegBase, (INTR_TOKDNE | INTR_USBRST));

            // bus reset
            usb_hal_khci_start_bus_reset(usb_host_ptr->usbRegBase);
            timer.ms_count = 30;
            timer.pfn_timer_callback = _usb_khci_attach_timer_callback;
            timer.arg = usb_host_ptr;
            usb_host_ptr->device_attach_state = USB_ATTACH_STATE_RESET;
            AddTimerQ(&timer);
            break;
        }

        case USB_ATTACH_STATE_RESET_DONE:
        {
            usb_hal_khci_stop_bus_reset(usb_host_ptr->usbRegBase);
            // enable SOF sending
            usb_hal_khci_enable_sof(usb_host_ptr->usbRegBase);
            timer.ms_count = 100;
            timer.pfn_timer_callback = _usb_khci_attach_timer_callback;
            timer.arg = usb_host_ptr;
            usb_host_ptr->device_attach_state = USB_ATTACH_STATE_ENABLE_SOF;
            AddTimerQ(&timer);
            break;
        }

        case USB_ATTACH_STATE_SOF_ENABLED:
        {
            usb_hal_khci_enable_interrupts(usb_host_ptr->usbRegBase, (INTR_TOKDNE | INTR_USBRST));
            usb_host_ptr->device_attached ++;
            usb_host_dev_mng_attach((void*)usb_host_ptr->upper_layer_handle, NULL, usb_host_ptr->speed, 0, 0, 1, &dev_handle);
            usb_host_ptr->device_attach_state = USB_ATTACH_STATE_DONE;
            break;
        }

        default:
        break;
    }

    return;
#else
    uint8_t index = 0;

    usb_hal_khci_set_device_addr(usb_host_ptr->usbRegBase, 0);
#ifdef USBCFG_OTG
    OS_Time_delay(50);
#else
    OS_Time_delay(150);
#endif

    usb_hal_khci_disable_low_speed_support(usb_host_ptr->usbRegBase);
    do
    {
        temp = usb_hal_khci_get_line_status(usb_host_ptr->usbRegBase);
        OS_Time_delay(5);
        speed = usb_hal_khci_get_line_status(usb_host_ptr->usbRegBase);
        index++;
    }
    while ((temp != speed) && (index < USB_KHCI_MAX_SPEED_DETECTION_COUNT));

    if (temp != speed)
    {
#if _DEBUG
        USB_PRINTF("speed not match!\n");
#endif  
        return;
    }

    if (speed == USB_SPEED_FULL)
    {
        usb_hal_khci_disable_low_speed_support(usb_host_ptr->usbRegBase);
    }
    usb_hal_khci_clr_all_interrupts(usb_host_ptr->usbRegBase);   // clean each int flags
    usb_hal_khci_disable_interrupts(usb_host_ptr->usbRegBase, (INTR_TOKDNE | INTR_USBRST));

    // bus reset
    usb_hal_khci_start_bus_reset(usb_host_ptr->usbRegBase);
    OS_Time_delay(30);//wait for 30 milliseconds (2.5 is minimum for reset, 10 recommended)
    usb_hal_khci_stop_bus_reset(usb_host_ptr->usbRegBase);

    // Delay after reset was provided to be sure about speed- HS / FS. Since the KHCI does not run HS, the delay is redundant.
    // Some kinetis devices cannot recover after the delay, so it is better not to have delayed speed detection and SOF packet generation 
    // This is potential risk as any high priority task will get CPU now, the host will not begin the enumeration process.
    //OS_Time_delay(10);

    // enable SOF sending
    usb_hal_khci_enable_sof(usb_host_ptr->usbRegBase);
#ifdef USBCFG_OTG
    OS_Time_delay(30);
#else
    OS_Time_delay(100);
#endif
    usb_hal_khci_enable_interrupts(usb_host_ptr->usbRegBase, (INTR_TOKDNE | INTR_USBRST));
    usb_host_ptr->device_attached ++;

    usb_host_dev_mng_attach((void*)usb_host_ptr->upper_layer_handle, 0, speed, 0, 0, 1, &dev_handle);
#endif
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_reset
 *  Returned Value : none
 *  Comments       :
 *        KHCI reset event
 *END*-----------------------------------------------------------------*/
static void _usb_khci_reset(usb_khci_host_state_struct_t* usb_host_ptr)
{
    // clear attach flag
    usb_hal_khci_clr_interrupt(usb_host_ptr->usbRegBase, INTR_ATTACH);

    /* Test the presence of USB device */
    if (usb_hal_khci_is_interrupt_issued(usb_host_ptr->usbRegBase, INTR_ATTACH))
    {
        /* device attached, so really normal reset was performed */
        usb_hal_khci_set_device_addr(usb_host_ptr->usbRegBase,0);
        usb_hal_khci_endpoint_on_hub(usb_host_ptr->usbRegBase, 0);
    }
    else
    {
        /* device was detached, the reset event is false- never mind, notify about detach */
        //USB_PRINTF("d\n");
        OS_Event_set(usb_host_ptr->khci_event_ptr, KHCI_EVENT_DETACH);
    }
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_detach
 *  Returned Value : none
 *  Comments       :
 *        KHCI detach event
 *END*-----------------------------------------------------------------*/
static void _usb_khci_detach(usb_khci_host_state_struct_t* usb_host_ptr)
{

    if (usb_host_ptr->device_attached > 0)
    {
        usb_host_ptr->device_attached--;
    }
    else
    {
        return;
    }
    usb_host_dev_mng_detach((void*)usb_host_ptr->upper_layer_handle, 0, 0);
    usb_hal_khci_set_host_mode(usb_host_ptr->usbRegBase);

    /* This will clear all pending interrupts... In fact, there shouldn't be any
     ** after detaching a device.
     */
//    usb_hal_set_oddrst(usb_host_ptr->usbRegBase);
    usb_hal_khci_clr_all_interrupts(usb_host_ptr->usbRegBase);
//  usb_host_ptr->tx_bd = 1;

    //   usb_host_ptr->rx_bd = 1;
    /* Now, enable only USB interrupt attach for host mode */
    usb_hal_khci_enable_interrupts(usb_host_ptr->usbRegBase, INTR_ATTACH);
}

void _usb_khci_handle_iso_msg(usb_khci_host_state_struct_t* usb_host_ptr, tr_msg_struct_t msg)
{
    uint32_t remain = 0;
    uint32_t required = 0;
    int32_t res = 0;
    uint8_t *buf;
    switch (msg.type)
    {
        case TR_MSG_RECV:
        buf = msg.pipe_tr->rx_buffer;
        required = remain = msg.pipe_tr->rx_length;
        KHCI_DEBUG_LOG('p', msg.type, msg.pipe_desc->endpoint_number, required)
        do
        {
            _usb_khci_atom_noblocking_tr(usb_host_ptr, TR_IN, msg.pipe_desc, buf, remain);
            OS_Event_wait(usb_host_ptr->khci_event_ptr, KHCI_EVENT_TOK_DONE, FALSE, USBCFG_HOST_KHCI_WAIT_TICK);
            res = _usb_khci_tr_done(usb_host_ptr, msg);
            OS_Event_clear(usb_host_ptr->khci_event_ptr, KHCI_EVENT_TOK_DONE);

            if (res >= 0)
            {
                buf += res;
                remain -=res;
            }
            if ((res < 0) || (remain == 0) || (res < msg.pipe_desc->max_packet_size))
            {
                if (res >= 0)
                {
                    res = remain;
                }
                break;
            }

        }while (1);
        break;

        case TR_MSG_SEND:
        buf = msg.pipe_tr->tx_buffer;
        required = remain = msg.pipe_tr->tx_length;
        KHCI_DEBUG_LOG('p', msg.type, msg.pipe_desc->endpoint_number, required)
        do
        {
            _usb_khci_atom_noblocking_tr(usb_host_ptr, TR_OUT, msg.pipe_desc, buf, remain);
            OS_Event_wait(usb_host_ptr->khci_event_ptr, KHCI_EVENT_TOK_DONE, FALSE, USBCFG_HOST_KHCI_WAIT_TICK);
            res = _usb_khci_tr_done(usb_host_ptr, msg);
            OS_Event_clear(usb_host_ptr->khci_event_ptr, KHCI_EVENT_TOK_DONE);

            if (res >= 0)
            {
                buf += res;
                remain -=res;
            }

            if ((res < 0) || (remain == 0) || (res < msg.pipe_desc->max_packet_size))
            {
                if (res >= 0)
                {
                    res = remain;
                }
                break;
            }
        }while (1);
        break;
        default:
        break;
    }
    _usb_khci_process_tr_complete(msg.pipe_desc, msg.pipe_tr, required, remain, res);
}

#define ISO_TRANSFER_TIMERS 1
uint32_t _usb_process_iso_tr(usb_khci_host_state_struct_t* usb_host_ptr )
{
    tr_msg_struct_t msg;
    uint8_t timer = ISO_TRANSFER_TIMERS;
    uint8_t ret = 0;

    do
    {
        if (OS_MsgQ_Is_Empty(usb_host_ptr->tr_iso_que, (uint32_t *)&msg))
        {
            if ((OS_Event_check_bit(usb_host_ptr->khci_event_ptr, KHCI_EVENT_ISO_MSG)))
            {
                OS_Event_clear(usb_host_ptr->khci_event_ptr, KHCI_EVENT_ISO_MSG);
            }
            return 0;
        }
        _usb_khci_handle_iso_msg( usb_host_ptr, msg);
        ret = 1;

    }while (timer--);

    return ret;
}

static tr_msg_struct_t curr_msg;
static tr_msg_struct_t done_msg;
static volatile uint32_t tr_state = KHCI_TR_GET_MSG;
#if USBCFG_KHCI_4BYTE_ALIGN_FIX
typedef struct
{
    uint32_t rx_len;
    uint8_t *rx_buf;
    uint8_t *rx_buf_orig;
    bool is_dma_align;
}khci_xfer_sts_t;
static khci_xfer_sts_t s_xfer_sts = {0, NULL, NULL, TRUE};
#endif

static uint32_t deattached = 0;
static void _usb_khci_task(void* dev_inst_ptr)
{
    volatile ptr_usb_khci_host_state_struct_t usb_host_ptr = (usb_khci_host_state_struct_t*) dev_inst_ptr;
    static uint32_t remain = 0;
    static uint32_t required = 0;
    static int32_t res;
    uint8_t *buf;

    if (usb_host_ptr->device_attached)
    {
        if (tr_state == KHCI_TR_GET_MSG)
        {
            curr_msg.type = TR_MSG_UNKNOWN;
#if (USB_NONBLOCKING_MODE == 0)     
            OS_Event_wait(usb_host_ptr->khci_event_ptr, KHCI_EVENT_NAK_MSG|KHCI_EVENT_MSG|KHCI_EVENT_SOF_TOK, FALSE,5);
#else
            OS_Event_wait(usb_host_ptr->khci_event_ptr, KHCI_EVENT_NAK_MSG|KHCI_EVENT_MSG|KHCI_EVENT_SOF_TOK, FALSE,0);
#endif

            if ((OS_Event_check_bit(usb_host_ptr->khci_event_ptr, KHCI_EVENT_SOF_TOK)))
            {
                if ((OS_Event_check_bit(usb_host_ptr->khci_event_ptr, KHCI_EVENT_ISO_MSG)))
                {
                    _usb_process_iso_tr(usb_host_ptr);
                }

                OS_Event_clear(usb_host_ptr->khci_event_ptr, KHCI_EVENT_SOF_TOK);

            }

            if (_usb_khci_get_hot_tr(usb_host_ptr, &curr_msg, TYPE_INT) != 0)
            {
                if (_usb_khci_get_hot_tr(usb_host_ptr, &curr_msg, TYPE_NAK) != 0)
                {
                    ;
                }
                if ((OS_Event_check_bit(usb_host_ptr->khci_event_ptr, KHCI_EVENT_NAK_MSG)))
                {
                    OS_Event_clear(usb_host_ptr->khci_event_ptr, KHCI_EVENT_NAK_MSG);
                }
                if (curr_msg.type == TR_MSG_UNKNOWN)
                {
                    if (!OS_MsgQ_Is_Empty(usb_host_ptr->tr_que,&curr_msg))
                    {
                        if (curr_msg.pipe_desc->pipetype == USB_INTERRUPT_PIPE)
                        {
                            _usb_khci_add_tr(usb_host_ptr, &curr_msg, curr_msg.pipe_desc->interval,TYPE_INT);
                        }
                    }
                    else
                    {
                        if ((OS_Event_check_bit(usb_host_ptr->khci_event_ptr, KHCI_EVENT_MSG)))
                        {
                            OS_Event_clear(usb_host_ptr->khci_event_ptr, KHCI_EVENT_MSG);
                        }
                    }
                }

            }
            if (curr_msg.type != TR_MSG_UNKNOWN)
            {
                if (usb_host_ptr->device_attach_phy > 0)
                {
                    tr_state = KHCI_TR_START_TRANSMIT;
                }
                else
                {
                    OS_Mem_copy(&curr_msg, &done_msg, sizeof(tr_msg_struct_t));
                    done_msg.retry = 0;
                    res = KHCI_ATOM_TR_RESET;
                    required = 0;
                    tr_state = KHCI_TR_TRANSMIT_DONE;
                }
            }
            remain = 0;

        }
        if ((tr_state == KHCI_TR_START_TRANSMIT))
        {
            if ((curr_msg.msg_state == TR_MSG_NAK ))
            {
                if((_usb_khci_get_total_frame_count(usb_host_ptr) - curr_msg.frame) > curr_msg.naktimeout )
                {
                    res = KHCI_ATOM_TR_BUS_TIMEOUT;
                    tr_state = KHCI_TR_TRANSMIT_DONE;
                    done_msg = curr_msg;
                    //USB_PRINTF("1TR Timeout! %d %d\n", curr_msg.msg_state,curr_msg.naktimeout);
                    return;
                }
            }
            else if ((curr_msg.msg_state == TR_BUS_TIMEOUT ))
            {
                if ((_usb_khci_get_total_frame_count(usb_host_ptr) - curr_msg.frame) > TIMEOUT_OTHER )
                {
                    res = KHCI_ATOM_TR_BUS_TIMEOUT;
                    tr_state = KHCI_TR_TRANSMIT_DONE;
                    done_msg = curr_msg;
                    // USB_PRINTF("2TR Timeout! %d %d\n", curr_msg.msg_state,curr_msg.naktimeout);
                    return;
                }
            }
            if (curr_msg.type != TR_MSG_UNKNOWN)
            {
                switch (curr_msg.type)
                {
                    case TR_MSG_SETUP:
                        if ((curr_msg.pipe_tr->setup_status == 0))
                        {
                            _usb_khci_atom_noblocking_tr(usb_host_ptr, TR_CTRL, curr_msg.pipe_desc, (uint8_t *) &curr_msg.pipe_tr->setup_packet, 8);
                            remain = 8;
                        }
                        else if ( curr_msg.pipe_tr->setup_status == 1)
                        {
                            if (curr_msg.pipe_tr->rx_length)
                            {
                                buf = curr_msg.pipe_tr->rx_buffer;
                                required =  curr_msg.pipe_tr->rx_length;
                                remain = required - curr_msg.pipe_tr->transfered_length;
                                buf += curr_msg.pipe_tr->transfered_length; 
                                KHCI_DEBUG_LOG('p', curr_msg.type, curr_msg.pipe_desc->endpoint_number, required)
                                _usb_khci_atom_noblocking_tr(usb_host_ptr, TR_IN, curr_msg.pipe_desc, buf, remain);
                            }
                            else if (curr_msg.pipe_tr->tx_length)
                            {
                                buf = curr_msg.pipe_tr->tx_buffer;
                                required = curr_msg.pipe_tr->tx_length;
                                remain = required - curr_msg.pipe_tr->transfered_length;
                                buf += curr_msg.pipe_tr->transfered_length;
                                KHCI_DEBUG_LOG('p', curr_msg.type, curr_msg.pipe_desc->endpoint_number, required)
                                _usb_khci_atom_noblocking_tr(usb_host_ptr, TR_OUT, curr_msg.pipe_desc, buf, remain);
                            }
                            else
                            {
                                curr_msg.pipe_desc->nextdata01 = 1;
                                _usb_khci_atom_noblocking_tr(usb_host_ptr, TR_IN, curr_msg.pipe_desc, 0, 0);
                                curr_msg.pipe_tr->setup_status = 3;
                            }
                        }
                        else if( curr_msg.pipe_tr->setup_status == 2)
                        {
                            if (curr_msg.pipe_tr->rx_length)
                            {
                                curr_msg.pipe_desc->nextdata01 = 1;
                                _usb_khci_atom_noblocking_tr(usb_host_ptr, TR_OUT, curr_msg.pipe_desc, 0, 0);
                            }
                            else if (curr_msg.pipe_tr->tx_length)
                            {
                                curr_msg.pipe_desc->nextdata01 = 1;
                                _usb_khci_atom_noblocking_tr(usb_host_ptr, TR_IN, curr_msg.pipe_desc, 0, 0);
                            }
                            else
                            {
                                curr_msg.pipe_desc->nextdata01 = 1;
                                _usb_khci_atom_noblocking_tr(usb_host_ptr, TR_IN, curr_msg.pipe_desc, 0, 0);
                            }
                        }
                        else if ( curr_msg.pipe_tr->setup_status == 3)
                        {
                            curr_msg.pipe_desc->nextdata01 = 1;
                            _usb_khci_atom_noblocking_tr(usb_host_ptr, TR_IN, curr_msg.pipe_desc, 0, 0);
                        }
                        break;

                    case TR_MSG_RECV:
                        buf = curr_msg.pipe_tr->rx_buffer;
                        required =  curr_msg.pipe_tr->rx_length;
                        remain = required - curr_msg.pipe_tr->transfered_length;
                        buf += curr_msg.pipe_tr->transfered_length;
                        KHCI_DEBUG_LOG('p', curr_msg.type, curr_msg.pipe_desc->endpoint_number, required)
                        _usb_khci_atom_noblocking_tr(usb_host_ptr, TR_IN, curr_msg.pipe_desc, buf, remain);         
                        break;

                    case TR_MSG_SEND:
                        buf = curr_msg.pipe_tr->tx_buffer;
                        required =  curr_msg.pipe_tr->tx_length;
                        remain = required - curr_msg.pipe_tr->transfered_length;
                        buf += curr_msg.pipe_tr->transfered_length;
                        _usb_khci_atom_noblocking_tr(usb_host_ptr, TR_OUT, curr_msg.pipe_desc, buf, remain);
                        break;

                    default:
                        break;
                }

            }
        tr_state = KHCI_TR_TRANSMITING;
        }
        else if (( tr_state == KHCI_TR_TRANSMITING))
        { 
            if (curr_msg.pipe_desc->pipetype != USB_INTERRUPT_PIPE)
            {
                if ((_usb_khci_get_total_frame_count(usb_host_ptr) - curr_msg.frame) > TIMEOUT_OTHER )
                {
                    res = KHCI_ATOM_TR_BUS_TIMEOUT;
                    tr_state = KHCI_TR_TRANSMIT_DONE;
                    done_msg = curr_msg;
                    USB_PRINTF("3TR Timeout!\n");
                    return;
                }
            }
        }
        else if (tr_state == KHCI_TR_TRANSMIT_DONE)
        {
            if (done_msg.pipe_desc->pipetype == USB_INTERRUPT_PIPE)
            {
                /* for interrupt pipes, callback only if some data was received or serious error occurred */
                if ((required != remain) || ((res != KHCI_ATOM_TR_NAK) ))
                {
                    _usb_khci_process_tr_complete(done_msg.pipe_desc, done_msg.pipe_tr, required, remain, res);
                    if (curr_msg.msg_state == TR_BUS_TIMEOUT)
                    {
                        _usb_khci_rm_tr(usb_host_ptr, &done_msg, TYPE_NAK);
                    }
                    _usb_khci_rm_tr(usb_host_ptr, &done_msg, TYPE_INT);
                }
            }
            else
            {
                if(done_msg.msg_state != TR_MSG_IDLE )
                {
                    _usb_khci_rm_tr(usb_host_ptr, &done_msg, TYPE_NAK);
                    if (done_msg.pipe_desc->pipetype == USB_CONTROL_PIPE)
                    {
                        if ((res < 0) && (done_msg.retry))
                        {
                            tr_state = KHCI_TR_GET_MSG;
                            done_msg.type = TR_MSG_SETUP;

                            done_msg.pipe_tr->setup_status = 0;
                            done_msg.pipe_tr->transfered_length = 0;
                            done_msg.retry --;

                            if(done_msg.pipe_tr->rx_length)
                            {
                                done_msg.naktimeout = TIMEOUT_TOHOST;
                            }
                            else if(done_msg.pipe_tr->tx_length)
                            {
                                done_msg.naktimeout = TIMEOUT_TODEVICE;
                            }
                            else
                            {
                                done_msg.naktimeout = TIMEOUT_NODATA;
                            }

                            done_msg.frame = _usb_khci_get_total_frame_count(usb_host_ptr);
                            done_msg.msg_state = TR_BUS_TIMEOUT;
                            _usb_khci_add_tr(usb_host_ptr, &done_msg, NAK_RETRY_TIME*5,TYPE_NAK);
                         USB_PRINTF("Retry @@\n\r");
                            return;
                        }

                    }

                }
                _usb_khci_process_tr_complete(done_msg.pipe_desc, done_msg.pipe_tr, required, remain, res);
            }
            tr_state = KHCI_TR_GET_MSG;
            if(deattached == 1)
            {
                _usb_khci_detach(usb_host_ptr);
                deattached = 0;
            }
        }
    }
    else
    {
        // wait for event if device is not attached
        OS_Event_wait(usb_host_ptr->khci_event_ptr, KHCI_EVENT_ATTACH, FALSE, 0);
    }

    if (OS_Event_check_bit(usb_host_ptr->khci_event_ptr, KHCI_EVENT_MASK))
    {
        if (OS_Event_check_bit(usb_host_ptr->khci_event_ptr, KHCI_EVENT_ATTACH))
        {
            OS_Event_clear(usb_host_ptr->khci_event_ptr, KHCI_EVENT_ATTACH);
            usb_host_ptr->device_attach_phy = 1;
            _usb_khci_init_tr_que(usb_host_ptr);
            _usb_khci_attach(usb_host_ptr);
            tr_state = KHCI_TR_GET_MSG;
        }

        if (OS_Event_check_bit(usb_host_ptr->khci_event_ptr, KHCI_EVENT_RESET))
        {
            OS_Event_clear(usb_host_ptr->khci_event_ptr, KHCI_EVENT_RESET | KHCI_EVENT_TOK_DONE);
            _usb_khci_reset(usb_host_ptr);
        }

        if (OS_Event_check_bit(usb_host_ptr->khci_event_ptr, KHCI_EVENT_DETACH))
        {
            OS_Event_clear(usb_host_ptr->khci_event_ptr, KHCI_EVENT_DETACH);
            usb_host_ptr->device_attach_phy = 0;
            if (KHCI_TR_TRANSMITING == tr_state)
            {
                tr_state = KHCI_TR_TRANSMIT_DONE;
                done_msg = curr_msg;
                deattached = 1;
            }
            else
            {
                _usb_khci_detach(usb_host_ptr);
            }
            OS_Event_clear(usb_host_ptr->khci_event_ptr, KHCI_EVENT_MSG);
            usb_hal_khci_clr_all_interrupts(usb_host_ptr->usbRegBase);
            /* Enable week pull-downs, useful for detecting detach (effectively bus discharge) */
            usb_hal_khci_enable_pull_down(usb_host_ptr->usbRegBase);
            /* Remove suspend state */
            usb_hal_khci_clr_suspend(usb_host_ptr->usbRegBase);

            usb_hal_khci_set_oddrst(usb_host_ptr->usbRegBase);

            usb_hal_khci_set_host_mode(usb_host_ptr->usbRegBase);

            usb_host_ptr->tx_bd = 0;
            usb_host_ptr->rx_bd = 0;

        }
        if (OS_Event_check_bit(usb_host_ptr->khci_event_ptr, KHCI_EVENT_TOK_DONE))
        {
            OS_Event_clear(usb_host_ptr->khci_event_ptr, KHCI_EVENT_TOK_DONE);
            res = _usb_khci_tr_done(usb_host_ptr, curr_msg);
            //_usb_process_iso_tr(usb_host_ptr);
            if (res> 0)
            {
                if (curr_msg.type != TR_MSG_UNKNOWN)
                {
                    switch (curr_msg.type)
                    {
                        case TR_MSG_SETUP:
                            if (( curr_msg.pipe_tr->setup_status == 2)||( curr_msg.pipe_tr->setup_status == 3))
                            {
                                tr_state = KHCI_TR_TRANSMIT_DONE;
                                done_msg = curr_msg;
                                curr_msg.pipe_tr->setup_status = 0;
                            }
                            else
                            {
                                tr_state = KHCI_TR_START_TRANSMIT;
                                remain -= res;
                                if(curr_msg.pipe_tr->setup_status ==1)
                                {
                                    curr_msg.pipe_tr->transfered_length +=res;
                                    if ((remain <= 0)|| (res < curr_msg.pipe_desc->max_packet_size)) 
                                    {
                                        curr_msg.pipe_tr->setup_status ++;
                                    }
                                }
                                else
                                {
                                    curr_msg.pipe_tr->setup_status ++;
                                }
                            }
                            break;

                            case TR_MSG_RECV:
                                remain -= res;
                                curr_msg.pipe_tr->transfered_length +=res;
                                if ((remain == 0)|| (res < curr_msg.pipe_desc->max_packet_size)) 
                                {
                                    tr_state = KHCI_TR_TRANSMIT_DONE;
                                    done_msg = curr_msg;
                                    break;
                                }
                                else
                                {
                                    tr_state = KHCI_TR_START_TRANSMIT; 
                                }
                                break;

                            case TR_MSG_SEND:
                                remain -= res;
                                curr_msg.pipe_tr->transfered_length +=res;
                                if ((remain == 0)|| (res < curr_msg.pipe_desc->max_packet_size)) 
                                {
                                    tr_state = KHCI_TR_TRANSMIT_DONE;
                                    done_msg = curr_msg;
                                    break;
                                }
                                else
                                {
                                    tr_state = KHCI_TR_START_TRANSMIT; 
                                }
                                break;

                            default:
                                break;
                    }
                }
            }
            else
            {
                if ((res == KHCI_ATOM_TR_NAK) && (curr_msg.pipe_desc->pipetype != USB_INTERRUPT_PIPE))
                {
                    if (usb_host_dev_mng_get_attach_state(curr_msg.pipe_desc->dev_instance))
                    {
                        if (curr_msg.msg_state == TR_MSG_IDLE)
                        {
                            curr_msg.msg_state = TR_MSG_NAK;
                            _usb_khci_add_tr(usb_host_ptr, &curr_msg, 0, TYPE_NAK);
                        }
                        OS_Event_set(usb_host_ptr->khci_event_ptr, KHCI_EVENT_NAK_MSG);
                        tr_state = KHCI_TR_GET_MSG;
                    }
                    else
                    {
                        tr_state = KHCI_TR_TRANSMIT_DONE;
                        done_msg = curr_msg;
                    }
                }
                else if ((res == KHCI_ATOM_TR_BUS_TIMEOUT)&& (curr_msg.pipe_desc->pipetype != USB_INTERRUPT_PIPE))
                {
                    if (curr_msg.retry == 0)
                    {
                        tr_state = KHCI_TR_TRANSMIT_DONE;
                        done_msg = curr_msg;
                        return;
                    }
                    if (usb_host_dev_mng_get_attach_state(curr_msg.pipe_desc->dev_instance))
                    {
                        curr_msg.retry --;
                        if (curr_msg.msg_state == TR_MSG_IDLE)
                        {
                            curr_msg.msg_state = TR_BUS_TIMEOUT;
                            _usb_khci_add_tr(usb_host_ptr, &curr_msg, NAK_RETRY_TIME*5,TYPE_NAK);
                        }
                        tr_state = KHCI_TR_GET_MSG;
                    }
                    else
                    {
                        tr_state = KHCI_TR_TRANSMIT_DONE;
                        done_msg = curr_msg;
                    }
                }
                else
                {
                    tr_state = KHCI_TR_TRANSMIT_DONE;
                    done_msg = curr_msg;
                }
            }
        }
    }
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_task_stun
 *  Returned Value : none
 *  Comments       :
 *        KHCI task
 *END*-----------------------------------------------------------------*/
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && (USE_RTOS))) 
static void _usb_khci_task_stun(void* dev_inst_ptr)
{
    while (1)
    {
        _usb_khci_task(dev_inst_ptr);
    }
}
#endif

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_task_create
 *  Returned Value : error or USB_OK
 *  Comments       :
 *        Create KHCI task
 *END*-----------------------------------------------------------------*/
uint32_t task_id;
static usb_status _usb_khci_task_create(usb_host_handle handle)
{
    //usb_status status;
    //task_id = _task_create_blocked(0, 0, (uint32_t)&task_template);
    task_id = OS_Task_create(USB_KHCI_TASK_ADDRESS, (void*)handle, (uint32_t)USBCFG_HOST_KHCI_TASK_PRIORITY, USB_KHCI_TASK_STACKSIZE, USB_KHCI_TASK_NAME, NULL);

    if (task_id == (uint32_t)OS_TASK_ERROR)
    {
        return USBERR_ERROR;
    }

    //_task_ready(_task_get_td(task_id));
    //OS_Task_resume(task_id);

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : usb_khci_preinit
 *  Returned Value : error or USB_OK
 *  Comments       :
 *        Allocate the structures for KHCI
 *END*-----------------------------------------------------------------*/
usb_status usb_khci_preinit(usb_host_handle upper_layer_handle, usb_host_handle *handle)
{
    usb_khci_host_state_struct_t* usb_host_ptr = (usb_khci_host_state_struct_t*) OS_Mem_alloc_zero(sizeof(usb_khci_host_state_struct_t));
    pipe_struct_t* p;
    pipe_struct_t* pp;
    int32_t i;

    if (NULL != usb_host_ptr)
    {
        usb_host_ptr->device_attach_phy = 0;
        /* Allocate the USB Host Pipe Descriptors */
        usb_host_ptr->pipe_descriptor_base_ptr = (pipe_struct_t*)OS_Mem_alloc_zero(sizeof(pipe_struct_t) * USBCFG_HOST_MAX_PIPES);
        if (usb_host_ptr->pipe_descriptor_base_ptr == NULL)
        {
            OS_Mem_free(usb_host_ptr);
            return USBERR_ALLOC;
        }

        p = (pipe_struct_t*) usb_host_ptr->pipe_descriptor_base_ptr;
        pp = NULL;
        for (i = 0; i < USBCFG_HOST_MAX_PIPES; i++)
        {
            if (pp != NULL)
            {
                pp->next = (pipe_struct_t*) p;
            }
            pp = p;
            p++;
        }
        usb_host_ptr->upper_layer_handle = upper_layer_handle;
        //usb_host_ptr->G.PIPE_SIZE = sizeof(KHCI_PIPE_STRUCT);
        //usb_host_ptr->G.TR_SIZE = sizeof(KHCI_TR_STRUCT);

        *handle = (usb_host_handle) usb_host_ptr;
#if USBCFG_KHCI_4BYTE_ALIGN_FIX
        if (NULL == (_usb_khci_swap_buf_ptr = (uint8_t *)OS_Mem_alloc_uncached_zero(USBCFG_HOST_KHCI_SWAP_BUF_MAX + 4)))
        {
#ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_khci_swap_buf_ptr- memory allocation failed");
#endif
            return USB_log_error(__FILE__,__LINE__,USBERR_ALLOC);
        }
#endif
        return USB_OK;
    }
    else
    {
        *handle = NULL;
        return USBERR_ALLOC;
    }
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : usb_khci_init
 *  Returned Value : error or USB_OK
 *  Comments       :
 *        Initialize the kirin HCI controller
 *END*-----------------------------------------------------------------*/
usb_status usb_khci_init(uint8_t controller_id, usb_host_handle handle)
{
    usb_khci_host_state_struct_t* usb_host_ptr = (usb_khci_host_state_struct_t*)handle;
    usb_status status = USB_OK;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    bdt = (uint8_t *)OS_Mem_alloc_uncached_align(512, 512);
#endif
    usb_host_ptr->khci_event_ptr = OS_Event_create(0);
    if (usb_host_ptr->khci_event_ptr == NULL)
    {
#if _DEBUG
        USB_PRINTF(" memalloc failed in usb_khci_init\n");
#endif

        return USBERR_ALLOC;
    } /* Endif */

    /* The _lwmsgq_init accepts the size of tr_msg_struct_t as a multiplier of sizeof(_mqx_max_type) */
    usb_host_ptr->tr_que = (os_msgq_handle)OS_MsgQ_create(USBCFG_HOST_KHCI_TR_QUE_MSG_CNT, MSG_SIZE_IN_MAX_TYPE);
    if (usb_host_ptr->tr_que == NULL)
    {
        return USBERR_ALLOC;
    }

    usb_host_ptr->tr_iso_que = (os_msgq_handle)OS_MsgQ_create(USBCFG_HOST_KHCI_TR_QUE_MSG_CNT, MSG_SIZE_IN_MAX_TYPE);
    if (usb_host_ptr->tr_iso_que == NULL)
    {
        return USBERR_ALLOC;
    }
    usb_host_ptr->tr_que_bak= (os_msgq_handle)OS_MsgQ_create(USBCFG_HOST_KHCI_TR_QUE_MSG_CNT, MSG_SIZE_IN_MAX_TYPE);
    if (usb_host_ptr->tr_que_bak == NULL)
    {
        return USBERR_ALLOC;
    }

    _usb_khci_init_tr_que(usb_host_ptr);

    usb_host_ptr->controller_id = controller_id;
    //usb_ptr = usb_host_ptr->DEV_PTR = soc_get_usb_base_address(controller_id);
    usb_host_ptr->vector_number = soc_get_usb_vector_number(controller_id);
    //usb_host_ptr->vector_number = INT_USB0;
    usb_host_ptr->usbRegBase = soc_get_usb_base_address(controller_id);

    _usb_khci_task_create(usb_host_ptr);

    usb_host_global_handler = usb_host_ptr;
#ifndef USBCFG_OTG
    /* install isr */

#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)) 
    if (!(OS_install_isr(usb_host_ptr->vector_number, _usb_khci_isr, (void*)usb_host_ptr )))
    {
        return USBERR_INSTALL_ISR;
    }
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
    OS_install_isr(usb_host_ptr->vector_number, _usb_khci_isr, (void*)usb_host_ptr);
#endif
#endif 
    usb_hal_khci_clr_all_interrupts(usb_host_ptr->usbRegBase);
    /* Enable week pull-downs, useful for detecting detach (effectively bus discharge) */
    usb_hal_khci_enable_pull_down(usb_host_ptr->usbRegBase);
    /* Remove suspend state */
    usb_hal_khci_clr_suspend(usb_host_ptr->usbRegBase);
    usb_hal_khci_set_oddrst(usb_host_ptr->usbRegBase);
    usb_hal_khci_set_buffer_descriptor_table_addr(usb_host_ptr->usbRegBase, (uint32_t)BDT_BASE);

    /* Set SOF threshold */
    //usb_ptr->SOFTHLD = 1;
    usb_hal_khci_set_sof_theshold(usb_host_ptr->usbRegBase, 255);

    usb_hal_khci_set_host_mode(usb_host_ptr->usbRegBase);

    /* Following is for OTG control instead of internal bus control */
//    usb_ptr->OTGCTL = USB_OTGCTL_DMLOW_MASK | USB_OTGCTL_DPLOW_MASK | USB_OTGCTL_OTGEN_MASK;
    /* Wait for attach interrupt */
    usb_hal_khci_enable_interrupts(usb_host_ptr->usbRegBase, INTR_ATTACH| INTR_SOFTOK);

    return status;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : usb_khci_shutdown - NOT IMPLEMENT YET
 *  Returned Value : error or USB_OK
 *  Comments       :
 *        The function to shutdown the host
 *END*-----------------------------------------------------------------*/
usb_status usb_khci_shutdown
(
usb_host_handle handle
)
{
    usb_khci_host_state_struct_t* usb_host_ptr = (usb_khci_host_state_struct_t*)handle;

    if (task_id != (uint32_t)OS_TASK_ERROR)
    {
        OS_Task_delete(task_id);
    }
    usb_hal_khci_disable_interrupts(usb_host_ptr->usbRegBase,0xFF);

    usb_hal_khci_set_device_addr(usb_host_ptr->usbRegBase,0);

    usb_hal_khci_clear_control_register(usb_host_ptr->usbRegBase);
    usb_hal_khci_enable_pull_down(usb_host_ptr->usbRegBase);
    usb_hal_khci_set_suspend(usb_host_ptr->usbRegBase);
    usb_hal_khci_disable_low_speed_support(usb_host_ptr->usbRegBase);

    if (NULL != usb_host_ptr->pipe_descriptor_base_ptr)
    {
        OS_Mem_free(usb_host_ptr->pipe_descriptor_base_ptr);
    }
    if (NULL != usb_host_ptr->khci_event_ptr)
    {
        OS_Event_destroy(usb_host_ptr->khci_event_ptr);
    }
    if (NULL != usb_host_ptr->tr_que)
    {
        OS_MsgQ_destroy(usb_host_ptr->tr_que);
    }
    if (NULL != usb_host_ptr->tr_iso_que)
    {
        OS_MsgQ_destroy(usb_host_ptr->tr_iso_que);
    }
    if (NULL != usb_host_ptr->tr_que_bak)
    {
        OS_MsgQ_destroy(usb_host_ptr->tr_que_bak);
    }
    usb_host_global_handler = NULL;
    OS_Mem_free(usb_host_ptr);
#if USBCFG_KHCI_4BYTE_ALIGN_FIX
    if (NULL != _usb_khci_swap_buf_ptr)
    {
        OS_Mem_free(_usb_khci_swap_buf_ptr);
    }
#endif
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    if (NULL != bdt)
    {
        OS_Mem_free(bdt);
        bdt = NULL;
    }
#endif
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : usb_khci_open_pipe
 *  Returned Value : error or USB_OK
 *  Comments       :
 *        The function to open a pipe
 *END*-----------------------------------------------------------------*/
usb_status usb_khci_open_pipe
(
usb_host_handle handle,
/* [OUT] Handle of opened pipe */
usb_pipe_handle * pipe_handle_ptr,
pipe_init_struct_t* pipe_init_ptr
)
{
    usb_khci_host_state_struct_t* usb_host_ptr = (usb_khci_host_state_struct_t*) handle;
    pipe_struct_t* pipe_ptr;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_khci_open_pipe");
#endif

    OS_Lock();

    for (pipe_ptr = usb_host_ptr->pipe_descriptor_base_ptr; pipe_ptr != NULL; pipe_ptr = pipe_ptr->next)
    {
        if (!pipe_ptr->open)
        {
            pipe_ptr->open = (uint8_t)TRUE;
            break;
        }
    }

    OS_Unlock();

    if (pipe_ptr == NULL)
    {
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_khci_open_pipe failed");
#endif
        return USB_log_error(__FILE__,__LINE__,USBERR_OPEN_PIPE_FAILED);
    }

    pipe_ptr->endpoint_number = pipe_init_ptr->endpoint_number;
    pipe_ptr->direction       = pipe_init_ptr->direction;
    pipe_ptr->pipetype        = pipe_init_ptr->pipetype;
    pipe_ptr->max_packet_size = pipe_init_ptr->max_packet_size & PACKET_SIZE_MASK;
    pipe_ptr->flags           = pipe_init_ptr->flags;
    pipe_ptr->nak_count       = pipe_init_ptr->nak_count;
    pipe_ptr->nextdata01      = 0;
    pipe_ptr->tr_list_ptr     = NULL;
    pipe_ptr->dev_instance    = pipe_init_ptr->dev_instance;

    if (pipe_ptr->pipetype == USB_ISOCHRONOUS_PIPE)
    {
        pipe_ptr->interval = 1 << (pipe_init_ptr->interval - 1);
    }
    else
    {
        pipe_ptr->interval = pipe_init_ptr->interval;
    }

    *pipe_handle_ptr = pipe_ptr;

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : usb_khci_close_pipe
 *  Returned Value : error or USB_OK
 *  Comments       :
 *        The function to open a pipe
 *END*-----------------------------------------------------------------*/
usb_status usb_khci_close_pipe
(
usb_host_handle handle,
/* [in] Handle of opened pipe */
usb_pipe_handle pipe_handle
)
{
    usb_khci_host_state_struct_t* usb_host_ptr = (usb_khci_host_state_struct_t*) handle;
    pipe_struct_t* pipe_ptr = (pipe_struct_t*)pipe_handle;
    uint8_t matched = (uint8_t)FALSE;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_khci_close_pipe");
#endif

    OS_Lock();

    if ((pipe_ptr != NULL) && (pipe_ptr->open == (uint8_t)TRUE))
    {
        for (pipe_ptr = usb_host_ptr->pipe_descriptor_base_ptr; pipe_ptr != NULL; pipe_ptr = pipe_ptr->next)
        {
            if ((pipe_ptr->open) && (pipe_ptr == pipe_handle))
            {
                matched = (uint8_t)TRUE;
                break;
            }
        }
        if (matched)
        {
            pipe_ptr                  = (pipe_struct_t*)pipe_handle;
            pipe_ptr->open            = (uint8_t)FALSE;
            pipe_ptr->dev_instance    = NULL;
            pipe_ptr->endpoint_number = 0;
            pipe_ptr->direction       = 0;
            pipe_ptr->pipetype        = 0;
            pipe_ptr->max_packet_size = 0;
            pipe_ptr->interval        = 0;
            pipe_ptr->flags           = 0;
            pipe_ptr->nak_count       = 0;
            pipe_ptr->nextdata01      = 0;
            pipe_ptr->tr_list_ptr     = NULL;
        }
        else
        {
            USB_PRINTF("usb_khci_close_pipe can't find target pipe\n");
        }
    }
    else
    {
        USB_PRINTF("usb_khci_close_pipe invalid pipe \n");
    }

    OS_Unlock();

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : usb_khci_send
 *  Returned Value : error or USB_OK
 *  Comments       :
 *        The function to send data
 *END*-----------------------------------------------------------------*/
usb_status usb_khci_send(usb_host_handle handle, pipe_struct_t* pipe_ptr, tr_struct_t* tr_ptr)
{
    usb_status status = USB_OK;
    usb_khci_host_state_struct_t* usb_host_ptr = (usb_khci_host_state_struct_t*) handle;
    tr_msg_struct_t msg;

    msg.type = TR_MSG_SEND;
    msg.msg_state = TR_MSG_IDLE;
    msg.pipe_desc = pipe_ptr;
    msg.pipe_tr = tr_ptr;
    msg.pipe_tr->transfered_length = 0;
    if (pipe_ptr->nak_count == 0)
    {
        msg.naktimeout = TIMEOUT_DEFAULT;
    }
    else
    {
        msg.naktimeout = pipe_ptr->nak_count *NAK_RETRY_TIME;
    }
    msg.frame = _usb_khci_get_total_frame_count(usb_host_ptr);
    msg.retry = RETRY_TIME;
    KHCI_DEBUG_LOG('i', msg.type, msg.pipe_desc->endpoint_number, msg.pipe_tr->tx_length)
    if (pipe_ptr->pipetype ==USB_ISOCHRONOUS_PIPE )
    {
        if (0 != OS_MsgQ_send(usb_host_ptr->tr_iso_que, (void *)&msg, 0))
        {
            status = USBERR_TR_FAILED;
        }
        OS_Event_set(usb_host_ptr->khci_event_ptr, KHCI_EVENT_ISO_MSG);
    }
    else
    {
        if (0 != OS_MsgQ_send(usb_host_ptr->tr_que, (void *)&msg, 0))
        {
            status = USBERR_TR_FAILED;
        }
        OS_Event_set(usb_host_ptr->khci_event_ptr, KHCI_EVENT_MSG);
    }
    return status;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : usb_khci_send_setup
 *  Returned Value : error or USB_OK
 *  Comments       :
 *        The function to send setup data
 *END*-----------------------------------------------------------------*/
usb_status usb_khci_send_setup(usb_host_handle handle, pipe_struct_t* pipe_ptr, tr_struct_t* tr_ptr)
{
    usb_status status = USB_OK;
    usb_khci_host_state_struct_t* usb_host_ptr = (usb_khci_host_state_struct_t*) handle;
    tr_msg_struct_t msg;

#if USBCFG_KHCI_4BYTE_ALIGN_FIX
    if ((tr_ptr->rx_length & USB_DMA_ALIGN_MASK) || ((uint32_t)tr_ptr->rx_buffer & USB_DMA_ALIGN_MASK))
    {
        if (_usb_khci_swap_buf_ptr == NULL)
        {
            return USBERR_LACK_OF_SWAP_BUFFER;
        }
        if (pipe_ptr->max_packet_size > USBCFG_HOST_KHCI_SWAP_BUF_MAX)
        {
            return USBERR_LEAK_OF_SWAP_BUFFER;
        }
    }
#endif

    msg.type = TR_MSG_SETUP;
    msg.msg_state = TR_MSG_IDLE;
    msg.pipe_desc = pipe_ptr;
    msg.pipe_tr = tr_ptr;
    msg.pipe_tr->setup_status = 0;
    msg.pipe_tr->transfered_length = 0;
    msg.retry = RETRY_TIME;

    if (msg.pipe_tr->rx_length)
    {
        msg.naktimeout = TIMEOUT_TOHOST;
    }
    else if (msg.pipe_tr->tx_length)
    {
        msg.naktimeout = TIMEOUT_TODEVICE;
    }
    else
    {
        msg.naktimeout = TIMEOUT_NODATA;
    }

    msg.frame = _usb_khci_get_total_frame_count(usb_host_ptr);

    if (msg.pipe_tr->rx_length)
    {
        KHCI_DEBUG_LOG('i', msg.type, msg.pipe_desc->endpoint_number, msg.pipe_tr->rx_length)
    }
    else
    {
        KHCI_DEBUG_LOG('i', msg.type, msg.pipe_desc->endpoint_number, msg.pipe_tr->tx_length)
    }

    if(0 != OS_MsgQ_send(usb_host_ptr->tr_que, (void *)&msg, 0))
    {
        status = USBERR_TR_FAILED;
    }
    OS_Event_set(usb_host_ptr->khci_event_ptr, KHCI_EVENT_MSG);

    return status;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : usb_khci_recv
 *  Returned Value : error or USB_OK
 *  Comments       :
 *        The function to receive data
 *END*-----------------------------------------------------------------*/
usb_status usb_khci_recv(usb_host_handle handle, pipe_struct_t* pipe_ptr, tr_struct_t* tr_ptr)
{
    usb_status status = USB_OK;
    usb_khci_host_state_struct_t* usb_host_ptr = (usb_khci_host_state_struct_t*) handle;
    tr_msg_struct_t msg;

#if USBCFG_KHCI_4BYTE_ALIGN_FIX
    if ((tr_ptr->rx_length & USB_DMA_ALIGN_MASK) || ((uint32_t)tr_ptr->rx_buffer & USB_DMA_ALIGN_MASK))
    {
        if (_usb_khci_swap_buf_ptr == NULL)
        {
            return USBERR_LACK_OF_SWAP_BUFFER;
        }
        if (pipe_ptr->max_packet_size > USBCFG_HOST_KHCI_SWAP_BUF_MAX)
        {
            return USBERR_LEAK_OF_SWAP_BUFFER;
        }
    }
#endif

    msg.type = TR_MSG_RECV;
    msg.msg_state = TR_MSG_IDLE;
    msg.pipe_desc = pipe_ptr;
    msg.pipe_tr = tr_ptr;
    msg.pipe_tr->transfered_length = 0;
    if (pipe_ptr->nak_count == 0)
    {
        msg.naktimeout = TIMEOUT_DEFAULT;
    }
    else
    {
        msg.naktimeout = pipe_ptr->nak_count *NAK_RETRY_TIME;
    }
    msg.retry = RETRY_TIME;
    msg.frame = _usb_khci_get_total_frame_count(usb_host_ptr);
    KHCI_DEBUG_LOG('i', msg.type, msg.pipe_desc->endpoint_number, msg.pipe_tr->rx_length)
    if (pipe_ptr->pipetype ==USB_ISOCHRONOUS_PIPE )
    {
        if (0 != OS_MsgQ_send(usb_host_ptr->tr_iso_que, (void *)&msg, 0))
        {
            status = USBERR_TR_FAILED;
        }
        OS_Event_set(usb_host_ptr->khci_event_ptr, KHCI_EVENT_ISO_MSG);
    }
    else
    {
        if (0 != OS_MsgQ_send(usb_host_ptr->tr_que, (void *)&msg, 0))
        {
            status = USBERR_TR_FAILED;
        }
        OS_Event_set(usb_host_ptr->khci_event_ptr, KHCI_EVENT_MSG);
    }
    return status;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : usb_khci_cancel
 *  Returned Value : error or USB_OK
 *  Comments       :
 *        The function to cancel the transfer
 *END*-----------------------------------------------------------------*/
usb_status usb_khci_cancel_pipe(usb_host_handle handle, pipe_struct_t* pipe_ptr, tr_struct_t* tr_ptr)
{
    usb_khci_host_state_struct_t* usb_host_ptr = (usb_khci_host_state_struct_t*) handle;
    tr_msg_struct_t msg;
    uint32_t required = 0;
    uint32_t i;
    tr_int_que_itm_struct_t* tr = usb_host_ptr->tr_int_que;
    tr_struct_t* tr_temp;

    while (OS_MsgQ_recv(usb_host_ptr->tr_iso_que, (void *) &msg, 0, 0) == 0)
    {
        if ((msg.type != TR_MSG_UNKNOWN) && (pipe_ptr == (pipe_struct_t*)(msg.pipe_desc)))
        {
            tr_temp = (tr_struct_t*)msg.pipe_tr;
            if(msg.type ==TR_MSG_RECV )
            {
                required = curr_msg.pipe_tr->rx_length;
            }
            else if (msg.type ==TR_MSG_SEND )
            {
                required = curr_msg.pipe_tr->tx_length;
            }
            _usb_khci_process_tr_complete(msg.pipe_desc, msg.pipe_tr, required, 0, required);
            /* got a valid msg */
            if (tr_temp->callback != NULL)
            {
                tr_temp->callback((void*)tr_temp, tr_temp->callback_param, NULL, 0, USBERR_TR_CANCEL);
            }
        }
        else
        {
            /* this one should be put into the tr_que_bak and put back to tr_que later */
            if(OS_MSGQ_OK != OS_MsgQ_send(usb_host_ptr->tr_que_bak, (void *)&msg, 0))
            {
                USB_PRINTF("some error on host tr_que_bak\n");
            }
        }
    }

    while (OS_MsgQ_recv(usb_host_ptr->tr_que_bak, (void *) &msg, 0, 0) == 0)
    {
        if (OS_MSGQ_OK != OS_MsgQ_send(usb_host_ptr->tr_iso_que, (void *)&msg, 0))
        {
            USB_PRINTF("some error on host tr_que\n");
        }
    }

    while (OS_MsgQ_recv(usb_host_ptr->tr_que, (void *) &msg, 0, 0) == 0)
    {
        if ((msg.type != TR_MSG_UNKNOWN) && (pipe_ptr == (pipe_struct_t*)(msg.pipe_desc)))
        {
            tr_temp = (tr_struct_t*)msg.pipe_tr;
            /* got a valid msg */
            if (tr_temp->callback != NULL)
            {
                tr_temp->callback((void*)tr_temp, tr_temp->callback_param, NULL, 0, USBERR_TR_CANCEL);
            }
        }
        else
        {
            /* this one should be put into the tr_que_bak and put back to tr_que later */
            if (OS_MSGQ_OK != OS_MsgQ_send(usb_host_ptr->tr_que_bak, (void *)&msg, 0))
            {
                USB_PRINTF("some error on host tr_que_bak\n");
            }
        }
    }

    while (OS_MsgQ_recv(usb_host_ptr->tr_que_bak, (void *) &msg, 0, 0) == 0)
    {
        if (OS_MSGQ_OK != OS_MsgQ_send(usb_host_ptr->tr_que, (void *)&msg, 0))
        {
            USB_PRINTF("some error on host tr_que\n");
        }
    }

    /* we also need to check the interrupt queue */
    for (i = 0; i < USBCFG_HOST_KHCI_MAX_INT_TR; i++)
    {
        if ((tr->msg.type != TR_MSG_UNKNOWN) && ((pipe_struct_t*)(tr->msg.pipe_desc) == pipe_ptr))
        {
            /* got a valid msg */
            tr_temp = (tr_struct_t*)tr->msg.pipe_tr;
            if (tr_temp->callback != NULL)
            {
                tr_temp->callback((void*)tr_temp, tr_temp->callback_param, NULL, 0, USBERR_TR_CANCEL);
            }
            tr->msg.type = TR_MSG_UNKNOWN;
        }
        tr++;
    }
    tr = usb_host_ptr->tr_nak_que;
    /* we also need to check the interrupt queue */
    for (i = 0; i < USBCFG_HOST_KHCI_MAX_INT_TR; i++)
    {
        if ((tr->msg.type != TR_MSG_UNKNOWN) && ((pipe_struct_t*)(tr->msg.pipe_desc) == pipe_ptr))
        {
            /* got a valid msg */
            tr_temp = (tr_struct_t*)tr->msg.pipe_tr;
            if (tr_temp->callback != NULL)
            {
                tr_temp->callback((void*)tr_temp, tr_temp->callback_param, NULL, 0, USBERR_TR_CANCEL);
            }
            tr->msg.type = TR_MSG_UNKNOWN;
        }
        tr++;
    }
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : usb_khci_bus_control
 *  Returned Value : error or USB_OK
 *  Comments       :
 *        The function for USB bus control
 *END*-----------------------------------------------------------------*/
usb_status usb_khci_bus_control(usb_host_handle handle, uint8_t bus_control)
{

    ptr_usb_khci_host_state_struct_t usb_host_ptr = (usb_khci_host_state_struct_t*) handle;
    if(bus_control == 1)
    {
        usb_hal_khci_start_bus_reset(usb_host_ptr->usbRegBase);
        OS_Time_delay(30);        //wait for 30 milliseconds (2.5 is minimum for reset, 10 recommended)
        usb_hal_khci_stop_bus_reset(usb_host_ptr->usbRegBase);
        usb_hal_khci_set_oddrst(usb_host_ptr->usbRegBase);

        usb_hal_khci_set_host_mode(usb_host_ptr->usbRegBase);

        usb_host_ptr->tx_bd = 0;
        usb_host_ptr->rx_bd = 0;
    }
    else if(bus_control == 2)
    {
        usb_host_ptr->device_attached = 0;

        usb_hal_khci_set_host_mode(usb_host_ptr->usbRegBase);
        usb_hal_khci_clr_all_interrupts(usb_host_ptr->usbRegBase);
        /* Now, enable only USB interrupt attach for host mode */
        usb_hal_khci_enable_interrupts(usb_host_ptr->usbRegBase, INTR_ATTACH);
    }

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : usb_khci_get_frame_number
 *  Returned Value : frame number
 *  Comments       :
 *        The function to get frame number
 *END*-----------------------------------------------------------------*/
uint32_t usb_khci_get_frame_number(usb_host_handle handle)
{
    usb_khci_host_state_struct_t* usb_host_ptr = (usb_khci_host_state_struct_t*) handle;
    return usb_hal_khci_get_frame_number(usb_host_ptr->usbRegBase);
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_process_tr_complete
 *  Returned Value : none
 *  Comments       :
 *        Transaction complete
 *END*-----------------------------------------------------------------*/
static void _usb_khci_process_tr_complete(
pipe_struct_t* pipe_desc_ptr,
tr_struct_t* pipe_tr_ptr,
uint32_t required,
uint32_t remaining,
int32_t err
)
{
    uint8_t * buffer_ptr = NULL;
    uint32_t status = 0;

    KHCI_DEBUG_LOG('o', TR_MSG_UNKNOWN, pipe_desc_ptr->endpoint_number, err)

    if (err == KHCI_ATOM_TR_STALL)
    {
        status = USBERR_ENDPOINT_STALLED;
    }
    else if ((err == KHCI_ATOM_TR_NAK) || (err >= 0))
    {
        status = USB_OK;

        if (err == KHCI_ATOM_TR_NAK)
        {
            status = USBERR_TR_FAILED;
        }

        if (pipe_desc_ptr->pipetype == USB_CONTROL_PIPE)
        {
            if (pipe_tr_ptr->send_phase)
            {
                buffer_ptr = pipe_tr_ptr->tx_buffer;
                pipe_tr_ptr->send_phase = FALSE;
            }
            else
            {
                buffer_ptr = pipe_tr_ptr->rx_buffer;
            }
        }
        else
        {
            if (pipe_desc_ptr->direction)
            {
                buffer_ptr = pipe_tr_ptr->tx_buffer;
            }
            else
            {
                buffer_ptr = pipe_tr_ptr->rx_buffer;
            }
        }
    }
    else if (err < 0)
    {
        status = USBERR_TR_FAILED;
    }

    pipe_tr_ptr->status = USB_STATUS_IDLE;

    if (pipe_tr_ptr->status == USB_STATUS_IDLE)
    {
        /* Transfer done. Call the callback function for this 
         ** transaction if there is one (usually true). 
         */
        if (_usb_host_unlink_tr(pipe_desc_ptr, pipe_tr_ptr) != USB_OK)
        {
#ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("_usb_host_recv_data transfer queue failed");
#endif
        }
        if (pipe_tr_ptr->callback != NULL)
        {
            /* To ensure that the USB DMA transfer will work on a buffer that is not cached,
             ** we invalidate buffer cache lines.
             */
            pipe_tr_ptr->callback((void*)pipe_tr_ptr, pipe_tr_ptr->callback_param,
            buffer_ptr, required - remaining, status);

        }
    }

}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_tr_done
 *  Returned Value : none
 *  Comments       :
 *        Transaction complete
 *END*-----------------------------------------------------------------*/
static int32_t _usb_khci_tr_done(
usb_khci_host_state_struct_t* usb_host_ptr,
tr_msg_struct_t msg
)
{
    uint32_t bd;
    int32_t res = 0;
    uint32_t type = 0;
    uint32_t *bd_ptr = NULL;
    pipe_struct_t* pipe_desc_ptr = msg.pipe_desc;

    switch (msg.type)
    {
        case TR_MSG_SETUP:
            if (msg.pipe_tr->setup_status == 0)
            {
                type = TR_CTRL;
            }
            else if (( msg.pipe_tr->setup_status ==1) )
            {
                if (msg.pipe_tr->rx_length)
                {
                    type = TR_IN;
                }
                else if (msg.pipe_tr->tx_length)
                {
                    type = TR_OUT;
                }
            }
            else if (msg.pipe_tr->setup_status ==2)
            {
                if (msg.pipe_tr->rx_length)
                {
                    type = TR_OUT;
                }
                else if (msg.pipe_tr->tx_length)
                {
                    type = TR_IN;
                }
                else
                {
                    type = TR_IN;
                }
            }
            else if (msg.pipe_tr->setup_status ==3)
            {
                type = TR_IN;
            }
            break;

        case TR_MSG_RECV:
            type = TR_IN;
            break;

        case TR_MSG_SEND:
            type = TR_OUT;
            break;
        default:
            return KHCI_ATOM_TR_INVALID;
    }

    switch (type)
    {
        case TR_CTRL:
            usb_host_ptr->tx_bd ^= 1;
            //USB_PRINTF("len %d %d\n", len, pipe_desc_ptr->max_packet_size);
            bd_ptr = (uint32_t*) BD_PTR(0, 1, usb_host_ptr->tx_bd);
            usb_host_ptr->tx_bd ^= 1;
            break;
        case TR_IN:
            usb_host_ptr->rx_bd ^= 1;
            bd_ptr = (uint32_t*) BD_PTR(0, 0, usb_host_ptr->rx_bd);
            usb_host_ptr->rx_bd ^= 1;
            break;
        case TR_OUT:
            usb_host_ptr->tx_bd ^= 1;
            bd_ptr = (uint32_t*) BD_PTR(0, 1, usb_host_ptr->tx_bd);
            usb_host_ptr->tx_bd ^= 1;
            break;                
        default:
            bd_ptr = NULL;
            break;
    }

    bd = USB_LONG_LE_TO_HOST(*bd_ptr);
    //USB_PRINTF("bd 0x%x 0x%x\n", bd, usb_ptr->ERRSTAT);
    if (usb_hal_khci_is_error_happend(usb_host_ptr->usbRegBase,
    ERROR_PIDERR |
#if defined(KHCICFG_BASIC_SCHEDULING)
    ERROR_CRC5EOF |
#endif
    ERROR_CRC16 |
    ERROR_DFN8 |
    ERROR_DMAERR |
    ERROR_BTSERR
    ))
    {
#if defined(KHCICFG_BASIC_SCHEDULING)
        if (usb_hal_khci_is_error_happend(usb_host_ptr->usbRegBase, ERROR_CRC5EOF))
        {
            //retry = 0;
        }
#endif
        res = -(int32_t)usb_hal_khci_get_error_interrupt_status(usb_host_ptr->usbRegBase);
        return res;
    }
    else
    {
        if (bd & USB_BD_OWN)
        {
            USB_PRINTF("2 BIG ERROR 0x%x\n", (unsigned int)bd);
            *bd_ptr = 0;
            USB_PRINTF("2 after change 0x%x\n", (unsigned int)USB_LONG_LE_TO_HOST(*bd_ptr));
        }
        if((pipe_desc_ptr->pipetype == USB_ISOCHRONOUS_PIPE))
        {
            res = (bd >> 16) & 0x3ff;
        }
        else
        {
            switch (bd >> 2 & 0xf)
            {
                case 0x03:  // DATA0
                case 0x0b:  // DATA1
                case 0x02:  // ACK
                    //retry = 0;
                    res = (bd >> 16) & 0x3ff;
                    pipe_desc_ptr->nextdata01 ^= 1;     // switch data toggle
                    break;

                case 0x0e:  // STALL
                    res = KHCI_ATOM_TR_STALL;
                    //retry = 0;
                    break;

                case 0x0a:  // NAK
                    res = KHCI_ATOM_TR_NAK;     
                    break;

                case 0x00:  // bus timeout  
                    {
                       //retry = 0;
                       res = KHCI_ATOM_TR_BUS_TIMEOUT;
                    }
                    break;

                case 0x0f:  // data error
                //if this event happens during enumeration, then return means not finished enumeration
                res = KHCI_ATOM_TR_DATA_ERROR;
                break;
                default:
                break;
            }
        }
    }

#if USBCFG_KHCI_4BYTE_ALIGN_FIX
    if ((TR_IN == type) && (FALSE == s_xfer_sts.is_dma_align))
    {
        s_xfer_sts.is_dma_align = TRUE;
        if (res > 0)
        {
            OS_Mem_copy(s_xfer_sts.rx_buf, s_xfer_sts.rx_buf_orig, res);
        }
    }
#endif
    return res;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_atom_tr
 *  Returned Value : 
 *  Comments       :
 *        Atomic transaction
 *END*-----------------------------------------------------------------*/
static int32_t _usb_khci_atom_noblocking_tr(
usb_khci_host_state_struct_t* usb_host_ptr,
uint32_t type,
pipe_struct_t* pipe_desc_ptr,
uint8_t *buf_ptr,
uint32_t len
)
{
    //USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->DEV_PTR;
    //uint32_t bd;
    uint32_t *bd_ptr = NULL;
    //uint32_t *bd_ptr_tmp = NULL;
    uint8_t *buf = buf_ptr;
    //uint8_t *buf_ptr_temp = NULL;
    int32_t res;
    //int32_t delay_const = 10;
    //uint32_t event_value = (uint32_t)0;
    uint8_t speed;
    uint8_t address;
    uint8_t level;
    uint8_t counter = 0;
#if (USB_NONBLOCKING_MODE == 0)
    uint32_t ret;
#endif

    usb_host_ptr->last_to_pipe = NULL;  // at the beginning, consider that there was not timeout
    len = (len > pipe_desc_ptr->max_packet_size) ? pipe_desc_ptr->max_packet_size : len;
    level = usb_host_dev_mng_get_level(pipe_desc_ptr->dev_instance);
    speed = usb_host_dev_mng_get_speed(pipe_desc_ptr->dev_instance);
    address = usb_host_dev_mng_get_address(pipe_desc_ptr->dev_instance);

    if(speed == 1)
    {
        usb_hal_khci_enable_low_speed_support(usb_host_ptr->usbRegBase);
    }
    else
    {
        usb_hal_khci_disable_low_speed_support(usb_host_ptr->usbRegBase);
    }
    usb_hal_khci_set_device_addr(usb_host_ptr->usbRegBase, USB_ADDR_ADDR(address));

    usb_hal_khci_endpoint0_init(usb_host_ptr->usbRegBase, level, (pipe_desc_ptr->pipetype == USB_ISOCHRONOUS_PIPE ? 1 : 0));

    res = 0;
    counter = 0;
    // wait for USB ready, but with timeout
    while (usb_hal_khci_is_token_busy(usb_host_ptr->usbRegBase))
    {
        if (OS_Event_wait(usb_host_ptr->khci_event_ptr, KHCI_EVENT_MASK, FALSE, 1) == OS_EVENT_OK)
        {
            res = KHCI_ATOM_TR_RESET;
            break;
        }
        else
        {
            counter++;
            if (counter >= 3)
            {
                res = KHCI_ATOM_TR_CRC_ERROR;
                //usb_ptr->CTL &= ~USB_CTL_TXSUSPENDTOKENBUSY_MASK;
                //USB_PRINTF("big error, USB_CTL_TXSUSPENDTOKENBUSY_MASK 0x%x, %d\n", usb_ptr->CTL, retry);
                return res;
            }
        }
    }

    if (!res)
    {

        //USB_PRINTF("%d %d\n", pipe_desc_ptr->max_packet_size, usb_ptr->SOFTHLD);
        usb_hal_khci_clr_interrupt(usb_host_ptr->usbRegBase, INTR_SOFTOK);//clear SOF
        usb_hal_khci_clr_all_error_interrupts(usb_host_ptr->usbRegBase);//clear error status

#if USBCFG_KHCI_4BYTE_ALIGN_FIX
        if ((TR_IN == type) && ((len & USB_DMA_ALIGN_MASK) || ((uint32_t)buf_ptr & USB_DMA_ALIGN_MASK)))
        {
            if ((_usb_khci_swap_buf_ptr != NULL) && (len <= USBCFG_HOST_KHCI_SWAP_BUF_MAX))
            {
                buf = (uint8_t*)USB_DMA_ALIGN((int32_t)_usb_khci_swap_buf_ptr);
                s_xfer_sts.rx_buf = buf;
                s_xfer_sts.rx_buf_orig = buf_ptr;
                s_xfer_sts.rx_len = len;
                s_xfer_sts.is_dma_align = FALSE;
            }
        }
        else
        {
            s_xfer_sts.is_dma_align = TRUE;
        }
#endif
        switch (type)
        {
        case TR_CTRL:
            //USB_PRINTF("len %d %d\n", len, pipe_desc_ptr->max_packet_size);
            bd_ptr = (uint32_t*) BD_PTR(0, 1, usb_host_ptr->tx_bd);
            *(bd_ptr + 1) = USB_HOST_TO_LE_LONG((uint32_t)buf);
            *bd_ptr = USB_HOST_TO_LE_LONG(USB_BD_BC(len) | USB_BD_OWN);
            usb_hal_khci_set_target_token(usb_host_ptr->usbRegBase, USB_TOKEN_TOKENPID_SETUP, (uint8_t)pipe_desc_ptr->endpoint_number);
            //   USB_PRINTF("2 CTRL 0x%x 0x%x 0x%x %X %x\n", *bd_ptr, *(bd_ptr+1), usb_ptr->TOKEN, usb_host_ptr->tx_bd,bd_ptr);
            usb_host_ptr->tx_bd ^= 1;
            break;
        case TR_IN:
            //bd_ptr_tmp = (uint32_t*) BD_PTR(0, 0, 0);
            bd_ptr = (uint32_t*) BD_PTR(0, 0, usb_host_ptr->rx_bd);
            *(bd_ptr + 1) = USB_HOST_TO_LE_LONG((uint32_t)buf);
            *bd_ptr = USB_HOST_TO_LE_LONG(USB_BD_BC(len) | USB_BD_OWN | USB_BD_DATA01(pipe_desc_ptr->nextdata01));
            usb_hal_khci_set_target_token(usb_host_ptr->usbRegBase, USB_TOKEN_TOKENPID_IN, (uint8_t)pipe_desc_ptr->endpoint_number);
            //USB_PRINTF("B 0x%x 0x%x %d\n", *bd_ptr_tmp,*(bd_ptr_tmp+2),usb_host_ptr->rx_bd);
            //    USB_PRINTF("IN 0x%x 0x%x 0x%x %d %x\n", *bd_ptr, *(bd_ptr+1), usb_ptr->TOKEN, usb_host_ptr->rx_bd,bd_ptr);
            //USB_PRINTF("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", buf, *buf, *(buf+1),*(buf+2), *(buf+3),*(buf+4), *(buf+5),*(buf+6), *(buf+7));
            usb_host_ptr->rx_bd ^= 1;
            break;
        case TR_OUT:
            bd_ptr = (uint32_t*) BD_PTR(0, 1, usb_host_ptr->tx_bd);
            *(bd_ptr + 1) = USB_HOST_TO_LE_LONG((uint32_t)buf);
            *bd_ptr = USB_HOST_TO_LE_LONG(USB_BD_BC(len) | USB_BD_OWN | USB_BD_DATA01(pipe_desc_ptr->nextdata01));
            usb_hal_khci_set_target_token(usb_host_ptr->usbRegBase, USB_TOKEN_TOKENPID_OUT, (uint8_t)pipe_desc_ptr->endpoint_number);
            //      USB_PRINTF("OUT 0x%x 0x%x 0x%x %d %x\n", *bd_ptr, *(bd_ptr+1), usb_ptr->TOKEN, usb_host_ptr->tx_bd,bd_ptr);
            usb_host_ptr->tx_bd ^= 1;
            break;
        default:
            bd_ptr = NULL;
            break;
        }
        //     USB_PRINTF(" %x\n" ,bd_ptr);
#if (USB_NONBLOCKING_MODE == 0)       
        ret = OS_Event_wait(usb_host_ptr->khci_event_ptr, KHCI_EVENT_TOK_DONE, FALSE, USBCFG_HOST_KHCI_WAIT_TICK);
        if ((uint32_t)OS_EVENT_TIMEOUT == ret)
        {
            res = KHCI_ATOM_TR_TO;
            usb_host_ptr->last_to_pipe = pipe_desc_ptr;  // remember this pipe as it had timeout last time
        }
#endif
    }
    return res;
}

#endif

