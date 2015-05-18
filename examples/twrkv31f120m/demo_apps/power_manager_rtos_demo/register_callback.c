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
#include <stddef.h>

// SDK Included Files
#include "board.h"
#include "fsl_power_manager.h"
#include "fsl_clock_manager.h"

// Project Included Files
#include "task_lpm.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Structures & enumerations
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

lptmrStructure_t cmCallbackData;
// lptmr_user_config_t lptmrUserConfig;
// lptmr_state_t gLptmrState;

// power_manager_callback_tbl_data_t const * const rtosLpmCallbackData = 0;

/*--------------------------------MODE---------------------------------------*/
power_manager_user_config_t const vlprConfig =
{
    .mode = kPowerManagerVlpr,
#if FSL_FEATURE_SMC_HAS_LPWUI
    .lowPowerWakeUpOnInterruptValue = kSmcLpwuiEnabled,
#endif
    .sleepOnExitValue = false,
};

power_manager_user_config_t const vlpwConfig =
{
    .mode = kPowerManagerVlpw,
#if FSL_FEATURE_SMC_HAS_LPWUI
    .lowPowerWakeUpOnInterruptValue = kSmcLpwuiEnabled,
#endif
};

#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE0 & BOARD_SW_HAS_LLWU_PIN
power_manager_user_config_t const vlls0Config =
{
    .mode = kPowerManagerVlls0,
#if FSL_FEATURE_SMC_HAS_LPWUI
    .lowPowerWakeUpOnInterruptValue = kSmcLpwuiEnabled,
#endif
    .sleepOnExitValue = false,
};
#endif
power_manager_user_config_t const vlls1Config =
{
    .mode = kPowerManagerVlls1,
#if FSL_FEATURE_SMC_HAS_LPWUI
    .lowPowerWakeUpOnInterruptValue = kSmcLpwuiEnabled,
#endif
    .sleepOnExitValue = false,
};
#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE2
power_manager_user_config_t const vlls2Config =
{
    .mode = kPowerManagerVlls2,
#if FSL_FEATURE_SMC_HAS_LPWUI
    .lowPowerWakeUpOnInterruptValue = kSmcLpwuiEnabled,
#endif
    .sleepOnExitValue = false,
};
#endif
power_manager_user_config_t const vlls3Config =
{
    .mode = kPowerManagerVlls3,
#if FSL_FEATURE_SMC_HAS_LPWUI
    .lowPowerWakeUpOnInterruptValue = kSmcLpwuiEnabled,
#endif
    .sleepOnExitValue = false,
};
#if FSL_FEATURE_SMC_HAS_LOW_LEAKAGE_STOP_MODE
power_manager_user_config_t const llsConfig =
{
#if FSL_FEATURE_SMC_HAS_LLS_SUBMODE
    // LLS3 mode retains all ram content so CPU wake up doesn't go through restart sequence
    .mode = kPowerManagerLls3,
#else
    // classic LLS mode retains all ram content too
    .mode = kPowerManagerLls,
#endif

#if FSL_FEATURE_SMC_HAS_LPWUI
    .lowPowerWakeUpOnInterruptValue = kSmcLpwuiEnabled,
#endif
    .sleepOnExitValue = false,
};
#endif
power_manager_user_config_t const vlpsConfig =
{
    .mode = kPowerManagerVlps,
#if FSL_FEATURE_SMC_HAS_LPWUI
    .lowPowerWakeUpOnInterruptValue = kSmcLpwuiEnabled,
#endif
    .sleepOnExitValue = false,
};
power_manager_user_config_t const waitConfig =
{
    .mode = kPowerManagerWait,
#if FSL_FEATURE_SMC_HAS_LPWUI
    .lowPowerWakeUpOnInterruptValue = kSmcLpwuiEnabled,
#endif
    .sleepOnExitValue = false,
};
power_manager_user_config_t const stopConfig =
{
    .mode = kPowerManagerStop,
#if FSL_FEATURE_SMC_HAS_LPWUI
    .lowPowerWakeUpOnInterruptValue = kSmcLpwuiEnabled,
#endif
    .sleepOnExitValue = false,
};
power_manager_user_config_t const runConfig =
{
    .mode = kPowerManagerRun
};
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
power_manager_user_config_t const hsrunConfig =
{
    .mode = kPowerManagerHsrun
};
#endif

power_manager_user_config_t const *powerConfigs[] =
{
    &runConfig,
    &waitConfig,
    &stopConfig,
    &vlprConfig,
    &vlpwConfig,
    &vlpsConfig,
#if FSL_FEATURE_SMC_HAS_LOW_LEAKAGE_STOP_MODE
    &llsConfig,
#endif
#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE0 & BOARD_SW_HAS_LLWU_PIN
    &vlls0Config,
#endif
    &vlls1Config,
#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE2
    &vlls2Config,
#endif
    &vlls3Config,
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
    &hsrunConfig
#endif
};

/*---------------------------------------------------------------------------*/
clock_manager_callback_user_config_t dbg_console_cm_callback_tbl_data =
{
    .callback     = dbg_console_cm_callback,
    .callbackType = kClockManagerCallbackBeforeAfter,
    .callbackData = NULL
};

clock_manager_callback_user_config_t rtos_cm_callback_tbl_data =
{
    .callback     = rtos_cm_callback,
    .callbackType = kClockManagerCallbackBeforeAfter,
    .callbackData = &cmCallbackData,
};

clock_manager_callback_user_config_t *cm_callback_tbl[] =
{
    &rtos_cm_callback_tbl_data,
    &dbg_console_cm_callback_tbl_data,
};

power_manager_callback_user_config_t adc16_pm_callback_tbl_data =
{
    .callback     = adc16_pm_callback,
    .callbackType = kPowerManagerCallbackBeforeAfter,
    .callbackData = NULL
};

power_manager_callback_user_config_t rtos_pm_callback_tbl_data =
{
    .callback     = rtos_pm_callback,
    .callbackType = kPowerManagerCallbackBeforeAfter,
    .callbackData = NULL,
};

power_manager_callback_user_config_t *pm_callback_tbl[] =
{
    &adc16_pm_callback_tbl_data,
    &rtos_pm_callback_tbl_data
};

size_t const cm_callback_tbl_size = sizeof(cm_callback_tbl)/sizeof(clock_manager_callback_user_config_t *);
size_t const powerConfigsSize = sizeof(powerConfigs)/sizeof(power_manager_user_config_t *);
size_t const pm_callback_tbl_size = sizeof(pm_callback_tbl)/sizeof(power_manager_callback_user_config_t *);
///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
