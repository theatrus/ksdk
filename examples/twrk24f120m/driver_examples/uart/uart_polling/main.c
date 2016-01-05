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
#include "fsl_uart_hal.h"
#include "fsl_clock_manager.h"

///////////////////////////////////////////////////////////////////////////////
//  Consts
///////////////////////////////////////////////////////////////////////////////
const uint8_t buffStart[]    = "\r\n++++++++++++++++ UART Send/Receive Polling Example +++++++++++++++++\r\n";
const uint8_t bufferData1[]  = "\r\nType characters from keyboard, the board will receive and then echo them to terminal screen\r\n";
/*
 * This example will example the efficiency of the transmit/receive drivers with
 * using polling method. Transfer data between board and PC. Board will transfer
 * and receive characters with PC through UART interface. Type characters from
 * keyboard, the board will receive and then echo them to terminal screen.
 * Look for intructions output to the terminal.
 */
/*!
 * @brief Check send/receive polling functionality
 *
 */
int main(void)
{
    uint8_t  rxChar          = 0;
    uint32_t byteCountBuff   = 0;
    uint32_t uartSourceClock = 0;
    UART_Type * baseAddr     = BOARD_DEBUG_UART_BASEADDR;

    // Enable clock for PORTs, setup board clock source, config pin
    hardware_init();

    // Initialize the uart module with base address and config structure
    CLOCK_SYS_EnableUartClock(BOARD_DEBUG_UART_INSTANCE);

    // Get working uart clock
    uartSourceClock = CLOCK_SYS_GetUartFreq(BOARD_DEBUG_UART_INSTANCE);

    // Initialize UART baud rate, bit count, parity and stop bit
    UART_HAL_SetBaudRate(baseAddr, uartSourceClock, BOARD_DEBUG_UART_BAUD);
    UART_HAL_SetBitCountPerChar(baseAddr, kUart8BitsPerChar);
    UART_HAL_SetParityMode(baseAddr, kUartParityDisabled);
#if FSL_FEATURE_UART_HAS_STOP_BIT_CONFIG_SUPPORT
    UART_HAL_SetStopBitCount(baseAddr, kUartOneStopBit);
#endif

    // Enable the UART transmitter and receiver
    UART_HAL_EnableTransmitter(baseAddr);
    UART_HAL_EnableReceiver(baseAddr);

    // Inform to start polling example
    byteCountBuff = sizeof(buffStart);
    UART_HAL_SendDataPolling(baseAddr, buffStart, byteCountBuff);

    // Inform user of what to do
    byteCountBuff = sizeof(bufferData1);
    UART_HAL_SendDataPolling(baseAddr, bufferData1, byteCountBuff);

    while(true)
    {
        // Wait to receive input data
        if (kStatus_UART_Success == UART_HAL_ReceiveDataPolling(baseAddr, &rxChar, 1u))
        {
            // Send any character that received
            UART_HAL_SendDataPolling(baseAddr, &rxChar, 1u);
        }
    }

}

/*******************************************************************************
 * EOF
 ******************************************************************************/
 
