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
 *
 */

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

// Standard C Included Files
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
// SDK Included Files
#include "fsl_soundcard.h"
#include "fsl_sai_driver.h"
#include "fsl_sgtl5000_driver.h"
// Application Included Files
#include "audio.h"
#include "terminal_menu.h"
// Include CMSIS-DSP library
#include "arm_math.h"

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

#if !(defined(CPU_MK22FN128VDC10) || defined(CPU_MK22FN256VDC12))
extern float32_t g_dspStore[2 * AUDIO_BUFFER_BLOCK_SIZE];
extern float32_t g_dspResult[AUDIO_BUFFER_BLOCK_SIZE];
#endif

static sai_data_format_t g_format;

static sound_card_t g_txCard;
static sound_card_t g_rxCard;
static sai_user_config_t g_txConfig;
static sai_user_config_t g_rxConfig;
static sai_state_t g_txState;
static sai_state_t g_rxState;
static volatile bool g_firstCopy = true;
static edma_state_t g_edmaState;
static edma_user_config_t g_edmaUserConfig;
static sgtl_handler_t g_codecHandler = {
    .i2c_instance = BOARD_SAI_DEMO_I2C_INSTANCE
};

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void audio_wav_init(wave_file_t *newWav)
{
    get_wav_data(newWav);

    // Configure the play audio g_format
    g_format.bits = newWav->header.bitSamp;
    g_format.sample_rate = newWav->header.sampFreq;
    g_format.mclk = 384 * g_format.sample_rate;
    g_format.mono_streo = (sai_mono_streo_t)((newWav->header.channels) - 1);

    // SAI configuration
    g_txConfig.protocol = kSaiBusI2SLeft;
    g_txConfig.channel = 0;
    g_txConfig.slave_master = kSaiMaster;
    g_txConfig.sync_mode = kSaiModeAsync;
    g_txConfig.bclk_source = kSaiBclkSourceMclkDiv;
    g_txConfig.mclk_source = kSaiMclkSourceSysclk;
    g_txConfig.mclk_divide_enable = true;
    g_txConfig.watermark = 4;

    g_rxConfig.protocol = kSaiBusI2SLeft;
    g_rxConfig.channel = 0;
    g_rxConfig.slave_master = kSaiMaster;
    g_rxConfig.sync_mode = kSaiModeSync;
    g_rxConfig.bclk_source = kSaiBclkSourceMclkDiv;
    g_rxConfig.mclk_source = kSaiMclkSourceSysclk;
    g_rxConfig.mclk_divide_enable = true;
    g_rxConfig.watermark = 4;

    g_txCard.controller.instance = 0;
    g_txCard.controller.fifo_channel = 0;
    g_txCard.controller.ops = &g_sai_ops;
    g_rxCard.controller.instance = 0;
    g_rxCard.controller.fifo_channel = 0;
    g_rxCard.controller.ops = &g_sai_ops;
#if USEDMA
    g_txCard.controller.dma_source = kDmaRequestMux0I2S0Tx;
    g_rxCard.controller.dma_source = kDmaRequestMux0I2S0Rx;
#endif
    g_txCard.codec.handler = &g_codecHandler;
    g_rxCard.codec.handler = &g_codecHandler;
    g_txCard.codec.ops = &g_sgtl_ops;
    g_rxCard.codec.ops = &g_sgtl_ops;

}

