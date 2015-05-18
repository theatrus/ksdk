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
// Standard C Included Files
#include <stdio.h>
#include <string.h>

// SDK Included Files
#include "fsl_tpm_driver.h"
#include "board.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Use FTM in PWM mode
 *
 * This function use PWM to controll brightness of a LED.
 * LED is brighter and then dimmer, continuously.
 */
int main(void)
{
    volatile uint32_t cnt = 0;
    int32_t step = 1;
    tpm_general_config_t driverInfo;
    tpm_pwm_param_t param = {
            .mode              = kTpmEdgeAlignedPWM,
            .edgeMode          = kTpmHighTrue,
            .uFrequencyHZ      = 120000u,
            .uDutyCyclePercent = 99u
    };
    // Init hardware
    hardware_init();

    // Print a note.
    PRINTF("See the change of LED brightness \r\n");

    // Prepare memory for initialization.
    memset(&driverInfo, 0, sizeof(driverInfo));
    // Init TPM.
    TPM_DRV_Init(BOARD_TPM_INSTANCE, &driverInfo);

    // Set clock for TPM.
    TPM_DRV_SetClock(BOARD_TPM_INSTANCE, kTpmClockSourceModuleHighFreq, kTpmDividedBy2);

    while(1)
    {
        // Init PWM module with updated configuration.
        TPM_DRV_PwmStart(BOARD_TPM_INSTANCE, &param, BOARD_TPM_CHANNEL);

        // Delay a while to see LED.
        for(cnt = 0; cnt < 200000u; cnt++)
        {
            __asm("nop");
        }

        // Change up or down direction of brightness.
        if((param.uDutyCyclePercent >= 100) || (param.uDutyCyclePercent <= 0))
        {
            step *= -1;
        }

        // Update duty cycle.
        param.uDutyCyclePercent += step;
    }
}
///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////

