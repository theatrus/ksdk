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

// SDK Included Files
#include "fsl_dac_driver.h"
#include "adc_hw_trigger.h"
#include "fsl_misc_utilities.h"
#include "fsl_hwtimer.h"


///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define DAC_DATA_BUF_SIZE 16U

// DAC buffer output data to create sine wave
static const uint16_t dacBuf[DAC_DATA_BUF_SIZE] = 
{
       0U,   39U,  156U,  345U,
     600U,  910U, 1264U, 1648U,
    2048U, 2448U, 2832U, 3186U,
    3496U, 3751U, 3940U, 4056U
};

///////////////////////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////////////////////

extern const hwtimer_devif_t kSystickDevif;
static hwtimer_t hwtimer;
static dac_buffer_config_t dacBuffConfig;

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////
void DAC_DMO_BuffUpdate(void);

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

static void hwtimer_callback(void* data)
{
#if (!FSL_FEATURE_DAC_HAS_BUFFER_SWING_MODE || (FSL_FEATURE_DAC_BUFFER_SIZE != DAC_DATA_BUF_SIZE))
    DAC_DMO_BuffUpdate();
#endif
    DAC_DRV_SoftTriggerBuffCmd(DAC_INST);    
}

void DAC_DMO_BuffUpdate(void)
{
    // initialize the dacBuffReadIndex
    static uint8_t dacBuffReadIndex = 0U;
    // initialize the upFlag which represents whether the index to dacBuf is increment or decrement
    static bool upFlag = true;    
    uint8_t i;
    
    //fill the data to the HW buffer from zero up to the upper member
    if ( DAC_DRV_GetBuffFlag(DAC_INST, kDacBuffIndexUpperFlag)) {       
        for (i = 0; i < (dacBuffConfig.upperIdx); i++) {
            // check the dacBuffReadIndex range and update upFlag
            if (dacBuffReadIndex >= (DAC_DATA_BUF_SIZE - 1U)) {
                upFlag = false;
            }
            if (dacBuffReadIndex <= 0U) {
                upFlag = true;
            }
            
            DAC_HAL_SetBuffValue(g_dacBase[DAC_INST], i, dacBuf[dacBuffReadIndex]);
            
            if (upFlag) {
               dacBuffReadIndex++;
            } else {
               dacBuffReadIndex--;
            }
        }
        DAC_DRV_ClearBuffFlag(DAC_INST, kDacBuffIndexUpperFlag);
    }
    //fill just the upper (last) member of the HW buffer when the readpointer is on start of the buffer
    if ( DAC_DRV_GetBuffFlag(DAC_INST, kDacBuffIndexStartFlag)) {              
            // check the dacBuffReadIndex range and update upFlag
            if (dacBuffReadIndex >= (DAC_DATA_BUF_SIZE - 1U)) {
                upFlag = false;
            }
            if (dacBuffReadIndex <= 0U) {
                upFlag = true;
            }
            
            DAC_HAL_SetBuffValue(g_dacBase[DAC_INST], dacBuffConfig.upperIdx, dacBuf[dacBuffReadIndex]);
            
            if (upFlag) {
               dacBuffReadIndex++;
            } else {
               dacBuffReadIndex--;
            }
            
            DAC_DRV_ClearBuffFlag(DAC_INST, kDacBuffIndexStartFlag);
    }    
}

/*!
 * @brief Use DAC fifo to generate sine wave on DACx_OUT
 */
int32_t dac_gen_wave(void)
{
    dac_converter_config_t dacUserConfig;
    uint32_t period;

    // Fill the structure with configuration of software trigger
    DAC_DRV_StructInitUserConfigNormal(&dacUserConfig);
    
    // Initialize the DAC Converter
    DAC_DRV_Init(DAC_INST, &dacUserConfig);

    // Enable the feature of DAC internal buffer
    dacBuffConfig.bufferEnable = true;
    
#if FSL_FEATURE_DAC_HAS_WATERMARK_SELECTION
    dacBuffConfig.idxWatermarkIntEnable = false;
    dacBuffConfig.watermarkMode = kDacBuffWatermarkFromUpperAs2Word;
#endif
    dacBuffConfig.triggerMode = kDacTriggerBySoftware;
    dacBuffConfig.dmaEnable = false;
    dacBuffConfig.idxStartIntEnable = false;
    dacBuffConfig.idxUpperIntEnable = false;
    dacBuffConfig.upperIdx = FSL_FEATURE_DAC_BUFFER_SIZE - 1U;
#if (FSL_FEATURE_DAC_HAS_BUFFER_SWING_MODE && (FSL_FEATURE_DAC_BUFFER_SIZE == DAC_DATA_BUF_SIZE))
    dacBuffConfig.buffWorkMode = kDacBuffWorkAsSwingMode;    
#else
    dacBuffConfig.buffWorkMode = kDacBuffWorkAsNormalMode;
#endif

    DAC_DRV_ConfigBuffer(DAC_INST, &dacBuffConfig);

#if (FSL_FEATURE_DAC_HAS_BUFFER_SWING_MODE && (FSL_FEATURE_DAC_BUFFER_SIZE == DAC_DATA_BUF_SIZE))
    // Fill the buffer with setting data, applicable only if the data and buffer lenght are equal
    DAC_DRV_SetBuffValue(DAC_INST, 0U, FSL_FEATURE_DAC_BUFFER_SIZE, (uint16_t *)dacBuf);
#endif
    
    // Use HW timer of systick to do SW trigger of DAC,
    if (kHwtimerSuccess != HWTIMER_SYS_Init(&hwtimer, &kSystickDevif, 0, NULL))
    {
        return -1;
    }

    // Get the period the systick triggered.
    // There's 30 times of systick interrupt for one period of sine wave,
    // as we only have 16 data depth buffer for DAC, so we need to trigger
    // 30 times of interrupt to do software trigger.

    period = INPUT_SIGNAL_FREQ * (2 * DAC_DATA_BUF_SIZE - 2);
    if (kHwtimerSuccess != HWTIMER_SYS_SetPeriod(&hwtimer, 1000000/period))
    {
        return -1;
    }

    // install call back for SW trigger for DAC
    if (kHwtimerSuccess != HWTIMER_SYS_RegisterCallback(&hwtimer, hwtimer_callback, 0))
    {
        return -1;
    }

    // start systick timer
    if (kHwtimerSuccess != HWTIMER_SYS_Start(&hwtimer))
    {
        return -1;
    }

    return 0;
}

void dac_stop_wave(void)
{
    // Disable the systick timer
    HWTIMER_SYS_Stop(&hwtimer);

    // De-initialize the DAC converter
    DAC_DRV_Deinit(0);
}
