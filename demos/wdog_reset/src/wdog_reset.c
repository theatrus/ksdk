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

// SDK Included Files
#include "fsl_wdog_driver.h"
#include "fsl_os_abstraction.h"
#include "board.h"
#include "fsl_clock_manager.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define SLOW_BLINK_TIME         80    // The slow blink time is 80ms*10=800ms
#define FAST_BLINK_TIME         400   // The fast blink time is 400ms

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

static void toggle_active_led(void)
{
    GPIO_DRV_TogglePinOutput(kGpioLED2);
}

static uint32_t is_key_pressed(void)
{
    uint32_t pinState;
    pinState = GPIO_DRV_ReadPinInput(kGpioSW1);
    return (!pinState);
}

/*! 
 * @brief Print chip reset reason
 *
 * print chip reset reson to the terminal
 */
static void print_reset_reason(void)
{
    // Determine the last cause(s) of reset
    printf("\r\n\r\n********************************");
    if (RCM->SRS1 & RCM_SRS1_SW_MASK)
    {
        printf("\r\nSoftware Reset");
    }
    if (RCM->SRS1 & RCM_SRS1_LOCKUP_MASK)
    {
        printf("\r\nCore Lockup Event Reset");
    }
#if FSL_FEATURE_RCM_HAS_JTAG
    if (RCM->SRS1 & RCM_SRS1_JTAG_MASK)
    {
        printf("\r\nJTAG Reset");
    }
#endif
    if (RCM->SRS0 & RCM_SRS0_POR_MASK)
    {
        printf("\r\nPower-on Reset");
    }
    if (RCM->SRS0 & RCM_SRS0_PIN_MASK)
    {
        printf("\r\nExternal Pin Reset");
    }
    if (RCM->SRS0 & RCM_SRS0_WDOG_MASK)
    {
        printf("\r\nWatchdog(COP) Reset");
    }
    if (RCM->SRS0 & RCM_SRS0_LOC_MASK)
    {
        printf("\r\nLoss of Clock Reset");
    }
    if (RCM->SRS0 & RCM_SRS0_LVD_MASK)
    {
        printf("\r\nLow-voltage Detect Reset");
    }
    if (RCM->SRS0 & RCM_SRS0_WAKEUP_MASK)
    {
        printf("\r\nLLWU Reset");
    }
}

/*!
 * @brief Watchdog main routine 
 *
 * Run a simple application which enables watchdog, then
 * continuously refreshes the watchdog to prevent CPU reset
 * Upon SW1 button push, the watchdog will expire after approximately 2 seconds and issue reset
 * To debug, use the OpenSDA debugger connection to allow recovery after watchdog reset, this way
 * the debug connection can be resumed after reset.  
 * Recovery not possible when debugging with JLink and powering through OpenSDA.
 *
 */
int main(void)
{
    uint32_t delay, wdogDemoLoopCount = 0U;

    const wdog_user_config_t wdogInit = 
    {
        .timeoutValue = 2048U,  // Watchdog overflow time is about 2s
        .windowValue = 0U,      // Watchdog window value, 0--disable window function
        .clockPrescalerValue = kWdogClockPrescalerValueDevide1, // Watchdog clock prescaler
        .updateRegisterEnable = true, // Update register enabled
        .clockSource = kClockWdogSrcLpoClk, // Watchdog clock source is LPO 1KHz
        .workInWaitModeEnable = true, // Enable watchdog in wait mode
        .workInStopModeEnable = true, // Enable watchdog in stop mode
        .workInDebugModeEnable = false, // Disable watchdog in debug mode
    };

    OSA_Init();
    hardware_init();
    dbg_uart_init();
    GPIO_DRV_Init(switchPins, ledPins);

    // Initialize wdog before the WDOG timer has a chance to reset the device
    WDOG_DRV_Init(&wdogInit);

    // print chip reset reason
    print_reset_reason();

    // if not wdog reset, clear reset count
    if (!(RCM->SRS0 & RCM_SRS0_WDOG_MASK)) 
    {
        WDOG_DRV_ClearResetCount();
    }

    printf("\r\nWatchdog(cop) reset count: %u", (unsigned int)WDOG_DRV_GetResetCount());
    
    /////////////////////////////////////////////////////////////////////
    // Continue to run in loop to refresh watchdog until SW1 is pushed //
    /////////////////////////////////////////////////////////////////////
    while (1) 
    {
        // LED toggle shows we are refreshing the watchdog
        toggle_active_led();

        // delay before we refresh the watchdog, check for SW1 button push which forces watchog to expire
        for (delay = 0U; delay < 10U; delay++)
        {
            // Check for SW1 button push.  Pin is grounded when button is pushed
            if (is_key_pressed())
            {
              while (1) 
              {
                // button has been pushed, blink LED rapidly showing that the watchdog is about to expire
                toggle_active_led();
                OSA_TimeDelay(FAST_BLINK_TIME);
              } 
            }

            OSA_TimeDelay(SLOW_BLINK_TIME);
        }
        // Refresh the watchdog so we don't reset
        WDOG_DRV_Refresh();
        
        // Print total loops
        printf("\n\rWatchdog example running, Loop #: %u, press <SW> to start watchdog timeout...",
               (unsigned int)wdogDemoLoopCount++);
    } 
}
