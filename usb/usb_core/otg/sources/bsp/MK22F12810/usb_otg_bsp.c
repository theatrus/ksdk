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
#include "usb_device_config.h"
#include "usb_misc.h"
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "usb.h"
#include "fsl_usb_khci_hal.h"
#include "fsl_gpio_driver.h"
#define SIM_SOPT2_IRC48MSEL_MASK                 0x30000u

extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);

#include "usb_otg.h"
#define BSP_USB_INT_LEVEL               (4)
#define USB_CLK_RECOVER_IRC_EN          (*(volatile unsigned char *)0x40072144)

#define MAX3353_INT_LEVEL               (4)
#define MAX3353_INT_PIN                 16U                 /*MAX3353_INT_PIN*/
/* TODO: Move this structure to other place */
/* struct contains max3353 init params */
#define KHCI_BASE_PTR                   USB0_BASE           /* KHCI_BASE_PTR */
#define KHCI_VECTOR                     USB0_IRQn           /* KHCI_VECTOR */
#define MAX3353_INT_PORT                PORTA_BASE          /* MAX3353_INT_PORT */
#define MAX3353_INT_VECTOR              PORTA_IRQn          /* MAX3353_INT_VECTOR */
#define MAX3353_I2C_CHANNEL             0                   /* I2C Channel */
#define MAX3353_I2C_VECTOR              I2C0_IRQn           /* I2C Vector */

/* Declare USB instance */
static int32_t usb_instance = 0;
/* Declare USB OTG default clock source */
#define USB_OTG_DEFAULT_CLK             USB_OTG_PLLFLL_CLK

/* Declare max3353 interrupt pin */
#define kGpiomax3353int                 GPIO_MAKE_PIN(GPIOA_IDX, 16U)

/* Declare max3353 low level trigger interrupt pin for the usb otg demo */
const gpio_input_pin_user_config_t max3353lowlevelintPin[] =
{
    {
        .pinName = kGpiomax3353int,
        .config.isPullEnable = true,
        .config.pullSelect = kPortPullUp,
        .config.isPassiveFilterEnabled = false,
        .config.interrupt = kPortIntLogicZero
    },
    {
        .pinName = GPIO_PINS_OUT_OF_RANGE,
    }
};

/* Declare max3353  falling edge interrupt pin for the usb otg demo */
const gpio_input_pin_user_config_t max3353fallingedgeintPin[] =
{
    {
        .pinName = kGpiomax3353int,
        .config.isPullEnable = true,
        .config.pullSelect = kPortPullUp,
        .config.isPassiveFilterEnabled = false,
        .config.interrupt = kPortIntFallingEdge
    },
    {
        .pinName = GPIO_PINS_OUT_OF_RANGE,
    }
};

/* Declare max3353 no interrupt pin for the usb otg demo */
const gpio_input_pin_user_config_t max3353nointerruptintPin[] =
{
    {
        .pinName = kGpiomax3353int,
        .config.isPullEnable = true,
        .config.pullSelect = kPortPullUp,
        .config.isPassiveFilterEnabled = false,
        .config.interrupt = kPortIntDisabled
    },
    {
        .pinName = GPIO_PINS_OUT_OF_RANGE,
    }
};

static usb_khci_otg_int_struct_t g_khci0_otg_init_param =
{
    (void*) KHCI_BASE_PTR,
    KHCI_VECTOR,
    BSP_USB_INT_LEVEL,
};

static usb_otg_max3353_init_struct_t g_otg_max3353_init_param =
{
    (void*) MAX3353_INT_PORT,
    MAX3353_INT_PIN,
    MAX3353_INT_VECTOR,
    MAX3353_INT_LEVEL,
    MAX3353_I2C_CHANNEL,
    MAX3353_I2C_VECTOR,
    0x2C
};

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : bsp_usb_otg_max3353_pin_int_clear
 * Returned Value   : none
 * Comments         :
 *    This function clears the pin interrupt flag associated with the max3353 interrupt pin 
 *
 *END*----------------------------------------------------------------------*/
