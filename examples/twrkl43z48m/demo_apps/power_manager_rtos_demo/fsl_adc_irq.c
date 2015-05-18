/*
 * Copyright (c) 2013 -2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_adc16_driver.h"
#include "adc16_temperature.h"
#include "lpm_rtos.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Define array to keep run-time callback set by application */
void (* volatile g_AdcCallback[ADC_INSTANCE_COUNT][ADC_SC1_COUNT])(void);
volatile uint16_t g_AdcValueInt[ADC_INSTANCE_COUNT][ADC_SC1_COUNT];


/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief User-defined function to install callback
 */
void ADC_InstallCallback(uint32_t instance, uint32_t chnGroup, void (*callbackFunc)(void) )
{
    g_AdcCallback[instance][chnGroup] = callbackFunc;
}
/*!
 * @brief User-defined function to read conversion value in ADC ISR
 */
uint16_t ADC_GetConvValueRAWInt(uint32_t instance, uint32_t chnGroup)
{
    return g_AdcValueInt[instance][chnGroup];
}
/*!
 * @brief User-defined ADC ISR
 */
static void ADC16_IRQHandler(uint32_t instance)
{
    uint32_t chnGroup;
    for (chnGroup = 0U; chnGroup < ADC_SC1_COUNT; chnGroup++)
    {
        if (   ADC16_DRV_GetChnFlag(instance, chnGroup, kAdcChnConvCompleteFlag) )
        {
            g_AdcValueInt[instance][chnGroup] = ADC16_DRV_GetConvValueRAW(instance, chnGroup);
            if ( g_AdcCallback[instance][chnGroup] )
            {
                (void)(*(g_AdcCallback[instance][chnGroup]))();
            }
        }
    }
}

#if (ADC_INSTANCE_COUNT > 0)
/* ADC IRQ handler that would cover the same name's APIs in startup code */
void MODULE_IRQ_HANDLER(ADC0)(void)
{
    // Add user-defined ISR for ADC0
    ADC16_IRQHandler(0U);
}
#endif

#if (ADC_INSTANCE_COUNT > 1U)
void MODULE_IRQ_HANDLER(ADC1)(void)
{
    // Add user-defined ISR for ADC1
    ADC16_IRQHandler(1U);
}
#endif

#if (ADC_INSTANCE_COUNT > 2U)
void MODULE_IRQ_HANDLER(ADC2)(void)
{
    // Add user-defined ISR for ADC2. */
    ADC16_IRQHandler(2U);
}
#endif

#if (ADC_INSTANCE_COUNT > 3U)
void MODULE_IRQ_HANDLER(ADC3)(void)
{
    // Add user-defined ISR for ADC3
    ADC16_IRQHandler(3U);
}
#endif
/*******************************************************************************
 * EOF
 ******************************************************************************/
