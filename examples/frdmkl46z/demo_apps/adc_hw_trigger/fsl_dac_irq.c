/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
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

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

// SDK Included Files
#include "fsl_dac_driver.h"

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

volatile uint32_t DAC_ISR_CurInstance = 0U;
volatile uint32_t DAC_ISR_IndexStartIntCounter = 0U;
volatile uint32_t DAC_ISR_IndexUpperIntCounter = 0U;
#if FSL_FEATURE_DAC_HAS_WATERMARK_SELECTION
    volatile uint32_t DAC_ISR_IndexWatermarkIntCounter = 0U;
#endif // FSL_FEATURE_DAC_HAS_WATERMARK_SELECTION

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/* Define array to keep run-time callback set by application. */
void (* volatile g_DacTestCallback[DAC_INSTANCE_COUNT])(void);

void DAC_TEST_InstallCallback(uint32_t instance, void (*callbackFunc)(void) )
{
    g_DacTestCallback[instance] = callbackFunc;
}

static void DAC_TEST_IRQHandler(uint32_t instance)
{
    if (g_DacTestCallback[instance])
    {
        (void)(*(g_DacTestCallback[instance]))();
    }
}

///////////////////////////////////////////////////////////////////////////////
// IRQ Handlers
///////////////////////////////////////////////////////////////////////////////

/* DAC IRQ handler that would cover the same name's APIs in startup code. */
void DAC0_IRQHandler(void)
{
    // Add user-defined ISR for DAC0
    DAC_TEST_IRQHandler(0U);
    
    // Clear flags
#if FSL_FEATURE_DAC_HAS_WATERMARK_SELECTION
    if ( DAC_DRV_GetFlag(0U, kDacBuffIndexWatermarkFlag) )
    {
        DAC_DRV_ClearFlag(0U, kDacBuffIndexWatermarkFlag);
    }
#endif // FSL_FEATURE_DAC_HAS_WATERMARK_SELECTION
    if ( DAC_DRV_GetFlag(0U, kDacBuffIndexStartFlag) )
    {
        DAC_DRV_ClearFlag(0U, kDacBuffIndexStartFlag);
    }
    if ( DAC_DRV_GetFlag(0U, kDacBuffIndexUpperFlag) )
    {
        DAC_DRV_ClearFlag(0U, kDacBuffIndexUpperFlag);
    }
}

#if (DAC_INSTANCE_COUNT > 1U)
void DAC1_IRQHandler(void)
{
    // Add user-defined ISR for DAC1
    DAC_TEST_IRQHandler(1U);

    // Clear flags
#if FSL_FEATURE_DAC_HAS_WATERMARK_SELECTION
    if ( DAC_DRV_GetFlag(1U, kDacBuffIndexWatermarkFlag) )
    {
        DAC_DRV_ClearFlag(1U, kDacBuffIndexWatermarkFlag);
    }
#endif // FSL_FEATURE_DAC_HAS_WATERMARK_SELECTION
    if ( DAC_DRV_GetFlag(1U, kDacBuffIndexStartFlag) )
    {
        DAC_DRV_ClearFlag(1U, kDacBuffIndexStartFlag);
    }
    if ( DAC_DRV_GetFlag(1U, kDacBuffIndexUpperFlag) )
    {
        DAC_DRV_ClearFlag(1U, kDacBuffIndexUpperFlag);
    }
}

#endif
