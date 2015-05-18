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

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "board.h"

#include "fsl_hwtimer_systick.h"
#include "fsl_mmdvsq_hal.h"
#include "fsl_debug_console.h"
#include <arm_math.h>

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

#define USE_STDIO_FUNCTIONS     // Define this symbol to use STDIO functions
#define SYSTICK_LOAD_VALUE      0xFFFFFF

#define HWTIMER_LL_DEVIF        kSystickDevif
#define HWTIMER_LL_ID           0
#define HWTIMER_PERIOD          100000
#define HWTIMER_DOTS_PER_LINE   40
#define HWTIMER_LINES_COUNT     2

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

extern const hwtimer_devif_t kSystickDevif;
extern const hwtimer_devif_t kPitDevif;
hwtimer_t hwtimer;
hwtimer_time_t time;
uint32_t overhead;
uint32_t cnt_start_value;
uint32_t cnt_end_value;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

int main(void)
{
    // Buffer used to hold the string to be transmitted
    char sourceBuff[24] = {"\r\nMMDVSQ Demo start!\n\r"};
    // Buffer used to hold received data
    uint8_t receiveBuff[19] = {0};
    uint32_t cnt_start_value;
    uint32_t cnt_end_value;
    uint32_t tickcycles;
    uint32_t dividend = 0xAAAA5555;
    uint32_t divisor  = 0xAA;
    uint32_t result;
    q15_t output;

    // Initialize standard SDK demo application pins
    hardware_init();

    // Print the initial banner
    PRINTF(sourceBuff);

    // Hwtimer initialization
    HWTIMER_SYS_Init(&hwtimer, &HWTIMER_LL_DEVIF, HWTIMER_LL_ID, NULL);
    HWTIMER_SYS_SetPeriod(&hwtimer, HWTIMER_PERIOD);
    HWTIMER_SYS_Start(&hwtimer);
    HWTIMER_SYS_GetTime(&hwtimer, &time);
    cnt_start_value = time.subTicks;
    HWTIMER_SYS_GetTime(&hwtimer, &time);
    cnt_end_value = time.subTicks;
    overhead = cnt_end_value - cnt_start_value;
    // This demo only shows the speed and efficiency and doesn't care the results
    // Stand C library 3 times unsigned divide and once square root calculation
    HWTIMER_SYS_Init(&hwtimer, &HWTIMER_LL_DEVIF, HWTIMER_LL_ID, NULL);
    HWTIMER_SYS_SetPeriod(&hwtimer, HWTIMER_PERIOD);
    HWTIMER_SYS_Start(&hwtimer);
    HWTIMER_SYS_GetTime(&hwtimer, &time);
    cnt_start_value = time.subTicks;
    result = dividend/divisor;
    result = result/divisor;
    result = result/divisor;
    arm_sqrt_q15((q15_t)result, &output) ;
    HWTIMER_SYS_GetTime(&hwtimer, &time);
    cnt_end_value = time.subTicks;
    tickcycles = (cnt_end_value - cnt_start_value - overhead);
    PRINTF("\r\nC library calculation takes %lu tickcycles\n\r",tickcycles);

    // mmdvsq 3 times unsigned divide and once square root calculation
    HWTIMER_SYS_Init(&hwtimer, &HWTIMER_LL_DEVIF, HWTIMER_LL_ID, NULL);
    HWTIMER_SYS_SetPeriod(&hwtimer, HWTIMER_PERIOD);
    HWTIMER_SYS_Start(&hwtimer);
    HWTIMER_SYS_GetTime(&hwtimer, &time);
    cnt_start_value = time.subTicks;
    result = MMDVSQ_HAL_DivUQ(MMDVSQ_BASE_PTR, dividend, divisor);
    result = MMDVSQ_HAL_DivUQ(MMDVSQ_BASE_PTR, result, divisor);
    result = MMDVSQ_HAL_DivUQ(MMDVSQ_BASE_PTR, result, divisor);
    result = MMDVSQ_HAL_Sqrt(MMDVSQ_BASE_PTR, result);
    HWTIMER_SYS_GetTime(&hwtimer, &time);
    cnt_end_value = time.subTicks;
    tickcycles = (cnt_end_value - cnt_start_value - overhead);
    PRINTF("\r\nMMDVSQ t calculation takes %lu tickcycles\n\r",tickcycles);

    PRINTF("\r\nMMDVSQ Demo end\n\r");
    while(1)
    {
        // Main routine that simply echoes received characters forever
        // First, get character.
        receiveBuff[0] = GETCHAR();

        // Now echo the received character
        PUTCHAR(receiveBuff[0]);
    }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
