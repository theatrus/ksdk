/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2015 Freescale Semiconductor;
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
* $FileName: pin_detect_mouse.c$
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
#include "usb_host_config.h"
#include "usb_device_config.h"
#include "usb.h"
#include "usb_pin_detect.h"
#include "host_mouse_api.h"
#include "dev_mouse_api.h"
#include "pin_detect_mouse.h"
#include "usb_host_stack_interface.h"
#include "usb_host_hid.h"
#include "host_mouse.h"


#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include "fsl_port_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "fsl_uart_driver.h"


/*****************************************************************************
* Constant and Macro's 
*****************************************************************************/

#if  USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined zero in usb_device_config.h. Please recompile USB library with this option.
#endif

#if  !USBCFG_DEV_EHCI_PIN_DETECT_ENABLE || !USBCFG_EHCI_PIN_DETECT_ENABLE
#error This application requires USBCFG_EHCI_PIN_DETECT_ENABLE defined non-zero in usb_host_config.h and USBCFG_DEV_EHCI_PIN_DETECT_ENABLE defined non-zero in usb_device_config.h. Please recompile usbd with this option.
#endif

#define MAIN_TASK                       (10)
/*****************************************************************************
* Local Types 
*****************************************************************************/

/*****************************************************************************
* Global Functions Prototypes
*****************************************************************************/

extern uint8_t usb_pin_detector_get_id_status( uint8_t controller_id);
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && (defined (FSL_RTOS_MQX)))
void            Main_Task(uint32_t param);
#endif
/****************************************************************************
* Global Variables
****************************************************************************/
os_event_handle         g_pin_detector_app_event_handle;

/*****************************************************************************
* Local Functions Prototypes
*****************************************************************************/


/*****************************************************************************
* Local Variables 
*****************************************************************************/


/*****************************************************************************
* Local Functions
*****************************************************************************/
/************************************************************************************
**
** Globals
************************************************************************************/

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : APP_task 
* Returned Value : none
* Comments       :
*    
*
*END*--------------------------------------------------------------------*/
void APP_task()
{
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)&& USE_RTOS)
    OS_Event_wait(g_pin_detector_app_event_handle,ID_CHANGE_EVENT_MARK, FALSE, 10);
#endif  

    if(OS_Event_check_bit(g_pin_detector_app_event_handle, ID_CHANGE_EVENT_MARK)) 
    {
      OS_Event_clear(g_pin_detector_app_event_handle,ID_CHANGE_EVENT_MARK);  
      if(usb_pin_detector_get_id_status(USB_CONTROLLER_EHCI_0))
      { 
        HOST_APP_uninit();
        DEV_APP_init();
      }
      else
      {
        DEV_APP_uninit();
        HOST_APP_init(); 
      }
    }
}

static void App_PinDetector_Event(uint32_t event)
{
    if((event == USB_HOST_ID_CHANGE) || (event == USB_DEVICE_ID_CHANGE))
        OS_Event_set(g_pin_detector_app_event_handle,ID_CHANGE_EVENT_MARK);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : OTG_App_Init
* Returned Value : none
* Comments       :
*
*
*END*--------------------------------------------------------------------*/
void APP_init()
{
    /* Initialize the current platform. Call for the _bsp_platform_init which is specific to each processor family */
    g_pin_detector_app_event_handle = OS_Event_create(0);
    USB_PRINTF("\n\r pin detector  is initilalizing:");
 
    /*call the Init and unint device app  to open phy clock to access the otgsc register */
    HOST_APP_init();
    HOST_APP_uninit();

    usb_device_register_pin_detect_service(USB_CONTROLLER_EHCI_0, App_PinDetector_Event, NULL);
    usb_host_register_pin_detect_service(USB_CONTROLLER_EHCI_0, App_PinDetector_Event, NULL);
    OS_Event_set(g_pin_detector_app_event_handle,ID_CHANGE_EVENT_MARK);

}

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { 1L,     Main_Task,      3000L,  MQX_MAIN_TASK_PRIORITY, "Main",      MQX_AUTO_START_TASK},
   { 0L,     0L,             0L,    0L, 0L,          0L }
};
#endif

static void Task_Start(void *arg)
{
#if (USE_RTOS)
    APP_init();

    for ( ; ; ) {
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

    OS_Task_create(Task_Start, NULL, 10L, 3000L, "task_start", NULL);
    OSA_Start();
#if !defined(FSL_RTOS_MQX)
    return 1;
#endif
}


