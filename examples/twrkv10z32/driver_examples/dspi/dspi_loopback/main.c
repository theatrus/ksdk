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
#include "fsl_dspi_master_driver.h"
#include "fsl_debug_console.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DSPI_INSTANCE               (0)       /*! User change define to choose DSPI instance */
#define TRANSFER_SIZE               (32)      /*! Transfer size */
#define TRANSFER_BAUDRATE           (500000U) /*! Transfer baudrate - 500k */
#define MASTER_TRANSFER_TIMEOUT     (5000U)   /*! Transfer timeout of master - 5s */
/*******************************************************************************
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
 * @brief DSPI loopback.
 *
 * This function sends an array through DSPI loopback interface and compare it with received buffer
 * whether they are the same.
 */
int main(void)
{
    uint32_t i;
    uint8_t loopCount = 1;
    uint32_t calculatedBaudRate;
    dspi_status_t dspiResult;
    dspi_master_state_t masterState;
    dspi_device_t masterDevice;
    dspi_master_user_config_t masterUserConfig = {
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

    // Print a note.
    PRINTF("\r\n DSPI master self loopback example ");
    PRINTF("\r\n This example run on instance 0 ");
    PRINTF("\r\n Be sure MISO-to-MOSI are connected \r\n ");

    // Setup the configuration and get user options.
    masterDevice.dataBusConfig.bitsPerFrame = 8;
    masterDevice.dataBusConfig.clkPhase     = kDspiClockPhase_FirstEdge;
    masterDevice.dataBusConfig.clkPolarity  = kDspiClockPolarity_ActiveHigh;
    masterDevice.dataBusConfig.direction    = kDspiMsbFirst;

    // Initialize master driver.
    dspiResult = DSPI_DRV_MasterInit(DSPI_INSTANCE, &masterState, &masterUserConfig);
    if (dspiResult != kStatus_DSPI_Success)
    {
        PRINTF("\r\n ERROR: Can not initialize master driver \n");
        return -1;
    }

    // configure baudrate.
    masterDevice.bitsPerSec = TRANSFER_BAUDRATE;
    dspiResult = DSPI_DRV_MasterConfigureBus(DSPI_INSTANCE, &masterDevice, &calculatedBaudRate);
    if (dspiResult != kStatus_DSPI_Success)
    {
        PRINTF("\r\nERROR: failure in config bus, error %d\n\r", dspiResult);
        return -1;
    }
    else
    {
        PRINTF("\r\n Transfer at baudrate %lu\n", calculatedBaudRate);
    }

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

        // Transfer the data.
        dspiResult = DSPI_DRV_MasterTransferBlocking(DSPI_INSTANCE,
                                                     NULL,
                                                     sendBuffer,
                                                     receiveBuffer,
                                                     TRANSFER_SIZE,
                                                     MASTER_TRANSFER_TIMEOUT);
        if (dspiResult != kStatus_DSPI_Success)
        {
            PRINTF("\r\n ERROR: transfer error, err %d\n\r", dspiResult);
            return -1;
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

        // Check receive buffer.
        for (i = 0; i < TRANSFER_SIZE; ++i)
        {
            if (receiveBuffer[i] != sendBuffer[i])
            {
                // Master received incorrect.
                PRINTF("\r\n ERROR: master received incorrect \n\r");
                return -1;
            }
        }

        PRINTF("\r\n DSPI Sends/ Recevies Successfully");
        // Wait for press any key.
        PRINTF("\r\n Press any key to run again\n");
        GETCHAR();
        // Increase loop count to change transmit buffer.
        loopCount++;
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
