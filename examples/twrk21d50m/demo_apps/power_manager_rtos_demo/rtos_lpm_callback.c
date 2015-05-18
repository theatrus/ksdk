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


// Standard C Included Files

// SDK Included Files
#include "fsl_clock_manager.h"
#include "fsl_power_manager.h"
#include "fsl_os_abstraction.h"
#include "fsl_lptmr_driver.h"
#include "board.h"
#if (defined FSL_RTOS_MQX)
#include "bsp.h"
#endif

// Project Included Files
#include "task_lpm.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

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
    uint32_t bytesRemaining = 0;
    volatile bool isLastByteTranmistComplete = false;

    switch (notify->notifyType)
    {
        case kClockManagerNotifyBefore:     // Received "pre" message
            do
            {
#if (defined BOARD_USE_LPSCI)
                LPSCI_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, &bytesRemaining);
                isLastByteTranmistComplete = LPSCI_HAL_GetStatusFlag(BOARD_DEBUG_UART_BASEADDR,kLpsciTxComplete);
#elif (defined BOARD_USE_LPUART)
                LPUART_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, &bytesRemaining);
                isLastByteTranmistComplete = LPUART_HAL_GetStatusFlag(BOARD_DEBUG_UART_BASEADDR,kLpuartTxComplete);
#elif (defined BOARD_USE_UART)
                UART_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, &bytesRemaining);
                isLastByteTranmistComplete = UART_HAL_GetStatusFlag(BOARD_DEBUG_UART_BASEADDR,kUartTxComplete);
#endif
            } while ((bytesRemaining != 0) || (!isLastByteTranmistComplete));
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
    uint32_t bytesRemaining = 0;
    power_manager_error_code_t result = kPowerManagerSuccess;
    volatile bool isLastByteTranmistComplete = false;
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
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
        || notify->targetPowerConfigPtr->mode == kPowerManagerHsrun
#endif
        || notify->targetPowerConfigPtr->mode == kPowerManagerVlpr
        )
        {
            /* if in run mode, do nothing. */
        }
        else
        {
            do
            {
#if (defined BOARD_USE_LPSCI)
                LPSCI_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, &bytesRemaining);
                isLastByteTranmistComplete = LPSCI_HAL_GetStatusFlag(BOARD_DEBUG_UART_BASEADDR,kLpsciTxComplete);
#elif (defined BOARD_USE_LPUART)
                LPUART_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, &bytesRemaining);
                isLastByteTranmistComplete = LPUART_HAL_GetStatusFlag(BOARD_DEBUG_UART_BASEADDR,kLpuartTxComplete);
#elif (defined BOARD_USE_UART)
                UART_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, &bytesRemaining);
                isLastByteTranmistComplete = UART_HAL_GetStatusFlag(BOARD_DEBUG_UART_BASEADDR,kUartTxComplete);
#endif
            } while ((bytesRemaining != 0) || (!isLastByteTranmistComplete));
            disable_unused_pins();
            /* Disable systick in all other mode. */
            SYSTICK_DISABLE();
        }
        break;

        case kPowerManagerNotifyAfter:
        /* TODO */
        /* Add code here. */
        if( crr_mode == kPowerManagerRun
#if FSL_FEATURE_SMC_HAS_HIGH_SPEED_RUN_MODE
         || crr_mode == kPowerManagerHsrun
#endif
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
