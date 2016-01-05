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

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "board.h"
#include "fsl_adc16_driver.h"
#include "fsl_pdb_driver.h"
#include "fsl_ftm_driver.h"
#include "fsl_debug_console.h"
#include "fsl_clock_manager.h"
/* arm DSP lib */
#include "arm_math.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

//! @brief ADC channels definition for thermistors using differential input
#define RT1_ADC_CHL     (0x01U)
#define RT2_ADC_CHL     (0x00U)
#define RT3_ADC_CHL     (0x01U)
#define RT4_ADC_CHL     (0x02U)

//! @brief PWM frequency
#define PWM_FREQ_HZ          8000

// use const to output FIR filter coefficients
// to the data section to be placed in data memory.
#include "FirCoefs.h"

//! @brief Low pass filter */
#define DIFF_TS     400
#define BUFF_SIZE   20                              //!< filter buf size

//! @brief S/W timer
#define SWTMR_TOUT  ((400*PWM_FREQ_HZ)/1000)        //!< 400 ms period
#define _100MS      ((100*PWM_FREQ_HZ)/1000)        //!< 100 ms period

//! @briefRT thresholds
#define RT1ONTH     -8
#define RT1OFFTH    8
#define RT2ONTH     -25
#define RT2OFFTH    20
#define RT3ONTH     -10
#define RT3OFFTH    8
#define RT4ONTH     -25
#define RT4OFFTH    10

//! @brief FIR algorithm related
#define Q15                     32768
#define FRAC16(a)               (a*Q15)

#define TEST_LENGTH_SAMPLES     1
#define BLOCK_SIZE              1
#define NUM_TAPS                31

//! @brief MAX no of ADC samples
#define MAX_NO_SAMPLES          16

#define ALL_LED_ON      LED1_ON;LED2_ON;LED3_ON;LED4_ON;LED5_ON;LED6_ON;LED7_ON;LED8_ON;
#define ALL_LED_OFF     LED1_OFF;LED2_OFF;LED3_OFF;LED4_OFF;LED5_OFF;LED6_OFF;LED7_OFF;LED8_OFF;
#define ALL_LED_EN      LED1_EN;LED2_EN;LED3_EN;LED4_EN;LED5_EN;LED6_EN;LED7_EN;LED8_EN;

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////


void RT_Check(int16_t i16Rt1, int16_t i16Rt2, int16_t i16Rt3, int16_t i16Rt4);
void RT_LEDs_On(void);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

typedef union
{
    struct
    {
        uint16_t rt1on          : 1;    //!< RT1ON
        uint16_t rt2on          : 1;    //!< RT2ON
        uint16_t rt3on          : 1;    //!< RT3ON
        uint16_t rt4on          : 1;    //!< RT4ON
        uint16_t idleloop       : 1;    //!< idle loop flag
        uint16_t Rsvd           : 11;   //!< RESERVED
    } Bits;
    uint16_t W16;
} RT_ControlType;                       //!< RT Control bits

//! @brief Global variables
volatile uint16_t        swDiffCNTR, swTimerCNTR;
volatile uint16_t        timeout;

volatile RT_ControlType  rton;
volatile uint16_t        rt_filter_on;
volatile uint16_t        rt_filter_off;

volatile int16_t         rt1_filt_buff[BUFF_SIZE], rt2_filt_buff[BUFF_SIZE];
volatile int16_t         rt3_filt_buff[BUFF_SIZE], rt4_filt_buff[BUFF_SIZE];

volatile uint16_t        i_delay = 1, i_sample = 0;

int16_t rt1, rt2, rt3, rt4;
int16_t rt1_filt, rt2_filt, rt3_filt, rt4_filt;
int16_t delta_rt1, delta_rt2;
int16_t delta_rt3, delta_rt4;

//! @brief Declare FIR State buffer of size (numTaps + u32BlockSize - 1)
static int16_t i16FirStateQ15_1[BLOCK_SIZE + NUM_TAPS - 1];
static int16_t i16FirStateQ15_2[BLOCK_SIZE + NUM_TAPS - 1];
static int16_t i16FirStateQ15_3[BLOCK_SIZE + NUM_TAPS - 1];
static int16_t i16FirStateQ15_4[BLOCK_SIZE + NUM_TAPS - 1];

