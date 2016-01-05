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
// Standard C Included Files
#include <stdio.h>
 // SDK Included Files
#include "board.h"
#include "fsl_spi_slave_driver.h"
#if FSL_FEATURE_SPI_HAS_DMA_SUPPORT
#include "fsl_spi_dma_slave_driver.h"
#endif
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SPI_SLAVE_INSTANCE          BOARD_SPI_INSTANCE             /*! User change define to choose SPI instance */
#define TRANSFER_SIZE               (64)
#define SLAVE_TRANSFER_TIMEOUT      (OSA_WAIT_FOREVER)             /*! Transfer timeout of slave - 5s */

/*******************************************************************************
 * Variables
 ******************************************************************************/
// Buffer for storing data received by the SPI slave driver.
uint8_t s_spiSinkBuffer[TRANSFER_SIZE] = {0};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief SPI slave DMA non-blocking.
 *
 * This function sends back received buffer from master through SPI interface.
 */
int main (void)
{
    uint32_t j;
    spi_status_t spiResult;
    dma_state_t dmaState;
    spi_dma_slave_state_t spiDmaSlaveState;
    spi_dma_slave_user_config_t userDmaConfig =
    {
#if FSL_FEATURE_SPI_16BIT_TRANSFERS
        .bitCount       = kSpi8BitMode,
#endif
        .polarity       = kSpiClockPolarity_ActiveHigh,
        .phase          = kSpiClockPhase_FirstEdge,
        .direction      = kSpiMsbFirst
    };

    // Init the DMA module
    DMA_DRV_Init(&dmaState);

    // init the hardware, this also sets up up the SPI pins for each specific SoC
    hardware_init();

    OSA_Init();

    PRINTF("\r\nSPI board to board dma non-blocking example");
    PRINTF("\r\nThis example run on instance %d", (uint32_t)SPI_SLAVE_INSTANCE);
    PRINTF("\r\nBe sure master's SPI%d and slave's SPI%d are connected",
                    (uint32_t)SPI_SLAVE_INSTANCE, (uint32_t)SPI_SLAVE_INSTANCE);

    // Initialize slave driver.
    if (SPI_DRV_DmaSlaveInit(SPI_SLAVE_INSTANCE, &spiDmaSlaveState, &userDmaConfig) != kStatus_SPI_Success)
    {
        PRINTF("\r\nError in slave DMA init \r\n");
    }

    while(1)
    {
        PRINTF("\r\nSlave example is running...\r\n");

        // Reset the sink buffer
        for (j = 0; j < TRANSFER_SIZE; j++)
        {
            s_spiSinkBuffer[j] = 0;
        }

        // Receive data from master
        spiResult = SPI_DRV_DmaSlaveTransfer(SPI_SLAVE_INSTANCE, NULL,
                                      s_spiSinkBuffer, TRANSFER_SIZE);
        while(kStatus_SPI_Success != SPI_DRV_DmaSlaveGetTransferStatus(SPI_SLAVE_INSTANCE, NULL));
        if (spiResult != kStatus_SPI_Success)
        {
            PRINTF("\r\nERROR: slave receives error ");
            return -1;
        }

        // Send back data to master
        spiResult = SPI_DRV_DmaSlaveTransfer(SPI_SLAVE_INSTANCE, s_spiSinkBuffer,
                                          NULL, TRANSFER_SIZE);
        while(kStatus_SPI_Success != SPI_DRV_DmaSlaveGetTransferStatus(SPI_SLAVE_INSTANCE, NULL));
        if (spiResult != kStatus_SPI_Success)
        {
            PRINTF("\r\nERROR: slave sends error ");
            return -1;
        }

        // Print out receive buffer
        PRINTF("\r\nSlave receive:");
        for (j = 0; j < TRANSFER_SIZE; j++)
        {
            // Print 16 numbers in a line.
            if ((j & 0x0F) == 0)
            {
                PRINTF("\r\n    ");
            }
            PRINTF(" %02X", s_spiSinkBuffer[j]);
        }
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/

