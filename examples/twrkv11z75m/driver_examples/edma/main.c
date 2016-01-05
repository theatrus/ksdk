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
#include "fsl_edma_driver.h"
#include "fsl_os_abstraction.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define BUFFER_SIZE               16/*! Total transfer size */
#define EDMA_TRANSFER_SIZE        2 /*! Transfer size on basic loop */
#define EDMA_CHAIN_LENGTH         1 /*! Number of srcSG and destSG */
#define EDMA_WARTERMARK_LEVEL     8 /*! number of bytes transfered on each EDMA request(minor loop) */

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

// Source address in flash.
uint8_t srcAddr[BUFFER_SIZE] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
// Destination address in ram.
uint8_t destAddr[BUFFER_SIZE] = {0};

semaphore_t sema;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief EDMA callback
 */
void EDMA_Callback(void *param, edma_chn_status_t chanStatus)
{
    OSA_SemaPost(&sema);
}

/*!
 * @brief EDMA transfers form memory to memory.
 *
 * This function use EDMA peripheral to transfer two buffers
 */
int main(void)
{
    edma_chn_state_t     chnState;
    edma_software_tcd_t *stcd;
    edma_state_t         edmaState;
    edma_user_config_t   edmaUserConfig;
    edma_scatter_gather_list_t srcSG, destSG;

    osa_status_t         syncStatus;
    bool                 result;
    uint32_t             i, channel = 0;

    // Init hardware.
    hardware_init();

    // Init OSA layer.
    OSA_Init();

    PRINTF("\r\n EDMA transfer from memory to memory \r\n");

    // Init eDMA modules.
    edmaUserConfig.chnArbitration = kEDMAChnArbitrationRoundrobin;
    edmaUserConfig.notHaltOnError = false;
    EDMA_DRV_Init(&edmaState, &edmaUserConfig);

    // Create semaphore to synchronize edma transaction
    OSA_SemaCreate(&sema, 0);

    while (1)
    {
        // EDMA channel request.
        EDMA_DRV_RequestChannel(channel, kDmaRequestMux0AlwaysOn62, &chnState);

        // Fill zero to destination buffer
        for (i = 0; i < BUFFER_SIZE; i ++)
        {
            destAddr[i] = 0x00;
        }

        // Prepare memory pointing to software TCDs.
        stcd = OSA_MemAllocZero(STCD_SIZE(EDMA_CHAIN_LENGTH));

        // Configure EDMA channel.
        srcSG.address  = (uint32_t)srcAddr;
        destSG.address = (uint32_t)destAddr;
        srcSG.length   = BUFFER_SIZE;
        destSG.length  = BUFFER_SIZE;

        // configure single end descritptor chain.
        EDMA_DRV_ConfigScatterGatherTransfer(
                &chnState, stcd, kEDMAMemoryToMemory,
                EDMA_TRANSFER_SIZE, EDMA_WARTERMARK_LEVEL,
                &srcSG, &destSG,
                EDMA_CHAIN_LENGTH);

        // Install callback for eDMA handler
        EDMA_DRV_InstallCallback(&chnState, EDMA_Callback, NULL);

        PRINTF("\r\n Starting EDMA channel No. %d to transfer data from addr 0x%x to addr 0x%x",  \
                                                                                        channel,  \
                                                                                (uint32_t)srcAddr,\
                                                                                (uint32_t)destAddr);

        // Initialize transfer.
        EDMA_DRV_StartChannel(&chnState);

        // Wait until transfer is complete
        do
        {
            syncStatus = OSA_SemaWait(&sema, OSA_WAIT_FOREVER);
        }while(syncStatus == kStatus_OSA_Idle);

        // Verify destAddr buff
        result = true;
        for (i = 0; i < BUFFER_SIZE; i ++)
        {
            if (destAddr[i] != srcAddr[i])
            {
                result = false;
                break;
            }
        }

        if (true == result)
        {
            PRINTF("\r\n Transfered with eDMA channel No.%d: successfull",channel);
        }
        else
        {
            PRINTF("\r\n Transfered with eDMA channel No.%d: fail",channel);
        }

        // Stop channel
        EDMA_DRV_StopChannel(&chnState);

        // Release channel
        EDMA_DRV_ReleaseChannel(&chnState);

        // Free stcd
        OSA_MemFree((void *)stcd);

        // Prepare for another channel
        PRINTF("\r\nPress any key to start transfer with other channel");
        GETCHAR();
        channel ++;
        if (channel == DMA_INSTANCE_COUNT * FSL_FEATURE_EDMA_MODULE_CHANNEL)
        {
            channel = 0;
        }
    }

}
///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////

