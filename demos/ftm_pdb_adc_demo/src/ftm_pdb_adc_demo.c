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
#define PDB_PRETRIGGER0         0U
#define PDB_PRETRIGGER1         1U
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
uint8_t ADC_Calibration(uint32_t baseAddr);

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

int main(void)
{
    // Buffer used to hold the string to be transmitted */
    char sourceBuff[43] = {"\r\nRun pdb trig adc with flextimer demo.\n\n\r"};
    uint32_t i;
    uint16_t cnvvalue[6];

    // Initialize standard SDK demo application pins
    hardware_init();

    // Call this function to initialize the console UART.
    // This function enables the use of STDIO functions (printf, scanf, etc.)
    dbg_uart_init();

    // Print the initial banner
    printf(sourceBuff);
    // Enable [OUTDIV5] bit in SIM_CLKDIV1 register, set the divider to 4, clock source is 75/4 =18.75MHz
    CLOCK_HAL_SetOutDiv5ENCmd(SIM_BASE, true);
    CLOCK_HAL_SetOutDiv5(SIM_BASE, 3);
    // Configure ADC0
    // Enable ADC0 clock gate
    CLOCK_SYS_EnableAdcClock(HW_ADC0);
    // Calibrate ADC0
    ADC_Calibration(ADC0_BASE);
    // Enable ADC0 IRQ
    INT_SYS_EnableIRQ(ADC0_IRQn );
    // Select 12-bit single-end mode
    ADC16_HAL_SetResolutionMode(ADC0_BASE, kAdcResolutionBitOf12or13);
    // High speed configuration.
    ADC16_HAL_SetHighSpeedCmd(ADC0_BASE, true);
    // Conversion trigger selection.
    ADC16_HAL_SetHwTriggerCmd(ADC0_BASE, true);
    // Select alternative clock for ADC0 from outdiv5
    ADC16_HAL_SetClkSrcMode(ADC0_BASE, kAdcClkSrcOfAltClk);

    // Configure ADC1
    // Enable ADC1 clock gate
    CLOCK_SYS_EnableAdcClock(HW_ADC1);
    // Calibrate ADC1
    ADC_Calibration(ADC1_BASE);
    // Enable ADC1 IRQ
    INT_SYS_EnableIRQ(ADC1_IRQn );
    // Conversion resolution mode.
    // Select 12-bit single-end mode
    ADC16_HAL_SetResolutionMode(ADC1_BASE, kAdcResolutionBitOf12or13);
    // High speed configuration.
    ADC16_HAL_SetHighSpeedCmd(ADC1_BASE, true);
    // Conversion trigger selection.
    ADC16_HAL_SetHwTriggerCmd(ADC1_BASE, true);
    // Select alternative clock for ADC1 from outdiv5
    ADC16_HAL_SetClkSrcMode(ADC1_BASE, kAdcClkSrcOfAltClk);
    // Write adc0 channel 1 to SC1A register and enable interrupt
    ADC16_HAL_ConfigChn(ADC0_BASE, ADC_SC1A_POINTER, true, false, ADC_CHANNEL_1);
    // Write adc0 channel 5 to SC1A register and enable interrupt
    ADC16_HAL_ConfigChn(ADC0_BASE, ADC_SC1B_POINTER, true, false, ADC_CHANNEL_5);
    // Write adc1 channel 1 to SC1A register and enable interrupt
    ADC16_HAL_ConfigChn(ADC1_BASE, ADC_SC1A_POINTER, true, false, ADC_CHANNEL_1);
    // Write adc1 channel 7 to SC1A register and enable interrupt
    ADC16_HAL_ConfigChn(ADC1_BASE, ADC_SC1B_POINTER, true, false, ADC_CHANNEL_7);

    // Configure PDB module
    // Enable PDB clock gate
    CLOCK_SYS_EnablePdbClock(HW_PDB0);
    // Clear PDB register
    PDB_HAL_Init(PDB0_BASE);
    // Enable PDB
    PDB_HAL_Enable(PDB0_BASE);
    // Set load register mode
    PDB_HAL_SetLoadMode(PDB0_BASE, kPdbLoadImmediately);
    // Select PDB external trigger source, it is from FTM0
    PDB_HAL_SetTriggerSrcMode(PDB0_BASE, kPdbTrigger8);
    // Select PDB modulo value
    PDB_HAL_SetModulusValue(PDB0_BASE, PDB_MODULO_VALUE);
    // Write int value
    PDB_HAL_SetIntDelayValue(PDB0_BASE, PDB_INT_VALUE);
    // Use PDB back-to-back mode, the channel 0 pre-trigger 0 is from delay counter flag, others trigger is from ADC acknowledge
    // Set PDB channel 0, pre-trigger 0 delay counter to 200
    PDB_HAL_SetPreTriggerDelayCount(PDB0_BASE, PDB_CH0, PDB_PRETRIGGER0, PRETRIGGER_DELAY_VALUE);
    // Enable pre-trigger
    PDB_HAL_SetPreTriggerCmd(PDB0_BASE, PDB_CH0, PDB_PRETRIGGER0, true);
    PDB_HAL_SetPreTriggerCmd(PDB0_BASE, PDB_CH0, PDB_PRETRIGGER1, true);
    PDB_HAL_SetPreTriggerCmd(PDB0_BASE, PDB_CH1, PDB_PRETRIGGER0, true);
    PDB_HAL_SetPreTriggerCmd(PDB0_BASE, PDB_CH1, PDB_PRETRIGGER1, true);
    // Enable pre-trigger output
    PDB_HAL_SetPreTriggerOutputCmd(PDB0_BASE, PDB_CH0, PDB_PRETRIGGER0, true);
    PDB_HAL_SetPreTriggerOutputCmd(PDB0_BASE, PDB_CH0, PDB_PRETRIGGER1, true);
    PDB_HAL_SetPreTriggerOutputCmd(PDB0_BASE, PDB_CH1, PDB_PRETRIGGER0, true);
    PDB_HAL_SetPreTriggerOutputCmd(PDB0_BASE, PDB_CH1, PDB_PRETRIGGER1, true);
    // Set back-to-back mode
    // Enable channel 0 pre-trigger 1 to back-to-back mode
    PDB_HAL_SetPreTriggerBackToBackCmd(PDB0_BASE, PDB_CH0, PDB_PRETRIGGER1, true);
    // Enable channel 1 pre-trigger 0 to back-to-back mode
    PDB_HAL_SetPreTriggerBackToBackCmd(PDB0_BASE, PDB_CH1, PDB_PRETRIGGER0, true);
    // Enable channel 1 pre-trigger 1 to back-to-back mode
    PDB_HAL_SetPreTriggerBackToBackCmd(PDB0_BASE, PDB_CH1, PDB_PRETRIGGER1, true);
    // load PDB register
    PDB_HAL_SetLoadRegsCmd(PDB0_BASE);

    // Configure FTM module
    // Configure FTM0 as complementary combine mode, the frequency is 16KHz, and insert the dead time is 1us
    // Enable FTM0 channel 0 trigger as FTM0 external trigger source for PDB
    // configure FTM output pin
    configure_ftm_pins(HW_FTM0);
    // Enable FTM clock gate
    CLOCK_SYS_EnableFtmClock(HW_FTM0);
    FTM_HAL_Enable(FTM0_BASE, true);
    // Clear CPWMS bit
    FTM_HAL_SetCpwms(FTM0_BASE, false);
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
        FTM_HAL_SetChnMSnBAMode(FTM0_BASE, i, 2);
        FTM_HAL_SetChnEdgeLevel(FTM0_BASE, i, 1);
        FTM_HAL_SetChnCountVal(FTM0_BASE, i, cnvvalue[i]);
    }
    // Configure FTM0 channel ouput period is 16KHz complementary waveform (channel n and n+1)
    // Insert the deadtime is 1us
    // Modulo value is (75MHz/16KHz -1)
    FTM_HAL_SetMod(FTM0_BASE, FTM_MODULO_VALUE); 
    FTM_HAL_SetWriteProtectionCmd(FTM0_BASE, false);
    // Configure combine mode
    for(i=0; i<FSL_FEATURE_FTM_CHANNEL_COUNT/2; i+=2)
    {
        FTM_HAL_SetDualChnCombineCmd(FTM0_BASE, i, true);
        FTM_HAL_SetDualChnCompCmd(FTM0_BASE, i, true);
        FTM_HAL_SetDualChnDeadtimeCmd(FTM0_BASE, i, true);
        FTM_HAL_SetDualChnPwmSyncCmd(FTM0_BASE, i, true);
    }
    // Set deadtime to 1us
    FTM_HAL_SetDeadtimePrescale(FTM0_BASE, kFtmDivided4);
    FTM_HAL_SetDeadtimeCount(FTM0_BASE, DEADTIME_VALUE);
    FTM_HAL_SetSoftwareTriggerCmd(FTM0_BASE, true);
    for(i=1; i<FSL_FEATURE_FTM_CHANNEL_COUNT; i+=2)
    {
        // Enable channel select in PWMLOAD register
        FTM_HAL_SetPwmLoadChnSelCmd(FTM0_BASE, i, true);
    }
    // Load FTM registers like MOD, CnV and INT
    FTM_HAL_SetPwmLoadCmd(FTM0_BASE, true);
    // Set PDB clock divide to 1
    FTM_HAL_SetClockPs(FTM0_BASE, kFtmDividedBy1);
    // Set PDB clock source, use system clock
    FTM_HAL_SetClockSource(FTM0_BASE, kClock_source_FTM_SystemClk);

    while(1)
    {
        // Input any character to start demo
        printf("\r\nInput any character to start demo.\n\n\r");
        getchar();
        // Reset counter
        FTM_HAL_SetCounter(FTM0_BASE, 0);
        // Enable FTM0 channel0 trigger as external trigger
        FTM_HAL_SetChnTriggerCmd(FTM0_BASE, HW_CHAN0, true);
        // Wait data buffer is full
        while(u16CycleTimes<255);
        // Disable FTM0 channel0 trigger
        FTM_HAL_SetChnTriggerCmd(FTM0_BASE, HW_CHAN0, false);
        // Clear FTM0 channel external trigger flag, it will not trig PDB
        if(FTM_HAL_IsChnTriggerGenerated(FTM0_BASE))
        {
            BW_FTM_EXTTRIG_TRIGF(FTM0_BASE, 0);
        }
        // Ouput ADC conversion result
        for(i=0;i<256;i++)
        {
            printf("\r\n%d, %d, %d, %d\n\r",u16Result0A[i],u16Result0B[i],u16Result1A[i],u16Result1B[i]);
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
    if(ADC16_HAL_GetChnConvCompletedCmd(ADC0_BASE, ADC_SC1A_POINTER))
    {
        u16Result0A[u16CycleTimes] = ADC16_HAL_GetChnConvValueRAW(ADC0_BASE, ADC_SC1A_POINTER);
    }
    if(ADC16_HAL_GetChnConvCompletedCmd(ADC0_BASE, ADC_SC1B_POINTER))
    {
        u16Result0B[u16CycleTimes] = ADC16_HAL_GetChnConvValueRAW(ADC0_BASE, ADC_SC1B_POINTER);
    }
}

/*!
 * @brief ADC1 interrupt service routine, read data from result register
 */
void ADC1_IRQHandler(void)
{
    if(ADC16_HAL_GetChnConvCompletedCmd(ADC1_BASE, ADC_SC1A_POINTER))
    {
        u16Result1A[u16CycleTimes] = ADC16_HAL_GetChnConvValueRAW(ADC1_BASE, ADC_SC1A_POINTER);
    }

    if(ADC16_HAL_GetChnConvCompletedCmd(ADC1_BASE, ADC_SC1B_POINTER))
    {
        u16Result1B[u16CycleTimes] = ADC16_HAL_GetChnConvValueRAW(ADC1_BASE, ADC_SC1B_POINTER);

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
uint8_t ADC_Calibration(uint32_t baseAddr)
{
#if FSL_FEATURE_ADC_HAS_OFFSET_CORRECTION
    uint32_t offsetValue;
#endif
    uint32_t plusSideGainValue;
#if FSL_FEATURE_ADC_HAS_DIFF_MODE
    uint32_t minusSideGainValue;
#endif
    // Special configuration for highest accuracy.
    // Enable clock for ADC.
    if(baseAddr == ADC0_BASE)
    {
        CLOCK_SYS_EnableAdcClock(HW_ADC0);
    }
    else
    {
        CLOCK_SYS_EnableAdcClock(HW_ADC1);
    }
    ADC16_HAL_Init(baseAddr);
    // Maximum Average.
#if FSL_FEATURE_ADC_HAS_HW_AVERAGE
    ADC16_HAL_SetHwAverageCmd(baseAddr, true);
    ADC16_HAL_SetHwAverageMode(baseAddr, kAdcHwAverageCountOf32);
#endif // FSL_FEATURE_ADC_HAS_HW_AVERAGE

    // Lowest ADC Frequency.
    ADC16_HAL_SetClkSrcMode(baseAddr, kAdcClkSrcOfBusClk);
    ADC16_HAL_SetClkDividerMode(baseAddr, kAdcClkDividerInputOf8);

    // Reference voltage as Vadd.
    ADC16_HAL_SetRefVoltSrcMode(baseAddr, kAdcRefVoltSrcOfVref);

    // Software trigger.
    ADC16_HAL_SetHwTriggerCmd(baseAddr, false);

    // Launch auto calibration.
    ADC16_HAL_SetAutoCalibrationCmd(baseAddr, true);
    while (!ADC16_HAL_GetChnConvCompletedCmd(baseAddr, ADC_SC1A_POINTER))
    {
        if (ADC16_HAL_GetAutoCalibrationFailedCmd(baseAddr))
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
