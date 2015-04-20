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
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
    #if (defined(CPU_MK22F25612))
#include "MK22F25612/MK22F25612_sim.h"
#include "MK22F25612/MK22F25612_usb.h"
#include "MK22F25612/MK22F25612_gpio.h"
#include "MK22F25612/MK22F25612_port.h"
    #endif
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include "MK22F25612.h"
#endif

extern uint8_t soc_get_usb_vector_number(uint8_t controller_id);

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
#define MAX_3353_INT_PORT            PORTA_BASE_PTR     /* MAX_3353_INT_PORT */
#define MAX3353_VECTOR               INT_PORTA          /* MAX3353_VECTOR */
#define I2C_CHANNEL                  "i2c0:"            /* I2C Channel */
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
#define KHCI_BASE_PTR                USB0_BASE          /* KHCI_BASE_PTR */
#define KHCI_VECTOR                  (USB0_IRQn + 16)   /* KHCI_VECTOR */
#define MAX_3353_INT_PORT            HW_PORTA         /* MAX_3353_INT_PORT */
#define MAX3353_VECTOR               (PORTA_IRQn + 16)        /* MAX3353_VECTOR */
#define I2C_CHANNEL                  0                  /* I2C Channel */
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#define KHCI_BASE_PTR                USB0_BASE          /* KHCI_BASE_PTR */
#define KHCI_VECTOR                  USB0_IRQn 		    /* KHCI_VECTOR */
#define MAX_3353_INT_PORT            PORTA_BASE         /* MAX_3353_INT_PORT */
#define MAX3353_VECTOR               PORTA_IRQn         /* MAX3353_VECTOR */
#define I2C_CHANNEL                  0                  /* I2C Channel */
#endif
#define MAX3353_INT_PIN              16 /*MAX3353_INT_PIN*/
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
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    if (PORT_ISFR_REG(MAX_3353_INT_PORT) & (1<<MAX3353_INT_PIN))
    {
        PORT_ISFR_REG(MAX_3353_INT_PORT) |= 1<<MAX3353_INT_PIN;
    }
#else
     if (HW_PORT_ISFR_RD(MAX_3353_INT_PORT) & (1<<MAX3353_INT_PIN))
    {
        HW_PORT_ISFR_SET(MAX_3353_INT_PORT,1<<MAX3353_INT_PIN);
    }
#endif
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
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)||(OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK))
    if (enable)
    {
        if (level)/* interrupt is triggered  by low level */  
        {
            HW_PORT_PCRn_WR(MAX_3353_INT_PORT,MAX3353_INT_PIN,0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x08));    
        }
        else/* interrupt is triggered by falling edge */
        {
            HW_PORT_PCRn_WR(MAX_3353_INT_PORT,MAX3353_INT_PIN,0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x0A));
        }
    }
    else
    {
        HW_PORT_PCRn_WR(MAX_3353_INT_PORT,MAX3353_INT_PIN,0|PORT_PCR_MUX(1)|PORT_PCR_PE_MASK|PORT_PCR_PS_MASK|PORT_PCR_IRQC(0x00));    
    }
#else
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
#endif
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

#define USBHS_USBMODE_CM_IDLE_MASK    USBHS_USBMODE_CM(0)
#define USBHS_USBMODE_CM_DEVICE_MASK  USBHS_USBMODE_CM(2)             
#define USBHS_USBMODE_CM_HOST_MASK    USBHS_USBMODE_CM(3)

