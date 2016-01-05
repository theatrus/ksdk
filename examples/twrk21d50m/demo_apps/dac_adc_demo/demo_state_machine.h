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
 *
 */

#ifndef __TERMINAL_MENU_H__
#define __TERMINAL_MENU_H__
///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

// Standard C Included Files
#include <stdint.h>
// SDK Included Files
#include "board.h"
#include "fsl_dac_driver.h"
#include "fsl_adc16_driver.h"
#if defined(BOARD_USE_LPUART)
#include "fsl_lpuart_driver.h"
#else
#include "fsl_uart_driver.h"
#endif
#include "fsl_debug_console.h"


///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define DAC_1_0_VOLTS  1241U
#define DAC_1_5_VOLTS  1862U
#define DAC_2_0_VOLTS  2482U
#define DAC_2_5_VOLTS  3103U
#define DAC_3_0_VOLTS  3724U

#define VREF_BRD  3.300
#define SE_12BIT  4096.0

typedef enum Demo_States
{
    kStart,
    kConfigDevice,
    kSetDAC,
    kWait,
    kGetADC,
    kDeinitDevice,
    kEndState,
    kStop
}demo_state_t;

typedef struct Demo_StateMachine
{
    demo_state_t state;
    uint8_t (*demo_func)(demo_state_t *prevState);
}demo_machine_t;

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Function to print out welcome message and pins required by demo.
 *
 * @param *prevState Pointer to previous state for state machine.
 *
 * @return msg Returns the character entered into the terminal by user.
 */
uint8_t demo_start(demo_state_t *prevState);

/*!
 * @brief Function configure DAC and ADC. DAC is configured for software updates. The ADC is set in 'Blocking Mode'.
 *
 * @param *prevState Pointer to previous state for state machine.
 *
 * @return msg Returns 0.
 */
uint8_t device_config(demo_state_t *prevState);

/*!
 * @brief Function sets output level on DAC.
 *
 * @param *prevState Pointer to previous state for state machine.
 *
 * @return msg Returns the character entered into the terminal by user.
 */
uint8_t dac_set(demo_state_t *prevState);

/*!
 * @brief Function to perform a wait and possible state change based on *prevState.
 *
 * @param *prevState Pointer to previous state for state machine.
 *
 * @return msg Returns 0..
 */
uint8_t wait_state(demo_state_t *prevState);

/*!
 * @brief Function to get ADC values from channel connected to DAC output.
 *
 * @param *prevState Pointer to previous state for state machine.
 *
 * @return msg Returns the character entered into the terminal by user.
 */
uint8_t adc_get(demo_state_t *prevState);

/*!
 * @brief Function deinitializes the DAC and ADC module following a user command to do terminate demo. Will also free allocated memory.
 *
 * @param *prevState Pointer to previous state for state machine.
 *
 * @return msg Returns 0..
 */
uint8_t device_deinit(demo_state_t *prevState);

/*!
 * @brief Function indicates to the user that the demo has been terminated.
 *
 * @param *prevState Pointer to previous state for state machine.
 *
 * @return msg Returns 0..
 */
uint8_t demo_end(demo_state_t *prevState);

#endif // __TERMINAL_MENU_H__
