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
* $FileName: usb_otg_bsp.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*         
*****************************************************************************/
#include "adapter.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_device_registers.h"
#define SIM_SOPT2_IRC48MSEL_MASK                 0x30000u
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)||(OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX))
#include "MK65F18.h"
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "usb_otg.h"
#define BSP_USB_INT_LEVEL                (4)
#define USB_CLK_RECOVER_IRC_EN (*(volatile unsigned char *)0x40072144)
#define SIM_SOPT2_IRC48MSEL_MASK                 0x30000u
#define BSPCFG_USB_USE_IRC48M            (1)
#define BSP_USB_OTG_MAX3353_INT_LEVEL    (4)
/* TODO: Move this structure to other place */
/* struct contains max3353 init params */
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#define KHCI_BASE_PTR                USB0_BASE_PTR      /* KHCI_BASE_PTR */
#define KHCI_VECTOR                  INT_USB0           /* KHCI_VECTOR */
#define MAX_3353_INT_PORT            PORTE_BASE_PTR     /* MAX_3353_INT_PORT */
#define MAX3353_VECTOR               INT_PORTE          /* MAX3353_VECTOR */
#define I2C_CHANNEL                  "i2c0:"            /* I2C Channel */
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
#define KHCI_BASE_PTR                USB0_BASE_PTR      /* KHCI_BASE_PTR */
#define KHCI_VECTOR                  INT_USB0           /* KHCI_VECTOR */
#define MAX_3353_INT_PORT            PORTE_BASE_PTR     /* MAX_3353_INT_PORT */
#define MAX3353_VECTOR               INT_PORTE          /* MAX3353_VECTOR */
#define I2C_CHANNEL                  0                  /* I2C Channel */
#endif
#define MAX3353_INT_PIN              27 /*MAX3353_INT_PIN*/

static const usb_khci_otg_int_struct_t g_khci0_otg_init_param = 
{
    (void*)KHCI_BASE_PTR,
    KHCI_VECTOR,
    BSP_USB_INT_LEVEL ,
};

static const usb_otg_max3353_init_struct_t g_otg_max3353_init_param = 
{
    (void*)MAX_3353_INT_PORT,
    MAX3353_INT_PIN,
    MAX3353_VECTOR,
    BSP_USB_OTG_MAX3353_INT_LEVEL,
    I2C_CHANNEL,
    0x2C
};
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_otg_max3353_pin_int_clear
* Returned Value   : none
* Comments         :
*    This function clears the pin interrupt flag associated with the max3353 interrupt pin 
*
*END*----------------------------------------------------------------------*/
void _bsp_usb_otg_max3353_clear_pin_int_flag()
{
    if (PORT_ISFR_REG(MAX_3353_INT_PORT) & (1<<MAX3353_INT_PIN))
    {
        PORT_ISFR_REG(MAX_3353_INT_PORT) |= 1<<MAX3353_INT_PIN;
    }
}    

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_otg_max3353_set_pin_int
* Returned Value   : none
* Comments         :
*    This function enables/disables the pin interrupt associated with the max3353 interrupt pin 
*
*END*----------------------------------------------------------------------*/
void _bsp_usb_otg_max3353_set_pin_int
(
    bool level,
    bool enable
)
{
    if (enable)
    {
        if (level)/* interrupt is triggered  by low level */  
        {
            PORT_PCR_REG(MAX_3353_INT_PORT,MAX3353_INT_PIN) |=  0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x08);
        }
        else/* interrupt is triggered by falling edge */
        {
            PORT_PCR_REG(MAX_3353_INT_PORT,MAX3353_INT_PIN) |=  0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x0A);
        }
    }
    else
    {
       PORT_PCR_REG(MAX_3353_INT_PORT,MAX3353_INT_PIN) |=  0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x00);
    }
}

void* bsp_usb_otg_get_init_param
(
    uint8_t controller_id
)
{
    if (controller_id == USB_CONTROLLER_KHCI_0)
    {
        return (void*)(&g_khci0_otg_init_param);
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
        return (void*)(&g_otg_max3353_init_param);
    }
    else
    {
        return NULL;
    }
}

static int32_t bsp_usb_otg_io_init
(
    int32_t i
)
{
    if (i == 0)
    {
#if BSPCFG_USB_USE_IRC48M

    /*
    * Configure SIM_CLKDIV2: USBDIV = 0, USBFRAC = 0
    */
    SIM_CLKDIV2 = (uint32_t)0x0UL; /* Update USB clock prescalers */

    /* Configure USB to be clocked from IRC 48MHz */
    SIM_SOPT2_REG(SIM_BASE_PTR)  |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_IRC48MSEL_MASK;

    /* Enable USB-OTG IP clocking */
    SIM_SCGC4_REG(SIM_BASE_PTR) |= SIM_SCGC4_USBOTG_MASK;
    /* Enable IRC 48MHz for USB module */
    USB_CLK_RECOVER_IRC_EN = 0x03;
#else
    /* Configure USB to be clocked from PLL0 */
    SIM_SOPT2_REG(SIM_BASE_PTR) |= SIM_SOPT2_USBSRC_MASK;
    /* Configure USB divider to be 120MHz * 2 / 5 = 48 MHz */
    SIM_CLKDIV2_REG(SIM_BASE_PTR) = SIM_CLKDIV2_USBDIV(4) | SIM_CLKDIV2_USBFRAC_MASK;

    /* Enable USB-OTG IP clocking */
    SIM_SCGC4_REG(SIM_BASE_PTR) |= SIM_SCGC4_USBOTG_MASK;
#endif
    }
    else
    {
        return -1; //unknow controller
    }

    return 0;
}

int32_t bsp_usb_otg_init(uint8_t controller_id)
{
    int32_t result = 0;

    result = bsp_usb_otg_io_init(controller_id);

    if (result != 0)
    {
        return result;
    }
    /* MPU is disabled. All accesses from all bus masters are allowed */
    MPU_CESR=0;
    if (0 == controller_id)
    {
        /* enable clock gate for port D, the PTD14 is conected to INT pin of max3353*/
        SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
        /* Configure enable USB regulator for device */
        SIM_SOPT1_REG(SIM_BASE_PTR) |= SIM_SOPT1_USBREGEN_MASK;

        /* reset USB CTRL register */
        USB_USBCTRL_REG(USB0_BASE_PTR) = 0;
        
        /* Enable internal pull-up resistor */
        USB_USBTRC0_REG(USB0_BASE_PTR) |= 0x40; /* Software must set this bit to 1 */
        /* setup interrupt */
        OS_intr_init(soc_get_usb_vector_number(0), BSP_USB_INT_LEVEL, 0, TRUE);
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
