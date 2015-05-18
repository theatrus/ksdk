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
#include "fsl_spi_master_driver.h"

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
#define COMM_SPI_MASTER          (1)
#define SPI_BAUDRATE           (200000U)           /*! Transfer baudrate - 200k */
static uint32_t s_flexioInstance;
flexio_spi_state_t slaveState;
static uint8_t s_masterReadBuffer[20] = {0};
static uint8_t s_slaveReadBuffer[20] = {0};

static uint8_t s_masterWriteBuffer[20] =
{
    "Standard SPI master"
};

static uint8_t s_slaveWriteBuffer[20] =
{
    "FlexIO SPI slave"
};

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
static uint32_t spi_compare_sink_source(uint8_t *source, uint8_t *sink, uint16_t count)
{
    uint16_t i;
    /* Comapre source and sink data*/
    for (i = 0; i < count ; i++)
    {
        if (source[i] != sink[i])
        {
            return -1;
        }
    }
    return 0;
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
    FLEXIO_SPI_DRV_TransferData(&slaveState,s_slaveWriteBuffer,s_slaveReadBuffer,20);
    FLEXIO_DRV_Start(s_flexioInstance);
    SPI_DRV_MasterTransferBlocking(COMM_SPI_MASTER,NULL,s_masterWriteBuffer,s_masterReadBuffer,20,1000);
    if((spi_compare_sink_source(s_masterWriteBuffer, s_slaveReadBuffer, 20))||(spi_compare_sink_source(s_slaveWriteBuffer, s_masterReadBuffer, 20)))
    {
       PRINTF("\r\nSPI master to FlexIO simulated SPI slave bidirectional transfer failed!!\r\n");
       return ;
    }
    PRINTF("\r\nSPI master to FlexIO simulated SPI slave bidirectional transfer succeed!!\r\n");
}
static void master_slave_transfer_dma(void)
{
    flexio_spi_reset_buffer(s_masterReadBuffer,20);
    flexio_spi_reset_buffer(s_slaveReadBuffer,20);
    FLEXIO_SPI_DRV_DmaTransferData(&slaveState,s_slaveWriteBuffer,s_slaveReadBuffer,20);
    FLEXIO_DRV_Start(s_flexioInstance);
    SPI_DRV_MasterTransferBlocking(COMM_SPI_MASTER,NULL,s_masterWriteBuffer,s_masterReadBuffer,20,1000);
    if((spi_compare_sink_source(s_masterWriteBuffer, s_slaveReadBuffer, 20))||(spi_compare_sink_source(s_slaveWriteBuffer, s_masterReadBuffer, 20)))
    {
       PRINTF("\r\nSPI master to FlexIO simulated SPI slave DMA bidirectional transfer failed!!\r\n");
       return ;
    }
    PRINTF("\r\nSPI master to FlexIO simulated SPI slave DMA bidirectional transfer succeed!!\r\n");
}
int main(void)
{
    s_flexioInstance = 0;
    dma_state_t dmaState;
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
    slaveConfig.spiHwConfig.sdiPinIdx = 1;
    slaveConfig.spiHwConfig.sclkPinIdx = 2;
    slaveConfig.spiHwConfig.csnPinIdx = 3;
    slaveConfig.spiHwConfig.shifterIdx[0] = 2;
    slaveConfig.spiHwConfig.shifterIdx[1] = 3;
    slaveConfig.spiHwConfig.timerIdx[0] = 2;
    /*Init FlexIO simulated SPI slave*/
    DMA_DRV_Init(&dmaState);
    FLEXIO_SPI_DRV_Init(s_flexioInstance,&slaveState,&slaveConfig); 
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
    PRINTF("\n\r++++++++++++++++ FLEXIO SPI Send/Receive Example Start +++++++++++++++++\n\r");
    PRINTF("\n\r1. SPI1 acts as master starts transfer with masterWriteBuff and masterReadBuff.\
            \r\n2. FlexIO simulated SPI slave also transfer with slaveWriteBuff and slaveReadBuff.\
            \r\n3. Compare masterWriteBuff and slaveReadBuff, slaveWriteBuff and masterReadBuff to see result.");
    PRINTF("\r\n============================================================\r\n");
    PRINTF("\r\nPress any key to start transfer:\r\n\n");
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
