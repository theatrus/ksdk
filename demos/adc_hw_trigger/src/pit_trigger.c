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
#include "adc_hw_trigger.h"
#include "fsl_pit_driver.h"

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

extern const uint32_t gSimBaseAddr[];

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @Brief enable the trigger source of PIT0, chn0
 */
void init_trigger_source(uint32_t adcInstance)
{
    uint32_t freqUs;

    freqUs = 1000000U/(INPUT_SIGNAL_FREQ*NR_SAMPLES)*2;

    pit_user_config_t pitUserInit = {
        .isInterruptEnabled = false,
        .isTimerChained = false,
        .periodUs = freqUs,
    };

    // Init PIT0 module and enable run in debug
    PIT_DRV_Init(0, true);

    // Initialize PIT0 timer 0
    PIT_DRV_InitChannel(0, 0, &pitUserInit);  

    // Start the PIT timer
    PIT_DRV_StartTimer(0, 0);

    // Configure SIM for ADC hw trigger source selection
    SIM_HAL_SetAdcAlternativeTriggerCmd(gSimBaseAddr[0], adcInstance, true);
    SIM_HAL_SetAdcPreTriggerMode(gSimBaseAddr[0], adcInstance, kSimAdcPretrgselA);
    SIM_HAL_SetAdcTriggerMode(gSimBaseAddr[0], adcInstance, kSimAdcTrgSelPit0);
}

/*!
 * @Brief disable the trigger source
 */
void deinit_trigger_source(uint32_t adcInstance)
{
    PIT_DRV_StopTimer(0, 0);
}
