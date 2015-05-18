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

#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include "task_lpm.h"

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
#if (defined(BOARD_USE_LPUART))
            if (CLOCK_VLPR == CLOCK_SYS_GetCurrentConfiguration())
            {
                CLOCK_SYS_SetLpuartSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpuartSrcMcgIrClk);
            }
            else
            {
                CLOCK_SYS_SetLpuartSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpuartSrcIrc48M);
            }
#endif
#if (defined(BOARD_USE_LPSCI))
            if (CLOCK_VLPR == CLOCK_SYS_GetCurrentConfiguration())
            {
                 CLOCK_SYS_SetLpsciSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpsciSrcMcgIrClk);
            }
            else
            {
                CLOCK_SYS_SetLpsciSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpsciSrcPllFllSel);
            }
#endif
#if (defined(BOARD_USE_LPSCI))
            DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_LOW_POWER_UART_BAUD, kDebugConsoleLPSCI);
#elif (defined(BOARD_USE_LPUART))
            DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_LOW_POWER_UART_BAUD, kDebugConsoleLPUART);
#else
            DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_LOW_POWER_UART_BAUD, kDebugConsoleUART);
#endif
        break;

        default:
            result = kClockManagerError;
            break;
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
