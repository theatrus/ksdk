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
// Includes
///////////////////////////////////////////////////////////////////////////////
// Standard C Included Files
#include <stdio.h>
#include <string.h>
// Standard C Included Files
#include "fsl_flexio_driver.h"
#include "fsl_flexio_i2s_driver.h"
#include "fsl_sgtl5000_driver.h"
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_os_abstraction.h"
#include "music.h"
///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define MUSIC_LEN 65000 /*! Audio data length to playback */
///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
static sgtl_handler_t handler;
uint32_t sample_rate = 12000;
uint32_t bits = 16;
flexio_i2s_handler_t i2s_handler;
flexio_i2s_config_t config;
#if FSL_FEATURE_SOC_DMA_COUNT
dma_state_t dma_state;
#else
edma_state_t dma_state;
edma_user_config_t dma_config;
#endif

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
* @brief Configure sgtl5000 codec.
*/
void Configure_Sgtl5000(bool master)
{
    handler.i2c_instance = BOARD_SAI_DEMO_I2C_INSTANCE;
    sgtl_init_t codec_init;
    codec_init.bus = kSgtlBusI2S;
    codec_init.master_slave = master;
    codec_init.route = kSgtlRoutePlaybackandRecord;
    SGTL_Init(&handler, &codec_init);
    uint32_t mclk = sample_rate * 512; 
    SGTL_ConfigDataFormat(&handler, mclk, sample_rate, bits);
    if (!master)
    {
        SGTL_ModifyReg(&handler, CHIP_I2S_CTRL, SGTL5000_I2S_SLCKFREQ_CLR_MASK, SGTL5000_I2S_SCLKFREQ_64FS);
        SGTL_ModifyReg(&handler, CHIP_I2S_CTRL, SGTL5000_I2S_SCLK_INV_CLR_MASK, SGTL5000_I2S_VAILD_FALLING_EDGE);
        SGTL_ModifyReg(&handler, CHIP_I2S_CTRL, SGTL5000_I2S_DLEN_CLR_MASK, SGTL5000_I2S_DLEN_32);
    }
}

/*!
* @brief Set flexio as master and use interrupt to send data
*/
void i2s_master_int(void)
{
    //Configure Codec
    Configure_Sgtl5000(false);
    config.master_slave = kFlexioI2SMaster;
    //Configure flexio as an i2s master controller
    FLEXIO_I2S_DRV_Init(0, &i2s_handler, &config);
    //Start flexio functionality
    FLEXIO_DRV_Start(0);
    //Interrupt transfer way
    FLEXIO_I2S_DRV_SendDataInt(&i2s_handler, (uint8_t *)music, MUSIC_LEN);
    //Wait for data transfer finished
    osa_status_t status;
    do
    {
        status = OSA_SemaWait(&(i2s_handler.tx_sem), OSA_WAIT_FOREVER);
    }while(status == kStatus_OSA_Idle);
    PRINTF("\r\n Master Interrupt transfer Succeed! \r\n");
    SGTL_Deinit(&handler);
    FLEXIO_DRV_Pause(0);
}

/*!
* @brief Set flexio as master and use dma to send data
*/
void i2s_master_dma(void)
{
    //Configure Codec
    Configure_Sgtl5000(false);
    config.master_slave = kFlexioI2SMaster;
#if defined(K80F25615_SERIES) || defined(K81F25615_SERIES)
    config.baseSource = kDmaRequestMux0Group1FlexIO0Channel0;
#else
    config.baseSource = kDmaRequestMux0FlexIOChannel0;
#endif
    //Configure flexio as an i2s master controller
    FLEXIO_I2S_DRV_Init(0, &i2s_handler, &config);
    //Start flexio functionality
    FLEXIO_DRV_Start(0);
    FLEXIO_I2S_DRV_SendDataDma(&i2s_handler, (uint8_t *) music, MUSIC_LEN);
    osa_status_t status;
    //Wait for data transfer finished
    do
    {
        status = OSA_SemaWait(&(i2s_handler.tx_sem), OSA_WAIT_FOREVER);
    }while(status == kStatus_OSA_Idle);
	#if FSL_FEATURE_SOC_DMA_COUNT
    //Stop and release dma channel
    DMA_DRV_StopChannel(&i2s_handler.tx_dma_chn);
	DMA_DRV_FreeChannel(&i2s_handler.tx_dma_chn);
#else
    EDMA_DRV_StopChannel(&i2s_handler.tx_edma_state);
    EDMA_DRV_ReleaseChannel(&i2s_handler.tx_edma_state);
#endif

    PRINTF("\r\n Master DMA transfer Succeed! \r\n");
    SGTL_Deinit(&handler);
    FLEXIO_DRV_Pause(0);
}

