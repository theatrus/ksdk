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
// SDK Included Files
#include "board.h"
#include "fsl_gpio_driver.h"
#include "fsl_cop_driver.h"
#include "fsl_rcm_hal.h"
#include "fsl_os_abstraction.h"
#include "fsl_debug_console.h"

/********************* IMPORTANT ***********************
* when running this example, to enable COP you must change
* definition DISABLE_WDOG into 0 in file system_MKLxxxx.h
********************************************************/
#if (DISABLE_WDOG)
    #error "Watchdog is disable. To enable watchdog to run this example, change \
    definition DISABLE_WDOG into 0 in file system_MKLxxxx.h "
#endif

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define COP_INSTANCE        0

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
 * @brief COP reset chip.
 *
 * Run a simple application which enables watchdog, then
 * continuously refreshes the watchdog to prevent CPU reset
 * Upon SW button push, the watchdog will expire after
 * approximately 1 seconds and chip will reset.
 */
int main(void)
{
    cop_config_t copInit =
    {
        .copWindowModeEnable = (uint8_t)false,
#if FSL_FEATURE_COP_HAS_LONGTIME_MODE
        .copTimeoutMode      = kCopShortTimeoutMode,
        .copStopModeEnable   = (uint8_t)false,
        .copDebugModeEnable  = (uint8_t)false,
#endif
        .copClockSource      = kCopLpoClock,
        // COP reset after about 1s.
        .copTimeout          = kCopTimeout_short_2to10_or_long_2to18
    };

    // Init hardware.
    hardware_init();

    // Init OSA layer.
    OSA_Init();

    // Init LED and switch.
    GPIO_DRV_Init(switchPins, ledPins);

    // Check if WDOG reset occurred , turn off LED1, wait to press any key to continue.
    if (RCM_HAL_GetSrcStatus(RCM, kRcmWatchDog) == kRcmWatchDog)
    {
        PRINTF("\r\n COP reset the chip successfully\r\n");
    }
    // If WDOG reset is not occurred, enables COP.
    else
    {
        //Print a note.
        PRINTF("COP example begin.\r\n");
    }

    // Init COP module.
    COP_DRV_Init(COP_INSTANCE, &copInit);

    // Turn on LED1
    LED1_ON;
    // Print a message
    PRINTF("Press %s to begin expiring COP \r\n",BOARD_SW_NAME);
    while (1)
    {
        // COP keep refreshing until SW is pressed.
        if (is_key_pressed())
        {
            PRINTF("Board will reset after 1 seconds.\r\n");
            while(1)
            {
                // LED is blinking and wait for chip reset.
                LED1_TOGGLE;
                OSA_TimeDelay(100u);
            }
        }
        // Reset COP counter.
        COP_DRV_Refresh(COP_INSTANCE);
        // Delay for the next refresh.
        OSA_TimeDelay(100u);
    }
}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
