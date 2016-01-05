/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
*   This file contains device driver for mass storage class. This code tests
*   the FAT functionalities.
*
*END************************************************************************/

/**************************************************************************
Include the OS and BSP dependent files that define IO functions and
basic types. You may like to change these files for your board and RTOS 
**************************************************************************/
/**************************************************************************
Include the USB stack header files.
**************************************************************************/
#include <usb_host_config.h>
#include <usb.h>
#include <usb_host_stack_interface.h>
#include <mqx.h>
#include <lwevent.h>
#include <bsp.h>
#include <mfs.h>
#include <part_mgr.h>
#include <mfs_usb.h>
#if USBCFG_HOST_HUB
#include <usb_host_hub_sm.h>
#endif
#include "usb_file.h"

 /* USB device states */
#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACE_OPENED       (4)
#define  USB_DEVICE_SETTING_PROTOCOL       (5)
#define  USB_DEVICE_INUSE                  (6)
#define  USB_DEVICE_DETACHED               (7)
#define  USB_DEVICE_OTHER                  (8)
#define  USB_DEVICE_INTERFACE_CLOSED       (9)

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#define DEBUG_UART_BAUD (115200)
#endif

#if HIGH_SPEED
#define CONTROLLER_ID                      USB_CONTROLLER_EHCI_0
#else
#define CONTROLLER_ID                      USB_CONTROLLER_KHCI_0
#endif

void   APP_task(void);
uint32_t   usb_host_mass_device_event (usb_device_instance_handle,usb_interface_descriptor_handle,uint32_t);

const usb_host_driver_info_t DriverInfoTable[] = 
{
   /* Floppy drive */
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_MASS_STORAGE,       /* Class code                       */
      USB_SUBCLASS_MASS_UFI,        /* Sub-Class code                   */
      USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
      0,                            /* Reserved                         */
      (event_callback)usb_host_mass_device_event    /* Application call back function   */
   },

   /* USB 2.0 hard drive */
   {

      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_MASS_STORAGE,       /* Class code                       */
      USB_SUBCLASS_MASS_SCSI,       /* Sub-Class code                   */
      USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
      0,                            /* Reserved                         */
      (event_callback)usb_host_mass_device_event    /* Application call back function   */
   },

#if USBCFG_HOST_HUB
   /* USB 1.1 hub */
   {

      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_HUB,                /* Class code                       */
      USB_SUBCLASS_HUB_NONE,        /* Sub-Class code                   */
      USB_PROTOCOL_HUB_ALL,         /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_hub_device_event     /* Application call back function   */
   },
#endif

   {
      {0x00,0x00},                  /* All-zero entry terminates        */
      {0x00,0x00},                  /*    driver info list.             */
      0,
      0,
      0,
      0,
      NULL
   }
};

/**************************************************************************
   Global variables
**************************************************************************/
usb_host_handle                  g_host_handle;         /* global handle for calling host   */
mfsmsd_device_t                  g_mass_device[USBCFG_MAX_INSTANCE] = {0};
usb_device_interface_struct_t*   g_interface_info[USBCFG_MAX_INSTANCE][USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];
uint8_t                          g_interface_number[USBCFG_MAX_INSTANCE] = {0};
uint8_t                          g_mass_device_new_index = 0;


extern int mfs_unmount(uint8_t device_no);
extern int mfs_mount(uint8_t device_no);

#define SHELL_TASK_STACK_SIZE 4000
#define USB_TASK_STACK_SIZE   2500

#define MAIN_TASK             (10)
#define SHELL_TASK            (11)

