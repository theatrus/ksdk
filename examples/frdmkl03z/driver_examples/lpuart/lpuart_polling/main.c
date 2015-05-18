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
#include "fsl_lpuart_hal.h"

///////////////////////////////////////////////////////////////////////////////
//  Consts
///////////////////////////////////////////////////////////////////////////////

const uint8_t buffStart[]    = "\n\r++++++++++++++++ LPUART Send/Receive Polling Example +++++++++++++++++\n\r";
const uint8_t bufferData1[]  = "\n\rType characters from keyboard, the board will receive and then echo them to terminal screen\n\r";

///////////////////////////////////////////////////////////////////////////////
//  Code
///////////////////////////////////////////////////////////////////////////////

/*
 * This example will example the efficiency of the transmit/receive hal with
 * using polling method. Transfer data between board and PC. Board will transfer
 * and receive characters with PC through LPUART interface. Type characters from
 * keyboard, the board will receive and then echo them to terminal screen.
 * Look for intructions output to the terminal.
 */

/*!
 * @brief Check send/receive polling functionality
 *
 */
int main(void)
{
    uint8_t         txChar, rxChar  = 0;
    uint32_t        byteCountBuff   = 0;
    uint32_t        lpuartSourceClock;
    LPUART_Type*    baseAddr        = BOARD_DEBUG_UART_BASEADDR;

    // Enable clock for PORTs, setup board clock source
    hardware_init();
    // Ungate lpuart module clock
    CLOCK_SYS_EnableLpuartClock(BOARD_DEBUG_UART_INSTANCE);
    // Initialize the LPUART instance
    LPUART_HAL_Init(baseAddr);
    CLOCK_SYS_SetLpuartSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpuartSrcIrc48M);

    // LPUART clock source is either system or bus clock depending on instance
    lpuartSourceClock = CLOCK_SYS_GetLpuartFreq(BOARD_DEBUG_UART_INSTANCE);

    // Initialize the parameters of the LPUART config structure with desired data
    LPUART_HAL_SetBaudRate(baseAddr, lpuartSourceClock, BOARD_DEBUG_UART_BAUD);
    LPUART_HAL_SetBitCountPerChar(baseAddr, kLpuart8BitsPerChar);
    LPUART_HAL_SetParityMode(baseAddr, kLpuartParityDisabled);
    LPUART_HAL_SetStopBitCount(baseAddr, kLpuartOneStopBit);

    // Enable the LPUART transmitter and receiver
    LPUART_HAL_SetTransmitterCmd(baseAddr, true);
    LPUART_HAL_SetReceiverCmd(baseAddr, true);


    // Inform to start polling example
    byteCountBuff = sizeof(buffStart);
    LPUART_HAL_SendDataPolling(baseAddr, buffStart, byteCountBuff);

    // Inform user of what to do
    byteCountBuff = sizeof(bufferData1);
    LPUART_HAL_SendDataPolling(baseAddr, bufferData1, byteCountBuff);

    // Send/receive polling function
    while(true)
    {
        // Wait to receive input data
        if (kStatus_LPUART_Success == LPUART_HAL_ReceiveDataPolling(baseAddr, &rxChar, 1))
        {
            // Echo received character
            txChar = rxChar;
            LPUART_HAL_SendDataPolling(baseAddr, &txChar, 1);
        }
    }

}

/*******************************************************************************
 * EOF
 ******************************************************************************/

