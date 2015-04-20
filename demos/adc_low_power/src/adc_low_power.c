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
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

// SDK Included Files
#include "adc_low_power.h"
#include "fsl_smc_hal.h"
#include "fsl_pmc_hal.h"
#include "fsl_adc16_driver.h"
#include "board.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define ADC_0                   (0U)
#define CHANNEL_0               (0U)
#define LED_ON                  (0U)
#define LED_OFF                 (1U)
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

#define UPPER_VALUE_LIMIT       (1U)        /*! This value/10 is going to be added to current Temp to set the upper boundary*/
#define LOWER_VALUE_LIMIT       (1U)        /*! This Value/10 is going to be subtracted from current Temp to set the lower boundary*/
#define UPDATE_BOUNDARIES_TIME  (20U)       /*! This value indicates the number of cycles needed to update boundaries. To know the Time it will take, multiply this value times LPTMR_COMPARE_VALUE*/
#define kAdcChannelTemperature  (26U)       /*! ADC channel of temperature sensor */
#define kAdcChannelBandgap      (27U)       /*! ADC channel of BANDGAP */

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////

extern void init_trigger_source(uint32_t instance);

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

static uint32_t adcValue = 0;               /*! ADC value */
static uint32_t adcrTemp25 = 0;             /*! Calibrated ADCR_TEMP25 */
static uint32_t adcr100m = 0;
volatile bool conversionCompleted = false;  /*! Conversion is completed Flag */
const uint32_t gSimBaseAddr[] = SIM_BASE_ADDRS;
static smc_power_mode_config_t smcConfig;

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/* ADC Interrupt Handler */
void ADC1IRQHandler(void)
{
    // Get current ADC value
    adcValue = ADC_TEST_GetConvValueRAWInt (ADC_0, CHANNEL_0);
    // Set conversionCompleted flag. This prevents an wrong conversion in main function
    conversionCompleted = true;
}

/*!
 * @brief enter in VLPS mode
 */
void config_vlps(void)
{
    smc_power_mode_protection_config_t smcProtConfig;

    // set to allow entering specific modes
    smcProtConfig.vlpProt = true;
    SMC_HAL_SetProtection(SMC_BASE, &smcProtConfig);

    // set power mode to specific Run mode 
#if FSL_FEATURE_SMC_HAS_LPWUI
    smcConfig.lpwuiOption = false;
#endif
#if FSL_FEATURE_SMC_HAS_PORPO
    smcConfig.porOption = false;
#endif
    smcConfig.powerModeName = kPowerModeVlps;
}

/*!
 * Parameters calibration: VDD and ADCR_TEMP25
 */
void calibrateParams(void)
{
#if FSL_FEATURE_ADC16_HAS_CALIBRATION
    adc16_calibration_param_t adcCalibraitionParam;
#endif
    adc16_user_config_t adcUserConfig;
    adc16_chn_config_t adcChnConfig;
    uint32_t bandgapValue = 0;  /*! ADC value of BANDGAP */
    uint32_t vdd = 0;           /*! VDD in mV */

#if FSL_FEATURE_ADC16_HAS_CALIBRATION
    // Auto calibration
    ADC16_DRV_GetAutoCalibrationParam(ADC_0, &adcCalibraitionParam);
    ADC16_DRV_SetCalibrationParam(ADC_0, &adcCalibraitionParam);
#endif

    // Enable BANDGAP reference voltage
    PMC_HAL_SetBandgapBufferCmd(PMC_BASE, true);

    // Initialization ADC for
    // 16bit resolution, interrupt mode, hw trigger disabled.
    // normal convert speed, VREFH/L as reference,
    // disable continuous convert mode.
    ADC16_DRV_StructInitUserConfigDefault(&adcUserConfig);
    adcUserConfig.resolutionMode = kAdcResolutionBitOf16;
    adcUserConfig.continuousConvEnable = false;
    adcUserConfig.clkSrcMode = kAdcClkSrcOfAsynClk;
    ADC16_DRV_Init(ADC_0, &adcUserConfig);

#if FSL_FEATURE_ADC16_HAS_HW_AVERAGE
    ADC16_DRV_EnableHwAverage(ADC_0, kAdcHwAverageCountOf32);
#endif // FSL_FEATURE_ADC16_HAS_HW_AVERAGE

    adcChnConfig.chnNum = kAdcChannelBandgap;
    adcChnConfig.diffEnable = false;
    adcChnConfig.intEnable = false;
    adcChnConfig.chnMux = kAdcChnMuxOfA;
    ADC16_DRV_ConfigConvChn(ADC_0, CHANNEL_0, &adcChnConfig);

    // Wait for the conversion to be done
    ADC16_DRV_WaitConvDone(ADC_0, CHANNEL_0);

    // Get current ADC BANDGAP value
    bandgapValue = ADC16_DRV_GetConvValueRAW(ADC_0, CHANNEL_0);
    bandgapValue = ADC16_DRV_ConvRAWData(bandgapValue, false, adcUserConfig.resolutionMode);

    // ADC stop conversion
    ADC16_DRV_PauseConv(ADC_0, CHANNEL_0);

    // Get VDD value measured in mV: VDD = (ADCR_VDD x V_BG) / ADCR_BG
    vdd = ADCR_VDD * V_BG / bandgapValue;
    // Calibrate ADCR_TEMP25: ADCR_TEMP25 = ADCR_VDD x V_TEMP25 / VDD
    adcrTemp25 = ADCR_VDD * V_TEMP25 / vdd;
    // ADCR_100M = ADCR_VDD x M x 100 / VDD
    adcr100m = (ADCR_VDD * M) / (vdd * 10);

#if FSL_FEATURE_ADC16_HAS_HW_AVERAGE
    ADC16_DRV_DisableHwAverage(ADC_0);
#endif // FSL_FEATURE_ADC16_HAS_HW_AVERAGE

    // Disable BANDGAP reference voltage
    PMC_HAL_SetBandgapBufferCmd(PMC_BASE, false);
}
/*!
 * @brief Initialize the ADCx for HW trigger.
 *
 * @param instance The ADC instance number
 */
