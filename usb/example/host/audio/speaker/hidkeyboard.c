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
* $Version : 3.8.1.0$
* $Date    : Aug-9-2012$
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
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
#include "derivative.h"
#include "hidef.h"
#include "mem_util.h"
#endif
#include "usb_host_hub_sm.h"
#include "usb_host_audio.h"
#include "hidkeyboard.h"
#include "usb_host_hid.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "fsl_debug_console.h"
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)

#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif
#endif

#define KEYBOARD_EVENT_MASK (USB_Keyboard_Event_CTRL | USB_Keyboard_Event_DATA)

/*The user should change the different device audio speaker key MACRO to different number,
For EDIFIER_R18USB: MUTE_VOLUME = 4; INCREASE_VOLUME = 1; DECREASE_VOLUME = 2;
For LOGITECH_S150:  MUTE_VOLUME = 1; INCREASE_VOLUME = 2; DECREASE_VOLUME = 4;
For LOGITECH_Z305:  MUTE_VOLUME = 1; INCREASE_VOLUME = 2; DECREASE_VOLUME = 4;
The default device is EDIFIER_R18USB*/
#define MUTE_VOLUME       (4)
#define INCREASE_VOLUME   (1)
#define DECREASE_VOLUME   (2)
/***************************************
**
** Globals
*/
extern os_event_handle   usb_keyboard_event;
hid_command_t*         g_hid_com;

device_struct_t g_kbd_hid_device = { 0 };
extern void keyboard_task(uint32_t param);
void process_kbd_buffer(unsigned char *buffer);
uint8_t                 g_interface_number = 0;
//uint8_t                 g_interface_keyboard_number = 0;

