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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "fsl_os_abstraction.h"
#include "fsl_interrupt_manager.h"
#include "fsl_power_manager.h"
#include "fsl_gpio_driver.h"
#include "fsl_llwu_hal.h"
#include "fsl_smc_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include "fsl_sim_hal.h"
#include "board.h"
#include "fsl_misc_utilities.h"
#include "fsl_rtc_driver.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

// enum with power manager modes. This modes are supported by the demo
typedef enum demo_power_modes {
    kDemoMin = 'A' -1,
    kDemoWait = 'A',
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
    kDemoRun,           // Normal RUN mode
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
    kDemoHsRun,           // HighSpeed RUN mode
#endif
    kDemoCM0,           // Clock Manager Mode configuration 0
    kDemoCM1,           // Clock Manager Mode configuration 1
    kDemoMax
}demo_power_modes_t;

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

/* Checking return value then print out error message. */
#define CHECK_RET_VAL(ret, mode) \
if (ret != kPowerManagerSuccess) \
{ \
    printf("POWER_SYS_SetMode(%u) returned unexpected status : %u\n\r",mode,ret); \
}
////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////
static volatile bool gSecsFlag;
////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////
void dbg_uart_reinit(void);
/*******************************************************************************
 * For Clock transition test.
 ******************************************************************************/
/*
 * Callback for debug console.
 * Debug console may use UART or LPUART for different platforms, so there
 * should be callback for UART or LPUART, to simplify the unit test, here
 * call DbgConsole_Init/DbgConsole_DeInit in callback function, because
 * DbgConsole_xxx could access the proper IP.
 */
clock_manager_error_code_t debugCallback(clock_notify_struct_t *notify,
                                    void* callbackData)
{
    clock_manager_error_code_t result = kClockManagerSuccess;

    switch (notify->notifyType)
    {
        case kClockManagerNotifyBefore:     // Received "pre" message
            DbgConsole_DeInit();
            break;
        case kClockManagerNotifyRecover: // Received "recover" message
        case kClockManagerNotifyAfter:    // Received "post" message
        dbg_uart_reinit();
             break;
        default:
            result = kClockManagerError;
            break;
    }
    return result;
}

/* static clock callback table. */
clock_manager_callback_user_config_t debugConsoleCalback =
{
    .callback     = debugCallback,
    .callbackType = kClockManagerCallbackBeforeAfter,
    .callbackData = NULL
};

static clock_manager_callback_user_config_t *clockCallbackTable[] =
{
    &debugConsoleCalback
};

/*!
 * @brief override the RTC IRQ handler
 */
void RTC_IRQHandler(void)
{
    if (RTC_DRV_IsAlarmPending(0))
    {
        RTC_DRV_SetAlarmIntCmd(0, false);
    }
}

/*!
 * @brief override the RTC Second IRQ handler
 */
void RTC_Seconds_IRQHandler(void)
{
    gSecsFlag = true;
}

/*!
 * @brief set alarm command.
 *
 * This function set the alarm which will be
 * trigerred x secs later. The alarm trigger
 * will print a notification on the console.
 */
static void cmd_alarm(uint8_t offsetSec)
{
    rtc_datetime_t date;
    uint32_t seconds;

    if ((offsetSec < 1) || (offsetSec > 60))
    {
        offsetSec = 5;
    }

    // get date time and convert to seconds
    RTC_DRV_GetDatetime(0, &date);

    // convert to sec and add offset
    RTC_HAL_ConvertDatetimeToSecs(&date, &seconds);
    seconds += offsetSec;
    RTC_HAL_ConvertSecsToDatetime(&seconds, &date);

    // set the datetime for alarm
    if (RTC_DRV_SetAlarm(0, &date, true))
    {
    }
    else
    {
        printf("Failed to set alarm. Alarm time is not in the future\r\n");
        return;
    }
}

