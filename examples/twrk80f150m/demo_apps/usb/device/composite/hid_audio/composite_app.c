/**HEADER********************************************************************
 * 
 * Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
 * $FileName: composite.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 * @brief  The file implement composite driver.
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "audio_generator.h"
#include "mouse.h"
#include "usb_class_composite.h"
#include "composite_app.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_port_hal.h"

#include <stdio.h>
#include <stdlib.h>
#endif


/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
#define MAIN_TASK       10

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void TestApp_Init(void);
extern void Main_Task(uint32_t param);

/****************************************************************************
 * Global Variables
 ****************************************************************************/              
composite_device_struct_t                 g_composite_device;
uint16_t                                  g_composite_speed;
extern usb_desc_request_notify_struct_t   desc_callback;

extern void Main_Task(uint32_t param);

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Local Variables 
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
extern void Audio_USB_App_Device_Callback(uint8_t event_type, void* val, void* arg);
extern void Hid_USB_App_Device_Callback(uint8_t event_type, void* val, void* arg);
extern void Hid_TestApp_Init(void);
extern void Audio_TestApp_Init(void);
extern uint8_t Hid_USB_App_Class_Callback(uint8_t request, uint16_t value,
    uint8_t ** data, uint32_t* size, void* arg);
extern uint8_t Audio_USB_App_Class_Callback(uint8_t request, uint16_t value,
    uint8_t ** data, uint32_t* size, void* arg);

/*****************************************************************************
 *  
 *   @name        TestApp_Init
 * 
 *   @brief       This function is the entry for Audio generator
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/
void APP_init(void)
{
    class_config_struct_t* audio_config_callback_handle;
    class_config_struct_t* hid_mouse_config_callback_handle;

    USB_PRINTF("Hid_audio_TestApp_Init\n");

    /* audio device */
    audio_config_callback_handle = &g_composite_device.composite_device_config_list[AUDIO_INTERFACE_INDEX];
    audio_config_callback_handle->composite_application_callback.callback = Audio_USB_App_Device_Callback;
    audio_config_callback_handle->composite_application_callback.arg = &g_composite_device.audio_handle;
    audio_config_callback_handle->class_specific_callback.callback = Audio_USB_App_Class_Callback;
    audio_config_callback_handle->class_specific_callback.arg = &g_composite_device.audio_handle;
    audio_config_callback_handle->board_init_callback.callback = usb_device_board_init;
    audio_config_callback_handle->board_init_callback.arg = CONTROLLER_ID;
    audio_config_callback_handle->desc_callback_ptr = &desc_callback;
    audio_config_callback_handle->type = USB_CLASS_AUDIO;

    /* hid mouse device */
    hid_mouse_config_callback_handle = &g_composite_device.composite_device_config_list[HID_MOUSE_INTERFACE_INDEX];
    hid_mouse_config_callback_handle->composite_application_callback.callback = Hid_USB_App_Device_Callback;
    hid_mouse_config_callback_handle->composite_application_callback.arg = &g_composite_device.hid_mouse;
    hid_mouse_config_callback_handle->class_specific_callback.callback = Hid_USB_App_Class_Callback;
    hid_mouse_config_callback_handle->class_specific_callback.arg = &g_composite_device.hid_mouse;
    hid_mouse_config_callback_handle->board_init_callback.callback = usb_device_board_init;
    hid_mouse_config_callback_handle->board_init_callback.arg = CONTROLLER_ID;
    hid_mouse_config_callback_handle->desc_callback_ptr = &desc_callback;
    hid_mouse_config_callback_handle->type = USB_CLASS_HID;
    OS_Mem_zero(&g_composite_device.hid_mouse, sizeof(hid_mouse_struct_t));

    g_composite_device.composite_device_config_callback.count = 2;
    g_composite_device.composite_device_config_callback.class_app_callback = g_composite_device.composite_device_config_list;

    /* Initialize the USB interface */
    USB_Composite_Init(CONTROLLER_ID, &g_composite_device.composite_device_config_callback, &g_composite_device.composite_device);

    g_composite_device.audio_handle = (audio_handle_t) g_composite_device.composite_device_config_list[AUDIO_INTERFACE_INDEX].class_handle;
    g_composite_device.hid_mouse.app_handle = (hid_handle_t) g_composite_device.composite_device_config_list[HID_MOUSE_INTERFACE_INDEX].class_handle;

    hid_mouse_init(&g_composite_device.hid_mouse);
    audio_init(&g_composite_device.audio_handle);
}

void APP_task(void)
{

}


#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    {   1L, Main_Task, 2500L, MQX_MAIN_TASK_PRIORITY, "Main", MQX_AUTO_START_TASK},
    {   0L, 0L, 0L, 0L, 0L, 0L}
};
#endif

#if (USE_RTOS)
static void Task_Start(void *arg)
{
    APP_init();
}
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
    hardware_init();
    OSA_Init();
    dbg_uart_init();

#if !(USE_RTOS)    
    APP_init();
#endif
    APP_task();

#if (USE_RTOS)
    OS_Task_create(Task_Start, NULL, 4L, 3000L, "task_start", NULL);
#endif
    OSA_Start();
#if (!defined(FSL_RTOS_MQX))&(defined(__CC_ARM) || defined(__GNUC__))
    return 1;
#endif
}
#endif

/* EOF */
