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
#include "usb_misc.h"
#include "usb_error.h"
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_usb_khci_hal.h"
#define BSP_USB_INT_LEVEL                (4)

static int32_t bsp_usb_dev_io_init
(
    int32_t i
)
{
    int32_t ret = USB_OK;
    uint32_t freq;
    clock_usbfs_src_t src;
    if (USB_CONTROLLER_KHCI_0 == i)
    {

        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetPllfllSel(kClockPllFllSelPll);
        CLOCK_SYS_SetUsbfsSrc(i, kClockUsbfsSrcPllFllSel);
        /* Set USB clock divider */
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
        if(48000000U != CLOCK_SYS_GetUsbfsFreq(i))
        {
            ret = USBERR_BAD_STATUS;
        }
        
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(i);

        /* Enable clock gating to all ports, A, B, C, D, E*/
        CLOCK_SYS_EnablePortClock(0);
        CLOCK_SYS_EnablePortClock(1);
        CLOCK_SYS_EnablePortClock(2);
        CLOCK_SYS_EnablePortClock(3);
        CLOCK_SYS_EnablePortClock(4);

        /* Weak pull downs */
        usb_hal_khci_set_weak_pulldown(USB0_BASE);
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

    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        /* Configure enable USB regulator for device */
		SIM_HAL_SetUsbVoltRegulatorWriteCmd((SIM_Type*)SIM_BASE, SIM_SOPT1CFG_URWE_MASK);
		SIM_HAL_SetUsbVoltRegulatorCmd((SIM_Type *)SIM_BASE, SIM_SOPT1_USBREGEN_MASK);

        /* reset USB CTRL register */
		usb_hal_khci_reset_control_register(USB0_BASE);

        /* Enable internal pull-up resistor */
		usb_hal_khci_set_internal_pullup(USB0_BASE);
		usb_hal_khci_set_trc0(USB0_BASE); /* Software must set this bit to 1 */
        /* setup interrupt */
        OS_intr_init(USB0_IRQn, BSP_USB_INT_LEVEL, 0, TRUE);
    }
    else
    {
        /* unknown controller */
        result = -1;
    }

    return result;
}

/* EOF */
