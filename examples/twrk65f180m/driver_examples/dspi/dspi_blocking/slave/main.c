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

// Standard C Included Files
#include <stdio.h>
 // SDK Included Files
#include "board.h"
#include "fsl_clock_manager.h"
#include "fsl_os_abstraction.h"
#include "fsl_dspi_slave_driver.h"
#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DSPI_SLAVE_INSTANCE         BOARD_DSPI_INSTANCE /*! User change define to choose DSPI instance */
#define TRANSFER_SIZE               (32)                /*! Transfer size */
#define TRANSFER_BAUDRATE           (500000U)           /*! Transfer baudrate - 500k */
#define SLAVE_TRANSFER_TIMEOUT      (OSA_WAIT_FOREVER)  /*! Transfer timeout of slave */
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t receiveBuffer[TRANSFER_SIZE] = {0};
/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief DSPI slave blocking.
 *
 * This function sends back received buffer from master through DSPI interface.
 */
int main(void)
{
    uint32_t i;

    // Set up and init the slave.
    dspi_status_t dspiResult;
    dspi_slave_state_t slaveState;
    dspi_slave_user_config_t slaveConfig;

    // Init hardware
    hardware_init();
    // Init OSA layer, used in DSPI_DRV_MasterTransferBlocking.
    OSA_Init();

    // Print a note.
    PRINTF("\r\n DSPI board to board blocking example");
    PRINTF("\r\n This example run on instance %d ", (uint32_t)DSPI_SLAVE_INSTANCE);
    PRINTF("\r\n Be sure DSPI%d-DSPI%d are connected \r\n",
                    (uint32_t)DSPI_SLAVE_INSTANCE,(uint32_t)DSPI_SLAVE_INSTANCE);

    // Setup the configuration
    slaveConfig.dataConfig.bitsPerFrame = 8;
    slaveConfig.dataConfig.clkPhase     = kDspiClockPhase_FirstEdge;
    slaveConfig.dataConfig.clkPolarity  = kDspiClockPolarity_ActiveHigh;
    slaveConfig.dummyPattern            = 0;
    // Initialize slave driver.
    dspiResult = DSPI_DRV_SlaveInit(DSPI_SLAVE_INSTANCE,
                                     &slaveState,
                                     &slaveConfig);
    if (dspiResult != kStatus_DSPI_Success)
    {
        PRINTF("\r\n ERROR: Can not initialize slave driver \r\n");
        return -1;
    }

    while(1)
    {
        PRINTF("\r\n Slave example is running...\r\n");

        // Reset the sink buffer.
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
            receiveBuffer[i] = 0;
        }

        // Receive the data.
        dspiResult = DSPI_DRV_SlaveTransferBlocking(DSPI_SLAVE_INSTANCE,
                                                    NULL,
                                                    receiveBuffer,
                                                    TRANSFER_SIZE,
                                                    SLAVE_TRANSFER_TIMEOUT);
        if (dspiResult != kStatus_DSPI_Success)
        {
            PRINTF("\r\nERROR: slave receives error \r\n");
            return -1;
        }

        // Transfer the data back to master.
        dspiResult = DSPI_DRV_SlaveTransferBlocking(DSPI_SLAVE_INSTANCE,
                                                    receiveBuffer,
                                                    NULL,
                                                    TRANSFER_SIZE,
                                                    SLAVE_TRANSFER_TIMEOUT);
        if (dspiResult != kStatus_DSPI_Success)
        {
            PRINTF("\r\nERROR: slave sends error \r\n");
            return -1;
        }

        // Print out receive buffer.
        PRINTF("\r\nSlave receive:\r\n    ");
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
            // Print 16 numbers in a line.
            if ((i & 0x0F) == 0)
            {
                PRINTF("\r\n    ");
            }
            PRINTF(" %02X", receiveBuffer[i]);
        }
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
