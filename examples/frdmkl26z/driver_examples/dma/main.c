/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
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
// Includes
///////////////////////////////////////////////////////////////////////////////
// Standard C Included Files
#include <stdio.h>
#include <string.h>

// SDK Included Files
#include "board.h"
#include "fsl_os_abstraction.h"
#include "fsl_dma_driver.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define BUFFER_SIZE         16u /*! The size of buffer */
#define TRANSFER_SIZE       4u /*! Transfer size on basic loop */

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

// Source address in flash.
const uint8_t  srcAddr[BUFFER_SIZE] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
// Destination address in ram.
uint8_t destAddr[BUFFER_SIZE] = {0};

semaphore_t sema;

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief EDMA callback
 */
void DMA_callback(void *param, dma_channel_status_t chanStatus)
{
    OSA_SemaPost(&sema);
}

/*!
 * @brief DMA transfers form memory to memory.
 *
 * This function use DMA peripheral to transfer two buffers
 */
int main(void)
{
    dma_state_t   state;
    dma_channel_t chnState;
    osa_status_t  syncStatus;
    uint32_t      channel, i;
    bool          result;

    // Init hardware.
    hardware_init();

    // Init OSA layer.
    OSA_Init();

    PRINTF("\r\nDMA EXAMPLE \r\n");

    // Init DMA module.
    memset(&state, 0, sizeof(dma_state_t));
    DMA_DRV_Init(&state);

    // Create semaphore to synchronize dma transaction
    OSA_SemaCreate(&sema, 0);

    channel = 0;
    while (1)
    {
        // Fill zero to dest buffer
        for (i = 0; i < BUFFER_SIZE ; i ++)
        {
            destAddr[i] = 0;
        }

        // Request a channel.
        PRINTF("\r\nRequesting channel %d to transfer data from address 0x%x to 0x%x\r\n",   \
                                                                                    channel, \
                                                                          (uint32_t)srcAddr, \
                                                                          (uint32_t)destAddr);

        if (kDmaInvalidChannel == DMA_DRV_RequestChannel(channel, kDmaRequestMux0AlwaysOn62, &chnState))
        {
            PRINTF("\r\nCannot request channel No. %d", channel);
            DMA_DRV_Deinit();
            return -1;
        }

        // Configure a channel.
        DMA_DRV_ConfigTransfer(&chnState,           \
                               kDmaMemoryToMemory,  \
                               TRANSFER_SIZE,       \
                               (uint32_t)srcAddr,   \
                               (uint32_t)destAddr,  \
                               BUFFER_SIZE);

        // Add callback for dma handler
        DMA_DRV_RegisterCallback(&chnState, DMA_callback, NULL);

        // Starting channel
        PRINTF("\r\nStarting tranfer data ...");
        DMA_DRV_StartChannel(&chnState);

        // Wait until transfer is complete
        do
        {
            syncStatus = OSA_SemaWait(&sema, OSA_WAIT_FOREVER);
        } while(syncStatus == kStatus_OSA_Idle);

        // Verify destAddr buff
        result = true;
        for (i = 0; i < BUFFER_SIZE; i ++)
        {
            if (srcAddr[i] != destAddr[i])
            {
                result = false;
                break;
            }
        }

        if (true == result)
        {
            PRINTF("\r\n Transfered with DMA channel No.%d: successfull",channel);
        }
        else
        {
            PRINTF("\r\n Transfered with DMA channel No.%d: fail",channel);
        }

        // Stop channel
        DMA_DRV_StopChannel(&chnState);

        // Release channel.
        DMA_DRV_FreeChannel(&chnState);

        // Prepare next channel
        PRINTF("\r\nPress any key to start tranferring data with other channel");
        GETCHAR();
        channel ++;
        if (channel == FSL_FEATURE_DMA_DMAMUX_CHANNELS)
        {
            channel = 0;
        }
    }

}
///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////

