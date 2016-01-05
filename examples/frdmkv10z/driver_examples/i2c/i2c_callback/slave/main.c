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
// Includes
///////////////////////////////////////////////////////////////////////////////

 // Standard C Included Files
#include <stdio.h>
// SDK Included Files
#include "board.h"
#include "fsl_i2c_slave_driver.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define DATA_LENGTH        64
#define RECV_CMD           0    // receive command
#define RECV_DATA          1    // receive data
#define SEND_DATA          2    // send data

typedef struct
{
    uint8_t *pTxBuff;   // pointer points to transmit buffer
    uint32_t txSize;    // Size of transmit buffer
    uint8_t *pRxBuff;   // pointer points to receive buffer
    uint32_t rxSize;    // Size of receive buffer
} call_param_t;

volatile uint8_t i2cState = RECV_CMD;
volatile bool isDataValid = false;

/*!
 * @brief slave event callback.
 *
 * This callback function is used by the I2C slave IRQ handler to process an event
 *
 * @param instance I2C instance
 * @param i2cEvent The event type
 */
static void i2c_slave_callback(uint8_t instance,i2c_slave_event_t i2cEvent,void *callParam)
{
    i2c_slave_state_t * slaveState = I2C_DRV_SlaveGetHandler(instance);
    call_param_t *userData = (call_param_t*)callParam;
    uint32_t i = 0;

    switch(i2cEvent)
    {
        // Transmit request
        case kI2CSlaveTxReq:
            // Update information for transmit process
            slaveState->txSize = userData->txSize;
            slaveState ->txBuff = userData->pTxBuff;
            slaveState ->isTxBusy = true;
        break;

        // Receive request
        case kI2CSlaveRxReq:
            // Update information for received process
            slaveState ->rxBuff = userData->pRxBuff;
            slaveState ->rxSize = userData->rxSize;
            slaveState ->isRxBusy = true;
        break;

        // Transmit buffer is empty
        case kI2CSlaveTxEmpty:
            i2cState = RECV_CMD;
            slaveState->isTxBusy = false;
        break;

        // Receive buffer is full
        case kI2CSlaveRxFull:
            if(i2cState == RECV_CMD)
            {
                slaveState->rxSize = userData->pRxBuff[0];
                userData->rxSize   = userData->pRxBuff[0];
                userData->txSize   = userData->pRxBuff[0];
                slaveState->rxBuff = userData->pRxBuff;
                i2cState = RECV_DATA;
                // Clear receive buffer
                for(i = 0; i < userData->rxSize; i++)
                {
                    slaveState->rxBuff[i] = 0;
                }
            }
            else
            {
                isDataValid = true;
                userData->rxSize = 1;
                i2cState = SEND_DATA;
            }
            slaveState->isRxBusy = false;
        break;

        default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
* @brief The i2c slave
* The function runs i2c slave with interrupt passive mode. Slave receive data from
* master and echo back to master
*/
int main(void)
{
    // Buffer store data to transfer
    uint8_t dataBuff[DATA_LENGTH] = {0};
    // State of slave
    i2c_slave_state_t slave;
    uint32_t i = 0;
    // Param for callback function
    call_param_t callParam =
    {
        .pTxBuff = dataBuff,
        .pRxBuff = dataBuff,
        .txSize = 1,
        .rxSize = 1,
    };
    // user configuration
    i2c_slave_user_config_t userConfig =
    {
        .address        = 0x7FU,
        .slaveCallback  = i2c_slave_callback,
        .callbackParam  = &callParam,
        .slaveListening = true,
#if FSL_FEATURE_I2C_HAS_START_STOP_DETECT
        .startStopDetect  = true,
#endif
#if FSL_FEATURE_I2C_HAS_STOP_DETECT
        .stopDetect       = true,
#endif
    };

    // Initialize hardware
    hardware_init();

    PRINTF("==================== I2C SLAVE CALLBACK ===================\r\n\r\n");
    PRINTF("Slave is running ...");

    // Initialize slave
    I2C_DRV_SlaveInit(BOARD_I2C_INSTANCE, &userConfig, &slave);

    // Loop transfer
    while(1)
    {
        if(isDataValid == true)
        {
            // Print receive data
            PRINTF("\r\nSlave received:\r\n");
            for (i = 0; i < callParam.txSize; i++)
            {
                // Print 16 numbers in a line.
                if ((i & 0x0F) == 0)
                {
                    PRINTF("\r\n    ");
                }
                PRINTF(" %02X", callParam.pRxBuff[i]);
            }
            isDataValid = false;
        }
    }
}
/*******************************************************************************
 * EOF
 ******************************************************************************/
