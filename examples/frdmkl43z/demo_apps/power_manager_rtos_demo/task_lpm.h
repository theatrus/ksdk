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
#ifndef TASK_LPM_H
#define TASK_LPM_H
///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////
// Standard C Included Files
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

// SDK Included Files
#include "fsl_adc16_driver.h"
// #include "fsl_pdb_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_pmc_hal.h"
#include "board.h"

// Project Included Files
#include "lpm_rtos.h"
#include "adc16_temperature.h"
#include "fsl_debug_console.h"
#if (defined PM_RTOS_DEMO_RTC_FUNC_INSTANCE)
#include "rtc_setup.h"
#endif
#include "lptmr_setup.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
/* Checking return value then print out error message. */
#define CHECK_RET_VAL(ret, mode) \
if (ret != kPowerManagerSuccess) \
{ \
    PRINTF("POWER_SYS_SetMode(%u) returned unexpected status : %u\n\r",mode,ret); \
}

///////////////////////////////////////////////////////////////////////////////
// Structures & enumerations
///////////////////////////////////////////////////////////////////////////////
typedef enum demo_power_modes {
    kDemoMin = 'A' -1,
    kDemoRun  = 'A',           // Normal RUN mode
    kDemoWait,
    kDemoStop,
    kDemoVlpr,
    kDemoVlpw,
    kDemoVlps,
#if FSL_FEATURE_SMC_HAS_LOW_LEAKAGE_STOP_MODE
    kDemoLls,
#endif
#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE0 & BOARD_SW_HAS_LLWU_PIN
    kDemoVlls0,
#endif
    kDemoVlls1,
#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE2
    kDemoVlls2,
#endif
    kDemoVlls3,
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
    kDemoHsRun,           // HighSpeed RUN mode
#endif
    kDemoCM0,           // Clock Manager Mode configuration 0
    kDemoCM1,           // Clock Manager Mode configuration 1
    KDemoADC,
    kDemoMax
} demo_power_modes_t;

// callback type for power manager user callback data.
// each callback message has this variables for counting events
typedef struct {
    uint32_t counter;
    uint32_t status;
    uint32_t err;
} callback_data_t;

// callback type which is used for power manager user callback
typedef struct {
    callback_data_t before;
    callback_data_t after;
    power_manager_notify_t lastType;
    uint32_t err;
} user_callback_data_t;

typedef enum wakeUpSource
{
    wakeUpSourceErr = 0,
    wakeUpSourceRtc = 1,
    wakeUpSourceLptmr = 2,
    wakeUpSourceSwBtn =  4,
} wakeUpSource_t;

typedef struct lptmrStructure
{
    uint32_t                instance;
    lptmr_user_config_t     lptmrUserConfig;
    lptmr_state_t           lptmrState;
} lptmrStructure_t;

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

extern lptmrStructure_t cmCallbackData;

/*--------------------------------MODE---------------------------------------*/
extern power_manager_user_config_t const vlprConfig;

extern power_manager_user_config_t const vlpwConfig;

#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE0 & BOARD_SW_HAS_LLWU_PIN
extern power_manager_user_config_t const vlls0Config;
#endif
extern power_manager_user_config_t const vlls1Config;
#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE2
extern power_manager_user_config_t const vlls2Config;
#endif
extern power_manager_user_config_t const vlls3Config;
#if FSL_FEATURE_SMC_HAS_LOW_LEAKAGE_STOP_MODE
extern power_manager_user_config_t const llsConfig;
#endif
extern power_manager_user_config_t const vlpsConfig;
extern power_manager_user_config_t const waitConfig;
extern power_manager_user_config_t const stopConfig;
extern power_manager_user_config_t const runConfig;
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
extern power_manager_user_config_t const hsrunConfig;
#endif

extern power_manager_user_config_t const *powerConfigs[];

