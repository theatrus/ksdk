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
#include "fsl_uart_driver.h"
#include "fsl_clock_manager.h"

///////////////////////////////////////////////////////////////////////////////
//  Consts
///////////////////////////////////////////////////////////////////////////////
const uint8_t buffStart[]   = "\n\r++++++++++++++++ UART Send/Receive Non Blocking Example +++++++++++++++++\n\r";
const uint8_t bufferData1[] = "\n\rType characters from keyboard, the board will receive and then echo them to terminal screen\n\r";

///////////////////////////////////////////////////////////////////////////////
//  Code
///////////////////////////////////////////////////////////////////////////////

/*
 * This example will example the efficiency of the transmit/receive drivers with
 * using non blocking method. Transfer data between board and PC. Board will transfer
 * and receive characters with PC through UART interface. Type characters from
 * keyboard, the board will receive and then echo them to terminal screen.
 * Look for intructions output to the terminal.
 */

/*!
 * @brief Check send/receive non blocking functionality
 *
 */
int main(void)
{
    uint8_t rxChar, txChar;
    uint32_t byteCountBuff = 0;

    // Initialize variable uartState of type uart_state_t
    uart_state_t uartState;

    // Fill in uart config data
    uart_user_config_t uartConfig = {
        .bitCountPerChar = kUart8BitsPerChar,
        .parityMode      = kUartParityDisabled,
        .stopBitCount    = kUartOneStopBit,
        .baudRate        = BOARD_DEBUG_UART_BAUD
    };

    // Enable clock for PORTs, setup board clock source, config pin
    hardware_init();

    // Initialize the uart module with base address and config structure
    UART_DRV_Init(BOARD_DEBUG_UART_INSTANCE, &uartState, &uartConfig);

    // Inform to start non blocking example
    byteCountBuff = sizeof(buffStart);
    UART_DRV_SendData(BOARD_DEBUG_UART_INSTANCE, buffStart, byteCountBuff);

    // Wait until transmission is finished
    while (kStatus_UART_TxBusy == UART_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, NULL)){}

    // Inform user of what to do
    byteCountBuff = sizeof(bufferData1);
    UART_DRV_SendData(BOARD_DEBUG_UART_INSTANCE, bufferData1, byteCountBuff);

    // Wait until transmission is finished
    while (kStatus_UART_TxBusy == UART_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, NULL)){}


    while(true)
    {
        // Call received API
        UART_DRV_ReceiveData(BOARD_DEBUG_UART_INSTANCE, &rxChar, 1u);

        // Wait until we receive a character
        while (kStatus_UART_RxBusy == UART_DRV_GetReceiveStatus(BOARD_DEBUG_UART_INSTANCE, NULL)){}

        // Echo received character
        txChar = rxChar;
        UART_DRV_SendData(BOARD_DEBUG_UART_INSTANCE, &txChar, 1u);
    }

}

/*******************************************************************************
 * EOF
 ******************************************************************************/
