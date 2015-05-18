/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
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
 * $FileName: main.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file is an example of device drivers for the HID class. This example
 *   demonstrates the keyboard functionality. Note that a real keyboard driver also
 *   needs to distinguish between intentionally repeated and unrepeated key presses.
 *   This example simply demonstrates how to receive data from a USB Keyboard. 
 *   Interpretation of data is upto the application and customers can add the code
 *   for it.
 *
 *END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_hid.h"
#include "usb_host_hub_sm.h"
#include "mouse_keyboard.h"

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)

//#include "MK70F12_port.h"
//#include "fsl_usb_features.h"
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_port_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include "fsl_uart_driver.h"
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)

#if !MQX_HAS_TIME_SLICE
#error This application requires that MQX_HAS_TIME_SLICE is set to 1
#endif

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#endif
/*
 ** Globals
 */
extern usb_status usb_host_hid_keyboard_event(usb_device_instance_handle dev_handle, usb_interface_descriptor_handle intf_handle, uint32_t event_code);
extern usb_status usb_host_hid_mouse_event(usb_device_instance_handle dev_handle, usb_interface_descriptor_handle intf_handle, uint32_t event_code);

/* Table of driver capabilities this application wants to use */
static usb_host_driver_info_t DriverInfoTable[] =
{
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_HID,                /* Class code                       */
      USB_SUBCLASS_HID_BOOT,        /* Sub-Class code                   */
      USB_PROTOCOL_HID_KEYBOARD,    /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_hid_keyboard_event   /* Application call back function   */
   },
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_HID,                /* Class code                       */
      USB_SUBCLASS_HID_BOOT,        /* Sub-Class code                   */
      USB_PROTOCOL_HID_MOUSE,       /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_hid_mouse_event      /* Application call back function   */
   },
   /* USB 2.0 hub */
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

usb_host_handle g_host_handle;

extern os_event_handle mouse_usb_event;
extern hid_command_t* mouse_hid_com;
extern os_event_handle kbd_usb_event;
extern hid_command_t* kbd_hid_com;
extern void time_init(void);

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#if USE_RTOS
#define USB_KEYBOARD_TASK_ADDRESS        Keyboard_Task_Stun
#define USB_MOUSE_TASK_ADDRESS           Mouse_Task_Stun
extern void Keyboard_Task_Stun(void* param);
extern void Mouse_Task_Stun(void* param);
#else
void Keyboard_Task(void* param);
void Mouse_Task(void* param);
#define USB_KEYBOARD_TASK_ADDRESS        Keyboard_Task
#define USB_MOUSE_TASK_ADDRESS           Mouse_Task
#endif
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#define USB_KEYBOARD_TASK_ADDRESS                       Keyboard_Task_Stun
#define USB_MOUSE_TASK_ADDRESS                          Mouse_Task_Stun
#define USB_KEYBOARD_PRESS_TASK_ADDRESS                 Long_Press_Task
#define USB_KEYBOARD_PRESS_TASK_PRIORITY                7
#define MAIN_TASK          (10)
#define LONG_PRESS_TASK    (7)
//#define KEYBOARD_TASK      (11)
//#define MOUSE_TASK         (12)
extern void Main_Task(uint32_t param);
extern void Keyboard_Task_Stun(void* param);
extern void Mouse_Task_Stun(void* param);
extern void Long_Press_Task(uint32_t param);
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    { MAIN_TASK, Main_Task, 3000L, 9L, "Main", MQX_AUTO_START_TASK | MQX_TIME_SLICE_TASK },
    //{ KEYBOARD_TASK,  Keyboard_Task,  3000L,  9L, "Keyboard",  MQX_TIME_SLICE_TASK},
    //{ MOUSE_TASK,     Mouse_Task,     3000L,  9L, "Mouse",     MQX_TIME_SLICE_TASK},
        { LONG_PRESS_TASK, USB_KEYBOARD_PRESS_TASK_ADDRESS, 1000L, 7L, "press_task", MQX_AUTO_START_TASK | MQX_TIME_SLICE_TASK },
        { 0L, 0L, 0L, 0L, 0L, 0L }
};
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
void Keyboard_Task(void* param);
void Mouse_Task(void* param);
#define USB_KEYBOARD_TASK_ADDRESS        Keyboard_Task
#define USB_MOUSE_TASK_ADDRESS           Mouse_Task
#endif

#define USB_KEYBOARD_TASK_PRIORITY       9
#define USB_MOUSE_TASK_PRIORITY          9
#define USB_KEYBOARD_TASK_STACKSIZE      3000
#define USB_MOUSE_TASK_STACKSIZE         3000

#define USB_KEYBOARD_TASK_NAME           "Keyboard Task"
#define USB_MOUSE_TASK_NAME              "Mouse Task"

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_hid_unsupported_device_event
 * Returned Value : usb_status
 * Comments       :
 *     Called when unsupported device has been attached.
 *END*--------------------------------------------------------------------*/

