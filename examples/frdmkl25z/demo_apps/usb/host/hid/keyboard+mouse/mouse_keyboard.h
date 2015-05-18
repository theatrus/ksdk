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
 * $FileName: hidkeyboard.h$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file contains keyboard-application types and definitions.
 *
 *END************************************************************************/
#ifndef __mouse_keyboard_h__
#define __mouse_keyboard_h__

#ifdef __USB_OTG__
#include "otgapi.h"
#include "devapi.h"
#else
//#include "hostapi.h"
#endif

/***************************************
 **
 ** Application-specific definitions
 */

/* Used to initialize USB controller */

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
#define  USB_DEVICE_GET_REPORT_DESCRIPTOR  (10)
#define  USB_DEVICE_GET_REPORT_DESCRIPTOR_DONE (11)
#define  USB_DEVICE_SET_IDLE               (12)
#define  USB_DEVICE_SETTING_IDLE           (13)

#define  HIGH_SPEED                        (0)

#if HIGH_SPEED
#define CONTROLLER_ID                      USB_CONTROLLER_EHCI_0
#else
#define CONTROLLER_ID                      USB_CONTROLLER_KHCI_0
#endif

#define KB_CONTINUE_PRESS                   50 /* KB_CONTINUE_PRESS * 10ms */
#define USB_EVEN_INIT            (0x10)
/*
 ** Following structs contain all states and pointers
 ** used by the application to control/operate devices.
 */

typedef struct device_struct
{
    uint32_t DEV_STATE; /* Attach/detach state */
    usb_device_instance_handle DEV_HANDLE;
    usb_interface_descriptor_handle INTF_HANDLE;
    usb_class_handle CLASS_HANDLE; /* Class-specific info */
    os_mutex_handle STATE_MUTEX;
} device_struct_t;

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C"
{
#endif

    void usb_host_hid_keyboard_ctrl_callback(void* unused, void* user_parm, uint8_t *buffer, uint32_t buflen, usb_status status);
    void usb_host_hid_keyboard_recv_callback(void* unused, void* user_parm, uint8_t *buffer, uint32_t buflen, usb_status status);
    usb_status usb_host_hid_keyboard_event(usb_device_instance_handle dev_handle, usb_interface_descriptor_handle intf_handle, uint32_t event_code);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
