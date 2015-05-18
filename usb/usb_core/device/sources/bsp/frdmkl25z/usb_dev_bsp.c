/**HEADER********************************************************************
* 
* Copyright (c) 2013 - 2015 Freescale Semiconductor;
* All Rights Reserved
*
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
* Comments:  
*
*END************************************************************************/
#include "adapter.h"
#include "usb_device_config.h"
#include "usb.h"
#include "usb_misc.h"
#include "usb_error.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_usb_khci_hal.h"
#endif
#define BSP_USB_INT_LEVEL                (4)
extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);
extern uint32_t soc_get_usb_base_address(uint8_t controller_id);
#ifdef __cplusplus
extern "C" {
#endif
extern _WEAK_FUNCTION(usb_status bsp_usb_dev_board_init(uint8_t controller_id));
#ifdef __cplusplus
    }
#endif
static usb_status bsp_usb_dev_soc_init
(
    uint8_t controller_id
)
{
    int32_t ret = USB_OK;
    uint8_t usb_instance = 0;
    uint32_t base_addres = 0;
    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {   
        usb_instance = controller_id - USB_CONTROLLER_KHCI_0;
        base_addres = soc_get_usb_base_address(controller_id);
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
        uint32_t freq;
        clock_usbfs_src_t src;
        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetPllfllSel(kClockPllFllSelPll);
        CLOCK_SYS_SetUsbfsSrc(usb_instance, kClockUsbfsSrcPllFllSel);
        /* Set USB clock divider */
        src = CLOCK_SYS_GetUsbfsSrc(usb_instance);
        switch(src)
        {
        case kClockUsbfsSrcExt:
            break;
        case kClockUsbfsSrcPllFllSel:
            freq = CLOCK_SYS_GetPllFllClockFreq();
            switch(freq)
            {
            case 120000000U:
            case 96000000U:
            case 72000000U:
                ret = USBERR_BAD_STATUS;
                break;
            case 48000000U:
                ret = USB_OK;
                break;
            default:
                ret = USBERR_BAD_STATUS;
                break;
            }
            break;
        default:
            ret = USBERR_BAD_STATUS;
            break;
        }
        /* Confirm the USB souce frequency is 48MHz */
        if(48000000U != CLOCK_SYS_GetUsbfsFreq(usb_instance))
        {
            ret = USBERR_BAD_STATUS;
        }
        
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(usb_instance);

        /* Weak pull downs */
        usb_hal_khci_set_weak_pulldown(base_addres);

        /* Configure enable USB regulator for device */
        SIM_HAL_SetUsbVoltRegulatorWriteCmd((SIM_Type*)SIM_BASE, SIM_SOPT1CFG_URWE_MASK);
        SIM_HAL_SetUsbVoltRegulatorCmd((SIM_Type *)SIM_BASE, SIM_SOPT1_USBREGEN_MASK);
    
        /* reset USB CTRL register */
        usb_hal_khci_reset_control_register(base_addres);
    
        /* Enable internal pull-up resistor */
        usb_hal_khci_set_internal_pullup(base_addres);
        usb_hal_khci_set_trc0(USB0_BASE); /* Software must set this bit to 1 */
        /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
#endif
    }
    else
    {
        ret = USBERR_BAD_STATUS; //unknow controller
    }

    return ret;
}

_WEAK_FUNCTION(usb_status bsp_usb_dev_board_init(uint8_t controller_id))
{
    usb_status ret = USB_OK;
    
    if (USB_CONTROLLER_KHCI_0 == controller_id )
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)

#else

#endif
    }
    else
    {
        ret = USBERR_BAD_STATUS;
    }
    
    return ret;
}
usb_status bsp_usb_dev_init(uint8_t controller_id){
    usb_status ret = USB_OK;
    
    ret = bsp_usb_dev_soc_init(controller_id);
    if (ret == USB_OK)
    {
        ret = bsp_usb_dev_board_init(controller_id);
    }

    return ret;
}

/* EOF */