//! @brief Instances of FIR internal state
static arm_fir_instance_q15 S1,S2,S3,S4;

uint32_t u32BlockSize = BLOCK_SIZE;
uint32_t u32NumBlocks = TEST_LENGTH_SAMPLES/BLOCK_SIZE;

//! @brief For ADC samples
uint16_t u16Result0A[MAX_NO_SAMPLES]={0},u16Result0B[MAX_NO_SAMPLES]={0},u16Result1A[MAX_NO_SAMPLES]={0},u16Result1B[MAX_NO_SAMPLES]={0};
volatile uint16_t u8CycleTimes=0,gu8RdCounter =0;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Initialize the FTM to generate trigger source
 *
 */
static int32_t init_ftm(void)
{
    ftm_user_config_t ftmInfo;
    ftm_pwm_param_t ftmParam;

    ftmParam.mode                   = kFtmEdgeAlignedPWM;
    ftmParam.edgeMode               = kFtmLowTrue;
    ftmParam.uFrequencyHZ           = PWM_FREQ_HZ;
    ftmParam.uDutyCyclePercent      = 50;
    ftmParam.uFirstEdgeDelayPercent = 0;

    // Enable FTM module and generate PWM
    memset(&ftmInfo, 0, sizeof(ftmInfo));
    ftmInfo.syncMethod = kFtmUseSoftwareTrig;

    FTM_DRV_Init(BOARD_FTM_INSTANCE, &ftmInfo);
    FTM_DRV_SetClock(BOARD_FTM_INSTANCE, kClock_source_FTM_SystemClk, kFtmDividedBy1);
    FTM_DRV_PwmStart(BOARD_FTM_INSTANCE, &ftmParam, BOARD_FTM_CHANNEL);

    // Issue a software trigger to update registers
    FTM_HAL_SetSoftwareTriggerCmd(g_ftmBase[BOARD_FTM_INSTANCE], true);

    // Enable the generation of the FTM peripheral timer channel trigger
    FTM_HAL_SetChnTriggerCmd(g_ftmBase[BOARD_FTM_INSTANCE], BOARD_FTM_CHANNEL, true);

    return 0;
}

/*!
 * @brief Initialize the ADCx for HW trigger.
 *
 */
