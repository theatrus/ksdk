/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: usb_host_hub_sm.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file defines structures used by the implementation of hub SM on host.
*
*END************************************************************************/
#ifndef __usb_host_hub_sm_h__
#define __usb_host_hub_sm_h__

/*
** Following structs contain all states and pointers
** used by the application to control/operate devices.
*/
#define MAX_HUB_PORT_NUMBER         (7)

#define HUB_PORT_ATTACHED           (0x01) /* flag informs that application started attach process */
#define HUB_PORT_REMOVABLE          (0x02) /* flag informs that application started attach process */
#define HUB_PORT_BEGINRESET         (0x04)

typedef struct hub_device_port_struct
{
    /* status read from GetPortStatus transfer */
    uint32_t                          status;
    uint8_t                           app_status;
} hub_port_struct_t;

typedef enum
{
    HUB_IDLE,
    HUB_BEGIN_GET_DESCRIPTOR_TINY_PROCESS,
    HUB_GET_DESCRIPTOR_TINY_PROCESS,
    HUB_BEGIN_GET_DESCRIPTOR_PROCESS,
    HUB_GET_DESCRIPTOR_PROCESS,
    HUB_BEGIN_SET_PORT_FEATURE_PROCESS,
    HUB_SET_PORT_FEATURE_PROCESS,
    HUB_BEGIN_CLEAR_PORT_FEATURE_PROCESS,
    HUB_CLEAR_PORT_FEATURE_PROCESS,
    HUB_BEGIN_GET_PORT_STATUS_PROCESS,
    HUB_GET_PORT_STATUS_PROCESS,
    HUB_BEGIN_GET_PORT_STATUS_ASYNC,
    HUB_GET_PORT_STATUS_ASYNC,
    HUB_BEGIN_RESET_DEVICE_PORT_PROCESS,
    HUB_RESET_DEVICE_PORT_PROCESS,
    HUB_BEGIN_ADDRESS_DEVICE_PORT_PROCESS,
    HUB_ADDRESS_DEVICE_PORT_PROCESS,
    HUB_BEGIN_DETACH_DEVICE_PORT_PROCESS,
    HUB_DETACH_DEVICE_PORT_PROCESS,
    HUB_BEGIN_GET_STATUS_ASYNC,
    HUB_GET_STATUS_ASYNC,
    HUB_BEGIN_PORT_RESET,
    HUB_WAIT_FOR_PORT_RESET,
    HUB_TO_BE_DELETED,
    HUB_NONE
} hub_state_t;


typedef struct hub_device_struct
{
    struct hub_device_struct  *      next;             /* next device, or null */
    usb_host_handle                  host_handle; 
    usb_device_instance_handle       dev_handle;
    usb_interface_descriptor_handle  intf_handle;
    class_handle                     class_handle; /* class-specific info */
    hub_state_t                      state;  /* attach/detach state */
    /* below the hub specific data */
    os_event_handle                  hub_event; 
    int32_t                           port_iterator;
    hub_port_struct_t*               hub_ports;
    void*                            device_list_ptr;
    uint8_t*                         hub_descriptor_buffer;
    uint8_t*                         port_status_buffer;
    uint8_t*                         bit_map_buffer;
    uint8_t                          hub_level;
    uint8_t                            hub_port_nr;
    uint8_t                          to_be_deleted;
    uint8_t                          in_control;
    uint8_t                          in_recv;
    uint8_t                          opened_interface;
	uint8_t                          speed;
	uint8_t                          hs_hub_no;
    uint8_t                          hs_port_no;
} hub_device_struct_t;

#define USB_HUB_lock()                OS_Mutex_lock(((usb_host_state_struct_t*)usb_host_ptr)->hub_mutex)
#define USB_HUB_unlock()              OS_Mutex_unlock(((usb_host_state_struct_t*)usb_host_ptr)->hub_mutex)

/* List of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

void usb_host_hub_device_event(usb_device_instance_handle, usb_interface_descriptor_handle, uint32_t);

#ifdef __cplusplus
}
#endif

#endif