usb_interface_descriptor_handle mass_get_interface(uint8_t num)
{
    return (usb_interface_descriptor_handle)(g_interface_info[num][0]);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : APP_init
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/
void APP_init(void) 
{  
    usb_status status;
   
    status = usb_host_init(CONTROLLER_ID, usb_host_board_init, &g_host_handle);
    if(status != USB_OK) 
    {
        printf("\r\nUSB Host Initialization failed! STATUS: 0x%x", status);
        return;
    }
    /*
     ** since we are going to act as the host driver, register the driver
     ** information for wanted class/subclass/protocols
     */
    status = usb_host_register_driver_info(g_host_handle, (void *)DriverInfoTable);
    if(status != USB_OK) 
    {         
        printf("\r\nUSB Initialization driver info failed! STATUS: 0x%x", status);
        return;
    }
    
    printf("\r\nUSB file system test\r\nWaiting for USB mass storage to be attached...\r\n");
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : APP_task
* Returned Value : none
* Comments       :
*     
*
*END*--------------------------------------------------------------------*/

struct msd_dev_data_struct *dev_data;

void APP_task ( void )
{ 
    /* Body */
    static uint32_t             fs_mountp = 0;
    usb_status           status = USB_OK;
    static uint8_t       fat_task_flag[USBCFG_MAX_INSTANCE] = {0};
    uint8_t              i = 0;

    /*----------------------------------------------------**
    ** Infinite loop, waiting for events requiring action **
    **----------------------------------------------------*/
    for(i = 0; i < USBCFG_MAX_INSTANCE; i++)
    {
        mfsmsd_device_t *mass_device = &g_mass_device[i];
	switch (mass_device->dev_state) 
	{
	    case USB_DEVICE_IDLE:
	        break;
            case USB_DEVICE_ATTACHED:
                printf( "Mass Storage Device Attached\r\n" );
                dev_data = _mem_alloc_zero(sizeof(MSD_DEV_DATA_STRUCT));
                if (NULL == dev_data)
                {
                    puts("\r\nCould not allocate memory for MSD instance in application");
                    break;
                }
                dev_data->ccs = mass_device;
                mass_device->dev_state = USB_DEVICE_SET_INTERFACE_STARTED;
                status = usb_host_open_dev_interface(g_host_handle, mass_device->dev_handle, mass_device->intf_handle, (usb_class_handle *)&mass_device->class_handle);
                if (status != USB_OK)
                {
                    printf("\r\nError in _usb_hostdev_open_interface: %x\r\n", status);
                    return;
                } /* Endif */
                /* Can run fat task */
                fat_task_flag[i] = 1;    
                break;
            case USB_DEVICE_SET_INTERFACE_STARTED:
                break;
            case USB_DEVICE_INTERFACE_OPENED:
                if (1 == fat_task_flag[i])
                {
                    fat_task_flag[i] = 0;
                    g_mass_device_new_index = i;
                    usb_msd_install(dev_data, &fs_mountp);
                }
                /* Disable flag to run FAT task */
                break;
            case USB_DEVICE_DETACHED:
                printf ( "\r\nMass Storage Device Detached\r\n" );

                /* Force uninstall msd when device was detached */
                usb_msd_uninstall(dev_data, &fs_mountp, TRUE);
                _mem_free(dev_data);

                status = usb_host_close_dev_interface(g_host_handle, g_mass_device[i].dev_handle, g_mass_device[i].intf_handle, g_mass_device[i].class_handle);
                if (status != USB_OK)
                {
                    printf("error in _usb_hostdev_close_interface %x\r\n", status);
                }
                g_mass_device[i].dev_handle = NULL;
                g_mass_device[i].intf_handle = NULL;
                g_mass_device[i].class_handle = NULL;
                printf("Going to idle state\r\n");
                g_mass_device[i].dev_state = USB_DEVICE_IDLE;

                break;
            case USB_DEVICE_OTHER:
                break;
            default:
                printf ( "Unknown Mass Storage Device State = %d\r\n", g_mass_device[i].dev_state );
                break;
        } /* Endswitch */
    }
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_device_event
* Returned Value : USB status
* Comments       : Called when mass storage device has been attached, detached, etc.
*
*END*--------------------------------------------------------------------*/
uint32_t usb_host_mass_device_event
   (
      /* [IN] pointer to device instance */
      usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint32_t           event_code
   )
{ /* Body */
   usb_device_interface_struct_t*      pHostIntf = (usb_device_interface_struct_t*)intf_handle;
   interface_descriptor_t*             intf_ptr = pHostIntf->lpinterfaceDesc;
   volatile mfsmsd_device_t*           mass_device_ptr = NULL;
   uint8_t                             i = 0;
   
   for(i = 0; i < USBCFG_MAX_INSTANCE;i++)
   {
	   if(g_mass_device[i].dev_handle == dev_handle)
	   {
		   mass_device_ptr = &g_mass_device[i];
		   break;
	   }
   }
   
   if(NULL == mass_device_ptr)
   {
	   for(i = 0; i < USBCFG_MAX_INSTANCE;i++)
	   {
		   if(USB_DEVICE_IDLE == g_mass_device[i].dev_state)
		   {
			   mass_device_ptr = &g_mass_device[i];
			   break;
		   }
	   }
   }
   
   if(NULL == mass_device_ptr)
   {
	   printf("Access devices is full.\r\n");
	   return USBERR_BAD_STATUS;
   }
   
   switch (event_code) 
   {
      case USB_ATTACH_EVENT:
         g_interface_info[i][g_interface_number[i]] = pHostIntf;
         g_interface_number[i]++;
         printf("----- Attach Event -----\r\n");
         printf("State = %d", mass_device_ptr->dev_state);
         printf("  Interface Number = %d", intf_ptr->bInterfaceNumber);
         printf("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
         printf("  Class = %d", intf_ptr->bInterfaceClass);
         printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
         printf("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
         break;
         /* Drop through into attach, same processing */
         
      case USB_CONFIG_EVENT:
         if (mass_device_ptr->dev_state == USB_DEVICE_IDLE) 
         {
        	 mass_device_ptr->dev_handle = dev_handle;
        	 mass_device_ptr->intf_handle = mass_get_interface(i);
        	 mass_device_ptr->dev_state = USB_DEVICE_ATTACHED;
         } 
         else 
         {
            printf("Mass Storage Device is already attached - DEV_STATE = %d\r\n", mass_device_ptr->dev_state);
         } /* EndIf */
         break;
          
      case USB_INTF_OPENED_EVENT:
         printf("----- Interface opened Event -----\r\n");
         mass_device_ptr->dev_state = USB_DEVICE_INTERFACE_OPENED;
         break ;
         
      case USB_DETACH_EVENT:
         /* Use only the interface with desired protocol */
         printf("----- Detach Event -----\r\n");
         printf("State = %d", mass_device_ptr->dev_state);
         printf("  Interface Number = %d", intf_ptr->bInterfaceNumber);
         printf("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
         printf("  Class = %d", intf_ptr->bInterfaceClass);
         printf("  SubClass = %d", intf_ptr->bInterfaceSubClass);
         printf("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
         g_interface_number[i] = 0;
         mass_device_ptr->dev_state = USB_DEVICE_DETACHED;
         break;
      default:
    	 printf("Mass Storage Device state = %d??\r\n", mass_device_ptr->dev_state);
    	 mass_device_ptr->dev_state = USB_DEVICE_IDLE;
         break;
   } /* EndSwitch */
   return USB_OK;
} /* Endbody */
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)// && (!defined (FSL_RTOS_MQX))

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
extern void Shell_Task(uint32_t temp);

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { 1L,     Main_Task,      1024L,  MQX_MAIN_TASK_PRIORITY, "Main",      MQX_AUTO_START_TASK},
   { 2L,     Shell_Task,      2048L,  MQX_MAIN_TASK_PRIORITY, "Shell",      MQX_AUTO_START_TASK},
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
        _time_delay(500);
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
    OSA_Init();


#if !(USE_RTOS)
    APP_init();
#endif

    OS_Task_create(Task_Start, NULL, 9L, 4000L, "task_start", NULL);
    OSA_Start();
#if !defined(FSL_RTOS_MQX)
    return 1;
#endif
}
#endif

/* EOF */
