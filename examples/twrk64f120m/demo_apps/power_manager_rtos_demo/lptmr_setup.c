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

// SDK Included Files
#include "board.h"

// Project Included Files
#include "lpm_rtos.h"
#include "lptmr_setup.h"
#include "fsl_interrupt_manager.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Structures & enumerations
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief LPTMR interrupt callback
 */
// void lptmr_isr_callback(void)
// {

// }

/*!
 * @brief LPTMR initialize
 */
void lptmrInit(lptmr_user_config_t *lptmrUserConfig, lptmr_state_t *gLPTMRState)
{
    // Initialize LPTMR
    LPTMR_DRV_Init(PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE, gLPTMRState, lptmrUserConfig);
#if (defined FSL_RTOS_BM)
    // With BM, we must let LPTMR continues counting for OSA_GetMsec function work.
    LPTMR_DRV_Start(PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE);
#endif
}

void lptmrSetAlarm(uint8_t offsetSec)
{
    /* Stop lptmr. */
    LPTMR_DRV_Stop(PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE);
    // Enable interrupt.
    LPTMR_HAL_SetIntCmd(g_lptmrBase[PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE],true);
    INT_SYS_EnableIRQ(g_lptmrIrqId[PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE]);
    // Clear interrupt flag.
    LPTMR_HAL_ClearIntFlag(g_lptmrBase[PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE]);
    // Gets the LPTMR counter value
    LPTMR_HAL_GetCounterValue(g_lptmrBase[PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE]);
    // Set the timer period for 1 seconds
    LPTMR_DRV_SetTimerPeriodUs(PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE, (offsetSec * 1000000));
    // Start counting
    LPTMR_DRV_Start(PM_RTOS_DEMO_LPTMR_FUNC_INSTANCE);
}
