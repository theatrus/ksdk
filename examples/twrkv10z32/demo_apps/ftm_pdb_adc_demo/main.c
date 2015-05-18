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

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "fsl_clock_manager.h"
#include "fsl_interrupt_manager.h"

#include "fsl_adc16_hal.h"
#include "fsl_pdb_hal.h"
#include "fsl_ftm_hal.h"

#include "board.h"
#include "fsl_debug_console.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

#define USE_STDIO_FUNCTIONS     // Define this symbol to use STDIO functions
#define ADC_SC1A_POINTER        0U
#define ADC_SC1B_POINTER        1U
#define ADC_CHANNEL_1           1U
#define ADC_CHANNEL_5           5U
#define ADC_CHANNEL_7           7U
#define PDB_MODULO_VALUE        2000U
#define PDB_INT_VALUE           1500U
#define PDB_CH0                 0U
#define PDB_CH1                 1U
#define PDB_PRETRIGGER0         0x0001U
#define PDB_PRETRIGGER1         0x0002U
#define PRETRIGGER_DELAY_VALUE  500U
#define FTM_MODULO_VALUE        4678U
#define DEADTIME_VALUE          0x13U

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

volatile uint16_t u16Result0A[256]={0};
volatile uint16_t u16Result0B[256]={0};
volatile uint16_t u16Result1A[256]={0};
volatile uint16_t u16Result1B[256]={0};
volatile uint16_t u16CycleTimes=0;

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

void ADC0_IRQHandler(void);
void ADC1_IRQHandler(void);
uint8_t ADC_Calibration(ADC_Type* baseAddr);

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

