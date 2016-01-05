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
#include <string.h>
#include <math.h>
#include <stdio.h>

// SDK Included Files
#include "board.h"
#include "fsl_i2c_slave_driver.h"
#include "fsl_smc_hal.h"
#include "fsl_os_abstraction.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
//  Definitions
///////////////////////////////////////////////////////////////////////////////

#define CMD_MODE                (0)     /*! This value indicate the slave is receiving command from master */
#define PROCESS_MODE            (1)     /*! This value indicate the slave is transfering data with master */

enum _subaddress_index_e
{
    Subaddress_Index_0 = 0x00,
    Subaddress_Index_1 = 0x01u,
    Subaddress_Index_2 = 0x02u,
    Subaddress_Index_3 = 0x03u,
    Subaddress_Index_4 = 0x04u,
    Subaddress_Index_5 = 0x05u,
    Subaddress_Index_6 = 0x06u,
    Subaddress_Index_7 = 0x07u,
    Invalid_Subaddress_Index,
    Max_Subaddress_Index
};

///////////////////////////////////////////////////////////////////////////////
//  Variables
///////////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint8_t  subAddress;
    uint8_t  data;
    uint8_t  state;
} i2cData_t;

uint8_t u8SlaveDataBuffer[Max_Subaddress_Index]   = {'I', '2', 'C', '-', 'C', 'O', 'M', 'M', };

///////////////////////////////////////////////////////////////////////////////
//  Code
///////////////////////////////////////////////////////////////////////////////

static void LED_turnoff_master(void)
{
    GPIO_DRV_SetPinOutput(kGpioLED1);
}
static void LED_toggle_slave(void)
{
    GPIO_DRV_TogglePinOutput(kGpioLED2);
}
static void LED_turnon_slave(void)
{
    GPIO_DRV_ClearPinOutput(kGpioLED2);
}
static void LED_turnoff_slave(void)
{
    GPIO_DRV_SetPinOutput(kGpioLED2);
}

static void i2c_slave_event_callback_passive(uint8_t instance, i2c_slave_event_t i2cEvent,void *callParam)
{
    i2c_slave_state_t * slaveState = I2C_DRV_SlaveGetHandler(instance);
    i2cData_t *userData = (i2cData_t*)callParam;
    switch(i2cEvent)
    {
        case kI2CSlaveTxReq:
            slaveState ->txBuff = &u8SlaveDataBuffer[userData->subAddress];
            slaveState ->txSize = 1;
            slaveState ->isTxBusy = true;

            userData->state = CMD_MODE;
            LED_toggle_slave();
        break;

        case kI2CSlaveRxReq:
            if (userData->state == CMD_MODE)
            {
                slaveState ->rxBuff = &userData->subAddress;
                userData->state = PROCESS_MODE;
            }

            slaveState ->rxSize = 1;
            slaveState ->isRxBusy = true;
            LED_toggle_slave();
        break;

        case kI2CSlaveRxFull:
            if (userData->state == PROCESS_MODE)
            {
                // current state == PROCESS_MODE -> we need to receive data
                slaveState ->rxBuff = &userData->data;
                slaveState ->rxSize = 1;
                userData->state = CMD_MODE;
                slaveState ->isRxBusy = true;
                LED_toggle_slave();
            }
            else
            {
                u8SlaveDataBuffer[userData->subAddress] = userData->data;
                LED_toggle_slave();
            }
        break;

        default:
        break;
    }
}
/*!
 * @brief main function
 */
int main(void)
{
    i2c_slave_state_t slave;

    i2cData_t i2cData =
    {
      .subAddress = Invalid_Subaddress_Index,
      .data = 0,
      .state = CMD_MODE
    };

    i2c_slave_user_config_t userConfig =
    {
        .address = 0x3A,
        .slaveListening = true,
        .slaveCallback  = i2c_slave_event_callback_passive,
        .callbackParam  = &i2cData,
#if FSL_FEATURE_I2C_HAS_START_STOP_DETECT
        .startStopDetect  = false,
#endif
#if FSL_FEATURE_I2C_HAS_STOP_DETECT
        .stopDetect       = false,
#endif
    };

    // Low Power Configuration
    smc_power_mode_config_t smcConfig;

    // Init struct
    memset(&smcConfig, 0, sizeof(smcConfig));

    hardware_init();
    OSA_Init();
    GPIO_DRV_Init(0, ledPins);

    // Initiate I2C instance module
    I2C_DRV_SlaveInit(BOARD_I2C_INSTANCE, &userConfig, &slave);

    PRINTF("\r\n====== I2C Slave ======\r\n\r\n");

    // turn LED_slave on to indicate I2C slave status is waiting for date receiving
    LED_turnon_slave();
    LED_turnoff_master();
    OSA_TimeDelay(50);

    PRINTF("\r\n I2C slave enters low power mode...\r\n");

    // set to allow entering specific modes
    SMC_HAL_SetProtection(SMC, kAllowPowerModeVlp);

    // set power mode to specific Run mode
#if FSL_FEATURE_SMC_HAS_LPWUI
    smcConfig.lpwuiOptionValue = kSmcLpwuiEnabled;
#endif
#if FSL_FEATURE_SMC_HAS_PORPO
    smcConfig.porOptionValue = kSmcPorEnabled;
#endif

#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
    smcConfig.powerModeName = kPowerModeRun;
    // If current status is HSRUN mode, change to RUN mode first.
    if (kStatHsrun == SMC_HAL_GetStat(SMC_BASE_PTR))
    {
        SMC_HAL_SetMode(SMC_BASE_PTR, &smcConfig);
    }
#endif

    smcConfig.powerModeName = kPowerModeWait;
    // Entry to Low Power Mode
    SMC_HAL_SetMode(SMC_BASE_PTR, &smcConfig);

    // LED_slave is still on during low power mode until I2C master send data to slave.
    // Turn off LED_slave to indicate MCU wake up by I2C address matching interrupt
    LED_turnoff_slave();
    PRINTF("\r\n I2C slave wakes up from low power mode by I2C address matching.\r\n");

    while(1);
}
