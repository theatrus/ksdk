/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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

// Project Included Files
#include "task_lpm.h"

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
extern const clock_manager_user_config_t g_defaultClockConfigRun;
extern const clock_manager_user_config_t g_defaultClockConfigVlpr;

const clock_manager_user_config_t * g_defaultClockConfigurations[] = {
    NULL,
    &g_defaultClockConfigVlpr,
    &g_defaultClockConfigRun,
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
 * task to blink led rtos between 1 seconds.
 */
void task_led_rtos(task_param_t param)
{
    LED_RTOS_EN;
    while(1)
    {
        LED_RTOS_TOGGLE;
        OSA_TimeDelay(1000);
    }
}
/*!
 * task to blink led clock after delay loop.
 */
/*!
 * @brief wait uart finished.
 *
 */
void wait_finish_uart(void)
{
    uint32_t bytesRemaining = 0;
    volatile bool isLastByteTranmistComplete = false;
    do
    {
        LPSCI_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, &bytesRemaining);
        isLastByteTranmistComplete = LPSCI_HAL_GetStatusFlag(BOARD_DEBUG_UART_BASEADDR,kLpsciTxComplete);
    } while ((bytesRemaining != 0) || (!isLastByteTranmistComplete));
}

/*!
 * @brief get input charater.
 *
 */
char getInput()
{
    char ch;
    /* We use GETCHAR() for BM and UART Blocking technic for other RTOS. */
#if (defined FSL_RTOS_BM) || ((defined FSL_RTOS_MQX)&&(MQX_COMMON_CONFIG != MQX_LITE_CONFIG))
    ch = GETCHAR();
#else
    LPSCI_DRV_ReceiveDataBlocking(BOARD_DEBUG_UART_INSTANCE,(uint8_t*)(&ch),1,OSA_WAIT_FOREVER);
#endif // FSL_RTOS_BM
    return ch;
}

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
        rtcSetAlarm(PM_RTOS_DEMO_RTC_FUNC_INSTANCE, offsetSec);
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
    if(val == 'T' || val == 't')
    {
        ret = wakeUpSourceRtc;
    }
    if(val == 'L' || val == 'l')
    {
        ret = wakeUpSourceLptmr;
    }
    if(val == 'S' || val == 's')
    {
        ret =  wakeUpSourceSwBtn;
    }
    return ret;
}

/* LLW_IRQHandler that would cover the same name's APIs in startup code */
void LLWU_IRQHandler(void)
{
    // The LLWU wakeup interrup is LPTMR source
    if (LLWU_HAL_GetInternalModuleWakeupFlag(LLWU_BASE_PTR, PM_RTOS_DEMO_LPTMR_LLWU_WAKEUP_MODULE))
    {
        LPTMR_HAL_ClearIntFlag(g_lptmrBase[PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE]);
    }

    // The LLWU wakeup interrup is RTC source
    if (LLWU_HAL_GetInternalModuleWakeupFlag(LLWU_BASE_PTR, PM_RTOS_DEMO_RTC_LLWU_WAKEUP_MODULE))
    {
        RTC_DRV_SetAlarmIntCmd(PM_RTOS_DEMO_RTC_FUNC_INSTANCE, false);
    }

    // The LLWU wakeup interrup is Switch/Button source
    if (LLWU_HAL_GetExternalPinWakeupFlag(LLWU_BASE_PTR,(llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN))
    {
         LLWU_HAL_ClearExternalPinWakeupFlag(LLWU_BASE_PTR, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
    }
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
        PRINTF("\r\nWaiting for key press..\r\n\r\n");
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
                    OSA_EnterCritical(kCriticalDisableInt);
                    if((wus & wakeUpSourceRtc) != 0)
                    {
                        LLWU_HAL_SetInternalModuleCmd(LLWU_BASE_PTR,PM_RTOS_DEMO_RTC_LLWU_WAKEUP_MODULE,true);
                    }
                    if((wus & wakeUpSourceLptmr) != 0)
                    {
                        LLWU_HAL_SetInternalModuleCmd(LLWU_BASE_PTR,PM_RTOS_DEMO_LPTMR_LLWU_WAKEUP_MODULE,true);
                    }
                    OSA_ExitCritical(kCriticalDisableInt);
                    cmdAlarm(wus, val0);
                    return val0;
                }
            }
        }

        PRINTF("Wrong value!\r\n");
    }
}

