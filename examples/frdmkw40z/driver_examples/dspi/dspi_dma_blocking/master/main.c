/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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
#include "fsl_dma_driver.h"
#include "fsl_dspi_dma_master_driver.h"
#include "fsl_debug_console.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DSPI_MASTER_INSTANCE        (BOARD_DSPI_INSTANCE) /*! User change define to choose DSPI instance */
#define TRANSFER_SIZE               (32U)                 /*! Transfer size */
#define TRANSFER_BAUDRATE           (500000U)             /*! Transfer baudrate - 500k */
#define MASTER_TRANSFER_TIMEOUT     (5000U)               /*! Transfer timeout of master - 5s */
/*!******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t receiveBuffer[TRANSFER_SIZE] = {0U};
uint8_t sendBuffer[TRANSFER_SIZE] = {0U};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief DSPI master DMA - blocking.
 *
 * Thid function uses DSPI master to send an array to slave
 * and receive the array back from slave,
 * then compare whether the two buffers are the same.
 */

int main(void)
{
    uint32_t i;
    uint8_t errTransfer;
    uint32_t loopCount = 1U;
    uint32_t calculatedBaudRate;
    dspi_status_t dspiResult;

    dma_state_t dmaState;
    dspi_dma_master_state_t dmaMasterState;
    dspi_dma_device_t dmaDevice;
    dspi_dma_master_user_config_t dmaMasterUserConfig =
    {
        .isChipSelectContinuous     = false,
        .isSckContinuous            = false,
        .pcsPolarity                = kDspiPcs_ActiveLow,
        .whichCtar                  = kDspiCtar0,
        .whichPcs                   = kDspiPcs0
    };

    // Init hardware
    hardware_init();
    // Init OSA layer, used in DSPI_DRV_MasterTransferBlocking.
    OSA_Init();

    // Init the DMA module
    DMA_DRV_Init(&dmaState);

    // Print a note.
    PRINTF("\r\n DSPI board to board DMA-blocking example");
    PRINTF("\r\n This example run on instance %d ", (uint32_t)DSPI_MASTER_INSTANCE);
    PRINTF("\r\n Be sure DSPI%d-DSPI%d are connected \r\n",
                    (uint32_t)DSPI_MASTER_INSTANCE,(uint32_t)DSPI_MASTER_INSTANCE);
    PRINTF("\r\n Bus clock: %d", CLOCK_SYS_GetBusClockFreq());

    // Setup the configuration.
    dmaDevice.dataBusConfig.bitsPerFrame = 8U;
    dmaDevice.dataBusConfig.clkPhase     = kDspiClockPhase_FirstEdge;
    dmaDevice.dataBusConfig.clkPolarity  = kDspiClockPolarity_ActiveHigh;
    dmaDevice.dataBusConfig.direction    = kDspiMsbFirst;

    //Init the dspi module for DMA operation
    dspiResult = DSPI_DRV_DmaMasterInit(DSPI_MASTER_INSTANCE,
                                         &dmaMasterState,
                                         &dmaMasterUserConfig);
    if (dspiResult != kStatus_DSPI_Success)
    {
        PRINTF("\r\nERROR: Can not initialize master driver\r\n");
        return -1;
    }

    // Configure baudrate.
    dmaDevice.bitsPerSec = TRANSFER_BAUDRATE;
    dspiResult = DSPI_DRV_DmaMasterConfigureBus(DSPI_MASTER_INSTANCE,
                                                 &dmaDevice,
                                                 &calculatedBaudRate);
    if (dspiResult != kStatus_DSPI_Success)
    {
        PRINTF("\r\nERROR: failure in config bus \r\n");
        return -1;
    }
    else
    {
        PRINTF("\r\n Transfer at baudrate %lu\r\n", calculatedBaudRate);
    }

    while(1)
    {
        // Initialize the source buffer.
        for (i = 0U; i < TRANSFER_SIZE; i++)
        {
            sendBuffer[i] = i + loopCount;
        }

        // Print out transmit buffer.
        PRINTF("\r\n Master transmit:\r\n");
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
            // Print 16 numbers in a line.
            if ((i & 0x0F) == 0)
            {
                PRINTF("\r\n    ");
            }
            PRINTF(" %02X", sendBuffer[i]);
        }
        // Reset the sink buffer.
        for (i = 0U; i < TRANSFER_SIZE; i++)
        {
            receiveBuffer[i] = 0U;
        }

        // Send the data.
        dspiResult = DSPI_DRV_DmaMasterTransferBlocking(DSPI_MASTER_INSTANCE,
                                                        NULL,
                                                        sendBuffer,
                                                        NULL,
                                                        TRANSFER_SIZE,
                                                        MASTER_TRANSFER_TIMEOUT);
        if (dspiResult != kStatus_DSPI_Success)
        {
            PRINTF("\r\nERROR: send data error \r\n");
            return -1;
        }
        // Wait until slave is ready to send.
        OSA_TimeDelay(100U);

        // Receive the data.
        dspiResult = DSPI_DRV_DmaMasterTransferBlocking(DSPI_MASTER_INSTANCE,
                                                        NULL,
                                                        NULL,
                                                        receiveBuffer,
                                                        TRANSFER_SIZE,
                                                        MASTER_TRANSFER_TIMEOUT);
        if (dspiResult != kStatus_DSPI_Success)
        {
            PRINTF("\r\nERROR: receive data error \r\n");
            return -1;
        }

        // Print out receive buffer.
        PRINTF("\r\n Master receive:\r\n");
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
            // Print 16 numbers in a line.
            if ((i & 0x0F) == 0)
            {
                PRINTF("\r\n    ");
            }
            PRINTF(" %02X", receiveBuffer[i]);
        }

        errTransfer = false;
        // Check receiveBuffer.
        for (i = 0; i < TRANSFER_SIZE; ++i)
        {
            if (receiveBuffer[i] != sendBuffer[i])
            {
                errTransfer = true;
                break;
            }
        }

        if (errTransfer)
        {
            // Master received incorrect.
            PRINTF("\r\n ERROR: master received incorrect at No.%d\r\n", i);
        }
        else
        {
            PRINTF("\r\n DSPI Master Sends/ Recevies Successfully");
        }
        // Wait for press any key.
        PRINTF("\r\n Press any key to run again\r\n");
        GETCHAR();
        // Increase loop count to change transmit buffer.
        loopCount++;
    }
}
/*******************************************************************************
 * EOF
 ******************************************************************************/
