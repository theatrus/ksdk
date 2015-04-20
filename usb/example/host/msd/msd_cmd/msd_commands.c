/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: msd_commands.c$
* $Version : 3.4.21.0$
* $Date    : Sep-15-2009$
*
* Comments:
*
*   This file contains device driver for mass storage class. This code tests
*   the UFI set of commands.
*
*END************************************************************************/

/**************************************************************************
Include the OS and BSP dependent files that define IO functions and
basic types. You may like to change these files for your board and RTOS 
**************************************************************************/
/**************************************************************************
Include the USB stack header files.
**************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
#include "derivative.h"
#include "hidef.h"
#include "mem_util.h"
#endif
#include "usb_host_hub_sm.h"

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "board.h"
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include <stdio.h>
#include <stdlib.h>
#endif

#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK))
#define ticks_per_second (1000)
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#define ticks_per_second (1000)
#endif

/**************************************************************************
Local header files for this application
**************************************************************************/
#include "msd_commands.h"

/**************************************************************************
Class driver files for this application
**************************************************************************/
#include <usb_host_msd_bo.h>
#include <usb_host_msd_ufi.h>

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#endif
/**************************************************************************
A driver info table defines the devices that are supported and handled
by file system application. This table defines the PID, VID, class and
subclass of the USB device that this application listens to. If a device
that matches this table entry, USB stack will generate a attach callback.
As noted, this table defines a UFI class device and a USB
SCSI class device (e.g. high-speed hard disk) as supported devices.
see the declaration of structure USB_HOST_DRIVER_INFO for details
or consult the software architecture guide.
**************************************************************************/

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
      usb_host_mass_device_event    /* Application call back function   */
   },

   /* USB 2.0 hard drive */
   {

      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_MASS_STORAGE,       /* Class code                       */
      USB_SUBCLASS_MASS_SCSI,       /* Sub-Class code                   */
      USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_mass_device_event    /* Application call back function   */
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
device_struct_t                           g_mass_device    = { 0 };   /* mass storage device struct */
volatile bool                             g_bCallBack      = FALSE;
volatile usb_status                       g_bStatus        = USB_OK;
volatile uint32_t                         g_dBuffer_length = 0;
static volatile uint32_t                  g_mass_device_test_flag = 0;
static volatile uint8_t                   g_mass_device_open_flag = 0;

usb_host_handle                           g_host_handle;         /* global handle for calling host   */
usb_device_interface_struct_t*            g_interface_info[USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];
uint8_t                                   g_interface_number = 0;
/* the following is the mass storage class driver object structure. This is
used to send commands down to  the class driver. See the Class API document
for details */
mass_command_struct_t                     g_pCmd;

/*some handles for communicating with USB stack */
uint8_t *                                 g_buff_in;
uint8_t *                                 g_buff_out;
const uint16_t                            g_buff_in_size  = MAX_FRAME_SIZE;
const uint16_t                            g_buff_out_size = MAX_FRAME_SIZE;
mass_storage_read_capacity_cmd_struct_t   g_read_capacity;

#if TEST_SECTOR_READ_WRITE_SPEED
volatile uint64_t                         g_time_count       = 0;
uint32_t                                  g_cpu_core_clk_khz = 120000000 / ticks_per_second;
#endif

#if TEST_SECTOR_READ_WRITE_SPEED
#define EACH_TRANSFOR_SECTORS (128)
uint8_t                                   g_test_buffer[EACH_TRANSFOR_SECTORS * 512];
#endif
   
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
/**************************************************************************
The following is the way to define a multi tasking system in MQX RTOS.
Remove this code and use your own RTOS way of defining tasks (or threads).
**************************************************************************/
#define MAIN_TASK          (10)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
#if TEST_SECTOR_READ_WRITE_SPEED
   { MAIN_TASK,      Main_Task,      30000L,  9L, "Main",      MQX_AUTO_START_TASK},
#else
   { MAIN_TASK,      Main_Task,      3000L,  9L, "Main",      MQX_AUTO_START_TASK},
#endif
   { 0L,             0L,             0L,     0L, 0L,          0L }
};
#endif

usb_interface_descriptor_handle mass_get_interface()
{
    return (usb_interface_descriptor_handle)(g_interface_info[0]);
}