/* LLW_IRQHandler that would cover the same name's APIs in startup code */
#if (defined FRDM_KL03Z48M)
void LLWU_IRQHandler(void)
#else
void LLW_IRQHandler(void)
#endif
{
#if FSL_FEATURE_LLWU_HAS_INTERNAL_MODULE
    if (RTC_DRV_IsAlarmPending(0))
    {
        RTC_DRV_SetAlarmIntCmd(0, false);
    }
#endif
#if BOARD_SW_HAS_LLWU_PIN
    LLWU_HAL_ClearExternalPinWakeupFlag(LLWU_BASE, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
#endif
}

/* IRQ handler for switch/button. */
void BOARD_SW_LLWU_IRQ_HANDLER(void)
{
    PORT_HAL_ClearPortIntFlag(BOARD_SW_LLWU_BASE);
}

/* Power manager user callback */
power_manager_error_code_t callback0(power_manager_notify_struct_t * notify,
                                     power_manager_callback_data_t * dataPtr)
{
    user_callback_data_t * userData = (user_callback_data_t*) dataPtr;
    power_manager_error_code_t ret = kPowerManagerError;

    switch (notify->notifyType)
    {
        case kPowerManagerNotifyBefore:
            userData->before.counter++;
            ret = kPowerManagerSuccess;
            break;
        case kPowerManagerNotifyAfter:
            userData->after.counter++;
            ret = kPowerManagerSuccess;
            break;
        default:
            userData->err++;
            break;
    }

    userData->lastType = notify->notifyType;

    return ret;
}


/* little delay for uart flush */
void delay(uint32_t delay_time)
{
    uint32_t wait;
    for(wait=0;wait<delay_time;wait++)
    {
        __asm("NOP");
    }
}

uint8_t setWakeUpTimeOut(void)
{
    uint8_t val0;
    uint8_t val1;

    while(1)
    {
        printf("Select the wake up timeout in format DD. Possible decimal value is from range 01 - 60 seconds. Eg. 05 means 5 seconds delay");
        printf("\n\rWaiting for key press..\n\r\n\r");
        val0 = getchar();
        if( (val0 >= '0') && (val0 <= '6') )
        {
            val1 = getchar();
            if( (val1 >= '0') && (val1 <= '9') )
            {
                val0 = (val0-'0')*10 + (val1-'0');
                if( (val0!=0) && (val0<=60) )
                {
#if FSL_FEATURE_LLWU_HAS_INTERNAL_MODULE
                    OSA_EnterCritical(kCriticalDisableInt);
                    LLWU_HAL_SetInternalModuleCmd(LLWU_BASE,kLlwuWakeupModule5,true);
                    OSA_ExitCritical(kCriticalDisableInt);
#endif
                    cmd_alarm(val0);
                    return val0;
                }
            }
        }

        printf("Wrong value!\n\r");
    }
}

void printWakeUpSourceText(void)
{
    printf("Select the wake up source:\n\r");
    printf("Press T for RTC - RTC Timer\n\r");
    printf("Press S for switch/button ");
    PRINT_LLWU_SW_NUM;
    printf("\n\r");
}

/* TWR_K60D100M doesn't have switch pins connected to LLWU.
 * It's not possible to wake up by SWx buttons from some modes.
 * returns 0 when RTC is selected as wake up source
 * returns 1 when sw pins are selected as wake up source
 */
uint8_t selectWakeUpSource(demo_power_modes_t mode){
    uint8_t testVal;

    while(1)
    {
        switch(mode)
        {
            case kDemoWait:
            case kDemoVlpr:
            case kDemoVlpw:

            case kDemoStop:
            case kDemoVlps:
                printWakeUpSourceText();
                break;
#if FSL_FEATURE_SMC_HAS_LOW_LEAKAGE_STOP_MODE
            case kDemoLls:
#endif
            case kDemoVlls1:
#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE2
            case kDemoVlls2:
#endif
            case kDemoVlls3:
#if BOARD_SW_HAS_LLWU_PIN
                printWakeUpSourceText();
                break;
#else
                // switch is not connected to LLWU, we can use only RTC wake up
                printf("The board does not support wake up from this mode by switch/button. RTC is used as wake up source:\n\r");
                return 0;
#endif
#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE0 & BOARD_SW_HAS_LLWU_PIN
            case kDemoVlls0:

                // in VLLS0 LPO oscillator is disabled.
#if FSL_FEATURE_MCG_HAS_RTC_32K
                printf("The board does not support wake up from this mode by RTC due to disabled External reference.\n\r");
#else
                printf("The board does not support wake up from this mode by RTC due to disabled LPO.\n\r");
#endif
                return 1;
#endif
            default:
                // default is RTC
                return 0;
        }
        printf("\n\rWaiting for key press..\n\r\n\r");
        // Wait for user response
        testVal = getchar();
        if(testVal == 'T')
        {
            return 0;
        }
        else if(testVal == 'S')
        {
            return 1;
        }
        else
        {
            printf("Wrong value!\n\r");
        }
    }
}

#if FSL_FEATURE_LLWU_HAS_INTERNAL_MODULE
void llwuDisableWakeUp(void)
{
    LLWU_HAL_SetInternalModuleCmd(LLWU_BASE,kLlwuWakeupModule5,false);
}
#endif

void gpioDisableWakeUp(void)
{
    // disables interrupt
    PORT_HAL_SetPinIntMode(BOARD_SW_LLWU_BASE, BOARD_SW_LLWU_PIN, kPortIntDisabled);
    INT_SYS_DisableIRQ(BOARD_SW_LLWU_IRQ_NUM);

#if BOARD_SW_HAS_LLWU_PIN
    LLWU_HAL_ClearExternalPinWakeupFlag(LLWU_BASE, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
    LLWU_HAL_SetExternalInputPinMode(LLWU_BASE,kLlwuExternalPinDisabled, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
#endif
}

void gpioEnableWakeUp(void)
{
    // enables falling edge interrupt for switch SWx
    PORT_HAL_SetPinIntMode(BOARD_SW_LLWU_BASE, BOARD_SW_LLWU_PIN, kPortIntFallingEdge);
    INT_SYS_EnableIRQ(BOARD_SW_LLWU_IRQ_NUM);

#if BOARD_SW_HAS_LLWU_PIN
    LLWU_HAL_ClearExternalPinWakeupFlag(LLWU_BASE, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
    LLWU_HAL_SetExternalInputPinMode(LLWU_BASE,kLlwuExternalPinFallingEdge, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
#endif
}

void setWakeUpSource(uint8_t source,char *textMode)
{
    uint8_t timeout;

    if(!source)
    {
        gpioDisableWakeUp();
        // wake up on RTC interrupt
        timeout = setWakeUpTimeOut();
        printf("Entering %s, will wake up on RTC interrupt after %u seconds\n\r",textMode,timeout);
    }
    else
    {
#if FSL_FEATURE_LLWU_HAS_INTERNAL_MODULE
        llwuDisableWakeUp();
#endif
        // wake up on gpio interrupt from button(s)
        gpioEnableWakeUp();
        printf("Entering %s, press the ",textMode);
        PRINT_LLWU_SW_NUM;
        printf(" button to wake up.\n\r");
    }
}

void displayPowerMode(void)
{
    uint32_t smcMode = SMC_HAL_GetStat(SMC_BASE);

    switch (smcMode)
    {
    case kStatRun:
        printf("    SMC mode = kPowerModeRun\n\r");
        break;
    case kStatStop:
        printf("    SMC mode = kPowerModeStop\n\r");
        break;
    case kStatVlpr:
        printf("    SMC mode = kPowerModeVlpr\n\r");
        break;
    case kStatVlpw:
        printf("    SMC mode = kPowerModeVlpw\n\r");
        break;
    case kStatVlps:
        printf("    SMC mode = kPowerModeVlps\n\r");
        break;
#if FSL_FEATURE_SMC_HAS_LOW_LEAKAGE_STOP_MODE
    case kStatLls:
        printf("    SMC mode = kPowerModeLls\n\r");
        break;
#endif
    case kStatVlls:
        printf("    SMC mode = kPowerModeVlls\n\r");
        break;
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
    case kStatHsrun:
        printf("    SMC mode = kPowerModeHsrun\n\r");
        break;
#endif
    default:
        break;
    }
}

void updateClockManagerToRunMode(uint8_t cmConfigMode)
{
    // if current config mode is RUN but CM is not, need to re-config it to RUN
    if ((cmConfigMode == CLOCK_CONFIG_INDEX_FOR_RUN) &&
        (CLOCK_SYS_GetCurrentConfiguration() != CLOCK_CONFIG_INDEX_FOR_RUN))
    {
        CLOCK_SYS_UpdateConfiguration(CLOCK_CONFIG_INDEX_FOR_RUN, kClockManagerPolicyForcible);
    }
}

void updateClockManagerToVlprMode(uint8_t cmConfigMode)
{
    // if current config mode and CM are both RUN, need to re-config it to VLPR
    if ((cmConfigMode == CLOCK_CONFIG_INDEX_FOR_RUN) &&
        (CLOCK_SYS_GetCurrentConfiguration() != CLOCK_CONFIG_INDEX_FOR_VLPR))
    {
        CLOCK_SYS_UpdateConfiguration(CLOCK_CONFIG_INDEX_FOR_VLPR, kClockManagerPolicyForcible);
    }
}

#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
void updateClockManagerToHsRunMode(uint8_t cmConfigMode)
{
    // if current config mode and CM are both RUN, need to re-config it to VLPR
    if ((cmConfigMode == CLOCK_CONFIG_INDEX_FOR_RUN) &&
        (CLOCK_SYS_GetCurrentConfiguration() != CLOCK_CONFIG_INDEX_FOR_HSRUN))
    {
        CLOCK_SYS_UpdateConfiguration(CLOCK_CONFIG_INDEX_FOR_HSRUN, kClockManagerPolicyForcible);
    }
}
#endif

/* Update clock to compatible with RUN mode. */
void update_clock_mode(uint8_t cmConfigMode)
{
#if (defined(FSL_FEATURE_MCGLITE_MCGLITE))
    if (g_defaultClockConfigurations[cmConfigMode].mcgliteConfig.mcglite_mode == kMcgliteModeHirc48M)
#else
    if (g_defaultClockConfigurations[cmConfigMode].mcgConfig.mcg_mode == kMcgModePEE)
#endif
    {
        CLOCK_SYS_UpdateConfiguration(CLOCK_CONFIG_INDEX_FOR_RUN, kClockManagerPolicyForcible);
    }
    else
    {
        updateClockManagerToRunMode(cmConfigMode);
    }
}

/* Re-init uart module. */
void dbg_uart_reinit()
{
#if (defined(BOARD_USE_LPUART))
    if (CLOCK_CONFIG_INDEX_FOR_VLPR == CLOCK_SYS_GetCurrentConfiguration())
    {
        CLOCK_SYS_SetLpuartSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpuartSrcMcgIrClk);
    }
    else
    {
        CLOCK_SYS_SetLpuartSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpuartSrcIrc48M);
    }
#endif
#if (defined(BOARD_USE_LPSCI))
    if (CLOCK_CONFIG_INDEX_FOR_VLPR == CLOCK_SYS_GetCurrentConfiguration())
    {
         CLOCK_SYS_SetLpsciSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpsciSrcMcgIrClk);
    }
    else
    {
        CLOCK_SYS_SetLpsciSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpsciSrcPllFllSel);
    }
#endif
#if (defined(BOARD_USE_LPSCI))
    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUD, kDebugConsoleLPSCI);
#elif (defined(BOARD_USE_LPUART))
    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUD, kDebugConsoleLPUART);