void audio_stream_init(void)
{
    // Configure the play audio g_format
    g_format.bits = 16;
    g_format.sample_rate = 48000;
    g_format.mclk = 512 * g_format.sample_rate;
    g_format.mono_streo = kSaiStreo;

    // SAI configuration
    g_txConfig.protocol = kSaiBusI2SLeft;
    g_txConfig.channel = 0;
    g_txConfig.slave_master = kSaiMaster;
    g_txConfig.sync_mode = kSaiModeAsync;
    g_txConfig.bclk_source = kSaiBclkSourceMclkDiv;
    g_txConfig.mclk_source = kSaiMclkSourceSysclk;
    g_txConfig.mclk_divide_enable = true;
    g_txConfig.watermark = 4;

    g_rxConfig.protocol = kSaiBusI2SLeft;
    g_rxConfig.channel = 0;
    g_rxConfig.slave_master = kSaiMaster;
    g_rxConfig.sync_mode = kSaiModeSync;
    g_rxConfig.bclk_source = kSaiBclkSourceMclkDiv;
    g_rxConfig.mclk_source = kSaiMclkSourceSysclk;
    g_rxConfig.mclk_divide_enable = true;
    g_rxConfig.watermark = 4;

    g_txCard.controller.instance = 0;
    g_txCard.controller.fifo_channel = 0;
    g_txCard.controller.ops = &g_sai_ops;
    g_rxCard.controller.instance = 0;
    g_rxCard.controller.fifo_channel = 0;
    g_rxCard.controller.ops = &g_sai_ops;
#if USEDMA
    g_txCard.controller.dma_source = kDmaRequestMux0I2S0Tx;
    g_rxCard.controller.dma_source = kDmaRequestMux0I2S0Rx;
#endif

    g_txCard.codec.handler = &g_codecHandler;
    g_rxCard.codec.handler = &g_codecHandler;
    g_txCard.codec.ops = &g_sgtl_ops;
    g_rxCard.codec.ops = &g_sgtl_ops;

}

uint32_t config_volume(sgtl_handler_t *handler, sgtl_module_t module, uint32_t volumeCtrl)
{
    uint32_t tempVol;
    sgtl_status_t codecStatus;

    codecStatus = kStatus_SGTL_Success;

    switch(volumeCtrl)
    {
        case 1:
            codecStatus = SGTL_SetVolume(handler, module, (uint32_t)HP_GAIN_POS_03_0);
            break;

        case 2:
            codecStatus = SGTL_SetVolume(handler, module, (uint32_t)HP_GAIN_POS_00_0);
            break;

        case 3:
            codecStatus = SGTL_SetVolume(handler, module, (uint32_t)HP_GAIN_NEG_03_0);
            break;

        case 4:
            codecStatus = SGTL_SetVolume(handler, module, (uint32_t)HP_GAIN_NEG_06_0);
            break;

        case 5:
            codecStatus = SGTL_SetVolume(handler, module, (uint32_t)HP_GAIN_NEG_12_0);
            break;

        case 6:
            codecStatus = SGTL_SetVolume(handler, module, (uint32_t)HP_GAIN_NEG_24_0);
            break;

        case 7:
            codecStatus = SGTL_SetVolume(handler, module, (uint32_t)HP_GAIN_NEG_48_0);
            break;

        default:
            __asm("NOP");
            break;
    }

    tempVol = SGTL_GetVolume(handler, module);

    if(tempVol != volumeCtrl)
    {
        return codecStatus;
    }
    else
    {
        return tempVol;
    }

}