int main(void)
{
    // Buffer used to hold the string to be transmitted */
    char sourceBuff[43] = {"\r\nRun pdb trig adc with flextimer demo.\n\n\r"};
    uint32_t i;
    uint16_t cnvvalue[6];
    adc16_converter_config_t adcUserConfig;
    adc16_chn_config_t adcChnConfig;
    pdb_timer_config_t pdbUserConfig;
    // Initialize standard SDK demo application pins
    hardware_init();

    // Print the initial banner
    PRINTF(sourceBuff);
    // Enable [OUTDIV5] bit in SIM_CLKDIV1 register, set the divider to 4, clock source is 75/4 =18.75MHz
    CLOCK_HAL_SetOutDiv5ENCmd(SIM_BASE_PTR, true);
    CLOCK_HAL_SetOutDiv5(SIM_BASE_PTR, 3);

    // Configure ADC0
    // Enable ADC0 clock gate
    CLOCK_SYS_EnableAdcClock(ADC0_IDX);
    // Calibrate ADC0
    ADC_Calibration(ADC0_BASE_PTR);
    // Enable ADC0 IRQ
    INT_SYS_EnableIRQ(ADC0_IRQn );
    // Select alternative clock for ADC0 from outdiv5
    adcUserConfig.clkSrc = kAdc16ClkSrcOfAltClk;
    adcUserConfig.continuousConvEnable = false;
    adcUserConfig.asyncClkEnable = false;
    adcUserConfig.longSampleTimeEnable=false;
    adcUserConfig.clkDividerMode = kAdc16ClkDividerOf1;
#if FSL_FEATURE_ADC16_HAS_DMA
    adcUserConfig.dmaEnable = false;
#endif /* FSL_FEATURE_ADC16_HAS_DMA */
    // Select 12-bit single-end mode
    adcUserConfig.resolution = kAdc16ResolutionBitOf12or13;
    // Conversion trigger selection.
    adcUserConfig.hwTriggerEnable = true;
    // High speed configuration.
    adcUserConfig.highSpeedEnable = true;
    ADC16_HAL_ConfigConverter(ADC0_BASE_PTR, &adcUserConfig);

    // Configure ADC1
    // Enable ADC1 clock gate
    CLOCK_SYS_EnableAdcClock(ADC1_IDX);
    // Calibrate ADC1
    ADC_Calibration(ADC1_BASE_PTR);
    // Enable ADC1 IRQ
    INT_SYS_EnableIRQ(ADC1_IRQn);
    // Conversion resolution mode.
    // Select alternative clock for ADC0 from outdiv5
    adcUserConfig.clkSrc = kAdc16ClkSrcOfAltClk;
    adcUserConfig.continuousConvEnable = false;
    adcUserConfig.asyncClkEnable = false;
    adcUserConfig.longSampleTimeEnable=false;
    adcUserConfig.clkDividerMode = kAdc16ClkDividerOf1;
    // Select 12-bit single-end mode
    adcUserConfig.resolution = kAdc16ResolutionBitOf12or13;
    // Conversion trigger selection.
    adcUserConfig.hwTriggerEnable = true;
    // High speed configuration.
    adcUserConfig.highSpeedEnable = true;
    ADC16_HAL_ConfigConverter(ADC1, &adcUserConfig);
    adcChnConfig.chnIdx = (adc16_chn_t)ADC_CHANNEL_1;
#if FSL_FEATURE_ADC16_HAS_DIFF_MODE
    adcChnConfig.diffConvEnable = false;
#endif
    adcChnConfig.convCompletedIntEnable = true;
    // Write adc0 channel 1 to SC1A register and enable interrupt
    ADC16_HAL_ConfigChn(ADC0_BASE_PTR, ADC_SC1A_POINTER, &adcChnConfig);
    // Write adc0 channel 5 to SC1A register and enable interrupt
    adcChnConfig.chnIdx = (adc16_chn_t)ADC_CHANNEL_5;
    ADC16_HAL_ConfigChn(ADC0_BASE_PTR, ADC_SC1B_POINTER, &adcChnConfig);
    // Write adc1 channel 1 to SC1A register and enable interrupt
    adcChnConfig.chnIdx = (adc16_chn_t)ADC_CHANNEL_1;
    ADC16_HAL_ConfigChn(ADC1_BASE_PTR, ADC_SC1A_POINTER, &adcChnConfig);
    // Write adc1 channel 7 to SC1A register and enable interrupt
    adcChnConfig.chnIdx = (adc16_chn_t)ADC_CHANNEL_7;
    ADC16_HAL_ConfigChn(ADC1_BASE_PTR, ADC_SC1B_POINTER, &adcChnConfig);

    // Configure PDB module
    // Enable PDB clock gate
    CLOCK_SYS_EnablePdbClock(PDB0_IDX);
    // Clear PDB register
    PDB_HAL_Init(PDB0_BASE_PTR);
    // Enable PDB
    PDB_HAL_Enable(PDB0_BASE_PTR);
    // Set load register mode
    pdbUserConfig.loadValueMode = kPdbLoadValueImmediately;
    // Select PDB external trigger source, it is from FTM0
    pdbUserConfig.triggerInput = kPdbTrigger8;
    pdbUserConfig.continuousModeEnable = false;
    pdbUserConfig.dmaEnable= false;
    pdbUserConfig.intEnable= false;
    pdbUserConfig.seqErrIntEnable= false;
    pdbUserConfig.clkPreMultFactor = kPdbClkPreMultFactorAs1;
    pdbUserConfig.clkPreDiv = kPdbClkPreDivBy1;
    PDB_HAL_ConfigTimer(PDB0_BASE_PTR,&pdbUserConfig);
    // Select PDB modulo value
    PDB_HAL_SetTimerModulusValue(PDB0_BASE_PTR, PDB_MODULO_VALUE);
    // Write int value
    PDB_HAL_SetValueForTimerInterrupt(PDB0_BASE_PTR, PDB_INT_VALUE);
    // Use PDB back-to-back mode, the channel 0 pre-trigger 0 is from delay counter flag, others trigger is from ADC acknowledge
    // Set PDB channel 0, pre-trigger 0 delay counter to 200
    PDB_HAL_SetAdcPreTriggerDelayValue(PDB0_BASE_PTR, PDB_CH0, PDB_PRETRIGGER0, PRETRIGGER_DELAY_VALUE);
    // Enable pre-trigger
    PDB_HAL_SetAdcPreTriggerEnable(PDB0_BASE_PTR, PDB_CH0, PDB_PRETRIGGER0, true);
    PDB_HAL_SetAdcPreTriggerEnable(PDB0_BASE_PTR, PDB_CH0, PDB_PRETRIGGER1, true);
    PDB_HAL_SetAdcPreTriggerEnable(PDB0_BASE_PTR, PDB_CH1, PDB_PRETRIGGER0, true);
    PDB_HAL_SetAdcPreTriggerEnable(PDB0_BASE_PTR, PDB_CH1, PDB_PRETRIGGER1, true);
    // Enable pre-trigger output
    PDB_HAL_SetAdcPreTriggerOutputEnable(PDB0_BASE_PTR, PDB_CH0, PDB_PRETRIGGER0, true);
    PDB_HAL_SetAdcPreTriggerOutputEnable(PDB0_BASE_PTR, PDB_CH0, PDB_PRETRIGGER1, true);
    PDB_HAL_SetAdcPreTriggerOutputEnable(PDB0_BASE_PTR, PDB_CH1, PDB_PRETRIGGER0, true);
    PDB_HAL_SetAdcPreTriggerOutputEnable(PDB0_BASE_PTR, PDB_CH1, PDB_PRETRIGGER1, true);
    // Set back-to-back mode
    // Enable channel 0 pre-trigger 1 to back-to-back mode
    PDB_HAL_SetAdcPreTriggerBackToBackEnable(PDB0_BASE_PTR, PDB_CH0, PDB_PRETRIGGER1, true);
    // Enable channel 1 pre-trigger 0 to back-to-back mode
    PDB_HAL_SetAdcPreTriggerBackToBackEnable(PDB0_BASE_PTR, PDB_CH1, PDB_PRETRIGGER0, true);
    // Enable channel 1 pre-trigger 1 to back-to-back mode
    PDB_HAL_SetAdcPreTriggerBackToBackEnable(PDB0_BASE_PTR, PDB_CH1, PDB_PRETRIGGER1, true);
    // load PDB register
    PDB_HAL_SetLoadValuesCmd(PDB0_BASE_PTR);

    // Configure FTM module
    // Configure FTM0 as complementary combine mode, the frequency is 16KHz, and insert the dead time is 1us
    // Enable FTM0 channel 0 trigger as FTM0 external trigger source for PDB
    // Enable FTM clock gate
    CLOCK_SYS_EnableFtmClock(FTM0_IDX);
    FTM_HAL_Enable(FTM0_BASE_PTR, true);
    // Clear CPWMS bit
    FTM_HAL_SetCpwms(FTM0_BASE_PTR, false);
    // Write FTM0 channel register
    for(i=0; i<FSL_FEATURE_FTM_CHANNEL_COUNT; i+=2)
    {
        cnvvalue[i] = (1000 + i*100);
    }
    for(i=1; i<FSL_FEATURE_FTM_CHANNEL_COUNT; i+=2)
    {
        cnvvalue[i] = ((1000 + (i-1)*100) + (FTM_MODULO_VALUE + 1)*50/100);
        if(cnvvalue[i] > FTM_MODULO_VALUE)
        {
            cnvvalue[i] = (FTM_MODULO_VALUE - 1);
        }
    }
    for(i=0; i<FSL_FEATURE_FTM_CHANNEL_COUNT; i++)
    {
        FTM_HAL_SetChnMSnBAMode(FTM0_BASE_PTR, i, 2);
        FTM_HAL_SetChnEdgeLevel(FTM0_BASE_PTR, i, 1);
        FTM_HAL_SetChnCountVal(FTM0_BASE_PTR, i, cnvvalue[i]);
    }
    // Configure FTM0 channel ouput period is 16KHz complementary waveform (channel n and n+1)
    // Insert the deadtime is 1us
    // Modulo value is (75MHz/16KHz -1)
    FTM_HAL_SetMod(FTM0_BASE_PTR, FTM_MODULO_VALUE);
    FTM_HAL_SetWriteProtectionCmd(FTM0_BASE_PTR, false);
    // Configure combine mode
    for(i=0; i<FSL_FEATURE_FTM_CHANNEL_COUNT/2; i+=2)
    {
        FTM_HAL_SetDualChnCombineCmd(FTM0_BASE_PTR, i, true);
        FTM_HAL_SetDualChnCompCmd(FTM0_BASE_PTR, i, true);
        FTM_HAL_SetDualChnDeadtimeCmd(FTM0_BASE_PTR, i, true);
        FTM_HAL_SetDualChnPwmSyncCmd(FTM0_BASE_PTR, i, true);
    }
    // Set deadtime to 1us
    FTM_HAL_SetDeadtimePrescale(FTM0_BASE_PTR, kFtmDivided4);
    FTM_HAL_SetDeadtimeCount(FTM0_BASE_PTR, DEADTIME_VALUE);
    FTM_HAL_SetSoftwareTriggerCmd(FTM0_BASE_PTR, true);
    for(i=1; i<FSL_FEATURE_FTM_CHANNEL_COUNT; i+=2)
    {
        // Enable channel select in PWMLOAD register
        FTM_HAL_SetPwmLoadChnSelCmd(FTM0_BASE_PTR, i, true);
    }
    // Load FTM registers like MOD, CnV and INT
    FTM_HAL_SetPwmLoadCmd(FTM0_BASE_PTR, true);
    // Set PDB clock divide to 1
    FTM_HAL_SetClockPs(FTM0_BASE_PTR, kFtmDividedBy1);
    // Set PDB clock source, use system clock
    FTM_HAL_SetClockSource(FTM0_BASE_PTR, kClock_source_FTM_SystemClk);

    while(1)
    {
        // Input any character to start demo
        PRINTF("\r\nInput any character to start demo.\n\n\r");
        GETCHAR();
        // Reset counter
        FTM_HAL_SetCounter(FTM0_BASE_PTR, 0);
        // Enable FTM0 channel0 trigger as external trigger
        FTM_HAL_SetChnTriggerCmd(FTM0_BASE_PTR, CHAN0_IDX, true);
        // Wait data buffer is full
        while(u16CycleTimes<255);
        // Disable FTM0 channel0 trigger
        FTM_HAL_SetChnTriggerCmd(FTM0_BASE_PTR, CHAN0_IDX, false);
        // Clear FTM0 channel external trigger flag, it will not trig PDB
        if(FTM_HAL_IsChnTriggerGenerated(FTM0_BASE_PTR))
        {
            FTM_BWR_EXTTRIG_TRIGF(FTM0_BASE_PTR, 0);
        }
        // Ouput ADC conversion result
        for(i=0;i<256;i++)
        {
            PRINTF("\r\n%d, %d, %d, %d\n\r",u16Result0A[i],u16Result0B[i],u16Result1A[i],u16Result1B[i]);
        }
        // Clear data buffer
        for(i=0;i<256;i++)
        {
            u16Result0A[i]=0;
            u16Result0B[i]=0;
            u16Result1A[i]=0;
            u16Result1B[i]=0;
        }

        u16CycleTimes=0;
    }
}

