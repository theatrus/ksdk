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
#include "fsl_debug_console.h"
// Application Included Files
#include "audio.h"
#include "equalizer.h"
#include "terminal_menu.h"
// Include CMSIS-DSP library
#include "arm_math.h"

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

float32_t do_fft(sai_data_format_t *dataFormat, uint8_t *buffer, float32_t *fftData, float32_t *fftResult)
{
    // Counter variable for navigating buffers
    uint32_t counter;

    // Return value for wav frequency in hertz
    float32_t wavFreqHz;

    // CMSIS status & FFT instance
    arm_status status;                  // ARM status variable
    arm_cfft_radix2_instance_f32 fft;   // ARM FFT instance

    // Frequency analysis variables
    float32_t maxValue;             // max value for greatest FFT bin amplitude
    uint32_t testIndex = 0;         // value for storing the bin location with maxValue
    uint32_t complexBuffSize = AUDIO_BUFFER_BLOCK_SIZE * 2;
    uint32_t fftSize = AUDIO_BUFFER_BLOCK_SIZE;     // FFT bin size
    uint32_t ifftFlag = 0;                          // Flag for the selection of CFFT/CIFFT
    uint32_t doBitReverse = 1;  // Flag for selection of normal order or bit reversed order
    uint32_t sampleRate = dataFormat->sample_rate;  // Get sample rate from current format
    float32_t hzPerBin = 2 * ((float32_t)sampleRate/(float32_t)fftSize);    // Calculate hz per FFT bin

    uint8_t *temp8;                 // Point to data for 8 bit samples
    uint8_t  temp8Data;

    uint16_t *temp16;               // Point to data for 16 bit samples
    int16_t   temp16Data;

    uint32_t *temp32;               // Point to data for 32 bit samples
    int32_t   temp32Data;

    // Set status as success
    status = ARM_MATH_SUCCESS;

    // Wav data variables
    switch(dataFormat->bits)
    {
        case 8:
            temp8 = (uint8_t *)buffer;
            temp8Data = 0;

            // Copy wav data to fft input array
            for(counter = 0; counter < complexBuffSize; counter++)
            {
                if(counter % 2 == 0)
                {
                    temp8Data = (uint8_t)*temp8;
                    fftData[counter] = (float32_t)temp8Data;
                    temp8++;
                }
                else
                {
                    fftData[counter] = 0.0;
                }
            }

            // Set instance for Real FFT
            status = arm_cfft_radix2_init_f32(&fft, fftSize, ifftFlag, doBitReverse);

            // Perform Real FFT on fftData
            arm_cfft_radix2_f32(&fft, fftData);

            // Populate FFT bins
            arm_cmplx_mag_f32(fftData, fftResult, fftSize);

            // Zero out non-audible, low-frequency noise from FFT Results.
            fftResult[0] = 0.0;

            // Find max bin and location of max (first half of bins as this is the only valid section)
            arm_max_f32(fftResult, fftSize, &maxValue, &testIndex);

            break;

        case 16:
            temp16 = (uint16_t *)buffer;
            temp16Data = 0;

            // Copy wav data to fft input array
            for(counter = 0; counter < complexBuffSize; counter++)
            {
                if(counter % 2 == 0)
                {
                    temp16Data = (int16_t)*temp16;
                    fftData[counter] = (float32_t)temp16Data;
                    temp16++;
                }
                else
                {
                    fftData[counter] = 0.0;
                }
            }

            // Set instance for Real FFT
            status = arm_cfft_radix2_init_f32(&fft, fftSize, ifftFlag, doBitReverse);

            // Perform Real FFT on fftData
            arm_cfft_radix2_f32(&fft, fftData);

            // Populate FFT bins
            arm_cmplx_mag_f32(fftData, fftResult, fftSize);

            // Zero out non-audible, low-frequency noise from FFT Results.
            fftResult[0] = 0.0;

            // Find max bin and location of max (first half of bins as this is the only valid section)
            arm_max_f32(fftResult, fftSize, &maxValue, &testIndex);

            break;

        case 32:
            temp32 = (uint32_t *)buffer;
            temp32Data = 0;

            // Copy wav data to fft input array
            for(counter = 0; counter < complexBuffSize; counter++)
            {
                if(counter % 2 == 0)
                {
                    temp32Data = (int32_t)*temp32;
                    fftData[counter] = (float32_t)temp32Data;
                    temp32++;
                }
                else
                {
                    fftData[counter] = 0.0;
                }
            }

            // Set instance for Real FFT
            status = arm_cfft_radix2_init_f32(&fft, fftSize, ifftFlag, doBitReverse);

            // Perform Real FFT on fftData
            arm_cfft_radix2_f32(&fft, fftData);

            // Populate FFT bins
            arm_cmplx_mag_f32(fftData, fftResult, fftSize);

            // Zero out non-audible, low-frequency noise from FFT Results.
            fftResult[0] = 0.0;

            // Find max bin and location of max (first half of bins as this is the only valid section)
            arm_max_f32(fftResult, fftSize, &maxValue, &testIndex);

            break;

        default:
            __asm("NOP");
            break;
    }

    if(status != ARM_MATH_SUCCESS)
    {
        wavFreqHz = 0;  // If an error has occured set frequency of wav data to 0Hz
        PRINTF("\r\nFFT compuation error.\r\n");
    }
    else
    {
        // Set wavFreqHz to bin location of max amplitude multiplied by the hz per bin
        wavFreqHz = testIndex * hzPerBin;
    }

    return wavFreqHz;

}
