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
#include "adapter.h"
#include "usb.h"
#include "usb_otg_main.h"
#include "usb_otg.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_usb_khci_hal.h"
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
#if (defined(CPU_MK60D10))
#include "MK60D10/MK60D10_sim.h"
#include "MK60D10/MK60D10_usb.h"
#include "MK60D10/MK60D10_gpio.h"
#include "MK60D10/MK60D10_port.h"
#include "MK60D10/MK60D10_mpu.h"
#endif
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include "MK60D10.h"
#endif

extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);
extern uint32_t soc_get_usb_base_address(uint8_t controller_id);
#ifdef __cplusplus
extern "C"
{
#endif
extern _WEAK_FUNCTION(usb_status bsp_usb_otg_board_init(uint8_t controller_id));
#ifdef __cplusplus
}
#endif

#include "usb_otg.h"
#define BSP_USB_INT_LEVEL                (4)
#define MAX3353_INT_LEVEL                (4)
/* TODO: Move this structure to other place */
/* struct contains max3353 init params */
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#define MAX3353_INT_PORT             PORTA_BASE_PTR     /* MAX3353_INT_PORT */
#define MAX3353_INT_VECTOR           INT_PORTA          /* MAX3353_VECTOR */
#define MAX3353_I2C_CHANNEL          "i2c0:"            /* I2C Channel */
#define MAX3353_I2C_VECTOR            INT_I2C0          /* I2C Vector */
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
#define MAX3353_INT_PORT             HW_PORTA           /* MAX3353_INT_PORT */
#define MAX3353_INT_VECTOR           (PORTA_IRQn + 6)   /* MAX3353_VECTOR */
#define MAX3353_I2C_CHANNEL          0                  /* I2C Channel */
#define MAX3353_I2C_VECTOR           (I2C0_IRQn + 16)   /* I2C Vector */
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#define MAX3353_INT_PORT              PORTA_BASE          /* MAX3353_INT_PORT */
#define MAX3353_INT_VECTOR            PORTA_IRQn          /* MAX3353_INT_VECTOR */
#define MAX3353_I2C_CHANNEL           0                   /* I2C Channel */
#define MAX3353_I2C_VECTOR            I2C0_IRQn           /* I2C Vector */
#endif
#define MAX3353_INT_PIN               26U                  /*MAX3353_INT_PIN*/

static usb_khci_otg_int_struct_t g_khci0_otg_init_param =
{
    NULL,
    0,
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

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#define kGpiomax3353int GPIO_MAKE_PIN(GPIOA_IDX, MAX3353_INT_PIN)

/* Declare max3353 low level trigger interrupt pin for the usb otg demo */
gpio_input_pin_user_config_t max3353intPin[] =
{
{
    .pinName = kGpiomax3353int,
    .config.isPullEnable = true,
    .config.pullSelect = kPortPullUp,
    .config.isPassiveFilterEnabled = false,
    .config.interrupt = kPortIntLogicZero /* kPortIntDisabled, kPortIntFallingEdge */
},
{
    .pinName = GPIO_PINS_OUT_OF_RANGE,
}
};
#endif

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
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    if (PORT_ISFR_REG((PORT_MemMapPtr)g_otg_max3353_init_param.int_port) & (1u << g_otg_max3353_init_param.int_pin))
    {
        PORT_ISFR_REG((PORT_MemMapPtr)g_otg_max3353_init_param.int_port) |= 1 << g_otg_max3353_init_param.int_pin;
    }
#else
    GPIO_DRV_ClearPinIntFlag(kGpiomax3353int);
#endif
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
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
if (enable)
{
    if (level)/* interrupt is triggered  by low level */
    {
        PORT_WR_PCR((PORT_Type*)g_otg_max3353_init_param.int_port, g_otg_max3353_init_param.int_pin, 0 | PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0x08));
    }
    else/* interrupt is triggered by falling edge */
    {
        PORT_WR_PCR((PORT_Type*)g_otg_max3353_init_param.int_port, g_otg_max3353_init_param.int_pin, 0 | PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0x0A));
    }
}
else
{
    PORT_WR_PCR((PORT_Type*)g_otg_max3353_init_param.int_port, g_otg_max3353_init_param.int_pin, 0 | PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0x00));
}
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
if (enable)
{
    if (level)/* interrupt is triggered  by low level */
    {
        PORT_PCR_REG((PORT_MemMapPtr)g_otg_max3353_init_param.int_port, g_otg_max3353_init_param.int_pin) |= 0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x08);
    }
    else/* interrupt is triggered by falling edge */
    {
        PORT_PCR_REG((PORT_MemMapPtr)g_otg_max3353_init_param.int_port, g_otg_max3353_init_param.int_pin) |= 0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x0A);
    }
}
else
{
    PORT_PCR_REG((PORT_MemMapPtr)g_otg_max3353_init_param.int_port, g_otg_max3353_init_param.int_pin) |= 0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x00);
}
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
if (enable)
{
    if (level)
    {
        max3353intPin[0].config.interrupt = kPortIntLogicZero;
        GPIO_DRV_Init(max3353intPin, NULL);
    }
    else
    {
        max3353intPin[0].config.interrupt = kPortIntFallingEdge;
        GPIO_DRV_Init(max3353intPin, NULL);
    }
}
else
{
    max3353intPin[0].config.interrupt = kPortIntDisabled;
    GPIO_DRV_Init(max3353intPin, NULL);
}
#endif
}