#else
    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUD, kDebugConsoleUART);
#endif
}

int main(void) {
    demo_power_modes_t testVal;
    uint8_t cmConfigMode = CLOCK_CONFIG_INDEX_FOR_RUN;
    uint8_t mode;
    power_manager_error_code_t ret;
    uint32_t freq = 0;
    rtc_datetime_t date;

    power_manager_user_config_t vlpwConfig;
#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE0 & BOARD_SW_HAS_LLWU_PIN
    power_manager_user_config_t vlls0Config;
#endif
    power_manager_user_config_t vlls1Config;
#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE2
    power_manager_user_config_t vlls2Config;
#endif
    power_manager_user_config_t vlls3Config;
#if FSL_FEATURE_SMC_HAS_LOW_LEAKAGE_STOP_MODE
    power_manager_user_config_t llsConfig;
#endif
    power_manager_user_config_t vlpsConfig;
    power_manager_user_config_t waitConfig;
    power_manager_user_config_t stopConfig;
    power_manager_user_config_t runConfig;
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
    power_manager_user_config_t hsrunConfig;
#endif

    // Example of constant configuration
    // It may save the space in RAM
    const power_manager_user_config_t vlprConfig = {
        .mode = kPowerManagerVlpr,
#if FSL_FEATURE_SMC_HAS_LPWUI
        .lowPowerWakeUpOnInterruptOption = true,
        .lowPowerWakeUpOnInterruptValue = kSmcLpwuiEnabled,
#endif
        .sleepOnExitValue = false,
        .sleepOnExitOption = false
    };

    // Initializes array of pointers to power manager configurations
    power_manager_user_config_t const *powerConfigs[] =
    {
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
      &runConfig,
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
      &hsrunConfig
#endif
    };

    // User callback data
    user_callback_data_t callbackData0;

    // Initializes callback configuration structure for power manager
    const power_manager_callback_user_config_t callbackCfg0 = { callback0,
        kPowerManagerCallbackBeforeAfter,
        (power_manager_callback_data_t*) &callbackData0 };

    // Initializes array of pointers to power manager callbacks
    power_manager_callback_user_config_t const * callbacks[] =
    { &callbackCfg0 };

    // Initializes hardware
    hardware_init();
    // Initializes OS abstraction layer which uses LPTMR HAL layer
    OSA_Init();

    // Initializes debug UART console
    dbg_uart_init();

    // Make the current Clock Manager mode configuration 1 (default configuration)
    /* Set clock configurations to clock manager. */
    CLOCK_SYS_Init(g_defaultClockConfigurations, CLOCK_CONFIG_NUM,
                   &clockCallbackTable, ARRAY_SIZE(clockCallbackTable));

    CLOCK_SYS_UpdateConfiguration(cmConfigMode, kClockManagerPolicyForcible);

    // Using RTC as interrupt source for this demo
#ifndef FRDM_K22F120M
    // Configure RTC pins
    configure_rtc_pins(BOARD_RTC_FUNC_INSTANCE);
#endif
    // select the 1Hz for RTC_CLKOUT
    CLOCK_SYS_SetRtcOutSrc(kClockRtcoutSrc1Hz);

    RTC_DRV_Init(0);

    // Set a start date time and start RTC
    date.year = 2014U;
    date.month = 4U;
    date.day = 30U;
    date.hour = 14U;
    date.minute = 0U;
    date.second = 0U;
    RTC_DRV_SetDatetime(0, &date);

   // Initializes GPIO driver for LEDs and buttons
    GPIO_DRV_Init(switchPins, ledPins);

    memset(&callbackData0, 0, sizeof(user_callback_data_t));

    // initializes configuration structures

    vlpwConfig = vlprConfig;
    vlpwConfig.mode = kPowerManagerVlpw;

#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE0 & BOARD_SW_HAS_LLWU_PIN
    // VLLS0 mode is supported only by some SOCs.
    vlls0Config = vlprConfig;
    vlls0Config.mode = kPowerManagerVlls0;
#endif

    vlls1Config = vlprConfig;
    vlls1Config.mode = kPowerManagerVlls1;

#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE2
    vlls2Config = vlprConfig;
    vlls2Config.mode = kPowerManagerVlls2;
#endif

    vlls3Config = vlprConfig;
    vlls3Config.mode = kPowerManagerVlls3;

#if FSL_FEATURE_SMC_HAS_LOW_LEAKAGE_STOP_MODE
    llsConfig = vlprConfig;
#if FSL_FEATURE_SMC_HAS_LLS_SUBMODE
    // LLS3 mode retains all ram content so CPU wake up doesn't go through restart sequence
    llsConfig.mode = kPowerManagerLls3;
#else
    // classic LLS mode retains all ram content too
    llsConfig.mode = kPowerManagerLls;
#endif
#endif

    vlpsConfig = vlprConfig;
    vlpsConfig.mode = kPowerManagerVlps;

    waitConfig = vlprConfig;
    waitConfig.mode = kPowerManagerWait;

    stopConfig = vlprConfig;
    stopConfig.mode = kPowerManagerStop;

    runConfig.mode = kPowerManagerRun;
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
    hsrunConfig.mode = kPowerManagerHsrun;
#endif

    // initialize power manager driver
    POWER_SYS_Init(&powerConfigs,
    sizeof(powerConfigs)/sizeof(power_manager_user_config_t *),
    &callbacks,
    sizeof(callbacks)/sizeof(power_manager_callback_user_config_t *));

    // Enables LLWU interrupt
#if (defined FRDM_KL03Z48M)
    INT_SYS_EnableIRQ(LLWU_IRQn);
#else
    INT_SYS_EnableIRQ(LLW_IRQn);
#endif
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
    mode = kDemoRun - kDemoMin - 1;
    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
    if (ret != kPowerManagerSuccess)
    {
        printf("POWER_SYS_SetMode(%u) returned unexpected status : %u\n\r",mode,ret);
    }
    else
    {
        updateClockManagerToRunMode(cmConfigMode);
    }
#endif
    while (1)
    {
        mode = 0;

        CLOCK_SYS_GetFreq(kCoreClock, &freq);
        printf("\n\r####################  Power Manager Demo ####################\n\n\r");
        printf("    Core Clock = %luHz \n\r", freq);
        displayPowerMode();

        printf("\n\rSelect the desired operation \n\n\r");
        printf("Press  %c for enter: Wait  - Wait mode\n\r",kDemoWait);
        printf("Press  %c for enter: Stop  - Stop mode\n\r",kDemoStop);
        printf("Press  %c for enter: VLPR  - Very Low Power Run mode\n\r",kDemoVlpr);
        printf("Press  %c for enter: VLPW  - Very Low Power Wait mode\n\r",kDemoVlpw);
        printf("Press  %c for enter: VLPS  - Very Low Power Stop mode\n\r",kDemoVlps);

#if FSL_FEATURE_SMC_HAS_LOW_LEAKAGE_STOP_MODE
#if FSL_FEATURE_SMC_HAS_LLS_SUBMODE
        printf("Press  %c for enter: LLS3  - Low Leakage Stop mode\n\r",kDemoLls);
#else
        printf("Press  %c for enter: LLS   - Low Leakage Stop mode\n\r",kDemoLls);
#endif
#endif

#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE0 & BOARD_SW_HAS_LLWU_PIN
        printf("Press  %c for enter: VLLS0 - Very Low Leakage Stop 0 mode\n\r",kDemoVlls0);
#endif

        printf("Press  %c for enter: VLLS1 - Very Low Leakage Stop 1 mode\n\r",kDemoVlls1);

#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE2
        printf("Press  %c for enter: VLLS2 - Very Low Leakage Stop 2 mode\n\r",kDemoVlls2);
#endif

        printf("Press  %c for enter: VLLS3 - Very Low Leakage Stop 3 mode\n\r",kDemoVlls3);

        printf("Press  %c for enter: RUN   - Normal RUN mode\n\r",kDemoRun);
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
        printf("Press  %c for enter: HSRUN   - High Speed RUN mode\n\r",kDemoHsRun);
#endif

        printf("\n\rWaiting for key press..\n\r\n\r");

        // Wait for user response
        testVal = (demo_power_modes_t)getchar();

        if (testVal > kDemoMin && testVal < kDemoMax)
        {

            mode = testVal - kDemoMin - 1;

            switch (testVal)
            {
                case kDemoWait:
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerVlpr)
                    {
                        printf("Can not go from VLPR to WAIT directly\n\r");
                        break;
                    }
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                    {
                        printf("Can not go from HSRUN to WAIT directly\n\r");
                        break;
                    }
#endif
                    setWakeUpSource(selectWakeUpSource(testVal),"Wait mode");

                    delay(1000); // wait for uart tx fifo complete
                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                    delay(1000);
                    CHECK_RET_VAL(ret, mode);

                    // update Clock Mode
                    updateClockManagerToRunMode(cmConfigMode);
                    break;

                case kDemoStop:
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerVlpr)
                    {
                        printf("Can not go from VLPR to STOP directly\n\r");
                        break;
                    }
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                    {
                        printf("Can not go from HSRUN to STOP directly\n\r");
                        break;
                    }
#endif                    
                    setWakeUpSource(selectWakeUpSource(testVal),"Stop mode");

                    delay(100000); // wait for uart tx fifo complete
                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                    DbgConsole_DeInit();
                    dbg_uart_init();
                    CHECK_RET_VAL(ret, mode);

                    // update Clock Mode
                    update_clock_mode(cmConfigMode);
                    break;

                case kDemoVlpr:
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                    {
                        printf("Can not go from HSRUN to VLPR directly\n\r");
                        break;
                    }
#endif
                    if(kPowerManagerVlpr != POWER_SYS_GetCurrentMode())
                    {
                        /*
                         If apps default CM config mode is not VLPR, but needs to enter VLPR, and real CM config
                         is not VLPR, then we need to update it to VLPR mode here. Otherwise pass through.
                         */
                        if ((cmConfigMode != CLOCK_CONFIG_INDEX_FOR_VLPR) &&
                            (CLOCK_SYS_GetCurrentConfiguration() != CLOCK_CONFIG_INDEX_FOR_VLPR))
                        {
                            CLOCK_SYS_UpdateConfiguration(CLOCK_CONFIG_INDEX_FOR_VLPR, kClockManagerPolicyForcible);
                        }

                        printf("Entering Very Low Power Run mode\n\r");

                        delay(1000); // wait for uart tx fifo complete
                        ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);

                        CHECK_RET_VAL(ret, mode);
                    }
                    else
                    {
                        printf("Very Low Power Run mode already active\n\r");
                    }
                    break;

                case kDemoVlpw:
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerRun)
                    {
                        printf("Can not go from RUN to VLPW directly\n\r");
                        break;
                    }

