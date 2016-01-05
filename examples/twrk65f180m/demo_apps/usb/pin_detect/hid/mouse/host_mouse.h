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
* $FileName: host_mouse.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*
*END************************************************************************/
#ifndef __HOST_MOUSE_H__
#define __HOST_MOUSE_H__

/***************************************
**
** Application-specific definitions
*/

#define  USB_DEVICE_IDLE                        (0)
#define  USB_DEVICE_ATTACHED                    (1)
#define  USB_DEVICE_CONFIGURED                  (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED       (3)
#define  USB_DEVICE_INTERFACE_OPENED            (4)
#define  USB_DEVICE_SETTING_PROTOCOL            (5)
#define  USB_DEVICE_INUSE                       (6)
#define  USB_DEVICE_DETACHED                    (7)
#define  USB_DEVICE_OTHER                       (8)
#define  USB_DEVICE_INTERFACE_CLOSED            (9)
#define  USB_DEVICE_GET_REPORT_DESCRIPTOR       (10)
#define  USB_DEVICE_GET_REPORT_DESCRIPTOR_DONE  (11)
#define  USB_DEVICE_SET_IDLE               (12)
#define  USB_DEVICE_SETTING_IDLE           (13)

/*
** Following structs contain all states and pointers
** used by the application to control/operate devices.
*/

typedef struct device_struct {
    uint32_t                        dev_state;  /* Attach/detach state */
    usb_device_instance_handle      dev_handle;
    usb_interface_descriptor_handle intf_handle;
    usb_class_handle                    class_handle; /* Class-specific info */
} device_struct_t;


/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

void usb_host_hid_recv_callback(usb_pipe_handle, void*, uint8_t *, uint32_t, usb_status);
void usb_host_hid_ctrl_callback(usb_pipe_handle, void*, uint8_t *, uint32_t, usb_status);

#ifdef __cplusplus
}
#endif
#endif

/* EOF */
