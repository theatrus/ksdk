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
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "usb.h"
#include "fsl_usb_khci_hal.h"

extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);

#define BSP_USB_INT_LEVEL                (4)

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : bsp_usb_dev_io_init
* Returned Value   : USB status
* Comments         :
*    This function performs BSP-specific I/O initialization related to USB
*
*END*----------------------------------------------------------------------*/
static int32_t bsp_usb_dev_io_init
(
    uint8_t controller_id
)
{
    int32_t ret = 0;
    if (controller_id == USB_CONTROLLER_KHCI_0)
    {
#if USBCFG_DEV_USE_IRC48M
        /* USB clock divider */
        CLOCK_SYS_SetUsbfsDiv(i, 0U, 0U);
        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetUsbfsSrc(i, kClockUsbfsSrcPllFllSel);
        CLOCK_SYS_SetPllfllSel(kClockPllFllSelIrc48M);
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(i);
        /* Enable IRC 48MHz for USB module */
        usb_hal_khci_ungate_irc48m(USB0_BASE);
        usb_hal_khci_enable_irc48m_recovery_block(USB0_BASE);
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
    }
    else
    {
        ret = USBERR_BAD_STATUS; //unknown controller
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
int32_t bsp_usb_dev_init(uint8_t controller_id)
{
    int32_t result = 0;

    result = bsp_usb_dev_io_init(controller_id);
    if (result != 0)
    {
        return result;
    }

    /* MPU is disabled. All accesses from all bus masters are allowed */
    //MPU_CESR=0;
    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        /* Configure enable USB regulator for device */
        SIM_HAL_SetUsbVoltRegulatorWriteCmd((SIM_Type*)(SIM_BASE), TRUE);
        SIM_HAL_SetUsbVoltRegulatorCmd((SIM_Type*)(SIM_BASE), TRUE);
    }
    else
    {
        /* unknown controller */
        result = USBERR_BAD_STATUS;
    }

    return result;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : soc_get_usb_dev_int_level
* Returned Value   : usb interrupt level
* Comments         :
*    This function return usb interrupt level
*
*END*----------------------------------------------------------------------*/
uint32_t soc_get_usb_dev_int_level(uint8_t controller_id)
{
    return (uint32_t) BSP_USB_INT_LEVEL;
}
/* EOF */
