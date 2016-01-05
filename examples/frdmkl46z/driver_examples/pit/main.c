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

#include <stdio.h>

// SDK Included Files
#include "board.h"
#include "gpio_pins.h"
#include "fsl_pit_driver.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
//  Definitions
///////////////////////////////////////////////////////////////////////////////

#define BOARD_PIT_INSTANCE  0

///////////////////////////////////////////////////////////////////////////////
//  Variables
///////////////////////////////////////////////////////////////////////////////

volatile bool pitIsrFlag[2] = {false};

///////////////////////////////////////////////////////////////////////////////
//  Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Simple example for PIT driver
 *          Measure the time of the generated pulse using oscilloscope on LED1 and LED2.
 *          LED2 will toggle faster 2 times than LED1.
 *          Find the pin of LED1, LED2 from gpio_pins.c and board schematics.
 */

/*!
 * @brief Example single timer period and interrupt on multiple channels
 *
 */
int main(void)
{
    // Structure of initialize PIT channel No.0
    pit_user_config_t chn0Confg = {
        .isInterruptEnabled = true,
        .periodUs = 1000000u
    };

    // Structure of initialize PIT channel No.1
    pit_user_config_t chn1Confg = {
        .isInterruptEnabled = true,
        .periodUs = 2000000u
    };

    // Initialize hardware
    hardware_init();

    // Enable LED indicator
    LED1_EN;
    LED2_EN;

    // Init pit module and enable run in debug
    PIT_DRV_Init(BOARD_PIT_INSTANCE, false);

    // Initialize PIT timer instance for channel 0 and 1
    PIT_DRV_InitChannel(BOARD_PIT_INSTANCE, 0, &chn0Confg);
    PIT_DRV_InitChannel(BOARD_PIT_INSTANCE, 1, &chn1Confg);

    // Start channel 0
    PRINTF("\r\nStarting channel No.0 ...");
    PIT_DRV_StartTimer(BOARD_PIT_INSTANCE, 0);

    // Start channel 1
    PRINTF("\r\nStarting channel No.1 ...");
    PIT_DRV_StartTimer(BOARD_PIT_INSTANCE, 1);

    while (true)
    {
        // Check whether occur interupt and toggle LED
        if (true == pitIsrFlag[0])
        {
            PRINTF("\r\n Channel No.0 interrupt is occured !");
            LED1_TOGGLE;
            pitIsrFlag[0] = false;
        }

        if (true == pitIsrFlag[1])
        {
            PRINTF("\r\n Channel No.1 interrupt is occured !");
            LED2_TOGGLE;
            pitIsrFlag[1] = false;
        }
    }

}

/*******************************************************************************
 * EOF
 ******************************************************************************/