usb_device_interface_struct_t*   g_interface_info[USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];
usb_interface_descriptor_handle hid_get_interface()
{
    return (usb_interface_descriptor_handle)(g_interface_info[0]);
}
static uint8_t keyboard_inited = 0;
extern usb_host_handle        host_handle;
extern audio_control_device_struct_t      audio_stream; 
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : main (Main_Task if using MQX)
* Returned Value : none
* Comments       :
*     Execution starts here
*
*END*--------------------------------------------------------------------*/
void keyboard_task(uint32_t param)
{
    usb_status status = USB_OK;
    static unsigned char *buffer;

    if(keyboard_inited == 0) 
    {
        g_hid_com = (hid_command_t*) OS_Mem_alloc_zero(sizeof(hid_command_t));
        buffer = (unsigned char *)OS_Mem_alloc_uncached(HID_KEYBOARD_BUFFER_SIZE);
        if (buffer == NULL) {
            USB_PRINTF("\r\nMemory allocation failed. STATUS: %x", status);
            //fflush(stdout);
            return;
        }
        keyboard_inited = 1;
    }

    if (OS_Event_check_bit(usb_keyboard_event, USB_Keyboard_Event_CTRL))
    {
        OS_Event_clear(usb_keyboard_event, USB_Keyboard_Event_CTRL);
        switch (g_kbd_hid_device.dev_state) {
            case USB_DEVICE_CONFIGURED:
                status = usb_host_open_dev_interface(host_handle, g_kbd_hid_device.dev_handle, g_kbd_hid_device.intf_handle, (void *) & g_kbd_hid_device.class_handle);
        //g_interface_keyboard_number++;
                if(status != USB_OK) {
                USB_PRINTF("\r\nError in _usb_hostdev_select_interface: %x", status);
                //fflush(stdout);
                return;
                }
                break;
            case USB_DEVICE_IDLE:
        //g_interface_keyboard_number = 0;
                status = usb_host_close_dev_interface(host_handle, g_kbd_hid_device.dev_handle, g_kbd_hid_device.intf_handle,  g_kbd_hid_device.class_handle);
            
        if (status != USB_OK)
                {
                  USB_PRINTF("error in _usb_hostdev_close_interface %x\r\n", status);
                }
                g_kbd_hid_device.dev_handle = NULL;
                g_kbd_hid_device.intf_handle = NULL;
                break;
            case USB_DEVICE_INTERFACED:
                g_hid_com->class_ptr = g_kbd_hid_device.class_handle;
                g_hid_com->callback_fn = usb_host_hid_keyboard_recv_callback;
                g_hid_com->callback_param = 0;
                OS_Event_clear(usb_keyboard_event, USB_Keyboard_Event_DATA);
                status = usb_class_hid_recv_data(g_hid_com, (unsigned char *) buffer, HID_KEYBOARD_BUFFER_SIZE);
                if(status != USB_OK) {
                    USB_PRINTF("\r\nError in _usb_host_recv_data: %x", status);
                //fflush(stdout);
                }
                
                break;
        }
    }


    if (OS_Event_check_bit(usb_keyboard_event, USB_Keyboard_Event_DATA))
    {
        OS_Event_clear(usb_keyboard_event, USB_Keyboard_Event_DATA);
        if (g_kbd_hid_device.dev_state == USB_DEVICE_INTERFACED)
        {
            if(audio_stream.dev_state != USB_DEVICE_INTERFACED)
            {
               process_kbd_buffer((unsigned char *)buffer);
            }
            g_hid_com->class_ptr = g_kbd_hid_device.class_handle;
            g_hid_com->callback_fn = usb_host_hid_keyboard_recv_callback;
            g_hid_com->callback_param = 0;
            //USB_PRINTF("\r\nClass handle %x",  g_kbd_hid_device.class_handle);
            OS_Event_clear(usb_keyboard_event, USB_Keyboard_Event_DATA);
            status = usb_class_hid_recv_data(g_hid_com, (unsigned char *) buffer, HID_KEYBOARD_BUFFER_SIZE);
            if(status != USB_OK) {
                USB_PRINTF("\r\nError in _usb_host_recv_data: %x", status);
            }
        }
    }
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    OS_Time_delay(1);
#endif
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_task_stun
*  Returned Value : none
*  Comments       :
*        KHCI task
*END*-----------------------------------------------------------------*/
void keyboard_task_stun(uint32_t param)
{
    while(1)
    {   
        keyboard_task(param);
        OS_Event_wait(usb_keyboard_event, KEYBOARD_EVENT_MASK, FALSE, 0);
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hid_keyboard_event
* Returned Value : None
* Comments       :
*     Called when HID device has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_hid_keyboard_event(
/* [IN] pointer to device instance */
usb_device_instance_handle dev_handle,
/* [IN] pointer to interface descriptor */
usb_interface_descriptor_handle intf_handle,
/* [IN] code number for event causing callback */
uint32_t event_code)
{
    usb_device_interface_struct_t* pHostIntf = (usb_device_interface_struct_t*)intf_handle;
    interface_descriptor_t* intf_ptr = pHostIntf->lpinterfaceDesc;
     
    //fflush(stdout);
    switch (event_code) {
        case USB_ATTACH_EVENT:
            g_interface_info[g_interface_number] = pHostIntf;
            g_interface_number++;
            
            USB_PRINTF("HID State = %d", g_kbd_hid_device.dev_state);
            USB_PRINTF("HID  Class = %d", intf_ptr->bInterfaceClass);
            USB_PRINTF("HID  SubClass = %d", intf_ptr->bInterfaceSubClass);
            USB_PRINTF("HID  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
            break;
            /* Drop through config event for the same processing */
        case USB_CONFIG_EVENT:
            //fflush(stdout);
            USB_PRINTF("HID State = USB_CONFIG_EVENT\r\n");
            
            if(g_kbd_hid_device.dev_state == USB_DEVICE_IDLE) 
            {
                g_kbd_hid_device.dev_handle = dev_handle;
                g_kbd_hid_device.intf_handle = hid_get_interface();;
                g_kbd_hid_device.dev_state = USB_DEVICE_CONFIGURED;
            }
            else
            {
                USB_PRINTF("HID device already attached\r\n");
                //fflush(stdout);
            }
            break;
        case USB_INTF_OPENED_EVENT:
            USB_PRINTF("HID State = USB_INTF_OPENED_EVENT\r\n");
            g_kbd_hid_device.dev_state = USB_DEVICE_INTERFACED;
            OS_Event_set(usb_keyboard_event, USB_Keyboard_Event_CTRL);
            break;
          
        case USB_DETACH_EVENT:
            /* Use only the interface with desired protocol */
            g_interface_number = 0;
            if (g_kbd_hid_device.dev_state < USB_DEVICE_INTERFACED)
            {
                g_kbd_hid_device.dev_handle = NULL;
                g_kbd_hid_device.intf_handle = NULL;
                g_kbd_hid_device.dev_state = USB_DEVICE_IDLE;
                return;
            }
            USB_PRINTF("\r\n----- Detach Event -----\r\n");
            USB_PRINTF("State = %d", g_kbd_hid_device.dev_state);
            USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
            USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
            USB_PRINTF("  Protocol = %d\r\n", intf_ptr->bInterfaceProtocol);
            //fflush(stdout);
            
            g_kbd_hid_device.dev_state = USB_DEVICE_IDLE;
            OS_Event_set(usb_keyboard_event, USB_Keyboard_Event_CTRL);
            break;
    }
   
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hid_ctrl_callback
* Returned Value : None
* Comments       :
*     Called when a control pipe command is completed. 
*
*END*--------------------------------------------------------------------*/
void usb_host_hid_keyboard_ctrl_callback(
/* [IN] pointer to pipe */
usb_pipe_handle pipe_handle,
/* [IN] user-defined parameter */
void *user_parm,
/* [IN] buffer address */
unsigned char *buffer,
/* [IN] length of data transferred */
uint32_t buflen,
/* [IN] status, hopefully USB_OK or USB_DONE */
uint32_t status)
{
    if(status == USBERR_ENDPOINT_STALLED) {
        USB_PRINTF("\r\nHID Set_Protocol Request BOOT is not supported!\r\n");
        //fflush(stdout);
    }
    else if(status) {
        USB_PRINTF("\r\nHID Set_Protocol Request BOOT failed!: 0x%x ... END!\r\n", status);
        //fflush(stdout);
    }

    if(g_kbd_hid_device.dev_state == USB_DEVICE_SETTING_PROTOCOL)
        g_kbd_hid_device.dev_state = USB_DEVICE_INUSE;

    /* notify application that status has changed */
    OS_Event_set(usb_keyboard_event, USB_Keyboard_Event_CTRL);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_kbd_host_hid_recv_callback
* Returned Value : None
* Comments       :
*     Called when a interrupt pipe transfer is completed.
*
*END*--------------------------------------------------------------------*/
void usb_host_hid_keyboard_recv_callback(
/* [IN] pointer to pipe */
usb_pipe_handle pipe_handle,
/* [IN] user-defined parameter */
void *user_parm,
/* [IN] buffer address */
unsigned char *buffer,
/* [IN] length of data transferred */
uint32_t buflen,
/* [IN] status, hopefully USB_OK or USB_DONE */
uint32_t status)
{
    /* notify application that data are available */
    if(!OS_Event_check_bit(usb_keyboard_event, USB_Keyboard_Event_DATA))
    {
        OS_Event_set(usb_keyboard_event, USB_Keyboard_Event_DATA);
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
void process_kbd_buffer(unsigned char *buffer)
{
   // USB_PRINTF("\nprocess_kbd_buffer :%d\n",buffer[0]);
    if(MUTE_VOLUME==buffer[0]) audio_mute_command();
    if(INCREASE_VOLUME==buffer[0]) audio_increase_volume_command(1);
    if(DECREASE_VOLUME==buffer[0]) audio_decrease_volume_command(1);
    //fflush(stdout);

}
