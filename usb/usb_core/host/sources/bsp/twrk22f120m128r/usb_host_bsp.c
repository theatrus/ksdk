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
#include "usb_misc.h"
#include "usb_host_config.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "usb.h"
#include "fsl_usb_khci_hal.h"
#include "fsl_gpio_driver.h"
#define SIM_SOPT2_IRC48MSEL_MASK                 0x30000u
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
    #if (defined(CPU_MK22F12810))
#include "MK22F12810/MK22F12810_sim.h"
#include "MK22F12810/MK22F12810_usb.h"
#include "MK22F12810/MK22F12810_gpio.h"
#include "MK22F12810/MK22F12810_port.h"
    #endif
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include "MK22F12810.h"
#endif

extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#define BSP_USB_INT_LEVEL                (4)
#define USB_CLK_RECOVER_IRC_EN (*(volatile unsigned char *)0x40072144)
#define USB_HOST_USE_EXTERNAL_CLKIN      (1)
#define BSPCFG_USB_USE_IRC48M            (0)

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#if USBCFG_HOST_PORT_NATIVE
#define kGpioUsbVbus                     GPIO_MAKE_PIN(GPIOC_IDX, 9U)
/* Declare usb vbus gpio enable pin usb otg demo and host demo*/
const gpio_output_pin_user_config_t usbvbusenablePin[] = {
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
#endif

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : bsp_usb_host_io_init
* Returned Value   : USB status
* Comments         :
*    This function performs BSP-specific I/O initialization related to USB
*
*END*----------------------------------------------------------------------*/
static int32_t bsp_usb_host_io_init
(
   int32_t i
)
{
    int32_t ret = 0;
    if (i == USB_CONTROLLER_KHCI_0)
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
        /* USB Clock Gating */
        SIM_SCGC4 |= (SIM_SCGC4_USBOTG_MASK);
        #if USB_HOST_USE_EXTERNAL_CLKIN
        /* External bypass clock USB_CLKIN */
         SIM_SOPT2 &= ~SIM_SOPT2_USBSRC_MASK;
        #else
        /* PLL/FLL selected as CLK source */
        SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;
        SIM_SOPT2 |= (SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(0x00));
        #endif
        
        /* Weak pull downs */
        USB0_USBCTRL = (0x40);
    #if USBCFG_HOST_PORT_NATIVE
        /* Source the P5V0_K22_USB. Set PTC9 to high */
        PORTC_PCR9 = PORT_PCR_MUX(0x1); /* GPIO mux */
        GPIOC_PDDR |= (1 << 9); /* Set output */
        GPIOC_PSOR |= (1 << 9); /* Output high */
    #endif
#if USB_HOST_USE_EXTERNAL_CLKIN
    /* Set PTA5 as USB_CLK_IN */
    PORTA_PCR5 = PORT_PCR_MUX(0x2);

    /* Set PTE26 as USB_CLK_IN */
    //PORTE_PCR26 = PORT_PCR_MUX(0x7);
#endif  

#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
       /* USB Clock Gating */
        HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);
        
        #if USB_HOST_USE_EXTERNAL_CLKIN
        /* External bypass clock (USB_CLKIN */
        HW_SIM_SOPT2_CLR(SIM_SOPT2_USBSRC_MASK);
        #else
        /* PLL/FLL selected as CLK source */
        HW_SIM_SOPT2_CLR(SIM_SOPT2_PLLFLLSEL_MASK);
        HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(0x00));
        #endif

        /* Weak pull downs */
        HW_USB_USBCTRL_WR(0x40);
    #if USBCFG_HOST_PORT_NATIVE
        HW_SIM_SCGC5_SET(SIM_SCGC5_PORTC_MASK);
        /* Source the P5V0_K22_USB. Set PTC9 to high */
        BW_PORT_PCRn_MUX(HW_PORTC, 9, 1); /* GPIO mux */
        HW_GPIO_PDDR_SET(2, 1<<9);        /* Set output */
        HW_GPIO_PSOR_WR(2, HW_GPIO_PSOR(2).U | 1<<9);   /* Output high */
     #endif
     #if USB_HOST_USE_EXTERNAL_CLKIN
        /* Set PTA5 as USB_CLK_IN */
        BW_PORT_PCRn_MUX(HW_PORTA, 5, 2); /* GPIO mux */

        /* Set PTE26 as USB_CLK_IN */
        //BW_PORT_PCRn_MUX(HW_PORTE, 26, 7); /* GPIO mux */
      #endif

#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#if BSPCFG_USB_USE_IRC48M
        /* USB clock divider */
        CLOCK_SYS_SetUsbfsDiv(i, 0U, 0U);
        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetUsbfsSrc(i, kClockUsbfsSrcPllFllSel);
        CLOCK_SYS_SetPllfllSel(kClockPllFllSelIrc48M);
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(i);
        /* Enable IRC 48MHz for USB module */
        USB_CLK_RECOVER_IRC_EN = 0x03;
#elif USB_HOST_USE_EXTERNAL_CLKIN
        /* External bypass clock (USB_CLKIN */
        CLOCK_SYS_SetUsbfsSrc(i, kClockUsbfsSrcExt);
#else
        uint32_t freq;
        clock_usbfs_src_t src;
        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetPllfllSel(kClockPllFllSelPll);
        CLOCK_SYS_SetUsbfsSrc(i, kClockUsbfsSrcPllFllSel);
        
        /* USB clock divider */
        src = CLOCK_SYS_GetUsbfsSrc(i);
        switch(src)
        {
        case kClockUsbfsSrcExt:
            break;
        case kClockUsbfsSrcPllFllSel:
            freq = CLOCK_SYS_GetPllFllClockFreq();
            switch(freq)
            {
            case 120000000U:
                CLOCK_SYS_SetUsbfsDiv(i, 4, 1);
                break;
            case 96000000U:
                CLOCK_SYS_SetUsbfsDiv(i, 1, 0);
                break;
            case 72000000U:
                CLOCK_SYS_SetUsbfsDiv(i, 2, 1);
                break;
            case 48000000U:
                CLOCK_SYS_SetUsbfsDiv(i, 0, 0);
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
        if (ret != USB_OK)
        {
            return USBERR_BAD_STATUS;
        } 
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(i);
#endif
        /* set internal register pull down */
        usb_hal_khci_set_weak_pulldown(USB0_BASE);
#if USBCFG_HOST_PORT_NATIVE
        /* Enable clock gating to all ports C*/
        CLOCK_SYS_EnablePortClock(2);
        GPIO_DRV_Init(NULL, usbvbusenablePin);
        GPIO_DRV_WritePinOutput(kGpioUsbVbus, 1);
#endif
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
* Function Name    : bsp_usb_host_init
* Returned Value   : USB status
* Comments         :
*    This function performs BSP-specific initialization related to USB
*
*END*----------------------------------------------------------------------*/
int32_t bsp_usb_host_init(uint8_t controller_id)
{
    int32_t result = 0;

    result = bsp_usb_host_io_init(controller_id);
    if (result != 0)
    {
        return result;
    }

    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
        /* reset USB CTRL register */
        USB0_USBCTRL = (0x0);
        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
        /* reset USB CTRL register */
        usb_hal_khci_reset_control_register(USB0_BASE);
        /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
#else
        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(0);
        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
#endif

    }
    else
    {
        /* unknown controller */
        result = USBERR_BAD_STATUS;
    }
    return result;
}
#endif
/* EOF */

