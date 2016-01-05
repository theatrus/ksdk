/**HEADER********************************************************************
 * 
 * Copyright (c) 2010, 2013 - 2015 Freescale Semiconductor;
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
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "msd_diskio.h"

#include "usb_host_hub_sm.h"
#include "usb_host_msd_bo.h"
#include "usb_host_msd_ufi.h"

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

/* for state change */
#define  USB_STATE_CHANGE_ATTACHED         (0x01)
#define  USB_STATE_CHANGE_OPENED           (0x02)
#define  USB_STATE_CHANGE_DETACHED         (0x04)
#define  USB_STATE_CHANGE_IDLE             (0x08)


#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#define DEBUG_UART_BAUD (115200)

#if USBCFG_HOST_COMPLIANCE_TEST
extern usb_status usb_test_mode_init (usb_device_instance_handle dev_handle);
#endif

void APP_task(void);
usb_status usb_host_mass_device_event(usb_device_instance_handle dev_handle, usb_interface_descriptor_handle intf_handle, uint32_t event_code);
usb_status usb_host_test_device_event(usb_device_instance_handle dev_handle, usb_interface_descriptor_handle intf_handle, uint32_t event_code);
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
#if USBCFG_HOST_COMPLIANCE_TEST  
    {
        { 0x0a, 0x1a }, /* Vendor ID per USB-IF             */  //0x1a0a 0x0a 0x1a
        { 0x00, 0x00 }, /* Product ID per manufacturer      */ 
        0xfe, /* Class code                       */
        0xfe, /* Sub-Class code                   */
        0xfe, /* Protocol                         */
        0, /* Reserved                         */
        usb_host_test_device_event /* Application call back function   */
    },
#endif
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
        { 0x00, 0x00 }, /* All-zero entry terminates        */
        { 0x00, 0x00 }, /*    driver info list.             */
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
usb_host_handle g_host_handle; /* global handle for calling host   */
usb_device_interface_struct_t* g_interface_info[USBCFG_MAX_INSTANCE][USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];
uint8_t g_interface_number[USBCFG_MAX_INSTANCE] = { 0 };

extern int fat_demo(void);
#if THROUGHPUT_TEST_ENABLE
extern int fat_throughput_test(void);
#endif



static void update_state(void)
{
    for (uint8_t i = 0; i < USBCFG_MAX_INSTANCE; i++)
    {
        if (g_mass_device[i].state_change != 0)
        {
            if (g_mass_device[i].state_change & USB_STATE_CHANGE_ATTACHED)
            {
                if (g_mass_device[i].dev_state == USB_DEVICE_IDLE)
                {
                    g_mass_device[i].dev_state = USB_DEVICE_ATTACHED;
                }
                g_mass_device[i].state_change &= ~(USB_STATE_CHANGE_ATTACHED);
            }
            if (g_mass_device[i].state_change & USB_STATE_CHANGE_OPENED)
            {
                if (g_mass_device[i].dev_state != USB_DEVICE_DETACHED)
                {
                    g_mass_device[i].dev_state = USB_DEVICE_INTERFACE_OPENED;
                }
                g_mass_device[i].state_change &= ~(USB_STATE_CHANGE_OPENED);
            }
            if (g_mass_device[i].state_change & USB_STATE_CHANGE_DETACHED)
            {
                g_mass_device[i].dev_state = USB_DEVICE_DETACHED;
                g_mass_device[i].state_change &= ~(USB_STATE_CHANGE_DETACHED);
            }
            if (g_mass_device[i].state_change & USB_STATE_CHANGE_IDLE)
            {
                g_mass_device[i].dev_state = USB_DEVICE_IDLE;
                g_mass_device[i].state_change &= ~(USB_STATE_CHANGE_IDLE);
            }
        }
    }
}

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
    usb_status status = USB_OK;
    /* DisableInterrupts; */
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
    usb_int_dis();
#endif 

    status = usb_host_init(CONTROLLER_ID, usb_host_board_init, &g_host_handle);
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

    USB_PRINTF("\r\nUSB file system test\r\nWaiting for USB mass storage to be attached...\r\n");

    /* Enable Interrupts; */
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
    usb_int_en();
#endif
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : APP_task
 * Returned Value : none
 * Comments       :
 *     
 *
 *END*--------------------------------------------------------------------*/

void APP_task(void)
{
    /* Body */
    usb_status status = USB_OK;
    static uint8_t fat_task_flag[USBCFG_MAX_INSTANCE] = { 0 };
    uint8_t i = 0;
    
    /* update state for not app_task context */
    update_state();

    /*----------------------------------------------------**
     ** Infinite loop, waiting for events requiring action **
     **----------------------------------------------------*/
    for (i = 0; i < USBCFG_MAX_INSTANCE; i++)
    {
        switch(g_mass_device[i].dev_state)
        {
        case USB_DEVICE_IDLE:
            break;
        case USB_DEVICE_ATTACHED:
            USB_PRINTF("Mass Storage Device Attached\r\n");
            g_mass_device[i].dev_state = USB_DEVICE_SET_INTERFACE_STARTED;
            status = usb_host_open_dev_interface(g_host_handle, g_mass_device[i].dev_handle, g_mass_device[i].intf_handle, (usb_class_handle*) &g_mass_device[i].class_handle);
            if (status != USB_OK)
            {
                USB_PRINTF("\r\nError in _usb_hostdev_open_interface: %x\r\n", status);
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
                g_mass_device_new_index = i;
#if THROUGHPUT_TEST_ENABLE
                fat_throughput_test();
#else
                fat_demo();
#endif
            }
            /* Disable flag to run FAT task */
            fat_task_flag[i] = 0;
            break;
        case USB_DEVICE_DETACHED:
            USB_PRINTF("\r\nMass Storage Device Detached\r\n");

            status = usb_host_close_dev_interface(g_host_handle, g_mass_device[i].dev_handle, g_mass_device[i].intf_handle, g_mass_device[i].class_handle);
            if (status != USB_OK)
            {
                USB_PRINTF("error in _usb_hostdev_close_interface %x\r\n", status);
            }
            g_mass_device[i].intf_handle = NULL;
            g_mass_device[i].class_handle = NULL;
            USB_PRINTF("Going to idle state\r\n");
            g_mass_device[i].dev_state = USB_DEVICE_IDLE;
            break;
        case USB_DEVICE_OTHER:
            break;
        default:
            USB_PRINTF("Unknown Mass Storage Device State = %d\r\n",
                g_mass_device[i].dev_state);
            break;
        } /* Endswitch */
    }
} /* Endbody */

