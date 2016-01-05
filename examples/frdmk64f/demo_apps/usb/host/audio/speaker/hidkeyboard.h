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
 * $Version : 3.8.1.0$
 * $Date    : Aug-9-2012$
 *
 * Comments:
 *
 *   This file contains keyboard-application types and definitions.
 *
 *END************************************************************************/

#ifndef __hidkeyboard_h__
#define __hidkeyboard_h__

#include "audio_speaker.h"

/***************************************
 **
 ** Application-specific definitions
 */

/* Used to initialize USB controller */
#define MAX_FRAME_SIZE           1024

#define  HID_KEYBOARD_BUFFER_SIZE    1

#define USB_Keyboard_Event_CTRL 0x01
#define USB_Keyboard_Event_DATA 0x02

/*
 ** Following structs contain all states and pointers
 ** used by the application to control/operate devices.
 */
typedef struct device_struct
{
    uint32_t dev_state; /* attach/detach state */
    usb_device_instance_handle dev_handle;
    usb_interface_descriptor_handle intf_handle;
    usb_class_handle class_handle; /* class-specific info */
} device_struct_t;

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C"
{
#endif

    void usb_host_hid_keyboard_recv_callback(usb_pipe_handle, void *, unsigned char *, uint32_t,
        uint32_t);
    void usb_host_hid_keyboard_ctrl_callback(usb_pipe_handle, void *, unsigned char *, uint32_t,
        uint32_t);
    usb_status usb_host_hid_keyboard_event(usb_device_instance_handle,
        usb_interface_descriptor_handle, uint32_t);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
