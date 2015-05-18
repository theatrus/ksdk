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
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_usb_khci_hal.h"
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
    #if (defined(CPU_MK64F12))
#include "MK64F12/MK64F12_sim.h"
#include "MK64F12/MK64F12_usb.h"
#include "MK64F12/MK64F12_mpu.h"
    #endif
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include "MK64F12.h"
#endif
extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);
extern uint32_t soc_get_usb_base_address(uint8_t controller_id);
#ifdef __cplusplus
extern "C" {
#endif
extern _WEAK_FUNCTION(usb_status bsp_usb_dev_board_init(uint8_t controller_id));
#ifdef __cplusplus
    }
#endif
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#define BSP_USB_INT_LEVEL                (4)
#define USB_CLK_RECOVER_IRC_EN (*(volatile unsigned char *)0x40072144)
#define SIM_SOPT2_IRC48MSEL_MASK         0x30000u
#ifdef BSPCFG_USB_USE_IRC48M
#undef BSPCFG_USB_USE_IRC48M
#endif
#define BSPCFG_USB_USE_IRC48M            (1)

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : bsp_usb_dev_soc_init
* Returned Value   : USB status
* Comments         :
*    This function performs BSP-specific I/O initialization related to USB
*
*END*----------------------------------------------------------------------*/
static usb_status bsp_usb_dev_soc_init
(
    int32_t controller_id
)
{
    usb_status ret = USB_OK;
    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#if BSPCFG_USB_USE_IRC48M
        /*
            * Configure SIM_CLKDIV2: USBDIV = 0, USBFRAC = 0
            */
        SIM_CLKDIV2 &= ~(SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV_MASK);
        /* Configure USB to be clocked from IRC 48MHz */
        SIM_SOPT2 |= (SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_IRC48MSEL_MASK);
        /* Enable USB-OTG IP clocking */
        SIM_SCGC4 |= (SIM_SCGC4_USBOTG_MASK);
        /* Enable IRC 48MHz for USB module */
        USB_CLK_RECOVER_IRC_EN = 0x03;
#else
        /* Configure USBFRAC = 0, USBDIV = 0 => frq(USBout) = 1 / 1 * frq(PLLin) */
        SIM_CLKDIV2 &= ~(SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV_MASK);
        /* Configure USB to be clocked from PLL */
        SIM_SOPT2 |= (SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(0x01));
        /* Enable USB-OTG IP clocking */
        SIM_SCGC4 |= (SIM_SCGC4_USBOTG_MASK);
#endif
        MPU_CESR=0;
        SIM_SOPT1CFG |= (SIM_SOPT1CFG_URWE_MASK);
        SIM_SOPT1 |= (SIM_SOPT1_USBREGEN_MASK);

        /* reset USB CTRL register */
        USB0_USBCTRL = 0;

        /* Enable internal pull-up resistor */
        USB0_CONTROL = (USB_CONTROL_DPPULLUPNONOTG_MASK);
        USB0_USBTRC0 |= (0x40); /* Software must set this bit to 1 */
        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#if !BSPCFG_USB_USE_IRC48M
        uint32_t freq;
        clock_usbfs_src_t src;
#endif
        uint8_t instance = controller_id - USB_CONTROLLER_KHCI_0;
        uint32_t base_addres = soc_get_usb_base_address(controller_id);
#if BSPCFG_USB_USE_IRC48M
        /* USB clock divider */
        CLOCK_SYS_SetUsbfsDiv(instance, 0U, 0U);
        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetUsbfsSrc(instance, kClockUsbfsSrcPllFllSel);
        CLOCK_SYS_SetPllfllSel(kClockPllFllSelIrc48M);
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(instance);
        /* Enable IRC 48MHz for USB module */
        USB_CLK_RECOVER_IRC_EN = 0x03;
#else
        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetPllfllSel(kClockPllFllSelPll);
        CLOCK_SYS_SetUsbfsSrc(instance, kClockUsbfsSrcPllFllSel);

        /* USB clock divider */
        src = CLOCK_SYS_GetUsbfsSrc(instance);
        switch(src)
        {
            case kClockUsbfsSrcExt:
                ret = USBERR_BAD_STATUS;
                break;
            case kClockUsbfsSrcPllFllSel:
                freq = CLOCK_SYS_GetPllFllClockFreq();
                switch(freq)
                {
                case 120000000U:
                    CLOCK_SYS_SetUsbfsDiv(instance, 4, 1);
                    break;
                case 96000000U:
                    CLOCK_SYS_SetUsbfsDiv(instance, 1, 0);
                    break;
                case 72000000U:
                    CLOCK_SYS_SetUsbfsDiv(instance, 2, 1);
                    break;
                case 48000000U:
                    CLOCK_SYS_SetUsbfsDiv(instance, 0, 0);
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
        if(48000000U != CLOCK_SYS_GetUsbfsFreq(instance))
        {
            ret = USBERR_BAD_STATUS;
        }

        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(instance);
#endif
        MPU_CESR=0;
        /* Weak pull downs */
        usb_hal_khci_set_weak_pulldown(base_addres);
        /* Configure enable USB regulator for device */
        SIM_HAL_SetUsbVoltRegulatorWriteCmd((SIM_Type*)(SIM_BASE), TRUE);
        SIM_HAL_SetUsbVoltRegulatorCmd((SIM_Type*)(SIM_BASE), TRUE);

        /* reset USB CTRL register */
        usb_hal_khci_reset_control_register(base_addres);

        /* Enable internal pull-up resistor */
        usb_hal_khci_set_internal_pullup(base_addres);
        usb_hal_khci_set_trc0(base_addres); /* Software must set this bit to 1 */
        /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
#else
#if BSPCFG_USB_USE_IRC48M
        /*
        * Configure SIM_CLKDIV2: USBDIV = 0, USBFRAC = 0
        */
        HW_SIM_CLKDIV2_CLR(SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV_MASK);
        /* Configure USB to be clocked from IRC 48MHz */
        HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_IRC48MSEL_MASK);
        /* Enable USB-OTG IP clocking */
        HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);
        /* Enable IRC 48MHz for USB module */
        USB_CLK_RECOVER_IRC_EN = 0x03;
#else
        /* Configure USBFRAC = 0, USBDIV = 0 => frq(USBout) = 1 / 1 * frq(PLLin) */
        HW_SIM_CLKDIV2_CLR(SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV_MASK);
        /* Configure USB to be clocked from PLL */
        HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(0x01));
        /* Enable USB-OTG IP clocking */
        HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);
