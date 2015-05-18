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
#include "fsl_pmc_hal.h"
#include "fsl_adc16_driver.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief These values are used to get the temperature. DO NOT MODIFY
 * The method used in this demo to calculate temperature of chip is mapped to
 * Temperature Sensor for the HCS08 Microcontroller Family document (Document Number: AN3031)
 */
#define ADCR_VDD                (65535U)    // Maximum value when use 16b resolution
#define V_BG                    (1000U)     // BANDGAP voltage in mV (trim to 1.0V)
#define V_TEMP25                (716U)      // Typical converted value at 25 oC in mV
#define M                       (1620U)     // Typical slope:uV/oC
#define STANDARD_TEMP           (25)

#define ADC16_INSTANCE                (0)   // ADC instacne
#define ADC16_TEMPERATURE_CHN         (kAdc16Chn26) // Temperature Sensor Channel
#define ADC16_BANDGAP_CHN             (kAdc16Chn27) // ADC channel of BANDGAP
#define ADC16_CHN_GROUP               (0)   // ADC group configuration selection

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////
void ADC16_Measure(void);
void calibrateParams(void);
int32_t get_current_temperature(uint32_t adcValue);

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
uint32_t adcValue = 0;               // ADC value
uint32_t adcrTemp25 = 0;             // Calibrated ADCR_TEMP25
uint32_t adcr100m = 0;               // calibrated conversion value of 100mV
adc16_converter_config_t adcUserConfig;   // structure for user config
///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Measures internal temperature of chip.
 *
 * This function used the input of user as trigger to start the measurement.
 * When user press any key, the conversion will begin, then print
 * converted value and current temperature of the chip.
 */
int main(void)
{
    // Init hardware
    hardware_init();

    // Initialization ADC for
    // 12bit resolution.
    // interrupt mode and hw trigger disabled,
    // normal convert speed, VREFH/L as reference,
    // disable continuous convert mode.
    ADC16_DRV_StructInitUserConfigDefault(&adcUserConfig);
    // Use 16bit resolution if enable.
#if (FSL_FEATURE_ADC16_MAX_RESOLUTION >= 16)
    adcUserConfig.resolution = kAdc16ResolutionBitOf16;
#endif

#if (  defined(FRDM_KL43Z)   /* CPU_MKL43Z256VLH4 */ \
    || defined(TWR_KL43Z48M) /* CPU_MKL43Z256VLH4 */ \
    || defined(FRDM_KL27Z)   /* CPU_MKL27Z64VLH4  */ \
    )
    adcUserConfig.refVoltSrc = kAdc16RefVoltSrcOfValt;
#endif
    ADC16_DRV_Init(ADC16_INSTANCE, &adcUserConfig);
    // Calibrate VDD and ADCR_TEMP25
    calibrateParams();

    while(1)
    {
        // Wait for user input before starting measurement.
        PRINTF("\r\n press any key to start measure temperature ");
        GETCHAR();

        // Measure temperature and print converted values.
        ADC16_Measure();
    }
}

/*!
 * @brief Parameters calibration: VDD and ADCR_TEMP25
 *
 * This function used BANDGAP as reference voltage to measure vdd and
 * calibrate V_TEMP25 with that vdd value.
 */
