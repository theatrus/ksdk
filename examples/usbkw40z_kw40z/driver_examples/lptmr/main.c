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
// SDK Included Files
#include "fsl_lptmr_driver.h"
#include "board.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define LPTMR_INSTANCE     0U

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

volatile uint32_t lptmrCounter=0;

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief LPTMR interrupt callback
 */
void lptmr_isr_callback(void)
{
    lptmrCounter++;
    LED1_TOGGLE;
}

/*!
 * @brief The example uses LPTMR to generate interrupt each 1 second.
 *        When interrupt occurs, LED1 changes status & print to terminal
 */
int main (void)
{
    lptmr_state_t lptmrState;
    uint32_t      currentCounter = 0;
    // Configure LPTMR.
    lptmr_user_config_t lptmrUserConfig =
    {
        .timerMode            = kLptmrTimerModeTimeCounter, /*! Use LPTMR in Time Counter mode */
        .freeRunningEnable    = false, /*! When hit compare value, set counter back to zero */
        .prescalerEnable      = false, /*! bypass prescaler */
        .prescalerClockSource = kClockLptmrSrcLpoClk, /*! use 1kHz Low Power Clock */
        .isInterruptEnabled   = true
    };

    // Init hardware.
    hardware_init();

    LED1_EN;
    // Initialize LPTMR
    LPTMR_DRV_Init(LPTMR_INSTANCE, &lptmrState, &lptmrUserConfig);

    // Set the timer period for 1 second
    LPTMR_DRV_SetTimerPeriodUs(LPTMR_INSTANCE,1000000);

    // Specify the callback function when a LPTMR interrupt occurs
    LPTMR_DRV_InstallCallback(LPTMR_INSTANCE,lptmr_isr_callback);

    PRINTF("Low Power Timer Example\r\n");

    // Start counting
    LPTMR_DRV_Start(LPTMR_INSTANCE);
    while(1)
    {
        if(currentCounter != lptmrCounter)
        {
            currentCounter = lptmrCounter;
            PRINTF("LPTMR interrupt No.%d \r\n",currentCounter);
        }
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
