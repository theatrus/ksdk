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
#include "mouse_keyboard.h"

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#endif
/************************************************************************************
 **
 ** Globals
 ************************************************************************************/
device_struct_t mouse_hid_device = { 0 };
os_event_handle mouse_usb_event = NULL;
hid_command_t* mouse_hid_com = NULL;
uint8_t * mouse_buffer = NULL;
uint8_t * mouse_reportDescriptor = NULL;
uint16_t mouse_size = 0;
uint16_t mouse_reportLength = 0;
volatile uint32_t mouse_data_received = 1;
volatile uint32_t mouse_target = 2;
uint8_t mouse_interface_number = 0;
usb_device_interface_struct_t* mouse_interface_info[USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];

#define USB_EVENT_CTRL           (0x01)
#define USB_EVENT_DATA           (0x02)
#define USB_EVENT_DATA_CORRUPTED (0x04)

void process_mouse_buffer(uint8_t * buffer);
static void usb_host_hid_mouse_ctrl_callback(void* unused, void* user_parm, uint8_t *buffer, uint32_t buflen, usb_status status);

static void update_state(uint32_t next_state)
{
    if (mouse_hid_device.STATE_MUTEX != NULL)
    {
        OS_Mutex_lock(mouse_hid_device.STATE_MUTEX);
    }
    else
    {
        printf("mouse state mutex is NULL\r\n");
    }
    switch(next_state)
    {
    case USB_DEVICE_ATTACHED:
        if (mouse_hid_device.DEV_STATE == USB_DEVICE_IDLE)
        {
            mouse_hid_device.DEV_STATE = next_state;
        }
        break;
    case USB_DEVICE_SET_INTERFACE_STARTED:
        if (mouse_hid_device.DEV_STATE == USB_DEVICE_ATTACHED)
        {
            mouse_hid_device.DEV_STATE = next_state;
        }
        break;
    case USB_DEVICE_INTERFACE_OPENED:
        if (mouse_hid_device.DEV_STATE == USB_DEVICE_SET_INTERFACE_STARTED)
        {
            mouse_hid_device.DEV_STATE = next_state;
        }
        break;
    case USB_DEVICE_GET_REPORT_DESCRIPTOR:
        if (mouse_hid_device.DEV_STATE == USB_DEVICE_INTERFACE_OPENED)
        {
            mouse_hid_device.DEV_STATE = next_state;
        }
        break;
    case USB_DEVICE_GET_REPORT_DESCRIPTOR_DONE:
        if (mouse_hid_device.DEV_STATE == USB_DEVICE_GET_REPORT_DESCRIPTOR)
        {
            mouse_hid_device.DEV_STATE = next_state;
        }
        break;
    case USB_DEVICE_SETTING_PROTOCOL:
        if (mouse_hid_device.DEV_STATE == USB_DEVICE_GET_REPORT_DESCRIPTOR_DONE)
        {
            mouse_hid_device.DEV_STATE = next_state;
        }
        break;
    case USB_DEVICE_SET_IDLE:
        if (mouse_hid_device.DEV_STATE == USB_DEVICE_SETTING_PROTOCOL)
        {
            mouse_hid_device.DEV_STATE = next_state;
        }
        break;
    case USB_DEVICE_SETTING_IDLE:
        if (mouse_hid_device.DEV_STATE == USB_DEVICE_SET_IDLE)
        {
            mouse_hid_device.DEV_STATE = next_state;
        }
        break;
    case USB_DEVICE_INUSE:
        if (mouse_hid_device.DEV_STATE == USB_DEVICE_SETTING_IDLE)
        {
            mouse_hid_device.DEV_STATE = next_state;
        }
        break;
    case USB_DEVICE_DETACHED:
        mouse_hid_device.DEV_STATE = next_state;
        break;
    case USB_DEVICE_IDLE:
        mouse_hid_device.DEV_STATE = next_state;
        break;
    default:
        break;
    }
    if (mouse_hid_device.STATE_MUTEX != NULL)
    {
        OS_Mutex_unlock(mouse_hid_device.STATE_MUTEX);
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
        mouse_buffer = (uint8_t*) OS_Mem_alloc_uncached_zero(mouse_size);
        if (mouse_buffer == NULL)
        {
            USB_PRINTF("allocate memory failed in hid_get_buffer\r\n");
            return (uint32_t)(-1);
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
            mouse_reportDescriptor = (uint8_t*) OS_Mem_alloc_uncached_zero(mouse_reportLength);
            if (mouse_reportDescriptor == NULL)
            {
                USB_PRINTF("allocate memory failed in hid_get_buffer\r\n");
                return;
            }

            update_state (USB_DEVICE_GET_REPORT_DESCRIPTOR);
            mouse_hid_com->class_ptr = mouse_hid_device.CLASS_HANDLE;
            mouse_hid_com->callback_fn = usb_host_hid_mouse_ctrl_callback;
            mouse_hid_com->callback_param = 0;

            status = usb_class_hid_get_descriptor(mouse_hid_com, (uint8_t) USB_DESC_TYPE_REPORT, mouse_reportDescriptor, mouse_reportLength);

            if (status != USB_OK)
            {
                USB_PRINTF("\r\nError in usb_class_hid_get_descriptor: %x\r\n", status);
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
        USB_PRINTF("State = %d", mouse_hid_device.DEV_STATE);
        USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
        USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
        break;
    case USB_CONFIG_EVENT:
        if (mouse_hid_device.DEV_STATE == USB_DEVICE_IDLE)
        {
            if (mouse_hid_device.STATE_MUTEX == NULL)
            {
                mouse_hid_device.STATE_MUTEX = OS_Mutex_create();
                if (mouse_hid_device.STATE_MUTEX == NULL)
                {
                    USB_PRINTF("mouse state mutex create fail\r\n");
                }
            }
            mouse_hid_device.DEV_HANDLE = dev_handle;
            mouse_hid_device.INTF_HANDLE = mouse_hid_get_interface();
            update_state (USB_DEVICE_ATTACHED);
        }
        else
        {
            USB_PRINTF("HID device already attached - DEV_STATE = %d\r\n", mouse_hid_device.DEV_STATE);
        }
        break;

    case USB_INTF_OPENED_EVENT:
        update_state (USB_DEVICE_INTERFACE_OPENED);
        USB_PRINTF("----- Interfaced Event -----\r\n");
        break;

    case USB_DETACH_EVENT:
        /* Use only the interface with desired protocol */
        USB_PRINTF("\r\n----- Detach Event -----\r\n");
        USB_PRINTF("State = %d", mouse_hid_device.DEV_STATE);
        USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
        USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
        mouse_interface_number = 0;
        update_state (USB_DEVICE_DETACHED);
        break;
    default:
        USB_PRINTF("HID Device state = %d??\r\n", mouse_hid_device.DEV_STATE);
        update_state (USB_DEVICE_IDLE);
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
static void usb_host_hid_mouse_ctrl_callback
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
        USB_PRINTF("\r\nHID control Request failed DEV_STATE 0x%x status 0x%x!\r\n", mouse_hid_device.DEV_STATE, status);
    }
    else if (status)
    {
        USB_PRINTF("\r\nHID control Request failed DEV_STATE 0x%x status 0x%x!\r\n", mouse_hid_device.DEV_STATE, status);
    }

    if (mouse_hid_device.DEV_STATE == USB_DEVICE_SETTING_PROTOCOL)
    {
        update_state (USB_DEVICE_SET_IDLE);
        USB_PRINTF("setting protocol done\r\n");
    }
    else if (mouse_hid_device.DEV_STATE == USB_DEVICE_GET_REPORT_DESCRIPTOR)
    {
        update_state (USB_DEVICE_GET_REPORT_DESCRIPTOR_DONE);
        USB_PRINTF("get report descriptor done\r\n");
    }
    else if (mouse_hid_device.DEV_STATE == USB_DEVICE_SETTING_IDLE)
    {
        update_state (USB_DEVICE_INUSE);
        USB_PRINTF("setting idle done\r\n");
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
        //USB_PRINTF("tr cancel\r\n");
        OS_Event_set(mouse_usb_event, USB_EVENT_DATA_CORRUPTED);
    }
    else
    {
        //USB_PRINTF("tr completed 0x%x\r\n", status);
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
 * Function Name  : Mouse_Task
 * Returned Value : none
 * Comments       :
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/
void Mouse_Task(void* param)
{
    usb_status status = USB_OK;
    static uint8_t i = 0;
    usb_host_handle mouse_host_handle = (usb_host_handle) param;

    // Wait for insertion or removal event
    OS_Event_wait(mouse_usb_event, USB_EVENT_CTRL | USB_EVENT_DATA | USB_EVENT_DATA_CORRUPTED, FALSE, 0);
    if (OS_Event_check_bit(mouse_usb_event, USB_EVENT_CTRL))
    {
        OS_Event_clear(mouse_usb_event, USB_EVENT_CTRL);
    }

    switch(mouse_hid_device.DEV_STATE)
    {
    case USB_DEVICE_IDLE:
        break;

    case USB_DEVICE_ATTACHED:
        USB_PRINTF("\r\nMouse device attached\n");
        update_state (USB_DEVICE_SET_INTERFACE_STARTED);
        status = usb_host_open_dev_interface(mouse_host_handle, mouse_hid_device.DEV_HANDLE, mouse_hid_device.INTF_HANDLE, (usb_class_handle*) &mouse_hid_device.CLASS_HANDLE);
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
        update_state (USB_DEVICE_SETTING_PROTOCOL);

        mouse_hid_com->class_ptr = mouse_hid_device.CLASS_HANDLE;
        mouse_hid_com->callback_fn = usb_host_hid_mouse_ctrl_callback;
        mouse_hid_com->callback_param = 0;

        status = usb_class_hid_set_protocol(mouse_hid_com, USB_PROTOCOL_HID_KEYBOARD);

        if (status != USB_OK)
        {
            USB_PRINTF("\r\nError in usb_class_hid_set_protocol: %x\r\n", status);
        }

        break;
    case USB_DEVICE_SET_IDLE:
        update_state (USB_DEVICE_SETTING_IDLE);
        mouse_hid_com->class_ptr = mouse_hid_device.CLASS_HANDLE;
        mouse_hid_com->callback_fn = usb_host_hid_mouse_ctrl_callback;
        mouse_hid_com->callback_param = 0;
        status = usb_class_hid_set_idle(mouse_hid_com, 0, 0);

        if (status != USB_OK)
        {
            USB_PRINTF("\r\nError in usb_class_hid_set_idle: %x\r\n", status);
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
            mouse_hid_com->class_ptr = mouse_hid_device.CLASS_HANDLE;
            mouse_hid_com->callback_fn = usb_host_hid_mouse_recv_callback;
            mouse_hid_com->callback_param = 0;

            status = usb_class_hid_recv_data(mouse_hid_com, mouse_buffer, mouse_size);
            if (status != USB_OK)
            {
#if _DEBUG
                USB_PRINTF("\r\nError in usb_class_hid_recv_data: %x", status);
#endif
            }
            else
            {
                //USB_PRINTF("try to get recv data\r\n");
                mouse_data_received = 0;
            }

        }
        /* Wait until we get the data from keyboard. */

        if (OS_Event_wait(mouse_usb_event, USB_EVENT_CTRL | USB_EVENT_DATA | USB_EVENT_DATA_CORRUPTED, FALSE, 0) == OS_EVENT_OK)
        {
            if (OS_Event_check_bit(mouse_usb_event, USB_EVENT_DATA))
            {
                //USB_PRINTF("get data\r\n");
                OS_Event_clear(mouse_usb_event, USB_EVENT_DATA);
                mouse_data_received = 1;
                if (mouse_hid_device.DEV_STATE == USB_DEVICE_INUSE)
                {
                    process_mouse_buffer((uint8_t *) mouse_buffer);
                }
                status = usb_class_hid_recv_data(mouse_hid_com, mouse_buffer, mouse_size);
                if (status != USB_OK)
                {
#if _DEBUG
                    USB_PRINTF("\rError in usb_class_hid_recv_data: %x", status);
#endif
                }
                else
                {
                    //USB_PRINTF("try to get recv data\r");
                    mouse_data_received = 0;
                }
            }

            if (OS_Event_check_bit(mouse_usb_event, USB_EVENT_DATA_CORRUPTED))
            {
                //USB_PRINTF("get corrupted\r\n");
                mouse_data_received = 1;
                OS_Event_clear(mouse_usb_event, USB_EVENT_DATA_CORRUPTED);
                status = usb_class_hid_recv_data(mouse_hid_com, mouse_buffer, mouse_size);
                if (status != USB_OK)
                {
#if _DEBUG
                    USB_PRINTF("\rError in usb_class_hid_recv_data: %x", status);
#endif
                }
                else
                {
                    //USB_PRINTF("try to get recv data\r");
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

    case USB_DEVICE_DETACHED:
        if (mouse_data_received == 1)
        {
            status = usb_host_close_dev_interface(mouse_host_handle, mouse_hid_device.DEV_HANDLE, mouse_hid_device.INTF_HANDLE, mouse_hid_device.CLASS_HANDLE);
            if (status != USB_OK)
            {
                USB_PRINTF("error in _usb_hostdev_close_interface %x\r\n", status);
            }
            mouse_hid_device.INTF_HANDLE = NULL;
            mouse_hid_device.CLASS_HANDLE = NULL;
            USB_PRINTF("Going to idle state\r\n");
            update_state (USB_DEVICE_IDLE);
            if (mouse_hid_device.STATE_MUTEX != NULL)
            {
                OS_Mutex_destroy(mouse_hid_device.STATE_MUTEX);
                mouse_hid_device.STATE_MUTEX = NULL;
            }
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
                    if (mouse_hid_device.DEV_STATE == USB_DEVICE_INUSE)
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

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : Keyboard_Task_Stun
 * Returned Value : none
 * Comments       :
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/
void Mouse_Task_Stun(void* param)
{
    while (1)
    {
        //OS_Event_wait(mouse_usb_event, USB_EVENT_CTRL, FALSE, 0);
        Mouse_Task(param);
    }
}

/* EOF */

