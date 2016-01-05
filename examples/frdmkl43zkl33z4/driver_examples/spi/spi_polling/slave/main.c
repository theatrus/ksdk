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
#include "fsl_spi_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SPI_SLAVE_INSTANCE          BOARD_SPI_INSTANCE    /*! User change define to choose SPI instance */
#define TRANSFER_SIZE               (64)
#define SLAVE_TRANSFER_TIMEOUT      (OSA_WAIT_FOREVER)    /*! Transfer timeout of slave - 5s */

/*******************************************************************************
 * Variables
 ******************************************************************************/
// Table of SPI FIFO sizes per instance.
extern const uint32_t g_spiFifoSize[SPI_INSTANCE_COUNT];
// Table of base pointers for SPI instances.
extern SPI_Type * const g_spiBase[SPI_INSTANCE_COUNT];

// Buffer for storing data received by the SPI slave driver.
uint8_t s_spiSinkBuffer[TRANSFER_SIZE] = {0};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief SPI slave polling.
 *
 * This function sends back received buffer from master through SPI interface.
 */
int main (void)
{
    uint32_t j;
    SPI_Type * spiBaseAddr = g_spiBase[SPI_SLAVE_INSTANCE];

    // init the hardware, this also sets up up the SPI pins for each specific SoC
    hardware_init();

    OSA_Init();

    PRINTF("\r\nSPI board to board polling example");
    PRINTF("\r\nThis example run on instance %d", (uint32_t)SPI_SLAVE_INSTANCE);
    PRINTF("\r\nBe sure master's SPI%d and slave's SPI%d are connected",
                    (uint32_t)SPI_SLAVE_INSTANCE, (uint32_t)SPI_SLAVE_INSTANCE);

    // Enable clock for SPI
    CLOCK_SYS_EnableSpiClock(SPI_SLAVE_INSTANCE);

    // Reset the SPI module to its default settings including disabling SPI
    SPI_HAL_Init(spiBaseAddr);

    // Set SPI to slave mode
    SPI_HAL_SetMasterSlave(spiBaseAddr, kSpiSlave);

    // Set the SPI pin mode to normal mode
    SPI_HAL_SetPinMode(spiBaseAddr, kSpiPinMode_Normal);

#if FSL_FEATURE_SPI_FIFO_SIZE
    if (g_spiFifoSize[SPI_SLAVE_INSTANCE] != 0)
    {
        // If SPI module contains a FIFO, disable it and set watermarks to half full/empty
        SPI_HAL_SetFifoMode(spiBaseAddr, false, kSpiTxFifoOneHalfEmpty, kSpiRxFifoOneHalfFull);
    }
#endif

    //USER CONFIGURABLE OPTION FOR 8 or 16-BIT MODE (if applicable)
#if FSL_FEATURE_SPI_16BIT_TRANSFERS
    SPI_HAL_Set8or16BitMode(spiBaseAddr, kSpi8BitMode);
#endif
    // SPI system Enable
    SPI_HAL_Enable(spiBaseAddr);
    // Setup format as same as master
    SPI_HAL_SetDataFormat(spiBaseAddr, kSpiClockPolarity_ActiveHigh, kSpiClockPhase_FirstEdge, kSpiMsbFirst);

    while(1)
    {
        PRINTF("\r\nSlave example is running...\r\n");

        // Reset the sink buffer
        for (j = 0; j < TRANSFER_SIZE; j++)
        {
            s_spiSinkBuffer[j] = 0;
        }

        SPI_HAL_Disable(spiBaseAddr);
        SPI_HAL_Enable(spiBaseAddr);

        // Disable module to clear shift register
        for (j = 0; j <TRANSFER_SIZE; j++)
        {
            // Check if data received
            while(SPI_HAL_IsReadBuffFullPending(spiBaseAddr)==0){}

#if FSL_FEATURE_SPI_16BIT_TRANSFERS
            // Read data from data register
            s_spiSinkBuffer[j] = SPI_HAL_ReadDataLow(spiBaseAddr);
#else
            s_spiSinkBuffer[j] = SPI_HAL_ReadData(spiBaseAddr);
#endif
        }

        // Disable module to clear shift register
        SPI_HAL_Disable(spiBaseAddr);
        SPI_HAL_Enable(spiBaseAddr);

        // Send data back to master
        for (j = 0; j <TRANSFER_SIZE; j++)
        {
#if FSL_FEATURE_SPI_16BIT_TRANSFERS
            SPI_HAL_WriteDataBlocking(spiBaseAddr, kSpi8BitMode, 0, s_spiSinkBuffer[j]);
#else
            SPI_HAL_WriteDataBlocking(spiBaseAddr, s_spiSinkBuffer[j]);
#endif
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
