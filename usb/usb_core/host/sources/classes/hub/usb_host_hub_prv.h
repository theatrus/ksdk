/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013- 2014 Freescale Semiconductor;
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
* $FileName: usb_host_hub_prv.h$
* $Version : 3.8.8.0$
* $Date    : Jun-7-2012$
*
* Comments:
*
*   This file defines a structure(s) for hub class driver.
*
*END************************************************************************/
#ifndef __usb_host_hub_prv_h__
#define __usb_host_hub_prv_h__

//#include <mqx.h>

//#include "usb.h"
//#include "usb_prv.h"

#define  C_HUB_LOCAL_POWER          (0)
#define  C_HUB_OVER_CURRENT         (1)
#define  PORT_CONNECTION            (0)
#define  PORT_ENABLE                (1)
#define  PORT_SUSPEND               (2)
#define  PORT_OVER_CURRENT          (3)
#define  PORT_RESET                 (4)
#define  PORT_POWER                 (8)
#define  PORT_LOW_SPEED             (9)
#define  PORT_HIGH_SPEED            (10)
#define  C_PORT_CONNECTION          (16)
#define  C_PORT_ENABLE              (17)
#define  C_PORT_SUSPEND             (18)
#define  C_PORT_OVER_CURRENT        (19)
#define  C_PORT_RESET               (20)
#define  C_PORT_POWER               (24)
#define  PORT_TEST                  (21)
#define  PORT_INDICATOR             (22)

/* structure for HUB class descriptor */
typedef struct {
    uint8_t     blength;
    uint8_t     bdescriptortype;
    uint8_t     bnrports;
    uint8_t     whubcharacteristics[2];
    uint8_t     bpwron2pwrgood;
    uint8_t     bhubcontrcurrent;
    uint8_t     deviceremovable[];
/* not used: */
/*    uint8_t     PORTPOWERCTRLMASK; */
} hub_descriptor_struct_t;

typedef struct {
    uint16_t    whubstatus;
    uint16_t    whubchange;
} hub_status_struct_t;

/* Class specific functions exported by hub class driver */
#ifdef __cplusplus
extern "C" {
#endif

//usb_status usb_class_hub_get_app(usb_device_instance_handle, usb_interface_descriptor_handle, CLASS_CALL_STRUCT_PTR *);
usb_status usb_class_hub_cntrl_common(hub_command_t*, uint8_t, uint8_t, uint16_t, uint16_t, uint16_t, uint8_t *);
void usb_class_hub_cntrl_callback(void*, void *, uint8_t *, uint32_t, usb_status);
usb_status usb_class_hub_get_descriptor(hub_command_t*, uint8_t*, uint8_t);
usb_status usb_class_hub_set_port_feature(hub_command_t*, uint8_t, uint8_t);
usb_status usb_class_hub_clear_feature(hub_command_t*, uint8_t);
usb_status usb_class_hub_clear_port_feature(hub_command_t*, uint8_t, uint8_t);
usb_status usb_class_hub_get_status(hub_command_t*, uint8_t*, uint8_t);
usb_status usb_class_hub_get_port_status(hub_command_t*, uint8_t, uint8_t*, uint8_t);

usb_status usb_class_hub_recv_bitmap(hub_command_t*, uint8_t*, uint8_t);
void usb_class_hub_int_callback(void*, void *, uint8_t *, uint32_t, usb_status);

#ifdef __cplusplus
}
#endif

#endif