/*!
 * @brief ADC0 interrupt service routine, read data from result register
 */
void ADC0_IRQHandler(void)
{
    if(ADC16_HAL_GetChnConvCompletedFlag(ADC0_BASE_PTR, ADC_SC1A_POINTER))
    {
        u16Result0A[u16CycleTimes] = ADC16_HAL_GetChnConvValue(ADC0_BASE_PTR, ADC_SC1A_POINTER);
    }
    if(ADC16_HAL_GetChnConvCompletedFlag(ADC0_BASE_PTR, ADC_SC1B_POINTER))
    {
        u16Result0B[u16CycleTimes] = ADC16_HAL_GetChnConvValue(ADC0_BASE_PTR, ADC_SC1B_POINTER);
    }
}

/*!
 * @brief ADC1 interrupt service routine, read data from result register
 */
void ADC1_IRQHandler(void)
{
    if(ADC16_HAL_GetChnConvCompletedFlag(ADC1_BASE_PTR, ADC_SC1A_POINTER))
    {
        u16Result1A[u16CycleTimes] = ADC16_HAL_GetChnConvValue(ADC1_BASE_PTR, ADC_SC1A_POINTER);
    }

    if(ADC16_HAL_GetChnConvCompletedFlag(ADC1_BASE_PTR, ADC_SC1B_POINTER))
    {
        u16Result1B[u16CycleTimes] = ADC16_HAL_GetChnConvValue(ADC1_BASE_PTR, ADC_SC1B_POINTER);

        if(u16CycleTimes<255)
        {
            u16CycleTimes++;
        }
        else
        {
            u16CycleTimes=0;
        }
    }
}