void printWakeUpSourceText(wakeUpSource_t wus)
{
    PRINTF("Select the wake up source:\r\n");
    if((wus & wakeUpSourceRtc) != 0)
    {
        PRINTF("Press T for RTC - RTC Timer\r\n");
    }
    if((wus & wakeUpSourceLptmr) != 0)
    {
        PRINTF("Press L for LPTMR - LPTMR Timer\r\n");
    }
    if((wus & wakeUpSourceSwBtn) != 0)
    {
        PRINTF("Press S for switch/button ");
        PRINT_LLWU_SW_NUM;
        PRINTF("\r\n");
    }
}

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
            case kDemoLls:
            case kDemoVlls1:
            case kDemoVlls3:
                break;
            case kDemoVlls0:
                // in VLLS0 LPO oscillator is disabled.
                PRINTF("The board does not support wake up from this mode by Timer due to disabled LPO.\r\n");
                wus &= ~(wakeUpSourceRtc | wakeUpSourceLptmr);
                break;
            default:
                break;
        }
        PRINTF("\r\nWaiting for key press..\r\n\r\n");
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

void llwuDisableWakeUp(void)
{
    LLWU_HAL_SetInternalModuleCmd(LLWU_BASE_PTR, PM_RTOS_DEMO_RTC_LLWU_WAKEUP_MODULE, false);
    LLWU_HAL_SetInternalModuleCmd(LLWU_BASE_PTR, PM_RTOS_DEMO_LPTMR_LLWU_WAKEUP_MODULE, false);
}