#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                    {
                        printf("Can not go from HSRUN to VLPW directly\n\r");
                        break;
                    }
#endif

                    setWakeUpSource(selectWakeUpSource(testVal),"Very Low Wait mode");

                    delay(1000); // wait for uart tx fifo complete

                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);

                    if (POWER_SYS_GetCurrentMode() == kPowerManagerRun)
                    {
                        // update Clock Mode to Run
                        updateClockManagerToRunMode(cmConfigMode);
                    }

                    CHECK_RET_VAL(ret, mode);
                    break;

                case kDemoVlps:
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                    {
                        printf("Can not go from HSRUN to VLPS directly\n\r");
                        break;
                    }
#endif
                    setWakeUpSource(selectWakeUpSource(testVal),"Very Low Power Stop mode");

                    delay(100000); // wait for uart tx fifo complete

                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);

                    if (POWER_SYS_GetCurrentMode() == kPowerManagerRun)
                    {
                        // update Clock Mode to Run
                        update_clock_mode(cmConfigMode);
                    }

                    DbgConsole_DeInit();
                    // Select clock source for UART
                    dbg_uart_reinit();
                    CHECK_RET_VAL(ret, mode);
                    break;
#if FSL_FEATURE_SMC_HAS_LOW_LEAKAGE_STOP_MODE
                case kDemoLls:
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                    {
                        printf("Can not go from HSRUN to LLSx directly\n\r");
                        break;
                    }
