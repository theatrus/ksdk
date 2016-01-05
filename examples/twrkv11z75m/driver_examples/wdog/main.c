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
// Includes
///////////////////////////////////////////////////////////////////////////////
// Standard C Included Files
#include <stdio.h>
// Standard C Included Files
#include "board.h"
#include "fsl_wdog_driver.h"
#include "fsl_os_abstraction.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define BLINK_TIME         400   // The blink time is 400ms

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Check if button is pressed.
 *
 * This function gets the state of button.
 *
 * @return 0 if botton is not pressed.
 *         1 if botton is pressed
 */
static uint32_t is_key_pressed(void)
{
    return (!GPIO_DRV_ReadPinInput(BOARD_SW_GPIO));
}

/*!
 * @brief Watchdog main routine
 * Run a simple application which enables watchdog, then
 * continuously refreshes the watchdog to prevent CPU reset
 * Upon SW button push, the watchdog will expire after
 * approximately 2 seconds and issue reset
 */
int main(void)
{
    // Configure watchdog.
    const wdog_config_t wdogConfig =
    {
        .wdogEnable             = true,// Watchdog mode
        .timeoutValue          = 2048U,// Watchdog overflow time is about 2s
        .winEnable             = false, //Disable window function
        .windowValue           = 0,    // Watchdog window value
        .prescaler   = kWdogClkPrescalerDivide1, // Watchdog clock prescaler
        .updateEnable  = true, // Update register enabled
        .clkSrc           = kWdogLpoClkSrc, // Watchdog clock source is LPO 1KHz
#if FSL_FEATURE_WDOG_HAS_WAITEN
        .workMode.kWdogEnableInWaitMode  = true, // Enable watchdog in wait mode
#endif
        .workMode.kWdogEnableInStopMode  = true, // Enable watchdog in stop mode
        .workMode.kWdogEnableInDebugMode = false,// Disable watchdog in debug mode
    };

    // Init hardware.
    hardware_init();

    // Init OSA layer.
    OSA_Init();

    // Init pinsfor switch and led.

    GPIO_DRV_Init(switchPins, ledPins);
    // Initialize wdog before the WDOG timer has a chance
    //to reset the device

    // Turn LED1 on;
    LED1_ON;

    WDOG_DRV_Init(&wdogConfig);

    // If not wdog reset
    if (!(RCM->SRS0 & RCM_SRS0_WDOG_MASK))
    {
        PRINTF("\r\n WDOG example \r\n");
    }
    else
    // Check if WDOG reset occurred
    {
        PRINTF("\r\n WDOG reset occurred" );
    }

    PRINTF("\r\n Press %s to expire watchdog ",BOARD_SW_NAME);

    // Continue to run in loop to refresh watchdog until SW is pushed
    while (1)
    {
        // Check for SW button push.Pin is grounded when button is pushed.
        if (0 != is_key_pressed())
        {
            while (1)
            {
                // Button has been pushed,blink LED
                // showing that the watchdog is about to expire.
                LED1_TOGGLE;
                OSA_TimeDelay(BLINK_TIME);
            }
        }

        // Restart the watchdog so it doesn't reset.
        WDOG_DRV_Refresh();
        OSA_TimeDelay(100u);
    }

}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////

