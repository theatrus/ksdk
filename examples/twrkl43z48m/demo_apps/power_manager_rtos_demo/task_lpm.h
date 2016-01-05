/*
 * Copyright (c) 2014 - 2015, Freescale Semiconductor, Inc.
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
#include "rtc_setup.h"
#include "lptmr_setup.h"
#if (defined FSL_RTOS_MQX)
#include "bsp.h"
#endif

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
/* Checking return value then print out error message. */

#define DELAY_COUNT           1000000

#define SYSTICK_CM4_CSR_REG (*((volatile unsigned long *)0xE000E010))
#define SYSTICK_CM4_RVR_REG (*((volatile unsigned long *)0xE000E014))
#define SYSTICK_CM4_CVR_REG (*((volatile unsigned long *)0xE000E018))
#define SYSTICK_CM4_CALIB_REG (*((volatile unsigned long *)0xE000E01C))

#define SYSTICK_DISABLE()       (SYSTICK_CM4_CSR_REG &= (~1))
#define SYSTICK_ENABLE()        (SYSTICK_CM4_CSR_REG |= 1)
#define SYSTICK_RELOAD(tps)     (SYSTICK_CM4_RVR_REG = tps)

#if (FSL_RTOS_FREE_RTOS)
#define TICK_PER_SEC            configTICK_RATE_HZ
#elif (FSL_RTOS_UCOSII)
#define TICK_PER_SEC            OS_TICKS_PER_SEC
#elif (FSL_RTOS_UCOSIII)
#define TICK_PER_SEC            OSCfg_TickRate_Hz
#elif (FSL_RTOS_MQX)
#define TICK_PER_SEC            BSP_ALARM_FREQUENCY
#else
#define TICK_PER_SEC            1000
#endif

#define CHECK_RET_VAL(ret, mode) \
if (ret != kPowerManagerSuccess) \
{ \
    PRINTF("POWER_SYS_SetMode(%u) returned unexpected status : %u\r\n",mode,ret); \
}

///////////////////////////////////////////////////////////////////////////////
// Structures & enumerations
///////////////////////////////////////////////////////////////////////////////
typedef enum demo_power_modes {
    kDemoMin  = 'A' -1,
    kDemoRun  = 'A',           // Normal RUN mode
    kDemoWait,
    kDemoStop,
    kDemoVlpr,
    kDemoVlpw,
    kDemoVlps,
    kDemoLls,
    kDemoVlls1,
    kDemoVlls3,
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

lptmrStructure_t cmCallbackData;

/*--------------------------------MODE---------------------------------------*/
power_manager_user_config_t const vlprConfig =
{
    .mode = kPowerManagerVlpr,
    .sleepOnExitValue = false,
};

power_manager_user_config_t const vlpwConfig =
{
    .mode = kPowerManagerVlpw,
};

power_manager_user_config_t const vlls1Config =
{
    .mode = kPowerManagerVlls1,
    .sleepOnExitValue = false,
};
power_manager_user_config_t const vlls3Config =
{
    .mode = kPowerManagerVlls3,
    .sleepOnExitValue = false,
};
power_manager_user_config_t const llsConfig =
{
    // classic LLS mode retains all ram content too
    .mode = kPowerManagerLls,

    .sleepOnExitValue = false,
};
power_manager_user_config_t const vlpsConfig =
{
    .mode = kPowerManagerVlps,
    .sleepOnExitValue = false,
};
power_manager_user_config_t const waitConfig =
{
    .mode = kPowerManagerWait,
    .sleepOnExitValue = false,
};
power_manager_user_config_t const stopConfig =
{
    .mode = kPowerManagerStop,
    .sleepOnExitValue = false,
};
power_manager_user_config_t const runConfig =
{
    .mode = kPowerManagerRun
};

power_manager_user_config_t const *powerConfigs[] =
{
    &runConfig,
    &waitConfig,
    &stopConfig,
    &vlprConfig,
    &vlpwConfig,
    &vlpsConfig,
    &llsConfig,
    &vlls1Config,
    &vlls3Config,
};

/*--------------------------------Callback---------------------------------------*/
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
// Prototypes
///////////////////////////////////////////////////////////////////////////////

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

wakeUpSource_t selectWakeUpSource(demo_power_modes_t mode);

/*!
 * @brief disable wakeup source.
 */
void llwuDisableWakeUp(void);

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

void wait_finish_uart(void);

void task_led_rtos(task_param_t param);

void task_led_clock(task_param_t param);

/*----------------------Callback handler-------------------------*/
/*!
 * @brief task low power management.
 * Function Name : dbg_console_cm_callback
 * Description   : debug console callback for change event from power manager
 */
clock_manager_error_code_t dbg_console_cm_callback(clock_notify_struct_t *notify, void* callbackData);

power_manager_error_code_t rtos_pm_callback(power_manager_notify_struct_t * notify,  power_manager_callback_data_t * dataPtr);

clock_manager_error_code_t rtos_cm_callback(clock_notify_struct_t *notify, void* callbackData);

/*----------------------------------------------------------------*/


///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
