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
#include "fsl_llwu_hal.h"
#include "fsl_smc_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include "fsl_sim_hal.h"
#include "board.h"
#include "fsl_misc_utilities.h"
#include "fsl_uart_driver.h"

#include "fsl_rtc_hal.h"
#include "fsl_device_registers.h"
///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define DEFAULT_ALARM_SECONDS   5
// enum with power manager modes. This modes are supported by the demo
typedef enum demo_power_modes {
    kDemoMin = 'A' -1,
    kDemoRun = 'A',           // Normal RUN mode
    kDemoWait ,
    kDemoStop,
    kDemoVlpr,
    kDemoVlpw,
    kDemoVlps,
    kDemoLls,
    kDemoVlls1,
    kDemoVlls3,
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

// Checking return value then print out error message.
#define CHECK_RET_VAL(ret, mode) \
if (ret != kPowerManagerSuccess) \
{ \
    PRINTF("POWER_SYS_SetMode(%u) returned unexpected status : %u\r\n",mode,ret); \
}
////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////
static volatile bool gSecsFlag;

extern const clock_manager_user_config_t g_defaultClockConfigRun;
extern const clock_manager_user_config_t g_defaultClockConfigVlpr;

const clock_manager_user_config_t * g_defaultClockConfigurations[] = {
    NULL,
    &g_defaultClockConfigVlpr,
    &g_defaultClockConfigRun,
};

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

// static clock callback table.
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
    if (RTC_HAL_HasAlarmOccured(RTC_BASE_PTR))
    {
        RTC_HAL_SetAlarmIntCmd(RTC_BASE_PTR, false);
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
    uint32_t seconds = 0;
    if ((offsetSec < 1) || (offsetSec > 60))
    {
        offsetSec = DEFAULT_ALARM_SECONDS;
    }

    RTC_HAL_GetDatetimeInSecs(RTC_BASE_PTR, &seconds);

    seconds += offsetSec;

    // set the datetime for alarm
    // set alarm in seconds
    RTC_HAL_SetAlarmReg(RTC_BASE_PTR, seconds);

    // Activate or deactivate the Alarm interrupt based on user choice
    RTC_HAL_SetAlarmIntCmd(RTC_BASE_PTR, true);

}
/*!
 * @brief LLW_IRQHandler that would cover the same name's APIs in startup code
 */

void LLWU_IRQHandler(void)
{

    if (RTC_HAL_HasAlarmOccured(RTC))
    {
        RTC_HAL_SetAlarmIntCmd(RTC_BASE_PTR, false);
    }
}

/*!
 * @brief IRQ handler for switch/button.
 */
void BOARD_SW_LLWU_IRQ_HANDLER(void)
{
    PORT_HAL_ClearPortIntFlag(BOARD_SW_LLWU_BASE);
}

/*!
 * @brief Power manager user callback
 */
power_manager_error_code_t callback0(power_manager_notify_struct_t * notify,
                                     power_manager_callback_data_t * dataPtr)
{
    user_callback_data_t * userData = (user_callback_data_t*) dataPtr;
    power_manager_error_code_t ret = kPowerManagerError;
    volatile bool isLastByteTranmistComplete = false;

    switch (notify->notifyType)
    {
        case kPowerManagerNotifyBefore:
            userData->before.counter++;
            do
            {
                isLastByteTranmistComplete = UART_HAL_GetStatusFlag(BOARD_DEBUG_UART_BASEADDR,kUartTxComplete);
            } while (!isLastByteTranmistComplete);

            disable_unused_pins();
            ret = kPowerManagerSuccess;
            break;
        case kPowerManagerNotifyAfter:
            userData->after.counter++;
            enable_unused_pins();
            ret = kPowerManagerSuccess;
            break;
        default:
            userData->err++;
            break;
    }

    userData->lastType = notify->notifyType;

    return ret;
}

/*!
 * @brief Get inup from user about wakeup timeout
 */
uint8_t setWakeUpTimeOut(void)
{
    uint8_t val0;
    uint8_t val1;

    while(1)
    {
        PRINTF("Select the wake up timeout in format DD. Possible decimal value is from range 01 - 60 seconds. Eg. 05 means 5 seconds delay");
        PRINTF("\r\nWaiting for key press..\r\n\r\n");
        val0 = GETCHAR();
        if( (val0 >= '0') && (val0 <= '6') )
        {
            val1 = GETCHAR();
            if( (val1 >= '0') && (val1 <= '9') )
            {
                val0 = (val0-'0')*10 + (val1-'0');
                if( (val0!=0) && (val0<=60) )
                {
                    INT_SYS_DisableIRQGlobal();
                    LLWU_HAL_SetInternalModuleCmd(LLWU_BASE_PTR,kLlwuWakeupModule5,true);
                    INT_SYS_EnableIRQGlobal();
                    cmd_alarm(val0);
                    return val0;
                }
            }
        }

        PRINTF("Wrong value!\r\n");
    }
}
/*!
 * @brief Print on debug console the available wakeup source memu
 */
void printWakeUpSourceText(void)
{
    PRINTF("Select the wake up source:\r\n");
    PRINTF("Press T for RTC - RTC Timer\r\n");
    PRINTF("Press S for switch/button ");
    PRINT_LLWU_SW_NUM;
    PRINTF("\r\n");
}

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
            case kDemoLls:
            case kDemoVlls1:
            case kDemoVlls3:
                // switch is not connected to LLWU, we can use only RTC wake up
                PRINTF("The board does not support wake up from this mode by switch/button. RTC is used as wake up source:\r\n");
                return 0;
            default:
                // default is RTC
                return 0;
        }
        PRINTF("\r\nWaiting for key press..\r\n\r\n");
        // Wait for user response
        testVal = GETCHAR();

        if ((testVal >= 'a') && (testVal <= 'z'))
        {
        testVal -= 'a' - 'A';
        }

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
            PRINTF("Wrong value!\r\n");
        }
    }
}

