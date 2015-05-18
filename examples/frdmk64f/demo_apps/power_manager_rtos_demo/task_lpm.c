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
#include <string.h>

// SDK Included Files
#include "board.h"
#include "lpm_rtos.h"
#include "fsl_uart_driver.h"
#include "fsl_os_abstraction.h"
#include "fsl_interrupt_manager.h"
#include "fsl_gpio_driver.h"
#include "fsl_llwu_hal.h"
#include "fsl_smc_hal.h"
#include "fsl_power_manager.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include "fsl_sim_hal.h"
#include "fsl_misc_utilities.h"

// Project Included Files
#include "task_lpm.h"

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
extern const clock_manager_user_config_t g_defaultClockConfigRun;
extern const clock_manager_user_config_t g_defaultClockConfigVlpr;
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
extern const clock_manager_user_config_t g_defaultClockConfigHsrun;
#endif

const clock_manager_user_config_t * g_defaultClockConfigurations[] = {
    NULL,
    &g_defaultClockConfigVlpr,
    &g_defaultClockConfigRun,
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
    &g_defaultClockConfigHsrun,
#endif
};

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/******************************************************************************
 * function
 ******************************************************************************/
/*!
 * @brief set alarm command.
 *
 * This function set the alarm which will be
 * trigerred x secs later. The alarm trigger
 * will print a notification on the console.
 */
void cmdAlarm(wakeUpSource_t wus, uint8_t offsetSec)
{
    if((wus & wakeUpSourceRtc) != 0)
    {
#if (defined PM_RTOS_DEMO_RTC_FUNC_INSTANCE)
        rtcSetAlarm(PM_RTOS_DEMO_RTC_FUNC_INSTANCE, offsetSec);
#endif
    }
    else
    {
        lptmrSetAlarm(offsetSec);
    }
}

/*!
 * @brief get wake up source from given character.
 */
wakeUpSource_t getWakeupSource(uint8_t val, demo_power_modes_t mode)
{
    wakeUpSource_t ret = wakeUpSourceErr;
#if (defined PM_RTOS_DEMO_RTC_FUNC_INSTANCE)
    if(val == 'T' || val == 't')
    {
        ret = wakeUpSourceRtc;
    }
#endif
    if(val == 'L' || val == 'l')
    {
        ret = wakeUpSourceLptmr;
    }
#if BOARD_SW_HAS_LLWU_PIN
    if(val == 'S' || val == 's')
    {
        ret =  wakeUpSourceSwBtn;
    }
#else
    if(val == 'S' || val == 's')
    {
        switch (mode)
        {
            case kDemoWait:
            case kDemoVlpr:
            case kDemoVlpw:
            case kDemoStop:
            case kDemoVlps:
                ret = wakeUpSourceSwBtn;
                break;
            default:
                ret = wakeUpSourceErr;
                break;
        }
    }
#endif
    return ret;
}

