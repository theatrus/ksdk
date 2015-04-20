/**HEADER********************************************************************
* 
* Copyright (c) 2013 - 2014 Freescale Semiconductor;
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
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#define SIM_SOPT2_IRC48MSEL_MASK                 0x30000u
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
    #if (defined(CPU_MK22F51212))
#include "MK22F51212/MK22F51212_sim.h"
#include "MK22F51212/MK22F51212_usb.h"
    #endif
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include "MK22F51212.h"
#endif

extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#define BSP_USB_INT_LEVEL                (4)
#define USB_CLK_RECOVER_IRC_EN (*(volatile unsigned char *)0x40072144)
#define SIM_SOPT2_IRC48MSEL_MASK                 0x30000u
#define BSPCFG_USB_USE_IRC48M            (1)

static int32_t bsp_usb_dev_io_init
(
    int32_t i
)
{
    int32_t ret = 0;
    if (i == USB_CONTROLLER_KHCI_0)
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
#else
        /* USB clock divider */
        CLOCK_SYS_SetUsbfsDiv(i, 0U, 0U);

        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetUsbfsSrc(i, kClockUsbfsSrcPllFllSel);
        CLOCK_SYS_SetPllfllSel(kClockPllFllSelPll);
        
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(i);
#endif
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
#endif
    }
    else
    {
        ret = -1; //unknow controller
    }

    return ret;
}

int32_t bsp_usb_dev_init(uint8_t controller_id)
{
    int32_t result = 0;

    result = bsp_usb_dev_io_init(controller_id);
    if (result != 0)
        return result;

    /* MPU is disabled. All accesses from all bus masters are allowed */
    //MPU_CESR=0;
    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
        /* Configure enable USB regulator for device */
        SIM_SOPT1CFG |= (SIM_SOPT1CFG_URWE_MASK);
        SIM_SOPT1 |= (SIM_SOPT1_USBREGEN_MASK);

        /* reset USB CTRL register */
        USB0_USBCTRL = 0;

        /* Enable internal pull-up resistor */
        USB0_CONTROL = (USB_CONTROL_DPPULLUPNONOTG_MASK);
        USB0_USBTRC0 |= (0x40); /* Software must set this bit to 1 */
          /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(0), BSP_USB_INT_LEVEL, 0, TRUE);
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
        /* Configure enable USB regulator for device */
        HW_SIM_SOPT1CFG_SET(SIM_BASE, SIM_SOPT1CFG_URWE_MASK);
        HW_SIM_SOPT1_SET(SIM_BASE, SIM_SOPT1_USBREGEN_MASK);

        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(USB0_BASE, 0);

        /* Enable internal pull-up resistor */
        HW_USB_CONTROL_WR(USB0_BASE, USB_CONTROL_DPPULLUPNONOTG_MASK);
        HW_USB_USBTRC0_SET(USB0_BASE, 0x40); /* Software must set this bit to 1 */
         /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(0), BSP_USB_INT_LEVEL, 0, TRUE);
#else
        /* Configure enable USB regulator for device */
        HW_SIM_SOPT1CFG_SET(SIM_SOPT1CFG_URWE_MASK);
        HW_SIM_SOPT1_SET(SIM_SOPT1_USBREGEN_MASK);

        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(0);

        /* Enable internal pull-up resistor */
        HW_USB_CONTROL_WR(USB_CONTROL_DPPULLUPNONOTG_MASK);
        HW_USB_USBTRC0_SET(0x40); /* Software must set this bit to 1 */
         /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(0), BSP_USB_INT_LEVEL, 0, TRUE);
#endif
    }
    else
    {
        /* unknown controller */
        result = -1;
    }

    return result;
}
#endif
/* EOF */
