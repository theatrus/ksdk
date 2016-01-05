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
 * $FileName: hidmouse.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file is an example of device drivers for a Mouse device.
 *   It has been tested in a Dell and logitech USB 3 button wheel Mouse. The program
 *   queues transfers on Interrupt USB pipe and waits till the data comes
 *   back. It prints the data and queues another transfer on the same pipe.
 *   See the code for details.
 *
 *END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_hid.h"
#include "usb_host_hub_sm.h"
#include "hidmouse.h"


//#include "MK70F12_port.h"
//#include "fsl_usb_features.h"
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_debug_console.h"
#include <stdio.h>
#include <stdlib.h>

/************************************************************************************
 **
 ** Globals
 ************************************************************************************/
device_struct_t                 mouse_hid_device = { 0 };
os_event_handle                 mouse_usb_event = NULL;
hid_command_t*                  mouse_hid_com = NULL;
uint8_t *                       mouse_buffer = NULL;
uint8_t *                       mouse_reportDescriptor = NULL;
uint16_t                        mouse_size = 0;
uint16_t                        mouse_reportLength = 0;
volatile uint32_t               mouse_data_received = 1;
uint8_t                         mouse_interface_number = 0;
usb_device_interface_struct_t*  mouse_interface_info[USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];
usb_host_handle              host_handle = NULL;
#if 0
volatile uint32_t sleep_test = FALSE;
#endif

#define USB_EVENT_CTRL           (0x01)
#define USB_EVENT_DATA           (0x02)
#define USB_EVENT_DATA_CORRUPTED (0x04)
#define USB_EVENT_STATE_CHANGE   (0x08)

/************************************************************************************
 Table of driver capabilities this application wants to use. See Host API document for
 details on How to define a driver info table. This table defines that this driver
 supports a HID class, boot subclass and mouse protocol. 
 ************************************************************************************/
