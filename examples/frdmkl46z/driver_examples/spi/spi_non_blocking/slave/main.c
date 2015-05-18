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

#include <string.h>
#include "fsl_spi_slave_driver.h"

#if FSL_FEATURE_SPI_HAS_DMA_SUPPORT
#include "fsl_spi_dma_slave_driver.h"
#endif

#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include "board.h"
#include <stdio.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if (defined FRDM_KL43Z) || (defined MRB_KW01)
#define SPI_SLAVE_INSTANCE          (1)                 /*! User change define to choose SPI instance */
#else
#define SPI_SLAVE_INSTANCE          (0)                 /*! User change define to choose SPI instance */
#endif
#define TRANSFER_SIZE               (64)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*!
 * @brief Buffer for storing data received by the SPI slave driver.
 */
uint8_t s_spiSinkBuffer[TRANSFER_SIZE] = {0};

/*!
 * @brief Buffer that supplies data to be transmitted with the SPI slave driver.
 */
uint8_t s_spiSourceBuffer[TRANSFER_SIZE] = {0};

/*******************************************************************************
 * Code
 ******************************************************************************/

int main (void)
{
    uint32_t j;
    spi_status_t spiResult;
    spi_slave_state_t spiSlaveState;

    // Create slaveUserConfig
    spi_slave_user_config_t slaveUserConfig =
    {
    #if FSL_FEATURE_SPI_16BIT_TRANSFERS
        .bitCount       = kSpi8BitMode,
    #endif
        .direction      = kSpiMsbFirst,
        .polarity       = kSpiClockPolarity_ActiveHigh,
        .phase          = kSpiClockPhase_FirstEdge
    };

    // init the hardware, this also sets up up the SPI pins for each specific SoC
    hardware_init();

    OSA_Init();

    PRINTF("\r\nSPI board to board non-blocking example");
    PRINTF("\r\nThis example run on instance %d", (uint32_t)SPI_SLAVE_INSTANCE);
    PRINTF("\r\nBe sure master's SPI%d and slave's SPI%d are connected",
                    (uint32_t)SPI_SLAVE_INSTANCE, (uint32_t)SPI_SLAVE_INSTANCE);

    // USER CONFIGURABLE OPTION FOR SPI INSTANCE (if applicable)

    SPI_DRV_SlaveInit(SPI_SLAVE_INSTANCE, &spiSlaveState, &slaveUserConfig);
    while(1)
    {
        PRINTF("\r\nSlave example is running...");

        // Reset the sink buffer
        for (j = 0; j < TRANSFER_SIZE; j++)
        {
            s_spiSinkBuffer[j] = 0;
        }

        // Receive data from master
        PRINTF("\r\nSPI is waiting to receive data \r\n");
        spiResult = SPI_DRV_SlaveTransfer(SPI_SLAVE_INSTANCE,
                                                NULL,
                                                s_spiSinkBuffer,
                                                TRANSFER_SIZE);
        while (SPI_DRV_SlaveGetTransferStatus(SPI_SLAVE_INSTANCE, NULL) == kStatus_SPI_Busy)
        {}
        if (spiResult != kStatus_SPI_Success)
        {
            PRINTF("\r\nERROR: slave receives error ");
            return -1;
        }

        for (j = 0; j < TRANSFER_SIZE; j++)
        {
            s_spiSourceBuffer[j] = s_spiSinkBuffer[j];
        }

        spiResult = SPI_DRV_SlaveTransfer(SPI_SLAVE_INSTANCE,
                                                s_spiSourceBuffer,
                                                NULL,
                                                TRANSFER_SIZE);
        while (SPI_DRV_SlaveGetTransferStatus(SPI_SLAVE_INSTANCE, NULL) == kStatus_SPI_Busy)
        {}
        if (spiResult != kStatus_SPI_Success)
        {
            PRINTF("\r\nERROR: slave sends error ");
            return -1;
        }

        // Print out receive buffer
        PRINTF("\r\nSlave receive:");
        for (j = 0; j < TRANSFER_SIZE; j++)
        {
            if (j%16 == 0)
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

