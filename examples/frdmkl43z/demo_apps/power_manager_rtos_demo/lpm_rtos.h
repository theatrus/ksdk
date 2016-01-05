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
#ifndef LPM_RTOS_H
#define LPM_RTOS_H

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////
// Standard C Included Files
#include <stdio.h>
// SDK Included Files
#include "board.h"
#include "fsl_clock_manager.h"
#include "fsl_power_manager.h"
#include "fsl_pmc_hal.h"
#include "fsl_smc_hal.h"
#include "fsl_os_abstraction.h"

#if (defined BOARD_USE_LPSCI)
#include "fsl_lpsci_driver.h"
#elif (defined BOARD_USE_LPUART)
#include "fsl_lpuart_driver.h"
#else
#include "fsl_uart_driver.h"
#endif // BOARD_USE_LPSCI
// Project Included Files

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#if (defined(FSL_FEATURE_SOC_RTC_COUNT) && FSL_FEATURE_SOC_RTC_COUNT >= 1)
/* The RTC instance used for RTC */
#define PM_RTOS_DEMO_RTC_FUNC_INSTANCE                  0
#endif

/* The LPTMR instance used for LPTMR */
#define PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE                0

/* The LLWU wakeup module for RTC */
#define PM_RTOS_DEMO_RTC_LLWU_WAKEUP_MODULE             kLlwuWakeupModule5

/* The LLWU wakeup module for LPTMR */
#define PM_RTOS_DEMO_LPTMR_LLWU_WAKEUP_MODULE           kLlwuWakeupModule0


///////////////////////////////////////////////////////////////////////////////
// Structures & enumerations
///////////////////////////////////////////////////////////////////////////////
enum LPM_DEMO_RETURN
{
    LPM_DEMO_RETURN_OK,
    LPM_DEMO_RETURN_NULL,
};

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////
clock_manager_error_code_t rtos_cm_callback(clock_notify_struct_t *notify, void* callbackData);
clock_manager_error_code_t dbg_console_cm_callback(clock_notify_struct_t *notify, void* callbackData);
power_manager_error_code_t rtos_pm_callback(power_manager_notify_struct_t * notify, power_manager_callback_data_t * dataPtr);
power_manager_error_code_t adc16_pm_callback(power_manager_notify_struct_t * notify, power_manager_callback_data_t * dataPtr);

/*!
 * @Brief get character from uart0 in blocking mode.
 */
char getInput(void);
#endif

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
