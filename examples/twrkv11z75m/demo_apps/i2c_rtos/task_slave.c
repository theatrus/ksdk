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

// SDK Included Files
#include "fsl_i2c_slave_driver.h"
#include "fsl_smc_hal.h"
#include "i2c_rtos.h"
#include "fsl_debug_console.h"

/* Prototypes */
extern uint8_t* get_temp_pointer(void);

/* Variables */
static i2c_slave_state_t slaveDev;
static i2c_slave_user_config_t  userConfig = {
            .address = I2C_RTOS_SLAVE_ADDRESS,
            .slaveListening   = false,
            .slaveCallback    = NULL,
            .callbackParam    = NULL,
#if FSL_FEATURE_I2C_HAS_START_STOP_DETECT
            .startStopDetect  = false,
#endif
#if FSL_FEATURE_I2C_HAS_STOP_DETECT
            .stopDetect       = false,
#endif
};
uint32_t gSlaveId;

// SMC Power mode configuration variables
static smc_power_mode_config_t smc_config_data =
{
    .powerModeName = kPowerModeVlps,
    .stopSubMode = kSmcStopSub0,
#if FSL_FEATURE_SMC_HAS_LPWUI
    .lpwuiOptionValue = kSmcLpwuiEnabled,
#endif
#if FSL_FEATURE_SMC_HAS_PORPO
    .porOptionValue = kSmcPorDisabled
#endif
};

static void handle_cmd(uint8_t cmd)
{
    uint8_t  bufRecv[2];
    uint32_t slaveId = 0;
    static uint8_t *pSlaveId = (uint8_t*)&gSlaveId;
    static uint8_t *pTemper = NULL;

    switch (cmd)
    {
    case I2C_RTOS_LIGHT_CMD:
        I2C_DRV_SlaveReceiveDataBlocking(I2C_RTOS_SLAVE_INSTANCE, bufRecv, 2, 1000);
        switch (bufRecv[0])
        {
        case 'R':
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_RED, !bufRecv[1]);
            break;
        case 'G':
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_GREEN, !bufRecv[1]);
            break;
#if defined(BOARD_GPIO_LED_BLUE)
        case 'B':
            GPIO_DRV_WritePinOutput(BOARD_GPIO_LED_BLUE, !bufRecv[1]);
            break;
#endif
        default:
            break;
        }
        break;
    case I2C_RTOS_SLEEP_CMD:
        I2C_DRV_SlaveReceiveDataBlocking(I2C_RTOS_SLAVE_INSTANCE, bufRecv, 2, 1000);
        /* check if request from self master */
        slaveId = *(uint8_t *)&bufRecv[0];
        if (slaveId == gSlaveId)
        {
            PRINTF("Can not sleep on yourself\r\n");
            break;
        }
        PRINTF("Master tell me to go sleep..\r\n");
        // TODO: stop the sys tick, and let OS know change mode
        // Enter VLPS mode
        SMC_HAL_SetMode(SMC_BASE_PTR, &smc_config_data);
        PRINTF("Wake up from sleep\r\n");
        break;
    case I2C_RTOS_TEMP_CMD:
        pTemper = get_temp_pointer();
        I2C_DRV_SlaveSendDataBlocking(I2C_RTOS_SLAVE_INSTANCE, pTemper, 4, 1000);
        break;
    case I2C_RTOS_READID_CMD:
        I2C_DRV_SlaveSendDataBlocking(I2C_RTOS_SLAVE_INSTANCE, pSlaveId, 4, 1000);
        break;
    default:
        break;
    }
}

/*!
 * Task for slave getting request and put response
 */
void task_slave(task_param_t param)
{
    uint8_t bufRecv[2];

    /* Init i2c slave driver */
    I2C_DRV_SlaveInit(I2C_RTOS_SLAVE_INSTANCE, &userConfig, &slaveDev);

    while (1)
    {
        /* Receive block data from block data */
        I2C_DRV_SlaveReceiveDataBlocking(I2C_RTOS_SLAVE_INSTANCE, bufRecv, 2, OSA_WAIT_FOREVER);
        if(bufRecv[0] == I2C_RTOS_MAGIC)
        {
            handle_cmd(bufRecv[1]);
        }
    }
}