static int32_t init_adc(void)
{
    adc16_chn_config_t adcChnConfig;
    adc16_converter_config_t adcUserConfig;
    // out5 divider is 4, ADC clock source is 75/4MHz
    CLOCK_HAL_SetOutDiv5ENCmd(SIM_BASE_PTR, true);
    CLOCK_HAL_SetOutDiv5(SIM_BASE_PTR, 3);
    // ADC module configuration and initialization
    ADC16_DRV_StructInitUserConfigDefault(&adcUserConfig);
    adcUserConfig.hwTriggerEnable      = true;
    adcUserConfig.continuousConvEnable = false;
    adcUserConfig.lowPowerEnable       = false;
    adcUserConfig.asyncClkEnable       = false;
    adcUserConfig.refVoltSrc           = kAdc16RefVoltSrcOfVref;
#if BOARD_ADC_USE_ALT_VREF
    adcUserConfig.refVoltSrc = kAdc16RefVoltSrcOfValt;
#endif
    adcUserConfig.clkSrc               = kAdc16ClkSrcOfAltClk;
    adcUserConfig.clkDividerMode       = kAdc16ClkDividerOf1;
    adcUserConfig.resolution           = kAdc16ResolutionBitOf16;
    ADC16_DRV_Init(ADC0_IDX, &adcUserConfig);
    ADC16_DRV_Init(ADC1_IDX, &adcUserConfig);

    // Calibration
#if FSL_FEATURE_ADC_HAS_CALIBRATION
    adc_calibration_param_t adcCalibraitionParam;

    ADC16_DRV_GetAutoCalibrationParam(ADC0_IDX, &adcCalibraitionParam);
    ADC16_DRV_SetCalibrationParam(ADC0_IDX, &adcCalibraitionParam);
    ADC16_DRV_GetAutoCalibrationParam(ADC1_IDX, &adcCalibraitionParam);
    ADC16_DRV_SetCalibrationParam(ADC1_IDX, &adcCalibraitionParam);
#endif

    // RT1 Chl Config
    adcChnConfig.chnIdx = (adc16_chn_t)RT1_ADC_CHL;
#if FSL_FEATURE_ADC16_HAS_DIFF_MODE
    adcChnConfig.diffConvEnable = true;
#endif
    adcChnConfig.convCompletedIntEnable = true;
    ADC16_DRV_ConfigConvChn(ADC0_IDX, 0U, &adcChnConfig);
#if FSL_FEATURE_ADC16_HAS_MUX_SELECT
    ADC16_DRV_SetChnMux(ADC0_IDX, kAdc16ChnMuxOfA);
#endif
    // RT2 Chl Config
    adcChnConfig.chnIdx = (adc16_chn_t)RT2_ADC_CHL;
#if FSL_FEATURE_ADC16_HAS_DIFF_MODE
    adcChnConfig.diffConvEnable = true;
#endif
    adcChnConfig.convCompletedIntEnable = true;
    ADC16_DRV_ConfigConvChn(ADC0_IDX, 1U, &adcChnConfig);

    // RT3 Chl Config
    adcChnConfig.chnIdx = (adc16_chn_t)RT3_ADC_CHL;
#if FSL_FEATURE_ADC16_HAS_DIFF_MODE
    adcChnConfig.diffConvEnable = true;
#endif
    adcChnConfig.convCompletedIntEnable = true;
    ADC16_DRV_ConfigConvChn(ADC1_IDX, 0U, &adcChnConfig);
#if FSL_FEATURE_ADC16_HAS_MUX_SELECT
    ADC16_DRV_SetChnMux(ADC1_IDX, kAdc16ChnMuxOfA);
#endif
    // RT4 Chl Config
    adcChnConfig.chnIdx = (adc16_chn_t)RT4_ADC_CHL;
    #if FSL_FEATURE_ADC16_HAS_DIFF_MODE
    adcChnConfig.diffConvEnable = true;
#endif
    adcChnConfig.convCompletedIntEnable = true;
    ADC16_DRV_ConfigConvChn(ADC1_IDX, 1U, &adcChnConfig);

    return 0;
}

/*!
 * @brief Initialize the PDB for ADC hardware trigger.
 *
 */
