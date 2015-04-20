/**HEADER********************************************************************
*
* Copyright (c) 2004 -2010, 2013- 2014 Freescale Semiconductor;
* All Rights Reserved
*
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
* $FileName:keyboard.c
* $Version :
* $Date    :
*
* Comments:
* 
*
*END************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "usb_class_hid.h"
#include "keyboard.h"   /* Keyboard Application Header File */
#include "usb_descriptor.h"

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_port_hal.h"

#include <stdio.h>
#include <stdlib.h>
#endif
/* skip the inclusion in dependency statge */
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include <stdlib.h>
#include <string.h>
#endif

#if USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined zero in usb_device_config.h. Please recompile usbd with this option.
#endif

extern void Main_Task(uint32_t param);
#define MAIN_TASK       10
   
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, Main_Task, 2000L, 7L, "Main", MQX_AUTO_START_TASK, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L , 0, 0}
};
#endif

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
//void TestApp_Init(void);
/****************************************************************************
 * Global Variables 
 ****************************************************************************/
/* Add all the variables needed for mouse.c to this structure */
extern usb_desc_request_notify_struct_t     g_desc_callback;
keyboard_global_variable_struct_t           g_keyboard;
uint32_t                                    g_process_times = 1;
uint8_t                                     g_key_index = 2;
uint8_t                                     g_new_key_pressed = 0;


/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Device_Callback(uint8_t event_type, void* val,void* arg); 
uint8_t USB_App_Class_Callback(uint8_t request, uint16_t value, uint8_t ** data, 
    uint32_t* size,void* arg); 

/*****************************************************************************
 * Local Variables
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
/******************************************************************************
 * @name:         KeyBoard_Events_Process
 *
 * @brief:        This function gets the input from keyboard, the keyboard
 *                does not include the code to filter the glitch on keys since
 *                it is just for demo
 *
 * @param:        None
 *
 * @return:       None
 *
 *****************************************************************************
 * This function sends the keyboard data depending on which key is pressed on
 * the board
 *****************************************************************************/
void KeyBoard_Events_Process(void)
{
    static int32_t x = 0;  
    static enum { UP, DOWN} dir = UP;

    switch (dir)
    {
        case UP:
            g_keyboard.rpt_buf[g_key_index] = KEY_PAGEUP;
            x++;
            if (x > 100)
            {
                dir = DOWN;
            }
            break;
        case DOWN:
            g_keyboard.rpt_buf[g_key_index] = KEY_PAGEDOWN;
            x--;
            if (x < 0)
            {
                dir = UP;
            }
            break;
    }
    (void)USB_Class_HID_Send_Data(g_keyboard.app_handle, HID_ENDPOINT, g_keyboard.rpt_buf, KEYBOARD_BUFF_SIZE);

    return;
}

/******************************************************************************
 *
 *    @name        USB_App_Device_Callback
 *
 *    @brief       This function handles the callback  
 *
 *    @param       handle : handle to Identify the controller
 *    @param       event_type       : value of the event
 *    @param       val              : gives the configuration value
 *
 *    @return      None
 *
 *****************************************************************************/
