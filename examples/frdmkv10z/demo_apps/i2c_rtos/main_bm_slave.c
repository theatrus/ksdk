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

// Standard C Included Files
#include <stdio.h>
// SDK Included Files
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_pmc_hal.h"
#include "fsl_debug_console.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_i2c_slave_driver.h"
#include "fsl_smc_hal.h"
#include "i2c_rtos.h"
#include "fsl_adc16_driver.h"

extern uint32_t gSlaveId;

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
///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////
void calibrateParams(void);
extern void task_slave(void *param);

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
static uint32_t adcrTemp25 = 0;             // Calibrated ADCR_TEMP25
static uint32_t adcr100m = 0;               // calibrated conversion value of 100mV
static uint32_t temperature;

adc16_converter_config_t tempSnseAdcConfig;

// ADC temperature channel configuration
adc16_chn_config_t tempSnseChannelConfig =
{
    .chnIdx = kAdc16Chn26,
#if FSL_FEATURE_ADC16_HAS_DIFF_MODE
    .diffConvEnable = false,
#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */
    .convCompletedIntEnable = false
};

// task define
OSA_TASK_DEFINE(task_slave, TASK_SLAVE_STACK_SIZE);

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Parameters calibration: VDD and ADCR_TEMP25
 *
 * This function used BANDGAP as reference voltage to measure vdd and
 * calibrate V_TEMP25 with that vdd value.
 */
void calibrateParams(void)
{

    adc16_chn_config_t adcChnConfig;
    pmc_bandgap_buffer_config_t pmcBandgapConfig = {
        .enable = true,
#if FSL_FEATURE_PMC_HAS_BGEN
        .enableInLowPower = false,
#endif
#if FSL_FEATURE_PMC_HAS_BGBDS
        .drive = kPmcBandgapBufferDriveLow,
#endif
    };
    uint32_t bandgapValue;  // ADC value of BANDGAP
    uint32_t vdd;           // VDD in mV

    // Enable BANDGAP reference voltage
    PMC_HAL_BandgapBufferConfig(PMC_BASE_PTR, &pmcBandgapConfig);

    // Configure the conversion channel
    // differential and interrupt mode disable.
    adcChnConfig.chnIdx     = kAdc16Chn27;    // ADC Bandgap channel
#if FSL_FEATURE_ADC16_HAS_DIFF_MODE
    adcChnConfig.diffConvEnable = false;
#endif
    adcChnConfig.convCompletedIntEnable  = false;

    ADC16_DRV_ConfigConvChn(ADC_INSTANCE, 0U, &adcChnConfig);

    // Wait for the conversion to be done
    ADC16_DRV_WaitConvDone(ADC_INSTANCE, 0U);

    // Get current ADC BANDGAP value and format it.
    bandgapValue = ADC16_DRV_GetConvValueSigned(ADC_INSTANCE, 0U);
    // Calculates bandgapValue in 16bit resolution
    // from 12bit resolution to calibrate.
#if (FSL_FEATURE_ADC16_MAX_RESOLUTION < 16)
    bandgapValue = bandgapValue << 4U;
#endif
    // ADC stop conversion
    ADC16_DRV_PauseConv(ADC_INSTANCE, 0U);

    // Get VDD value measured in mV
    // VDD = (ADCR_VDD x V_BG) / ADCR_BG
    vdd = ADCR_VDD * V_BG / bandgapValue;
    // Calibrate ADCR_TEMP25
    // ADCR_TEMP25 = ADCR_VDD x V_TEMP25 / VDD
    adcrTemp25 = ADCR_VDD * V_TEMP25 / vdd;
    // Calculate conversion value of 100mV.
    // ADCR_100M = ADCR_VDD x 100 / VDD
    adcr100m = ADCR_VDD * 100/ vdd;

    // Disable BANDGAP reference voltage
    pmcBandgapConfig.enable = false;
    PMC_HAL_BandgapBufferConfig(PMC_BASE_PTR, &pmcBandgapConfig);
}
/*!
 * Get the temperature pointer
 * designed for BM version of I2C_RTOS demo
 * from the ISR context
 */
uint8_t* get_temp_pointer(void)
{
    uint32_t adcValue;
    // ADC starts conversion
    ADC16_DRV_ConfigConvChn(ADC_INSTANCE, 0U, &tempSnseChannelConfig);
    // poll to complete status and read back result
    ADC16_DRV_WaitConvDone(ADC_INSTANCE, 0U);
    // Get ADC converted value
    adcValue = ADC16_DRV_GetConvValueSigned(ADC_INSTANCE, 0U);
    // ADC stop conversion
    ADC16_DRV_PauseConv(ADC_INSTANCE, 0U);
    // Calculates adcValue in 16bit resolution
    // from 12bit resolution in order to convert to temperature.
#if (FSL_FEATURE_ADC16_MAX_RESOLUTION < 16)
    adcValue = adcValue << 4U;
#endif
    // convert to temperature
    // Multiplied by 1000 because M in uM/oC
    // temperature = 25 - (ADCR_T - ADCR_TEMP25) * 100*1000 / ADCR_100M*M
    temperature = (int32_t)(STANDARD_TEMP - ((int32_t)adcValue - (int32_t)adcrTemp25) * 100000 /(int32_t)(adcr100m*M));

    return (uint8_t*)&temperature;
}

/*!
 * @brief main function
 */
int main(void)
{
#if FSL_FEATURE_ADC16_HAS_CALIBRATION
    adc16_calibration_param_t tempSnseCalibraitionParam;
#endif

    hardware_init();
    GPIO_DRV_Init(NULL, ledPins);

    // Configure the power mode protection
    SMC_HAL_SetProtection(SMC_BASE_PTR, kAllowPowerModeVlp);

    ADC16_DRV_StructInitUserConfigDefault(&tempSnseAdcConfig);
#if (FSL_FEATURE_ADC16_MAX_RESOLUTION >= 16)
    tempSnseAdcConfig.resolution = kAdc16ResolutionBitOf16;
#endif

#if BOARD_ADC_USE_ALT_VREF
    tempSnseAdcConfig.refVoltSrc = kAdc16RefVoltSrcOfValt;
#endif
    // Init ADC
    ADC16_DRV_Init(ADC_INSTANCE, &tempSnseAdcConfig);
    // Calibrate VDD and ADCR_TEMP25
#if FSL_FEATURE_ADC16_HAS_CALIBRATION
    // Auto calibraion
    ADC16_DRV_GetAutoCalibrationParam(ADC_INSTANCE, &tempSnseCalibraitionParam);
    ADC16_DRV_SetCalibrationParam(ADC_INSTANCE, &tempSnseCalibraitionParam);
#endif // FSL_FEATURE_ADC16_HAS_CALIBRATION
    calibrateParams();

    // get cpu uid low value for slave
    gSlaveId = SIM_UIDL_UID(SIM_BASE_PTR);

    PRINTF("i2c_rtos_slave_bm demo\r\n");

    // task list initialize
    OSA_Init();

    // create task(in BM: only the first registered task can be executed)
    OSA_TaskCreate(task_slave,
                (uint8_t *)"slave",
                512,
                task_slave_stack,
                0,
                (void *)0,
                false,
                &task_slave_task_handler);

    OSA_Start();

    return 0;
}