uint32_t mass_get_buffer()
{
    if ((g_buff_in_size != 0) && (g_buff_in == NULL))
    {
        g_buff_in = OS_Mem_alloc_uncached_zero(g_buff_in_size);
        if (g_buff_in == NULL)
        {
            USB_PRINTF("allocate memory failed in mass_get_buffer\r\n");
            return (uint32_t)-1;
        }
    }
    
    if ((g_buff_out_size != 0) && (g_buff_out == NULL))
    {
        g_buff_out = OS_Mem_alloc_uncached_zero(g_buff_out_size);
        if (g_buff_out == NULL)
        {
            OS_Mem_free(g_buff_in);
            g_buff_in = NULL;
            USB_PRINTF("allocate memory failed in mass_get_buffer\r\n");
            return (uint32_t)-1;
        }
    }
    return (uint32_t)0;
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : APP_init
* Returned Value : none
* Comments       :
*     APP init
*
*END*--------------------------------------------------------------------*/
void APP_init(void) 
{  
    usb_status              status = USB_OK;
    /* DisableInterrupts; */
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
    usb_int_dis();
#endif 

#if TEST_SECTOR_READ_WRITE_SPEED
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    extern const TCpuClockConfiguration PE_CpuClockConfigurations[];
    g_cpu_core_clk_khz = PE_CpuClockConfigurations[Cpu_GetClockConfiguration()].cpu_core_clk_hz / ticks_per_second;
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
    CLOCK_SYS_GetFreq(kCoreClock, &g_cpu_core_clk_khz);
    g_cpu_core_clk_khz /= ticks_per_second;
#endif
#endif

    status = usb_host_init(CONTROLLER_ID, &g_host_handle);
    if(status != USB_OK) 
    {
        USB_PRINTF("\r\nUSB Host Initialization failed! STATUS: 0x%x", status);
        return;
    }
    /*
     ** since we are going to act as the host driver, register the driver
     ** information for wanted class/subclass/protocols
     */
    status = usb_host_register_driver_info(g_host_handle, (void *)DriverInfoTable);
    if(status != USB_OK) 
    {         
        USB_PRINTF("\r\nUSB Initialization driver info failed! STATUS: 0x%x", status);
        return;
    }
    
    g_pCmd.CBW_PTR = (cbw_struct_t*)OS_Mem_alloc_uncached_zero(sizeof(cbw_struct_t));  
    if (g_pCmd.CBW_PTR == NULL)
    {
       USB_PRINTF ("\r\nUnable to allocate Command Block Wrapper!");     
    }
     
    g_pCmd.CSW_PTR = (csw_struct_t*)OS_Mem_alloc_uncached_zero(sizeof(csw_struct_t));   
    if (g_pCmd.CSW_PTR == NULL)
    {
       USB_PRINTF ("\r\nUnable to allocate Command Status Wrapper!");     
    }

    USB_PRINTF("\r\nUSB MSD Command test\r\nWaiting for USB mass storage to be attached...\r\n");
      
#if TEST_SECTOR_READ_WRITE_SPEED
    DEMCR |= DEMCR_TRCENA; //enable DWT module counter
#endif
    /* EnableInterrupts; */
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
    usb_int_en();
#endif
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : APP_task
* Returned Value : none
* Comments       :
*     APP execution starts here
*
*END*--------------------------------------------------------------------*/

void APP_task (void)
{ /* Body */
   usb_status        status = USB_OK;   
   uint16_t          pid = 0;
   uint16_t          vid = 0;

   /*----------------------------------------------------**
   ** Infinite loop, waiting for events requiring action **
   **----------------------------------------------------*/
   switch (g_mass_device.dev_state) 
   {
       case USB_DEVICE_IDLE:
           break;
        
       case USB_DEVICE_ATTACHED:
           USB_PRINTF( "Mass Storage Device Attached\r\n" );
           if (1 == g_mass_device_open_flag)
           {
               g_mass_device_open_flag = 0;
               status = usb_host_open_dev_interface(g_host_handle, g_mass_device.dev_handle, g_mass_device.intf_handle, (class_handle*)&g_mass_device.CLASS_HANDLE);
               if (status != USB_OK)
               {
                   USB_PRINTF("\r\nError in _usb_hostdev_open_interface: %x\r\n", status);
                   return;
               } /* Endif */
               
               if(usb_class_mass_getvidpid(g_mass_device.CLASS_HANDLE, &vid, &pid) == USB_OK)
               {
               	   USB_PRINTF("vid = 0x%04X, pid = 0x%04X\r\n", vid, pid);
               }               
           }     
           break;

       case USB_DEVICE_SET_INTERFACE_STARTED:
           break;
        
       case USB_DEVICE_INTERFACE_OPENED:
    	   mass_get_buffer();
           if(1 == g_mass_device_test_flag)
           {
               g_mass_device_test_flag = 0;
               if (g_buff_in != NULL && g_buff_out != NULL)
                   usb_host_mass_test_storage();
               else
                   USB_PRINTF("test don't run because buff malloc fail\r\n");
           }
           break;
        
       case USB_DEVICE_DETACHED:
           USB_PRINTF ( "\r\nMass Storage Device Detached\r\n" );
           status = usb_host_close_dev_interface(g_host_handle, g_mass_device.dev_handle, g_mass_device.intf_handle, g_mass_device.CLASS_HANDLE);
           if (status != USB_OK)
           {
               USB_PRINTF("error in _usb_hostdev_close_interface %x\r\n", status);
           }
           g_mass_device.intf_handle = NULL;
           g_mass_device.CLASS_HANDLE = NULL;
           USB_PRINTF("Going to idle state\r\n");
           if (g_buff_in != NULL)
           {
               OS_Mem_free(g_buff_in);
               g_buff_in = NULL;
           }
           if (g_buff_out != NULL)
           {
               OS_Mem_free(g_buff_out);
               g_buff_out = NULL;
           }
           g_mass_device.dev_state = USB_DEVICE_IDLE;
           break;
        
       case USB_DEVICE_OTHER:
           break;
        
       default:
           USB_PRINTF ( "Unknown Mass Storage Device State = %d\r\n",\
               (int)g_mass_device.dev_state );
           break;
   } /* Endswitch */
} /* Endbody */

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main (Main_Task if using MQX)
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/

void Main_Task ( uint32_t param )
{
    APP_init();
   
    /*
    ** Infinite loop, waiting for events requiring action
    */
    for ( ; ; )
    {
        APP_task();
    } /* Endfor */
} /* Endbody */
#endif

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_device_event
* Returned Value : None
* Comments       :
*     called when mass storage device has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_mass_device_event
   (
      /* [IN] void * to device instance */
      usb_device_instance_handle      dev_handle,

      /* [IN] void * to interface descriptor */
      usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint32_t           event_code
   )
{ /* Body */
    
   usb_device_interface_struct_t* pHostIntf = (usb_device_interface_struct_t*)intf_handle;
   interface_descriptor_t*        intf_ptr = pHostIntf->lpinterfaceDesc;
   
   switch (event_code) 
   {
      case USB_ATTACH_EVENT:
         g_interface_info[g_interface_number] = pHostIntf;
         g_interface_number++;
         USB_PRINTF("----- Attach Event -----\r\n");
         USB_PRINTF("State = %d", g_mass_device.dev_state);
         USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
         USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
         USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
         USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
         USB_PRINTF("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
         break;
         /* Drop through into attach, same processing */
      case USB_CONFIG_EVENT:
         if (g_mass_device.dev_state == USB_DEVICE_IDLE) 
         {
             
            g_mass_device.dev_handle = dev_handle;
            g_mass_device.intf_handle = mass_get_interface();
            g_mass_device.dev_state = USB_DEVICE_ATTACHED;
            g_mass_device_open_flag = 1;
         } 
         else 
         {
            USB_PRINTF("Mass Storage Device is already attached - DEV_STATE = %d\r\n", g_mass_device.dev_state);
         } /* EndIf */
         break;
         
      case USB_INTF_OPENED_EVENT:
         USB_PRINTF("----- Interface opened Event -----\r\n");
         g_mass_device.dev_state = USB_DEVICE_INTERFACE_OPENED;
         g_mass_device_test_flag = 1;
         break ;
         
      case USB_DETACH_EVENT:
         /* Use only the interface with desired protocol */
         USB_PRINTF("----- Detach Event -----\r\n");
         USB_PRINTF("State = %d", g_mass_device.dev_state);
         USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
         USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
         USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
         USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
         USB_PRINTF("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
         g_interface_number = 0;
         g_mass_device.dev_state = USB_DEVICE_DETACHED;
         break;
         
      default:
         USB_PRINTF("Mass Storage Device state = %d??\r\n", g_mass_device.dev_state);
         g_mass_device.dev_state = USB_DEVICE_IDLE;
         break;
   } /* EndSwitch */
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_ctrl_calback
* Returned Value : None
* Comments       :
*     called on completion of a control-pipe transaction.
*
*END*--------------------------------------------------------------------*/

static void usb_host_mass_ctrl_callback
   (
      /* [IN] void * to pipe */
      void *            tr_ptr,

      /* [IN] user-defined parameter */
      void *            user_parm,

      /* [IN] buffer address */
      uint8_t *         buffer,

      /* [IN] length of data transferred */
      uint32_t          buflen,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      uint32_t          status
   )
{ /* Body */
 
    g_bCallBack = TRUE;
    g_bStatus = status;
} /* Endbody */


void callback_bulk_pipe
   (
      /* [IN] Status of this command */
      usb_status status,

      /* [IN] void * to USB_MASS_BULK_ONLY_REQUEST_STRUCT*/
      void *     p1,

      /* [IN] void * to the command object*/
      void *     p2,

      /* [IN] Length of data transmitted */
      uint32_t   buffer_length
   )
{ /* Body */
    
   g_dBuffer_length = buffer_length;
   g_bCallBack = TRUE;
   g_bStatus = status;

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_mass_test_storage
* Returned Value : None
* Comments       :
*     Calls the UFI command set for testing
*END*--------------------------------------------------------------------*/
static void usb_host_mass_test_storage
   (
      void
   )
{ /* Body */
   static usb_status                                 status = USB_OK;
   static uint8_t                                    bLun = 0;
   static cbw_struct_t*                              cbw_ptr; 
   static csw_struct_t*                              csw_ptr;
   uint32_t                                          block_len = 0;
#if TEST_SECTOR_READ_WRITE_SPEED
   volatile uint32_t                                 i = 0;
   volatile uint32_t                                 j = 0;
   uint64_t                                          temp = 0;
   uint32_t                                          test_case[] = {0, 2048 * 100, 2048 * 100}; /* sector count */
   uint32_t                                          sector_count = 0;
   uint32_t                                          transfor_length = 0;

   test_case[0] = sizeof(test_case) / sizeof(uint32_t);
#endif

   cbw_ptr = g_pCmd.CBW_PTR;
   csw_ptr = g_pCmd.CSW_PTR;

   OS_Mem_zero(g_pCmd.CSW_PTR, sizeof(csw_struct_t));
   OS_Mem_zero(g_pCmd.CBW_PTR, sizeof(cbw_struct_t));
   OS_Mem_zero(&g_pCmd, sizeof(mass_command_struct_t));
      
   g_pCmd.CBW_PTR   = cbw_ptr;
   g_pCmd.CSW_PTR   = csw_ptr;
   g_pCmd.LUN       = bLun;
   g_pCmd.CLASS_PTR = (void *)g_mass_device.CLASS_HANDLE;
   g_pCmd.CALLBACK  = callback_bulk_pipe;

   USB_PRINTF("\r\n ================ START OF A NEW SESSION ================\r\n");
   /* Test the GET MAX LUN command */
   USB_PRINTF("\r\nTesting: GET MAX LUN Command");
      
   g_bCallBack = FALSE;
      
   status = usb_class_mass_getmaxlun_bulkonly(
      (void *)g_mass_device.CLASS_HANDLE, &bLun,
      usb_host_mass_ctrl_callback, (void *)g_mass_device.CLASS_HANDLE);
      
   if ((status != USB_OK))
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif

      }     
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }

   /* Test the TEST UNIT READY command */
   USB_PRINTF("Testing: TEST UNIT READY Command");
      

   g_bCallBack = FALSE;

   status =  usb_mass_ufi_test_unit_ready(&g_pCmd);
      
   if ((status != USB_OK))
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif
      }         
      if (!g_bStatus)
      {
         USB_PRINTF("...OK\r\n");
      }
      else
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }

   /* Test the REQUEST SENSE command */
   USB_PRINTF("Testing: REQUEST SENSE Command");
       
   g_bCallBack = FALSE;

   status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

   if ((status != USB_OK))
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus)
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }

   /* Test the INQUIRY command */
   USB_PRINTF("Testing: INQUIRY Command");    

   g_bCallBack = FALSE;

   status = usb_mass_ufi_inquiry(&g_pCmd, (uint8_t *) g_buff_in, sizeof(inquiry_data_format_struct_t));

   if ((status != USB_OK))
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }
      
   /* Test the REQUEST SENSE command */
   USB_PRINTF("Testing: REQUEST SENSE Command");
      
   g_bCallBack = FALSE;

   status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

   if ((status != USB_OK))
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }
   
   /* Test the READ CAPACITY command */
   USB_PRINTF("Testing: READ CAPACITY Command");
      
   g_bCallBack = FALSE;

   status = usb_mass_ufi_read_capacity(&g_pCmd, (uint8_t *) g_buff_in, sizeof(mass_storage_read_capacity_cmd_struct_t));

   if ((status != USB_OK))
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus)
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }
   mass_storage_read_capacity_cmd_struct_t * read_capacity_temp;
      
   read_capacity_temp = (mass_storage_read_capacity_cmd_struct_t *)g_buff_in;
   block_len = SWAP4BYTE_CONST(*((uint32_t *)(read_capacity_temp->BLENGTH)));

   /* Test the REQUEST SENSE command */
   USB_PRINTF("Testing: REQUEST SENSE Command");
      
   g_bCallBack = FALSE;

   status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

   if (status != USB_OK)
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }
#if TEST_SECTOR_READ_WRITE_SPEED
   for(j = 1; j < test_case[0]; j++)
   {
       sector_count = test_case[j];
       USB_PRINTF("Testing: Start Test READ Throughput... Test data size: %d KB(%dB) : \r\n",sector_count / 2, 512 * sector_count);
    #if TEST_SECTOR_READ1
       USB_PRINTF("Testing: Start Test READ Throughput...(Test data size: %d KB(%dB). Read one sector at a time.)\r\n", sector_count / 2, 512 * sector_count);
       g_time_count = 0;
       
       for(i = 0;i < sector_count;i++)
       {
             DWT_CYCCNT = (uint32_t)0u;
             DWT_CR    |= DWT_CR_CYCCNTENA;
             g_bCallBack = FALSE;
             status = usb_mass_ufi_read_10(&g_pCmd, (i), g_test_buffer, 512, 1);

             if (status != USB_OK)
             {
                USB_PRINTF ("\r\n...ERROR");
                return;
             }
             else
             {
                /* Wait till command comes back */
                while (!g_bCallBack)
                {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
								Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
								OSA_PollAllOtherTasks();
#endif
                }
                if (!g_bStatus) 
                {
                   /* USB_PRINTF("...OK\r\n"); */
                }
                else 
                {
                   USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
                }
             }
             g_time_count += DWT_CYCCNT;
             DWT_CR &= ~((uint32_t)DWT_CR_CYCCNTENA);
       }
       
    #if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #else
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #endif
       temp = (uint64_t)((uint64_t)512*(uint64_t)ticks_per_second*(uint64_t)sector_count);
       temp = temp / (uint64_t)g_time_count;
       USB_PRINTF("Test results: Time = %dms, Speed = %dB/s\r\n", (uint32_t)g_time_count, temp);  
    #endif
       
       /* Test the REQUEST SENSE command */
       USB_PRINTF("Testing: REQUEST SENSE Command");
          
       g_bCallBack = FALSE;

       status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

       if (status != USB_OK)
       {
          USB_PRINTF ("\r\n...ERROR");
          return;
       }
       else
       {
          /* Wait till command comes back */
          while (!g_bCallBack)
          {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
						  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
						  OSA_PollAllOtherTasks();
#endif
          }
          if (!g_bStatus) 
          {
             USB_PRINTF("...OK\r\n");
          }
          else 
          {
             USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
          }
       }

    #if TEST_SECTOR_READ2
       
       USB_PRINTF("Testing: Start Test READ Throughput...(Test data size: %d KB(%dB). Read two sectors at a time.)\r\n", sector_count / 2, 512 * sector_count);
       g_time_count = 0;
       
       for(i = 0;i < sector_count;i += 2)
       {
             DWT_CYCCNT = (uint32_t)0u;
             DWT_CR    |= DWT_CR_CYCCNTENA;
             g_bCallBack = FALSE;
             status = usb_mass_ufi_read_10(&g_pCmd, (i), g_test_buffer, 1024, 2);

             if (status != USB_OK)
             {
                USB_PRINTF ("\r\n...ERROR");
                return;
             }
             else
             {
                /* Wait till command comes back */
                while (!g_bCallBack)
                {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
								Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
								OSA_PollAllOtherTasks();
#endif
                }
                if (!g_bStatus) 
                {
                   /* USB_PRINTF("...OK\r\n"); */
                }
                else 
                {
                   /* USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus); */
                }
             }
             g_time_count += DWT_CYCCNT;
             DWT_CR &= ~((uint32_t)DWT_CR_CYCCNTENA);
       }
       
    #if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS)))
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #else
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #endif
       temp = (uint64_t)((uint64_t)512*(uint64_t)ticks_per_second*(uint64_t)sector_count);
       temp = temp / (uint64_t)g_time_count;
       USB_PRINTF("Test results: Time = %dms, Speed = %dB/s\r\n", (uint32_t)g_time_count, temp); 

    #endif

       
       /* Test the REQUEST SENSE command */
       USB_PRINTF("Testing: REQUEST SENSE Command");
          
       g_bCallBack = FALSE;

       status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

       if (status != USB_OK)
       {
          USB_PRINTF ("\r\n...ERROR");
          return;
       }
       else
       {
          /* Wait till command comes back */
          while (!g_bCallBack)
          {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
						  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
						  OSA_PollAllOtherTasks();
#endif
          }
          if (!g_bStatus) 
          {
             USB_PRINTF("...OK\r\n");
          }
          else 
          {
             USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
          }
       }
       
    #if TEST_SECTOR_READ4
       
       USB_PRINTF("Testing: Start Test READ Throughput...(Test data size: %d KB(%dB). Read four sectors at a time.)\r\n", sector_count / 2, 512 * sector_count);
       g_time_count = 0;
       
       for(i = 0;i < sector_count;i += 4)
       {
             DWT_CYCCNT = (uint32_t)0u;
             DWT_CR    |= DWT_CR_CYCCNTENA;
             g_bCallBack = FALSE;
             status = usb_mass_ufi_read_10(&g_pCmd, (i), g_test_buffer, 2048, 4);

             if (status != USB_OK)
             {
                USB_PRINTF ("\r\n...ERROR");
                return;
             }
             else
             {
                /* Wait till command comes back */
                while (!g_bCallBack)
                {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
								Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
								OSA_PollAllOtherTasks();
#endif
                }
                if (!g_bStatus)
                {
                   /* USB_PRINTF("...OK\r\n"); */
                }
                else 
                {
                   /* USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus); */
                }
             }
             g_time_count += DWT_CYCCNT;
             DWT_CR &= ~((uint32_t)DWT_CR_CYCCNTENA);
       }       
       
    #if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS)))
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #else
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #endif
       temp = (uint64_t)((uint64_t)512*(uint64_t)ticks_per_second*(uint64_t)sector_count);
       temp = temp / (uint64_t)g_time_count;
       USB_PRINTF("Test results: Time = %dms, Speed = %dB/s\r\n", (uint32_t)g_time_count, temp); 

    #endif
       
       /* Test the REQUEST SENSE command */
       USB_PRINTF("Testing: REQUEST SENSE Command");
          
       g_bCallBack = FALSE;

       status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

       if (status != USB_OK)
       {
          USB_PRINTF ("\r\n...ERROR");
          return;
       }
       else
       {
          /* Wait till command comes back */
          while (!g_bCallBack)
          {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
						  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
						  OSA_PollAllOtherTasks();
#endif
          }
          if (!g_bStatus) 
          {
             USB_PRINTF("...OK\r\n");
          }
          else 
          {
             USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
          }
       }

    #if TEST_SECTOR_READ_MULTI
       
       USB_PRINTF("Testing: Start Test READ Throughput...(Test data size: %d KB(%dB). Read n sectors at a time.)\r\n", sector_count / 2, 512 * sector_count);
       g_time_count = 0;

       for(i = 0;i < sector_count;)
       {
             DWT_CYCCNT = (uint32_t)0u;
             DWT_CR    |= DWT_CR_CYCCNTENA;
             transfor_length = (sector_count-i) > EACH_TRANSFOR_SECTORS ? EACH_TRANSFOR_SECTORS : (sector_count-i);
             i += transfor_length;
             g_bCallBack = FALSE;
             status = usb_mass_ufi_read_10(&g_pCmd, (i), g_test_buffer, (transfor_length << 9) , transfor_length);

             if (status != USB_OK)
             {
                USB_PRINTF ("\r\n...ERROR");
                return;
             }
             else
             {
                /* Wait till command comes back */
                while (!g_bCallBack)
                {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
								Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
								OSA_PollAllOtherTasks();
#endif
                }
                if (!g_bStatus)
                {
                   /* USB_PRINTF("...OK\r\n"); */
                }
                else 
                {
                   /* USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus); */
                }
             }
             g_time_count += DWT_CYCCNT;
             DWT_CR &= ~((uint32_t)DWT_CR_CYCCNTENA);
       }       

    #if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS)))
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #else
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #endif
       temp = (uint64_t)((uint64_t)512*(uint64_t)ticks_per_second*(uint64_t)sector_count);
       temp = temp / (uint64_t)g_time_count;
       temp = temp / 1000;
       USB_PRINTF("Test results: Time = %dms, Speed = %dKB/s\r\n", (uint32_t)g_time_count, temp); 

    #endif
   }
      /* Test the REQUEST SENSE command */
   USB_PRINTF("Testing: REQUEST SENSE Command");
      
   g_bCallBack = FALSE;

   status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

   if (status != USB_OK)
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }
#endif

   /* Test the READ(10) command */
   USB_PRINTF("Testing: READ(10) Command");
      
   g_bCallBack = FALSE;

   status = usb_mass_ufi_read_10(&g_pCmd, 0, g_buff_in, block_len > g_buff_in_size ? g_buff_in_size : block_len, 1);

   if (status != USB_OK)
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }
      
   /* Test the MODE SENSE command */
   USB_PRINTF("Testing: MODE SENSE Command");
      
   g_bCallBack = FALSE;

   status = usb_mass_ufi_mode_sense(&g_pCmd, (uint32_t)2, (uint32_t)0x3F, g_buff_in, (uint32_t)0x08);

   if (status != USB_OK)
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }
      
   /* Test the PREVENT ALLOW command */
   USB_PRINTF("Testing: PREVENT-ALLOW MEDIUM REMOVAL Command");
      
   g_bCallBack = FALSE;

   status = usb_mass_ufi_prevent_allow_medium_removal( &g_pCmd, 1);

   if (status != USB_OK)
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus)
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }
      
   /* Test the REQUEST SENSE command */
   USB_PRINTF("Testing: REQUEST SENSE Command");
     
   g_bCallBack = FALSE;

   status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

   if (status != USB_OK)
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }
      
   /* Test the VERIFY command */
   USB_PRINTF("Testing: VERIFY Command");
      
   g_bCallBack = FALSE;

   status = usb_mass_ufi_verify( &g_pCmd, 0x400, 1);

   if (status != USB_OK)
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }

