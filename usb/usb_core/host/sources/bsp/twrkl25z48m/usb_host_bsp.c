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
#include "usb_misc.h"
#include "usb_error.h"
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_usb_khci_hal.h"
extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);

#define BSP_USB_INT_LEVEL                (4)

static int32_t bsp_usb_host_io_init
(
   int32_t i
)
{

    int32_t ret = USB_OK;
    uint32_t freq;
    clock_usbfs_src_t src;

    if (i == USB_CONTROLLER_KHCI_0)
    {
        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetUsbfsSrc(i, kClockUsbfsSrcPllFllSel);
        CLOCK_SYS_SetPllfllSel(kClockPllFllSelPll);

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
        usb_hal_khci_reset_control_register(USB0_BASE);


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
    {
        return result;
    }

    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        /* Do not configure enable USB regulator for host */
        // SIM_SOPT1CFG_REG(SIM_BASE_PTR) |= SIM_SOPT1CFG_URWE_MASK;
        // SIM_SOPT1_REG(SIM_BASE_PTR) |= SIM_SOPT1_USBREGEN_MASK;

        /* reset USB CTRL register */
        usb_hal_khci_reset_control_register(USB0_BASE);

        /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
    }
    else
    {
        /* unknown controller */
        result = -1;
    }

    return result;
}

/* EOF */