void gpioDisableWakeUp(void)
{
    // disables interrupt
    PORT_HAL_SetPinIntMode(BOARD_SW_LLWU_BASE, BOARD_SW_LLWU_PIN, kPortIntDisabled);
    INT_SYS_DisableIRQ(BOARD_SW_LLWU_IRQ_NUM);

    LLWU_HAL_ClearExternalPinWakeupFlag(LLWU_BASE_PTR, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
    LLWU_HAL_SetExternalInputPinMode(LLWU_BASE_PTR,kLlwuExternalPinDisabled, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
}

void gpioEnableWakeUp(void)
{
    // enables falling edge interrupt for switch SWx
    PORT_HAL_SetPinIntMode(BOARD_SW_LLWU_BASE, BOARD_SW_LLWU_PIN, kPortIntFallingEdge);
    INT_SYS_EnableIRQ(BOARD_SW_LLWU_IRQ_NUM);

    LLWU_HAL_ClearExternalPinWakeupFlag(LLWU_BASE_PTR, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
    LLWU_HAL_SetExternalInputPinMode(LLWU_BASE_PTR,kLlwuExternalPinFallingEdge, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
}

void setWakeUpSource(wakeUpSource_t wus,char *textMode)
{
    uint8_t timeout;

    if((wus & (wakeUpSourceRtc | wakeUpSourceLptmr)) != 0)
    {
        gpioDisableWakeUp();
        // Wake up on timer's interrupt
        timeout = setWakeUpTimeOut(wus);
        PRINTF("Entering %s, will wake up after %u seconds\r\n",textMode,timeout);
    }
    else if((wus & wakeUpSourceSwBtn) != 0)
    {
        llwuDisableWakeUp();
        // Wake up on gpio interrupt from button(s)
        gpioEnableWakeUp();
        PRINTF("Entering %s, press the ",textMode);
        PRINT_LLWU_SW_NUM;
        PRINTF(" button to wake up.\r\n");
    }
    else
    {
        PRINTF("Unknown error.\r\n");
    }
}

void displayPowerMode(void)
{
    uint32_t smcMode = SMC_HAL_GetStat(SMC_BASE_PTR);

    switch (smcMode)
    {
    case kStatRun:
        PRINTF("    SMC mode = kPowerModeRun\r\n");
        break;
    case kStatVlpr:
        PRINTF("    SMC mode = kPowerModeVlpr\r\n");
        break;
    default:
        PRINTF("    SMC mode = Unknown!\r\n");
        break;
    }
}

/* Update clock.*/
void update_clock_mode(uint8_t cmConfigMode)
{
    CLOCK_SYS_UpdateConfiguration(cmConfigMode, kClockManagerPolicyForcible);
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

    rtc_datetime_t date;
    memset(&date, 0, sizeof(rtc_datetime_t));
    memset(&cmCallbackData, 0, sizeof(lptmrStructure_t));
    cmCallbackData.instance = PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE;
    lptmr_user_config_t *lptmrUserConfig = &(cmCallbackData.lptmrUserConfig);
    lptmr_state_t *lptmrState = &(cmCallbackData.lptmrState);

    CLOCK_SYS_Init(g_defaultClockConfigurations,
                   CLOCK_NUMBER_OF_CONFIGURATIONS,
                   cm_callback_tbl,
                   cm_callback_tbl_size);

    CLOCK_SYS_UpdateConfiguration(cmConfigMode, kClockManagerPolicyForcible);

    // Set a start date time and start RTC
    date.year = 2014;
    date.month = 4U;
    date.day = 30U;
    date.hour = 14U;
    date.minute = 0U;
    date.second = 0U;
    rtcInit(PM_RTOS_DEMO_RTC_FUNC_INSTANCE, &date);
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

#if (defined FSL_RTOS_BM)
    PRINTF("\r\n####################  Power Manager BM Demo ####################\r\n\r\n");
#elif (defined FSL_RTOS_FREE_RTOS)
    PRINTF("\r\n####################  Power Manager FreeRTOS Demo ####################\r\n\r\n");
#elif (defined FSL_RTOS_MQX)
    PRINTF("\r\n####################  Power Manager MQX Demo ####################\r\n\r\n");
#elif (defined FSL_RTOS_UCOSII)
    PRINTF("\r\n####################  Power Manager Ucosii Demo ####################\r\n\r\n");
#elif (defined FSL_RTOS_UCOSIII)
    PRINTF("\r\n####################  Power Manager Ucosiii Demo ####################\r\n\r\n");
#else
    PRINTF("\r\nUnknown RTOS\r\n\r\n");
#endif

    while (1)
    {
        mode = 0;
        CLOCK_SYS_GetFreq(kCoreClock, &freq);
        PRINTF("    Core Clock = %luHz \r\n", freq);
        displayPowerMode();
        PRINTF("\r\nSelect the desired operation \r\n\r\n");
        PRINTF("Press  %c for enter: RUN   - Normal RUN mode\r\n",kDemoRun);
        PRINTF("Press  %c for enter: Wait  - Wait mode\r\n",kDemoWait);
        PRINTF("Press  %c for enter: Stop  - Stop mode\r\n",kDemoStop);
        PRINTF("Press  %c for enter: VLPR  - Very Low Power Run mode\r\n",kDemoVlpr);
        PRINTF("Press  %c for enter: VLPW  - Very Low Power Wait mode\r\n",kDemoVlpw);
        PRINTF("Press  %c for enter: VLPS  - Very Low Power Stop mode\r\n",kDemoVlps);
        PRINTF("Press  %c for enter: LLS   - Low Leakage Stop mode\r\n",kDemoLls);
        PRINTF("Press  %c for enter: VLLS0 - Very Low Leakage Stop 0 mode\r\n",kDemoVlls0);
        PRINTF("Press  %c for enter: VLLS1 - Very Low Leakage Stop 1 mode\r\n",kDemoVlls1);
        PRINTF("Press  %c for enter: VLLS3 - Very Low Leakage Stop 3 mode\r\n",kDemoVlls3);
        PRINTF("Press  %c to get current chip temperature\r\n",KDemoADC);
        PRINTF("------------------------------------------------------------\r\n");
        PRINTF("\r\nWaiting for key press..\r\n\r\n");

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
                    /*
                     If apps default CM config mode is not VLPR, but needs to enter VLPR, and real CM config
                     is not VLPR, then we need to update it to VLPR mode here. Otherwise pass through.
                     */
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

                if (POWER_SYS_GetCurrentMode() == kPowerManagerRun)
                {
                    // update Clock Mode to Run
                    update_clock_mode(CLOCK_RUN);
                }

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
            case kDemoVlls0:
                setWakeUpSource(selectWakeUpSource(testVal),"Very Low Leakage Stop 0 mode");
                PRINTF("Wake up goes through Reset sequence.\r\n");

                ret = POWER_SYS_SetMode(mode, kPowerManagerPolicyAgreement);
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
            case KDemoADC:
                adc16PrintTemperature();
            break;

            default:
                break;
        }
        PRINTF("\r\nNext loop\r\n\r\n");
    }
}

/*FUNCTION*********************************************************************
 *
 * Function Name : dbg_console_cm_callback
 * Description   : debug console callback for change event from power manager
 *
 *END*************************************************************************/
clock_manager_error_code_t dbg_console_cm_callback(clock_notify_struct_t *notify,
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
            if (CLOCK_VLPR == CLOCK_SYS_GetCurrentConfiguration())
            {
                 CLOCK_SYS_SetLpsciSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpsciSrcMcgIrClk);
            }
            else
            {
                CLOCK_SYS_SetLpsciSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpsciSrcPllFllSel);
            }
            DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_LOW_POWER_UART_BAUD, kDebugConsoleLPSCI);
        break;

        default:
            result = kClockManagerError;
            break;
    }
    return result;
}