static int32_t pdb_adc_trigger_init(void)
{
    // Allocate memory for PDB driver
    pdb_timer_config_t MyPdbUserConfigStruct;
    pdb_adc_pretrigger_config_t MyPdbAdcTriggerConfigStruct;

    // Prepare the configuration structure.
    MyPdbUserConfigStruct.continuousModeEnable = false;
    MyPdbUserConfigStruct.intEnable            = false;
    MyPdbUserConfigStruct.clkPreMultFactor     = kPdbClkPreMultFactorAs1;
    MyPdbUserConfigStruct.clkPreDiv            = kPdbClkPreDivBy1;
    MyPdbUserConfigStruct.triggerInput         = kPdbTrigger8;
    MyPdbUserConfigStruct.loadValueMode        = kPdbLoadValueImmediately;
    PDB_DRV_Init(PDB0_IDX, &MyPdbUserConfigStruct);
    PDB_DRV_SetTimerModulusValue(PDB0_IDX, 1500);
    PDB_DRV_SetValueForTimerInterrupt(PDB0_IDX,1500);

    // Initialize the ADC trigger in PDB.
    MyPdbAdcTriggerConfigStruct.preTriggerBackToBackEnable = true;
    MyPdbAdcTriggerConfigStruct.preTriggerOutputEnable     = true;
    MyPdbAdcTriggerConfigStruct.adcPreTriggerIdx           = ADC0_IDX;
    MyPdbAdcTriggerConfigStruct.preTriggerEnable           = true;
    PDB_DRV_SetAdcPreTriggerDelayValue(PDB0_IDX,ADC0_IDX,0,200);
    PDB_DRV_ConfigAdcPreTrigger(PDB0_IDX, 0, &MyPdbAdcTriggerConfigStruct);

    // Initialize the ADC trigger in PDB.
    MyPdbAdcTriggerConfigStruct.preTriggerBackToBackEnable = true;
    MyPdbAdcTriggerConfigStruct.preTriggerOutputEnable = true;
    MyPdbAdcTriggerConfigStruct.adcPreTriggerIdx = ADC0_IDX;
    MyPdbAdcTriggerConfigStruct.preTriggerEnable = true;
    PDB_DRV_SetAdcPreTriggerDelayValue(PDB0_IDX,ADC0_IDX,1,1000);
    PDB_DRV_ConfigAdcPreTrigger(PDB0_IDX, 1, &MyPdbAdcTriggerConfigStruct);

    // Initialize the ADC trigger in PDB.
    MyPdbAdcTriggerConfigStruct.preTriggerBackToBackEnable = false;
    MyPdbAdcTriggerConfigStruct.preTriggerOutputEnable     = true;
    MyPdbAdcTriggerConfigStruct.adcPreTriggerIdx           = ADC1_IDX;
    MyPdbAdcTriggerConfigStruct.preTriggerEnable           = true;
    PDB_DRV_SetAdcPreTriggerDelayValue(PDB0_IDX,ADC1_IDX,0,200);
    PDB_DRV_ConfigAdcPreTrigger(PDB0_IDX, 0, &MyPdbAdcTriggerConfigStruct);

    // Initialize the ADC trigger in PDB.
    MyPdbAdcTriggerConfigStruct.preTriggerBackToBackEnable = false;
    MyPdbAdcTriggerConfigStruct.preTriggerOutputEnable     = true;
    MyPdbAdcTriggerConfigStruct.adcPreTriggerIdx           = ADC1_IDX;
    MyPdbAdcTriggerConfigStruct.preTriggerEnable           = true;
    PDB_DRV_SetAdcPreTriggerDelayValue(PDB0_IDX,ADC1_IDX,1,1000);
    PDB_DRV_ConfigAdcPreTrigger(PDB0_IDX, 1, &MyPdbAdcTriggerConfigStruct);

    return 0;
}

