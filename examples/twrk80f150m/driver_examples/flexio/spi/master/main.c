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
 * ANY THEORY OF LIABILITY, WHETHER IN -        CONTRACT, STRICT LIABILITY, OR TORT
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
#include "fsl_flexio_spi_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#if FSL_FEATURE_SOC_SPI_COUNT
#include "fsl_spi_slave_driver.h"
#else
#include "fsl_dspi_slave_driver.h"
#endif
///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
#if defined TWR_K80F150M || defined TWR_K81F150M || defined FRDM_K82F
#define COMM_SPI_SLAVE          (0)
#else
#define COMM_SPI_SLAVE          (1)
#endif
static uint32_t s_flexioInstance;
flexio_spi_state_t masterState;
static uint8_t s_masterReadBuffer[20] = {0};
static uint8_t s_slaveReadBuffer[20] = {0};

static uint8_t s_masterWriteBuffer[20] =
{
    "FlexIO SPI master"
};

static uint8_t s_slaveWriteBuffer[20] =
{
    "Standard SPI slave "
};

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
static bool spi_compare_sink_source(uint8_t *source, uint8_t *sink, uint16_t count)
{
    uint16_t i;
    /* Comapre source and sink data*/
    for (i = 0; i < count ; i++)
    {
        if (source[i] != sink[i])
        {
            return false;
        }
    }
    return true;
}
static void flexio_spi_reset_buffer(uint8_t *txBuff, uint32_t count)
{
    uint32_t i;
    /* Comapre source and sink data*/
    for (i = 0; i < count ; i++)
    {
        txBuff[i] = 0;
    }
}
static void master_slave_transfer_int(void)
{
    flexio_spi_reset_buffer(s_masterReadBuffer,20);
    flexio_spi_reset_buffer(s_slaveReadBuffer,20);
#if FSL_FEATURE_SOC_SPI_COUNT
    SPI_DRV_SlaveTransfer(COMM_SPI_SLAVE,s_slaveWriteBuffer,s_slaveReadBuffer,20);
#else
    DSPI_DRV_SlaveTransfer(COMM_SPI_SLAVE,s_slaveWriteBuffer,s_slaveReadBuffer,20);
#endif
    FLEXIO_SPI_DRV_TransferDataBlocking(&masterState,s_masterWriteBuffer,s_masterReadBuffer,20,1000);
    if((spi_compare_sink_source(s_masterWriteBuffer, s_slaveReadBuffer, 20) != true)
        || (spi_compare_sink_source(s_slaveWriteBuffer, s_masterReadBuffer, 20) != true))
    {
       PRINTF("\r\nFlexIO simulated SPI master to SPI slave bidirectional transfer failed!!\r\n");
       return ;
    }
    PRINTF("\r\nFlexIO simulated SPI master to SPI slave bidirectional transfer succeed!!\r\n");
}
static void master_slave_transfer_dma(void)
{
    flexio_spi_reset_buffer(s_masterReadBuffer,20);
    flexio_spi_reset_buffer(s_slaveReadBuffer,20);
#if FSL_FEATURE_SOC_SPI_COUNT
    SPI_DRV_SlaveTransfer(COMM_SPI_SLAVE,s_slaveWriteBuffer,s_slaveReadBuffer,20);
#else
    DSPI_DRV_SlaveTransfer(COMM_SPI_SLAVE,s_slaveWriteBuffer,s_slaveReadBuffer,20);
#endif
#if defined FSL_FEATURE_EDMA_MODULE_CHANNEL
    FLEXIO_SPI_DRV_EdmaTransferDataBlocking(&masterState,s_masterWriteBuffer,s_masterReadBuffer,20,1000);
#else
    FLEXIO_SPI_DRV_DmaTransferDataBlocking(&masterState,s_masterWriteBuffer,s_masterReadBuffer,20,1000);  
#endif
    if((spi_compare_sink_source(s_masterWriteBuffer, s_slaveReadBuffer, 20) != true)
        ||(spi_compare_sink_source(s_slaveWriteBuffer, s_masterReadBuffer, 20) != true))
    {
       PRINTF("\r\nFlexIO simulated SPI master to SPI slave transfer DMA bidirectional transfer failed!!\r\n");
       return ;
    }
    PRINTF("\r\nFlexIO simulated SPI master to SPI slave transfer DMA bidirectional transfer succeed!!\r\n");
}