void* bsp_usb_otg_get_init_param
(
    uint8_t controller_id
)
{
    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        return (void*) (&g_khci0_otg_init_param);
    }
    else
    {
        return NULL;
    }
}

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
* Function Name    : bsp_usb_otg_set_peripheral_init_param
* Returned Value   :
* Comments         : Set USB OTG perepheral param
*                  
*END*----------------------------------------------------------------------*/
usb_status bsp_usb_otg_set_peripheral_init_param
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
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)    
        g_otg_max3353_init_param.i2c_dev_id = peripheral_param->max3353_init.i2c_dev_id;
#else
        g_otg_max3353_init_param.i2c_channel = peripheral_param->max3353_init.i2c_channel;
#endif
        g_otg_max3353_init_param.i2c_vector = peripheral_param->max3353_init.i2c_vector;
        g_otg_max3353_init_param.int_pin = peripheral_param->max3353_init.int_pin;
        g_otg_max3353_init_param.int_port = peripheral_param->max3353_init.int_port;
        g_otg_max3353_init_param.int_vector = peripheral_param->max3353_init.int_vector;
        g_otg_max3353_init_param.priority = peripheral_param->max3353_init.priority;

        return USB_OK;
    }
    else
    {
        return USBERR_INVALID_PARAM;
    }
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : bsp_usb_otg_clock_config
 * Returned Value   : none
 * Comments         :
 *    This function sets the selection of the high frequency clock for various USB clock options
 *
 *END*----------------------------------------------------------------------*/
static usb_status bsp_usb_otg_clock_config(int32_t controller_id, usb_otg_clock_sel_t setting)
{
    usb_status ret = USB_OK;
    uint8_t usb_instance;

    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        usb_instance = controller_id - USB_CONTROLLER_KHCI_0;
    #if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
        uint32_t freq;
        clock_usbfs_src_t src;

        CLOCK_SYS_DisableUsbfsClock(usb_instance);
        switch(setting)
        {
        case USB_OTG_EXTERNAL_CLKIN:
            ret = USBERR_BAD_STATUS;
            break;
        case USB_OTG_IRC_CLK48M:
            ret = USBERR_BAD_STATUS;
            break;
        case USB_OTG_PLLFLL_CLK:
            /* PLL/FLL selected as CLK source */
            CLOCK_SYS_SetPllfllSel (kClockPllFllSelPll);
            CLOCK_SYS_SetUsbfsSrc(usb_instance, kClockUsbfsSrcPllFllSel);
            /* USB clock divider */
            src = CLOCK_SYS_GetUsbfsSrc(usb_instance);
            ret = USB_OK;
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

            /* Confirm the USB souce frequency is 48MHz */
            if (48000000U != CLOCK_SYS_GetUsbfsFreq(usb_instance))
            {
                ret = USBERR_BAD_STATUS;
            }
            /* USB Clock Gating */
            CLOCK_SYS_EnableUsbfsClock(usb_instance);
            break;
        default:
            ret = USBERR_BAD_STATUS;
            break;
        }

    #else
    #endif
    }
    else
    {
        ret = USBERR_BAD_STATUS;
    }

    return ret;
}

