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
 * $FileName: usb_otg_dev_api.h$
 * $Version : 
 * $Date    : 
 *
 * Comments : This is the header file for the OTG driver
 *
 *         
 *****************************************************************************/

#ifndef __usb_otg_dev_api__
#define __usb_otg_dev_api__

#include "usb_types.h"
/* Public constants */

/* Public functions */
#ifdef __cplusplus
extern "C"
{
#endif
    extern uint32_t usb_otg_device_hnp_enable(usb_otg_handle handle, uint8_t enable);
    extern uint32_t usb_otg_device_on_class_init(usb_otg_handle otg_handle, usb_device_handle dev_handle, uint8_t bm_attributes);
#ifdef __cplusplus
}
#endif
#endif /* __usb_otg_dev_api__ */