snd_status_t get_wav_data(wave_file_t *waveFile)
{
    uint8_t *dataTemp = (uint8_t *)waveFile->data;

    // check for RIFF
    memcpy(waveFile->header.riff, dataTemp, 4);
    dataTemp += 4;
    if( memcmp( (uint8_t*)waveFile->header.riff, "RIFF", 4) )
    {
        return kStatus_SND_Fail;
    }

    // Get size
    memcpy(&waveFile->header.size, dataTemp, 4);
    dataTemp += 4;

    // .wav file flag
    memcpy(waveFile->header.waveFlag, dataTemp, 4);
    dataTemp += 4;
    if( memcmp( (uint8_t*)waveFile->header.waveFlag, "WAVE", 4) )
    {
        return kStatus_SND_Fail;
    }

    // fmt
    memcpy(waveFile->header.fmt, dataTemp, 4);
    dataTemp += 4;
    if( memcmp( (uint8_t*)waveFile->header.fmt, "fmt ", 4) )
    {
        return kStatus_SND_Fail;
    }

    // fmt length
    memcpy(&waveFile->header.fmtLen, dataTemp, 4);
    dataTemp += 4;

    // Tag: PCM or not
    memcpy(&waveFile->header.tag, dataTemp, 4);
    dataTemp += 2;

    // Channels
    memcpy(&waveFile->header.channels, dataTemp, 4);
    dataTemp += 2;

    // Sample Rate in Hz
    memcpy(&waveFile->header.sampFreq, dataTemp, 4);
    dataTemp += 4;
    memcpy(&waveFile->header.byteRate, dataTemp, 4);
    dataTemp += 4;

    // quantize bytes for per samp point
    memcpy(&waveFile->header.blockAlign, dataTemp, 4);
    dataTemp += 2;
    memcpy(&waveFile->header.bitSamp, dataTemp, 4);
    dataTemp += 2;

    // Data
    memcpy(waveFile->header.dataFlag, dataTemp, 4);
    dataTemp += 4;
    if( memcmp( (uint8_t*)waveFile->header.dataFlag, "data ", 4) )
    {
        return kStatus_SND_Fail;
    }
    memcpy(&waveFile->header.length, dataTemp, 4);
    dataTemp += 4;

    return kStatus_SND_Success;
}

snd_status_t stream_audio(dsp_types_t dspType, uint8_t volumeCtrl)
{
#if !(defined(CPU_MK22FN128VDC10) || defined(CPU_MK22FN256VDC12))
    float32_t tempF = 0.0;
#endif
    uint8_t *data = (uint8_t *)OSA_MemAllocZero(AUDIO_BUFFER_BLOCK_SIZE);
    uint32_t count = 0;
    uint8_t *pData = data;
    snd_state_t tx_status, rx_status;

    audio_stream_init();

#if USEDMA
    g_edmaUserConfig.chnArbitration = kEDMAChnArbitrationRoundrobin;
    g_edmaUserConfig.notHaltOnError = false;
    EDMA_DRV_Init(&g_edmaState, &g_edmaUserConfig);
#endif

    SND_TxInit(&g_txCard, &g_txConfig, NULL, &g_txState);
    SND_RxInit(&g_rxCard, &g_rxConfig, NULL, &g_rxState);
    SND_TxConfigDataFormat(&g_txCard,&g_format);
    SND_RxConfigDataFormat(&g_rxCard,&g_format);

    SND_RxStart(&g_rxCard);

    config_volume(g_txCard.codec.handler, kSgtlModuleHP, (uint32_t)volumeCtrl);

    for(count = 0; count < 3000; count++)
    {
        pData = data;
        // Record the data
        SND_WaitEvent(&g_rxCard);
        SND_GetStatus(&g_rxCard, &rx_status);
        memcpy(pData, rx_status.output_address, rx_status.size);
        pData = data;
        SND_RxUpdateStatus(&g_rxCard, AUDIO_BUFFER_BLOCK_SIZE);

#if !(defined(CPU_MK22FN128VDC10) || defined(CPU_MK22FN256VDC12))
        switch(dspType)
        {
            case kFFT:
                tempF = do_fft(&g_format, pData, g_dspStore, g_dspResult);
                break;

            case kNoDSP:
                __asm("NOP");
                break;

            default:
                printf("\r\nInvalid entry.\r\n");
                break;
        }
#endif

        // Play the data
        if(g_firstCopy)
        {
            g_firstCopy = false;
            SND_WaitEvent(&g_txCard);
            SND_GetStatus(&g_txCard, &tx_status);
            memcpy(tx_status.input_address, pData, tx_status.size);
            SND_TxStart(&g_txCard);
            SND_TxUpdateStatus(&g_txCard, AUDIO_BUFFER_BLOCK_SIZE);
        }
        else
        {
            SND_WaitEvent(&g_txCard);
            SND_GetStatus(&g_txCard, &tx_status);
            memcpy(tx_status.input_address, pData, tx_status.size);
            SND_TxUpdateStatus(&g_txCard,AUDIO_BUFFER_BLOCK_SIZE);
        }

    }

    OSA_MemFree(data);

    SND_TxDeinit(&g_txCard);
    SND_RxDeinit(&g_rxCard);

#if USEDMA
    EDMA_DRV_Deinit();
#endif

    g_firstCopy = true;
    
#if !(defined(CPU_MK22FN128VDC10) || defined(CPU_MK22FN256VDC12))
    if(dspType == kFFT)
    {
        printf("\r\nFrequency is %d Hz\r\n", (unsigned int)tempF);
    }
#endif
    
    memset(&g_txCard,0, sizeof(sound_card_t));
    memset(&g_rxCard,0, sizeof(sound_card_t));    

    return kStatus_SND_Success;
}

