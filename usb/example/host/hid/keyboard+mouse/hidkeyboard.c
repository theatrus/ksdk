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
* $FileName: hidkeyboard.c$
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
device_struct_t                 kbd_hid_device = { 0 };
os_event_handle               kbd_usb_event = NULL;
hid_command_t*               kbd_hid_com = NULL;
uint8_t *                     kbd_buffer = NULL;
uint8_t *                     kbd_reportDescriptor = NULL;
uint16_t                      kbd_size = 0;
uint16_t                      kbd_reportLength = 0;
volatile uint32_t             kbd_data_received = 1;
volatile uint32_t             kbd_target = 2;
uint8_t                       kbd_interface_number = 0;
usb_device_interface_struct_t*  kbd_interface_info[USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];
usb_host_handle              host_handle = NULL;
volatile uint32_t             sleep_test = FALSE;

#define USB_EVENT_CTRL           (0x01)
#define USB_EVENT_DATA           (0x02)
#define USB_EVENT_DATA_CORRUPTED (0x04)

void Keyboard_Task(void* param);
void process_kbd_buffer(uint8_t* buffer);
#define MAX_SUPPORTED_USAGE_ID 57

static const uint8_t HID_table[MAX_SUPPORTED_USAGE_ID][2] = {
    {0, 0},                     /* Usage ID  0 */
    {0, 0},                     /* Usage ID  1 */
    {0, 0},                     /* Usage ID  2 */
    {0, 0},                     /* Usage ID  3 */
    {0x61, 'A'},                /* Usage ID  4 */
    {'b', 'B'},                 /* Usage ID  5 */
    {'c', 'C'},                 /* Usage ID  6 */
    {'d', 'D'},                 /* Usage ID  7 */
    {'e', 'E'},                 /* Usage ID  8 */
    {'f', 'F'},                 /* Usage ID  9 */
    {'g', 'G'},                 /* Usage ID 10 */
    {'h', 'H'},                 /* Usage ID 11 */
    {'i', 'I'},                 /* Usage ID 12 */
    {'j', 'J'},                 /* Usage ID 13 */
    {'k', 'K'},                 /* Usage ID 14 */
    {'l', 'L'},                 /* Usage ID 15 */
    {'m', 'M'},                 /* Usage ID 16 */
    {'n', 'N'},                 /* Usage ID 17 */
    {'o', 'O'},                 /* Usage ID 18 */
    {'p', 'P'},                 /* Usage ID 19 */
    {'q', 'Q'},                 /* Usage ID 20 */
    {'r', 'R'},                 /* Usage ID 21 */
    {'s', 'S'},                 /* Usage ID 22 */
    {'t', 'T'},                 /* Usage ID 23 */
    {'u', 'U'},                 /* Usage ID 24 */
    {'v', 'V'},                 /* Usage ID 25 */
    {'w', 'W'},                 /* Usage ID 26 */
    {'x', 'X'},                 /* Usage ID 27 */
    {'y', 'Y'},                 /* Usage ID 28 */
    {'z', 'Z'},                 /* Usage ID 29 */
    {'1', '!'},                 /* Usage ID 30 */
    {'2', '@'},                 /* Usage ID 31 */
    {'3', '#'},                 /* Usage ID 32 */
    {'4', '$'},                 /* Usage ID 33 */
    {'5', '%'},                 /* Usage ID 34 */
    {'6', '^'},                 /* Usage ID 35 */
    {'7', '&'},                 /* Usage ID 36 */
    {'8', '*'},                 /* Usage ID 37 */
    {'9', '('},                 /* Usage ID 38 */
    {'0', ')'},                 /* Usage ID 39 */
    {'\n', '\n'},               /* Usage ID 40 */ //CARRIAGE
    {0x1B, 0x1B},               /* Usage ID 41 */ //ESC
    {'\b', '\b'},               /* Usage ID 42 */ //BACKSPACE
    {'\t', '\t'},               /* Usage ID 43 */ //TAB
    {' ', ' '},                 /* Usage ID 44 */ //SPACE
    {'-', '_'},                 /* Usage ID 45 */
    {'=', '+'},                 /* Usage ID 46 */
    {'[', '{'},                 /* Usage ID 47 */
    {']', '}'},                 /* Usage ID 48 */
    {'\\', '|'},                /* Usage ID 49 */
    {0, 0},                     /* Usage ID 50 */
    {';', ':'},                 /* Usage ID 51 */
    {'\'', '\"'},               /* Usage ID 52 */
    {'`', '~'},                 /* Usage ID 53 */
    {',', '<'},                 /* Usage ID 54 */
    {'.', '>'},                 /* Usage ID 55 */
    {'/', '?'},                 /* Usage ID 56 */
};