static int32_t bsp_usb_otg_io_init
(
   int32_t i
)
{
    if (i == 0)
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#if BSPCFG_USB_USE_IRC48M
        /*
        * Configure SIM_CLKDIV2: USBDIV = 0, USBFRAC = 0
        */
        //HW_SIM_CLKDIV2_CLR(SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV_MASK);
        SIM_CLKDIV2 &= ~(SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV_MASK);
        /* Configure USB to be clocked from IRC 48MHz */
        //HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_IRC48MSEL_MASK);
        SIM_SOPT2 |= (SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_IRC48MSEL_MASK);
        /* Enable USB-OTG IP clocking */
        //HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);
        SIM_SCGC4 |= (SIM_SCGC4_USBOTG_MASK);
        /* Enable IRC 48MHz for USB module */
        USB_CLK_RECOVER_IRC_EN = 0x03;
#else
        /* Configure USBFRAC = 0, USBDIV = 0 => frq(USBout) = 1 / 1 * frq(PLLin) */
        //HW_SIM_CLKDIV2_CLR(SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV_MASK);
        SIM_CLKDIV2 &= ~(SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV_MASK);
        /* Configure USB to be clocked from PLL */
        //HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL_MASK);
        SIM_SOPT2 |= (SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(0x01));
        /* Enable USB-OTG IP clocking */
        //HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);
        SIM_SCGC4 |= (SIM_SCGC4_USBOTG_MASK);
#endif
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#if BSPCFG_USB_USE_IRC48M
        /*
        * Configure SIM_CLKDIV2: USBDIV = 0, USBFRAC = 0
        */
        HW_SIM_CLKDIV2_CLR(SIM_BASE, SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV_MASK);
        /* Configure USB to be clocked from IRC 48MHz */
        HW_SIM_SOPT2_SET(SIM_BASE, SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_IRC48MSEL_MASK);
        /* Enable USB-OTG IP clocking */
        HW_SIM_SCGC4_SET(SIM_BASE, SIM_SCGC4_USBOTG_MASK);
        /* Enable IRC 48MHz for USB module */
        USB_CLK_RECOVER_IRC_EN = 0x03;
#else
        /* Configure USBFRAC = 0, USBDIV = 0 => frq(USBout) = 1 / 1 * frq(PLLin) */
        HW_SIM_CLKDIV2_CLR(SIM_BASE, SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV_MASK);
        /* Configure USB to be clocked from PLL */
        HW_SIM_SOPT2_SET(SIM_BASE, SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(0x01));
        /* Enable USB-OTG IP clocking */
        HW_SIM_SCGC4_SET(SIM_BASE, SIM_SCGC4_USBOTG_MASK);
#endif

        /* Weak pull downs */
        HW_USB_USBCTRL_WR(USB0_BASE, 0x40);
    #if KHCICFG_HOST_PORT_NATIVE
        /* Souce the P5V0_K22_USB. Set PTC9 to high */
        BW_PORT_PCRn_MUX(HW_PORTC, 9, 1); /* GPIO mux */
        HW_GPIO_PDDR_SET(2, 1<<9);        /* Set output */
        HW_GPIO_PSOR_SET(2, 1<<9);        /* Output high */
    #endif
#else
#if BSPCFG_USB_USE_IRC48M
        /*
        * Configure SIM_CLKDIV2: USBDIV = 0, USBFRAC = 0
        */
        HW_SIM_CLKDIV2_CLR(SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV_MASK);
        /* Configure USB to be clocked from IRC 48MHz */
        HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_IRC48MSEL_MASK);
        /* Enable USB-OTG IP clocking */
        HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);
        /* Enable IRC 48MHz for USB module */
        USB_CLK_RECOVER_IRC_EN = 0x03;
#else
        /* Configure USBFRAC = 0, USBDIV = 0 => frq(USBout) = 1 / 1 * frq(PLLin) */
        HW_SIM_CLKDIV2_CLR(SIM_CLKDIV2_USBFRAC_MASK | SIM_CLKDIV2_USBDIV_MASK);
        /* Configure USB to be clocked from PLL */
        HW_SIM_SOPT2_SET(SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(0x01));
        /* Enable USB-OTG IP clocking */
        HW_SIM_SCGC4_SET(SIM_SCGC4_USBOTG_MASK);
#endif

        /* Weak pull downs */
        HW_USB_USBCTRL_WR(0x40);
    #if KHCICFG_HOST_PORT_NATIVE
        /* Souce the P5V0_K22_USB. Set PTC9 to high */
        BW_PORT_PCRn_MUX(HW_PORTC, 9, 1); /* GPIO mux */
        HW_GPIO_PDDR_SET(2, 1<<9);        /* Set output */
        HW_GPIO_PSOR_SET(2, 1<<9);        /* Output high */
    #endif
#endif
    }
    else
    {
        return -1; /* unknow controller */
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

    if (0 == controller_id)
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
        /* Configure enable USB regulator for device */
        SIM_SOPT1CFG |= (SIM_SOPT1CFG_URWE_MASK);
        SIM_SOPT1 |= (SIM_SOPT1_USBREGEN_MASK);

        /* reset USB CTRL register */
        USB0_USBCTRL = 0;

        /* Enable internal pull-up resistor */
        USB0_CONTROL = (USB_CONTROL_DPPULLUPNONOTG_MASK);
        USB0_USBTRC0 |= (0x40); /* Software must set this bit to 1 */
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
        /* Configure enable USB regulator for device */
        HW_SIM_SOPT1CFG_SET(SIM_BASE, SIM_SOPT1CFG_URWE_MASK);
        HW_SIM_SOPT1_SET(SIM_BASE, SIM_SOPT1_USBREGEN_MASK);

        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(USB0_BASE, 0);

        /* Enable internal pull-up resistor */
        HW_USB_CONTROL_WR(USB0_BASE, USB_CONTROL_DPPULLUPNONOTG_MASK);
        HW_USB_USBTRC0_SET(USB0_BASE, 0x40); /* Software must set this bit to 1 */
#else
        /* Configure enable USB regulator for device */
        HW_SIM_SOPT1CFG_SET(SIM_SOPT1CFG_URWE_MASK);
        HW_SIM_SOPT1_SET(SIM_SOPT1_USBREGEN_MASK);

        /* reset USB CTRL register */
        HW_USB_USBCTRL_WR(0);

        /* Enable internal pull-up resistor */
        HW_USB_CONTROL_WR(USB_CONTROL_DPPULLUPNONOTG_MASK);
        HW_USB_USBTRC0_SET(0x40); /* Software must set this bit to 1 */
#endif
        /* setup interrupt */
        OS_intr_init((IRQn_Type)soc_get_usb_vector_number(0), BSP_USB_INT_LEVEL, 0, TRUE);
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
