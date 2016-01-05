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
#include "usb_device_config.h"
#include "usb_misc.h"
#include "fsl_usb_khci_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h" 
#include "usb_device_config.h"

#define BSP_USB_INT_LEVEL                (3)

extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);
extern uint32_t soc_get_usb_base_address(uint8_t controller_id);

usb_status usb_dev_soc_init
(
    uint8_t controller_id
)
{
    int32_t ret = USB_OK;
    uint8_t usb_instance = 0;
    uint32_t base_addres = 0;
    if (USB_CONTROLLER_KHCI_0==controller_id  )
    {    
         usb_instance = controller_id - USB_CONTROLLER_KHCI_0;
         base_addres = soc_get_usb_base_address(controller_id);
#if USBCFG_DEV_USE_IRC48M
        /* IRC48MHz selected as CLK source */
        CLOCK_SYS_SetUsbfsSrc(usb_instance, kClockUsbfsSrcIrc48M);
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(usb_instance);
        /* Enable IRC 48MHz for USB module, the regulator is always enabled on KL27 */
        usb_hal_khci_ungate_irc48m(base_addres);
        usb_hal_khci_enable_irc48m_recovery_block(base_addres);
#else
        /* USB_CLKIN selected as CLK source */
        CLOCK_SYS_SetUsbfsSrc(usb_instance, kClockUsbfsSrcExt);
        
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(usb_instance);
#endif       
        /* reset USB CTRL register */
        usb_hal_khci_reset_control_register(base_addres);

#if USBCFG_DEV_KEEP_ALIVE_MODE
        USB0_CLK_RECOVER_CTRL |= USB_CLK_RECOVER_CTRL_CLOCK_RECOVER_EN_MASK;
        USB0_KEEP_ALIVE_CTRL = USB_KEEP_ALIVE_CTRL_KEEP_ALIVE_EN_MASK|USB_KEEP_ALIVE_CTRL_OWN_OVERRD_EN_MASK
                                    |USB_KEEP_ALIVE_CTRL_STOP_ACK_DLY_EN_MASK|USB_KEEP_ALIVE_CTRL_AHB_DLY_EN_MASK
                                    |USB_KEEP_ALIVE_CTRL_WAKE_INT_EN_MASK;
        /* wake on out and setup transaction */
        USB0_KEEP_ALIVE_WKCTRL = 0x1;
        MCG_MC |= MCG_MC_HIRCLPEN_MASK;
        PMC_REGSC |= PMC_REGSC_BGEN_MASK|PMC_REGSC_VLPO_MASK;
#endif

    }
    else
    {
        ret = USBERR_BAD_STATUS; //unknow controller
    }

    return ret;
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