#if TEST_SECTOR_READ_WRITE_SPEED

   OS_Time_delay(1000);

   /* Test the REQUEST SENSE command */
   USB_PRINTF("Testing: REQUEST SENSE Command");
     
   g_bCallBack = FALSE;

   status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

   if (status != USB_OK)
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
				  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
				  OSA_PollAllOtherTasks();
#endif

      }
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }
   
   for(j = 1; j < test_case[0]; j++)
   {
       sector_count = test_case[j];
       USB_PRINTF("Testing: Start Test WRITE Throughput... Test data size: %d KB(%dB).\r\n", sector_count / 2, 512 * sector_count);

    #if TEST_SECTOR_WRITE1
       USB_PRINTF("Testing: Start Test WRITE Throughput...(Test data size: %d KB(%dB). Write one sector at a time.)\r\n", sector_count / 2, 512 * sector_count);
       g_time_count = 0;
       
       for(i = 0;i < sector_count;i++)
       {
             DWT_CYCCNT = (uint32_t)0u;
             DWT_CR    |= DWT_CR_CYCCNTENA;
             g_bCallBack = FALSE;
             status = usb_mass_ufi_write_10(&g_pCmd, (i), g_test_buffer, 512, 1);

             if (status != USB_OK)
             {
                USB_PRINTF ("\r\n...ERROR");
                return;
             }
             else
             {
                /* Wait till command comes back */
                while (!g_bCallBack)
                {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
        OSA_PollAllOtherTasks();
#endif
                }
                if (!g_bStatus) 
                {
                   /* USB_PRINTF("...OK\r\n"); */
                }
                else 
                {
                   USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
                }
             }
             g_time_count += DWT_CYCCNT;
             DWT_CR &= ~((uint32_t)DWT_CR_CYCCNTENA);
       }

    #if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS)))
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #else
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #endif
       temp = (uint64_t)((uint64_t)512*(uint64_t)ticks_per_second*(uint64_t)sector_count);
       temp = temp / (uint64_t)g_time_count;
       USB_PRINTF("Test results: Time = %dms, Speed = %dB/s\r\n", (uint32_t)g_time_count, temp); 

    #endif
       
       /* Test the REQUEST SENSE command */
       USB_PRINTF("Testing: REQUEST SENSE Command");
          
       g_bCallBack = FALSE;

       status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

       if (status != USB_OK)
       {
          USB_PRINTF ("\r\n\r\n...ERROR");
          return;
       }
       else
       {
          /* Wait till command comes back */
          while (!g_bCallBack)
          {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif

          }
          if (!g_bStatus) 
          {
             USB_PRINTF("...OK\r\n");
          }
          else 
          {
             USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
          }
       }
       
    #if TEST_SECTOR_WRITE2
       
       USB_PRINTF("Testing: Start Test WRITE Throughput...(Test data size: %d KB(%dB). Write two sectors at a time.)\r\n", sector_count / 2, 512 * sector_count);
       g_time_count = 0;
       
       for(i = 0;i < sector_count;i += 2)
       {
             DWT_CYCCNT = (uint32_t)0u;
             DWT_CR    |= DWT_CR_CYCCNTENA;
             g_bCallBack = FALSE;
             status = usb_mass_ufi_write_10(&g_pCmd, (i), g_test_buffer, 1024, 2);

             if (status != USB_OK)
             {
                USB_PRINTF ("\r\n...ERROR");
                return;
             }
             else
             {
                /* Wait till command comes back */
                while (!g_bCallBack)
                {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
							Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
							OSA_PollAllOtherTasks();
#endif

                }
                if (!g_bStatus) 
                {
                   /* USB_PRINTF("...OK\r\n"); */
                }
                else 
                {
                   USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
                }
             }
             g_time_count += DWT_CYCCNT;
             DWT_CR &= ~((uint32_t)DWT_CR_CYCCNTENA);
       }

    #if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS)))
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #else
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #endif
       temp = (uint64_t)((uint64_t)512*(uint64_t)ticks_per_second*(uint64_t)sector_count);
       temp = temp / (uint64_t)g_time_count;
       USB_PRINTF("Test results: Time = %dms, Speed = %dB/s\r\n", (uint32_t)g_time_count, temp); 

    #endif
       
       /* Test the REQUEST SENSE command */
       USB_PRINTF("Testing: REQUEST SENSE Command");
          
       g_bCallBack = FALSE;

       status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

       if (status != USB_OK)
       {
          USB_PRINTF ("\r\n...ERROR");
          return;
       }
       else
       {
          /* Wait till command comes back */
          while (!g_bCallBack)
          {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif

          }
          if (!g_bStatus) 
          {
             USB_PRINTF("...OK\r\n");
          }
          else
          {
             USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
          }
       }
       
    #if TEST_SECTOR_WRITE4
       
       USB_PRINTF("Testing: Start Test WRITE Throughput...(Test data size: %d KB(%dB). Write four sectors at a time.)\r\n", sector_count / 2, 512 * sector_count);
       g_time_count = 0;

       for(i = 0;i < sector_count;i += 4)
       {
             DWT_CYCCNT = (uint32_t)0u;
             DWT_CR    |= DWT_CR_CYCCNTENA;
             g_bCallBack = FALSE;
             status = usb_mass_ufi_write_10(&g_pCmd, (i), g_test_buffer, 2048, 4);

             if (status != USB_OK)
             {
                USB_PRINTF ("\r\n...ERROR");
                return;
             }
             else
             {
                /* Wait till command comes back */
                while (!g_bCallBack)
                {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
							Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
							OSA_PollAllOtherTasks();
#endif

                }
                if (!g_bStatus) 
                {
                   /* USB_PRINTF("...OK\r\n"); */
                }
                else 
                {
                   USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
                }
             }
             g_time_count += DWT_CYCCNT;
             DWT_CR &= ~((uint32_t)DWT_CR_CYCCNTENA);
       }

    #if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS)))
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #else
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #endif
       temp = (uint64_t)((uint64_t)512*(uint64_t)ticks_per_second*(uint64_t)sector_count);
       temp = temp / (uint64_t)g_time_count;
       USB_PRINTF("Test results: Time = %dms, Speed = %dB/s\r\n", (uint32_t)g_time_count, temp); 

    #endif 

       /* Test the REQUEST SENSE command */
       USB_PRINTF("Testing: REQUEST SENSE Command");
          
       g_bCallBack = FALSE;

       status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

       if (status != USB_OK)
       {
          USB_PRINTF ("\r\n...ERROR");
          return;
       }
       else
       {
          /* Wait till command comes back */
          while (!g_bCallBack)
          {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
					  Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
					  OSA_PollAllOtherTasks();
#endif

          }
          if (!g_bStatus) 
          {
             USB_PRINTF("...OK\r\n");
          }
          else 
          {
             USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
          }
       }

    #if TEST_SECTOR_WRITE_MULTI
       
       USB_PRINTF("Testing: Start Test WRITE Throughput...(Test data size: %d KB(%dB). Write n sectors at a time.)\r\n", sector_count / 2, 512 * sector_count);
       g_time_count = 0;

       for(i = 0;i < sector_count;)
       {
             DWT_CYCCNT = (uint32_t)0u;
             DWT_CR    |= DWT_CR_CYCCNTENA;
             transfor_length = (sector_count-i) > EACH_TRANSFOR_SECTORS ? EACH_TRANSFOR_SECTORS : (sector_count-i);
             i += transfor_length;
             g_bCallBack = FALSE;
             status = usb_mass_ufi_write_10(&g_pCmd, (i), g_test_buffer, (transfor_length << 9) , transfor_length);

             if (status != USB_OK)
             {
                USB_PRINTF ("\r\n...ERROR");
                return;
             }
             else
             {
                /* Wait till command comes back */
                while (!g_bCallBack)
                {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
							Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
							OSA_PollAllOtherTasks();
#endif

                }
                if (!g_bStatus) 
                {
                   /* USB_PRINTF("...OK\r\n"); */
                }
                else 
                {
                   USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
                }
             }
             g_time_count += DWT_CYCCNT;
             DWT_CR &= ~((uint32_t)DWT_CR_CYCCNTENA);
       }

    #if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #else
       g_time_count = g_time_count / (g_cpu_core_clk_khz);
    #endif
       temp = (uint64_t)((uint64_t)512*(uint64_t)ticks_per_second*(uint64_t)sector_count);
       temp = temp / (uint64_t)g_time_count;
       temp = temp / 1000;
       USB_PRINTF("Test results: Time = %dms, Speed = %dKB/s\r\n", (uint32_t)g_time_count, temp); 

    #endif 
   }
   
   /* Test the REQUEST SENSE command */
   USB_PRINTF("Testing: REQUEST SENSE Command");
      
   g_bCallBack = FALSE;

   status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

   if (status != USB_OK)
   {
      USB_PRINTF ("\r\n\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        Poll();
    #elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
        OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n\r\n", g_bStatus);
      }
   }