/* LLW_IRQHandler that would cover the same name's APIs in startup code */
void MODULE_IRQ_HANDLER(LLWU)(void)
{
    // The LLWU wakeup interrup is LPTMR source
    if (LLWU_HAL_GetInternalModuleWakeupFlag(LLWU_BASE_PTR, PM_RTOS_DEMO_LPTMR_LLWU_WAKEUP_MODULE))
    {
        LPTMR_HAL_ClearIntFlag(g_lptmrBase[PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE]);
    }

    // The LLWU wakeup interrup is RTC source
#if (FSL_FEATURE_LLWU_HAS_INTERNAL_MODULE) && (defined PM_RTOS_DEMO_RTC_FUNC_INSTANCE)
    if (LLWU_HAL_GetInternalModuleWakeupFlag(LLWU_BASE_PTR, PM_RTOS_DEMO_RTC_LLWU_WAKEUP_MODULE))
    {
        RTC_DRV_SetAlarmIntCmd(PM_RTOS_DEMO_RTC_FUNC_INSTANCE, false);
    }
#endif

    // The LLWU wakeup interrup is Switch/Button source
#if BOARD_SW_HAS_LLWU_PIN
    if (LLWU_HAL_GetExternalPinWakeupFlag(LLWU_BASE_PTR,(llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN))
    {
         LLWU_HAL_ClearExternalPinWakeupFlag(LLWU_BASE_PTR, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
    }
#endif
}

/* IRQ handler for switch/button. */
void BOARD_SW_LLWU_IRQ_HANDLER(void)
{
    PORT_HAL_ClearPortIntFlag(BOARD_SW_LLWU_BASE);
}

uint8_t setWakeUpTimeOut(wakeUpSource_t wus)
{
    uint8_t val0;
    uint8_t val1;

    while(1)
    {
        PRINTF("Select the wake up timeout in format DD. Possible decimal value is from range 01 - 60 seconds. Eg. 05 means 5 seconds delay");
        PRINTF("\n\rWaiting for key press..\n\r\n\r");
        val0 = getInput();
        PRINTF("You pressed: '%c", val0);
        if( (val0 >= '0') && (val0 <= '6') )
        {
            val1 = getInput();
            PRINTF("%c'\r\n", val1);
            if( (val1 >= '0') && (val1 <= '9') )
            {
                val0 = (val0-'0')*10 + (val1-'0');
                if( (val0!=0) && (val0<=60) )
                {
#if FSL_FEATURE_LLWU_HAS_INTERNAL_MODULE
                    OSA_EnterCritical(kCriticalDisableInt);
#if (defined PM_RTOS_DEMO_RTC_FUNC_INSTANCE)
                    if((wus & wakeUpSourceRtc) != 0)
                    {
                        LLWU_HAL_SetInternalModuleCmd(LLWU_BASE_PTR,PM_RTOS_DEMO_RTC_LLWU_WAKEUP_MODULE,true);
                    }
#endif
                    if((wus & wakeUpSourceLptmr) != 0)
                    {
                        LLWU_HAL_SetInternalModuleCmd(LLWU_BASE_PTR,PM_RTOS_DEMO_LPTMR_LLWU_WAKEUP_MODULE,true);
                    }
                    OSA_ExitCritical(kCriticalDisableInt);
#endif
                    cmdAlarm(wus, val0);
                    return val0;
                }
            }
        }

        PRINTF("Wrong value!\n\r");
    }
}

void printWakeUpSourceText(wakeUpSource_t wus)
{
    PRINTF("Select the wake up source:\n\r");
    if((wus & wakeUpSourceRtc) != 0)
    {
        PRINTF("Press T for RTC - RTC Timer\n\r");
    }
    if((wus & wakeUpSourceLptmr) != 0)
    {
        PRINTF("Press L for LPTMR - LPTMR Timer\n\r");
    }
    if((wus & wakeUpSourceSwBtn) != 0)
    {
        PRINTF("Press S for switch/button ");
        PRINT_LLWU_SW_NUM;
        PRINTF("\n\r");
    }
}

/* TWR_K60D100M doesn't have switch pins connected to LLWU.
 * It's not possible to wake up by SWx buttons from some modes.
 * returns 0 when RTC is selected as wake up source
 * returns 1 when sw pins are selected as wake up source
 */
wakeUpSource_t selectWakeUpSource(demo_power_modes_t mode)
{
    uint8_t val = 0;
    wakeUpSource_t retval = wakeUpSourceErr;
    wakeUpSource_t wus = (wakeUpSource_t)(wakeUpSourceRtc | wakeUpSourceSwBtn | wakeUpSourceLptmr);
    while(retval == wakeUpSourceErr)
    {
        switch(mode)
        {
            case kDemoWait:
            case kDemoVlpr:
            case kDemoVlpw:

            case kDemoStop:
            case kDemoVlps:
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
                break;
#else
                // switch is not connected to LLWU, we can use only RTC wake up
                PRINTF("The board does not support wake up from this mode by switch/button. Timer is used as wake up source:\n\r");
                wus &= ~wakeUpSourceSwBtn;
                break;
#endif
#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE0 & BOARD_SW_HAS_LLWU_PIN
            case kDemoVlls0:
                // in VLLS0 LPO oscillator is disabled.
                PRINTF("The board does not support wake up from this mode by Timer due to disabled LPO.\n\r");
                wus &= ~(wakeUpSourceRtc | wakeUpSourceLptmr);
                break;
#endif
            default:
                break;
        }
        PRINTF("\n\rWaiting for key press..\n\r\n\r");
#if (!defined PM_RTOS_DEMO_RTC_FUNC_INSTANCE)
        // Clear RTC option.
        wus &= (~wakeUpSourceRtc);
#endif
        if(wus == wakeUpSourceRtc || wus == wakeUpSourceLptmr || wus == wakeUpSourceSwBtn)
        {
            return wus;
        }
        printWakeUpSourceText(wus);
        // Wait for user response
        val = getInput();
        PRINTF("You pressed: '%c'\r\n", val);
        retval = getWakeupSource(val, mode);
    }

    return retval;
}

#if FSL_FEATURE_LLWU_HAS_INTERNAL_MODULE
void llwuDisableWakeUp(void)
{
#if (defined PM_RTOS_DEMO_RTC_FUNC_INSTANCE)
    LLWU_HAL_SetInternalModuleCmd(LLWU_BASE_PTR, PM_RTOS_DEMO_RTC_LLWU_WAKEUP_MODULE, false);
#endif
    LLWU_HAL_SetInternalModuleCmd(LLWU_BASE_PTR, PM_RTOS_DEMO_LPTMR_LLWU_WAKEUP_MODULE, false);
}
#endif

void gpioDisableWakeUp(void)
{
    // disables interrupt
    PORT_HAL_SetPinIntMode(BOARD_SW_LLWU_BASE, BOARD_SW_LLWU_PIN, kPortIntDisabled);
    INT_SYS_DisableIRQ(BOARD_SW_LLWU_IRQ_NUM);

#if BOARD_SW_HAS_LLWU_PIN
    LLWU_HAL_ClearExternalPinWakeupFlag(LLWU_BASE_PTR, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
    LLWU_HAL_SetExternalInputPinMode(LLWU_BASE_PTR,kLlwuExternalPinDisabled, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
#endif
}

void gpioEnableWakeUp(void)
{
    // enables falling edge interrupt for switch SWx
    PORT_HAL_SetPinIntMode(BOARD_SW_LLWU_BASE, BOARD_SW_LLWU_PIN, kPortIntFallingEdge);
    INT_SYS_EnableIRQ(BOARD_SW_LLWU_IRQ_NUM);

#if BOARD_SW_HAS_LLWU_PIN
    LLWU_HAL_ClearExternalPinWakeupFlag(LLWU_BASE_PTR, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
    LLWU_HAL_SetExternalInputPinMode(LLWU_BASE_PTR,kLlwuExternalPinFallingEdge, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
#endif
}

void setWakeUpSource(wakeUpSource_t wus,char *textMode)
{
    uint8_t timeout;

    if((wus & (wakeUpSourceRtc | wakeUpSourceLptmr)) != 0)
    {
        gpioDisableWakeUp();
        // Wake up on timer's interrupt
        timeout = setWakeUpTimeOut(wus);
        PRINTF("Entering %s, will wake up after %u seconds\n\r",textMode,timeout);
    }
    else if((wus & wakeUpSourceSwBtn) != 0)
    {
#if FSL_FEATURE_LLWU_HAS_INTERNAL_MODULE
        llwuDisableWakeUp();
#endif
        // Wake up on gpio interrupt from button(s)
        gpioEnableWakeUp();
        PRINTF("Entering %s, press the ",textMode);
        PRINT_LLWU_SW_NUM;
        PRINTF(" button to wake up.\n\r");
    }
    else
    {
        PRINTF("Unknown error.\n\r");
    }
}

void displayPowerMode(void)
{
    uint32_t smcMode = SMC_HAL_GetStat(SMC_BASE_PTR);

    switch (smcMode)
    {
    case kStatRun:
        PRINTF("    SMC mode = kPowerModeRun\n\r");
        break;
    case kStatVlpr:
        PRINTF("    SMC mode = kPowerModeVlpr\n\r");
        break;
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
    case kStatHsrun:
        PRINTF("    SMC mode = kPowerModeHsrun\n\r");
        break;
#endif
    default:
        PRINTF("    SMC mode = Unknown!\n\r");
        break;
    }
}

void updateClockManagerToRunMode(uint8_t cmConfigMode)
{
    // if current config mode is RUN but CM is not, need to re-config it to RUN
    if ((cmConfigMode == CLOCK_RUN) &&
        (CLOCK_SYS_GetCurrentConfiguration() != CLOCK_RUN))
    {
        CLOCK_SYS_UpdateConfiguration(CLOCK_RUN, kClockManagerPolicyForcible);
    }
}

void updateClockManagerToVlprMode(uint8_t cmConfigMode)
{
    // if current config mode and CM are both RUN, need to re-config it to VLPR
    if ((cmConfigMode == CLOCK_RUN) &&
        (CLOCK_SYS_GetCurrentConfiguration() == CLOCK_RUN))
    {
        CLOCK_SYS_UpdateConfiguration(CLOCK_VLPR, kClockManagerPolicyForcible);
    }
}

#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
void updateClockManagerToHsRunMode(uint8_t cmConfigMode)
{
    // if current config mode and CM are both RUN, need to re-config it to HSRUN
    if ((cmConfigMode == CLOCK_RUN) &&
        (CLOCK_SYS_GetCurrentConfiguration() == CLOCK_RUN))
    {
        CLOCK_SYS_UpdateConfiguration(CLOCK_HSRUN, kClockManagerPolicyForcible);
    }
}
#endif

/* Update clock to compatible with RUN mode. */
void updateClockMode(uint8_t cmConfigMode)
{
#if (defined(FSL_FEATURE_MCGLITE_MCGLITE))
    if (g_defaultClockConfigurations[cmConfigMode]->mcgliteConfig.mcglite_mode == kMcgliteModeHirc48M)
#else
    if (g_defaultClockConfigurations[cmConfigMode]->mcgConfig.mcg_mode == kMcgModePEE)
#endif
    {
        CLOCK_SYS_UpdateConfiguration(CLOCK_RUN, kClockManagerPolicyForcible);
    }
    else
    {
        updateClockManagerToRunMode(cmConfigMode);
    }
}

/******************************************************************************
 * Golobal function
 ******************************************************************************/
/* little delay for uart flush */
void delay(uint32_t delay_time)
{
    uint32_t wait;
    for(wait=0;wait<delay_time;wait++)
    {
        __asm("NOP");
    }
}

void task_lpm(task_param_t param)
{
    demo_power_modes_t testVal;
    uint8_t cmConfigMode = CLOCK_RUN;
    uint8_t mode;
    power_manager_error_code_t ret;
    uint32_t freq = 0;

#if (defined PM_RTOS_DEMO_RTC_FUNC_INSTANCE)
    rtc_datetime_t date;
    memset(&date, 0, sizeof(rtc_datetime_t));
#endif
    memset(&cmCallbackData, 0, sizeof(lptmrStructure_t));
    cmCallbackData.instance = PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE;
    lptmr_user_config_t *lptmrUserConfig = &(cmCallbackData.lptmrUserConfig);
    lptmr_state_t *lptmrState = &(cmCallbackData.lptmrState);

    CLOCK_SYS_Init(g_defaultClockConfigurations,
                   CLOCK_NUMBER_OF_CONFIGURATIONS,
                   cm_callback_tbl,
                   cm_callback_tbl_size);

    CLOCK_SYS_UpdateConfiguration(cmConfigMode, kClockManagerPolicyForcible);

#if (defined PM_RTOS_DEMO_RTC_FUNC_INSTANCE)
    // Set a start date time and start RTC
    date.year = 2014U;
    date.month = 4U;
    date.day = 30U;
    date.hour = 14U;
    date.minute = 0U;
    date.second = 0U;
    rtcInit(PM_RTOS_DEMO_RTC_FUNC_INSTANCE, &date);
#endif
    lptmrUserConfig->timerMode = kLptmrTimerModeTimeCounter; // Use LPTMR in Time Counter mode
    lptmrUserConfig->freeRunningEnable = false; // When hit compare value, set counter back to zero
    lptmrUserConfig->prescalerEnable = false; // bypass prescaler
    lptmrUserConfig->prescalerClockSource = kClockLptmrSrcLpoClk; // use 1kHz Low Power Clock
    lptmrUserConfig->isInterruptEnabled = false;
    lptmrInit(lptmrUserConfig, lptmrState);

    // initialize power manager driver
    POWER_SYS_Init(powerConfigs,
    powerConfigsSize,
    pm_callback_tbl,
    pm_callback_tbl_size);

    // Enables LLWU interrupt
    INT_SYS_EnableIRQ(LLWU_IRQn);

#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
    mode = kDemoRun - kDemoMin - 1;
    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
    if (ret != kPowerManagerSuccess)
    {
        PRINTF("POWER_SYS_SetMode(%u) returned unexpected status : %u\n\r",mode,ret);
    }
    else
    {
        updateClockManagerToRunMode(cmConfigMode);
    }
#endif
#if (defined FSL_RTOS_BM)
    PRINTF("\n\r####################  Power Manager BM Demo ####################\n\n\r");
#elif (defined FSL_RTOS_FREE_RTOS)
    PRINTF("\n\r####################  Power Manager FreeRTOS Demo ####################\n\n\r");
#elif (defined FSL_RTOS_MQX)
    PRINTF("\n\r####################  Power Manager MQX Demo ####################\n\n\r");
#elif (defined FSL_RTOS_UCOSII)
    PRINTF("\n\r####################  Power Manager Ucos2 Demo ####################\n\n\r");
#elif (defined FSL_RTOS_UCOSIII)
    PRINTF("\n\r####################  Power Manager Ucos3 Demo ####################\n\n\r");
#else
    PRINTF("\n\rUnknown RTOS\n\n\r");
#endif

    while (1)
    {
        mode = 0;
        CLOCK_SYS_GetFreq(kCoreClock, &freq);
        PRINTF("    Core Clock = %luHz \n\r", freq);
        displayPowerMode();
        PRINTF("\n\rSelect the desired operation \n\n\r");
        PRINTF("Press  %c for enter: RUN   - Normal RUN mode\n\r",kDemoRun);
        PRINTF("Press  %c for enter: Wait  - Wait mode\n\r",kDemoWait);
        PRINTF("Press  %c for enter: Stop  - Stop mode\n\r",kDemoStop);
        PRINTF("Press  %c for enter: VLPR  - Very Low Power Run mode\n\r",kDemoVlpr);
        PRINTF("Press  %c for enter: VLPW  - Very Low Power Wait mode\n\r",kDemoVlpw);
        PRINTF("Press  %c for enter: VLPS  - Very Low Power Stop mode\n\r",kDemoVlps);

#if FSL_FEATURE_SMC_HAS_LOW_LEAKAGE_STOP_MODE
#if FSL_FEATURE_SMC_HAS_LLS_SUBMODE
        PRINTF("Press  %c for enter: LLS3  - Low Leakage Stop mode\n\r",kDemoLls);
#else
        PRINTF("Press  %c for enter: LLS   - Low Leakage Stop mode\n\r",kDemoLls);
#endif
#endif

#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE0 & BOARD_SW_HAS_LLWU_PIN
        PRINTF("Press  %c for enter: VLLS0 - Very Low Leakage Stop 0 mode\n\r",kDemoVlls0);
#endif

        PRINTF("Press  %c for enter: VLLS1 - Very Low Leakage Stop 1 mode\n\r",kDemoVlls1);

#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE2
        PRINTF("Press  %c for enter: VLLS2 - Very Low Leakage Stop 2 mode\n\r",kDemoVlls2);
#endif

        PRINTF("Press  %c for enter: VLLS3 - Very Low Leakage Stop 3 mode\n\r",kDemoVlls3);
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
        PRINTF("Press  %c for enter: HSRUN   - High Speed RUN mode\n\r",kDemoHsRun);
#endif
        PRINTF("Press  %c to get current chip temperature\n\r",KDemoADC);

        PRINTF("------------------------------------------------------------\n\r");
        PRINTF("\n\rWaiting for key press..\n\r\n\r");

        // Wait for user response
        testVal = (demo_power_modes_t)getInput();
        PRINTF("You pressed: '%c'\r\n", testVal);

        // convert lower to upper character.
        if(testVal > kDemoMax)
        {
            testVal = (demo_power_modes_t)(testVal + 'A' - 'a');
        }

        mode = testVal - kDemoMin - 1;

        switch (testVal)
        {
            case kDemoWait:
                if (POWER_SYS_GetCurrentMode() == kPowerManagerVlpr)
                {
                    PRINTF("Can not go from VLPR to WAIT directly\n\r");
                    break;
                }
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                {
                    PRINTF("Can not go from HSRUN to WAIT directly\n\r");
                    break;
                }
#endif
                setWakeUpSource(selectWakeUpSource(testVal),"Wait mode");

                ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                CHECK_RET_VAL(ret, mode);

                // update Clock Mode
                updateClockManagerToRunMode(cmConfigMode);
                break;

            case kDemoStop:
                if (POWER_SYS_GetCurrentMode() == kPowerManagerVlpr)
                {
                    PRINTF("Can not go from VLPR to STOP directly\n\r");
                    break;
                }
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                {
                    PRINTF("Can not go from HSRUN to STOP directly\n\r");
                    break;
                }
#endif
                setWakeUpSource(selectWakeUpSource(testVal),"Stop mode");

                ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                CHECK_RET_VAL(ret, mode);

                // update Clock Mode
                updateClockMode(cmConfigMode);
                break;

            case kDemoVlpr:
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                {
                    PRINTF("Can not go from HSRUN to VLPR directly\n\r");
                    break;
                }
#endif
                if(kPowerManagerVlpr != POWER_SYS_GetCurrentMode())
                {
                    /*
                     If apps default CM config mode is not VLPR, but needs to enter VLPR, and real CM config
                     is not VLPR, then we need to update it to VLPR mode here. Otherwise pass through.
                     */
                    updateClockManagerToVlprMode(cmConfigMode);
                    PRINTF("Entering Very Low Power Run mode\n\r");

                    ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);

                    CHECK_RET_VAL(ret, mode);
                }
                else
                {
                    PRINTF("Very Low Power Run mode already active\n\r");
                }
                break;

            case kDemoVlpw:
                if (POWER_SYS_GetCurrentMode() == kPowerManagerRun)
                {
                    PRINTF("Can not go from RUN to VLPW directly\n\r");
                    break;
                }

#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                {
                    PRINTF("Can not go from HSRUN to VLPW directly\n\r");
                    break;
                }
#endif

                setWakeUpSource(selectWakeUpSource(testVal),"Very Low Wait mode");

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
                    PRINTF("Can not go from HSRUN to VLPS directly\n\r");
                    break;
                }
#endif
                setWakeUpSource(selectWakeUpSource(testVal),"Very Low Power Stop mode");

                ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);

                if (POWER_SYS_GetCurrentMode() == kPowerManagerRun)
                {
                    // update Clock Mode to Run
                    updateClockMode(cmConfigMode);
                }

                CHECK_RET_VAL(ret, mode);
                break;
#if FSL_FEATURE_SMC_HAS_LOW_LEAKAGE_STOP_MODE
            case kDemoLls:
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                {
                    PRINTF("Can not go from HSRUN to LLSx directly\n\r");
                    break;
                }
#endif
#if FSL_FEATURE_SMC_HAS_LLS_SUBMODE
                setWakeUpSource(selectWakeUpSource(testVal),"Low Leakage Stop mode 3");
#else
                setWakeUpSource(selectWakeUpSource(testVal),"Low Leakage Stop mode");
#endif
                ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);

                // Check the mode LLS was entered
                if(kPowerManagerVlpr != POWER_SYS_GetCurrentMode())
                {
                    updateClockMode(cmConfigMode);
                }

                CHECK_RET_VAL(ret, mode);
                break;
#endif
#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE0 & BOARD_SW_HAS_LLWU_PIN
            case kDemoVlls0:
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                {
                    PRINTF("Can not go from HSRUN to VLLS0 directly\n\r");
                    break;
                }
#endif
                setWakeUpSource(selectWakeUpSource(testVal),"Very Low Leakage Stop 0 mode");
                PRINTF("Wake up goes through Reset sequence.\n\r");

                ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                CHECK_RET_VAL(ret, mode);

                break;
#endif
            case kDemoVlls1:
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                {
                    PRINTF("Can not go from HSRUN to VLLS1 directly\n\r");
                    break;
                }
#endif
                setWakeUpSource(selectWakeUpSource(testVal),"Very Low Leakage Stop 1 mode");
                PRINTF("Wake up goes through Reset sequence.\n\r");

                ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                CHECK_RET_VAL(ret, mode);


                break;

#if FSL_FEATURE_SMC_HAS_STOP_SUBMODE2
            case kDemoVlls2:
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                {
                    PRINTF("Can not go from HSRUN to VLLS2 directly\n\r");
                    break;
                }
#endif
                setWakeUpSource(selectWakeUpSource(testVal),"Very Low Leakage Stop 2 mode");
                PRINTF("Wake up goes through Reset sequence.\n\r");

                ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                CHECK_RET_VAL(ret, mode);

                break;
#endif
            case kDemoVlls3:
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
                if (POWER_SYS_GetCurrentMode() == kPowerManagerHsrun)
                {
                    PRINTF("Can not go from HSRUN to VLLS3 directly\n\r");
                    break;
                }
#endif
                setWakeUpSource(selectWakeUpSource(testVal),"Very Low Leakage Stop 3 mode");
                PRINTF("Wake up goes through Reset sequence.\n\r");

                ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                CHECK_RET_VAL(ret, mode);

                break;

            case kDemoRun:

                ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                if (ret != kPowerManagerSuccess)
                {
                    PRINTF("POWER_SYS_SetMode(%u) returned unexpected status : %u\n\r",mode,ret);
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
                    PRINTF("Can not go from HSRUN to VLPR directly\n\r");
                    break;
                }
                ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
                if (ret != kPowerManagerSuccess)
                {
                    PRINTF("POWER_SYS_SetMode(%u) returned unexpected status : %u\n\r",mode,ret);
                }
                else
                {
                    updateClockManagerToHsRunMode(cmConfigMode);
                }
                break;
#endif
            case KDemoADC:
                adc16PrintTemperature();
            break;

            default:
                break;
        }
        PRINTF("\n\rNext loop\n\r\n\r");
    }
}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
