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
* $FileName: usb_host.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*   
*
*END************************************************************************/
#ifndef __host_main_h__
#define __host_main_h__

#define USB_Uncached    volatile

/* Macro for aligning the EP queue head to 32 byte boundary */
//#define USB_MEM32_ALIGN(n)      ((n) + (-(n) & 31))
#define USB_MEM32_ALIGN(n)      ((n + 31) & (0xFFFFFFE0u))
//#define USB_MEM64_ALIGN(n)      ((n) + (-(n) & 63))
#define USB_MEM64_ALIGN(n)      ((n + 63) & (0xFFFFFFC0u))

#define USB_CACHE_ALIGN(n)      USB_MEM32_ALIGN(n)

#define MAX_TR_NUMBER           16
#define MAX_HOST_SERVICE_NUMBER 2


typedef struct usb_host_api_functions_struct
{
    /* The Host/Device pre-init function */
    usb_status (_CODE_PTR_ host_preinit)(usb_host_handle upper_layer_handle, usb_host_handle* handle);
 
    /* The Host/Device init function */
    usb_status (_CODE_PTR_ host_init)(uint8_t controller_id, usb_host_handle handle);
 
    /* The function to shutdown the host/device */
    usb_status (_CODE_PTR_ host_shutdown)(usb_host_handle handle);
 
    /* The function to send data */
    usb_status (_CODE_PTR_ host_send)(usb_host_handle handle, struct pipe_struct* pipe_ptr, struct tr_struct* tr_ptr);
 
    /* The function to send setup data */
    usb_status (_CODE_PTR_ host_send_setup)(usb_host_handle handle, struct pipe_struct* pipe_ptr, struct tr_struct* tr_ptr);
 
    /* The function to receive data */
    usb_status (_CODE_PTR_ host_recv)(usb_host_handle handle, struct pipe_struct* pipe_ptr, struct tr_struct* tr_ptr);
    
    /* The function to cancel the transfer */
    usb_status (_CODE_PTR_ host_cancel)(usb_host_handle handle, struct pipe_struct* pipe_ptr, struct tr_struct* tr_ptr);
    
    /* The function for USB bus control */
    usb_status (_CODE_PTR_ host_bus_control)(usb_host_handle handle, uint8_t bus_control);
 
    usb_status (_CODE_PTR_ host_alloc_bandwith)(usb_host_handle handle, struct pipe_struct* pipe_ptr);
 
    usb_status (_CODE_PTR_ host_free_controller_resource)(usb_host_handle handle, struct pipe_struct* pipe_ptr);
   
    uint32_t (_CODE_PTR_ host_get_frame_num)(usb_host_handle handle);
    
    uint32_t (_CODE_PTR_ host_get_micro_frame_num)(usb_host_handle handle);   
    
    usb_status (_CODE_PTR_ host_open_pipe)(usb_host_handle handle, usb_pipe_handle* pipe_handle_ptr, pipe_init_struct_t* pipe_init_ptr);
 
    usb_status (_CODE_PTR_ host_close_pipe)(usb_host_handle handle, usb_pipe_handle pipe_handle);
    
    usb_status (_CODE_PTR_ host_update_max_packet_size)(usb_host_handle handle, struct pipe_struct* pipe_ptr);
    
    usb_status (_CODE_PTR_ host_update_device_address)(usb_host_handle handle, struct pipe_struct* pipe_ptr);

} usb_host_api_functions_struct_t;

/* Used to manage services and service callbacks */
typedef struct usb_host_service_struct
{
    uint32_t                        type;
    void                 (_CODE_PTR_ service)(void* handle, uint32_t length);
} usb_host_service_struct_t;

typedef struct usb_host_generic_structure
{
    uint8_t                                    occupied;
    uint8_t                                    controller_id; 
    usb_host_handle                            controller_handle;
    const usb_host_api_functions_struct_t *    host_controller_api;
    event_callback                             unsupport_device_callback;
    void*                                      device_list_ptr;
 //   struct host_service_struct *               service_head_ptr;
    struct driver_info *                       device_info_table;
    os_mutex_handle                            mutex;
    os_sem_handle                              hub_sem;
    os_mutex_handle                            hub_mutex;
    void*                                      hub_link;
    void*                                      hub_handle;
    uint32_t                                   hub_task;
    tr_struct_t                                tr_list[MAX_TR_NUMBER];
    uint32_t                                   tr_index;
    void*                                      root_hub_ptr;
    usb_host_service_struct_t                  services[MAX_HOST_SERVICE_NUMBER];
    uint8_t                                    tr_user;
#ifdef USBCFG_OTG
   usb_otg_handle                              otg_handle;  
#endif 
} usb_host_state_struct_t;

#define USB_Host_lock()                OS_Mutex_lock(((usb_host_state_struct_t*)usb_host_ptr)->mutex)
#define USB_Host_unlock()              OS_Mutex_unlock(((usb_host_state_struct_t*)usb_host_ptr)->mutex)

/* List of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif
usb_status  _usb_host_unlink_tr(usb_pipe_handle pipe_handle, tr_struct_t* tr_ptr);
usb_status _usb_host_update_max_packet_size_call_interface (usb_host_handle handle, struct pipe_struct* pipe_ptr);
usb_status _usb_host_update_device_address_call_interface (usb_host_handle handle, struct pipe_struct* pipe_ptr);
usb_status _usb_host_call_service (usb_host_handle handle, uint8_t type, uint32_t length);
//usb_status _usb_host_check_service (usb_host_handle, uint8_t, uint32_t);
usb_status USB_log_error(char* file, uint32_t line, usb_status error);


#ifdef __cplusplus
}
#endif

#endif
/* EOF */
