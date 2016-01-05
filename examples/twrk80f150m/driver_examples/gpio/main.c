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
#include "gpio_pins.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

volatile bool isButtonPress = false;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Uses a switch to controll a LED.
 *
 * This function toogles LED1 when press the switch.
 */
int main(void)
{
    // Define gpio input pin config structure SW.
    gpio_input_pin_user_config_t inputPin[] = {
        {
            .pinName                       = BOARD_SW_GPIO,
            .config.isPullEnable           = true,
#if FSL_FEATURE_PORT_HAS_PULL_SELECTION
            .config.pullSelect             = kPortPullUp,
#endif
#if FSL_FEATURE_PORT_HAS_PASSIVE_FILTER
            .config.isPassiveFilterEnabled = false,
#endif
#if FSL_FEATURE_PORT_HAS_DIGITAL_FILTER
            .config.isDigitalFilterEnabled = false,
#endif
            .config.interrupt              = kPortIntFallingEdge,
        },
        {
            .pinName = GPIO_PINS_OUT_OF_RANGE,
        }
    };

    // Define gpio output pin config structure LED1.
    gpio_output_pin_user_config_t outputPin[] = {
        {
            .pinName              = kGpioLED1,
            .config.outputLogic   = 0,
#if FSL_FEATURE_PORT_HAS_SLEW_RATE
            .config.slewRate      = kPortFastSlewRate,
#endif
#if FSL_FEATURE_PORT_HAS_DRIVE_STRENGTH
            .config.driveStrength = kPortHighDriveStrength,
#endif
        },
        {
            .pinName = GPIO_PINS_OUT_OF_RANGE,
        }
    };

    // Init hardware
    hardware_init();

    // Print a note to terminal.
    PRINTF("\r\n GPIO PD Driver example\r\n");
    PRINTF("\r\n Press %s to turn on/off a LED1\r\n",BOARD_SW_NAME);

    // Init LED1, Switch.
    GPIO_DRV_Init(inputPin, outputPin);
    // Turn LED1 on.
    GPIO_DRV_ClearPinOutput(kGpioLED1);

    while(1)
    {
        if(isButtonPress)
        {
            PRINTF(" %s is pressed \r\n",BOARD_SW_NAME);
            // Reset state of button.
            isButtonPress=false;
        }
    }
}

/*!
 * @brief Interrupt service fuction of switch.
 *
 * This function toogles LED1
 */
void BOARD_SW_IRQ_HANDLER(void)
{
    // Clear external interrupt flag.
    GPIO_DRV_ClearPinIntFlag(BOARD_SW_GPIO);
    // Change state of button.
    isButtonPress = true;
    // Toggle LED1.
    GPIO_DRV_TogglePinOutput(kGpioLED1);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
