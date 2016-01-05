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
#include "fsl_os_abstraction.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_debug_console.h"

#define DATA_LENGTH             64

// Buffer store data to send to slave
uint8_t txBuff[DATA_LENGTH] = {0};
// Buffer store data to receive from slave
uint8_t rxBuff[DATA_LENGTH] = {0};

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief compare the data received with data sent.
 * @param source The pointer to send buffer
 * @param sink The pointer to received buffer
 * @param count The number of bytes to be compared
 * @return true or false
 */
bool i2c_compare(uint8_t *txBuff, uint8_t *rxBuff, uint32_t count)
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
* @brief The i2c master
* The function send array to slave, then receive a array from slave
* Then compare whether the two buffers are same
*/
int main(void)
{
    // Number byte data will be transfer
    uint32_t count = 0;
    uint32_t i = 0;
    // i2c master state
    i2c_master_state_t master;
    // i2c device configuration
    i2c_device_t device =
    {
      .address = 0x7FU,
      .baudRate_kbps = 400   // 400 Kbps
    };

    // Init hardware
    hardware_init();

    // Initialize OSA
    OSA_Init();

    PRINTF("\r\n==================== I2C MASTER BLOCKING ===================\r\n");
    PRINTF("\r\n1. Master sends a frame includes CMD(size of data) and data\
    \r\n2. Master receives data from slave.\
    \r\n3. Compare rxBuff and txBuff to see result.\r\n");
    PRINTF("\r\n============================================================\r\n\r\n");

    // Initialize i2c master
    I2C_DRV_MasterInit(BOARD_I2C_INSTANCE, &master);

    // Initialize data to send
    for(i = 0; i < DATA_LENGTH; i++)
    {
        txBuff[i] = i + 1;
    }

    // Start transfer with buffer size is 1 byte
    count = 1;

    PRINTF("Press any key to start transfer:\r\n\r\n");

     // Loop for transfer
    while(1)
    {
      // Wait user press any key
        GETCHAR();

        // Print out transmit buffer.
        PRINTF("Master sends %d bytes:\r\n", count);
        for (i = 0; i < count; i++)
        {
            // Print 16 numbers in a line.
            if ((i & 0x0F) == 0)
            {
                PRINTF("\r\n    ");
            }
            PRINTF(" %02X", txBuff[i]);
        }

        // Master sends 1 bytes CMD and data to slave
        I2C_DRV_MasterSendDataBlocking(BOARD_I2C_INSTANCE, &device,
                        (const uint8_t*)&count, 1, (const uint8_t*)txBuff, count, 1000);

        // Delay to wait slave received data
        OSA_TimeDelay(25);

        // Clear rxBuff
        for(i = 0; i < count; i++)
        {
            rxBuff[i] = 0;
        }

        // Master receives count byte data from slave
        I2C_DRV_MasterReceiveDataBlocking(BOARD_I2C_INSTANCE, &device,
                                                  NULL, 0, rxBuff, count, 1000);

        /* Compare to check result */
        if(i2c_compare((uint8_t*)txBuff, rxBuff, count) != true)
        {
            PRINTF("\r\nFailure when transfer with size of buffer is %d.\r\n", count);
            break;
        }

        PRINTF("\r\nMaster Sends/ Receives %2d bytes Successfully\r\n\r\n", count);
        if(++count > DATA_LENGTH)
        {
            count = 1;
        }
    }

    PRINTF("\r\n==================== I2C MASTER FINISH =================== \r\n");

    // Deinit i2c
    I2C_DRV_MasterDeinit(BOARD_I2C_INSTANCE);

    return 0;
}
/*******************************************************************************
 * EOF
 ******************************************************************************/
