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
#include "fsl_spi_master_driver.h"

#if FSL_FEATURE_SPI_HAS_DMA_SUPPORT
#include "fsl_spi_dma_master_driver.h"
#endif

#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include "board.h"
#include <stdio.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if (defined FRDM_KL43Z) || (defined MRB_KW01)
#define SPI_MASTER_INSTANCE          (1)                 /*! User change define to choose SPI instance */
#else
#define SPI_MASTER_INSTANCE          (0)                 /*! User change define to choose SPI instance */
#endif
#define TRANSFER_SIZE               (64)
#define TRANSFER_BAUDRATE           (500000U)           /*! Transfer baudrate - 500k */
#define MASTER_TRANSFER_TIMEOUT     (5000U)             /*! Transfer timeout of master - 5s */
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

/* Setup the board as a spi master */
int main (void)
{
    uint8_t loopCount = 0;
    uint32_t j;
    uint32_t failCount = 0;
    uint32_t calculatedBaudRate;
    spi_master_state_t spiMasterState;
    spi_master_user_config_t userConfig =
    {
    #if FSL_FEATURE_SPI_16BIT_TRANSFERS
        .bitCount       = kSpi8BitMode,
    #endif
        .polarity       = kSpiClockPolarity_ActiveHigh,
        .phase          = kSpiClockPhase_FirstEdge,
        .direction      = kSpiMsbFirst,
        .bitsPerSec     = TRANSFER_BAUDRATE
    };
    // Init hardware
    hardware_init();
    // Init OSA layer, used in DSPI_DRV_MasterTransferBlocking.
    OSA_Init();

    PRINTF("\r\nSPI loopback example");
    PRINTF("\r\nThis example run on instance %d", (uint32_t)SPI_MASTER_INSTANCE);
    PRINTF("\r\nBe sure MISO-to-MOSI are connected\r\n");

    // USER CONFIGURABLE OPTION FOR SPI INSTANCE
    PRINTF("\r\nBaud rate in Hz is: %d\n\r", TRANSFER_BAUDRATE);

    // Init and setup baudrate for the master
    SPI_DRV_MasterInit(SPI_MASTER_INSTANCE, &spiMasterState);
    SPI_DRV_MasterConfigureBus(SPI_MASTER_INSTANCE,
                                &userConfig,
                                &calculatedBaudRate);

    // Check if the configuration is correct
    if (calculatedBaudRate > userConfig.bitsPerSec)
    {
        PRINTF("\r**Something failed in the master bus config \r\n");
        return -1;
    }
    while(1)
    {
        // Initialize the source buffer
        for (j = 0; j < TRANSFER_SIZE; j++)
        {
            s_spiSourceBuffer[j] = j + loopCount;
        }

        // Reset the sink buffer
        for (j = 0; j < TRANSFER_SIZE; j++)
        {
            s_spiSinkBuffer[j] = 0;
        }
        // Start transfer data to slave

        if (SPI_DRV_MasterTransferBlocking(SPI_MASTER_INSTANCE,
                                            NULL,
                                            s_spiSourceBuffer,
                                            s_spiSinkBuffer,
                                            TRANSFER_SIZE,
                                            MASTER_TRANSFER_TIMEOUT) == kStatus_SPI_Timeout)
        {
            PRINTF("\r**Sync transfer timed-out \r\n");
        }


        // Verify the contents of the master sink buffer
        // refer to the slave driver for the expected data pattern
        failCount = 0; // reset failCount variable

        for (j = 0; j < TRANSFER_SIZE; j++)
        {
            if (s_spiSinkBuffer[j] != s_spiSourceBuffer[j])
            {
                 failCount++;
            }
        }

        if (failCount == 0)
        {
            PRINTF("\r\nMaster transmit:");
            for (j = 0; j < TRANSFER_SIZE; j++)
            {
                if (j%16 == 0)
                {
                    PRINTF("\r\n    ");
                }
                PRINTF(" %02X", s_spiSourceBuffer[j]);
            }
            PRINTF("\r\nMaster receive:");
            for (j = 0; j < TRANSFER_SIZE; j++)
            {
                if (j%16 == 0)
                {
                    PRINTF("\r\n    ");
                }
                PRINTF(" %02X", s_spiSinkBuffer[j]);
            }
            PRINTF("\r\n");
            PRINTF("\r Spi master blocking transfer succeed! \r\n");
        }
        else
        {
            PRINTF("\r\nSource buffer:");
            for (j = 0; j < TRANSFER_SIZE; j++)
            {
                if (j%16 == 0)
                {
                    PRINTF("\r\n    ");
                }
                PRINTF(" %02X", s_spiSourceBuffer[j]);
            }
            PRINTF("\r\nSink buffer:");
            for (j = 0; j < TRANSFER_SIZE; j++)
            {
                if (j%16 == 0)
                {
                    PRINTF("\r\n    ");
                }
                PRINTF(" %02X", s_spiSinkBuffer[j]);
            }
            PRINTF("\r\n");
            PRINTF("\r **failures detected in Spi master blocking transfer! \r\n");
        }

        // Wait for press any key.
        PRINTF("\r\nPress any key to run again\r\n");
        GETCHAR();
        loopCount++;
    }
}
/*******************************************************************************
 * EOF
 ******************************************************************************/

