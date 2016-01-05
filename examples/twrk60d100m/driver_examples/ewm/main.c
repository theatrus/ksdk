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
#include "board.h"
#include "fsl_ewm_driver.h"
#include "fsl_os_abstraction.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define EWM_INSTANCE       0
///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

volatile bool ewmIsrFlag = false;
////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Check if button is pressed.
 *
 * This function gets the state of button.
 *
 * @return 0 if button is not pressed.
 *         1 if button is pressed
 */
static uint32_t is_key_pressed(void)
{
    return (!GPIO_DRV_ReadPinInput(BOARD_SW_GPIO));
}
/*!
 * @brief EWM in watchdog mode.
 *
 * This function use EWM as a watchdog for an external circuit.
 * when the counter reaches high value, EWM enter interrupt.
 */
int main(void)
{
    // Configure EWM as watchdog
    // interrupt enables
    // pin input disables.
    ewm_config_t ewmUserConfig =
    {
        .ewmEnable        = true,
        .ewmInAssertLogic = false,
        .ewmInEnable      = false,
        .intEnable        = true,
        .ewmCmpLowValue   = 0x00,
        .ewmCmpHighValue  = 0xFEU,
#if FSL_FEATURE_EWM_HAS_PRESCALER
        .ewmPrescalerValue= 0x00,
#endif
    };

    // Init hardware.
    hardware_init();
    // Init OSA layer.
    OSA_Init();
    // Init pins for switch.
    GPIO_DRV_Init(switchPins, NULL);

    PRINTF("\r\n EWM example ");
    // Init EWM
    EWM_DRV_Init(EWM_INSTANCE, &ewmUserConfig);

    //Enable EWM interrupt.
    EWM_DRV_SetIntCmd(EWM_INSTANCE, true);

    PRINTF("\r\n Press %s to expire EWM ",BOARD_SW_NAME);
    while (1)
    {
        // Restart counter.
        EWM_DRV_Refresh(EWM_INSTANCE);
        // Check for SW button push.Pin is grounded when button is pushed.
        if (is_key_pressed())
        {
            //Check ewm interrupt occurr.
            while (!ewmIsrFlag);
            PRINTF("\r\n EWM interrupt is occurred");

            // Clear interrupt flag.
            ewmIsrFlag = false;
            PRINTF("\r\n Press %s to expire EWM again",BOARD_SW_NAME);

            // Restart counter & enable interrupt for next run
            EWM_DRV_Refresh(EWM_INSTANCE);
            // Delay 1s in order to press button, interrrupt occurred only once.
            OSA_TimeDelay(1000);
            //Enable EWM interrupt.
            EWM_DRV_SetIntCmd(EWM_INSTANCE, true);
        }
    }
}
/*!
 * @brief Interrupt service fuction of EWM.
 *
 * This function disables interrupt and set ewm interrupt flags.
 */
void WDOG_EWM_IRQHandler(void)
{
    // Disable EWM interrupt.
    EWM_DRV_SetIntCmd(EWM_INSTANCE, false);
    // Set ewm interrupt flag.
    ewmIsrFlag = true;
}
/*******************************************************************************
 * EOF
 *******************************************************************************/

