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
* $FileName: usb_host_dev_mng.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file defines the device list for USB class drivers.
*   IMPLEMENTATION:
*   Linking list items depends on root pointers behaving like
*   a short entry containing a "next" pointer = NULL when
*   the list is empty.  So next MUST BE FIRST in the structs.
*
*END************************************************************************/
#ifndef __host_dev_list_h__
#define __host_dev_list_h__

#include "usb_host_hub_sm.h"

typedef struct _usb_device_configuration_struct
{
    usb_configuration_descriptor_t* lpconfigurationDesc;
    uint16_t                     configurationExlength;
    uint8_t *                    configurationEx;
    uint8_t                      interface_count;
    usb_device_interface_struct_t  interface[USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];
} usb_device_configuration_struct_t;

typedef struct usb_device_interface_info_struct
{
    usb_device_interface_struct_t* lphostintf;
    usb_host_driver_info_t*        lpDriverInfo;
    class_map_t*                   lpClassDriverMap;
    usb_class_handle               lpClassHandle;
    uint8_t                        open;
    uint8_t                        requesting_set_interface;
} usb_device_interface_info_struct_t;

/********************************************************************
Note that device instance structure keeps data buffers inside it. 
These buffers are passed to DMA when host does enumeration. This
means that we must ensure that buffers inside this structure
are aligned in PSP cache line size. 
********************************************************************/
/* Fixed-length fields applicable to all devices */
typedef struct _dev_instance
{
    struct _dev_instance    *next;             /* next device, or NULL */
    usb_host_handle         host;             /* host (several can exist) */
    hub_device_struct_t*    hub_instance;
    uint8_t                 speed;            /* device speed */
    uint8_t                 hub_no;           /* hub # (root hub = 0) */
    uint8_t                 port_no;          /* hub's port # (1 - 8) */
    uint8_t                 address;          /* USB address, 1 - 127 */

    uint16_t                cfg_value;
    uint8_t                 ctrl_retries;
    uint8_t                 stall_retries;
    uint8_t                 new_config;       /* non-zero = new config */

    uint8_t                 num_of_interfaces;
    uint8_t                 rerserved1[1];

    uint16_t                state;            /* device state */
    usb_pipe_handle         control_pipe;     /* control pipe handle */
    tr_callback             control_callback; /* control pipe callback */
    void*                   control_callback_param; /* control callback param */
   
    device_descriptor_t     dev_descriptor;   /* device descriptor */

    uint8_t                 rerserved2[2];

    uint8_t                 buffer[9];        /* enumeration buffer */

    uint8_t                 rerserved3[3];

    void*                   lpConfiguration;
 
    usb_device_configuration_struct_t  configuration;
    usb_device_interface_info_struct_t interface_info[USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];
    uint8_t                 attached;
    uint8_t                 pre_detached;
    uint8_t                 to_be_detached;
    uint8_t                 target_address;
    uint8_t                 level;
} dev_instance_t;

/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

extern usb_status usb_host_dev_mng_attach
(
  usb_host_handle  handle,
  hub_device_struct_t* hub_instance,
  uint8_t            speed,
  uint8_t            hub_no,
  uint8_t            port_no,
  uint8_t           level,
  usb_device_instance_handle* handle_ptr 
);
extern usb_status usb_host_dev_mng_detach(usb_host_handle handle, uint8_t hub_no, uint8_t port_no);
extern usb_status usb_host_dev_mng_pre_detach(usb_host_handle handle, uint8_t hub_no, uint8_t port_no);
extern usb_device_interface_info_struct_t* usb_host_dev_mng_get_interface_info(usb_device_instance_handle dev_handle, interface_descriptor_t* intf);
extern uint8_t usb_host_dev_mng_get_address(usb_device_instance_handle dev_handle);
extern uint8_t usb_host_dev_mng_get_hubno(usb_device_instance_handle dev_handle);
extern uint8_t usb_host_dev_mng_get_portno(usb_device_instance_handle dev_handle);
extern uint8_t usb_host_dev_mng_get_speed(usb_device_instance_handle dev_handle);
extern uint8_t usb_host_dev_mng_get_attach_state(usb_device_instance_handle dev_handle);
extern uint8_t usb_host_dev_mng_get_level(usb_device_instance_handle dev_handle);
extern uint16_t usb_host_dev_mng_get_pid(usb_device_instance_handle dev_handle);
extern uint16_t usb_host_dev_mng_get_vid(usb_device_instance_handle dev_handle);
extern usb_host_handle usb_host_dev_mng_get_host(usb_device_instance_handle dev_handle);
extern usb_pipe_handle usb_host_dev_mng_get_control_pipe(usb_device_instance_handle dev_handle);
extern bool  usb_host_dev_mng_check_configuration(usb_device_instance_handle dev_handle);
extern usb_status usb_host_dev_mng_parse_configuration_descriptor(usb_device_instance_handle dev_handle);
extern bool  usb_host_dev_notify(dev_instance_t* dev_ptr, uint32_t event_code);

extern uint8_t usb_host_dev_mng_get_hub_speed(usb_device_instance_handle dev_handle);
extern uint8_t usb_host_dev_mng_get_hs_hub_no(usb_device_instance_handle dev_handle);
extern uint8_t usb_host_dev_mng_get_hs_port_no(usb_device_instance_handle dev_handle);
extern uint8_t usb_host_dev_mng_get_hub_thinktime(usb_device_instance_handle dev_handle);
extern hub_device_struct_t* usb_host_dev_mng_get_hub_handle(usb_device_instance_handle dev_handle);
extern bool  usb_host_driver_info_match(dev_instance_t* dev_ptr, interface_descriptor_t* intf_ptr, usb_host_driver_info_t* info_ptr);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
