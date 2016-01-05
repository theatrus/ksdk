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
* $FileName: usb_otg_bsp.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*         
*****************************************************************************/
#include "usb_otg_config.h"
#include "adapter.h"
#include "usb.h"
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_usb_khci_hal.h"
#include "fsl_gpio_driver.h"

extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);
extern uint32_t soc_get_usb_base_address(uint8_t controller_id);


#include "usb_otg.h"
#define BSP_USB_INT_LEVEL               (4)

#define MAX3353_INT_LEVEL               (4)
#define USB_CLK_RECOVER_IRC_EN          (*(volatile unsigned char *)0x40072144)
#define SIM_SOPT2_IRC48MSEL_MASK        0x30000u

static usb_khci_otg_int_struct_t g_khci0_otg_init_param = 
{
    NULL,
    0,
    BSP_USB_INT_LEVEL ,
};

static usb_otg_max3353_init_struct_t g_otg_max3353_init_param;

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : bsp_usb_otg_get_init_param
* Returned Value   :
* Comments         : Get USB OTG init param
*                  
*END*----------------------------------------------------------------------*/
void* bsp_usb_otg_get_init_param
(
    uint8_t controller_id
)
{
    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        return (void*)(&g_khci0_otg_init_param);
    }
    else
    {
        return NULL;
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : bsp_usb_otg_get_peripheral_init_param
* Returned Value   :
* Comments         : Get USB OTG perepheral param
*                  
*END*----------------------------------------------------------------------*/
void* bsp_usb_otg_get_peripheral_init_param
(
    uint8_t peripheral_id
)
{
    if (USB_OTG_PERIPHERAL_MAX3353 == peripheral_id)
    {
        return (void*)(&g_otg_max3353_init_param);
    }
    else
    {
        return NULL;
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : usb_otg_set_peripheral_init_param
* Returned Value   :
* Comments         : Set USB OTG perepheral param
*                  
*END*----------------------------------------------------------------------*/
usb_status usb_otg_set_peripheral_init_param
(
    uint8_t peripheral_id,
    usb_otg_peripheral_union_t *peripheral_param
)
{
    if (peripheral_param == NULL)
    {
        return USBERR_INVALID_PARAM;
    }
    
    if (USB_OTG_PERIPHERAL_MAX3353 == peripheral_id)
    {
        g_otg_max3353_init_param.i2c_address = peripheral_param->max3353_init.i2c_address;
        g_otg_max3353_init_param.i2c_channel = peripheral_param->max3353_init.i2c_channel;
        g_otg_max3353_init_param.i2c_vector = peripheral_param->max3353_init.i2c_vector;
        g_otg_max3353_init_param.int_pin = peripheral_param->max3353_init.int_pin;
        g_otg_max3353_init_param.int_port = peripheral_param->max3353_init.int_port;
        g_otg_max3353_init_param.int_vector = peripheral_param->max3353_init.int_vector;
        g_otg_max3353_init_param.priority = peripheral_param->max3353_init.priority;

        g_otg_max3353_init_param.interrupt_pin_init = peripheral_param->max3353_init.interrupt_pin_init;
        g_otg_max3353_init_param.interrupt_pin_flag_clear = peripheral_param->max3353_init.interrupt_pin_flag_clear;

        return USB_OK;
    }
    else
    {
        return USBERR_INVALID_PARAM;
    }
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : usb_otg_soc_clock_config
* Returned Value   : none
* Comments         :
*    This function sets the selection of the high frequency clock for various USB clock options
*
*END*----------------------------------------------------------------------*/
static usb_status usb_otg_soc_clock_config(uint8_t controller_id, usb_otg_clock_sel_t setting)
{
    usb_status ret = USB_OK;
    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        uint32_t freq;
        clock_usbfs_src_t src;
        uint8_t instance = controller_id - USB_CONTROLLER_KHCI_0;
        switch(setting)
        {
            case USB_OTG_EXTERNAL_CLKIN:
                CLOCK_SYS_DisableUsbfsClock(instance);
                ret = USBERR_BAD_STATUS;
                break;
            case USB_OTG_IRC_CLK48M:
                CLOCK_SYS_DisableUsbfsClock(instance);
                /* USB clock divider */
                CLOCK_SYS_SetUsbfsDiv(instance, 0U, 0U);
                /* PLL/FLL selected as CLK source */
                CLOCK_SYS_SetUsbfsSrc(instance, kClockUsbfsSrcPllFllSel);
                CLOCK_SYS_SetPllfllSel(kClockPllFllSelIrc48M);
                /* USB Clock Gating */
                CLOCK_SYS_EnableUsbfsClock(instance);
                /* Enable IRC 48MHz for USB module */
                USB_CLK_RECOVER_IRC_EN = 0x03;
                break;
            case USB_OTG_PLLFLL_CLK:
                CLOCK_SYS_DisableUsbfsClock(instance);
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
                break;
            default:
                ret = USBERR_BAD_STATUS;
                break;
        }
    }
    else
    {
        ret = USBERR_BAD_STATUS;
    }
    return ret;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : usb_otg_dev_soc_init
* Returned Value   : USB status
* Comments         :
*    This function performs BSP-specific initialization related to USB, device mode
*
*END*----------------------------------------------------------------------*/
usb_status usb_otg_dev_soc_init(uint8_t controller_id)
{
    usb_status ret = USB_OK;
#if USBCFG_OTG_USE_IRC48M
    ret = usb_otg_soc_clock_config(controller_id, USB_OTG_IRC_CLK48M);
#else
    ret = usb_otg_soc_clock_config(controller_id, USB_OTG_PLLFLL_CLK);
#endif
    return ret;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : usb_otg_host_soc_init
* Returned Value   : USB status
* Comments         :
*    This function performs BSP-specific initialization related to USB, host mode
*
*END*----------------------------------------------------------------------*/
usb_status usb_otg_host_soc_init(uint8_t controller_id)
{
    usb_status ret = USB_OK;
    
    ret = usb_otg_soc_clock_config(controller_id, USB_OTG_PLLFLL_CLK);
    
    return ret;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : usb_otg_soc_init
* Returned Value   : USB status
* Comments         :
*    This function performs BSP-specific I/O initialization related to USB
*
*END*----------------------------------------------------------------------*/
usb_status usb_otg_soc_init
(
   uint8_t controller_id
)
{
    usb_status ret = USB_OK;
    
    ret = usb_otg_soc_clock_config(controller_id, USB_OTG_PLLFLL_CLK);
    if (USB_OK != ret)
    {
        return ret;
    }

    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        g_khci0_otg_init_param.base_ptr = (void*)soc_get_usb_base_address(controller_id);
        g_khci0_otg_init_param.vector = soc_get_usb_vector_number(controller_id);


        MPU_CESR = (0);
        uint32_t base_address = soc_get_usb_base_address(controller_id);
        /* Configure enable USB regulator for device */
        SIM_HAL_SetUsbVoltRegulatorWriteCmd((SIM_Type*)(SIM_BASE), TRUE);
        SIM_HAL_SetUsbVoltRegulatorCmd((SIM_Type*)(SIM_BASE), TRUE);

        /* reset USB CTRL register */
        usb_hal_khci_reset_control_register(base_address);

        /* Enable internal pull-up resistor */
        usb_hal_khci_set_internal_pullup(base_address);
        usb_hal_khci_set_trc0(base_address); /* Software must set this bit to 1 */
        /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(controller_id), (uint8_t)g_khci0_otg_init_param.priority, 0, TRUE);
    }
    else
    {
        ret = USBERR_BAD_STATUS; /* unknown controller */
    }

    return ret;
}

/* EOF */
