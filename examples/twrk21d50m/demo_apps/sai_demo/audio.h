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

#ifndef __AUDIO_H__
#define __AUDIO_H__

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

// Standard C Included Files
#include <stdint.h>
// SDK Included Files
#include "fsl_soundcard.h"
#include "fsl_sai_driver.h"
// Application Included Files
#include "equalizer.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Standard Winodws PCM wave file header length
 */
#define WAVE_FILE_HEADER_SIZE   0x2CU
// Gain Levels
#define HP_GAIN_POS_12_0    0x00
#define HP_GAIN_POS_11_5    0x01
#define HP_GAIN_POS_11_0    0x02
#define HP_GAIN_POS_10_5    0x03
#define HP_GAIN_POS_10_0    0x04
#define HP_GAIN_POS_09_5    0x05
#define HP_GAIN_POS_09_0    0x06
#define HP_GAIN_POS_08_5    0x07
#define HP_GAIN_POS_08_0    0x08
#define HP_GAIN_POS_07_5    0x09
#define HP_GAIN_POS_07_0    0x0A
#define HP_GAIN_POS_06_5    0x0B
#define HP_GAIN_POS_06_0    0x0C
#define HP_GAIN_POS_05_5    0x0D
#define HP_GAIN_POS_05_0    0x0E
#define HP_GAIN_POS_04_5    0x0F
#define HP_GAIN_POS_04_0    0x10
#define HP_GAIN_POS_03_5    0x11
#define HP_GAIN_POS_03_0    0x12
#define HP_GAIN_POS_02_5    0x13
#define HP_GAIN_POS_02_0    0x14
#define HP_GAIN_POS_01_5    0x15
#define HP_GAIN_POS_01_0    0x16
#define HP_GAIN_POS_00_5    0x17
#define HP_GAIN_POS_00_0    0x18
#define HP_GAIN_NEG_00_5    0x19
#define HP_GAIN_NEG_01_0    0x1A
#define HP_GAIN_NEG_01_5    0x1B
#define HP_GAIN_NEG_02_0    0x1C
#define HP_GAIN_NEG_02_5    0x1D
#define HP_GAIN_NEG_03_0    0x1E
#define HP_GAIN_NEG_03_5    0x1F
#define HP_GAIN_NEG_04_0    0x20
#define HP_GAIN_NEG_04_5    0x21
#define HP_GAIN_NEG_05_0    0x22
#define HP_GAIN_NEG_05_5    0x23
#define HP_GAIN_NEG_06_0    0x24
#define HP_GAIN_NEG_06_5    0x25
#define HP_GAIN_NEG_07_0    0x26
#define HP_GAIN_NEG_07_5    0x27
#define HP_GAIN_NEG_08_0    0x28
#define HP_GAIN_NEG_08_5    0x29
#define HP_GAIN_NEG_09_0    0x2A
#define HP_GAIN_NEG_09_5    0x2B
#define HP_GAIN_NEG_10_0    0x2C
#define HP_GAIN_NEG_10_5    0x2D
#define HP_GAIN_NEG_11_0    0x2E
#define HP_GAIN_NEG_11_5    0x2F
#define HP_GAIN_NEG_12_0    0x30
#define HP_GAIN_NEG_12_5    0x31
#define HP_GAIN_NEG_13_0    0x32
#define HP_GAIN_NEG_13_5    0x33
#define HP_GAIN_NEG_14_0    0x34
#define HP_GAIN_NEG_14_5    0x35
#define HP_GAIN_NEG_15_0    0x36
#define HP_GAIN_NEG_15_5    0x37
#define HP_GAIN_NEG_16_0    0x38
#define HP_GAIN_NEG_16_5    0x39
#define HP_GAIN_NEG_17_0    0x3A
#define HP_GAIN_NEG_17_5    0x3B
#define HP_GAIN_NEG_18_0    0x3C
#define HP_GAIN_NEG_18_5    0x3D
#define HP_GAIN_NEG_19_0    0x3E
#define HP_GAIN_NEG_19_5    0x3F
#define HP_GAIN_NEG_20_0    0x40
#define HP_GAIN_NEG_20_5    0x41
#define HP_GAIN_NEG_21_0    0x42
#define HP_GAIN_NEG_21_5    0x43
#define HP_GAIN_NEG_22_0    0x44
#define HP_GAIN_NEG_22_5    0x45
#define HP_GAIN_NEG_23_0    0x46
#define HP_GAIN_NEG_23_5    0x47
#define HP_GAIN_NEG_24_0    0x48
#define HP_GAIN_NEG_24_5    0x49
#define HP_GAIN_NEG_25_0    0x4A
#define HP_GAIN_NEG_25_5    0x4B
#define HP_GAIN_NEG_26_0    0x4C
#define HP_GAIN_NEG_26_5    0x4D
#define HP_GAIN_NEG_27_0    0x4E
#define HP_GAIN_NEG_27_5    0x4F
#define HP_GAIN_NEG_28_0    0x50
#define HP_GAIN_NEG_28_5    0x51
#define HP_GAIN_NEG_29_0    0x52
#define HP_GAIN_NEG_29_5    0x53
#define HP_GAIN_NEG_30_0    0x54
#define HP_GAIN_NEG_30_5    0x55
#define HP_GAIN_NEG_31_0    0x56
#define HP_GAIN_NEG_31_5    0x57
#define HP_GAIN_NEG_32_0    0x58
#define HP_GAIN_NEG_32_5    0x59
#define HP_GAIN_NEG_33_0    0x5A
#define HP_GAIN_NEG_33_5    0x5B
#define HP_GAIN_NEG_34_0    0x5C
#define HP_GAIN_NEG_34_5    0x5D
#define HP_GAIN_NEG_35_0    0x5E
#define HP_GAIN_NEG_35_5    0x5F
#define HP_GAIN_NEG_36_0    0x60
#define HP_GAIN_NEG_36_5    0x61
#define HP_GAIN_NEG_37_0    0x62
#define HP_GAIN_NEG_37_5    0x63
#define HP_GAIN_NEG_38_0    0x64
#define HP_GAIN_NEG_38_5    0x65
#define HP_GAIN_NEG_39_0    0x66
#define HP_GAIN_NEG_39_5    0x67
#define HP_GAIN_NEG_40_0    0x68
#define HP_GAIN_NEG_40_5    0x69
#define HP_GAIN_NEG_41_0    0x6A
#define HP_GAIN_NEG_41_5    0x6B
#define HP_GAIN_NEG_42_0    0x6C
#define HP_GAIN_NEG_42_5    0x6D
#define HP_GAIN_NEG_43_0    0x6E
#define HP_GAIN_NEG_43_5    0x6F
#define HP_GAIN_NEG_44_0    0x70
#define HP_GAIN_NEG_44_5    0x71
#define HP_GAIN_NEG_45_0    0x72
#define HP_GAIN_NEG_45_5    0x73
#define HP_GAIN_NEG_46_0    0x74
#define HP_GAIN_NEG_46_5    0x75
#define HP_GAIN_NEG_47_0    0x76
#define HP_GAIN_NEG_47_5    0x77
#define HP_GAIN_NEG_48_0    0x78
#define HP_GAIN_NEG_48_5    0x79
#define HP_GAIN_NEG_49_0    0x7A
#define HP_GAIN_NEG_49_5    0x7B
#define HP_GAIN_NEG_50_0    0x7C
#define HP_GAIN_NEG_50_5    0x7D
#define HP_GAIN_NEG_51_0    0x7E
#define HP_GAIN_NEG_51_5    0x7F