usb_status usb_host_hid_unsupported_device_event
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,

    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,

    /* [IN] code number for event causing callback */
    uint32_t event_code
    )
{
    usb_device_interface_struct_t* pDeviceIntf;
    interface_descriptor_t* intf_ptr;
    if (USB_ATTACH_INTF_NOT_SUPPORT == event_code)
    {
        pDeviceIntf = (usb_device_interface_struct_t*) intf_handle;
        intf_ptr = pDeviceIntf->lpinterfaceDesc;
        USB_PRINTF("----- Unsupported Interface of attached Device -----\r\n");
        USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
        USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
    }
    else if (USB_ATTACH_DEVICE_NOT_SUPPORT == event_code)
    {
        USB_PRINTF("----- Unsupported Device attached -----\r\n");
    }
    
    return USB_OK;
}

void APP_init(void)
{
    usb_status status = USB_OK;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    uint32_t opt = MQX_TIME_SLICE_TASK;
#else
    uint32_t opt = 0;
#endif

    status = usb_host_init(CONTROLLER_ID, &g_host_handle);
    if (status != USB_OK)
    {
        USB_PRINTF("\r\nUSB Host Initialization failed! STATUS: 0x%x", status);
        return;
    }
    /*
     ** since we are going to act as the host driver, register the driver
     ** information for wanted class/subclass/protocols
     */
    status = usb_host_register_driver_info(g_host_handle, (void *) DriverInfoTable);
    if (status != USB_OK)
    {
        USB_PRINTF("\r\nUSB Initialization driver info failed! STATUS: 0x%x", status);
        return;
    }

    status = usb_host_register_unsupported_device_notify(g_host_handle, usb_host_hid_unsupported_device_event);
    if (status != USB_OK)
    {
        USB_PRINTF("\r\nUSB Initialization driver info failed! STATUS: 0x%x", status);
        return;
    }

    mouse_usb_event = OS_Event_create(0);/* manually clear */
    if (mouse_usb_event == NULL)
    {
        USB_PRINTF("mouse_usb_event create failed\r\n");
        return;
    }
    mouse_hid_com = (hid_command_t*) OS_Mem_alloc_zero(sizeof(hid_command_t));
    if (mouse_hid_com == NULL)
    {
        USB_PRINTF("mouse_hid_com create failed\r\n");
        return;
    }

    kbd_usb_event = OS_Event_create(0);/* manually clear */
    if (kbd_usb_event == NULL)
    {
        USB_PRINTF("kbd_usb_event create failed\r\n");
        return;
    }
    OS_Event_set(kbd_usb_event, USB_EVEN_INIT);
    kbd_hid_com = (hid_command_t*) OS_Mem_alloc_zero(sizeof(hid_command_t));
    if (kbd_hid_com == NULL)
    {
        USB_PRINTF("kbd_hid_com create failed\r\n");
        return;
    }

    if ((uint32_t) OS_TASK_ERROR == OS_Task_create(USB_KEYBOARD_TASK_ADDRESS, (void*) g_host_handle, (uint32_t) USB_KEYBOARD_TASK_PRIORITY, USB_KEYBOARD_TASK_STACKSIZE, USB_KEYBOARD_TASK_NAME, &opt))
    {
        USB_PRINTF("keyboard task create failed\r\n");
        return;
    }

    if ((uint32_t) OS_TASK_ERROR == OS_Task_create(USB_MOUSE_TASK_ADDRESS, (void*) g_host_handle, (uint32_t) USB_MOUSE_TASK_PRIORITY, USB_MOUSE_TASK_STACKSIZE, USB_MOUSE_TASK_NAME, &opt))
    {
        USB_PRINTF("mouse task create failed\r\n");
        return;
    }
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK | OS_ADAPTER_ACTIVE_OS==OS_ADAPTER_BM)
    time_init();
#endif   
    USB_PRINTF("\fUSB HID Mouse+Keyboard\r\nWaiting for USB Mouse or Keyboard to be attached...\r\n");
}

void APP_task(void)
{
    return;
}

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : main (Main_Task if using MQX)
 * Returned Value : none
 * Comments       :
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/
void Main_Task(uint32_t param)
{
    APP_init();
    OS_Task_suspend(0);
    /*
     ** Infinite loop, waiting for events requiring action
     */
    for (;;)
    {
        APP_task();
    } /* Endfor */
} /* Endbody */
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)

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
#endif
    OSA_TaskDestroy (OSA_TaskGetHandler());
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

    OS_Task_create(Task_Start, NULL, 9L, 3000L, "task_start", NULL);
    OSA_Start();
#if !defined(FSL_RTOS_MQX)
    return 1;
#endif
}
#endif

