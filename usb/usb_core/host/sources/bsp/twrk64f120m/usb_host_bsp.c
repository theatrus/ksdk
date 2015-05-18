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
#include "usb.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_usb_khci_hal.h"
#include "fsl_gpio_driver.h"
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
    #if (defined(CPU_MK64F12))
#include "MK64F12/MK64F12_sim.h"
#include "MK64F12/MK64F12_usb.h"
//#include "MK64F12/MK64F12_gpio.h"
//#include "MK64F12/MK64F12_port.h"
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
extern _WEAK_FUNCTION(usb_status bsp_usb_host_board_init(uint8_t controller_id));
#ifdef __cplusplus
    }
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#define BSP_USB_INT_LEVEL                (4)
#define USB_CLK_RECOVER_IRC_EN           (*(volatile unsigned char *)0x40072144)
#define SIM_SOPT2_IRC48MSEL_MASK         0x30000u
#ifdef BSPCFG_USB_USE_IRC48M
#undef BSPCFG_USB_USE_IRC48M
#endif
#define BSPCFG_USB_USE_IRC48M            (0)


#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)

#define kGpioUsbVbus                     GPIO_MAKE_PIN(GPIOC_IDX, 9U)
/* Declare usb vbus gpio enable pin usb otg demo and host demo*/
const gpio_output_pin_user_config_t usbvbusenablePin[] = 
{
    {
        .pinName = kGpioUsbVbus,
        .config.outputLogic = 1,
        .config.slewRate = kPortSlowSlewRate,
        .config.isOpenDrainEnabled = false,
        .config.driveStrength = kPortLowDriveStrength,
    },
    {
        .pinName = GPIO_PINS_OUT_OF_RANGE,
    }
};
#endif

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : bsp_usb_host_soc_init
* Returned Value   : USB status
* Comments         :
*    This function performs BSP-specific I/O initialization related to USB
*
*END*----------------------------------------------------------------------*/
static usb_status bsp_usb_host_soc_init
(
   int32_t controller_id
)
{
    usb_status ret = 0;
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
        /* USB Clock Gating */
        SIM_SCGC4 |= (SIM_SCGC4_USBOTG_MASK);
        /* PLL/FLL selected as CLK source */
        SIM_SOPT2 |= (SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(0x01));
        /* Configure USB divider to be 120MHz * 2 / 5 = 48 MHz */
        SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV(4) | SIM_CLKDIV2_USBFRAC_MASK;
#endif
        MPU_CESR = (0);
        /* Weak pull downs */
        USB0_USBCTRL = (0x40);

        /* reset USB CTRL register */
        USB0_USBCTRL = (0x0);
        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        /* MPU is disabled. All accesses from all bus masters are allowed */
        MPU_CESR = (0);
        /* USB Clock Gating */
        HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);

        /* PLL/FLL selected as CLK source */
        HW_SIM_SOPT2_CLR(SIM_SOPT2_PLLFLLSEL_MASK);
        HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(0x01));
        /* Weak pull downs */
        HW_USB_USBCTRL_WR(0x40);
        HW_USB_USBCTRL_WR(0);
        /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
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
        /* MPU is disabled. All accesses from all bus masters are allowed */
        MPU_CESR = (0);
        /* set internal register pull down */
        usb_hal_khci_set_weak_pulldown(base_addres);
        /* reset USB CTRL register */
        usb_hal_khci_reset_control_register(base_addres);
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
* Function Name    : bsp_usb_host_board_init
* Returned Value   : USB status
* Comments         :
*    This function performs board-specific initialization related to USB
*
*END*----------------------------------------------------------------------*/
_WEAK_FUNCTION(usb_status bsp_usb_host_board_init(uint8_t controller_id))
{
    usb_status ret = USB_OK;
    
    if (controller_id == USB_CONTROLLER_KHCI_0)
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
        /* Source the P5V0_K64_USB. Set PTC9 to high */
        PORTC_PCR9 = PORT_PCR_MUX(0x1); /* GPIO mux */
        GPIOC_PDDR |= (1 << 9); /* Set output */
        GPIOC_PSOR |= (1 << 9); /* Output high */
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
        /* Enable clock gating to all ports C*/
        CLOCK_SYS_EnablePortClock(2);
        GPIO_DRV_Init(NULL, usbvbusenablePin);
        GPIO_DRV_WritePinOutput(kGpioUsbVbus, 1);
#else
        HW_SIM_SCGC5_SET(SIM_SCGC5_PORTC_MASK);
        /* Source the P5V0_K64_USB. Set PTC9 to high */
        BW_PORT_PCRn_MUX(HW_PORTC, 9, 1); /* GPIO mux */
        HW_GPIO_PDDR_SET(2, 1<<9);        /* Set output */
        HW_GPIO_PSOR_WR(2, HW_GPIO_PSOR(2).U | 1<<9);   /* Output high */
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
* Function Name    : bsp_usb_host_init
* Returned Value   : USB status
* Comments         :
*    This function performs BSP-specific initialization related to USB
*
*END*----------------------------------------------------------------------*/
usb_status bsp_usb_host_init(uint8_t controller_id)
{
    usb_status ret = USB_OK;
    
    ret = bsp_usb_host_soc_init(controller_id);
    if (ret == USB_OK)
    {
        ret = bsp_usb_host_board_init(controller_id);
    }
    return ret;
}
#endif
/* EOF */