static usb_status bsp_usb_otg_soc_init(uint8_t controller_id)
{
    usb_status ret = USB_OK;
    uint32_t base_addres = 0;

    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
        g_khci0_otg_init_param.base_ptr = (void*)soc_get_usb_base_address(controller_id);
        g_khci0_otg_init_param.vector = soc_get_usb_vector_number(controller_id);
        
        base_addres = soc_get_usb_base_address(controller_id);
        ret = bsp_usb_otg_clock_config(controller_id, USB_OTG_PLLFLL_CLK);
        if (ret != USB_OK)
        {
            return ret;
        }
    #if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
        /* Configure enable USB regulator for device */
        SIM_HAL_SetUsbVoltRegulatorWriteCmd((SIM_Type*) (SIM_BASE), TRUE);
        SIM_HAL_SetUsbVoltRegulatorCmd((SIM_Type*) (SIM_BASE), TRUE);
        /* reset USB CTRL register */
        usb_hal_khci_reset_control_register(base_addres);

        /* MPU is disabled. All accesses from all bus masters are allowed */
        MPU_CESR = 0;

        /* setup interrupt */
        OS_intr_init((IRQn_Type) soc_get_usb_vector_number(controller_id), g_khci0_otg_init_param.priority, 0, TRUE);

    #elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
        /* Configure enable USB regulator for device */
        SIM_SOPT1 |= (SIM_SOPT1_USBREGEN_MASK);
        /* reset USB CTRL register */
        USB0_USBCTRL = (0);

        /* MPU is disabled. All accesses from all bus masters are allowed */
        MPU_CESR=0;

        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(controller_id), g_khci0_otg_init_param.priority, 0, TRUE);
    #else
        /* Configure enable USB regulator for device */
        HW_SIM_SOPT1_SET(SIM_SOPT1_USBREGEN_MASK);
        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(0);

        /* MPU is disabled. All accesses from all bus masters are allowed */
        HW_MPU_CESR_WR(0);

        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(controller_id), BSP_USB_INT_LEVEL, 0, TRUE);
    #endif
    }
    else
    {
        ret = USBERR_BAD_STATUS;
    }

    return ret;
}

_WEAK_FUNCTION(usb_status bsp_usb_otg_board_init(uint8_t controller_id))
{
    usb_status ret = USB_OK;

    if (USB_CONTROLLER_KHCI_0 == controller_id)
    {
    #if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)

    #else

    #endif
    }
    else
    {
        ret = USBERR_BAD_STATUS;
    }

    return ret;
}

usb_status bsp_usb_otg_init(uint8_t controller_id)
{
    usb_status ret = USB_OK;

    ret = bsp_usb_otg_soc_init(controller_id);
    if (ret == USB_OK)
    {
        ret = bsp_usb_otg_board_init(controller_id);
    }

    return ret;
}

usb_status bsp_usb_otg_host_init(uint8_t controller_id)
{
    usb_status ret = USB_OK;

    ret = bsp_usb_otg_clock_config(controller_id, USB_OTG_PLLFLL_CLK);

    return ret;
}

usb_status bsp_usb_otg_dev_init(uint8_t controller_id)
{
    usb_status ret = USB_OK;

    ret = bsp_usb_otg_clock_config(controller_id, USB_OTG_PLLFLL_CLK);

    return ret;
}

/* EOF */
