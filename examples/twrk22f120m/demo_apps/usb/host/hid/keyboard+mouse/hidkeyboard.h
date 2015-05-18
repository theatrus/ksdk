/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 Freescale Semiconductor;
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
#ifndef __hidkeyboard_h__
#define __hidkeyboard_h__

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

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif
void usb_host_hid_keyboard_ctrl_callback(void* unused, void* user_parm, uint8_t *buffer, uint32_t buflen, usb_status status);
void usb_host_hid_keyboard_recv_callback(void* unused, void* user_parm, uint8_t *buffer, uint32_t buflen, usb_status status);
usb_status usb_host_hid_keyboard_event(usb_device_instance_handle dev_handle, usb_interface_descriptor_handle intf_handle, uint32_t event_code);
#ifdef __cplusplus
}
#endif


#endif

/* EOF */