void llwuDisableWakeUp(void)
{
    LLWU_HAL_SetInternalModuleCmd(LLWU_BASE_PTR,kLlwuWakeupModule5,false);
}

void gpioDisableWakeUp(void)
{
    // disables interrupt
    PORT_HAL_SetPinIntMode(BOARD_SW_LLWU_BASE, BOARD_SW_LLWU_PIN, kPortIntDisabled);
    INT_SYS_DisableIRQ(BOARD_SW_LLWU_IRQ_NUM);

}

void gpioEnableWakeUp(void)
{
    // enables falling edge interrupt for switch SWx
    PORT_HAL_SetPinIntMode(BOARD_SW_LLWU_BASE, BOARD_SW_LLWU_PIN, kPortIntFallingEdge);
    INT_SYS_EnableIRQ(BOARD_SW_LLWU_IRQ_NUM);

}

void setWakeUpSource(uint8_t source,char *textMode)
{
    uint8_t timeout;

    if(!source)
    {
        gpioDisableWakeUp();
        // wake up on RTC interrupt
        timeout = setWakeUpTimeOut();
        PRINTF("Entering %s, will wake up on RTC interrupt after %u seconds\r\n",textMode,timeout);
    }
    else
    {
        llwuDisableWakeUp();
        // wake up on gpio interrupt from button(s)
        gpioEnableWakeUp();
        PRINTF("Entering %s, press the ",textMode);
        PRINT_LLWU_SW_NUM;
        PRINTF(" button to wake up.\r\n");
    }
}

void displayPowerMode(void)
{
    uint32_t smcMode = SMC_HAL_GetStat(SMC_BASE_PTR);

    switch (smcMode)
    {
    case kStatRun:
        PRINTF("    SMC mode = kStatRun\r\n");
        break;
    case kStatVlpr:
        PRINTF("    SMC mode = kPowerModeVlpr\r\n");
        break;

    default:
        PRINTF("    SMC mode = Invalid mode\r\n");
        break;
    }
}

/* Update clock */
void update_clock_mode(uint8_t cmConfigMode)
{
    CLOCK_SYS_UpdateConfiguration(cmConfigMode, kClockManagerPolicyForcible);
}

/* Re-init uart module. */
void dbg_uart_reinit()
{
    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_LOW_POWER_UART_BAUD, kDebugConsoleUART);
}

/*!
 * @brief main demo function.
 */