#endif  
#if FSL_FEATURE_SMC_HAS_LLS_SUBMODE
                    setWakeUpSource(selectWakeUpSource(testVal),"Low Leakage Stop mode 3");              
#else
                    setWakeUpSource(selectWakeUpSource(testVal),"Low Leakage Stop mode");
#endif

                    delay(100000); // wait for uart tx fifo complete

                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);

                    DbgConsole_DeInit();
                    // Select clock source for UART
                    dbg_uart_reinit();

                    // Check the mode LLS was entered
                    if(kPowerManagerVlpr != POWER_SYS_GetCurrentMode())
                    {
                        update_clock_mode(cmConfigMode);
                    }

                    CHECK_RET_VAL(ret, mode);
                    break;
#endif
#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE0 & BOARD_SW_HAS_LLWU_PIN
                case kDemoVlls0:
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                    {
                        printf("Can not go from HSRUN to VLLS0 directly\n\r");
                        break;
                    }
#endif
                    setWakeUpSource(selectWakeUpSource(testVal),"Very Low Leakage Stop 0 mode");
                    printf("Wake up goes through Reset sequence.\n\r");

                    delay(100000); // wait for uart tx fifo complete

                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                    CHECK_RET_VAL(ret, mode);

                    break;
#endif
                case kDemoVlls1:
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                    {
                        printf("Can not go from HSRUN to VLLS1 directly\n\r");
                        break;
                    }