void USB_App_Device_Callback(uint8_t event_type, void* val,void* arg) 
{    
    UNUSED_ARGUMENT (arg)
    UNUSED_ARGUMENT (val)
    
    switch(event_type)
{
        case USB_DEV_EVENT_BUS_RESET:     
            g_keyboard.keyboard_init = FALSE;
            break;
        case USB_DEV_EVENT_ENUM_COMPLETE:    
            g_keyboard.keyboard_init = TRUE;    
            g_process_times = 1;
            KeyBoard_Events_Process();/* run the coursor movement code */
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
 *    @name        USB_App_Class_Callback
 *
 *    @brief       This function handles USB-HID Class callback
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
 *****************************************************************************
 * This function is called whenever a HID class request is received. This
 * function handles these class requests.
 *****************************************************************************/
uint8_t USB_App_Class_Callback
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
        if((g_keyboard.keyboard_init)&& (arg != NULL))
        {
                #if COMPLIANCE_TESTING
                    uint32_t g_compliance_delay = 0x009FFFFF;
                    while(g_compliance_delay--);
                #endif

                KeyBoard_Events_Process();/* run the coursor movement code */
        }
        return error;
    }

    /* index == 0 for get/set idle, index == 1 for get/set protocol */        
    *size =0;
    /* handle the class request */
    switch (request)
    {
        case USB_HID_GET_REPORT_REQUEST :
            *data = &g_keyboard.rpt_buf[0]; /* point to the report to send */
            *size = KEYBOARD_BUFF_SIZE; /* report size */
            break;

        case USB_HID_SET_REPORT_REQUEST :
            for (index = 0; index < KEYBOARD_BUFF_SIZE ; index++) 
            {   /* copy the report sent by the host */
    //            g_keyboard.rpt_buf[index] = *(*data + index);
            }
            break;

        case USB_HID_GET_IDLE_REQUEST :
            /* point to the current idle rate */
            *data = &g_keyboard.app_request_params[index];
            *size = REQ_DATA_SIZE;
            break;

        case USB_HID_SET_IDLE_REQUEST :
            /* set the idle rate sent by the host */
            g_keyboard.app_request_params[index] =(uint8_t)((value & MSB_MASK) >> 
                                                      HIGH_BYTE_SHIFT);
            break;

        case USB_HID_GET_PROTOCOL_REQUEST :
            /* point to the current protocol code
               0 = Boot Protocol
               1 = Report Protocol*/
            *data = &g_keyboard.app_request_params[index];
            *size = REQ_DATA_SIZE;
            break;

        case USB_HID_SET_PROTOCOL_REQUEST :
            /* set the protocol sent by the host
                 0 = Boot Protocol
                 1 = Report Protocol*/
               g_keyboard.app_request_params[index] = (uint8_t)(value);  
            break;
    }
    return error; 
}

/******************************************************************************
 *
 *   @name        APP_init
 *
 *   @brief       This function is the entry for Keyboard Application
 *
 *   @param       None
 *
 *   @return      None
 *
 *****************************************************************************
 * This function starts the keyboard application
 *****************************************************************************/
void APP_init(void)
{
    hid_config_struct_t config_struct;
    OS_Mem_zero(&g_keyboard,sizeof(keyboard_global_variable_struct_t));
    OS_Mem_zero(&config_struct,sizeof(hid_config_struct_t));

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    g_keyboard.rpt_buf = (uint8_t*)OS_Mem_alloc_uncached_align(KEYBOARD_BUFF_SIZE, 32);
    if(NULL == g_keyboard.rpt_buf)
    {
        USB_PRINTF("\nMalloc error in APP_init\n");
        return;
    }
    OS_Mem_zero(g_keyboard.rpt_buf,KEYBOARD_BUFF_SIZE);
#endif
    USB_PRINTF("\nbegin to test keyboard\n");
    config_struct.hid_application_callback.callback = USB_App_Device_Callback;
    config_struct.hid_application_callback.arg = &g_keyboard.app_handle;
    config_struct.class_specific_callback.callback = USB_App_Class_Callback;
    config_struct.class_specific_callback.arg = &g_keyboard.app_handle;
    config_struct.desc_callback_ptr = &g_desc_callback;

    USB_Class_HID_Init(CONTROLLER_ID, &config_struct, &g_keyboard.app_handle);
}

/******************************************************************************
 *
 *   @name        APP_task
 *
 *   @brief
 *
 *   @param       None
 *
 *   @return      None
 *
 *****************************************************************************
 * This function starts the keyboard application
 *****************************************************************************/
void APP_task()
{
    USB_HID_Periodic_Task(); 
}

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)

/*FUNCTION*----------------------------------------------------------------
 *
* Function Name  : Main_Task
* Returned Value : None
* Comments       :
*     First function called.  Calls Test_App
*     callback functions.
 *
*END*--------------------------------------------------------------------*/
void Main_Task( uint32_t param )
{
    UNUSED_ARGUMENT (param)
    APP_init();  
}
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { 1L,     Main_Task,      2500L,  MQX_MAIN_TASK_PRIORITY, "Main",      MQX_AUTO_START_TASK},
   { 0L,     0L,             0L,    0L, 0L,          0L }
};
#endif

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param)
#else

#if defined(__CC_ARM) || defined(__GNUC__)
int main(void)
#else
void main(void)
#endif

#endif
{
    OSA_Init();
    hardware_init();
    dbg_uart_init();
    
    APP_init();

    OSA_Start();
#if (!defined(FSL_RTOS_MQX))&(defined(__CC_ARM) || defined(__GNUC__))
    return 1;
#endif
}
#endif
/* EOF */
