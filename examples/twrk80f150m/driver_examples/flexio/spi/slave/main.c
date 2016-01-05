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
#include "fsl_spi_master_driver.h"
#else
#include "fsl_dspi_master_driver.h"
#endif

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
#if defined TWR_K80F150M || defined TWR_K81F150M || defined FRDM_K82F
#define COMM_SPI_MASTER          (0)
#else
#define COMM_SPI_MASTER          (1)
#endif
#define SPI_BAUDRATE           (200000U)           /*! Transfer baudrate - 200k */
static uint32_t s_flexioInstance;
flexio_spi_state_t slaveState;
#define BUF_LEN                 20
static uint8_t s_masterReadBuffer[BUF_LEN] = {0};
static uint8_t s_slaveReadBuffer[BUF_LEN] = {0};

static uint8_t s_masterWriteBuffer[BUF_LEN] =
{
    "Standard SPI master"
};

static uint8_t s_slaveWriteBuffer[BUF_LEN] =
{
    "FlexIO SPI slave"
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
    flexio_spi_reset_buffer(s_masterReadBuffer,BUF_LEN);
    flexio_spi_reset_buffer(s_slaveReadBuffer,BUF_LEN);
    FLEXIO_SPI_DRV_TransferData(&slaveState,s_slaveWriteBuffer,s_slaveReadBuffer,BUF_LEN);
    FLEXIO_DRV_Start(s_flexioInstance);
#if FSL_FEATURE_SOC_SPI_COUNT
    SPI_DRV_MasterTransferBlocking(COMM_SPI_MASTER,NULL,s_masterWriteBuffer,s_masterReadBuffer,BUF_LEN,1000);
#else
    DSPI_DRV_MasterTransferBlocking(COMM_SPI_MASTER,NULL,s_masterWriteBuffer,s_masterReadBuffer,BUF_LEN,1000);    
#endif
    if((spi_compare_sink_source(s_masterWriteBuffer, s_slaveReadBuffer, BUF_LEN) != true)
        ||(spi_compare_sink_source(s_slaveWriteBuffer, s_masterReadBuffer, BUF_LEN) != true))
    {
       PRINTF("\r\nSPI master to FlexIO simulated SPI slave bidirectional transfer failed!!\r\n");
       return ;
    }
    PRINTF("\r\nSPI master to FlexIO simulated SPI slave bidirectional transfer succeed!!\r\n");
}
static void master_slave_transfer_dma(void)
{
    flexio_spi_reset_buffer(s_masterReadBuffer,BUF_LEN);
    flexio_spi_reset_buffer(s_slaveReadBuffer,BUF_LEN);
#if defined FSL_FEATURE_EDMA_MODULE_CHANNEL
    FLEXIO_SPI_DRV_EdmaTransferData(&slaveState,s_slaveWriteBuffer,s_slaveReadBuffer,BUF_LEN);
#else
    FLEXIO_SPI_DRV_DmaTransferData(&slaveState,s_slaveWriteBuffer,s_slaveReadBuffer,20);    
#endif
    FLEXIO_DRV_Start(s_flexioInstance);
#if FSL_FEATURE_SOC_SPI_COUNT
    SPI_DRV_MasterTransferBlocking(COMM_SPI_MASTER,NULL,s_masterWriteBuffer,s_masterReadBuffer,BUF_LEN,1000);
#else
    DSPI_DRV_MasterTransferBlocking(COMM_SPI_MASTER,NULL,s_masterWriteBuffer,s_masterReadBuffer,BUF_LEN,1000);
#endif
    if((spi_compare_sink_source(s_masterWriteBuffer, s_slaveReadBuffer, BUF_LEN) != true)
        ||(spi_compare_sink_source(s_slaveWriteBuffer, s_masterReadBuffer, BUF_LEN) != true))
    {
       PRINTF("\r\nSPI master to FlexIO simulated SPI slave DMA bidirectional transfer failed!!\r\n");
       return ;
    }
    PRINTF("\r\nSPI master to FlexIO simulated SPI slave DMA bidirectional transfer succeed!!\r\n");
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
    uint32_t calculatedBaudRate;
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
    /* Fill in spi slave config data */
    flexio_spi_userconfig_t slaveConfig;
    slaveConfig.spiMode = kFlexIOSpiSlave;
    slaveConfig.clkPhase = kFlexIOSpiClockPhase_FirstEdge;
    slaveConfig.bitDirection = kFlexIOSpiMsbFirst;
    slaveConfig.dataSize = kFlexIOSpi8BitMode;
    slaveConfig.spiHwConfig.sdoPinIdx = 0;
#if defined FRDM_K82F    
    slaveConfig.spiHwConfig.sdiPinIdx = 2;
#else
    slaveConfig.spiHwConfig.sdiPinIdx = 1;
#endif    
#if defined TWR_K80F150M || defined TWR_K81F150M || defined FRDM_K82F
    slaveConfig.spiHwConfig.sclkPinIdx = 4;
    slaveConfig.spiHwConfig.csnPinIdx = 5;
    CLOCK_SYS_SetFlexioSrc(0,kClockFlexioSrcPllFllSelDiv);
#else
    slaveConfig.spiHwConfig.sclkPinIdx = 2;
    slaveConfig.spiHwConfig.csnPinIdx = 3;
#endif
    slaveConfig.spiHwConfig.shifterIdx[0] = 2;
    slaveConfig.spiHwConfig.shifterIdx[1] = 3;
    slaveConfig.spiHwConfig.timerIdx[0] = 2;
    /*Init FlexIO simulated SPI slave*/
#if defined FSL_FEATURE_EDMA_MODULE_CHANNEL
    edmaUserConfig.chnArbitration = kEDMAChnArbitrationRoundrobin;
    edmaUserConfig.notHaltOnError = false;
    EDMA_DRV_Init(&edmaState, &edmaUserConfig);
#else
    DMA_DRV_Init(&dmaState);
#endif
    FLEXIO_SPI_DRV_Init(s_flexioInstance,&slaveState,&slaveConfig); 
#if FSL_FEATURE_SOC_SPI_COUNT
    /*Config SPI1 as SPI master*/
    spi_master_state_t spiMasterState;
    spi_master_user_config_t masterUserConfig =
    {
    #if FSL_FEATURE_SPI_16BIT_TRANSFERS
        .bitCount       = kSpi8BitMode,
    #endif
        .polarity       = kSpiClockPolarity_ActiveHigh,
        .phase          = kSpiClockPhase_FirstEdge,
        .direction      = kSpiMsbFirst,
        .bitsPerSec     = SPI_BAUDRATE
    };
    // Init and setup baudrate for the master
    SPI_DRV_MasterInit(COMM_SPI_MASTER, &spiMasterState);
    NVIC_SetPriority(g_spiIrqId[COMM_SPI_MASTER],1);
    SPI_DRV_MasterConfigureBus(COMM_SPI_MASTER,
                                &masterUserConfig,
                                &calculatedBaudRate);
#else
    dspi_status_t dspiResult;
    dspi_master_state_t masterState;
    dspi_device_t masterDevice;
    dspi_master_user_config_t masterUserConfig = {
        .isChipSelectContinuous     = true,
        .isSckContinuous            = false,
        .pcsPolarity                = kDspiPcs_ActiveLow,
        .whichCtar                  = kDspiCtar0,
        .whichPcs                   = kDspiPcs0
    };
 // Setup the configuration.
    masterDevice.dataBusConfig.bitsPerFrame = 8;
    masterDevice.dataBusConfig.clkPhase     = kDspiClockPhase_FirstEdge;
    masterDevice.dataBusConfig.clkPolarity  = kDspiClockPolarity_ActiveHigh;
    masterDevice.dataBusConfig.direction    = kDspiMsbFirst;

    // Initialize master driver.
    dspiResult = DSPI_DRV_MasterInit(COMM_SPI_MASTER,
                                     &masterState,
                                     &masterUserConfig);
    if (dspiResult != kStatus_DSPI_Success)
    {
        PRINTF("\r\nERROR: Can not initialize master driver \r\n");
        return -1;
    }

    // Configure baudrate.
    masterDevice.bitsPerSec = 200000;
    dspiResult = DSPI_DRV_MasterConfigureBus(COMM_SPI_MASTER,
                                             &masterDevice,
                                             &calculatedBaudRate);
    if (dspiResult != kStatus_DSPI_Success)
    {
        PRINTF("\r\nERROR: failure in configuration bus\r\n");
        return -1;
    }
    else
    {
        PRINTF("\r\n Transfer at baudrate %lu \r\n", calculatedBaudRate);
    }
    NVIC_SetPriority(g_dspiIrqId[COMM_SPI_MASTER],1);
#endif
    PRINTF("\r\n++++++++++++++++ FLEXIO SPI Send/Receive Example Start +++++++++++++++++\r\n");
    PRINTF("\r\n1. SPI1 acts as master starts transfer with masterWriteBuff and masterReadBuff.\
            \r\n2. FlexIO simulated SPI slave also transfer with slaveWriteBuff and slaveReadBuff.\
            \r\n3. Compare masterWriteBuff and slaveReadBuff, slaveWriteBuff and masterReadBuff to see result.");
    PRINTF("\r\n============================================================\r\n");
    PRINTF("\r\nPress any key to start transfer:\r\n\r\n");
    while(true)
    {
      GETCHAR();
      FLEXIO_DRV_Init(s_flexioInstance,&userConfig);
      FLEXIO_SPI_DRV_Init(s_flexioInstance,&slaveState,&slaveConfig);
      master_slave_transfer_int();
      FLEXIO_DRV_Init(s_flexioInstance,&userConfig);
      FLEXIO_SPI_DRV_Init(s_flexioInstance,&slaveState,&slaveConfig);
      master_slave_transfer_dma();
      FLEXIO_SPI_DRV_Deinit(&slaveState);
    }
}
