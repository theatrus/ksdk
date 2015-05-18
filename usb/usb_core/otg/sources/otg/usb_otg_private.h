/**HEADER********************************************************************
*
* Copyright (c) 2010, 2013 - 2015 Freescale Semiconductor;
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
* $FileName: usb_otg_private.h$
* $Version : 
* $Date    : 
*
* Comments : This is the header file for the OTG driver
*
*         
*****************************************************************************/
#ifndef USB_OTG_PRIVATE_H_
#define USB_OTG_PRIVATE_H_

#include "usb.h"
#include "usb_otg_main.h"

/* Public constants */
enum{ srp_not_started , srp_se0 , srp_dp_puls};
#define OTG_STATUS_BM_REQ_TYPE          0x80
#define OTG_STATUS_SELECTOR             0xF000
#define  USB_OTG_HOST_REQUEST_FLAG      (0x01)
#define OTG_CTRL_PDOWN_DP               ((uint8_t)0x01)
#define OTG_CTRL_PDOWN_DM               ((uint8_t)0x02)
#define OTG_ATTR_SRP_SUPPORT            ((uint8_t)0x01)
#define OTG_ATTR_HNP_SUPPORT            ((uint8_t)0x02)

#ifdef __GNUC__
  #pragma pack(push)
  #pragma pack(1)
#endif
typedef struct
{
    uint8_t              id;                    /* Current ID state */
    uint8_t              vbus_valid;            /* V_BUS_VALID status */
    uint8_t              sess_valid;            /* SESS_VALID status */
    uint8_t              sess_end;              /* SESS_END status */
    uint16_t             ms_since_line_changed;
    uint16_t             host_req_poll_timer;
    uint8_t              line_stable;
    uint8_t              tmr_1ms;
    uint8_t              live_se0;
    uint8_t              live_jstate;
    uint8_t              srp_support;          /* Session Request Protocol */
    uint8_t              hnp_support;          /* Host Negotiation Protocol */

    uint8_t              b_timeout_en;
    uint16_t             b_timeout;            /* SRP detect timeout*/
    uint8_t              a_conn;               /* A-device connected */
    uint8_t              a_bus_drop;           /* determines A state machine behavior */
    uint8_t              a_bus_req;            /* determines A state machine behavior */
    uint8_t              a_clr_err;            /* setting this to TRUE is one way to escape from the a_vbus_err state */
    uint8_t              b_conn;               /* B-device connected */
    uint16_t             b_conn_dbnc_time;     /* debounce time */
    uint8_t              a_set_b_hnp_en;       /* HNP status */
    uint8_t              a_srp_det;            /* SRP pulls detected */
    uint8_t              a_srp_det_state;
    uint16_t             a_srp_pulse_duration;
    bool                 hnp_req;              /* B device HNP request */
    uint32_t             active_stack;
} usb_otg_status_t;
typedef struct usb_otg_callback_functions_struct
{
   /* The Host/Device pre-init function */
   usb_status (_CODE_PTR_ otg_preinit)(usb_otg_handle* handle);
   /* The Host/Device init function */
   usb_status (_CODE_PTR_ otg_init)(uint8_t controller_id, usb_otg_handle handle);
   /* The function to shutdown the host/device */
   usb_status (_CODE_PTR_ otg_shutdown)(usb_otg_handle handle);
   /* The function to send data */
   usb_status (_CODE_PTR_ otg_get_status)(usb_otg_handle handle);
   /* The function to send setup data */
   usb_status (_CODE_PTR_ otg_set_vbus)(usb_otg_handle handle, bool enable);
   /* The function to receive data */
   usb_status (_CODE_PTR_ otg_set_pulldowns)(usb_otg_handle handle,  uint8_t bitfield);
   /* The function to cancel the transfer */
   usb_status (_CODE_PTR_ otg_set_dp_pullup)(usb_otg_handle handle, bool enable);
   /* The function for USB bus control */
   usb_status (_CODE_PTR_ otg_generate_resume)(usb_otg_handle handle, bool enable);
} usb_otg_api_functions_struct_t;
/* Public types */

typedef struct usb_otg_state_struct
{
    const usb_otg_api_functions_struct_t *    otg_controller_api;
#ifdef __CC_ARM
    uint8_t                                 reserve1[3];  
#endif
    uint8_t                                 device_state;     /* Current device state (A or B) */
#ifdef __CC_ARM
    uint8_t                                 reserve2[3];
#endif
    uint8_t                                 sub_state;        /* Current SM sub-state */
#ifdef __CC_ARM
    uint8_t                                 reserve3[3];  
#endif
    uint8_t                                 srp_request;        /* SRP was requested by application (B device) */
#ifdef __CC_ARM
    uint8_t                                 reserve4[3];
#endif
    uint8_t                                 bus_request;        /* HNP was requested by application (B device) */
    uint8_t                                 bus_release;        /* USB bus released (B device) */
    uint8_t                                 power_up;
    uint8_t                                 hnp_enabled;
    usb_otg_status_t                        otg_status;
    usb_device_handle                       dev_inst_ptr;
    os_event_handle                         otg_isr_event;
    os_event_handle                         otg_app_event;     /* The app event signaled internally. 
                                                                  Bases on this, the application callback is called */
    const otg_int_struct_t                  *init_struct;      /* Application initialization structure containing also external circuit access functions */
    void*                                   init_param;
    uint32_t                                otg_task_id;
    uint32_t                                usbRegBase;
} usb_otg_state_struct_t;
/* Public definitions */
/* otg callback function call macros */
#define _usb_otg_callback_get_status(otg_handle)                ((usb_otg_state_struct_t *)otg_handle)->otg_controller_api->otg_get_status(otg_handle)
#define _usb_otg_callback_set_vbus(otg_handle, enable)          ((usb_otg_state_struct_t *)otg_handle)->otg_controller_api->otg_set_vbus(otg_handle, enable) 
#define _usb_otg_callback_set_pull_downs(otg_handle, bitfield)  ((usb_otg_state_struct_t *)otg_handle)->otg_controller_api->otg_set_pulldowns(otg_handle, bitfield)
#define _usb_otg_callback_set_dp_pull_up(otg_handle, enable)    ((usb_otg_state_struct_t *)otg_handle)->otg_controller_api->otg_set_dp_pullup(otg_handle, enable)
#define _usb_otg_callback_generate_resume(otg_handle, enable)   ((usb_otg_state_struct_t *)otg_handle)->otg_controller_api->otg_generate_resume(otg_handle, enable)
/* Public functions */

extern void _usb_otg_dp_pullup_enable(usb_otg_handle otg_handle, bool enable);

#ifdef __GNUC__
  #pragma pack(pop)
#endif
#endif /* USB_OTG_PRIVATE_H_ */