/*---------------------------------------------------------------------------*/
extern power_manager_error_code_t rtos_pm_callback(power_manager_notify_struct_t * notify,
 power_manager_callback_data_t * dataPtr);

extern clock_manager_error_code_t rtos_cm_callback(clock_notify_struct_t *notify,
                                    void* callbackData);
extern clock_manager_error_code_t dbg_console_cm_callback(clock_notify_struct_t *notify,
                                    void* callbackData);
extern power_manager_error_code_t adc16_pm_callback(power_manager_notify_struct_t * notify,
 power_manager_callback_data_t * dataPtr);

extern clock_manager_error_code_t adc16_cm_callback(clock_notify_struct_t * notify, void* callbackData);

extern clock_manager_callback_user_config_t dbg_console_cm_callback_data;

extern clock_manager_callback_user_config_t rtos_cm_callback_data;

extern clock_manager_callback_user_config_t adc16_cm_callback_data;

extern clock_manager_callback_user_config_t *cm_callback_tbl[];

extern power_manager_callback_user_config_t const adc16_pm_callback_data;

extern power_manager_callback_user_config_t const rtos_pm_callback_data;

extern power_manager_callback_user_config_t *pm_callback_tbl[];

extern size_t const cm_callback_tbl_size;
extern size_t const powerConfigsSize;
extern size_t const pm_callback_tbl_size;

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////

/* LLW_IRQHandler that would cover the same name's APIs in startup code */
void MODULE_IRQ_HANDLER(LLWU)(void);
/*!
 * @brief set alarm command.
 *
 * This function set the alarm which will be
 * trigerred x secs later. The alarm trigger
 * will print a notification on the console.
 */
void cmdAlarm(wakeUpSource_t wus, uint8_t offsetSec);

/*!
 * @brief get wake up source from given character.
 */
wakeUpSource_t getWakeupSource(uint8_t val, demo_power_modes_t mod);

/*!
 * @brief print wakeup source which was selected.
 */
void printWakeUpSourceText(wakeUpSource_t wus);

/*!
 * @brief set wakeup source.
 */
void setWakeUpSource(wakeUpSource_t wus,char *textMode);

/*!
 * @brief IRQ handler for switch/button.
 */
void BOARD_SW_LLWU_IRQ_HANDLER(void);

/*!
 * @brief set timeout for timer module.
 */
uint8_t setWakeUpTimeOut(wakeUpSource_t wus);

/*!
 * @brief select wakeup source.
 *
 * TWR_K60D100M doesn't have switch pins connected to LLWU.
 * It's not possible to wake up by SWx buttons from some modes.
 * returns 0 when RTC is selected as wake up source
 * returns 1 when sw pins are selected as wake up source
 */
wakeUpSource_t selectWakeUpSource(demo_power_modes_t mode);

#if FSL_FEATURE_LLWU_HAS_INTERNAL_MODULE
/*!
 * @brief disable wakeup source.
 */
void llwuDisableWakeUp(void);
#endif

/*!
 * @brief disable gpio switch.
 */
void gpioDisableWakeUp(void);

/*!
 * @brief set gpio switch as wakeup source.
 */
void gpioEnableWakeUp(void);

/*!
 * @brief display current power mode.
 */
void displayPowerMode(void);

/*!
 * @brief update clock manager to run mode.
 */
void updateClockManagerToRunMode(uint8_t cmConfigMode);

/*!
 * @brief update clock manager to vlpr mode.
 */
void updateClockManagerToVlprMode(uint8_t cmConfigMode);

#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
/*!
 * @brief update clock manager to hsrun mode.
 */
void updateClockManagerToHsRunMode(uint8_t cmConfigMode);
#endif

/*!
 * @brief Update clock to compatible with RUN mode.
 */
void updateClockMode(uint8_t cmConfigMode);

/*!
 * @brief little delay for uart flush.
 */
void delay(uint32_t delay_time);

/*!
 * @brief task low power management.
 */
void task_lpm(task_param_t param);

#endif

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