/*!
 * @brief Perform ADC calibration function
 */
uint8_t ADC_Calibration(ADC_Type* baseAddr)
{
    adc16_converter_config_t adcUserConfig;
#if FSL_FEATURE_ADC_HAS_OFFSET_CORRECTION
    uint32_t offsetValue;
#endif
    uint32_t plusSideGainValue;
#if FSL_FEATURE_ADC_HAS_DIFF_MODE
    uint32_t minusSideGainValue;
#endif
    // Special configuration for highest accuracy.
    // Enable clock for ADC.
    if(baseAddr == ADC0_BASE_PTR)
    {
        CLOCK_SYS_EnableAdcClock(ADC0_IDX);
    }
    else
    {
        CLOCK_SYS_EnableAdcClock(ADC1_IDX);
    }
    ADC16_HAL_Init(baseAddr);
    // Maximum Average.
#if FSL_FEATURE_ADC_HAS_HW_AVERAGE
    adcHwAverage.hwAverageEnable = true;
    adcHwAverage.hwAverageCountMode = kAdc16HwAverageCountOf32;
    ADC16_DRV_ConfigHwAverage(instance, &adcHwAverage);
#endif // FSL_FEATURE_ADC_HAS_HW_AVERAGE

    // Lowest ADC Frequency.
    adcUserConfig.clkSrc = kAdc16ClkSrcOfBusClk;
    adcUserConfig.clkDividerMode = kAdc16ClkDividerOf8;
    // Reference voltage as Vadd.
    adcUserConfig.refVoltSrc = kAdc16RefVoltSrcOfVref;
    // Software trigger.
    adcUserConfig.hwTriggerEnable = false;
    ADC16_HAL_ConfigConverter(baseAddr,&adcUserConfig);
    // Launch auto calibration.
    ADC16_HAL_SetAutoCalibrationCmd(baseAddr, true);
    while (!ADC16_HAL_GetChnConvCompletedFlag(baseAddr, ADC_SC1A_POINTER))
    {
        if (ADC16_HAL_GetAutoCalibrationFailedFlag(baseAddr))
        {
            return (uint8_t)false;
        }
    }
    // Read parameters that generated by auto calibration.
#if FSL_FEATURE_ADC_HAS_OFFSET_CORRECTION
    offsetValue = ADC16_HAL_GetOffsetValue(baseAddr);
#endif // FSL_FEATURE_ADC_HAS_OFFSET_CORRECTION
    plusSideGainValue = ADC16_HAL_GetAutoPlusSideGainValue(baseAddr);
#if FSL_FEATURE_ADC_HAS_DIFF_MODE
    minusSideGainValue = ADC16_HAL_GetAutoMinusSideGainValue(baseAddr);
#endif // FSL_FEATURE_ADC_HAS_DIFF_MODE

    ADC16_HAL_Init(baseAddr);

#if FSL_FEATURE_ADC_HAS_OFFSET_CORRECTION
    ADC16_HAL_SetOffsetValue(baseAddr, offsetValue);
#endif // FSL_FEATURE_ADC_HAS_OFFSET_CORRECTION
    ADC16_HAL_SetPlusSideGainValue(baseAddr, plusSideGainValue);
#if FSL_FEATURE_ADC_HAS_DIFF_MODE
    ADC16_HAL_SetMinusSideGainValue(baseAddr, minusSideGainValue);
#endif // FSL_FEATURE_ADC_HAS_DIFF_MODE

    return (uint8_t)true;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
