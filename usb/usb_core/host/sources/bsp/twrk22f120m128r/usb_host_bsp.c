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
#include "usb_host_config.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
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
#define SIM_SOPT2_IRC48MSEL_MASK                 0x30000u
#define USB_HOST_USE_EXTERNAL_CLKIN      (1)

static int32_t bsp_usb_host_io_init
(
   int32_t i
)
{
    int32_t ret = 0;
    if (i == 0)
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
        /* Souce the P5V0_K22_USB. Set PTC9 to high */
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
        /* Souce the P5V0_K22_USB. Set PTC9 to high */
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

        /* get 48M clock */
        extern uint32_t SystemCoreClock;
        if (SystemCoreClock == 72000000)
        {
            /* USB clock divider */
            CLOCK_SYS_SetUsbfsDiv(i, 2U, 1U);
        }
        else if (SystemCoreClock == 95977472)
        {
            CLOCK_SYS_SetUsbfsDiv(i, 1U, 0U);
        }
        else
            USB_PRINTF("clock error\r\n");

        #if USB_HOST_USE_EXTERNAL_CLKIN
        /* External bypass clock (USB_CLKIN */
        CLOCK_SYS_SetUsbfsSrc(i, kClockUsbfsSrcExt);
        #else
        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetUsbfsSrc(i, kClockUsbfsSrcPllFllSel);
        CLOCK_SYS_SetPllfllSel(kClockPllFllSelFll);
        #endif
        
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(i);
        /* Weak pull downs */
        HW_USB_USBCTRL_WR(USB0_BASE, 0x40);
    #if USBCFG_HOST_PORT_NATIVE
        /* Enable clock gating to ports C */
        CLOCK_SYS_EnablePortClock(2);
        /* Souce the P5V0_K22_USB. Set PTC9 to high */
        BW_PORT_PCRn_MUX(PORTC_BASE, 9, 1); /* GPIO mux */
        HW_GPIO_PDDR_SET(PTC_BASE, 1<<9);        /* Set output */
        HW_GPIO_PSOR_WR(PTC_BASE, HW_GPIO_PSOR_RD(PTC_BASE) | 1<<9);   /* Output high */
    #endif
     #if USB_HOST_USE_EXTERNAL_CLKIN
        /* Set PTA5 as USB_CLK_IN */
        BW_PORT_PCRn_MUX(PORTA_BASE, 5, 2); /* GPIO mux */

        /* Set PTE26 as USB_CLK_IN */
        //BW_PORT_PCRn_MUX(HW_PORTE, 26, 7); /* GPIO mux */
      #endif
#endif
    }
    else
    {
        ret = -1; //unknow controller
    }

    return ret;
    
}


int32_t bsp_usb_host_init(uint8_t controller_id)
{
    int32_t result = 0;

    result = bsp_usb_host_io_init(controller_id);
    if (result != 0)
        return result;

    if (0 == controller_id)
    {
        /* Do not configure enable USB regulator for host */
        // SIM_SOPT1CFG_REG(SIM_BASE_PTR) |= SIM_SOPT1CFG_URWE_MASK;
        // SIM_SOPT1_REG(SIM_BASE_PTR) |= SIM_SOPT1_USBREGEN_MASK;

        /* reset USB CTRL register */
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
        USB0_USBCTRL = (0x0);
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
        HW_USB_USBCTRL_WR(USB0_BASE, 0);
#else
        HW_USB_USBCTRL_WR(0);
#endif

        /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(0), BSP_USB_INT_LEVEL, 0, TRUE);
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