snd_status_t play_wav(uint32_t *pcmBuffer, uint8_t volumeCtrl)
{
    uint32_t bytesToRead    = 0;
    uint32_t bytesRead      = 0;

    wave_file_t *newWav = (wave_file_t *)OSA_MemAllocZero(sizeof(wave_file_t));
    newWav->data = (uint32_t *)pcmBuffer;

    audio_wav_init(newWav);

#if USEDMA
    g_edmaUserConfig.chnArbitration = kEDMAChnArbitrationRoundrobin;
    g_edmaUserConfig.notHaltOnError = false;
    EDMA_DRV_Init(&g_edmaState, &g_edmaUserConfig);
#endif

    SND_TxInit(&g_txCard, &g_txConfig, NULL, &g_txState);

    SND_TxConfigDataFormat(&g_txCard,&g_format);

    config_volume(g_txCard.codec.handler, kSgtlModuleHP, (uint32_t)volumeCtrl);

    // Remove header size from byte count
    // Adjust note duration by divider value, wav tables in pcm_data.h are 200ms by default
    bytesToRead = (newWav->header.length - WAVE_FILE_HEADER_SIZE);
    bytesRead = WAVE_FILE_HEADER_SIZE;

    // Send .wav data
    while (bytesRead < bytesToRead)
    {
        send_wav((uint8_t *)(newWav->data + bytesRead), bytesToRead, &g_format);
        bytesRead += bytesToRead;
    }

    SND_TxDeinit(&g_txCard);

#if USEDMA
    EDMA_DRV_Deinit();
#endif

    g_firstCopy = true;

    if(bytesRead >= bytesToRead)
    {
        return kStatus_SND_Success;
    }
    else
    {
        return kStatus_SND_Fail;
    }
}

void send_wav(uint8_t *dataBuffer, uint32_t length, sai_data_format_t *dataFormat)
{
    uint32_t index = 0;
    uint32_t count = 0; 
    uint8_t *pData = dataBuffer;
    snd_state_t tx_status;

    // As the sync can not initialized to be non-zero value, 
    // application have to copy a period of data firstly
    if(g_firstCopy)
    {
        g_firstCopy = false;
        SND_WaitEvent(&g_txCard);
        SND_GetStatus(&g_txCard, &tx_status);
        for(index = 0; index < tx_status.size; index++)
        {
            tx_status.input_address[index] = *pData;
            pData++; 
        }
        count += index;
        SND_TxUpdateStatus(&g_txCard, tx_status.size);
        SND_TxStart(&g_txCard);
    }
    while(count < length)
    {
        SND_WaitEvent(&g_txCard);
        SND_GetStatus(&g_txCard, &tx_status);
    // Memcopy to sai buffer
    for(index = 0; index < tx_status.size; index++)
    {
            tx_status.input_address[index] = *pData;
            pData++;
    }
        count += index;
        SND_TxUpdateStatus(&g_txCard, tx_status.size);
    }
}
