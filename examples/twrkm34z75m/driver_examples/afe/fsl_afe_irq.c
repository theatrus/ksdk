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

#include "fsl_afe_driver.h"

/* Define array to keep run-time callback set by application. */
void (* volatile g_AfeExampleCallback[AFE_INSTANCE_COUNT][FSL_FEATURE_AFE_CHANNEL_NUMBER])(void);
void AFE_EXAMPLE_InstallCallback(uint32_t instance, uint32_t chn, void (*callbackFunc)(void) );

void AFE_EXAMPLE_InstallCallback(uint32_t instance, uint32_t chn, void (*callbackFunc)(void) )
{
    g_AfeExampleCallback[instance][chn] = callbackFunc;
}

static void AFE_EXAMPLE_IRQHandler(uint32_t instance, uint32_t chn)
{
    if (g_AfeExampleCallback[instance][chn])
    {
        (void)(*(g_AfeExampleCallback[instance][chn]))();
    }
}

/******************************************************************************
 * IRQ Handlers
 *****************************************************************************/
/* AFE IRQ handler that would cover the same name's APIs in startup code. */
void AFE_CH0_IRQHandler(void)
{
    /* Add user-defined ISR for AFE chn 0. */
    AFE_EXAMPLE_IRQHandler(0U, 0U);
}

void AFE_CH1_IRQHandler(void)
{
    /* Add user-defined ISR for AFE chn 1. */
    AFE_EXAMPLE_IRQHandler(0U, 1U);
}

void AFE_CH2_IRQHandler(void)
{
    /* Add user-defined ISR for AFE chn 2. */
    AFE_EXAMPLE_IRQHandler(0U, 2U);
}

void AFE_CH3_IRQHandler(void)
{
    /* Add user-defined ISR for AFE chn 3. */
    AFE_EXAMPLE_IRQHandler(0U, 3U);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
