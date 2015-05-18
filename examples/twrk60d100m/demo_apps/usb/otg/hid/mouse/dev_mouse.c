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
 * $FileName: dev_mouse.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *         
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb.h"

#include "usb_device_stack_interface.h"
#include "dev_mouse.h"
#include "dev_mouse_api.h"
#include "otg_mouse.h"
/*****************************************************************************
 * Constant and Macro's 
 *****************************************************************************/
uint32_t g_dev_app_task_id = 0;
/* KHCI task parameters */
#define USB_DEV_HID_TASK_TEMPLATE_INDEX       0
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)||((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)&& USE_RTOS))  
/* USB stack running on OS */
#define USB_DEV_HID_TASK_ADDRESS              DEV_APP_task_stun
/* USB stack running on BM  */
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)||(OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM))
#define USB_DEV_HID_TASK_ADDRESS              DEV_APP_task
#endif
#define USB_DEV_HID_TASK_STACKSIZE            1600
#define USB_DEV_HID_TASK_PRIORITY             (11)
#define USB_DEV_HID_TASK_NAME                 "HID Device Task"
#define USB_DEV_HID_TASK_ATTRIBUTES           0
#define USB_DEV_HID_TASK_DEFAULT_TIME_SLICE   0
#define USB_DEV_HID_ISR_EVENT_MASK            (~(uint32_t)0)
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/
/* Add all the variables needed for mouse.c to this structure */
extern usb_endpoints_t g_usb_desc_ep;
extern usb_desc_request_notify_struct_t g_desc_callback;
mouse_global_variable_struct_t g_mouse;
extern os_event_handle g_otg_app_event_handle;
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val, void* arg);
uint8_t USB_App_Param_Callback(uint8_t request, uint16_t value, uint8_t ** data, uint32_t* size, void* arg);
void DEV_APP_task_stun(uint32_t param);
void DEV_APP_task(void);
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/

/*****************************************************************************
 * 
 *      @name     move_mouse
 *
 *      @brief    This function gets makes the cursor on screen move left,right
 *                up and down
 *
 *      @param    None      
 *
 *      @return   None
 *     
 * 
 ******************************************************************************/
void move_mouse(void)
{
    static int32_t x = 0;
    static int32_t y = 0;
    enum { RIGHT, DOWN, LEFT, UP };
    static uint8_t dir = (uint8_t) RIGHT;

    switch(dir)
    {
    case RIGHT:
        g_mouse.rpt_buf[1] = 2;
        g_mouse.rpt_buf[2] = 0;
        x++;
        if (x > 100)
        {
            dir++;
        }
        break;
    case DOWN:
        g_mouse.rpt_buf[1] = 0;
        g_mouse.rpt_buf[2] = 2;
        y++;
        if (y > 100)
        {
            dir++;
        }
        break;
    case LEFT:
        g_mouse.rpt_buf[1] = (uint8_t)(-2);
        g_mouse.rpt_buf[2] = 0;
        x--;
        if (x < 0)
        {
            dir++;
        }
        break;
    case UP:
        g_mouse.rpt_buf[1] = 0;
        g_mouse.rpt_buf[2] = (uint8_t)(-2);
        y--;
        if (y < 0)
        {
            dir = RIGHT;
        }
        break;
    }
    (void) USB_Class_HID_Send_Data(g_mouse.app_handle, HID_ENDPOINT,
        g_mouse.rpt_buf, MOUSE_BUFF_SIZE);

}

/******************************************************************************
 * 
 *    @name        USB_App_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       handle : handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void USB_App_Callback(uint8_t event_type, void* val, void* arg)
{
    UNUSED_ARGUMENT (arg)
    UNUSED_ARGUMENT (val)

    switch(event_type)
    {
    case USB_DEV_EVENT_BUS_RESET:
        g_mouse.mouse_init = FALSE;
        break;
    case USB_DEV_EVENT_ENUM_COMPLETE:
        g_mouse.mouse_init = TRUE;
        move_mouse();/* run the cursor movement code */
        break;
    case USB_DEV_EVENT_SEND_COMPLETE:
        /*check whether enumeration is complete or not */
        if ((g_mouse.mouse_init) && (arg != NULL))
        {
#if COMPLIANCE_TESTING             
            volatile uint32_t g_compliance_delay = 0x009FFFFFF;
            while (g_compliance_delay--);
#endif
            move_mouse();/* run the cursor movement code */
        }
        break;
    case USB_DEV_EVENT_ERROR:
        /* user may add code here for error handling 
         NOTE : val has the value of error from h/w*/
        break;
    default:
        break;
    }
    return;
}

/******************************************************************************
 * 
 *    @name        USB_App_Param_Callback
 *    
 *    @brief       This function handles the callback for Get/Set report req  
 *                  
 *    @param       request  :  request type
 *    @param       value    :  give report type and id
 *    @param       data     :  pointer to the data 
 *    @param       size     :  size of the transfer
 *
 *    @return      status
 *                  USB_OK  :  if successful
 *                  else return error
 *
 *****************************************************************************/
