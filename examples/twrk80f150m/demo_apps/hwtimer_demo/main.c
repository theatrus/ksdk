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

// Standard C Included Files
#include <stdio.h>

// SDK Included Files
#include "board.h"
#include "fsl_hwtimer.h"
#include "fsl_debug_console.h"


///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define HWTIMER_LL_DEVIF    kSystickDevif
#define HWTIMER_LL_ID       0

#define HWTIMER_ISR_PRIOR       5
#define HWTIMER_PERIOD          100000
#define HWTIMER_DOTS_PER_LINE   40
#define HWTIMER_LINES_COUNT     2

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

extern const hwtimer_devif_t kSystickDevif;
extern const hwtimer_devif_t kPitDevif;
hwtimer_t hwtimer;

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Hardware timer callback function
 */
void hwtimer_callback(void* data)
{
    PRINTF(".");
    if ((HWTIMER_SYS_GetTicks(&hwtimer) % HWTIMER_DOTS_PER_LINE) == 0)
    {
        PRINTF("\r\n");
    }
    if ((HWTIMER_SYS_GetTicks(&hwtimer) % (HWTIMER_LINES_COUNT * HWTIMER_DOTS_PER_LINE)) == 0)
    {
        if (kHwtimerSuccess != HWTIMER_SYS_Stop(&hwtimer))
        {
            PRINTF("\r\nError: hwtimer stop.\r\n");
        }
        PRINTF("End\r\n");
    }
}

/*!
 * @brief Main function
 */
int main (void)
{
    // Initialize standard SDK demo application pins
    hardware_init();

    // Print the initial banner
    PRINTF("\r\nHwtimer Example \r\n");

    // Hwtimer initialization
    if (kHwtimerSuccess != HWTIMER_SYS_Init(&hwtimer, &HWTIMER_LL_DEVIF, HWTIMER_LL_ID, NULL))
    {
        PRINTF("\r\nError: hwtimer initialization.\r\n");
    }

    /*
     * In case you wish to raise the Systick ISR priority, then use the below command with the
     * appropriate priority setting:
     * NVIC_SetPriority(SysTick_IRQn, isrPrior);
     */
    NVIC_SetPriority(SysTick_IRQn, HWTIMER_ISR_PRIOR);

    if (kHwtimerSuccess != HWTIMER_SYS_SetPeriod(&hwtimer, HWTIMER_PERIOD))
    {
        PRINTF("\r\nError: hwtimer set period.\r\n");
    }
    if (kHwtimerSuccess != HWTIMER_SYS_RegisterCallback(&hwtimer, hwtimer_callback, NULL))
    {
        PRINTF("\r\nError: hwtimer callback registration.\r\n");
    }
    if (kHwtimerSuccess != HWTIMER_SYS_Start(&hwtimer))
    {
        PRINTF("\r\nError: hwtimer start.\r\n");
    }

    // Wait for Hardware Timer interrupts
    while(1)
    {}
}
