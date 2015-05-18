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
#define SLAVE_TRANSFER_TIMEOUT      (OSA_WAIT_FOREVER)             /*! Transfer timeout of slave - 5s */

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

    dma_state_t dmaState;

    // Init the DMA module
    DMA_DRV_Init(&dmaState);

    // init the hardware, this also sets up up the SPI pins for each specific SoC
    hardware_init();

    OSA_Init();

    PRINTF("\r\n SPI board to board dma non-blocking example");
    PRINTF("\r\nThis example run on instance %d", (uint32_t)SPI_SLAVE_INSTANCE);
    PRINTF("\r\nBe sure master's SPI%d and slave's SPI%d are connected",
                    (uint32_t)SPI_SLAVE_INSTANCE, (uint32_t)SPI_SLAVE_INSTANCE);

    // USER CONFIGURABLE OPTION FOR SPI INSTANCE (if applicable)

    if (SPI_DRV_DmaSlaveInit(SPI_SLAVE_INSTANCE, &spiDmaSlaveState, &userDmaConfig) != kStatus_SPI_Success)
    {
        PRINTF("\r\nError in slave DMA init \r\n");
    }

    while(1)
    {
        PRINTF("\r\nSlave example is running...");
        spiResult = SPI_DRV_DmaSlaveTransfer(SPI_SLAVE_INSTANCE, NULL,
                                      s_spiSinkBuffer, TRANSFER_SIZE);
        while(kStatus_SPI_Success != SPI_DRV_DmaSlaveGetTransferStatus(SPI_SLAVE_INSTANCE, NULL));
        if (spiResult != kStatus_SPI_Success)
        {
            PRINTF("\r\nERROR: slave receives error ");
            return -1;
        }
        for (j = 0; j < TRANSFER_SIZE; j++)
        {
            s_spiSourceBuffer[j] = s_spiSinkBuffer[j];
        }

        spiResult = SPI_DRV_DmaSlaveTransfer(SPI_SLAVE_INSTANCE, s_spiSourceBuffer,
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

