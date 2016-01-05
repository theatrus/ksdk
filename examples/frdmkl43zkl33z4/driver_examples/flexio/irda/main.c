/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 * of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
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

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_clock_manager.h"
#include "fsl_os_abstraction.h"
#include "fsl_flexio_uart_driver.h"
#include "fsl_flexio_uart_hal.h"
#include "fsl_flexio_irda.h"
#include "fsl_cmp_driver.h"
#include "main.h"

flexio_uart_state_t uartState;

int main(void)
{
    uint32_t i;
    uint32_t instance = 0;
    flexio_uart_userconfig_t uartConfig;
    flexio_irda_config_t txConfig, rxConfig;
    uint8_t txBuff[] = "Hello World";
    uint32_t txSize = sizeof(txBuff);
    uint8_t rxBuff[12] = {0};
    uint32_t freq;
    flexio_user_config_t userConfig = 
    {
        .useInt = true,
        .onDozeEnable = false,
        .onDebugEnable = true,
        .fastAccessEnable = false
    };
    hardware_init();
    OSA_Init();

    CLOCK_SYS_EnableFlexioClock(FLEXIO_INSTANCE);
    freq = CLOCK_SYS_GetFlexioFreq(FLEXIO_INSTANCE);
    
    FLEXIO_DRV_Init(instance,&userConfig);
    
    /* Fill in uart config data */
    uartConfig.bitCounter = kFlexIOUart8BitsPerChar;
    uartConfig.baudRate = FLEXIO_UART_BAUDRATE;
    uartConfig.uartMode = flexioUART_TxRx;
    uartConfig.txConfig.pinIdx = FLEXIO_UART_TX_PIN;
    uartConfig.txConfig.shifterIdx = FLEXIO_UART_TX_SHIFTER;
    uartConfig.txConfig.timerIdx = FLEXIO_UART_TX_TIMER;
    uartConfig.rxConfig.pinIdx = FLEXIO_UART_RX_PIN;
    uartConfig.rxConfig.shifterIdx = FLEXIO_UART_RX_SHIFTER;
    uartConfig.rxConfig.timerIdx = FLEXIO_UART_RX_TIMER;
    
    uartState.rxBuff= rxBuff;
    
    txConfig.baudrate = uartConfig.baudRate;
    txConfig.flexioFrequency = freq;
    rxConfig = txConfig;
    rxConfig.timerIdx = FLEXIO_UART_IRDA_RX_TIMER;
    rxConfig.timerPinIdx = FLEXIO_UART_RX_PIN;
#if LOOPBACK_TEST
    rxConfig.trigPinIdx = FLEXIO_UART_IRDA_TX_PIN;
#else
    rxConfig.trigPinIdx = FLEXIO_UART_IRDA_RX_PIN;
#endif
    txConfig.timerIdx = FLEXIO_UART_IRDA_TX_TIMER;
    txConfig.timerPinIdx = FLEXIO_UART_IRDA_TX_PIN;
    txConfig.trigPinIdx = FLEXIO_UART_TX_PIN;
    
    
    /* init the uart module with base address and config structure*/
    FLEXIO_UART_DRV_Init(instance, &uartState, &uartConfig);
    
    PRINTF("Start flexio uart/irda demo\r\n");
    
 

#if IRDA_RX_CMP0_TRIG
    // Configuration for cmp
    cmp_state_t cmpState;
    cmp_comparator_config_t cmpUserConfig;
    cmp_sample_filter_config_t cmpSampleFilterConfig;
    cmp_dac_config_t cmpDacConfig;
    
    // Disable rising interrupt
    // Disable falling interrupt
    // Init the CMP comparator.
    CMP_DRV_StructInitUserConfigDefault(&cmpUserConfig, (cmp_chn_mux_mode_t)BOARD_CMP_CHANNEL, kCmpInputChnDac);
    cmpUserConfig.risingIntEnable = false;
    cmpUserConfig.fallingIntEnable = false;
    CMP_DRV_Init(CMP_INSTANCE, &cmpState, &cmpUserConfig);

    // Configure the internal DAC when in used.
    cmpDacConfig.dacEnable = true;
    cmpDacConfig.dacValue = IRDA_RX_CMP0_DAC_VALUE; // 0U - 63U
    cmpDacConfig.refVoltSrcMode = kCmpDacRefVoltSrcOf2;
    CMP_DRV_ConfigDacChn(CMP_INSTANCE, &cmpDacConfig);

    // Configure the Sample/Filter Mode.
    cmpSampleFilterConfig.workMode = kCmpContinuousMode;
    CMP_DRV_ConfigSampleFilter(CMP_INSTANCE, &cmpSampleFilterConfig);

    // Start the CMP function.
    CMP_DRV_Start(CMP_INSTANCE);
    
#endif
    
    //configure FlexIO timers to decode IRDA signals
    FLEXIO_IRDA_Init(FLEXIO, &rxConfig, &txConfig);

    FLEXIO_DRV_Start(instance);
    PRINTF("\r\nNon-blocking Send/Receive Demo");
    //non blocking test
    FLEXIO_UART_DRV_ReceiveData(&uartState, rxBuff, txSize);
    FLEXIO_UART_DRV_SendData(&uartState, txBuff, txSize);
    while(1)
    {
        if(FLEXIO_UART_DRV_GetReceiveStatus(&uartState,NULL) == kStatus_FlexIO_UART_Success)
        {   
            PRINTF("\r\nString data is : %s",rxBuff);
            PRINTF("\r\nBinary data is : 0x");
            for(i=0;i<sizeof(rxBuff);i++)
            {
                PRINTF("%x",rxBuff[i]);
            }
            break;
        }
    }
    PRINTF("\r\n\r\nBlocking Send Demo");
    //blocking send test
    //only send "Hello"
    txSize = 5;
    memset(rxBuff, 0, sizeof(rxBuff));
    FLEXIO_UART_DRV_ReceiveData(&uartState, rxBuff, txSize);
    FLEXIO_UART_DRV_SendDataBlocking(&uartState, txBuff, txSize, 1000U);
    while(1)
    {
        if(FLEXIO_UART_DRV_GetReceiveStatus(&uartState,NULL) == kStatus_FlexIO_UART_Success)
        {   
            PRINTF("\r\nString data is : %s",rxBuff);
            PRINTF("\r\nBinary data is : 0x");
            for(i=0;i<txSize;i++)
            {
                PRINTF("%x",rxBuff[i]);
            }
            break;
        }
    }

    PRINTF("\r\n\r\nBlocking Receive Demo");
    //blocking send test
    //only send "He"
    txSize = 2;
    memset(rxBuff, 0, sizeof(rxBuff));
    FLEXIO_UART_DRV_SendData(&uartState, txBuff, txSize);
    FLEXIO_UART_DRV_ReceiveDataBlocking(&uartState, rxBuff, txSize, 1000U);
    while(1)
    {
        if(FLEXIO_UART_DRV_GetReceiveStatus(&uartState,NULL) == kStatus_FlexIO_UART_Success)
        {   
            PRINTF("\r\nString data is : %s",rxBuff);
            PRINTF("\r\nBinary data is : 0x");
            for(i=0;i<txSize;i++)
            {
                PRINTF("%x",rxBuff[i]);
            }
            break;
        }
    }
    
    PRINTF("\r\n\r\nLast demo: Input one char each time from terminal tools\r\n");
    PRINTF("It will be transmitted via FlexIO IRDA driver, and echoed on terminal tool\r\n");
    while(1)
    {
        // Main routine that simply echoes received characters forever
        // First, get character
        rxBuff[0] = 0;
        txBuff[0] = GETCHAR();
        // Second send the character via IRDA
        FLEXIO_UART_DRV_SendData(&uartState, txBuff, 1);
        FLEXIO_UART_DRV_ReceiveDataBlocking(&uartState, rxBuff, 1, 1000U);
 
        // Now echo the received character
        PRINTF("%c",rxBuff[0]);
    }
}
