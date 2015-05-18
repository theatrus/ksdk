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
#include <string.h>
// SDK Included Files
#include "board.h"
#include "fsl_i2c_hal.h"
#include "fsl_i2c_slave_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define DATA_LENGTH     64

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
* @brief The i2c slave
* The function runs i2c slave with polling mode (HAL layer). Slave receive data
* from master and echo back to master
*/
int main(void)
{
    // Number byte data will be transfer
    uint32_t count = 0;
    uint32_t i = 0;
    // Buffer store data to transfer
    uint8_t dataBuff[DATA_LENGTH] = {0};
    // slave address
    uint16_t address = 0x7FU;
    // i2c slave base address
    I2C_Type * baseAddr = g_i2cBase[BOARD_I2C_COMM_INSTANCE];

    // Initialize hardware
    hardware_init();

    PRINTF("\r\n================== I2C SLAVE POLLING =================\r\n\n");
    PRINTF("Slave is running ...");

    /* Enable clock for I2C.*/
    CLOCK_SYS_EnableI2cClock(BOARD_I2C_COMM_INSTANCE);

    /* Init instance to known state. */
    I2C_HAL_Init(baseAddr);

    /* Set slave address.*/
    I2C_HAL_SetAddress7bit(baseAddr, address);

    /* Enable the peripheral operation.*/
    I2C_HAL_Enable(baseAddr);

    // Loop transfer
    while(1)
    {
        // count is length of string
        I2C_HAL_SlaveReceiveDataPolling(baseAddr, (uint8_t*)&count, 1);

        // Clear receive buffer
        for(i = 0; i < count; i++)
        {
            dataBuff[i] = 0;
        }

        // Slave receive buffer from master
        I2C_HAL_SlaveReceiveDataPolling(baseAddr, dataBuff, count);

        // Print receive data
        PRINTF("\r\nSlave received:\r\n");
        for (i = 0; i < count; i++)
        {
            // Print 16 numbers in a line.
            if ((i & 0x0F) == 0)
            {
                PRINTF("\r\n    ");
            }
            PRINTF(" %02X", dataBuff[i]);
        }

        // Slave send buffer received from master
        I2C_HAL_SlaveSendDataPolling(baseAddr, dataBuff, count);
    }
}
/*******************************************************************************
 * EOF
 ******************************************************************************/
