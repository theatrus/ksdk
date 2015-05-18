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
 * $FileName: host_mouse.c$
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
#include "usb.h"

#include "usb_host_config.h"
#include "usb_host_stack_interface.h"
#include "usb_host_hid.h"
#include "usb_host_hub_sm.h"

#include "host_mouse.h"
#include "otg_mouse.h"
#include "host_mouse_api.h"
/*****************************************************************************
 * Constant and Macro's 
 *****************************************************************************/
/* KHCI task parameters */
#define USB_HOST_HID_TASK_TEMPLATE_INDEX       0
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)||((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)&& USE_RTOS))  
/* USB stack running on OS */
#define USB_HOST_HID_TASK_ADDRESS              HOST_APP_task_stun
/* USB stack running on BM  */
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)||(OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM))
#define USB_HOST_HID_TASK_ADDRESS              HOST_APP_task
#endif
#define USB_HOST_HID_TASK_STACKSIZE            3000
#define USB_HOST_HID_TASK_PRIORITY             (11)
#define USB_HOST_HID_TASK_NAME                 "HID Host Task"
#define USB_HOST_HID_TASK_ATTRIBUTES           0
#define USB_HOST_HID_TASK_DEFAULT_TIME_SLICE   0
#define USB_HOST_HID_ISR_EVENT_MASK            (~(uint32_t)0)
/* mouse event */
#define USB_EVENT_CTRL           (0x01)
#define USB_EVENT_DATA           (0x02)
#define USB_EVENT_DATA_CORRUPTED (0x04)
#define USB_EVENT_IDLE           (0x08)
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void process_mouse_buffer(uint8_t * buffer);
void HOST_APP_task_stun(uint32_t param);
void HOST_APP_task(void);
#ifdef __cplusplus
extern "C"
{
#endif
    extern usb_status usb_host_dev_mng_detach(usb_host_handle, uint8_t, uint8_t);
#ifdef __cplusplus
}
#endif
/************************************************************************************
 **
 ** Globals
 ************************************************************************************/
extern os_event_handle g_otg_app_event_handle;
device_struct_t g_mouse_hid_device = { 0 };
os_event_handle g_mouse_usb_event = NULL;
hid_command_t* g_mouse_hid_com = NULL;
uint8_t * g_mouse_buffer = NULL;
uint8_t * g_mouse_reportDescriptor = NULL;
uint16_t g_mouse_size = 0;
uint16_t g_mouse_reportLength = 0;
volatile uint32_t g_mouse_data_received = 1;
uint8_t g_mouse_interface_number = 0;
usb_device_interface_struct_t* g_mouse_interface_info[USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];
usb_host_handle g_host_handle = NULL;
uint32_t g_host_app_task_id = 0;
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Local Variables 
 *****************************************************************************/

static void usb_host_hid_mouse_ctrl_callback(void* unused, void* user_parm, uint8_t *buffer, uint32_t buflen, usb_status status);
static usb_status usb_host_hid_mouse_event(usb_device_instance_handle, usb_interface_descriptor_handle, usb_status);

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
/*****************************************************************************
 * Local Functions
 *****************************************************************************/

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : App_Host_Task_Create
 *  Returned Value : error or USB_OK
 *  Comments       :
 *        Create HID HOST app task
 *END*-----------------------------------------------------------------*/
