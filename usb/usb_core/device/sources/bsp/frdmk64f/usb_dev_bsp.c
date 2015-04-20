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
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#define SIM_SOPT2_IRC48MSEL_MASK                 0x30000u
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)||(OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX))
#include "MK64F12.h"
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
    
    if (i == 0)
    {
#if (OS_ADAPTER_ACTIVE_OS != OS_ADAPTER_SDK)
#if BSPCFG_USB_USE_IRC48M

    /*
    * Configure SIM_CLKDIV2: USBDIV = 0, USBFRAC = 0
    */
    SIM_CLKDIV2 = (uint32_t)0x0UL; /* Update USB clock prescalers */

    /* Configure USB to be clocked from IRC 48MHz */
    SIM_SOPT2_REG(SIM_BASE_PTR)  |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_IRC48MSEL_MASK;

    /* Enable USB-OTG IP clocking */
    SIM_SCGC4_REG(SIM_BASE_PTR) |= SIM_SCGC4_USBOTG_MASK;
    /* Enable IRC 48MHz for USB module */
    USB_CLK_RECOVER_IRC_EN = 0x03;
#else
    /* Configure USB to be clocked from PLL0 */
    SIM_SOPT2_REG(SIM_BASE_PTR) |= SIM_SOPT2_PLLFLLSEL_MASK |SIM_SOPT2_USBSRC_MASK;
    /* Configure USB divider to be 120MHz * 2 / 5 = 48 MHz */
    SIM_CLKDIV2_REG(SIM_BASE_PTR) = SIM_CLKDIV2_USBDIV(4) | SIM_CLKDIV2_USBFRAC_MASK;

    /* Enable USB-OTG IP clocking */
    SIM_SCGC4_REG(SIM_BASE_PTR) |= SIM_SCGC4_USBOTG_MASK;
#endif

#else
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

        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetUsbfsSrc(i, kClockUsbfsSrcPllFllSel);
        CLOCK_SYS_SetPllfllSel(kClockPllFllSelPll);
        
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(i);
        
        /* Enable clock gating to all ports, A, B, C, D, E*/
        CLOCK_SYS_EnablePortClock(0);
        CLOCK_SYS_EnablePortClock(1);
        CLOCK_SYS_EnablePortClock(2);
        CLOCK_SYS_EnablePortClock(3);
        CLOCK_SYS_EnablePortClock(4);
        
        /* Weak pull downs */
        HW_USB_USBCTRL_WR(0x40);
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
    {
        return result;
    }

#if (OS_ADAPTER_ACTIVE_OS != OS_ADAPTER_SDK)
    /* MPU is disabled. All accesses from all bus masters are allowed */
    MPU_CESR=0;
    if (0 == controller_id)
    {
    
        /* Configure enable USB regulator for device */
        SIM_SOPT1_REG(SIM_BASE_PTR) |= SIM_SOPT1_USBREGEN_MASK;

        /* reset USB CTRL register */
        USB_USBCTRL_REG(USB0_BASE_PTR) = 0;
        
        /* Enable internal pull-up resistor */
        USB_CONTROL_REG(USB0_BASE_PTR) = USB_CONTROL_DPPULLUPNONOTG_MASK;
        USB_USBTRC0_REG(USB0_BASE_PTR) |= 0x40; /* Software must set this bit to 1 */
        /* setup interrupt */
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)        
        OS_intr_init(soc_get_usb_vector_number(0), BSP_USB_INT_LEVEL, 0, TRUE);
#else
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(0), BSP_USB_INT_LEVEL, 0, TRUE);
#endif
#else
    /* MPU is disabled. All accesses from all bus masters are allowed */
    MPU_CESR=0;
    if (0 == controller_id)
    {
        /* Configure enable USB regulator for device */
        HW_SIM_SOPT1CFG_SET(SIM_BASE, SIM_SOPT1CFG_URWE_MASK);
        HW_SIM_SOPT1_SET(SIM_BASE, SIM_SOPT1_USBREGEN_MASK);

        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(USB0_BASE, 0);

        /* Enable internal pull-up resistor */
        HW_USB_CONTROL_WR(USB0_BASE, USB_CONTROL_DPPULLUPNONOTG_MASK);
        HW_USB_USBTRC0_SET(USB0_BASE, 0x40); /* Software must set this bit to 1 */
        /* setup interrupt */
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        OS_intr_init(USB0_IRQn + 16, BSP_USB_INT_LEVEL, 0, TRUE);
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
        OS_intr_init(USB0_IRQn, BSP_USB_INT_LEVEL, 0, TRUE);
#endif
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