int main(void)
{
    uint16_t timeout100MS;

    arm_status err;
    hardware_init();

    PRINTF("Thermistor lab demo!\r\n");

    // Initialize GPIO
    GPIO_DRV_Init(switchPins, ledPins);
    ALL_LED_EN;

    // Call FIR init function to initialize the instance structure.
    err = arm_fir_init_q15(&S1, NUM_TAPS, (q15_t*)&i16FirCoefs[0], &i16FirStateQ15_1[0], u32BlockSize);
    if (err != ARM_MATH_SUCCESS)
    {
        return (-1);
    }
    err = arm_fir_init_q15(&S2, NUM_TAPS, (q15_t*)&i16FirCoefs[0], &i16FirStateQ15_2[0], u32BlockSize);
    if (err != ARM_MATH_SUCCESS)
    {
        return (-1);
    }

    err = arm_fir_init_q15(&S3, NUM_TAPS, (q15_t*)&i16FirCoefs[0], &i16FirStateQ15_3[0], u32BlockSize);
    if (err != ARM_MATH_SUCCESS)
    {
        return (-1);
    }

    err = arm_fir_init_q15(&S4, NUM_TAPS, (q15_t*)&i16FirCoefs[0], &i16FirStateQ15_4[0], u32BlockSize);
    if (err != ARM_MATH_SUCCESS)
    {
        return (-1);
    }

    // Initialize ADC module
    init_adc();

    // Initialize PDB
    pdb_adc_trigger_init();

    // Initialize FTM module
    init_ftm();

    // Ưait until rtx_filt_buffers are initialized
    while(swTimerCNTR <  ((350*PWM_FREQ_HZ)/1000));
    while(swTimerCNTR <  ((350*PWM_FREQ_HZ)/1000));
    PRINTF("begin test...\r\n");

    rton.W16 &= ~0xF;
    while(1)
    {
        timeout100MS = timeout / (_100MS);

        switch(timeout100MS)
        {
            case 0:
                RT_LEDs_On();
                break;
            case 1:
                ALL_LED_OFF;
                break;
            case 2:
                RT_LEDs_On();
                break;
            case 3:
                ALL_LED_OFF;
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief check 4 thermistors to see if fingers are close to them or left.
 *
 * This function is called by ADC interrupts.
 */
void RT_Check(int16_t i16Rt1, int16_t i16Rt2, int16_t i16Rt3, int16_t i16Rt4)
{
    rt1 = i16Rt1;
    rt2 = i16Rt2;
    rt3 = i16Rt3;
    rt4 = i16Rt4;

    // Execute FIR filter on ADC samples
    arm_fir_q15(&S1, &i16Rt1, &rt1_filt, 1);
    arm_fir_q15(&S2, &i16Rt2, &rt2_filt, 1);
    arm_fir_q15(&S3, &i16Rt3, &rt3_filt, 1);
    arm_fir_q15(&S4, &i16Rt4, &rt4_filt, 1);

    // Reset SWSamplerCNT
    if(swDiffCNTR >= DIFF_TS)
    {
        // Evaluate delta
        delta_rt1 = rt1_filt - rt1_filt_buff[i_delay];
        delta_rt2 = rt2_filt - rt2_filt_buff[i_delay];
        delta_rt3 = rt3_filt - rt3_filt_buff[i_delay];
        delta_rt4 = rt4_filt - rt4_filt_buff[i_delay];

        // Store samples in the buffer
        rt1_filt_buff[i_sample] = rt1_filt;
        rt2_filt_buff[i_sample] = rt2_filt;
        rt3_filt_buff[i_sample] = rt3_filt;
        rt4_filt_buff[i_sample] = rt4_filt;
        if(++i_sample >= BUFF_SIZE)
        {
            i_sample = 0;
        }
        if(++i_delay >= BUFF_SIZE)
        {
            i_delay = 0;
        }
        // Detect rt ON/OFF
        // Three consequent samples has to be higher than threshold limit
        // rt1 ON filter
        if(delta_rt1 < RT1ONTH)
        {
            rt_filter_on += 0x1;
            if((rt_filter_on & 0xf) >= 0x3)
            {
                rt_filter_on &= ~0xf;
                rton.Bits.rt1on = 1;
            }
        }
        else
        {
            rt_filter_on &= ~0xf;
        }
         // rt1 OFF filter
        if(delta_rt1 > RT1OFFTH)
        {
            rt_filter_off += 0x1;
            if((rt_filter_off & 0xf) >= 0x3)
            {
                rt_filter_off &= ~0xf;
                rton.W16 &= ~0xf;
            }
        }
        else
        {
            rt_filter_off &= ~0xf;
        }

        // rt2 ON filter
        if(delta_rt2 < RT2ONTH)
        {
            rt_filter_on += 0x10;
            if((rt_filter_on & 0xf0) >= 0x30)
            {
                rt_filter_on &= ~0xf0;
                rton.Bits.rt2on = 1;
            }
        }
        else
        {
            rt_filter_on &= ~0xf0;
        }
        // rt2 OFF filter
        if(delta_rt2 > RT2OFFTH)
        {
            rt_filter_off += 0x10;
            if((rt_filter_off & 0xf0) >= 0x30)
            {
                rt_filter_off &= ~0xf0;
                rton.W16 &= ~0xf;
            }
        }
        else
        {
            rt_filter_off &= ~0xf0;
        }

        // rt3 ON filter
        if(delta_rt3 < RT3ONTH)
        {
            rt_filter_on += 0x100;
            if((rt_filter_on & 0xf00) >= 0x300)
            {
                rt_filter_on &= ~0xf00;
                rton.Bits.rt3on = 1;
            }
        }
        else
        {
            rt_filter_on &= ~0xf00;
        }
        // rt3 OFF filter
        if(delta_rt3 > RT3OFFTH)
        {
            rt_filter_off += 0x100;
            if((rt_filter_off & 0xf00) >= 0x300)
            {
                rt_filter_off &= ~0xf00;
                rton.W16 &= ~0xf;
            }
        }
        else
        {
            rt_filter_off &= ~0xf00;
        }

        // rt4 ON filter
        if(delta_rt4 < RT4ONTH)
        {
            rt_filter_on += 0x1000;
            if((rt_filter_on & 0xf000) >= 0x3000)
            {
                rt_filter_on &= ~0xf000;
                rton.Bits.rt4on = 1;
            }
        }
        else
        {
            rt_filter_on &= ~0xf000;
        }
        // rt4 OFF filter
        if(delta_rt4 > RT4OFFTH)
        {
            rt_filter_off += 0x1000;
            if((rt_filter_off & 0xf000) >= 0x3000)
            {
                rt_filter_off &= ~0xf000;
                rton.W16 &= ~0xf;
            }
        }
        else
        {
            rt_filter_off &= ~0xf000;
        }

        // Reset counter
        swDiffCNTR = 0;
    }
    else
    {
        swDiffCNTR++;
    }

    // Reset SWTimerCNTR every 400ms
    // Software timer is used to control LED flashing intervals
    if(swTimerCNTR >= SWTMR_TOUT)
    {
        swTimerCNTR = 0;
    }
    else
    {
        swTimerCNTR++;
    }

    if(swTimerCNTR == timeout)
    {
        rton.Bits.idleloop = 0;
        // Time out event every 100 ms
        timeout += _100MS;
        if(timeout >= SWTMR_TOUT) timeout = 0;
    }
}

void RT_LEDs_On(void)
{
    if(rton.Bits.rt1on)
    {
       ALL_LED_OFF;
       LED1_ON;
       LED2_ON;
    }
    if(rton.Bits.rt2on)
    {
       ALL_LED_OFF;
       LED3_ON;
       LED4_ON;
    }
    if(rton.Bits.rt3on)
    {
       ALL_LED_OFF;
       LED5_ON;
       LED6_ON;
    }
    if(rton.Bits.rt4on)
    {
       ALL_LED_OFF;
       LED7_ON;
       LED8_ON;
    }
}

/*!
 * @brief override the ADC0_IRQ handler
 *
 */
void ADC0_IRQHandler(void)
{
    if(ADC16_HAL_GetChnConvCompletedFlag(ADC0_BASE_PTR, 0U))
    {
        u16Result0A[u8CycleTimes] = ADC16_HAL_GetChnConvValue(ADC0_BASE_PTR, 0U);
    }
    if(ADC16_HAL_GetChnConvCompletedFlag(ADC0_BASE_PTR, 1U))
    {
        u16Result0B[u8CycleTimes] = ADC16_HAL_GetChnConvValue(ADC0_BASE_PTR, 1U);
    }
}

/*!
 * @brief override the ADC1_IRQ handler
 *
 */
void ADC1_IRQHandler(void)
{
    if(ADC16_HAL_GetChnConvCompletedFlag(ADC1_BASE_PTR, 0U))
    {
        u16Result1A[u8CycleTimes] = ADC16_HAL_GetChnConvValue(ADC1_BASE_PTR, 0U);
    }
    if(ADC16_HAL_GetChnConvCompletedFlag(ADC1_BASE_PTR, 1U))
    {
        u16Result1B[u8CycleTimes] = ADC16_HAL_GetChnConvValue(ADC1_BASE_PTR, 1U);
        RT_Check(u16Result0A[u8CycleTimes], u16Result1A[u8CycleTimes],
                 u16Result0B[u8CycleTimes], u16Result1B[u8CycleTimes]);
        u8CycleTimes++;
        if(u8CycleTimes >= MAX_NO_SAMPLES)
        {
            u8CycleTimes = 0;
        }
    }
}

/*!
 * @brief for test only
 *
 */
void PDB0_IRQHandler(void)
{
    // Clear Flags.
    if ( PDB_DRV_GetTimerIntFlag(PDB0_IDX))
    {
        PDB_DRV_ClearTimerIntFlag(PDB0_IDX);
    }
    PRINTF("%s\r\n", __func__);
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