#if USBCFG_HOST_COMPLIANCE_TEST
usb_status usb_host_test_device_event
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,

    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,

    /* [IN] code number for event causing callback */
    uint32_t event_code
    )
{ /* Body */
    uint8_t* dev_instance_ptr = (uint8_t*) dev_handle;
    switch(event_code)
    {
    case USB_ATTACH_EVENT:
        return usb_test_mode_init(dev_handle);
        break;
        /* Drop through into attach, same processing */

    case USB_CONFIG_EVENT:
        break;

    case USB_INTF_OPENED_EVENT:
        USB_PRINTF("----- PET Interface opened Event -----\r\n");
        break;

    case USB_DETACH_EVENT:
        if (dev_instance_ptr != NULL)
        {
            OS_Mem_free(dev_instance_ptr);
            dev_instance_ptr = NULL;
        }
        USB_PRINTF("----- PET Test device Detach Event -----\r\n");
        break;
    default:
        break;
    } /* EndSwitch */

    return USB_OK;
} /* Endbody */
#endif
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_mass_device_event
 * Returned Value : usb_status
 * Comments       : Called when mass storage device has been attached, detached, etc.
 *
 *END*--------------------------------------------------------------------*/
usb_status usb_host_mass_device_event
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,

    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,

    /* [IN] code number for event causing callback */
    uint32_t event_code
    )
{ /* Body */
    usb_device_interface_struct_t* pHostIntf = (usb_device_interface_struct_t*) intf_handle;
    interface_descriptor_t* intf_ptr = pHostIntf->lpinterfaceDesc;
    volatile device_struct_t* mass_device_ptr = NULL;
    uint8_t i = 0;

    for (i = 0; i < USBCFG_MAX_INSTANCE; i++)
    {
        if (g_mass_device[i].dev_handle == dev_handle)
        {
            mass_device_ptr = &g_mass_device[i];
            break;
        }
    }

    if (NULL == mass_device_ptr)
    {
        for (i = 0; i < USBCFG_MAX_INSTANCE; i++)
        {
            if (USB_DEVICE_IDLE == g_mass_device[i].dev_state)
            {
                mass_device_ptr = &g_mass_device[i];
                break;
            }
        }
    }

    if (NULL == mass_device_ptr)
    {
        USB_PRINTF("Access devices is full.\r\n");
        return USBERR_BAD_STATUS;
    }

    switch(event_code)
    {
    case USB_ATTACH_EVENT:
        g_interface_info[i][g_interface_number[i]] = pHostIntf;
        g_interface_number[i]++;
        USB_PRINTF("----- Attach Event -----\r\n");
        USB_PRINTF("State = %d", mass_device_ptr->dev_state);
        USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
        USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
        break;
        /* Drop through into attach, same processing */

    case USB_CONFIG_EVENT:
        if (mass_device_ptr->dev_state == USB_DEVICE_IDLE)
        {
            mass_device_ptr->dev_handle = dev_handle;
            mass_device_ptr->intf_handle = mass_get_interface(i);
            mass_device_ptr->state_change |= USB_STATE_CHANGE_ATTACHED;
        }
        else
        {
            USB_PRINTF("Mass Storage Device is already attached - DEV_STATE = %d\r\n", mass_device_ptr->dev_state);
        } /* EndIf */
        break;

    case USB_INTF_OPENED_EVENT:
        USB_PRINTF("----- Interface opened Event -----\r\n");
        mass_device_ptr->state_change |= USB_STATE_CHANGE_OPENED;
        break;

    case USB_DETACH_EVENT:
        /* Use only the interface with desired protocol */
        USB_PRINTF("----- Detach Event -----\r\n");
        USB_PRINTF("State = %d", mass_device_ptr->dev_state);
        USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
        USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
        g_interface_number[i] = 0;
        mass_device_ptr->state_change |= USB_STATE_CHANGE_DETACHED;
        break;
    default:
        USB_PRINTF("Mass Storage Device state = %d??\r\n", mass_device_ptr->dev_state);
        mass_device_ptr->state_change |= USB_STATE_CHANGE_IDLE;
        break;
    } /* EndSwitch */

    return USB_OK;
} /* Endbody */
#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    {   1L, Main_Task, 1024L, MQX_MAIN_TASK_PRIORITY, "Main", MQX_AUTO_START_TASK},
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

    OS_Task_create(Task_Start, NULL, 4L, 4000L, "task_start", NULL);
    OSA_Start();
#if !defined(FSL_RTOS_MQX)
    return 1;
#endif
}
/* EOF */