/*!
* @brief Set flexio as slave and use interrupt to send data
*/
void i2s_slave_int()
{
    //Configure Codec as master
    Configure_Sgtl5000(true);
    config.master_slave = kFlexioI2SSlave;
    //Configure flexio as an i2s slave controller
    FLEXIO_I2S_DRV_Init(0, &i2s_handler, &config);
    //Start flexio functionality
    FLEXIO_DRV_Start(0);
    //Interrupt transfer way. 
    FLEXIO_I2S_DRV_SendDataInt(&i2s_handler, (uint8_t *)music, MUSIC_LEN);
    osa_status_t status;
    //Wait for data transfer finished
    do
    {
        status = OSA_SemaWait(&(i2s_handler.tx_sem), OSA_WAIT_FOREVER);
    }while(status == kStatus_OSA_Idle);
    PRINTF("\r\n Slave Interrupt transfer Succeed! \r\n");
    SGTL_Deinit(&handler);
    FLEXIO_DRV_Pause(0);
}

/*!
* @brief Set flexio as slave and use dma to send data
*/
void i2s_slave_dma()
{
    //Configure Codec as master
    Configure_Sgtl5000(true);
    config.master_slave = kFlexioI2SSlave;
#if defined(K80F25615_SERIES) || defined(K81F25615_SERIES)
    config.baseSource = kDmaRequestMux0Group1FlexIO0Channel0;
#else
    config.baseSource = kDmaRequestMux0FlexIOChannel0;
#endif
    //Configure flexio as an i2s slave controller
    FLEXIO_I2S_DRV_Init(0, &i2s_handler, &config);
    //Start flexio functionality
    FLEXIO_DRV_Start(0);
    FLEXIO_I2S_DRV_SendDataDma(&i2s_handler, (uint8_t *) music, MUSIC_LEN);
    osa_status_t status;
    //Wait for data transfer finished
    do
    {
        status = OSA_SemaWait(&(i2s_handler.tx_sem), OSA_WAIT_FOREVER);
    }while(status == kStatus_OSA_Idle);
#if FSL_FEATURE_SOC_DMA_COUNT
    //Stop and release dma channel
    DMA_DRV_StopChannel(&i2s_handler.tx_dma_chn);
    DMA_DRV_FreeChannel(&i2s_handler.tx_dma_chn);
#else
    EDMA_DRV_StopChannel(&i2s_handler.tx_edma_state);
    EDMA_DRV_ReleaseChannel(&i2s_handler.tx_edma_state);
#endif
    PRINTF("\r\n Slave DMA transfer Succeed! \r\n");
    SGTL_Deinit(&handler);
    FLEXIO_DRV_Pause(0);
}

/*!
* @brief Use flexio to do playback in 4 methods.
*/
int main(void)
{
    hardware_init();
    OSA_Init();
    //Use pin0, pin1, pin4, pin5 in flexio
    configure_flexio_pins(0,0);
    configure_flexio_pins(0,1);
    configure_flexio_pins(0,4);
    configure_flexio_pins(0,5);
#if FSL_FEATURE_SOC_DMA_COUNT
    DMA_DRV_Init(&dma_state);
#else
    dma_config.chnArbitration = kEDMAChnArbitrationRoundrobin;
    dma_config.notHaltOnError = false;
    EDMA_DRV_Init(&dma_state, &dma_config);
#endif
    while(1)
    {
        //Configure flexio I2S
        flexio_user_config_t userConfig = 
        {
            .useInt = true,
            .onDozeEnable = false,
            .onDebugEnable = true,
            .fastAccessEnable = false
        };
        //Set flexio clock source
        CLOCK_SYS_SetFlexioSrc(0,(clock_flexio_src_t)1);
        FLEXIO_DRV_Init(0,&userConfig);
        config.data_depth = bits;
        config.sample_rate = sample_rate;
        config.txPinIdx = 0; //flexio pin0 as Tx data line
        config.rxPinIdx = 1; //flexio pin1 as Rx data line
        config.sckPinIdx = 4; // flexio pin4 as SCLK
        config.wsPinIdx = 5; //flexio pin5 as Frame Sync
        config.shifterIdx[0] = 0; //Use shifter 0 for tx data
        config.shifterIdx[1] = 1; // Use shifer 1 for rx data
        config.timerIdx[0] = 0; // Use Timer0 as SCLK controller.
        config.timerIdx[1] = 1; // Use Timer1 as Frame Sync controller.
        i2s_master_int();
        i2s_master_dma();
        PRINTF("\r\nPress any key to start I2S slave transfer...\r\n");
        GETCHAR();
        i2s_slave_int();
        i2s_slave_dma();
        PRINTF("\r\nPress any key to playback again...\r\n");
        GETCHAR();
    }   
}