void bsp_usb_otg_max3353_clear_pin_int_flag(void)
{
    GPIO_DRV_ClearPinIntFlag(kGpiomax3353int);
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : bsp_usb_otg_max3353_set_pin_int
 * Returned Value   : none
 * Comments         :
 *    This function enables/disables the pin interrupt associated with the max3353 interrupt pin 
 *
 *END*----------------------------------------------------------------------*/
void bsp_usb_otg_max3353_set_pin_int
(
    bool level,
    bool enable
    )
{
    if (enable)
    {
        if (level)/* interrupt is triggered  by low level */
        {
            GPIO_DRV_Init(max3353lowlevelintPin, NULL);
        }
        else/* interrupt is triggered by falling edge */
        {
            GPIO_DRV_Init(max3353fallingedgeintPin, NULL);
        }
    }
    else
    {
        GPIO_DRV_Init(max3353nointerruptintPin, NULL);
    }
}

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
    if (controller_id == USB_CONTROLLER_KHCI_0)
    {
        return (void*) (&g_khci0_otg_init_param);
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
    if (peripheral_id == USB_OTG_PERIPHERAL_MAX3353)
    {
        return (void*) (&g_otg_max3353_init_param);
    }
    else
    {
        return NULL;
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
int32_t usb_otg_soc_clock_config(usb_otg_clock_sel_t setting)
{
    int32_t ret = USB_OK;
    uint32_t freq;
    clock_usbfs_src_t src;
    switch(setting)
    {
    case USB_OTG_EXTERNAL_CLKIN:
        CLOCK_SYS_DisableUsbfsClock (usb_instance);
        ret = USBERR_BAD_STATUS;
        break;
    case USB_OTG_IRC_CLK48M:
        CLOCK_SYS_DisableUsbfsClock(usb_instance);
        /* USB clock divider */
        CLOCK_SYS_SetUsbfsDiv(usb_instance, 0U, 0U);
        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetUsbfsSrc(usb_instance, kClockUsbfsSrcPllFllSel);
        CLOCK_SYS_SetPllfllSel (kClockPllFllSelIrc48M);
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(usb_instance);
        /* Enable IRC 48MHz for USB module */
        USB_CLK_RECOVER_IRC_EN = 0x03;
        break;
    case USB_OTG_PLLFLL_CLK:
        CLOCK_SYS_DisableUsbfsClock(usb_instance);
        /* PLL/FLL selected as CLK source */
        CLOCK_SYS_SetPllfllSel (kClockPllFllSelPll);
        CLOCK_SYS_SetUsbfsSrc(usb_instance, kClockUsbfsSrcPllFllSel);
        /* USB clock divider */
        src = CLOCK_SYS_GetUsbfsSrc(usb_instance);
        switch(src)
        {
        case kClockUsbfsSrcExt:
            break;
        case kClockUsbfsSrcPllFllSel:
            freq = CLOCK_SYS_GetPllFllClockFreq();
            switch(freq)
            {
            case 120000000U:
                CLOCK_SYS_SetUsbfsDiv(usb_instance, 4, 1);
                break;
            case 96000000U:
                CLOCK_SYS_SetUsbfsDiv(usb_instance, 1, 0);
                break;
            case 72000000U:
                CLOCK_SYS_SetUsbfsDiv(usb_instance, 2, 1);
                break;
            case 48000000U:
                CLOCK_SYS_SetUsbfsDiv(usb_instance, 0, 0);
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
        /* USB Clock Gating */
        CLOCK_SYS_EnableUsbfsClock(usb_instance);
        break;
    default:
        ret = USBERR_BAD_STATUS;
        break;
    }
    return ret;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : bsp_usb_otg_io_init
 * Returned Value   : USB status
 * Comments         :
 *    This function performs BSP-specific I/O initialization related to USB
 *
 *END*----------------------------------------------------------------------*/
static int32_t bsp_usb_otg_io_init
(
    int32_t i
    )
{
    int32_t ret = USB_OK;
    if (USB_CONTROLLER_KHCI_0 == i)
    {

        usb_instance = i;
        ret = usb_otg_soc_clock_config(USB_OTG_DEFAULT_CLK);
        /* Weak pull downs */
        usb_hal_khci_set_weak_pulldown(USB0_BASE);
    }
    else
    {
        ret = USBERR_BAD_STATUS; /* unknown controller */
    }
    return ret;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : usb_otg_soc_init
 * Returned Value   : USB status
 * Comments         :
 *    This function performs BSP-specific initialization related to USB
 *
 *END*----------------------------------------------------------------------*/
int32_t usb_otg_soc_init(uint8_t controller_id)
{
    int32_t result = 0;

    result = bsp_usb_otg_io_init(controller_id);

    if (result != 0)
    {
        return result;
    }

    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        /* Configure enable USB regulator for device */
//        SIM_HAL_SetUsbVoltRegulatorWriteCmd((SIM_Type*)(SIM_BASE), TRUE);
//        SIM_HAL_SetUsbVoltRegulatorCmd((SIM_Type*)(SIM_BASE), TRUE);
        /* reset USB CTRL register */
        usb_hal_khci_reset_control_register(USB0_BASE);

        /* Enable internal pull-up resistor */
        usb_hal_khci_set_internal_pullup(USB0_BASE);
        usb_hal_khci_set_trc0(USB0_BASE); /* Software must set this bit to 1 */
        /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
    }
    else
    {
        /* unknown controller */
        result = USBERR_BAD_STATUS;
    }
    return result;
}
/* EOF */
