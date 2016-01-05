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

// SDK Included Files
#include "board.h"
#include "fsl_clock_manager.h"
#include "fsl_lpuart_driver.h"

///////////////////////////////////////////////////////////////////////////////
//  Consts
///////////////////////////////////////////////////////////////////////////////

const uint8_t buffStart[]    = "\r\n++++++++++++++++ LPUART Send/Receive Blocking Example +++++++++++++++++\r\n";
const uint8_t bufferData1[]  = "\r\nType characters from keyboard, the board will receive and then echo them to terminal screen\r\n";

///////////////////////////////////////////////////////////////////////////////
//  Code
///////////////////////////////////////////////////////////////////////////////

/*
 * This example will example the efficiency of the transmit/receive drivers with
 * using blocking method. Transfer data between board and PC. Board will transfer
 * and receive characters with PC through LPUART interface. Type characters from
 * keyboard, the board will receive and then echo them to terminal screen.
 * Look for intructions output to the terminal.
 */

/*!
 * @brief Check send/receive blocking functionality
 *
 */
int main(void)
{
    uint8_t rxChar = 0, txChar = 0;
    uint32_t byteCountBuff = 0;

    // Initialize variable lpuartStatePtr of type lpuart_state_t
    lpuart_state_t lpuartStatePtr;

    // Fill in lpuart config data
    lpuart_user_config_t lpuartConfig = {
        .clockSource     = BOARD_LPUART_CLOCK_SOURCE,
        .bitCountPerChar = kLpuart8BitsPerChar,
        .parityMode      = kLpuartParityDisabled,
        .stopBitCount    = kLpuartOneStopBit,
        .baudRate        = BOARD_DEBUG_UART_BAUD
    };

    // Enable clock for PORTs, setup board clock source
    hardware_init();

    // Enable LPTIMER for timeout
    OSA_Init();

    // Initialize the lpuart module with instance number and config structure
    LPUART_DRV_Init(BOARD_DEBUG_UART_INSTANCE, &lpuartStatePtr, &lpuartConfig);

    // Inform to start blocking example
    byteCountBuff = sizeof(buffStart);
    LPUART_DRV_SendDataBlocking(BOARD_DEBUG_UART_INSTANCE, buffStart, byteCountBuff, 1000u);

    // Inform user of what to do
    byteCountBuff = sizeof(bufferData1);
    LPUART_DRV_SendDataBlocking(BOARD_DEBUG_UART_INSTANCE, bufferData1, byteCountBuff, 1000u);

    // Send/receive blocking function
    while(true)
    {
        // Wait to receive input data
        if (kStatus_LPUART_Success == LPUART_DRV_ReceiveDataBlocking(BOARD_DEBUG_UART_INSTANCE, &rxChar, 1, 1000u))
        {
            // Echo received character
            txChar = rxChar;
            LPUART_DRV_SendDataBlocking(BOARD_DEBUG_UART_INSTANCE, &txChar, 1, 1000u);
        }
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/

