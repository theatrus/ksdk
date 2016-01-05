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
//  Includes
///////////////////////////////////////////////////////////////////////////////
// SDK Included Files
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_irtc_hal.h"
#include "fsl_port_hal.h"
#include "fsl_xbar_hal.h"
#include "fsl_xbar_signals.h"
#include "fsl_sim_hal.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////
#define TEST_ERR_PPM (-100) // test ppm

static void CalCompValue(int32_t ppm, int8_t *comp_int, uint8_t *comp_frac);
static void IrtcUnlock(void);

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Main function
 */
int main (void)
{
    int8_t compInt;
    uint8_t compFrac;

    // Initialize boards and debug uart
    hardware_init();

    // route the core clock to PORTF7
    CLOCK_HAL_SetClkOutSel(SIM, kClockClkoutGatedCoreClk);
    

    // Print the initial banner
    PRINTF("\r\nRTC Compensate 1HZ output demo\r\n");

    // redirect the RTC CLKOUT through XBAR to output
    XBAR_HAL_SetOutSel(XBAR, kXbaraOutputXB_OUT10, kXbaraInputRTC_CLK_Output);

    // Select OSC_32K as RTC IP clock source
    CLOCK_HAL_SetRtcClkSel(SIM, kClockRtcSrcRtcOsc32kClk);

    IrtcUnlock();
    // disable all of the interrupts
    RTC_WR_IER(RTC, 0x0);
    // clear isr status
    RTC_WR_ISR(RTC, 0xffff);
    // RTC clock out for 1hz fine
    IRTC_HAL_SetClockOutMode(RTC, kIRTCFine1hzClk);

    // calculate the comp int and frac value from ppm
    CalCompValue(TEST_ERR_PPM, &compInt, &compFrac);

    // set the compensation Integer and Frac
    IrtcUnlock();
    // enable compensation
    IRTC_HAL_SetFineCompensation(RTC, compInt, compFrac, false);

    PRINTF("Compensate with error %dppm (int:%x, frac:%x)\r\n",
            TEST_ERR_PPM, compInt, compFrac);

    while(1)
    {
     ;
    }
}

/**
* iRTC unlock operation with status check
*/
static void IrtcUnlock(void)
{

    while (IRTC_HAL_GetStatusFlag(RTC, kIRTCReadOnly))
    {
        IRTC_HAL_SetLockRegisterCmd(RTC, false);
    }
}

/**
* Calculate the compensation integer and frac value from ppm
*/
static void CalCompValue(int32_t ppm, int8_t *comp_int, uint8_t *comp_frac)
{
    int32_t int_part, left;

    int_part = (ppm << 15) / 1000000;

    if (ppm < 0)
    {
        // need one more int part, as frac is postive
        int_part -= 1;
    }

    left = ppm - ((1000000 * int_part) >> 15);
    *comp_frac = (left << 22) / 1000000;

    // convert int part value from int32 to COMPEN[15:12]
    if (int_part < 0)
    {
        int_part += 16;
    }
    *comp_int = int_part;
}
