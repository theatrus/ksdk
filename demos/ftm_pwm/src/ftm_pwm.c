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

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Main function 
 */
int main(void)
{
    ftm_pwm_param_t ftmParam = {
        .mode                   = kFtmEdgeAlignedPWM,
        .edgeMode               = kFtmLowTrue,
        .uFrequencyHZ           = 240000,
        .uDutyCyclePercent      = 0,
        .uFirstEdgeDelayPercent = 0,
    };
    ftm_user_config_t ftmInfo;

    // Configure board specific pin muxing
    hardware_init();

    OSA_Init();
    // Initialize UART terminal
    dbg_uart_init();

    // Print a note to show the start of demo
    printf("\r\nWelcome to FTM PWM demo!\n\n\r");

    configure_ftm_pins(BOARD_FTM_INSTANCE);

    memset(&ftmInfo, 0, sizeof(ftmInfo));

    ftmInfo.syncMethod = kFtmUseSoftwareTrig;
    FTM_DRV_Init(BOARD_FTM_INSTANCE, &ftmInfo);

    while(1)
    {
        FTM_DRV_PwmStart(BOARD_FTM_INSTANCE, &ftmParam, BOARD_FTM_CHANNEL);
        // Issue a software trigger to update registers
        FTM_HAL_SetSoftwareTriggerCmd(g_ftmBaseAddr[BOARD_FTM_INSTANCE], true);

        // delay 50ms
        OSA_TimeDelay(50);

        if (ftmParam.uDutyCyclePercent++ >= 100)
        {
            ftmParam.uDutyCyclePercent = 0;
        }
    }
}