void calibrateParams(void)
{
    adc16_chn_config_t adcChnConfig;
#if FSL_FEATURE_ADC16_HAS_HW_AVERAGE
    adc16_hw_average_config_t userHwAverageConfig;
#endif
    pmc_bandgap_buffer_config_t pmcBandgapConfig = {
        .enable = true,
#if FSL_FEATURE_PMC_HAS_BGEN
        .enableInLowPower = false,
#endif
#if FSL_FEATURE_PMC_HAS_BGBDS
        .drive = kPmcBandgapBufferDriveLow,
#endif
    };

    uint32_t bandgapValue = 0;  // ADC value of BANDGAP
    uint32_t vdd = 0;           // VDD in mV

#if FSL_FEATURE_ADC16_HAS_CALIBRATION
    // Auto calibration
    adc16_calibration_param_t adcCalibraitionParam;
    ADC16_DRV_GetAutoCalibrationParam(ADC16_INSTANCE, &adcCalibraitionParam);
    ADC16_DRV_SetCalibrationParam(ADC16_INSTANCE, &adcCalibraitionParam);
#endif // FSL_FEATURE_ADC16_HAS_CALIBRATION.

    // Enable BANDGAP reference voltage
    PMC_HAL_BandgapBufferConfig(PMC_BASE_PTR, &pmcBandgapConfig);

#if FSL_FEATURE_ADC16_HAS_HW_AVERAGE
    // Use hardware average to increase stability of the measurement.
    userHwAverageConfig.hwAverageEnable = true;
    userHwAverageConfig.hwAverageCountMode = kAdc16HwAverageCountOf32;
    ADC16_DRV_ConfigHwAverage(ADC16_INSTANCE, &userHwAverageConfig);
#endif // FSL_FEATURE_ADC16_HAS_HW_AVERAGE

    // Configure the conversion channel
    // differential and interrupt mode disable.
    adcChnConfig.chnIdx                  = (adc16_chn_t)ADC16_BANDGAP_CHN;
#if FSL_FEATURE_ADC16_HAS_DIFF_MODE
    adcChnConfig.diffConvEnable          = false;
#endif
    adcChnConfig.convCompletedIntEnable  = false;
    ADC16_DRV_ConfigConvChn(ADC16_INSTANCE, ADC16_CHN_GROUP, &adcChnConfig);

    // Wait for the conversion to be done
    ADC16_DRV_WaitConvDone(ADC16_INSTANCE, ADC16_CHN_GROUP);

    // Get current ADC BANDGAP value and format it.
    bandgapValue = ADC16_DRV_GetConvValueSigned(ADC16_INSTANCE, ADC16_CHN_GROUP);
    // Calculates bandgapValue in 16bit resolution
    // from 12bit resolution to calibrate.
#if (FSL_FEATURE_ADC16_MAX_RESOLUTION < 16)
    bandgapValue = bandgapValue << 4;
#endif
    // ADC stop conversion
    ADC16_DRV_PauseConv(ADC16_INSTANCE, ADC16_CHN_GROUP);

    // Get VDD value measured in mV
    // VDD = (ADCR_VDD x V_BG) / ADCR_BG
    vdd = ADCR_VDD * V_BG / bandgapValue;
    // Calibrate ADCR_TEMP25
    // ADCR_TEMP25 = ADCR_VDD x V_TEMP25 / VDD
    adcrTemp25 = ADCR_VDD * V_TEMP25 / vdd;
    // Calculate conversion value of 100mV.
    // ADCR_100M = ADCR_VDD x 100 / VDD
    adcr100m = ADCR_VDD*100/ vdd;

    // Disable BANDGAP reference voltage
    pmcBandgapConfig.enable = false;
    PMC_HAL_BandgapBufferConfig(PMC_BASE_PTR, &pmcBandgapConfig);
}

/*!
 * @brief Calculates the current temperature
 *
 * This funcion calculate temperatue used calibrated value as formula in reference manual.
 *
 * @param ADC convered value of temperature.
 * @return current temperature in oC.
 */
int32_t get_current_temperature(uint32_t adcValue)
{
    int32_t currentTemp = 0;
    // Multiplied by 1000 because M in uM/oC
    // Temperature = 25 - (ADCR_T - ADCR_TEMP25) * 100*1000 / ADCR_100M*M
    currentTemp = (int32_t)(STANDARD_TEMP - ((int32_t)adcValue - (int32_t)adcrTemp25) * 100000 /(int32_t)(adcr100m*M));

    return currentTemp;
}
/*!
 * @brief Gets current temperature of chip.
 *
 * This function gets convertion value, converted temperature and print them to terminal.
 */
void ADC16_Measure(void)
{
    adc16_chn_config_t chnConfig;

    // Configure the conversion channel
    // differential and interrupt mode disable.
    chnConfig.chnIdx     = (adc16_chn_t)ADC16_TEMPERATURE_CHN;
#if FSL_FEATURE_ADC16_HAS_DIFF_MODE
    chnConfig.diffConvEnable = false;
#endif
    chnConfig.convCompletedIntEnable  = false;

    // Software trigger the conversion.
    ADC16_DRV_ConfigConvChn(ADC16_INSTANCE, ADC16_CHN_GROUP, &chnConfig);

    // Wait for the conversion to be done.
    ADC16_DRV_WaitConvDone(ADC16_INSTANCE, ADC16_CHN_GROUP);

    // Fetch the conversion value.
    adcValue = ADC16_DRV_GetConvValueSigned(ADC16_INSTANCE, ADC16_CHN_GROUP);

    // Show the current temperature value.
    PRINTF("\r\n ADC converted value: %ld\t", adcValue );
    // Calculates adcValue in 16bit resolution
    // from 12bit resolution in order to convert to temperature.
#if (FSL_FEATURE_ADC16_MAX_RESOLUTION < 16)
    adcValue = adcValue << 4;
#endif
    PRINTF("\r\n Temperature %ld", get_current_temperature(adcValue));

    // Pause the conversion.
    ADC16_DRV_PauseConv(ADC16_INSTANCE, ADC16_CHN_GROUP);

}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////