static int32_t init_adc(uint32_t instance)
{
#if FSL_FEATURE_ADC16_HAS_CALIBRATION
    adc16_calibration_param_t adcCalibraitionParam;
#endif
    adc16_user_config_t adcUserConfig;
    adc16_chn_config_t adcChnConfig;

#if FSL_FEATURE_ADC16_HAS_CALIBRATION
    // Auto calibration
    ADC16_DRV_GetAutoCalibrationParam(instance, &adcCalibraitionParam);
    ADC16_DRV_SetCalibrationParam(instance, &adcCalibraitionParam);
#endif

    // Initialization ADC for
    // 16bit resolution, interrupt mode, hw trigger enabled.
    // normal convert speed, VREFH/L as reference,
    // disable continuous convert mode.
    ADC16_DRV_StructInitUserConfigDefault(&adcUserConfig);
    adcUserConfig.intEnable = true;
    adcUserConfig.resolutionMode = kAdcResolutionBitOf16;
    adcUserConfig.hwTriggerEnable = true;
    adcUserConfig.continuousConvEnable = false;
    adcUserConfig.clkSrcMode = kAdcClkSrcOfAsynClk;
    ADC16_DRV_Init(instance, &adcUserConfig);

    // Install Callback function into ISR
    ADC_TEST_InstallCallback(instance, CHANNEL_0, ADC1IRQHandler);

    adcChnConfig.chnNum = kAdcChannelTemperature;
    adcChnConfig.diffEnable = false;
    adcChnConfig.intEnable = true;
    adcChnConfig.chnMux = kAdcChnMuxOfA;

    // Configure channel0
    ADC16_DRV_ConfigConvChn(instance, CHANNEL_0, &adcChnConfig);

    return 0;
}

/* Calculate the current temperature */
int32_t GetCurrentTempValue(void)
{
    int32_t currentTemperature = 0;

    // Temperature = 25 - (ADCR_T - ADCR_TEMP25) * 100 / ADCR_100M
    currentTemperature = (int32_t)(STANDARD_TEMP - ((int32_t)adcValue - (int32_t)adcrTemp25) * 100 / (int32_t)adcr100m);

    return currentTemperature;
}

/* Calculate the average temperature and set boundaries */
lowPowerAdcBoundaries_t TempSensorCalibration(uint32_t updateBoundariesCounter,
                                                     int32_t *tempArray)
{
    uint32_t avgTemp = 0;
    lowPowerAdcBoundaries_t boundaries;

    for(int i = 0; i < updateBoundariesCounter; i++)
    {
        avgTemp += tempArray[i];
    }
    // Get average temperature
    avgTemp /= updateBoundariesCounter;

    // Set upper boundary
    boundaries.upperBoundary = avgTemp + UPPER_VALUE_LIMIT;

    // Set lower boundary
    boundaries.lowerBoundary = avgTemp - LOWER_VALUE_LIMIT;

    return boundaries;
}

/*!
 * @brief main function
 */