/*! 
 * @brief Standard Windows PCM wave file header structure.
 */
typedef struct wave_header
{
    uint8_t  riff[4];
    uint32_t size;
    uint8_t  waveFlag[4];
    uint8_t  fmt[4];
    uint32_t fmtLen;
    uint16_t tag;
    uint16_t channels;
    uint32_t sampFreq;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitSamp;
    uint8_t  dataFlag[4];
    uint32_t length;
} wave_header_t;

/*! 
 * @brief Wave file structure
 */
typedef struct wave_file
{
    wave_header_t header;
    uint32_t *data;
}wave_file_t;

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Initialize I2S, I2C, & TWR-AUDIO-SGTL board.
 */
void audio_stream_init(void);

/*!
 * @brief Initialize audio WAV.
 *
 * @param [in] newWav Pointer to a WAV file.
 */
void audio_wav_init(wave_file_t *newWav);

/*!
 * @brief Sets volume from user input.
 * 
 * @param [in] handler    Pointer to SGTL handler.
 * @param [in] module     SGTL module.
 * @param [in] volumeCtrl Input data from terminal menu.
 * @retval kStatus_Success if function completed successfully.
 * @retval kStatusFail if function failed.
 */
uint32_t config_volume(sgtl_handler_t *handler, sgtl_module_t module, uint32_t volumeCtrl);

/*!
 * @brief Plays a stream of audio.
 * 
 * @param [in] dspType    Used to select one DSP function to perform on the data
 * @param [in] volumeCtrl Input data from terminal menu.
 * @return soundcard status.
 */
snd_status_t stream_audio(dsp_types_t dspType, uint8_t volumeCtrl);

/*!
 * @brief Collects data from wav file header.
 * 
 * @param [in] waveFile Data structure of pcm data array.
 * @retval kStatus_Success if function completed successfully.
 * @retval kStatusFail if function failed.
 */
snd_status_t get_wav_data(wave_file_t *waveFile);

/*!
 * @brief Play PCM audio data from wav format array.
 * 
 * @param [in] pcmBuffer  Pointer to data array containing wav formatted audio data.
 * @param [in] volumeCtrl Input data from terminal menu.
 * @retval kStatus_Success if function completed successfully.
 * @retval kStatusFail if function failed.
 */
snd_status_t play_wav(uint32_t *pcmBuffer, uint8_t volumeCtrl);

/*!
 * @brief Send audio data to sound card.
 * 
 * @param [in] dataBuffer Pointer to data array containing wav formatted audio data.
 * @param [in] length     Length of the dataBuffer
 * @param [in] dataFormat Point to audio_data_format_t for sound card.
 */
void send_wav(uint8_t *dataBuffer, uint32_t length, sai_data_format_t *dataFormat);

#endif // __AUDIO_H__
