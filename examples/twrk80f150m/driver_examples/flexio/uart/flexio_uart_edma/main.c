/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
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
#include "fsl_flexio_uart_edma_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_lpuart_driver.h"
#include "fsl_debug_console.h"
///////////////////////////////////////////////////////////////////////////////
//  Variables
///////////////////////////////////////////////////////////////////////////////
static uint32_t s_flexioInstance;
flexio_uart_edmastate_t uartState;
#define DATA_LENGTH         65
const uint8_t txBuff[DATA_LENGTH] = "This data to transfer between FlexIO simulated UART and LPUART0";
uint8_t rxBuff[DATA_LENGTH] = {0};
#define COMM_UART_INSTANCE    0
///////////////////////////////////////////////////////////////////////////////
//  Codes
///////////////////////////////////////////////////////////////////////////////
/*
 * This example will show transmit/receive FlexIO simulated UART's driver,
 * the efficiency of the transmit/receive drivers with using blocking method.
 * Transfer data between FlexIO simulated UART and LPUART1.Compare the rxBuff
 * and txBuff to see whether the result is right.
 */
/*!
 * @brief compare the data received with data sent.
 * @param source The pointer to send buffer
 * @param sink The pointer to received buffer
 * @param count The number of bytes to be compared
 * @return true or false
 */
bool flexio_uart_compare(uint8_t *txBuff, uint8_t *rxBuff, uint32_t count)
{
    uint32_t i;
    /* Comapre source and sink data*/
    for (i = 0; i < count ; i++)
    {
        if (txBuff[i] != rxBuff[i])
        {
            return false;
        }
    }
    return true;
}
/*!
 * @brief reset a buffer.
 * @param source The pointer to the buffer
 * @param size of the buffer
 */
void flexio_uart_reset_buffer(uint8_t *txBuff, uint32_t count)
{
    uint32_t i;
    /* Comapre source and sink data*/
    for (i = 0; i < count ; i++)
    {
        txBuff[i] = 0;
    }
}
/*!
 * @brief Check send/receive blocking functionality
 *
 */
int main(void)
{
    lpuart_state_t lpuartStatePtr;
    edma_state_t         edmaState;
    edma_user_config_t   edmaUserConfig;
    s_flexioInstance = 0;
    /* Fill in FlexIO config data */
    flexio_user_config_t userConfig =
    {
        .useInt = true,
        .onDozeEnable = false,
        .onDebugEnable = true,
        .fastAccessEnable = false
    };

    // Enable clock for PORTs, setup board clock source, config pin
    hardware_init();

    // Call OSA_Init to setup LP Timer for timeout
    OSA_Init();

    FLEXIO_DRV_Init(s_flexioInstance,&userConfig);
    FLEXIO_DRV_Start(s_flexioInstance);

    /* Fill in FlexIO UART config data */
    flexio_uartedma_userconfig_t uartConfig;
    uartConfig.bitCounter = kFlexIOUart8BitsPerChar;
    uartConfig.baudRate = 115200;
    uartConfig.uartMode = flexioUART_TxRx;
    uartConfig.txConfig.pinIdx = 5;
    uartConfig.txConfig.shifterIdx = 0;
    uartConfig.txConfig.timerIdx = 0;
    uartConfig.rxConfig.pinIdx = 4;
    uartConfig.rxConfig.shifterIdx = 1;
    uartConfig.rxConfig.timerIdx = 1;

//    // Fill in lpuart config data
    lpuart_user_config_t lpuartConfig = {
        .clockSource     = BOARD_LPUART_CLOCK_SOURCE,
        .bitCountPerChar = kLpuart8BitsPerChar,
        .parityMode      = kLpuartParityDisabled,
        .stopBitCount    = kLpuartOneStopBit,
        .baudRate        = 115200
    };
//
//    /* init the FlexIO simulated UART module with base address and config structure*/
//    // Init eDMA modules.
    edmaUserConfig.chnArbitration = kEDMAChnArbitrationRoundrobin;
    edmaUserConfig.notHaltOnError = false;
    EDMA_DRV_Init(&edmaState, &edmaUserConfig);
    FLEXIO_UART_DRV_EdmaInit(s_flexioInstance, &uartState, &uartConfig);
    // Initialize the lpuart module with instance number and config structure
    LPUART_DRV_Init(COMM_UART_INSTANCE, &lpuartStatePtr, &lpuartConfig);
    // Inform to start blocking example
    PRINTF("\r\n++++++++++++++++ FLEXIO UART Send/Receive EDMA Example Start +++++++++++++++++\r\n");
    PRINTF("\r\n1. FlexIO simulated UART send a buffer using EDMA\
            \r\n2. LPUART0 receives data from FlexIO simulated UART.\
            \r\n3. Compare rxBuff and txBuff to see result.\
            \r\n4. LPUART0 send a buffer\
            \r\n5. FlexIO simulated UART receives data from LPUART0 using EDMA.\
            \r\n6. Compare rxBuff and txBuff to see result.\r\n");
    PRINTF("\r\n============================================================\r\n");
    PRINTF("\r\nPress any key to start transfer:\r\n\r\n");

    while(true)
    {
        GETCHAR();
        // FlexIO UART sends data to LPUART0 using EDMA mdoe
        FLEXIO_UART_DRV_EdmaSendData(&uartState, txBuff, DATA_LENGTH);
        LPUART_DRV_ReceiveDataBlocking(COMM_UART_INSTANCE, rxBuff, DATA_LENGTH, 1000U);
        if(flexio_uart_compare((uint8_t*)txBuff, rxBuff, DATA_LENGTH) != true)
        {
            PRINTF("\r\nFailure transfer from FlexIO simulated UART to LPUART0 using DMA");
            break;
        }
        PRINTF("Transfer from FlexIO simualted UART to LPUART0 using DMA successfully\r\n");
        flexio_uart_reset_buffer(rxBuff,DATA_LENGTH);
        // FlexIO UART receives data from LPUART0 using EDMA mdoe
        FLEXIO_UART_DRV_EdmaReceiveData(&uartState, rxBuff, DATA_LENGTH);
        LPUART_DRV_SendDataBlocking(COMM_UART_INSTANCE, txBuff, DATA_LENGTH,1000U);
        if(flexio_uart_compare((uint8_t*)txBuff, rxBuff, DATA_LENGTH) != true)
        {
            PRINTF("\r\nFailure FlexIO simulated UART receive from FlexIO LPUART1");
            break;
        }
        PRINTF("FlexIO simulated UART receive from FlexIO LPUART0 using DMA successfully\r\n");
    }

    return 0;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