#endif

   /* Test the WRITE(10) command */
   USB_PRINTF("Testing: WRITE(10) Command");
      
   g_bCallBack = FALSE;

   status = usb_mass_ufi_write_10(&g_pCmd, 8, g_buff_out, block_len > g_buff_out_size ? g_buff_out_size : block_len, 1);

   if (status != USB_OK)
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        Poll();
    #elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
        OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }

   /* Test the REQUEST SENSE command */
   USB_PRINTF("Testing: REQUEST SENSE Command");
   
   g_bCallBack = FALSE;

   status = usb_mass_ufi_request_sense(&g_pCmd, g_buff_in, sizeof(req_sense_data_format_struct_t));

   if (status != USB_OK)
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
            OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }

   /* Test the START-STOP UNIT command */
   USB_PRINTF("Testing: START-STOP UNIT Command");

   g_bCallBack = FALSE;

   status = usb_mass_ufi_start_stop(&g_pCmd, 0, 1);

   if (status != USB_OK)
   {
      USB_PRINTF ("\r\n...ERROR");
      return;
   }
   else
   {
      /* Wait till command comes back */
      while (!g_bCallBack)
      {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
            OSA_PollAllOtherTasks();
#endif
      }
      if (!g_bStatus) 
      {
         USB_PRINTF("...OK\r\n");
      }
      else 
      {
         USB_PRINTF("...Unsupported by device (g_bStatus=0x%x)\r\n", g_bStatus);
      }
   }

   USB_PRINTF("\r\nTest done!");
} /* Endbody */
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { 1L,     Main_Task,      1024L,  MQX_MAIN_TASK_PRIORITY, "Main",      MQX_AUTO_START_TASK},
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
    OSA_Init();
    hardware_init();
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
