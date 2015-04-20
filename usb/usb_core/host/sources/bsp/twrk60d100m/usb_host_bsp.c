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
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
    #if (defined(CPU_MK60D10))
#include "MK60D10/MK60D10_sim.h"
#include "MK60D10/MK60D10_usb.h"
#include "MK60D10/MK60D10_mpu.h"
    #endif
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include "MK60D10.h"
#endif

extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || \
    (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#define BSP_USB_INT_LEVEL                        (4)
#define BSPCFG_USB_USE_IRC48M                    (0)
static int32_t bsp_usb_host_io_init
(
    int32_t i
)
{
    int32_t ret = 0;
    
    if (i == 0)
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)      
        /* USB clock divider */
        CLOCK_SYS_SetUsbfsDiv(i, 1U, 0U);
        
        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetUsbfsSrc(i, kClockUsbfsSrcPllFllSel);
        CLOCK_SYS_SetPllfllSel(kClockPllFllSelPll);
        
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(i);
        /* Enable clock gating to all ports C*/
        CLOCK_SYS_EnablePortClock(3);
        /* Weak pull downs */
        HW_USB_USBCTRL_WR(USB0_BASE, 0x40);
        /* Souce the P5V0_K22_USB. Set PTC9 to high */
        BW_PORT_PCRn_MUX(PORTC_BASE, 9, 1); /* GPIO mux */
        HW_GPIO_PDDR_SET(PTC_BASE, 1<<9);        /* Set output */
        HW_GPIO_PSOR_WR(PTC_BASE, HW_GPIO_PSOR_RD(PTC_BASE) | 1<<9);   /* Output high */
#else
        /* Configure USB to be clocked from PLL0 */
        HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK );
        BW_SIM_SOPT2_PLLFLLSEL(1);
        /* Configure USB divider to be 96MHz * 1 / 2 = 48 MHz */
        BW_SIM_CLKDIV2_USBFRAC(1 - 1);
        BW_SIM_CLKDIV2_USBDIV(2 - 1);
        /* Enable USB-OTG IP clocking */
        HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);
        
        SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
        BW_PORT_PCRn_MUX(HW_PORTC, 9, 1); /* GPIO mux */
        HW_GPIO_PDDR_SET(2, 1<<9);        /* Set output */
        HW_GPIO_PSOR_WR(2, HW_GPIO_PSOR(2).U | 1<<9);       /* Output high */
#endif
    }
    else
    {
        /*unknow controller*/
        ret = -1; 
    }

    return ret;
    
}


int32_t bsp_usb_host_init(uint8_t controller_id)
{
    int32_t result = 0;

    result = bsp_usb_host_io_init(controller_id);
    if (result != 0)
    {
        return result;
    }
    if (0 == controller_id)
    {
#if (OS_ADAPTER_ACTIVE_OS != OS_ADAPTER_SDK)
        /* MPU is disabled. All accesses from all bus masters are allowed */
        HW_MPU_CESR_WR(0);
        /* Do not configure enable USB regulator for host */
        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(0);

        /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(0), BSP_USB_INT_LEVEL, 0, TRUE);
#else
        /*Disable MPU so the module can access RAM*/
        MPU_CESR=0;
        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(USB0_BASE, 0);
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