/*FUNCTION*********************************************************************
 *
 * Function Name : rtos_cm_callback
 * Description   : rtos callback for change event from clock manager
 *
 *END*************************************************************************/
clock_manager_error_code_t rtos_cm_callback(clock_notify_struct_t *notify,
     void* callbackData)
{
    clock_manager_error_code_t result = kClockManagerSuccess;
    lptmrStructure_t *lptmrStr = (lptmrStructure_t*)callbackData;

    switch (notify->notifyType)
    {
        case kClockManagerNotifyBefore:     // Received "pre" message
          wait_finish_uart();
          SYSTICK_DISABLE();
        break;

        case kClockManagerNotifyRecover: // Received "recover" message
            SYSTICK_DISABLE();
            SYSTICK_RELOAD((CLOCK_SYS_GetCoreClockFreq()/TICK_PER_SEC)-1UL);
            SYSTICK_ENABLE();
        break;
        case kClockManagerNotifyAfter:    // Received "post" message
            /* Caculate prescaler clock frequency */
            if ( kLptmrTimerModeTimeCounter == lptmrStr->lptmrUserConfig.timerMode)
            {
                lptmrStr->lptmrState.prescalerClockHz = CLOCK_SYS_GetLptmrFreq(lptmrStr->instance,
                        lptmrStr->lptmrUserConfig.prescalerClockSource);

                if (lptmrStr->lptmrUserConfig.prescalerEnable)
                {
                    lptmrStr->lptmrState.prescalerClockHz = (lptmrStr->lptmrState.prescalerClockHz >> ((uint32_t)(lptmrStr->lptmrUserConfig.prescalerValue+1)));
                }
            }

            SYSTICK_RELOAD((CLOCK_SYS_GetCoreClockFreq()/TICK_PER_SEC)-1UL);
            SYSTICK_ENABLE();
        break;

        default:
            result = kClockManagerError;
            break;
    }
    return result;
}
/*FUNCTION*********************************************************************
 *
 * Function Name : rtos_pm_callback
 * Description   : rtos callback for change event from power manager
 *
 *END*************************************************************************/
power_manager_error_code_t rtos_pm_callback(power_manager_notify_struct_t * notify,
    power_manager_callback_data_t * dataPtr)
{
    power_manager_error_code_t result = kPowerManagerSuccess;
    power_manager_modes_t crr_mode = POWER_SYS_GetCurrentMode();
    switch (notify->notifyType)
    {
        case kPowerManagerNotifyRecover:
        /* TODO */
        /* Add code here. */
        break;

        case kPowerManagerNotifyBefore:
        /* TODO */
        /* Add code here. */
        if(notify->targetPowerConfigPtr->mode == kPowerManagerRun
        || notify->targetPowerConfigPtr->mode == kPowerManagerVlpr
        )
        {
            /* if in run mode, do nothing. */
        }
        else
        {
            wait_finish_uart();
            disable_unused_pins();
            /* Disable systick in all other mode. */
            SYSTICK_DISABLE();
        }
        break;

        case kPowerManagerNotifyAfter:
        /* TODO */
        /* Add code here. */
        if( crr_mode == kPowerManagerRun
         || crr_mode == kPowerManagerVlpr
         )
        {
#if (defined FSL_RTOS_BM)
            // With BM, we must let LPTMR continues counting for OSA_GetMsec function work.
            // Disable interrupt.
            LPTMR_HAL_SetIntCmd(g_lptmrBase[PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE],false);
#else
            /* Stop lptmr. */
            LPTMR_DRV_Stop(PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE);
#endif
            enable_unused_pins();
            /* Enable systick in all run mode. */
            SYSTICK_ENABLE();
        }
        else
        {
            /* if in other mode, do nothing. */
        }
        break;

        default:
            result = kPowerManagerError;
        break;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
