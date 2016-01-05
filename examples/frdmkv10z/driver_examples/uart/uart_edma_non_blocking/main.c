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
#include "fsl_edma_driver.h"
#include "fsl_uart_edma_driver.h"

///////////////////////////////////////////////////////////////////////////////
//  Consts
///////////////////////////////////////////////////////////////////////////////
const uint8_t buffStart[]   = "\r\n++++++++++++++++ UART-DMA Send/Receive Non Blocking Example +++++++++++++++++\r\n";
const uint8_t bufferData1[] = "\r\nType characters from keyboard, the board will receive and then echo them to terminal screen\r\n";

///////////////////////////////////////////////////////////////////////////////
//  Code
///////////////////////////////////////////////////////////////////////////////
/*
 * This example will example the efficiency of the transmit/receive drivers with
 * using DMA blocking and DMA non blocking method. Transfer data between board and PC.
 * Board will transfer and receive characters with PC through UART interface.
 * Type characters from keyboard, the board will receive and then echo them to terminal screen.
 * Look for intructions output to the terminal.
 */

/*!
 * @brief Check send/receive non blocking with DMA
 *
 */
int main(void)
{
    uint8_t rxChar  = 0, txChar = 0;
    uint32_t byteCountBuff = 0;

    // Store runtime state structure for the eDMA driver
    edma_state_t                state;
    // Store runtime state structure for UART driver with EDMA
    uart_edma_state_t           uartStateEdma;

    // Config the eDMA driver
    edma_user_config_t userConfig = {
        .chnArbitration     = kEDMAChnArbitrationRoundrobin,
#if (FSL_FEATURE_EDMA_CHANNEL_GROUP_COUNT > 0x1U)
        .groupArbitration   = kEDMAGroupArbitrationFixedPriority,
        .groupPriority      = kEDMAGroup0PriorityLowGroup1PriorityHigh,
#endif
        .notHaltOnError     = false
    };
    // Config the UART driver
    uart_edma_user_config_t uartConfig = {
        .bitCountPerChar = kUart8BitsPerChar,
        .parityMode      = kUartParityDisabled,
        .stopBitCount    = kUartOneStopBit,
        .baudRate        = BOARD_DEBUG_UART_BAUD
    };

    // Enable clock for PORTs, setup board clock source, config pin
    hardware_init();

    // Initialize EDMA module for UART
    EDMA_DRV_Init(&state, &userConfig);
    UART_DRV_EdmaInit(BOARD_DEBUG_UART_INSTANCE, &uartStateEdma, &uartConfig);

    // Inform to start dma-uart non blocking example
    byteCountBuff = sizeof(buffStart);
    UART_DRV_EdmaSendData(BOARD_DEBUG_UART_INSTANCE, buffStart, byteCountBuff);
    while (kStatus_UART_TxBusy == UART_DRV_EdmaGetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, NULL)){}

    // Inform user of what to do
    byteCountBuff = sizeof(bufferData1);
    UART_DRV_EdmaSendData(BOARD_DEBUG_UART_INSTANCE, bufferData1, byteCountBuff);
    while (kStatus_UART_TxBusy == UART_DRV_EdmaGetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, NULL)){}

    while(true)
    {
        // Call received API
        UART_DRV_EdmaReceiveData(BOARD_DEBUG_UART_INSTANCE, &rxChar, 1u);

        // Wait until we receive a character
        while (kStatus_UART_RxBusy == UART_DRV_EdmaGetReceiveStatus(BOARD_DEBUG_UART_INSTANCE, NULL)){}

        // Echo received character
        txChar = rxChar;
        UART_DRV_EdmaSendData(BOARD_DEBUG_UART_INSTANCE, &txChar, 1u);
    }

}

/*******************************************************************************
 * EOF
 ******************************************************************************/
