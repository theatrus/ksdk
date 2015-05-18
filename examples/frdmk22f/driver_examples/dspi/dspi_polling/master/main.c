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
#include "fsl_dspi_master_driver.h"
#include "fsl_os_abstraction.h"
#include "fsl_debug_console.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DSPI_MASTER_INSTANCE        (0)                 /*! User change define to choose DSPI instance */
#define TRANSFER_SIZE               (32)                /*! Transfer size */
#define TRANSFER_BAUDRATE           (500000U)           /*! Transfer baudrate - 500k */

/*!******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t receiveBuffer[TRANSFER_SIZE] = {0};
uint8_t sendBuffer[TRANSFER_SIZE] = {0};

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief DSPI master Polling.
 *
 * Thid function uses DSPI master to send an array to slave
 * and receive the array back from slave,
 * thencompare whether the two buffers are the same.
 */
int main(void)
{
    uint32_t i;
    uint32_t loopCount = 1;
    SPI_Type * dspiBaseAddr = (SPI_Type*)SPI0_BASE;
    uint32_t dspiSourceClock;
    uint32_t calculatedBaudRate;
    dspi_device_t masterDevice;
    dspi_command_config_t commandConfig =
    {
        .isChipSelectContinuous = false,
        .whichCtar              = kDspiCtar0,
        .whichPcs               = kDspiPcs0,
        .clearTransferCount     = true,
        .isEndOfQueue           = false
    };

    // Init hardware
    hardware_init();
    // Init OSA layer, used in DSPI_DRV_MasterTransferBlocking.
    OSA_Init();

    // Print a note.
    PRINTF("\r\n DSPI board to board polling example");
    PRINTF("\r\n This example run on instance 0 ");
    PRINTF("\r\n Be sure DSPI0-DSPI0 are connected \n");

    // Enable DSPI clock.
    CLOCK_SYS_EnableSpiClock(DSPI_MASTER_INSTANCE);
    // Initialize the DSPI module registers to default value, which disables the module
    DSPI_HAL_Init(dspiBaseAddr);
    // Set to master mode.
    DSPI_HAL_SetMasterSlaveMode(dspiBaseAddr, kDspiMaster);
    // Configure for continuous SCK operation
    DSPI_HAL_SetContinuousSckCmd(dspiBaseAddr, false);
    // Configure for peripheral chip select polarity
    DSPI_HAL_SetPcsPolarityMode(dspiBaseAddr, kDspiPcs0, kDspiPcs_ActiveLow);
    // Disable FIFO operation.
    DSPI_HAL_SetFifoCmd(dspiBaseAddr, false, false);
    // Initialize the configurable delays: PCS-to-SCK, prescaler = 0, scaler = 1
    DSPI_HAL_SetDelay(dspiBaseAddr, kDspiCtar0, 0, 1, kDspiPcsToSck);
    // DSPI system enable
    DSPI_HAL_Enable(dspiBaseAddr);

    // Configure baudrate.
    masterDevice.dataBusConfig.bitsPerFrame = 8;
    masterDevice.dataBusConfig.clkPhase     = kDspiClockPhase_FirstEdge;
    masterDevice.dataBusConfig.clkPolarity  = kDspiClockPolarity_ActiveHigh;
    masterDevice.dataBusConfig.direction    = kDspiMsbFirst;

    DSPI_HAL_SetDataFormat(dspiBaseAddr, kDspiCtar0, &masterDevice.dataBusConfig);
    // Get DSPI source clock.
    dspiSourceClock = CLOCK_SYS_GetSpiFreq(DSPI_MASTER_INSTANCE);
    calculatedBaudRate = DSPI_HAL_SetBaudRate(dspiBaseAddr, kDspiCtar0, TRANSFER_BAUDRATE, dspiSourceClock);
    PRINTF("\r\n Transfer at baudrate %lu \r\n", calculatedBaudRate);

    while(1)
    {
        // Initialize the transmit buffer.
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
            sendBuffer[i] = i + loopCount;
        }

        // Print out transmit buffer.
        PRINTF("\r\n Master transmit:\n");
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
            // Print 16 numbers in a line.
            if ((i & 0x0F) == 0)
            {
                PRINTF("\r\n    ");
            }
            PRINTF(" %02X", sendBuffer[i]);
        }

        // Reset the receive buffer.
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
            receiveBuffer[i] = 0;
        }

        // Restart the transfer by stop then start again, this will clear out the shift register
        DSPI_HAL_StopTransfer(dspiBaseAddr);
        // Flush the FIFOs
        DSPI_HAL_SetFlushFifoCmd(dspiBaseAddr, true, true);
        // Clear status flags that may have been set from previous transfers.
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiTxComplete);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiEndOfQueue);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiTxFifoUnderflow);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiTxFifoFillRequest);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiRxFifoOverflow);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiRxFifoDrainRequest);
        // Clear the transfer count.
        DSPI_HAL_PresetTransferCount(dspiBaseAddr, 0);
        // Start the transfer process in the hardware
        DSPI_HAL_StartTransfer(dspiBaseAddr);
        // Send the data to slave.
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
            // Write data to PUSHR
            DSPI_HAL_WriteDataMastermodeBlocking(dspiBaseAddr, &commandConfig, sendBuffer[i]);
            // Delay to wait slave is ready.
            OSA_TimeDelay(1);
        }

        // Delay to wait slave is ready.
        OSA_TimeDelay(10);

        // Restart the transfer by stop then start again, this will clear out the shift register
        DSPI_HAL_StopTransfer(dspiBaseAddr);
        // Flush the FIFOs
        DSPI_HAL_SetFlushFifoCmd(dspiBaseAddr, true, true);
        //Clear status flags that may have been set from previous transfers.
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiTxComplete);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiEndOfQueue);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiTxFifoUnderflow);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiTxFifoFillRequest);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiRxFifoOverflow);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiRxFifoDrainRequest);
        // Clear the transfer count.
        DSPI_HAL_PresetTransferCount(dspiBaseAddr, 0);
        // Start the transfer process in the hardware
        DSPI_HAL_StartTransfer(dspiBaseAddr);
        // Receive the data from slave.
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
            // Write command to PUSHR.
            DSPI_HAL_WriteDataMastermodeBlocking(dspiBaseAddr, &commandConfig, 0);
            // Check RFDR flag
            while (DSPI_HAL_GetStatusFlag(dspiBaseAddr, kDspiRxFifoDrainRequest)== false)
            {}
            // Read data from POPR
            receiveBuffer[i] = DSPI_HAL_ReadData(dspiBaseAddr);
            // Clear RFDR flag
            DSPI_HAL_ClearStatusFlag(dspiBaseAddr,kDspiRxFifoDrainRequest);
            // Delay to wait slave is ready.
            OSA_TimeDelay(1);
        }

        // Print out receive buffer.
        PRINTF("\r\n Master receive:\n");
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
            // Print 16 numbers in a line.
            if ((i & 0x0F) == 0)
            {
                PRINTF("\r\n    ");
            }
            PRINTF(" %02X", receiveBuffer[i]);
        }

        // Check receiveBuffer.
        for (i = 0; i < TRANSFER_SIZE; ++i)
        {
            if (receiveBuffer[i] != sendBuffer[i])
            {
                // Master received incorrect.
                PRINTF("\r\n ERROR: master received incorrect \n\r");
                return -1;
            }
        }

        PRINTF("\r\n DSPI Master Sends/ Recevies Successfully");
        // Wait for press any key.
        PRINTF("\r\n Press any key to run again\n");
        GETCHAR();
        // Increase loop count to change transmit buffer.
        loopCount++;
    }
}

/*******************************************************************************
 * EOF
 *******************************************************************************/
