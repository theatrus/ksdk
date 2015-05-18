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

#include "board.h"
#include "fsl_clock_manager.h"
#include "fsl_lpsci_driver.h"
#include "fsl_lpsci_dma_driver.h"
#include "fsl_dma_driver.h"

///////////////////////////////////////////////////////////////////////////////
//  Consts
///////////////////////////////////////////////////////////////////////////////

const uint8_t buffStart[] = "\n\r++++++++++++++ LPSCI-DMA Non Blocking Example ++++++++++++++++++\n\r";
const uint8_t bufferData1[] = "\n\rType characters from keyboard, the board will receive and then echo them to terminal screen\n\r";


///////////////////////////////////////////////////////////////////////////////
//  Code
///////////////////////////////////////////////////////////////////////////////
/*
 * This example will show transmit/receive LPSCI's driver, the efficiency of the
 * transmit/receive drivers with using non blocking DMA. Transfer data between
 * board and PC. Board will transfer and receive characters with PC through
 * LPSCI interface. Type characters from keyboard, the board will receive and
 * then echo them to terminal screen. Look for intructions output to the terminal.
 */

/*!
 * @brief Check send/receive non blocking with DMA
 *
 */
int main(void)
{
    uint8_t rxChar = 0, txChar = 0;
    uint32_t byteCountBuff = 0;

    dma_state_t state;
    lpsci_dma_state_t lpsciStateDma;

    lpsci_dma_user_config_t lpsciConfig = {
#if defined(KL02Z4_SERIES)
        .clockSource     = kClockLpsciSrcFll,
#else
        .clockSource     = kClockLpsciSrcPllFllSel,
#endif
        .bitCountPerChar = kLpsci8BitsPerChar,
        .parityMode      = kLpsciParityDisabled,
        .stopBitCount    = kLpsciOneStopBit,
        .baudRate        = BOARD_DEBUG_UART_BAUD
    };

    // Enable clock for PORTs, setup board clock source, config pin
    hardware_init();

    // Initialize DMA module for LPSCI
    DMA_DRV_Init(&state);
    LPSCI_DRV_DmaInit(BOARD_DEBUG_UART_INSTANCE, &lpsciStateDma, &lpsciConfig);

    // Inform to start dma-uart blocking example
    byteCountBuff = sizeof(buffStart);
    LPSCI_DRV_DmaSendData(BOARD_DEBUG_UART_INSTANCE, buffStart, byteCountBuff);
    while (kStatus_LPSCI_TxBusy == LPSCI_DRV_DmaGetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, NULL));

    // Inform user of what to do
    byteCountBuff = sizeof(bufferData1);
    LPSCI_DRV_DmaSendData(BOARD_DEBUG_UART_INSTANCE, bufferData1, byteCountBuff);
    while (kStatus_LPSCI_TxBusy == LPSCI_DRV_DmaGetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, NULL));

    while(true)
    {
        // Wait to receive input data
        LPSCI_DRV_DmaReceiveData(BOARD_DEBUG_UART_INSTANCE, &rxChar,1u);

        while (kStatus_LPSCI_RxBusy == LPSCI_DRV_DmaGetReceiveStatus(BOARD_DEBUG_UART_INSTANCE, NULL)){}

        // Echo received character
        txChar = rxChar;
        LPSCI_DRV_DmaSendData(BOARD_DEBUG_UART_INSTANCE, &txChar, 1u);
    }

}

/*******************************************************************************
 * EOF
 ******************************************************************************/

