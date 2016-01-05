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

#ifndef ADC16_TEMPERATURE_H
#define ADC16_TEMPERATURE_H
///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////
// Standard C Included Files
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

// SDK Included Files
#include "fsl_adc16_driver.h"
#include "fsl_pit_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_pmc_hal.h"
#include "board.h"

// Project Included Files
#include "lpm_rtos.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define CHANNEL_0               (0U)        /*! ADC channel 0. */
#define INPUT_SIGNAL_FREQ       20U /* in HZ */
#define NR_SAMPLES              100U /*!< number of samples in one period */
/*!
 * @brief These values are used to get the temperature. DO NOT MODIFY
 * The method used in this demo to calculate temperature of chip is mapped to
 * Temperature Sensor for the HCS08 Microcontroller Family document (Document Number: AN3031)
 */
#define ADCR_VDD                (65535U)    /*! Maximum value when use 16b resolution */
#define V_BG                    (1000U)     /*! BANDGAP voltage in mV (trim to 1.0V) */
#define V_TEMP25                (716U)      /*! Typical VTEMP25 in mV */
#define M                       (1620U)     /*! Typical slope: (mV x 1000)/oC */
#define STANDARD_TEMP           (25)

#define ADC16_TEMPERATURE_CHN         (kAdc16Chn26) /* Temperature Sensor Channel.*/
#define ADC16_BANDGAP_CHN             (kAdc16Chn27) /* ADC channel of BANDGAP.*/

///////////////////////////////////////////////////////////////////////////////
// Structures & enumerations
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
extern int32_t gTemperature;               /*! Temperature. */
extern const uint32_t gSimBaseAddr[]; // driver state store
extern volatile uint8_t gConversionCompleteFlag;

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////
 /*!
 * @brief ADC1 IRQ handler.
 */
void adc1IrqHandler(void);

/*!
 * @brief calibrate param for adc.
 */
void adc16CalibrateParams(void);

/*!
 * @brief Getting current temperature value.
 * @return Current temperature.
 */
int32_t adc16GetCurrentTempValue(void);

/*!
 * @brief Initialize the ADCx for HW trigger.
 * @return Error code.
 */
int adc16Init(adc16_converter_config_t *adcUserConfig, adc16_chn_config_t *adcChnConfig, adc16_calibration_param_t *adcCalibraitionParam);

/*!
 * @brief Print temperature got from adc.
 */
void adc16PrintTemperature(void);

/*!
 * @Brief enable the trigger source
 *
 * @param instance The ADC instance number
 */
void adc16InitPitTriggerSource(uint32_t adcInstance);

/*!
 * @Brief disable the trigger source
 */
void adc16DeinitPitTriggerSource(uint32_t adcInstance);


/*!
 * @brief User-defined function to install callbach
 */
void ADC_InstallCallback(uint32_t instance, uint32_t chnGroup, void (*callbackFunc)(void) );
/*!
 * @brief User-defined function to read conversion value in ADC ISR
 */
uint16_t ADC_GetConvValueRAWInt(uint32_t instance, uint32_t chnGroup);

#endif