int main(void)
{
    int32_t currentTemperature = 0;
    uint32_t updateBoundariesCounter = 0;
    int32_t tempArray[UPDATE_BOUNDARIES_TIME * 2];
    lowPowerAdcBoundaries_t boundaries;

    // Init hardware
    hardware_init();

    // Call this function to initialize the console UART.  This function
    // enables the use of STDIO functions (printf, scanf, etc.)
    dbg_uart_init();

    // Initialize GPIO pins
    GPIO_DRV_Init(accelIntPins, ledPins);

    // Configure SMC module to entry to vlps mode
    config_vlps();

    // Calibrate param Temperature sensor
    calibrateParams();

    // Initialize ADC
    if (init_adc(ADC_0))
    {
        printf("Failed to do the ADC init\n");
        return -1;
    }

    // Show the currentTemperature value
    printf("\r\n ADC LOW POWER DEMO \r\n\n\n");
    printf("\r\nThe Low Power ADC project is designed to work with the Tower System or in a stand alone setting. \n\n");
    printf("\r 1. Set your target board in a place where the temperature is constant.  \n");
#ifndef FREEDOM
    printf("\r 2. Wait until the green LED light turns on. \n");
#else
    printf("\r 2. Wait until the white LED light turns on. \n");
#endif
    printf("\r 3. Increment or decrement the temperature to see the changes. \n");

    // setup the HW trigger source
    init_trigger_source(ADC_0);

    // Warm up microcontroller and allow to set first boundaries
    while(updateBoundariesCounter < (UPDATE_BOUNDARIES_TIME * 2))
    {
        while(!conversionCompleted);
        currentTemperature = GetCurrentTempValue();
        tempArray[updateBoundariesCounter] = currentTemperature;
        updateBoundariesCounter++;
        conversionCompleted = false;
    }

    // Temp Sensor Calibration 
    boundaries = TempSensorCalibration(updateBoundariesCounter, tempArray);
    updateBoundariesCounter = 0;

#ifndef FREEDOM
    // Green LED is turned on indicating calibration is done
    GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_GREEN, LED_ON);
#endif
    
    // Wait for user input before beginning demo
    printf("\n\n\rEnter any character to begin...\n\n\r");
    getchar();
    
    ///////////////////////////////////////////////////////////////////////////
    // Main application loop
    //
    // NOTE: UART prints in this loop may not work. 
    // 
    // This demo performs ADC conversions and then moves to a low power mode. 
    // By default, the startup code of this demo enters PEE mode at the 
    // maximum operating frequency of the device. However, after exiting the
    // low power mode, the device will no longer be operating at the maximum
    // operating frequency of the device in PEE mode.  It will be in PBE mode
    // operating at the frequency of the external crystal.  Therefore, UART 
    // prints may not work inside this loop.  
    ///////////////////////////////////////////////////////////////////////////
    for(;;)
    {
        // Prevents the use of wrong values
        while(!conversionCompleted)
        {}

        // Get current Temperature Value
        currentTemperature = GetCurrentTempValue();

        // Store temperature values that are going to be use to calculate average temperature
        tempArray[updateBoundariesCounter] = currentTemperature;

#ifdef FREEDOM
#if !defined(BOARD_GPIO_LED_BLUE)
        // Select which LED is going to be turned on and off
        if(currentTemperature > boundaries.upperBoundary)
        {
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_RED, LED_ON);
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_GREEN, LED_OFF);
        }
        else if(currentTemperature < boundaries.lowerBoundary)
        {
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_RED, LED_OFF);
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_GREEN, LED_ON);
        }
        else
        {
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_RED, LED_OFF);
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_GREEN, LED_OFF);
        }
#else
        // Select which LED is going to be turned on and off
        if(currentTemperature > boundaries.upperBoundary)
        {
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_RED, LED_ON);
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_BLUE, LED_OFF);
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_GREEN, LED_OFF);
        }
        else if(currentTemperature < boundaries.lowerBoundary)
        {
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_RED, LED_OFF);
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_BLUE, LED_ON);
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_GREEN, LED_OFF);
        }
        else
        {
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_RED, LED_ON);
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_BLUE, LED_ON);
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_GREEN, LED_ON);
        }
#endif
#else

        // Show how fast is each cycle
        GPIO_DRV_TogglePinOutput(BOARD_GPIO_LED_GREEN);

        // Select which LED is going to be turned on and off
        if(currentTemperature > boundaries.upperBoundary)
        {
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_RED, LED_ON);
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_BLUE, LED_OFF);
        }
        else if(currentTemperature < boundaries.lowerBoundary)
        {
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_RED, LED_OFF);
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_BLUE, LED_ON);
        }
        else
        {
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_RED, LED_OFF);
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_BLUE, LED_OFF);
        }
#endif

        // Call update function
        if(updateBoundariesCounter >= (UPDATE_BOUNDARIES_TIME))
        {
            boundaries = TempSensorCalibration(updateBoundariesCounter, tempArray);
            updateBoundariesCounter = 0;
        }
        else
        {
            updateBoundariesCounter++;
        }

        // Clear conversionCompleted flag
        conversionCompleted = false;

        // Entry to Low Power Mode
        // Once this mode exited, it will no longer be in PEE mode (assuming
        // the device entered this mode from PEE).  Therefore, the UART 
        // baud rate will not be correct because the device's operating 
        // frequency will be different from the startup of the demo. 
        SMC_HAL_SetMode(SMC_BASE, &smcConfig);
    }
}
