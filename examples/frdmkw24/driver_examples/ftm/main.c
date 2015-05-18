/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
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
//  Includes
///////////////////////////////////////////////////////////////////////////////

// Standard C Included Files
#include <stdio.h>
#include <string.h>
// SDK Included Files
#include "board.h"
#include "fsl_ftm_driver.h"
#include "fsl_os_abstraction.h"
#include "fsl_debug_console.h"

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Use FTM in PWM mode
 *
 * This function use PWM to controll brightness of a LED.
 * LED is brighter and then dimmer, continuously.
 */
int main(void)
{
    bool brightnessUp = true;       /*! Indicate LED is brighter or dimmer*/
    ftm_user_config_t ftmInfo;
    // Configure ftm params with frequency 24kHZ
    ftm_pwm_param_t ftmParam = {
        .mode                   = kFtmEdgeAlignedPWM,
        .edgeMode               = kFtmLowTrue,
        .uFrequencyHZ           = 24000u,
        .uDutyCyclePercent      = 0,
        .uFirstEdgeDelayPercent = 0,
    };
    // Init hardware
    hardware_init();

    // Init OSA.
    OSA_Init();

    // Print a note to terminal.
    PRINTF("\r\nWelcome to FTM example\n\r");
    PRINTF("\r\nSee the change of LED brightness");

#if defined (FRDM_K22F)
    LED1_EN;
    LED1_OFF;
#endif

    // Initialize FTM module,
    // configure for software trigger.
    memset(&ftmInfo, 0, sizeof(ftmInfo));
    ftmInfo.syncMethod = kFtmUseSoftwareTrig;
    FTM_DRV_Init(BOARD_FTM_INSTANCE, &ftmInfo);
    FTM_DRV_SetClock(BOARD_FTM_INSTANCE, kClock_source_FTM_SystemClk, kFtmDividedBy1);

    while(1)
    {
        // Start PWM mode with updated duty cycle.
        FTM_DRV_PwmStart(BOARD_FTM_INSTANCE, &ftmParam, BOARD_FTM_CHANNEL);

        // Software trigger to update registers.
        FTM_HAL_SetSoftwareTriggerCmd(g_ftmBase[BOARD_FTM_INSTANCE], true);

        // Delay 50ms to see the change of LED brightness.
        OSA_TimeDelay(50u);

        if(brightnessUp)
        {
            // Increase duty cycle until it reach limited value
            if (++ftmParam.uDutyCyclePercent == 100u)
                brightnessUp = false;
        }
        else
        {
            // Decrease duty cycle until it reach limited value.
            if (--ftmParam.uDutyCyclePercent == 0)
                brightnessUp = true;
        }
    } // end while

}

/*******************************************************************************
 * EOF
 ******************************************************************************/

