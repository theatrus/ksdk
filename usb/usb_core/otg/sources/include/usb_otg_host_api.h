/**HEADER********************************************************************
 * 
 * Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
 * $FileName: usb_otg_host_api.h$
 * $Version : 
 * $Date    : 
 *
 * Comments : This is the header file for the OTG driver
 *
 *         
 *****************************************************************************/

#ifndef __usb_otg_host_api__
#define __usb_otg_host_api__

#include "usb_types.h"

/* Public constants */
/* OTG related features */
#define OTG_B_HNP_ENABLE      3
#define OTG_A_HNP_SUPPORT     4

/* Public functions */
extern uint32_t usb_otg_host_a_set_b_hnp_en(usb_otg_handle otg_handle, bool b_hnp_en);
extern uint32_t usb_otg_host_on_interface_event(usb_otg_handle otg_handle, usb_device_handle dev_handle);
extern uint32_t usb_otg_host_on_detach_event(usb_otg_handle otg_handle);
extern uint32_t usb_otg_host_get_otg_attribute(usb_otg_handle otg_handle, uint8_t bm_attributes);
extern uint8_t usb_otg_host_set_feature_required(usb_otg_handle otg_handle);

#endif /*__usb_otg_host_api__*/
