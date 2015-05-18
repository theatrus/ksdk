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
#include "usb_host_config.h"
#include "usb.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_gpio_driver.h"
#include "fsl_usb_khci_hal.h"

#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
    #if (defined(CPU_MK21FA12))
#include "MK21FA12/MK21FA12_sim.h"
#include "MK21FA12/MK21FA12_usb.h"
#include "MK21FA12/MK21FA12_gpio.h"
#include "MK21FA12/MK21FA12_port.h"
#include "MK21FA12/MK21FA12_mpu.h"
    #endif
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include "MK21FA12.h"
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
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#if USBCFG_HOST_PORT_NATIVE
#define kGpioUsbVbus                 GPIO_MAKE_PIN(GPIOC_IDX, 9U)

/*! @brief Configuration structure 0 for out pins */
const gpio_output_pin_user_config_t usbvbusenablePin[] = 
{
  {
    .pinName = kGpioUsbVbus,
    .config.outputLogic = 1,
    .config.slewRate = kPortSlowSlewRate,
    .config.driveStrength = kPortLowDriveStrength,
  },
  {
    .pinName = GPIO_PINS_OUT_OF_RANGE,
  }
};
#endif
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
        /* Configure USB to be clocked from PLL0 */
        SIM_SOPT2 |= (SIM_SOPT2_USBSRC_MASK );
        SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL(1);
        /* Configure USB divider to be 120MHz * 2 / 5 = 48 MHz */
        SIM_CLKDIV2 &= ~(SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV_MASK);
        SIM_CLKDIV2 |= SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV(5 - 1);
        /* Enable USB-OTG IP clocking */
        SIM_SCGC4 |= (SIM_SCGC4_USBOTG_MASK);

        /* MPU is disabled. All accesses from all bus masters are allowed */
        MPU_CESR = (0);
        /* Do not configure enable USB regulator for host */
        /* reset USB CTRL register */
        USB0_USBCTRL = (0);

        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        /* Configure USB to be clocked from PLL0 */
        HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK );
        BW_SIM_SOPT2_PLLFLLSEL(1);
        /* Configure USB divider to be 120MHz * 2 / 5 = 48 MHz */
        BW_SIM_CLKDIV2_USBFRAC(2 - 1);
        BW_SIM_CLKDIV2_USBDIV(5 - 1);
        /* Enable USB-OTG IP clocking */
        HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);
        /* MPU is disabled. All accesses from all bus masters are allowed */
        HW_MPU_CESR_WR(0);
        /* Do not configure enable USB regulator for host */
        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(0);

        /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
        uint32_t freq;
        clock_usbfs_src_t src;
        uint8_t instance = controller_id - USB_CONTROLLER_KHCI_0;
        uint32_t base_addres = soc_get_usb_base_address(controller_id);
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

        MPU_CESR=0;

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
extern _WEAK_FUNCTION(usb_status bsp_usb_host_board_init(uint8_t controller_id))
{
    usb_status ret = USB_OK;
    
    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
#if USBCFG_HOST_PORT_NATIVE
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
        /* Source the P5V0_K22_USB. Set PTC9 to high */
        SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
        PORTC_PCR9 |= PORT_PCR_MUX(1); /* GPIO mux */
        GPIOC_PDDR |= GPIO_PDDR_PDD(1);     /* Set output */
        GPIOC_PSOR |= GPIO_PSOR_PTSO(1);    /* Output high */

#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
        /* Source the P5V0_K22_USB. Set PTC9 to high */
        CLOCK_SYS_EnablePortClock(2);
        GPIO_DRV_Init(NULL, usbvbusenablePin);
        GPIO_DRV_WritePinOutput(kGpioUsbVbus, 1);
#else
        HW_SIM_SCGC5_SET(SIM_SCGC5_PORTC_MASK);
        /* Source the P5V0_K22_USB. Set PTC9 to high */
        BW_PORT_PCRn_MUX(HW_PORTC, 9, 1); /* GPIO mux */
        HW_GPIO_PDDR_SET(2, 1<<9);        /* Set output */
        HW_GPIO_PSOR_WR(2, HW_GPIO_PSOR(2).U | 1<<9);   /* Output high */
#endif
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
    if(USB_OK == bsp_usb_host_soc_init(controller_id))
    {
        return bsp_usb_host_board_init(controller_id);
    }
    return USBERR_ERROR;
}

#endif
/* EOF */
