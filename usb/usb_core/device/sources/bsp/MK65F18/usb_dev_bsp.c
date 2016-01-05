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
#include "usb_ehci.h"
#include "fsl_usb_ehci_phy_hal.h"
#include "fsl_usb_ehci_hal.h"

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_usb_khci_hal.h"
#include "fsl_mcg_hal.h"
#include "fsl_gpio_driver.h"

extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);
extern uint32_t soc_get_usb_base_address(uint8_t controller_id);


#define BSP_USB_INT_LEVEL                (4)
#define crystal_val                       16000000
#define USBHS_USBMODE_CM_IDLE_MASK    USBHS_USBMODE_CM(0)
#define USBHS_USBMODE_CM_DEVICE_MASK  USBHS_USBMODE_CM(2)       
#define USBHS_USBMODE_CM_HOST_MASK    USBHS_USBMODE_CM(3)

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : usb_dev_soc_init
* Returned Value   : USB status
* Comments         :
*    This function performs BSP-specific I/O initialization related to USB
*
*END*----------------------------------------------------------------------*/
usb_status usb_dev_soc_init
(
    uint8_t controller_id
)
{
    usb_status ret = 0;

    /* MPU is disabled. All accesses from all bus masters are allowed */
    MPU_CESR=0;
    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
#if !USBCFG_DEV_USE_IRC48M
        uint32_t freq;
        clock_usbfs_src_t src;
#endif
        uint8_t instance = controller_id - USB_CONTROLLER_KHCI_0;
        uint32_t base_address = soc_get_usb_base_address(controller_id);
#if USBCFG_DEV_USE_IRC48M
        /* USB clock divider */
        CLOCK_SYS_SetUsbfsDiv(instance, 0U, 0U);
        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetUsbfsSrc(instance, kClockUsbfsSrcPllFllSel);
        CLOCK_SYS_SetPllfllSel(kClockPllFllSelIrc48M);
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(instance);
        /* Enable IRC 48MHz for USB module */
        usb_hal_khci_ungate_irc48m(base_address);
        usb_hal_khci_enable_irc48m_recovery_block(base_address);
#else
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
#endif
        /* Configure enable USB regulator for device */
        SIM_HAL_SetUsbVoltRegulatorWriteCmd((SIM_Type*)(SIM_BASE), TRUE);
        SIM_HAL_SetUsbVoltRegulatorCmd((SIM_Type*)(SIM_BASE), TRUE);
    }
    else if (USB_CONTROLLER_EHCI_0 == controller_id)
    {   
        //uint8_t instance = controller_id - USB_CONTROLLER_KHCI_0;  
        oscer_config_t   oscerConfig =
        {
            true,  // OSCERCLK enable.
            false, // OSCERCLK disable in STOP mode.
            0U,    // OSCERCLK divider setting.
        };     

        /* 
        * Four conditions need to be set for using USB HS PHY PLL
        * 1. 32kHz IRC clock enable
        * 2. external reference clock enable on XTAL
        * 3. USB PHY 1.2V PLL regulator enabled
        * 4. 3.3V USB regulator enabled, which means either VREGIN0 or VREGIN1 
        *    should be connected with 5V input
        */
        CLOCK_HAL_SetInternalRefClkEnableCmd(MCG, 1);
        CLOCK_SYS_SetOscerConfigration(0, &oscerConfig);

        SIM_HAL_SetUsbPhyPllRegulatorCmd(SIM, 1);
        SIM_HAL_EnableClock(SIM,kSimClockGateUsbhs0);
        SIM_HAL_EnableClock(SIM,kSimClockGateUsbhsphy0);
        OS_Time_delay(5);
        SIM_HAL_SetUsbVoltRegulatorOutputTargetCmd(SIM, kSimUsbvout3_310V);        
        SIM_USBPHYCTL |=  SIM_USBPHYCTL_USBVREGSEL_MASK | SIM_USBPHYCTL_USBDISILIM_MASK; 
        
    }
    else
    {
        ret = USBERR_BAD_STATUS; //unknown controller
    }

    return ret;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : usb_dev_ehci_phy_init
* Returned Value   : USB status
* Comments         :
*    This function performs usb ehci phy initialize
*
*END*----------------------------------------------------------------------*/
usb_status usb_dev_ehci_phy_init( uint8_t controller_id )
{
    if (USB_CONTROLLER_EHCI_0 == controller_id)
    {
        usb_hal_ehci_phy_trim_override_enable(controller_id); //override IFR value
            
        usb_hal_ehci_phy_pll_enalbe_power_up(controller_id); //power up PLL
            
        usb_hal_ehci_phy_select_pll_reference_clock(controller_id, crystal_val);

        usb_hal_ehci_phy_pll_clear_bypass(controller_id);  //clear bypass bit
            
        usb_hal_ehci_phy_enable_usb_clock(controller_id);  //enable USB clock output from USB PHY PLL
            
        usb_hal_ehci_phy_wait_pll_lock(controller_id); 
            
        usb_hal_ehci_phy_release_from_reset(controller_id);  //release PHY from reset
            
        usb_hal_ehci_phy_run_clock(controller_id); //Clear to 0 to run clocks

        usb_hal_ehci_phy_enable_utmi_level2(controller_id);
        usb_hal_ehci_phy_enable_utmi_level3(controller_id);

        usb_hal_ehci_phy_set_power_state_to_normal(controller_id);   //for normal operation

        usb_hal_ehci_phy_set_pfd_frac_value(controller_id,24);//N=24

        usb_hal_ehci_phy_select_pfd_clock_divider(controller_id,4);      //div by 4

        usb_hal_ehci_phy_disable_pulldown_resistor(controller_id);
        usb_hal_ehci_phy_disable_pfd_clock_gate(controller_id);
        usb_hal_ehci_phy_wait_pfd_stable(controller_id);
            
        usb_hal_ehci_phy_enable_tx_sync_mux(controller_id); 
        return USB_OK;
    }
    else
    {
        return USBERR_BAD_STATUS;
    }    

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