int main(void)
{
    s_flexioInstance = 0;
#if defined FSL_FEATURE_EDMA_MODULE_CHANNEL
    edma_state_t         edmaState;
    edma_user_config_t   edmaUserConfig;
#else
    dma_state_t dmaState;
#endif
    flexio_user_config_t userConfig = 
    {
        .useInt = true,
        .onDozeEnable = false,
        .onDebugEnable = true,
        .fastAccessEnable = false
    };
    hardware_init();
    OSA_Init();
    FLEXIO_DRV_Init(s_flexioInstance,&userConfig);
    /* Fill in spi master config data */
    flexio_spi_userconfig_t masterConfig;
    masterConfig.spiMode = kFlexIOSpiMaster;
    masterConfig.baudRate = 200000;
    masterConfig.clkPhase = kFlexIOSpiClockPhase_FirstEdge;
    masterConfig.bitDirection = kFlexIOSpiMsbFirst;
    masterConfig.dataSize = kFlexIOSpi8BitMode;
    masterConfig.spiHwConfig.sdoPinIdx = 0;
#if defined FRDM_K82F    
    masterConfig.spiHwConfig.sdiPinIdx = 2;
#else
    masterConfig.spiHwConfig.sdiPinIdx = 1;
#endif
#if defined TWR_K80F150M || defined TWR_K81F150M || defined FRDM_K82F
    masterConfig.spiHwConfig.sclkPinIdx = 4;
    masterConfig.spiHwConfig.csnPinIdx = 5;
#else
    masterConfig.spiHwConfig.sclkPinIdx = 2;
    masterConfig.spiHwConfig.csnPinIdx = 3;
#endif
    masterConfig.spiHwConfig.shifterIdx[0] = 0;
    masterConfig.spiHwConfig.shifterIdx[1] = 1;
    masterConfig.spiHwConfig.timerIdx[0] = 0;
    masterConfig.spiHwConfig.timerIdx[1] = 1;
    /*Init FlexIO simulated SPI master*/
#if defined FSL_FEATURE_EDMA_MODULE_CHANNEL
    edmaUserConfig.chnArbitration = kEDMAChnArbitrationRoundrobin;
    edmaUserConfig.notHaltOnError = false;
    EDMA_DRV_Init(&edmaState, &edmaUserConfig);
#else
    DMA_DRV_Init(&dmaState);
#endif
    FLEXIO_SPI_DRV_Init(s_flexioInstance,&masterState,&masterConfig); 
#if FSL_FEATURE_SOC_SPI_COUNT
    /*Config SPI1 as SPI slave*/
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
    /*Init SPI1 as slave to communicate with FlexIO simulated SPI master*/
    SPI_DRV_SlaveInit(COMM_SPI_SLAVE, &spiSlaveState, &slaveUserConfig);
#else
    /*Config DSP1 as SPI slave*/
    dspi_status_t dspiResult;
    dspi_slave_state_t slaveState;
    dspi_slave_user_config_t slaveConfig;
    // Setup the configuration
    slaveConfig.dataConfig.bitsPerFrame = 8;
    slaveConfig.dataConfig.clkPhase     = kDspiClockPhase_FirstEdge;
    slaveConfig.dataConfig.clkPolarity  = kDspiClockPolarity_ActiveHigh;
    slaveConfig.dummyPattern            = 0;
    // Initialize slave driver.
    dspiResult = DSPI_DRV_SlaveInit(COMM_SPI_SLAVE,
                                     &slaveState,
                                     &slaveConfig);
    if (dspiResult != kStatus_DSPI_Success)
    {
        PRINTF("\r\n ERROR: Can not initialize slave driver \r\n");
        return -1;
    }

#endif
#if FSL_FEATURE_SOC_SPI_COUNT
    NVIC_SetPriority(g_spiIrqId[COMM_SPI_SLAVE],1);
#else
    NVIC_SetPriority(g_dspiIrqId[COMM_SPI_SLAVE],1);
#endif
    FLEXIO_DRV_Start(s_flexioInstance);
    PRINTF("\r\n++++++++++++++++ FLEXIO SPI Send/Receive Example Start +++++++++++++++++\r\n");
    PRINTF("\r\n1. FlexIO simulated SPI master starts transfer with masterWriteBuff and masterReadBuff.\
            \r\n2. SPI1 acts as slave also transfer with slaveWriteBuff and slaveReadBuff.\
            \r\n3. Compare masterWriteBuff and slaveReadBuff, slaveWriteBuff and masterReadBuff to see result.");
    PRINTF("\r\n============================================================\r\n");
    PRINTF("\r\nPress any key to start transfer:\r\n\r\n");
    while(true)
    {
      GETCHAR();
      master_slave_transfer_int();
      master_slave_transfer_dma();
    }
}