#endif
                    setWakeUpSource(selectWakeUpSource(testVal),"Very Low Leakage Stop 1 mode");
                    printf("Wake up goes through Reset sequence.\n\r");

                    delay(100000); // wait for uart tx fifo complete

                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                    CHECK_RET_VAL(ret, mode);


                    break;

#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE2
                case kDemoVlls2:
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                    {
                        printf("Can not go from HSRUN to VLLS2 directly\n\r");
                        break;
                    }
#endif
                    setWakeUpSource(selectWakeUpSource(testVal),"Very Low Leakage Stop 2 mode");
                    printf("Wake up goes through Reset sequence.\n\r");

                    delay(100000); // wait for uart tx fifo complete

                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                    CHECK_RET_VAL(ret, mode);

                    break;
#endif
                case kDemoVlls3:
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                    {
                        printf("Can not go from HSRUN to VLLS3 directly\n\r");
                        break;
                    }
#endif
                    setWakeUpSource(selectWakeUpSource(testVal),"Very Low Leakage Stop 3 mode");
                    printf("Wake up goes through Reset sequence.\n\r");

                    delay(100000); // wait for uart tx fifo complete

                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                    CHECK_RET_VAL(ret, mode);

                    break;

                case kDemoRun:

                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                    if (ret != kPowerManagerSuccess)
                    {
                        printf("POWER_SYS_SetMode(%u) returned unexpected status : %u\n\r",mode,ret);
                    }
                    else
                    {
                        updateClockManagerToRunMode(cmConfigMode);
                    }
                    break;
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                case kDemoHsRun:
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerVlpr)
                    {
                        printf("Can not go from HSRUN to VLPR directly\n\r");
                        break;
                    }
                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                    if (ret != kPowerManagerSuccess)
                    {
                        printf("POWER_SYS_SetMode(%u) returned unexpected status : %u\n\r",mode,ret);
                    }
                    else
                    {
                        updateClockManagerToHsRunMode(cmConfigMode);
                    }
                    break;
#endif
                default:
                    printf("Wrong value");
                    break;
            }
            printf("\n\rNext loop\n\r");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