static void update_state(uint32_t next_state)
{
    if (kbd_hid_device.STATE_MUTEX != NULL)
        OS_Mutex_lock(kbd_hid_device.STATE_MUTEX);
    else
        printf("keyboard state mutex is NULL\r\n");
    switch(next_state)
    {
        case USB_DEVICE_ATTACHED:
            if (kbd_hid_device.DEV_STATE == USB_DEVICE_IDLE)
                kbd_hid_device.DEV_STATE = next_state;
            break;
        case USB_DEVICE_SET_INTERFACE_STARTED:
            if (kbd_hid_device.DEV_STATE == USB_DEVICE_ATTACHED)
                kbd_hid_device.DEV_STATE = next_state;
            break;
        case USB_DEVICE_INTERFACE_OPENED:
            if (kbd_hid_device.DEV_STATE == USB_DEVICE_SET_INTERFACE_STARTED)
                kbd_hid_device.DEV_STATE = next_state;
            break;
        case USB_DEVICE_GET_REPORT_DESCRIPTOR:
            if (kbd_hid_device.DEV_STATE == USB_DEVICE_INTERFACE_OPENED)
                kbd_hid_device.DEV_STATE = next_state;
            break;
        case USB_DEVICE_GET_REPORT_DESCRIPTOR_DONE:
            if (kbd_hid_device.DEV_STATE == USB_DEVICE_GET_REPORT_DESCRIPTOR)
                kbd_hid_device.DEV_STATE = next_state;
            break;
        case USB_DEVICE_SETTING_PROTOCOL:
            if (kbd_hid_device.DEV_STATE == USB_DEVICE_GET_REPORT_DESCRIPTOR_DONE)
                kbd_hid_device.DEV_STATE = next_state;
            break;
        case USB_DEVICE_INUSE:
            if (kbd_hid_device.DEV_STATE == USB_DEVICE_SETTING_PROTOCOL)
                kbd_hid_device.DEV_STATE = next_state;
            break;
        case USB_DEVICE_DETACHED:
            kbd_hid_device.DEV_STATE = next_state;
            break;
        case USB_DEVICE_IDLE:
            kbd_hid_device.DEV_STATE = next_state;
            break;
        default:
            break;
    }
    if (kbd_hid_device.STATE_MUTEX != NULL)
        OS_Mutex_unlock(kbd_hid_device.STATE_MUTEX);
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : kbd_hid_get_interface
* Returned Value : none
* Comments       :
*     Used to get the proper interface if there are multiple interfaces in a device are available
*     for us.
*     In this example, we always choose the first interface.
*     For the customer, a proper way should be implemented as needed.
*
*END*--------------------------------------------------------------------*/
usb_interface_descriptor_handle kbd_hid_get_interface()
{
    return (usb_interface_descriptor_handle)(kbd_interface_info[0]);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : kbd_hid_get_buffer
* Returned Value : none
* Comments       :
*     used to get the buffer to store the data transferred from device.
*
*END*--------------------------------------------------------------------*/
uint32_t kbd_hid_get_buffer()
{
    uint8_t ep_no;;
    usb_device_ep_struct_t *lpEp;
    /* first get the max packet size from interface info */
    usb_device_interface_struct_t* lpHostIntf = (usb_device_interface_struct_t*)kbd_hid_get_interface();
    for (ep_no = 0; ep_no < lpHostIntf->ep_count; ep_no++)
    {
        lpEp = &lpHostIntf->ep[ep_no];
        if (((lpEp->lpEndpointDesc->bEndpointAddress & IN_ENDPOINT) != 0) && ((lpEp->lpEndpointDesc->bmAttributes & IRRPT_ENDPOINT) != 0))
        {
            kbd_size = (USB_SHORT_UNALIGNED_LE_TO_HOST(lpEp->lpEndpointDesc->wMaxPacketSize) & PACKET_SIZE_MASK);
            break;
        }
    }

    if ((kbd_size != 0) && (kbd_buffer == NULL))
    {
        kbd_buffer = (uint8_t*)OS_Mem_alloc_uncached_zero(kbd_size);
        if (kbd_buffer == NULL)
        {
            USB_PRINTF("allocate memory failed in hid_get_buffer\r\n");
            return (uint32_t)(-1);
        }
    }
    return 0;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : kbd_hid_get_buffer
* Returned Value : none
* Comments       :
*     Find and parse the HID descriptor, then try to get the report descriptor.
*
*END*--------------------------------------------------------------------*/
void kbd_hid_get_report_descriptor()
{
    usb_status status = USB_OK;
    usb_device_interface_struct_t* interfaceDescriptor = (usb_device_interface_struct_t*)kbd_hid_get_interface();
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
        hid_descriptor_struct_t* hidDescriptor = ptr1.pntr;
        hid_class_descriptor_struct_t* hidClassDescriptor = (hid_class_descriptor_struct_t*)&(hidDescriptor->bclassdescriptortype);
        uint8_t index;
        kbd_reportLength = 0;
        for (index = 0; index < hidDescriptor->bnumdescriptor; index ++)
        {
            hid_class_descriptor_struct_t* temp;
            temp = hidClassDescriptor + index;
            if (temp->classdescriptortype == USB_DESC_TYPE_REPORT)
            {
                kbd_reportLength = USB_SHORT_UNALIGNED_LE_TO_HOST(temp->descriptorlength);
                break;
            }
        }
        if (kbd_reportLength != 0)
        {
            
            if (kbd_reportDescriptor != NULL)
            {
                OS_Mem_free(kbd_reportDescriptor);
            }
            kbd_reportDescriptor = (uint8_t*)OS_Mem_alloc_uncached_zero(kbd_reportLength);
            if (kbd_reportDescriptor == NULL)
            {
                USB_PRINTF("allocate memory failed in hid_get_buffer\r\n");
                return ;
            }

            update_state(USB_DEVICE_GET_REPORT_DESCRIPTOR);
            kbd_hid_com->class_ptr = kbd_hid_device.CLASS_HANDLE;
            kbd_hid_com->callback_fn = usb_host_hid_keyboard_ctrl_callback;
            kbd_hid_com->callback_param = 0;
            
            status = usb_class_hid_get_descriptor(kbd_hid_com, (uint8_t)USB_DESC_TYPE_REPORT, kbd_reportDescriptor, kbd_reportLength);
         
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
    return ;
}






/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hid_keyboard_event
* Returned Value : None
* Comments       :
*     Called when HID device has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_hid_keyboard_event
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,
    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,
    /* [IN] code number for event causing callback */
    uint32_t event_code
)
{
    usb_device_interface_struct_t* pHostIntf = (usb_device_interface_struct_t*)intf_handle;
    interface_descriptor_t* intf_ptr = pHostIntf->lpinterfaceDesc;

    switch (event_code)
    {
        case USB_ATTACH_EVENT:
            kbd_interface_info[kbd_interface_number] = pHostIntf;
            kbd_interface_number++;
            USB_PRINTF("----- Attach Event -----\r\n");
            USB_PRINTF("State = %d", kbd_hid_device.DEV_STATE);
            USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
            USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
            USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
            USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
            USB_PRINTF("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
            break;
        case USB_CONFIG_EVENT:
            if(kbd_hid_device.DEV_STATE == USB_DEVICE_IDLE)
            {          
                if (kbd_hid_device.STATE_MUTEX == NULL)
                {
                    kbd_hid_device.STATE_MUTEX = OS_Mutex_create();
                    if (kbd_hid_device.STATE_MUTEX == NULL)
                        USB_PRINTF("keyboard state mutex create fail\r\n");
                }
                kbd_hid_device.DEV_HANDLE = dev_handle;
                kbd_hid_device.INTF_HANDLE = kbd_hid_get_interface();
                update_state(USB_DEVICE_ATTACHED);
            }
            else
            {
                 USB_PRINTF("HID device already attached - DEV_STATE = %d\r\n", kbd_hid_device.DEV_STATE);
            }
            break;
    
        case USB_INTF_OPENED_EVENT:
            update_state(USB_DEVICE_INTERFACE_OPENED);
            USB_PRINTF("----- Interfaced Event -----\r\n");
            break;
    
        case USB_DETACH_EVENT:
            /* Use only the interface with desired protocol */
            USB_PRINTF("\r\n----- Detach Event -----\r\n");
            USB_PRINTF("State = %d", kbd_hid_device.DEV_STATE);
            USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
            USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
            USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
            USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
            USB_PRINTF("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
            kbd_interface_number = 0;
            update_state(USB_DEVICE_DETACHED);
            break;
        default:
            USB_PRINTF("HID Device state = %d??\r\n", kbd_hid_device.DEV_STATE);
            update_state(USB_DEVICE_IDLE);
            break;
    }

    /* notify application that status has changed */
    OS_Event_set(kbd_usb_event, USB_EVENT_CTRL);
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hid_keyboard_ctrl_callback
* Returned Value : None
* Comments       :
*     Called when a control pipe command is completed. 
*
*END*--------------------------------------------------------------------*/
void usb_host_hid_keyboard_ctrl_callback
(
      /* [IN] no used */
      void*             unused,

      /* [IN] user-defined parameter */
      void*             user_parm,

      /* [IN] buffer address */
      uint8_t *         buffer,

      /* [IN] length of data transferred */
      uint32_t          buflen,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      usb_status        status
)
{
    if (status == USBERR_ENDPOINT_STALLED)
    {
        USB_PRINTF("\r\nHID control Request failed DEV_STATE 0x%x status 0x%x!\r\n", kbd_hid_device.DEV_STATE, status);
    }
    else if (status)
    {
        USB_PRINTF("\r\nHID control Request failed DEV_STATE 0x%x status 0x%x!\r\n", kbd_hid_device.DEV_STATE, status);
    }

    if(kbd_hid_device.DEV_STATE == USB_DEVICE_SETTING_PROTOCOL)
    {
        update_state(USB_DEVICE_INUSE);
        USB_PRINTF("setting protocol done\r\n");
    }
    else if (kbd_hid_device.DEV_STATE == USB_DEVICE_GET_REPORT_DESCRIPTOR)
    {
        update_state(USB_DEVICE_GET_REPORT_DESCRIPTOR_DONE);
        USB_PRINTF("get report descriptor done\r\n");
    }

    /* notify application that status has changed */
    OS_Event_set(kbd_usb_event, USB_EVENT_CTRL);
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hid_keyboard_recv_callback
* Returned Value : None
* Comments       :
*     Called when a interrupt pipe transfer is completed.
*
*END*--------------------------------------------------------------------*/

void usb_host_hid_keyboard_recv_callback
(
      /* [IN] not used */
      void*             unused,

      /* [IN] user-defined parameter */
      void*             user_parm,

      /* [IN] buffer address */
      uint8_t *         buffer,

      /* [IN] length of data transferred */
      uint32_t          buflen,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      usb_status        status
   )
{
    if (status == USB_OK)
    {
        /* notify application that data are available */
    	OS_Event_set(kbd_usb_event, USB_EVENT_DATA);
    }
    else if (status == USBERR_TR_CANCEL)
    {
        /* notify application that data are available */
        //USB_PRINTF("tr cancel\n");
    	OS_Event_set(kbd_usb_event, USB_EVENT_DATA_CORRUPTED);
    }
    else
    {
        //USB_PRINTF("tr completed 0x%x\n", status);
        OS_Event_set(kbd_usb_event, USB_EVENT_DATA_CORRUPTED);
    }
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : process_kbd_buffer
* Returned Value : None
* Comments       :
*      The way keyboard works is that it sends reports of 8 bytes of data
*      every time keys are pressed. However, it reports all the keys
*      that are pressed in a single report. The following code should
*      really be implemented by a user in the way he would like it to be.
*END*--------------------------------------------------------------------*/

void process_kbd_buffer(uint8_t *buffer)
{
    /* a little array in which we count how long each key is pressed */
    static unsigned char special_last;

    uint32_t i, shift = 0;
    uint8_t code;

    /* The first byte in buffer gives special key status.
     ** Process only the keys which are newly pressed. */
    code = (buffer[0] ^ special_last) & buffer[0];
    special_last = buffer[0];

    shift = 0;
    /* Check Modifiers in byte 0 (see HID specification 1.11 page 56) */
    if(code & 0x01) {
        USB_PRINTF("LEFT CTRL ");
    }
    if(code & 0x02) {
        USB_PRINTF("LEFT SHIFT ");
    }
    if(code & 0x04) {
        USB_PRINTF("LEFT ALT ");
    }
    if(code & 0x08) {
        USB_PRINTF("LEFT GUI ");
    }
    if(code & 0x10) {
        USB_PRINTF("RIGHT CTRL ");
    }
    if(code & 0x20) {
        USB_PRINTF("RIGHT SHIFT ");
    }
    if(code & 0x40) {
        USB_PRINTF("RIGHT ALT ");
    }
    if(code & 0x80) {
        USB_PRINTF("RIGHT GUI ");
    }

    /* immediate state of left or right SHIFT */
    if(buffer[0] & 0x22) {
        shift = 1;
    }

    /* Byte 1 is reserved (HID specification 1.11 page 60) */

    /*
     ** Build initial press-map by checking Keybcodes in bytes 2 to 7 
     ** (HID spec 1.11 page 60)
     */
    for(i = 8 - 1; i >= 2; i--) {
        if((buffer[2] != buffer[3]) && (buffer[3] != 0))
            {
               break;
            }
        else
            {
        code = buffer[i];

        /* if valid keyboard code was received */
        if(code > 1) {
            if(code < MAX_SUPPORTED_USAGE_ID && HID_table[code][shift]) {
                switch (code) {
                    case 0x29:
                      USB_PRINTF("ESC ");
                      break;
                    case 0x2A:
                      USB_PRINTF("BACKSPACE ");
                      break;
                    default:
                      USB_PRINTF("%c",HID_table[code][shift]);
                }
            }
            else {
                USB_PRINTF("\\x%02x", code);
            }

            /* only print the newest key */
            break;
        }
    }
}
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : Keyboard_Task
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/
void Keyboard_Task(void* param)
{
	usb_status              status = USB_OK;
    static uint8_t          i = 0;
    usb_host_handle        kbd_host_handle = (usb_host_handle) param;

    // Wait for insertion or removal event
    OS_Event_wait(kbd_usb_event, USB_EVENT_CTRL | USB_EVENT_DATA | USB_EVENT_DATA_CORRUPTED, FALSE, 0);
	if (OS_Event_check_bit(kbd_usb_event, USB_EVENT_CTRL))
        OS_Event_clear(kbd_usb_event, USB_EVENT_CTRL);
	
    switch ( kbd_hid_device.DEV_STATE )
    {
        case USB_DEVICE_IDLE:
            break;

        case USB_DEVICE_ATTACHED:
            USB_PRINTF("\r\nKeyboard device attached\r\n");
            update_state(USB_DEVICE_SET_INTERFACE_STARTED);
            status = usb_host_open_dev_interface(kbd_host_handle, kbd_hid_device.DEV_HANDLE, kbd_hid_device.INTF_HANDLE, (class_handle*)&kbd_hid_device.CLASS_HANDLE);
            if (status != USB_OK)
            {
                USB_PRINTF("\r\nError in _usb_hostdev_open_interface: %x\r\n", status);
                return;
            }
            break;

        case USB_DEVICE_INTERFACE_OPENED:
            kbd_hid_get_report_descriptor();
            break;

        case USB_DEVICE_GET_REPORT_DESCRIPTOR_DONE:
            kbd_hid_get_buffer();
            USB_PRINTF("Keyboard interfaced, setting protocol...\r\n");
            /* now we will set the USB Hid standard boot protocol */
            update_state(USB_DEVICE_SETTING_PROTOCOL);
        
            kbd_hid_com->class_ptr = kbd_hid_device.CLASS_HANDLE;
            kbd_hid_com->callback_fn = usb_host_hid_keyboard_ctrl_callback;
            kbd_hid_com->callback_param = 0;
        
            status = usb_class_hid_set_protocol(kbd_hid_com, USB_PROTOCOL_HID_KEYBOARD);
     
            if (status != USB_OK)
            {
                USB_PRINTF("\r\nError in usb_class_hid_set_protocol: %x\r\n", status);
            }
            break;

        case USB_DEVICE_SET_INTERFACE_STARTED:
            break;

        case USB_DEVICE_INUSE:
            //USB_PRINTF("%d\n", data_received);
            i++;
            if (kbd_data_received)
            {
                /******************************************************************
                            Initiate a transfer request on the interrupt pipe
                            ******************************************************************/                                          
                kbd_hid_com->class_ptr = kbd_hid_device.CLASS_HANDLE;
                kbd_hid_com->callback_fn = usb_host_hid_keyboard_recv_callback;
                kbd_hid_com->callback_param = 0;
                if (i==4)
                {
                    sleep_test = TRUE;
                }

                status = usb_class_hid_recv_data(kbd_hid_com, kbd_buffer, kbd_size);
                if (sleep_test)
                    sleep_test = FALSE;
                if (status != USB_OK)
                {
                    USB_PRINTF("\r\nError in usb_class_hid_recv_data: %x", status);
                }
                else
                {
                    //USB_PRINTF("try to get recv data\n");
                    kbd_data_received = 0;
                }

            }
                /* Wait untill we get the data from keyboard. */

            if(OS_Event_wait(kbd_usb_event, USB_EVENT_CTRL | USB_EVENT_DATA | USB_EVENT_DATA_CORRUPTED, FALSE, 0) == OS_EVENT_OK)
            {
                if (OS_Event_check_bit(kbd_usb_event, USB_EVENT_DATA))
                {
                    //USB_PRINTF("get data\n");
            	    OS_Event_clear(kbd_usb_event, USB_EVENT_DATA);
            	    kbd_data_received = 1;
                    if(kbd_hid_device.DEV_STATE == USB_DEVICE_INUSE)
                    {
                        process_kbd_buffer((uint8_t *)kbd_buffer);
                    }
                    status = usb_class_hid_recv_data(kbd_hid_com, kbd_buffer, kbd_size);
                    if (status != USB_OK)
                    {
                         USB_PRINTF("\r\nError in usb_class_hid_recv_data: %x", status);
                    }
                    else
                    {
                        //USB_PRINTF("try to get recv data\n");
                        kbd_data_received = 0;
                    }
                }
            
                if (OS_Event_check_bit(kbd_usb_event, USB_EVENT_DATA_CORRUPTED))
                {
                    //USB_PRINTF("get corrupted\n");
                	kbd_data_received = 1;
                    OS_Event_clear(kbd_usb_event, USB_EVENT_DATA_CORRUPTED);              
                    status = usb_class_hid_recv_data(kbd_hid_com, kbd_buffer, kbd_size);
                    if (status != USB_OK)
                    {
                         USB_PRINTF("\r\nError in usb_class_hid_recv_data: %x", status);
                    }
                    else
                    {
                        //USB_PRINTF("try to get recv data\n");
                        kbd_data_received = 0;
                    }
                }
            
                if (OS_Event_check_bit(kbd_usb_event, USB_EVENT_CTRL))
                {
                    //USB_PRINTF("get control\n");
            	    //OS_Event_set(USB_Event, USB_EVENT_CTRL);
                }
            }

            break;
            
            
        case USB_DEVICE_DETACHED:
            if (kbd_data_received == 1)
            {
                status = usb_host_close_dev_interface(kbd_host_handle, kbd_hid_device.DEV_HANDLE, kbd_hid_device.INTF_HANDLE, kbd_hid_device.CLASS_HANDLE);
                if (status != USB_OK)
                {
                    USB_PRINTF("error in _usb_hostdev_close_interface %x\r\n", status);
                }
                kbd_hid_device.INTF_HANDLE = NULL;
                kbd_hid_device.CLASS_HANDLE = NULL;
                USB_PRINTF("Going to idle state\r\n");
                update_state(USB_DEVICE_IDLE);          
                if (kbd_hid_device.STATE_MUTEX != NULL)
                {
                    OS_Mutex_destroy(kbd_hid_device.STATE_MUTEX);
                    kbd_hid_device.STATE_MUTEX = NULL;
                }
				if (kbd_reportDescriptor != NULL)
				{
					OS_Mem_free(kbd_reportDescriptor);
					kbd_reportDescriptor = NULL;
				}
				if (kbd_buffer != NULL)
				{
					OS_Mem_free(kbd_buffer);
					kbd_buffer = NULL;
				}

            }
            else
            {
                if(OS_Event_wait(kbd_usb_event, USB_EVENT_CTRL | USB_EVENT_DATA | USB_EVENT_DATA_CORRUPTED, FALSE, 0) == OS_EVENT_OK)
                {
                    if (OS_Event_check_bit(kbd_usb_event, USB_EVENT_DATA))
                    {
                        //USB_PRINTF("get data\n");
                	    OS_Event_clear(kbd_usb_event, USB_EVENT_DATA);
                	    kbd_data_received = 1;
                        if(kbd_hid_device.DEV_STATE == USB_DEVICE_INUSE)
                        {
                            process_kbd_buffer((uint8_t *)kbd_buffer);
                            OS_Mem_zero(kbd_buffer,kbd_size);
                        }
                    }
                
                    if (OS_Event_check_bit(kbd_usb_event, USB_EVENT_DATA_CORRUPTED))
                    {
                        //USB_PRINTF("get corrupted\n");
                    	 kbd_data_received = 1;
                         OS_Event_clear(kbd_usb_event, USB_EVENT_DATA_CORRUPTED);              
                    }
                
                    if (OS_Event_check_bit(kbd_usb_event, USB_EVENT_CTRL))
                    {
                        //USB_PRINTF("get control\n");
                	    //OS_Event_set(USB_Event, USB_EVENT_CTRL);
                    }
                }
                OS_Event_set(kbd_usb_event, USB_EVENT_CTRL);
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
void Keyboard_Task_Stun(void* param)
{
    while(1)
    {
        //OS_Event_wait(kbd_usb_event, USB_EVENT_CTRL, FALSE, 0);
        Keyboard_Task(param);
    }
}