static uint32_t App_Host_Task_Create(usb_host_handle host_handle)
{

    int32_t task_id = 0;

    task_id = OS_Task_create((task_start_t) USB_HOST_HID_TASK_ADDRESS, (void*) host_handle, (uint32_t)USB_HOST_HID_TASK_PRIORITY, USB_HOST_HID_TASK_STACKSIZE, USB_HOST_HID_TASK_NAME, NULL);
    if (task_id == OS_TASK_ERROR)
    {
        return 0;
    }
    return task_id;
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
usb_interface_descriptor_handle mouse_hid_get_interface()
{
    return (usb_interface_descriptor_handle)(g_mouse_interface_info[0]);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : mouse_hid_get_buffer
 * Returned Value : none
 * Comments       :
 *     used to get the buffer to store the data transferred from device.
 *
 *END*--------------------------------------------------------------------*/
uint32_t mouse_hid_get_buffer()
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
            g_mouse_size = (USB_SHORT_UNALIGNED_LE_TO_HOST(lpEp->lpEndpointDesc->wMaxPacketSize) & PACKET_SIZE_MASK);
            break;
        }
    }

    if ((g_mouse_size != 0) && (g_mouse_buffer == NULL))
    {
        g_mouse_buffer = (uint8_t *) OS_Mem_alloc_uncached_zero(g_mouse_size);
        if (g_mouse_buffer == NULL)
        {
            USB_PRINTF("\n\r allocate memory failed in hid_get_buffer");
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
void mouse_hid_get_report_descriptor()
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
        hid_descriptor_struct_t* hidDescriptor = (hid_descriptor_struct_t *) ptr1.pntr;
        hid_class_descriptor_struct_t* hidClassDescriptor = (hid_class_descriptor_struct_t*) &(hidDescriptor->bclassdescriptortype);
        uint8_t index;
        g_mouse_reportLength = 0;
        for (index = 0; index < hidDescriptor->bnumdescriptor; index++)
        {
            hid_class_descriptor_struct_t* temp;
            temp = hidClassDescriptor + index;
            if (temp->classdescriptortype == USB_DESC_TYPE_REPORT)
            {
                g_mouse_reportLength = USB_SHORT_UNALIGNED_LE_TO_HOST(temp->descriptorlength);
                break;
            }
        }
        if (g_mouse_reportLength != 0)
        {

            if (g_mouse_reportDescriptor != NULL)
            {
                OS_Mem_free(g_mouse_reportDescriptor);
            }
            g_mouse_reportDescriptor = (uint8_t *) OS_Mem_alloc_uncached_zero(g_mouse_reportLength);
            if (g_mouse_reportDescriptor == NULL)
            {
                USB_PRINTF("\n\r allocate memory failed in hid_get_buffer");
                return;
            }

            g_mouse_hid_device.dev_state = USB_DEVICE_GET_REPORT_DESCRIPTOR;
            g_mouse_hid_com->class_ptr = g_mouse_hid_device.class_handle;
            g_mouse_hid_com->callback_fn = usb_host_hid_mouse_ctrl_callback;
            g_mouse_hid_com->callback_param = 0;

            status = usb_class_hid_get_descriptor(g_mouse_hid_com, (uint8_t) USB_DESC_TYPE_REPORT, g_mouse_reportDescriptor, g_mouse_reportLength);

            if (status != USB_OK)
            {
                USB_PRINTF("\n\r Error in usb_class_hid_get_descriptor: %x", status);
            }
        }
    }
    else
    {
        USB_PRINTF("\n\r Can't find HID_DESCRIPTOR");
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
static usb_status usb_host_hid_mouse_event
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
        g_mouse_interface_info[g_mouse_interface_number] = pHostIntf;
        g_mouse_interface_number++;
        USB_PRINTF("\n\r ----- Attach Event -----");
        USB_PRINTF("\n\r State = %d", g_mouse_hid_device.dev_state);
        USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
        USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d", intf_ptr->bInterfaceProtocol);
        break;
    case USB_CONFIG_EVENT:
        if (g_mouse_hid_device.dev_state == USB_DEVICE_IDLE)
        {
            g_mouse_hid_device.dev_handle = dev_handle;
            g_mouse_hid_device.intf_handle = mouse_hid_get_interface();
            g_mouse_hid_device.dev_state = USB_DEVICE_ATTACHED;
        }
        else
        {
            USB_PRINTF("\n\r HID device already attached - dev_state = %d", g_mouse_hid_device.dev_state);
        }
        break;

    case USB_INTF_OPENED_EVENT:
        USB_PRINTF("\n\r ----- Interfaced Event -----");
        g_mouse_hid_device.dev_state = USB_DEVICE_INTERFACE_OPENED;
        break;
    case USB_DETACH_EVENT:
        /* Use only the interface with desired protocol */
        USB_PRINTF("\n\r ----- Detach Event -----");
        USB_PRINTF("\n\r State = %d", g_mouse_hid_device.dev_state);
        USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
        USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d", intf_ptr->bInterfaceProtocol);
        g_mouse_interface_number = 0;
        g_mouse_hid_device.dev_state = USB_DEVICE_DETACHED;
        break;
    default:
        USB_PRINTF("\n\r HID Device state = %d??", g_mouse_hid_device.dev_state);
        g_mouse_hid_device.dev_state = USB_DEVICE_IDLE;
        break;
    }

    /* notify application that status has changed */
    OS_Event_set(g_mouse_usb_event, USB_EVENT_CTRL);
    
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
        USB_PRINTF("\n\r HID control Request failed dev_state 0x%x status 0x%x!\n\r", g_mouse_hid_device.dev_state, status);
    }
    else if (status)
    {
        USB_PRINTF("\n\r HID control Request failed dev_state 0x%x status 0x%x!\n\r", g_mouse_hid_device.dev_state, status);
    }

    if (g_mouse_hid_device.dev_state == USB_DEVICE_SETTING_PROTOCOL)
    {
        g_mouse_hid_device.dev_state = USB_DEVICE_SET_IDLE;
        USB_PRINTF("\n\r setting protocol done");
    }
    else if (g_mouse_hid_device.dev_state == USB_DEVICE_GET_REPORT_DESCRIPTOR)
    {
        g_mouse_hid_device.dev_state = USB_DEVICE_GET_REPORT_DESCRIPTOR_DONE;
        USB_PRINTF("\n\r get report descriptor done");
    }
    else if (g_mouse_hid_device.dev_state == USB_DEVICE_SETTING_IDLE)
    {
        g_mouse_hid_device.dev_state = USB_DEVICE_INUSE;
        USB_PRINTF("\n\r setting idle done\n");
    }
    /* notify application that status has changed */
    OS_Event_set(g_mouse_usb_event, USB_EVENT_CTRL);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_hid_recv_callback
 * Returned Value : None
 * Comments       :
 *     Called when data is received on a pipe
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
        OS_Event_set(g_mouse_usb_event, USB_EVENT_DATA);
    }
    else if (status == USBERR_TR_CANCEL)
    {
        /* notify application that data are available */
        USB_PRINTF("\n\r tr cancel");
        OS_Event_set(g_mouse_usb_event, USB_EVENT_DATA_CORRUPTED);
    }
    else
    {
        OS_Event_set(g_mouse_usb_event, USB_EVENT_DATA_CORRUPTED);
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : process_mouse_buffer
 * Returned Value : None
 * Comments       :
 *   Process the data from mouse buffer
 *END*--------------------------------------------------------------------*/
void process_mouse_buffer
(
    uint8_t * buffer
    )
{
    USB_PRINTF("\n\r");
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
        USB_PRINTF("             ");
    }
    if (buffer[0] & 0x02)
    {
        USB_PRINTF("Right Click ");
    }
    else
    {
        USB_PRINTF("            ");
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
        USB_PRINTF("\n\r ----- Unsupported Interface of attached Device -----");
        USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
        USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d", intf_ptr->bInterfaceProtocol);
    }
    else if (USB_ATTACH_DEVICE_NOT_SUPPORT == event_code)
    {
        USB_PRINTF("\n\r ----- Unsupported Device attached -----");
    }
    
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : HOST_APP_load
 * Returned Value :
 * Comments       :
 * Returned Value :
 *
 *END*--------------------------------------------------------------------*/
usb_status HOST_APP_load(void)
{
    OS_Event_set(g_otg_app_event_handle, OTG_LOAD_HOST);
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : HOST_APP_unload
 * Returned Value :
 * Comments       :
 * Returned Value :
 *
 *END*--------------------------------------------------------------------*/
usb_status HOST_APP_unload(void)
{
    OS_Event_set(g_otg_app_event_handle, OTG_UNLOAD_HOST);
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : HOST_APP_init
 * Returned Value :
 * Comments       :
 * Returned Value :
 *
 *END*--------------------------------------------------------------------*/
usb_status HOST_APP_init(void)
{
    usb_status status = USB_OK;
    status = usb_host_init(0, &g_host_handle);
    if (status != USB_OK)
    {
        USB_PRINTF("\n\r USB Host Initialization failed! STATUS: 0x%x", status);
        return USBERR_UNKNOWN_ERROR;
    }
    /*
     ** since we are going to act as the host driver, register the driver
     ** information for wanted class/subclass/protocols
     */
    status = usb_host_register_driver_info(g_host_handle, (void *) DriverInfoTable);
    if (status != USB_OK)
    {
        USB_PRINTF("\n\r USB Initialization driver info failed! STATUS: 0x%x", status);
        return USBERR_UNKNOWN_ERROR;
    }

    status = usb_host_register_unsupported_device_notify(g_host_handle, usb_host_hid_unsupported_device_event);
    if (status != USB_OK)
    {
        USB_PRINTF("\n\r USB Initialization driver info failed! STATUS: 0x%x", status);
        return USBERR_UNKNOWN_ERROR;
    }

    g_mouse_usb_event = OS_Event_create(0);/* manually clear */
    if (g_mouse_usb_event == NULL)
    {
        USB_PRINTF("\n\r OS_Event_create failed!\n");
        return USBERR_UNKNOWN_ERROR;
    }
    g_mouse_hid_com = (hid_command_t*) OS_Mem_alloc_zero(sizeof(hid_command_t));
    if (g_mouse_hid_com == NULL)
    {
        USB_PRINTF("\n\r mouse_hid_com allocate failed!\n");
        return USBERR_UNKNOWN_ERROR;
    }
    /* create the host app */
    g_host_app_task_id = App_Host_Task_Create(g_host_handle);
    if (g_host_app_task_id == 0)
    {
        return USBERR_UNKNOWN_ERROR;
    }
    USB_PRINTF("\n\r USB HID Mouse\n\r Waiting for USB Mouse to be attached...\n");
    return status;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : HOST_APP_uninit
 * Returned Value :
 * Comments       :
 * Returned Value :
 *
 *END*--------------------------------------------------------------------*/
void HOST_APP_uninit(void)
{
    usb_status status = USB_OK;

    /* handle the uncompleted tasks */

    if (g_mouse_hid_device.intf_handle != NULL)
    {
        usb_host_dev_mng_detach(g_host_handle, 0, 0);
        status = usb_host_close_dev_interface(g_host_handle, g_mouse_hid_device.dev_handle, g_mouse_hid_device.intf_handle, g_mouse_hid_device.class_handle);
        if (status != USB_OK)
        {
            USB_PRINTF("\n\r error in _usb_hostdev_close_interface %x", status);
        }
    }
    g_mouse_hid_device.dev_state = USB_DEVICE_IDLE;
    if (g_host_handle != NULL)
    {
        usb_host_deinit(g_host_handle);
    }
    if (g_mouse_hid_com != NULL)
    {
        OS_Mem_free(g_mouse_hid_com);
        g_mouse_hid_com = NULL;
    }
    if (g_host_app_task_id != 0)
    {
        OS_Task_delete(g_host_app_task_id);
    }
    OS_Event_destroy(g_mouse_usb_event);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : HOST_APP_task
 * Returned Value :
 * Comments       :
 * Returned Value :
 *
 *END*--------------------------------------------------------------------*/

void HOST_APP_task()
{
    usb_status status = USB_OK;
    static uint8_t i = 0;

    /* Wait for insertion or removal event */
    OS_Event_wait(g_mouse_usb_event, USB_EVENT_CTRL | USB_EVENT_DATA | USB_EVENT_DATA_CORRUPTED, FALSE, 0);
    if (OS_Event_check_bit(g_mouse_usb_event, USB_EVENT_CTRL))
    {
        OS_Event_clear(g_mouse_usb_event, USB_EVENT_CTRL);
    }

    switch(g_mouse_hid_device.dev_state)
    {
    case USB_DEVICE_IDLE:
        break;
    case USB_DEVICE_ATTACHED:
        USB_PRINTF("\n\r Mouse device attached");
        g_mouse_hid_device.dev_state = USB_DEVICE_SET_INTERFACE_STARTED;
        status = usb_host_open_dev_interface(g_host_handle, g_mouse_hid_device.dev_handle, g_mouse_hid_device.intf_handle, (usb_class_handle*) &g_mouse_hid_device.class_handle);
        if (status != USB_OK)
        {
            USB_PRINTF("\n\r Error in _usb_hostdev_open_interface: %x", status);
            return;
        }
        break;
    case USB_DEVICE_INTERFACE_OPENED:
        mouse_hid_get_report_descriptor();
        break;
    case USB_DEVICE_GET_REPORT_DESCRIPTOR_DONE:
        mouse_hid_get_buffer();
        USB_PRINTF("\n\r Mouse interfaced, setting protocol...");
        /* now we will set the USB Hid standard boot protocol */
        g_mouse_hid_device.dev_state = USB_DEVICE_SETTING_PROTOCOL;
        g_mouse_hid_com->class_ptr = g_mouse_hid_device.class_handle;
        g_mouse_hid_com->callback_fn = usb_host_hid_mouse_ctrl_callback;
        g_mouse_hid_com->callback_param = 0;

        status = usb_class_hid_set_protocol(g_mouse_hid_com, USB_PROTOCOL_HID_KEYBOARD);
        if (status != USB_OK)
        {
            USB_PRINTF("\n\r Error in usb_class_hid_set_protocol: %x", status);
        }
        g_mouse_data_received = 1;
        break;
    case USB_DEVICE_SET_IDLE:
        g_mouse_hid_device.dev_state = USB_DEVICE_SETTING_IDLE;
        g_mouse_hid_com->class_ptr = g_mouse_hid_device.class_handle;
        g_mouse_hid_com->callback_fn = usb_host_hid_mouse_ctrl_callback;
        g_mouse_hid_com->callback_param = 0;
        status = usb_class_hid_set_idle(g_mouse_hid_com, 0, 0);

        if (status != USB_OK)
        {
            USB_PRINTF("\nError in usb_class_hid_set_idle: %x\n", status);
        }
    case USB_DEVICE_SET_INTERFACE_STARTED:
        break;
    case USB_DEVICE_INUSE:
        i++;
        if (g_mouse_data_received)
        {
            /******************************************************************
             Initiate a transfer request on the interrupt pipe
             ******************************************************************/
            g_mouse_hid_com->class_ptr = g_mouse_hid_device.class_handle;
            g_mouse_hid_com->callback_fn = usb_host_hid_mouse_recv_callback;
            g_mouse_hid_com->callback_param = 0;
            status = usb_class_hid_recv_data(g_mouse_hid_com, g_mouse_buffer, g_mouse_size);
            if (status != USB_OK)
            {
                USB_PRINTF("\n\r Error in usb_class_hid_recv_data: %x", status);
            }
            else
            {
                g_mouse_data_received = 0;
            }

        }
        /* Wait until we get the data from keyboard. */
        if (OS_Event_wait(g_mouse_usb_event, USB_EVENT_CTRL | USB_EVENT_DATA | USB_EVENT_DATA_CORRUPTED, FALSE, 10) == OS_EVENT_OK)
        {
            if (OS_Event_check_bit(g_mouse_usb_event, USB_EVENT_DATA))
            {
                OS_Event_clear(g_mouse_usb_event, USB_EVENT_DATA);
                g_mouse_data_received = 1;
                if (g_mouse_hid_device.dev_state == USB_DEVICE_INUSE)
                {
                    process_mouse_buffer((uint8_t *) g_mouse_buffer);
                }
                status = usb_class_hid_recv_data(g_mouse_hid_com, g_mouse_buffer, g_mouse_size);
                if (status != USB_OK)
                {
                    USB_PRINTF("\r\nError in usb_class_hid_recv_data: %x", status);
                }
                else
                {
                    //USB_PRINTF("try to get recv data\r\n");
                    g_mouse_data_received = 0;
                }
            }

            if (OS_Event_check_bit(g_mouse_usb_event, USB_EVENT_DATA_CORRUPTED))
            {
                g_mouse_data_received = 1;
                OS_Event_clear(g_mouse_usb_event, USB_EVENT_DATA_CORRUPTED);
                status = usb_class_hid_recv_data(g_mouse_hid_com, g_mouse_buffer, g_mouse_size);
                if (status != USB_OK)
                {
                    USB_PRINTF("\r\nError in usb_class_hid_recv_data: %x", status);
                }
                else
                {
                    //USB_PRINTF("try to get recv data\r\n");
                    g_mouse_data_received = 0;
                }
            }

            if (OS_Event_check_bit(g_mouse_usb_event, USB_EVENT_CTRL))
            {
                /* OS_Event_set(USB_Event, USB_EVENT_CTRL); */
            }
        }

        break;

    case USB_DEVICE_DETACHED:
        if (g_mouse_data_received == 1)
        {
            status = usb_host_close_dev_interface(g_host_handle, g_mouse_hid_device.dev_handle, g_mouse_hid_device.intf_handle, g_mouse_hid_device.class_handle);
            if (status != USB_OK)
            {
                USB_PRINTF("\n\r error in _usb_hostdev_close_interface %x", status);
            }
            g_mouse_hid_device.intf_handle = NULL;
            g_mouse_hid_device.class_handle = NULL;
            USB_PRINTF("\n\r Going to idle state");
            g_mouse_hid_device.dev_state = USB_DEVICE_IDLE;
        }
        else
        {
            if (OS_Event_wait(g_mouse_usb_event, USB_EVENT_CTRL | USB_EVENT_DATA | USB_EVENT_DATA_CORRUPTED, FALSE, 0) == OS_EVENT_OK)
            {
                if (OS_Event_check_bit(g_mouse_usb_event, USB_EVENT_DATA))
                {
                    OS_Event_clear(g_mouse_usb_event, USB_EVENT_DATA);
                    g_mouse_data_received = 1;
                    if (g_mouse_hid_device.dev_state == USB_DEVICE_INUSE)
                    {
                        process_mouse_buffer((uint8_t *) g_mouse_buffer);
                        OS_Mem_zero(g_mouse_buffer, g_mouse_size);
                    }
                }

                if (OS_Event_check_bit(g_mouse_usb_event, USB_EVENT_DATA_CORRUPTED))
                {
                    g_mouse_data_received = 1;
                    OS_Event_clear(g_mouse_usb_event, USB_EVENT_DATA_CORRUPTED);
                }

                if (OS_Event_check_bit(g_mouse_usb_event, USB_EVENT_CTRL))
                {
                    /* OS_Event_set(USB_Event, USB_EVENT_CTRL); */
                }
            }
            OS_Event_set(g_mouse_usb_event, USB_EVENT_CTRL);
        }

        break;
    case USB_DEVICE_OTHER:
        break;
    default:
        break;
    } /* Endswitch */
}

#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)||((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)&& USE_RTOS))  
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : HOST_task
 * Returned Value :
 * Comments       :
 * Returned Value :
 *
 *END*--------------------------------------------------------------------*/
void HOST_APP_task_stun(uint32_t param)
{
    /*
     ** Infinite loop, waiting for events requiring action
     */
    for (;;)
    {
        HOST_APP_task();
    } /* Endfor */
} /* Endbody */
#endif