int main(void) {
    demo_power_modes_t testVal = kDemoRun;
    uint8_t mode;
    power_manager_error_code_t ret = kPowerManagerSuccess;
    uint32_t freq = 0;
    rtc_datetime_t date =
    {
        .year = 2014U,
        .month = 4U,
        .day = 30U,
        .hour = 14U,
        .minute = 0U,
        .second = 0U,
    };

    // Example of constant configuration
    // It may save the space in RAM
    const power_manager_user_config_t vlprConfig = {
        .mode = kPowerManagerVlpr,
        .sleepOnExitValue = false,
    };
    power_manager_user_config_t vlpwConfig     =    vlprConfig;
    power_manager_user_config_t vlls1Config    =    vlprConfig;
    power_manager_user_config_t vlls3Config    =    vlprConfig;
    power_manager_user_config_t llsConfig      =    vlprConfig;
    power_manager_user_config_t vlpsConfig     =    vlprConfig;
    power_manager_user_config_t waitConfig     =    vlprConfig;
    power_manager_user_config_t stopConfig     =    vlprConfig;
    power_manager_user_config_t runConfig      =    vlprConfig;

    // Initializes array of pointers to power manager configurations
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

    // User callback data
    user_callback_data_t callbackData0;

    // Initializes callback configuration structure for power manager
    power_manager_callback_user_config_t callbackCfg0 = { callback0,
        kPowerManagerCallbackBeforeAfter,
        (power_manager_callback_data_t*) &callbackData0 };

    // Initializes array of pointers to power manager callbacks
    power_manager_callback_user_config_t * callbacks[] =
    { &callbackCfg0 };

    // Initializes hardware
    hardware_init();
    // Make the current Clock Manager mode configuration 1 (default configuration)
    /* Set clock configurations to clock manager. */
    CLOCK_SYS_Init(g_defaultClockConfigurations, CLOCK_NUMBER_OF_CONFIGURATIONS,
                   clockCallbackTable, ARRAY_SIZE(clockCallbackTable));

    CLOCK_SYS_UpdateConfiguration(CLOCK_RUN, kClockManagerPolicyForcible);
    // select the 1Hz for RTC_CLKOUT
    CLOCK_SYS_SetRtcOutSrc(kClockRtcoutSrc1Hz);

    /* Enable clock gate to RTC module */
    CLOCK_SYS_EnableRtcClock( 0U);

    /* Initialize the general configuration for RTC module.*/
    RTC_HAL_Init(RTC_BASE_PTR);

    /* Enable the RTC Clock output */
    RTC_HAL_SetClockOutCmd(RTC_BASE_PTR, true);

    NVIC_ClearPendingIRQ(RTC_IRQn);

    INT_SYS_EnableIRQ(RTC_IRQn);

    //RTC_DRV_SetDatetime(0, &date);
    RTC_HAL_SetDatetime(RTC_BASE_PTR, &date);
   // Initializes GPIO driver for LEDs and buttons
    GPIO_DRV_Init(switchPins, ledPins);

    memset(&callbackData0, 0, sizeof(user_callback_data_t));

    // initializes configuration structures
    vlpwConfig.mode = kPowerManagerVlpw;
    vlls1Config.mode = kPowerManagerVlls1;
    vlls3Config.mode = kPowerManagerVlls3;

    // classic LLS mode retains all ram content too
    llsConfig.mode = kPowerManagerLls;

    vlpsConfig.mode = kPowerManagerVlps;

    waitConfig.mode = kPowerManagerWait;

    stopConfig.mode = kPowerManagerStop;

    runConfig.mode  = kPowerManagerRun;

    // initialize power manager driver
    POWER_SYS_Init(powerConfigs,
    sizeof(powerConfigs)/sizeof(power_manager_user_config_t *),
    callbacks,
    sizeof(callbacks)/sizeof(power_manager_callback_user_config_t *));

    // Enables LLWU interrupt
    INT_SYS_EnableIRQ(LLWU_IRQn);

    while (1)
    {
        mode = 0;

        CLOCK_SYS_GetFreq(kCoreClock, &freq);
        PRINTF("\r\n####################  Power Manager Demo ####################\r\n\r\n");
        PRINTF("    Core Clock = %dHz \r\n", freq);
        displayPowerMode();

        PRINTF("\r\nSelect the desired operation \r\n\r\n");
        PRINTF("Press  %c for enter: RUN   - Normal RUN mode\r\n",kDemoRun);
        PRINTF("Press  %c for enter: Wait  - Wait mode\r\n",kDemoWait);
        PRINTF("Press  %c for enter: Stop  - Stop mode\r\n",kDemoStop);
        PRINTF("Press  %c for enter: VLPR  - Very Low Power Run mode\r\n",kDemoVlpr);
        PRINTF("Press  %c for enter: VLPW  - Very Low Power Wait mode\r\n",kDemoVlpw);
        PRINTF("Press  %c for enter: VLPS  - Very Low Power Stop mode\r\n",kDemoVlps);

        PRINTF("Press  %c for enter: LLS   - Low Leakage Stop mode\r\n",kDemoLls);
        PRINTF("Press  %c for enter: VLLS1 - Very Low Leakage Stop 1 mode\r\n",kDemoVlls1);
        PRINTF("Press  %c for enter: VLLS3 - Very Low Leakage Stop 3 mode\r\n",kDemoVlls3);

        PRINTF("\r\nWaiting for key press..\r\n\r\n");

        // Wait for user response
        testVal = (demo_power_modes_t)GETCHAR();

        if ((testVal >= 'a') && (testVal <= 'z'))
        {
            testVal -= 'a' - 'A';
        }

        if (testVal > kDemoMin && testVal < kDemoMax)
        {

            mode = testVal - kDemoMin - 1;

            switch (testVal)
            {
                case kDemoWait:
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerVlpr)
                    {
                        PRINTF("Can not go from VLPR to WAIT directly\r\n");
                        break;
                    }
                    setWakeUpSource(selectWakeUpSource(testVal),"Wait mode");

                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                    CHECK_RET_VAL(ret, mode);

                    break;

                case kDemoStop:
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerVlpr)
                    {
                        PRINTF("Can not go from VLPR to STOP directly\r\n");
                        break;
                    }
                    setWakeUpSource(selectWakeUpSource(testVal),"Stop mode");

                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);

                    CHECK_RET_VAL(ret, mode);

                    // update Clock Mode
                    update_clock_mode(CLOCK_RUN);
                    break;

                case kDemoVlpr:
                    if(kPowerManagerVlpr != POWER_SYS_GetCurrentMode())
                    {
                        update_clock_mode(CLOCK_VLPR);
                        PRINTF("Entering Very Low Power Run mode\r\n");
                        ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                        CHECK_RET_VAL(ret, mode);
                    }
                    else
                    {
                        PRINTF("Very Low Power Run mode already active\r\n");
                    }
                    break;

                case kDemoVlpw:
                    if (POWER_SYS_GetCurrentMode() == kPowerManagerRun)
                    {
                        PRINTF("Can not go from RUN to VLPW directly\r\n");
                        break;
                    }
                    setWakeUpSource(selectWakeUpSource(testVal),"Very Low Wait mode");
                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                    CHECK_RET_VAL(ret, mode);
                    break;

                case kDemoVlps:
                    setWakeUpSource(selectWakeUpSource(testVal),"Very Low Power Stop mode");
                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);

                    if (POWER_SYS_GetCurrentMode() == kPowerManagerRun)
                    {
                        // update Clock Mode to Run
                        update_clock_mode(CLOCK_RUN);
                    }

                    CHECK_RET_VAL(ret, mode);
                    break;
                case kDemoLls:

                    setWakeUpSource(selectWakeUpSource(testVal),"Low Leakage Stop mode");
                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);

                    // Check the mode LLS was entered
                    if(kPowerManagerVlpr != POWER_SYS_GetCurrentMode())
                    {
                        update_clock_mode(CLOCK_RUN);
                    }

                    CHECK_RET_VAL(ret, mode);
                    break;
                case kDemoVlls1:
                    setWakeUpSource(selectWakeUpSource(testVal),"Very Low Leakage Stop 1 mode");
                    PRINTF("Wake up goes through Reset sequence.\r\n");
                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                    CHECK_RET_VAL(ret, mode);
                    break;

                case kDemoVlls3:
                    setWakeUpSource(selectWakeUpSource(testVal),"Very Low Leakage Stop 3 mode");
                    PRINTF("Wake up goes through Reset sequence.\r\n");
                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                    CHECK_RET_VAL(ret, mode);

                    break;

                case kDemoRun:
                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);

                    if (ret != kPowerManagerSuccess)
                    {
                        PRINTF("POWER_SYS_SetMode(%u) returned unexpected status : %u\r\n",mode,ret);
                    }
                    else
                    {
                        update_clock_mode(CLOCK_RUN);
                    }
                    break;
                default:
                    PRINTF("Wrong value");
                    break;
            }
            PRINTF("\r\nNext loop\r\n");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
