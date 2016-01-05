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
 * $FileName: usb_otg_khci.h$
 * $Version : 
 * $Date    : 
 *
 * Comments :
 *
 *         
 *****************************************************************************/
#ifndef __usb_otg_khci_h__
#define __usb_otg_khci_h__

#include "usb_types.h"
#include "usb_otg_private.h" 

#define USB_OTG_KHCI_ISR_EVENT (0x01) 

/* Todo: Check struct here */
typedef struct usb_generic_init_struct
{
    void* base_ptr;
    uint32_t vector;
    uint32_t priority;
} usb_int_struct_t;
typedef usb_int_struct_t usb_khci_otg_int_struct_t;
typedef struct usb_otg_khci_call_struct
{
    usb_otg_state_struct_t * otg_handle_ptr;
    usb_khci_otg_int_struct_t * init_param_ptr;
} usb_otg_khci_call_struct_t;

#ifdef __cplusplus
extern "C"
{
#endif
    extern usb_status _usb_otg_khci_init(uint8_t controller_id, usb_otg_khci_call_struct_t * otg_khci_call_ptr);
    extern usb_status _usb_otg_khci_shut_down(usb_otg_khci_call_struct_t * otg_khci_call_ptr);
    extern usb_status _usb_otg_khci_get_status(usb_otg_khci_call_struct_t * otg_khci_call_ptr);
    extern usb_status _usb_otg_khci_set_pull_downs(usb_otg_khci_call_struct_t * otg_khci_call_ptr, uint8_t bitfield);
    extern usb_status _usb_otg_khci_set_dp_pull_up(usb_otg_khci_call_struct_t * otg_khci_call_ptr, bool enable);
    extern usb_status _usb_otg_khci_generate_resume(usb_otg_khci_call_struct_t * otg_khci_call_ptr, bool enable);
#ifdef __cplusplus
}
#endif

#endif

/* EOF */