uint8_t USB_App_Param_Callback
(
    uint8_t request,
    uint16_t value,
    uint8_t ** data,
    uint32_t* size,
    void* arg
    )
{
    uint8_t error = USB_OK;
    uint8_t index = (uint8_t)((request - 2) & USB_HID_REQUEST_TYPE_MASK);

    if ((request == USB_DEV_EVENT_SEND_COMPLETE) && (value == USB_REQ_VAL_INVALID))
    {
        if ((g_mouse.mouse_init) && (arg != NULL))
        {
#if COMPLIANCE_TESTING                  
            volatile uint32_t g_compliance_delay = 0x0009FFFF;
            while (g_compliance_delay--);
#endif
            move_mouse();/* run the cursor movement code */
        }
        return error;
    }
    /* index == 0 for get/set idle, index == 1 for get/set protocol */
    *size = 0;
    /* handle the class request */
    switch(request)
    {
    case USB_HID_GET_REPORT_REQUEST:
        *data = &g_mouse.rpt_buf[0]; /* point to the report to send */
        *size = MOUSE_BUFF_SIZE; /* report size */
        break;
    case USB_HID_SET_REPORT_REQUEST:
        for (index = 0; index < MOUSE_BUFF_SIZE; index++)
        { /* copy the report sent by the host */
            g_mouse.rpt_buf[index] = *(*data + index);
        }
        break;
    case USB_HID_GET_IDLE_REQUEST:
        /* point to the current idle rate */
        *data = &g_mouse.app_request_params[index];
        *size = REQ_DATA_SIZE;
        break;
    case USB_HID_SET_IDLE_REQUEST:
        /* set the idle rate sent by the host */
        g_mouse.app_request_params[index] = (uint8_t)((value & MSB_MASK) >> HIGH_BYTE_SHIFT);
        break;
    case USB_HID_GET_PROTOCOL_REQUEST:
        /* point to the current protocol code 
         0 = Boot Protocol
         1 = Report Protocol*/
        *data = &g_mouse.app_request_params[index];
        *size = REQ_DATA_SIZE;
        break;
    case USB_HID_SET_PROTOCOL_REQUEST:
        /* set the protocol sent by the host 
         0 = Boot Protocol
         1 = Report Protocol*/
        g_mouse.app_request_params[index] = (uint8_t)(value);
        break;
    }
    return error;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : DEV_APP_load
 * Returned Value :
 * Comments       :
 * 
 *
 *END*--------------------------------------------------------------------*/
usb_status DEV_APP_load(void)
{
    OS_Event_set(g_otg_app_event_handle, OTG_LOAD_DEVICE);
    return USB_OK;
}
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : DEV_APP_unload
 * Returned Value :
 * Comments       :
 * 
 *
 *END*--------------------------------------------------------------------*/
usb_status DEV_APP_unload(void)
{
    OS_Event_set(g_otg_app_event_handle, OTG_UNLOAD_DEVICE);
    return USB_OK;
}
/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : App_Device_Task_Create
 *  Returned Value : error or USB_OK
 *  Comments       :
 *        Create HID HOST app task
 *END*-----------------------------------------------------------------*/
static uint32_t App_Dev_Task_Create(usb_device_handle device_handle)
{

    int32_t task_id = 0;

    task_id = OS_Task_create((task_start_t) USB_DEV_HID_TASK_ADDRESS, (void*) device_handle, (uint32_t)USB_DEV_HID_TASK_PRIORITY, USB_DEV_HID_TASK_STACKSIZE, USB_DEV_HID_TASK_NAME, NULL);
    if (task_id == OS_TASK_ERROR)
    {
        return 0;
    }
    return task_id;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : DEV_APP_init
 * Returned Value :
 * Comments       :
 *
 *
 *END*--------------------------------------------------------------------*/
usb_status DEV_APP_init(void)
{
    hid_config_struct_t config_struct;
    usb_status status = USB_OK;
    /* initialize the Global Variable Structure */
    OS_Mem_zero(&g_mouse, sizeof(mouse_global_variable_struct_t));
    OS_Mem_zero(&config_struct, sizeof(hid_config_struct_t));

    /* Initialize the USB interface */
    USB_PRINTF("\n\r begin to test mouse");
    config_struct.hid_application_callback.callback = USB_App_Callback;
    config_struct.hid_application_callback.arg = &g_mouse.app_handle;
    config_struct.class_specific_callback.callback = USB_App_Param_Callback;
    config_struct.class_specific_callback.arg = &g_mouse.app_handle;
    config_struct.desc_callback_ptr = &g_desc_callback;

    USB_Class_HID_Init(0x0, &config_struct, &g_mouse.app_handle);
    /* create the device app task */
    g_dev_app_task_id = App_Dev_Task_Create(&g_mouse);
    if (g_dev_app_task_id == 0)
    {
        return USBERR_UNKNOWN_ERROR;
    }
    return status;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : DEV_APP_task
 * Returned Value :
 * Comments       :
 *
 *
 *END*--------------------------------------------------------------------*/
void DEV_APP_task(void)
{
    USB_HID_Periodic_Task();
    OS_Time_delay(10);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : DEV_APP_uninit
 * Returned Value :
 * Comments       :
 *
 *
 *END*--------------------------------------------------------------------*/
void DEV_APP_uninit(void)
{
    USB_Class_HID_Deinit(g_mouse.app_handle);
    if (g_dev_app_task_id != 0)
    {
        OS_Task_delete(g_dev_app_task_id);
    }
}
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && (defined (USE_RTOS)))

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : DEV_task
 * Returned Value :
 * Comments       :
 *
 *
 *END*--------------------------------------------------------------------*/
void DEV_APP_task_stun(uint32_t param)
{
    for (;;)
    {
        DEV_APP_task();
    } /* Endfor */
} /* Endbody */
#endif
/* EOF */