#endif
        MPU_CESR = (0);
        /* Configure enable USB regulator for device */
        HW_SIM_SOPT1CFG_SET(SIM_SOPT1CFG_URWE_MASK);
        HW_SIM_SOPT1_SET(SIM_SOPT1_USBREGEN_MASK);

        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(0);

        /* Enable internal pull-up resistor */
        HW_USB_CONTROL_WR(USB_CONTROL_DPPULLUPNONOTG_MASK);
        HW_USB_USBTRC0_SET(0x40); /* Software must set this bit to 1 */
         /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
#endif
    }
    else
    {
        ret = USBERR_BAD_STATUS; //unknown controller
    }
    return ret;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : bsp_usb_dev_board_init
* Returned Value   : USB status
* Comments         :
*    This function performs board-specific initialization related to USB
*
*END*----------------------------------------------------------------------*/
_WEAK_FUNCTION(usb_status bsp_usb_dev_board_init(uint8_t controller_id))
{
    usb_status ret = USB_OK;
    
    if (USB_CONTROLLER_KHCI_0 == controller_id)
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

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : bsp_usb_dev_init
* Returned Value   : USB status
* Comments         :
*    This function performs BSP-specific initialization related to USB
*
*END*----------------------------------------------------------------------*/
usb_status bsp_usb_dev_init(uint8_t controller_id)
{
    usb_status ret = USB_OK;
    
    ret = bsp_usb_dev_soc_init(controller_id);
    if (ret == USB_OK)
    {
        ret = bsp_usb_dev_board_init(controller_id);
    }
    return ret;
}
#endif
/* EOF */
