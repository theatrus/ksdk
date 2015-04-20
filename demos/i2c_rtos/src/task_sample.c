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
//  Includes
///////////////////////////////////////////////////////////////////////////////
// SDK Included Files
#include "fsl_adc16_driver.h"
#include "fsl_interrupt_manager.h"
#include "i2c_rtos.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
// These values are used covert temperature. DO NOT MODIFY
#define VTEMP25_ADC             (14219)
#define K                       (10000)
#define M1                      (250000)
#define M2                      (311)

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
// temperature global variable
static int32_t gTemperature;

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * task to read ADC for internal temperature
 */
void task_sample(task_param_t param)
{
#if FSL_FEATURE_ADC16_HAS_CALIBRATION
    adc16_calibration_param_t tempSnseCalibraitionParam;
#endif
    adc16_user_config_t tempSnseAdcConfig;
    // ADC Channel Configuration
    adc16_chn_config_t tempSnseChannelConfig =
    {
        .chnNum = 26,
        .chnMux = kAdcChnMuxOfA,
        .diffEnable = false,
        .intEnable = false
    };
    uint32_t adcValue;

#if FSL_FEATURE_ADC16_HAS_CALIBRATION
    // Auto calibraion
    ADC16_DRV_GetAutoCalibrationParam(HWADC_INSTANCE, &tempSnseCalibraitionParam);
    ADC16_DRV_SetCalibrationParam(HWADC_INSTANCE, &tempSnseCalibraitionParam);
#endif // FSL_FEATURE_ADC16_HAS_CALIBRATION
    
    ADC16_DRV_StructInitUserConfigDefault(&tempSnseAdcConfig);
    tempSnseAdcConfig.clkSrcMode = kAdcClkSrcOfAsynClk;
    tempSnseAdcConfig.resolutionMode = kAdcResolutionBitOfSingleEndAs16;
    // Initialize ADC
    ADC16_DRV_Init(HWADC_INSTANCE, &tempSnseAdcConfig);

    while (1)
    {
        // ADC starts conversion
        ADC16_DRV_ConfigConvChn(HWADC_INSTANCE, 0U, &tempSnseChannelConfig);
        // poll to complete status and read back result
        ADC16_DRV_WaitConvDone(HWADC_INSTANCE, 0U);
        adcValue = ADC16_DRV_GetConvValueRAW(HWADC_INSTANCE, 0U);
        adcValue = ADC16_DRV_ConvRAWData(adcValue, false, tempSnseAdcConfig.resolutionMode);
        // ADC stop conversion
        ADC16_DRV_PauseConv(HWADC_INSTANCE, 0U);
        // convert to temperature
        INT_SYS_DisableIRQGlobal();
        gTemperature = (M1 - (adcValue - VTEMP25_ADC) * M2)/K;
        INT_SYS_EnableIRQGlobal();
        // sleep 4s
        OSA_TimeDelay(4000);
    }
}

/*!
 * Get temperature pointer
 * from the ISR context
 */
uint8_t* get_temp_pointer(void)
{
    static uint8_t *pTemp = (uint8_t*)&gTemperature;
    return pTemp;
}
