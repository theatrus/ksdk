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
#define DSPI_SLAVE_INSTANCE         (0)                 /*! User change define to choose DSPI instance */
#define TRANSFER_SIZE               (32)                /*! Transfer size */
#define TRANSFER_BAUDRATE           (500000U)           /*! Transfer baudrate - 500k */
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
 * @brief DSPI slave Polling.
 *
 * This function sends back received buffer from master through DSPI interface.
 */
int main(void)
{
    uint32_t i;
    SPI_Type * dspiBaseAddr = (SPI_Type*)SPI0_BASE;
    dspi_slave_user_config_t slaveConfig;

    // Init hardware
    hardware_init();
    // Init OSA layer, used in DSPI_DRV_MasterTransferBlocking.
    OSA_Init();

    // Print a note.
    PRINTF("\r\n DSPI board to board polling example");
    PRINTF("\r\n This example run on instance 0 ");
    PRINTF("\r\n Be sure DSPI0-DSPI0 are connected \n");

    // Enable clock for DSPI
    CLOCK_SYS_EnableSpiClock(DSPI_SLAVE_INSTANCE);
    // Reset the DSPI module, which also disables the DSPI module
    DSPI_HAL_Init(dspiBaseAddr);
    // Set to slave mode.
    DSPI_HAL_SetMasterSlaveMode(dspiBaseAddr, kDspiSlave);
    // Set data format
    slaveConfig.dataConfig.clkPhase     = kDspiClockPhase_FirstEdge;
    slaveConfig.dataConfig.clkPolarity  = kDspiClockPolarity_ActiveHigh;
    slaveConfig.dataConfig.bitsPerFrame = 8;
    DSPI_HAL_SetDataFormat(dspiBaseAddr, kDspiCtar0, &(slaveConfig.dataConfig));
    // DSPI system enable
    DSPI_HAL_Enable(dspiBaseAddr);
    // Disable FIFO operation.
    DSPI_HAL_SetFifoCmd(dspiBaseAddr, false, false);

    while(1)
    {
         PRINTF("\r\n Slave example is running...\n");
        // Reset the receive buffer.
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
            receiveBuffer[i] = 0;
        }

        // Restart the transfer by stop then start again, this will clear out the shift register
        DSPI_HAL_StopTransfer(dspiBaseAddr);
        // Flush the FIFOs
        DSPI_HAL_SetFlushFifoCmd(dspiBaseAddr, true, true);
        // Clear status flags that may have been set from previous transfers
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiTxComplete);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiEndOfQueue);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiTxFifoUnderflow);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiTxFifoFillRequest);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiRxFifoOverflow);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiRxFifoDrainRequest);
        // Clear the transfer count
        DSPI_HAL_PresetTransferCount(dspiBaseAddr, 0);
        // Start the transfer process in the hardware
        DSPI_HAL_StartTransfer(dspiBaseAddr);
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
                // Check RFDR flag
                while (DSPI_HAL_GetStatusFlag(dspiBaseAddr, kDspiRxFifoDrainRequest)== false)
                {}
                // Read data from POPR
                receiveBuffer[i] = DSPI_HAL_ReadData(dspiBaseAddr);
                // Clear RFDR flag
                DSPI_HAL_ClearStatusFlag(dspiBaseAddr,kDspiRxFifoDrainRequest);
        }

        // Restart the transfer by stop then start again, this will clear out the shift register
        DSPI_HAL_StopTransfer(dspiBaseAddr);
        // Flush the FIFOs
        DSPI_HAL_SetFlushFifoCmd(dspiBaseAddr, true, true);
        // Clear status flags that may have been set from previous transfers
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiTxComplete);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiEndOfQueue);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiTxFifoUnderflow);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiTxFifoFillRequest);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiRxFifoOverflow);
        DSPI_HAL_ClearStatusFlag(dspiBaseAddr, kDspiRxFifoDrainRequest);
        // Clear the transfer count
        DSPI_HAL_PresetTransferCount(dspiBaseAddr, 0);
        // Start the transfer process in the hardware
        DSPI_HAL_StartTransfer(dspiBaseAddr);
        // Send the data to slave.
        for (i = 0; i < TRANSFER_SIZE; i++)
        {
            // Write data to PUSHR
            DSPI_HAL_WriteDataSlavemodeBlocking(dspiBaseAddr, receiveBuffer[i]);
        }

        // Print out receive buffer.
        PRINTF("\r\n Slave receive:");
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
