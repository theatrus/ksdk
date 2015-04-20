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
#include "fsl_debug_console.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_uart_driver.h"
#include "fsl_i2c_slave_driver.h"
#include "fsl_smc_hal.h"
#include "i2c_rtos.h"
#include "fsl_adc16_driver.h"

extern uint32_t gSlaveId;

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

// These values are used covert temperature. DO NOT MODIFY
#define VTEMP25_ADC             (14219)
#define K                       (10000)
#define M1                      (250000)
#define M2                      (311)

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////

extern void task_slave(void *param);

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
static uint32_t Temperature;

adc16_user_config_t tempSnseAdcConfig;

// ADC Channel Configuration
adc16_chn_config_t tempSnseChannelConfig =
{
    .chnNum = 26,
    .chnMux = kAdcChnMuxOfA,
    .diffEnable = false,
    .intEnable = false
};

// task define
OSA_TASK_DEFINE(task_slave, TASK_SLAVE_STACK_SIZE);

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * Get the temperature pointer
 * designed for BM version of I2C_RTOS demo
 * from the ISR context
 */
uint8_t* get_temp_pointer(void)
{
    uint32_t adcValue;
    // ADC starts conversion
    ADC16_DRV_ConfigConvChn(HWADC_INSTANCE, 0U, &tempSnseChannelConfig);
    // poll to complete status and read back result
    ADC16_DRV_WaitConvDone(HWADC_INSTANCE, 0U);
    adcValue = ADC16_DRV_GetConvValueRAW(HWADC_INSTANCE, 0U);
    adcValue = ADC16_DRV_ConvRAWData(adcValue, false, tempSnseAdcConfig.resolutionMode);
    // ADC stop conversion
    ADC16_DRV_PauseConv(HWADC_INSTANCE, 0U);
    // convert to temperature
    Temperature = (M1 - (adcValue - VTEMP25_ADC) * M2)/K;
    return (uint8_t*)&Temperature;
}

/*!
 * @brief main function
 */
int main(void)
{
    // SMC Power mode protection configurations
    smc_power_mode_protection_config_t smc_power_prot_cfg = 
    {
        .vlpProt = true,
        .llsProt = false,
        .vllsProt = false
    };
    
#if FSL_FEATURE_ADC16_HAS_CALIBRATION
    adc16_calibration_param_t tempSnseCalibraitionParam;
#endif

    hardware_init();
    GPIO_DRV_Init(NULL, ledPins);

    // Configure the power mode protection
    SMC_HAL_SetProtection(SMC_BASE, &smc_power_prot_cfg);

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
    
    // get cpu uid low value for slave
    gSlaveId = HW_SIM_UIDL_RD(SIM_BASE);
    
    // enable printf
    dbg_uart_init();

    // Configure I2C pins
    configure_i2c_pins(I2C_RTOS_SLAVE_INSTANCE);

    printf("i2c_rtos_slave_bm demo\r\n");
    
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