static usb_host_driver_info_t DriverInfoTable[] =
{
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_HID,                /* Class code                       */
      USB_SUBCLASS_HID_BOOT,        /* Sub-Class code                   */
      USB_PROTOCOL_HID_MOUSE,       /* Protocol                         */
      0,                            /* Reserved                         */
      usb_host_hid_mouse_event      /* Application call back function   */
   },
   /* USB 1.1 hub */
   {
      {0x00,0x00},                  /* Vendor ID per USB-IF             */
      {0x00,0x00},                  /* Product ID per manufacturer      */
      USB_CLASS_HUB,                /* Class code                       */
      USB_SUBCLASS_HUB_NONE,        /* Sub-Class code                   */
      USB_PROTOCOL_HUB_ALL,          /* Protocol                         */
      0,                            /* Reserved                         */
#if USBCFG_HOST_HUB  
      usb_host_hub_device_event     /* Application call back function   */
#endif
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

void process_mouse_buffer(uint8_t * buffer);
void usb_host_hid_mouse_ctrl_callback(void* unused, void* user_parm, uint8_t *buffer, uint32_t buflen, usb_status status);



static void update_state(void)
{
    if (mouse_hid_device.state_change != 0)
    {
        if (mouse_hid_device.state_change & USB_STATE_CHANGE_ATTACHED)
        {
            if (mouse_hid_device.dev_state == USB_DEVICE_IDLE)
            {
                mouse_hid_device.dev_state = USB_DEVICE_ATTACHED;
            }
            mouse_hid_device.state_change &= ~(USB_STATE_CHANGE_ATTACHED);
        }
        if (mouse_hid_device.state_change & USB_STATE_CHANGE_OPENED)
        {
            if (mouse_hid_device.dev_state != USB_DEVICE_DETACHED)
            {
                mouse_hid_device.dev_state = USB_DEVICE_INTERFACE_OPENED;
            }
            mouse_hid_device.state_change &= ~(USB_STATE_CHANGE_OPENED);
        }
        if (mouse_hid_device.state_change & USB_STATE_CHANGE_DETACHED)
        {
            mouse_hid_device.dev_state = USB_DEVICE_DETACHED;
            mouse_hid_device.state_change &= ~(USB_STATE_CHANGE_DETACHED);
        }
        if (mouse_hid_device.state_change & USB_STATE_CHANGE_IDLE)
        {
            mouse_hid_device.dev_state = USB_DEVICE_IDLE;
            mouse_hid_device.state_change &= ~(USB_STATE_CHANGE_IDLE);
        }
        if (mouse_hid_device.state_change & USB_STATE_CHANGE_SET_IDLE)
        {
            if (mouse_hid_device.dev_state == USB_DEVICE_SETTING_PROTOCOL)
            {
                mouse_hid_device.dev_state = USB_DEVICE_SET_IDLE;
            }
            mouse_hid_device.state_change &= ~(USB_STATE_CHANGE_SET_IDLE);
        }
        if (mouse_hid_device.state_change & USB_STATE_CHANGE_REMOVE)
        {
            mouse_hid_device.dev_state = USB_DEVICE_REMOVE;
            mouse_hid_device.state_change &= ~(USB_STATE_CHANGE_REMOVE);
        }
        if (mouse_hid_device.state_change & USB_STATE_CHANGE_DESCRIPTOR_DONE)
        {
            if ((mouse_hid_device.dev_state == USB_DEVICE_GET_REPORT_DESCRIPTOR) || (mouse_hid_device.dev_state == USB_DEVICE_SETTING_PROTOCOL))
            {
                mouse_hid_device.dev_state = USB_DEVICE_GET_REPORT_DESCRIPTOR_DONE;
            }
            mouse_hid_device.state_change &= ~(USB_STATE_CHANGE_DESCRIPTOR_DONE);
        }
        if (mouse_hid_device.state_change & USB_STATE_CHANGE_INUSE)
        {
            if (mouse_hid_device.dev_state == USB_DEVICE_SETTING_IDLE)
            {
                mouse_hid_device.dev_state = USB_DEVICE_INUSE;
            }
            mouse_hid_device.state_change &= ~(USB_STATE_CHANGE_INUSE);
        }
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : mouse_hid_get_interface
 * Returned Value : none
 * Comments       :
 *     Used to get the proper interface if there are multiple interfaces in a device are available
 *     for us.
 *     In this example, we always choose the first interface.
 *     For the customer, a proper way should be implemented as needed.
 *
 *END*--------------------------------------------------------------------*/
usb_interface_descriptor_handle mouse_hid_get_interface(void)
{
    return (usb_interface_descriptor_handle)(mouse_interface_info[0]);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : mouse_hid_get_buffer
 * Returned Value : none
 * Comments       :
 *     used to get the buffer to store the data transferred from device.
 *
 *END*--------------------------------------------------------------------*/
uint32_t mouse_hid_get_buffer(void)
{
    uint8_t ep_no;
    usb_device_ep_struct_t *lpEp;
    /* first get the max packet size from interface info */
    usb_device_interface_struct_t* lpHostIntf = (usb_device_interface_struct_t*) mouse_hid_get_interface();
    for (ep_no = 0; ep_no < lpHostIntf->ep_count; ep_no++)
    {
        lpEp = &lpHostIntf->ep[ep_no];
        if (((lpEp->lpEndpointDesc->bEndpointAddress & IN_ENDPOINT) != 0) && ((lpEp->lpEndpointDesc->bmAttributes & IRRPT_ENDPOINT) != 0))
        {
            mouse_size = (USB_SHORT_UNALIGNED_LE_TO_HOST(lpEp->lpEndpointDesc->wMaxPacketSize) & PACKET_SIZE_MASK);
            break;
        }
    }

    if ((mouse_size != 0) && (mouse_buffer == NULL))
    {
        mouse_buffer = (uint8_t *) OS_Mem_alloc_uncached_zero(mouse_size);
        if (mouse_buffer == NULL)
        {
            USB_PRINTF("allocate memory failed in hid_get_buffer\n");
            return (uint32_t) - 1;
        }
    }
    return 0;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : mouse_hid_get_report_descriptor
 * Returned Value : none
 * Comments       :
 *     Find and parse the HID descriptor, then try to get the report descriptor.
 *
 *END*--------------------------------------------------------------------*/
void mouse_hid_get_report_descriptor(void)
{
    usb_status status = USB_OK;
    usb_device_interface_struct_t* interfaceDescriptor = (usb_device_interface_struct_t*) mouse_hid_get_interface();
    descriptor_union_t ptr1, ptr2;

    ptr1.pntr = interfaceDescriptor->interfaceEx;
    ptr2.word = ptr1.word + interfaceDescriptor->interfaceExlength;

    while (ptr1.cfig->bDescriptorType != USB_DESC_TYPE_HID)
    {
        ptr1.word += ptr1.cfig->bLength;
        if (ptr1.word >= ptr2.word)
        {
            break;
        }
    }

    if (ptr1.word < ptr2.word)
    {
        hid_descriptor_struct_t* hidDescriptor = (hid_descriptor_struct_t*) ptr1.pntr;
        hid_class_descriptor_struct_t* hidClassDescriptor = (hid_class_descriptor_struct_t*) &(hidDescriptor->bclassdescriptortype);
        uint8_t index;
        mouse_reportLength = 0;
        for (index = 0; index < hidDescriptor->bnumdescriptor; index++)
        {
            hid_class_descriptor_struct_t* temp;
            temp = hidClassDescriptor + index;
            if (temp->classdescriptortype == USB_DESC_TYPE_REPORT)
            {
                mouse_reportLength = USB_SHORT_UNALIGNED_LE_TO_HOST(temp->descriptorlength);
                break;
            }
        }
        if (mouse_reportLength != 0)
        {

            if (mouse_reportDescriptor != NULL)
            {
                OS_Mem_free(mouse_reportDescriptor);
            }
            mouse_reportDescriptor = (uint8_t *) OS_Mem_alloc_uncached_zero(mouse_reportLength);
            if (mouse_reportDescriptor == NULL)
            {
                USB_PRINTF("allocate memory failed in hid_get_buffer\r\n");
                return;
            }

            mouse_hid_device.dev_state = USB_DEVICE_GET_REPORT_DESCRIPTOR;
            mouse_hid_com->class_ptr = mouse_hid_device.class_handle;
            mouse_hid_com->callback_fn = usb_host_hid_mouse_ctrl_callback;
            mouse_hid_com->callback_param = 0;

            status = usb_class_hid_get_descriptor(mouse_hid_com, (uint8_t) USB_DESC_TYPE_REPORT, mouse_reportDescriptor, mouse_reportLength);

            if (status != USB_OK)
            {
                USB_PRINTF("\r\nError in usb_class_hid_get_descriptor: %x\n", status);
            }
        }
    }
    else
    {
        USB_PRINTF("Can't find HID_DESCRIPTOR\r\n");
    }
    return;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_hid_mouse_event
 * Returned Value : usb_status
 * Comments       :
 *     Called when HID device has been attached, detached, etc.
 *END*--------------------------------------------------------------------*/

usb_status usb_host_hid_mouse_event
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,
    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,
    /* [IN] code number for event causing callback */
    uint32_t event_code
    )
{
    usb_device_interface_struct_t* pHostIntf = (usb_device_interface_struct_t*) intf_handle;
    interface_descriptor_t* intf_ptr = pHostIntf->lpinterfaceDesc;

    switch(event_code)
    {
    case USB_ATTACH_EVENT:
        mouse_interface_info[mouse_interface_number] = pHostIntf;
        mouse_interface_number++;
        USB_PRINTF("----- Attach Event -----\r\n");
        USB_PRINTF("State = %d", mouse_hid_device.dev_state);
        USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
        USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
        break;
    case USB_CONFIG_EVENT:
        if (mouse_hid_device.dev_state == USB_DEVICE_IDLE)
        {
            mouse_hid_device.dev_handle = dev_handle;
            mouse_hid_device.intf_handle = mouse_hid_get_interface();
            mouse_hid_device.state_change |= USB_STATE_CHANGE_ATTACHED;
            OS_Event_set(mouse_usb_event, USB_EVENT_STATE_CHANGE);
        }
        else
        {
            USB_PRINTF("HID device already attached - DEV_STATE = %d\r\n", mouse_hid_device.dev_state);
        }
        break;

    case USB_INTF_OPENED_EVENT:
        USB_PRINTF("----- Interfaced Event -----\r\n");
        mouse_hid_device.state_change |= USB_STATE_CHANGE_OPENED;
        OS_Event_set(mouse_usb_event, USB_EVENT_STATE_CHANGE);
        break;

    case USB_DETACH_EVENT:
        /* Use only the interface with desired protocol */
        USB_PRINTF("\r\n----- Detach Event -----\r\n");
        USB_PRINTF("State = %d", mouse_hid_device.dev_state);
        USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
        USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
        mouse_interface_number = 0;
        mouse_hid_device.state_change |= USB_STATE_CHANGE_DETACHED;
        OS_Event_set(mouse_usb_event, USB_EVENT_STATE_CHANGE);
        break;
    default:
        USB_PRINTF("HID Device state = %d??\r\n", mouse_hid_device.dev_state);
        mouse_hid_device.state_change |= USB_STATE_CHANGE_IDLE;
        OS_Event_set(mouse_usb_event, USB_EVENT_STATE_CHANGE);
        break;
    }

    /* notify application that status has changed */
    OS_Event_set(mouse_usb_event, USB_EVENT_CTRL);

    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_hid_mouse_ctrl_callback
 * Returned Value : None
 * Comments       :
 *     Called when a command is completed
 *END*--------------------------------------------------------------------*/
void usb_host_hid_mouse_ctrl_callback
(
    /* [IN] no used */
    void* unused,

    /* [IN] user-defined parameter */
    void* user_parm,

    /* [IN] buffer address */
    uint8_t * buffer,

    /* [IN] length of data transferred */
    uint32_t buflen,

    /* [IN] status, hopefully USB_OK or USB_DONE */
    usb_status status
    )
{
    if (status == USBERR_ENDPOINT_STALLED)
    {
        USB_PRINTF("\r\nHID control Request failed DEV_STATE 0x%x status 0x%x!\r\n", mouse_hid_device.dev_state, status);
        if (mouse_hid_device.stall_tries)
        {
            mouse_hid_device.stall_tries--;
        }
        else
        {
            mouse_hid_device.state_change |= USB_STATE_CHANGE_REMOVE;
            OS_Event_set(mouse_usb_event, USB_EVENT_STATE_CHANGE);
            USB_PRINTF("\nGoing to remove the device!\r\n");
            /* notify application that status has changed */
            OS_Event_set(mouse_usb_event, USB_EVENT_CTRL);
            return;
        }
    }
    else if (status == USBERR_TR_RETRY_FAILED)
    {
        mouse_hid_device.state_change |= USB_STATE_CHANGE_REMOVE;
        OS_Event_set(mouse_usb_event, USB_EVENT_STATE_CHANGE);
        mouse_hid_device.stall_tries = USBCFG_HOST_CTRL_STALL_RETRY;
        USB_PRINTF("\nGoing to remove the device!\r\n");
        /* notify application that status has changed */
        OS_Event_set(mouse_usb_event, USB_EVENT_CTRL);
        return;
    }
    else if (status)
    {
        USB_PRINTF("\nHID control Request failed DEV_STATE 0x%x status 0x%x!\r\n", mouse_hid_device.dev_state, status);
        mouse_hid_device.stall_tries = USBCFG_HOST_CTRL_STALL_RETRY;
    }
    else
    {
        mouse_hid_device.stall_tries = USBCFG_HOST_CTRL_STALL_RETRY;
    }

    if (mouse_hid_device.dev_state == USB_DEVICE_SETTING_PROTOCOL)
    {
        if (status == USBERR_ENDPOINT_STALLED)
        {
            mouse_hid_device.state_change |= USB_STATE_CHANGE_DESCRIPTOR_DONE;
            OS_Event_set(mouse_usb_event, USB_EVENT_STATE_CHANGE);
        }
        else
        {
            mouse_hid_device.state_change |= USB_STATE_CHANGE_SET_IDLE;
            OS_Event_set(mouse_usb_event, USB_EVENT_STATE_CHANGE);
            USB_PRINTF("setting protocol done\r\n");
        }
    }
    else if (mouse_hid_device.dev_state == USB_DEVICE_GET_REPORT_DESCRIPTOR)
    {
        if (status == USBERR_ENDPOINT_STALLED)
        {
            mouse_hid_device.state_change |= USB_STATE_CHANGE_OPENED;
            OS_Event_set(mouse_usb_event, USB_EVENT_STATE_CHANGE);
        }
        else
        {
            mouse_hid_device.state_change |= USB_STATE_CHANGE_DESCRIPTOR_DONE;
            OS_Event_set(mouse_usb_event, USB_EVENT_STATE_CHANGE);
            USB_PRINTF("get report descriptor done\r\n");
        }
    }
    else if (mouse_hid_device.dev_state == USB_DEVICE_SETTING_IDLE)
    {
#if 0
        if (status == USBERR_ENDPOINT_STALLED)
        {
            mouse_hid_device.dev_state = USB_DEVICE_SET_IDLE;
        }
        else
        {
            mouse_hid_device.dev_state = USB_DEVICE_INUSE;
            USB_PRINTF("setting idle done\r\n");
        }
#else
        if (status == USBERR_ENDPOINT_STALLED)
        {
            mouse_hid_device.stall_tries = USBCFG_HOST_CTRL_STALL_RETRY;
        }
        mouse_hid_device.state_change |= USB_STATE_CHANGE_INUSE;
        OS_Event_set(mouse_usb_event, USB_EVENT_STATE_CHANGE);
        USB_PRINTF("setting idle done\r\n");
#endif
    }
    /* notify application that status has changed */
    OS_Event_set(mouse_usb_event, USB_EVENT_CTRL);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_hid_mouse_recv_callback
 * Returned Value : None
 * Comments       :
 *     Called when a interrupt pipe transfer is completed.
 *
 *END*--------------------------------------------------------------------*/

void usb_host_hid_mouse_recv_callback
(
    /* [IN] not used */
    void* unused,

    /* [IN] user-defined parameter */
    void* user_parm,

    /* [IN] buffer address */
    uint8_t * buffer,

    /* [IN] length of data transferred */
    uint32_t buflen,

    /* [IN] status, hopefully USB_OK or USB_DONE */
    usb_status status
    )
{
    if (status == USB_OK)
    {
        /* notify application that data are available */
        OS_Event_set(mouse_usb_event, USB_EVENT_DATA);
    }
    else if (status == USBERR_TR_CANCEL)
    {
        /* notify application that data are available */
        USB_PRINTF("tr cancel\r\n");
        OS_Event_set(mouse_usb_event, USB_EVENT_DATA_CORRUPTED);
    }
    else
    {
        //       USB_PRINTF("tr completed 0x%x\n", status);
        OS_Event_set(mouse_usb_event, USB_EVENT_DATA_CORRUPTED);
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : process_mouse_buffer
 * Returned Value : None
 * Comments       :
 *   Process the data from mouse buffer
 *END*--------------------------------------------------------------------*/
void process_mouse_buffer(uint8_t * buffer)
{

    if (buffer[0] & 0x01)
    {
        USB_PRINTF("Left Click ");
    }
    else
    {
        USB_PRINTF("           ");
    }
    if (buffer[0] & 0x04)
    {
        USB_PRINTF("Middle Click ");
    }
    else
    {
        USB_PRINTF("            ");
    }
    if (buffer[0] & 0x02)
    {
        USB_PRINTF("Right Click ");
    }
    else
    {
        USB_PRINTF("           ");
    }

    if (buffer[1])
    {
        if (buffer[1] > 127)
        {
            USB_PRINTF("Left  ");
        }
        else
        {
            USB_PRINTF("Right ");
        }
    }
    else
    {
        USB_PRINTF("      ");
    }

    if (buffer[2])
    {
        if (buffer[2] > 127)
        {
            USB_PRINTF("UP   ");
        }
        else
        {
            USB_PRINTF("Down ");
        }
    }
    else
    {
        USB_PRINTF("     ");
    }

    if (buffer[3])
    {
        if (buffer[3] > 127)
        {
            USB_PRINTF("Wheel Down");
        }
        else
        {
            USB_PRINTF("Wheel UP  ");
        }
    }
    else
    {
        USB_PRINTF("          ");
    }

    USB_PRINTF("\r\n");
    //fflush(stdout); 

}

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

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : APP_init
 * Returned Value : None
 * Comments       :
 *      Used to init the whole example, including the host controller, event and so on
 *END*--------------------------------------------------------------------*/
void APP_init(void)
{
    usb_status status = USB_OK;

    status = usb_host_init(CONTROLLER_ID, usb_host_board_init, &host_handle);
    if (status != USB_OK)
    {
        USB_PRINTF("\r\nUSB Host Initialization failed! STATUS: 0x%x", status);
        return;
    }
    /*
     ** since we are going to act as the host driver, register the driver
     ** information for wanted class/subclass/protocols
     */
    status = usb_host_register_driver_info(host_handle, (void *) DriverInfoTable);
    if (status != USB_OK)
    {
        USB_PRINTF("\r\nUSB Initialization driver info failed! STATUS: 0x%x", status);
        return;
    }

    status = usb_host_register_unsupported_device_notify(host_handle, usb_host_hid_unsupported_device_event);
    if (status != USB_OK)
    {
        USB_PRINTF("\r\nUSB Initialization driver info failed! STATUS: 0x%x", status);
        return;
    }

    mouse_usb_event = OS_Event_create(0);/* manually clear */
    if (mouse_usb_event == NULL)
    {
        USB_PRINTF("\r\nOS_Event_create failed!\r\n");
        return;
    }

    mouse_hid_com = (hid_command_t*) OS_Mem_alloc_zero(sizeof(hid_command_t));
    if (mouse_hid_com == NULL)
    {
        USB_PRINTF("\r\nmouse_hid_com allocate failed!\r\n");
        return;
    }
    mouse_hid_device.stall_tries = USBCFG_HOST_CTRL_STALL_RETRY;

    USB_PRINTF("\fUSB HID Mouse\r\nWaiting for USB Mouse to be attached...\r\n");
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : APP_task
 * Returned Value : none
 * Comments       :
 *     Used to execute the whole keyboard state machine running and keyboard data process.
 *
 *END*--------------------------------------------------------------------*/
void APP_task(void)
{
    usb_status status = USB_OK;
    static uint8_t i = 0;

    // Wait for insertion or removal event
    OS_Event_wait(mouse_usb_event, USB_EVENT_CTRL | USB_EVENT_DATA | USB_EVENT_DATA_CORRUPTED | USB_EVENT_STATE_CHANGE, FALSE, 0);
    /* update state for not app_task context */
    if (OS_Event_check_bit(mouse_usb_event, USB_EVENT_STATE_CHANGE))
    {
        OS_Event_clear(mouse_usb_event, USB_EVENT_STATE_CHANGE);
        update_state();
    }
    if (OS_Event_check_bit(mouse_usb_event, USB_EVENT_CTRL))
    {
        OS_Event_clear(mouse_usb_event, USB_EVENT_CTRL);
    }

    switch(mouse_hid_device.dev_state)
    {
    case USB_DEVICE_IDLE:
        break;

    case USB_DEVICE_ATTACHED:
        USB_PRINTF("Mouse device attached\r\n");
        mouse_hid_device.dev_state = USB_DEVICE_SET_INTERFACE_STARTED;
        status = usb_host_open_dev_interface(host_handle, mouse_hid_device.dev_handle, mouse_hid_device.intf_handle, (usb_class_handle*) &mouse_hid_device.class_handle);
        if (status != USB_OK)
        {
            USB_PRINTF("\r\nError in _usb_hostdev_open_interface: %x\r\n", status);
            return;
        }
        break;

    case USB_DEVICE_INTERFACE_OPENED:
        mouse_hid_get_report_descriptor();
        break;

    case USB_DEVICE_GET_REPORT_DESCRIPTOR_DONE:
        mouse_hid_get_buffer();
        USB_PRINTF("Mouse interfaced, setting protocol...\r\n");
        /* now we will set the USB Hid standard boot protocol */
        mouse_hid_device.dev_state = USB_DEVICE_SETTING_PROTOCOL;

        mouse_hid_com->class_ptr = mouse_hid_device.class_handle;
        mouse_hid_com->callback_fn = usb_host_hid_mouse_ctrl_callback;
        mouse_hid_com->callback_param = 0;

        status = usb_class_hid_set_protocol(mouse_hid_com, USB_PROTOCOL_HID_KEYBOARD);

        if (status != USB_OK)
        {
            USB_PRINTF("\nError in usb_class_hid_set_protocol: %x\n", status);
        }
        break;
    case USB_DEVICE_SET_IDLE:
        mouse_hid_device.dev_state = USB_DEVICE_SETTING_IDLE;
        mouse_hid_com->class_ptr = mouse_hid_device.class_handle;
        mouse_hid_com->callback_fn = usb_host_hid_mouse_ctrl_callback;
        mouse_hid_com->callback_param = 0;
        status = usb_class_hid_set_idle(mouse_hid_com, 0, 0);

        if (status != USB_OK)
        {
            USB_PRINTF("\nError in usb_class_hid_set_idle: %x\n", status);
        }
        break;

    case USB_DEVICE_SET_INTERFACE_STARTED:
        break;

    case USB_DEVICE_INUSE:
        //USB_PRINTF("%d\n", data_received);
        i++;
        if (mouse_data_received)
        {
            /******************************************************************
             Initiate a transfer request on the interrupt pipe
             ******************************************************************/
            mouse_hid_com->class_ptr = mouse_hid_device.class_handle;
            mouse_hid_com->callback_fn = usb_host_hid_mouse_recv_callback;
            mouse_hid_com->callback_param = 0;
#if 0
            if (i==4)
            {
                sleep_test = TRUE;
            }
#endif
            status = usb_class_hid_recv_data(mouse_hid_com, mouse_buffer, mouse_size);
#if 0
            if (sleep_test)
            sleep_test = FALSE;
#endif
            if (status != USB_OK)
            {
                USB_PRINTF("\nError in usb_class_hid_recv_data: %x", status);
            }
            else
            {
                //USB_PRINTF("try to get recv data\n");
                mouse_data_received = 0;
            }

        }
        /* Wait until we get the data from keyboard. */
        if (OS_Event_wait(mouse_usb_event, USB_EVENT_CTRL | USB_EVENT_DATA | USB_EVENT_DATA_CORRUPTED, FALSE, 0) == OS_EVENT_OK)
        {
            if (OS_Event_check_bit(mouse_usb_event, USB_EVENT_DATA))
            {
                //USB_PRINTF("get data\n");
                OS_Event_clear(mouse_usb_event, USB_EVENT_DATA);
                mouse_data_received = 1;
                if (mouse_hid_device.dev_state == USB_DEVICE_INUSE)
                {
                    process_mouse_buffer((uint8_t *) mouse_buffer);
                }
                status = usb_class_hid_recv_data(mouse_hid_com, mouse_buffer, mouse_size);
                if (status != USB_OK)
                {
                    USB_PRINTF("\r\nError in usb_class_hid_recv_data: %x", status);
                }
                else
                {
                    //USB_PRINTF("try to get recv data\r\n");
                    mouse_data_received = 0;
                }
            }

            if (OS_Event_check_bit(mouse_usb_event, USB_EVENT_DATA_CORRUPTED))
            {
                //USB_PRINTF("get corrupted\n");
                mouse_data_received = 1;
                OS_Event_clear(mouse_usb_event, USB_EVENT_DATA_CORRUPTED);
                status = usb_class_hid_recv_data(mouse_hid_com, mouse_buffer, mouse_size);
                if (status != USB_OK)
                {
                    USB_PRINTF("\r\nError in usb_class_hid_recv_data: %x", status);
                }
                else
                {
                    //USB_PRINTF("try to get recv data\r\n");
                    mouse_data_received = 0;
                }
            }

            if (OS_Event_check_bit(mouse_usb_event, USB_EVENT_CTRL))
            {
                //USB_PRINTF("get control\n");
                //OS_Event_set(USB_Event, USB_EVENT_CTRL);
            }
        }

        break;
    case USB_DEVICE_REMOVE:
        status = usb_host_dev_remove(host_handle, mouse_hid_device.dev_handle);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_host_dev_remove %x\n", status);
        }
        mouse_interface_number = 0;
        mouse_hid_device.intf_handle = NULL;
        mouse_hid_device.class_handle = NULL;
        USB_PRINTF("Device removed\r\nGoing to idle state\r\n");
        mouse_hid_device.dev_state = USB_DEVICE_IDLE;
        break;

    case USB_DEVICE_DETACHED:
        if (mouse_data_received == 1)
        {
            status = usb_host_close_dev_interface(host_handle, mouse_hid_device.dev_handle, mouse_hid_device.intf_handle, mouse_hid_device.class_handle);
            if (status != USB_OK)
            {
                USB_PRINTF("error in _usb_hostdev_close_interface %x\n", status);
            }
            mouse_hid_device.intf_handle = NULL;
            mouse_hid_device.class_handle = NULL;
            USB_PRINTF("Going to idle state\r\n");
            mouse_hid_device.dev_state = USB_DEVICE_IDLE;
            if (mouse_reportDescriptor != NULL)
            {
                OS_Mem_free(mouse_reportDescriptor);
                mouse_reportDescriptor = NULL;
            }
            if (mouse_buffer != NULL)
            {
                OS_Mem_free(mouse_buffer);
                mouse_buffer = NULL;
            }
        }
        else
        {
            if (OS_Event_wait(mouse_usb_event, USB_EVENT_CTRL | USB_EVENT_DATA | USB_EVENT_DATA_CORRUPTED, FALSE, 0) == OS_EVENT_OK)
            {
                if (OS_Event_check_bit(mouse_usb_event, USB_EVENT_DATA))
                {
                    //USB_PRINTF("get data\n");
                    OS_Event_clear(mouse_usb_event, USB_EVENT_DATA);
                    mouse_data_received = 1;
                    if (mouse_hid_device.dev_state == USB_DEVICE_INUSE)
                    {
                        process_mouse_buffer((uint8_t *) mouse_buffer);
                        OS_Mem_zero(mouse_buffer, mouse_size);
                    }
                }

                if (OS_Event_check_bit(mouse_usb_event, USB_EVENT_DATA_CORRUPTED))
                {
                    //USB_PRINTF("get corrupted\n");
                    mouse_data_received = 1;
                    OS_Event_clear(mouse_usb_event, USB_EVENT_DATA_CORRUPTED);
                }

                if (OS_Event_check_bit(mouse_usb_event, USB_EVENT_CTRL))
                {
                    //USB_PRINTF("get control\n");
                    //OS_Event_set(USB_Event, USB_EVENT_CTRL);
                }
            }
            OS_Event_set(mouse_usb_event, USB_EVENT_CTRL);
        }

        break;
    case USB_DEVICE_OTHER:
        break;
    default:
        break;
    } /* Endswitch */
}



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

    OS_Task_create(Task_Start, NULL, 4L, 3000L, "task_start", NULL);
    OSA_Start();
#if !defined(FSL_RTOS_MQX)
    return 1;
#endif
}

