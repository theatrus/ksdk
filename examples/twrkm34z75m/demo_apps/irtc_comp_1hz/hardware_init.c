/*
 * Copyright (c) 2013-2014, Freescale Semiconductor, Inc.
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

#include "board.h"
#include "pin_mux.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"


/* Configuration for enter RUN mode. Core clock = 24MHz. */
const clock_manager_user_config_t clkFll24MCfg =
{
    .mcgConfig =
    {
        .mcg_mode           = kMcgModeFEE,   /* Work in FEE mode. */
        .irclkEnable        = true,         /* MCGIRCLK enable. */
        .irclkEnableInStop  = true,         /* MCGIRCLK enable in STOP mode. */
        .ircs               = kMcgIrcFast,  /* Select IRC32k. */
        .fcrdiv             = 0U,           /* FCRDIV is 0. */

        .frdiv   = 0U,
        .drs     = kMcgDcoRangeSelLow,      /* low frequency range */
        .dmx32   = kMcgDmx32Fine,           /* DCO is fine-tuned for maximum frequency */
        .oscsel  = kMcgOscselRtc,           /* Select RTC */

        .pll0EnableInFllMode = false,  /* PLL0 disable */
        .pll0EnableInStop    = false   /* PLL0 disable in STOP mode */
    },
    .simConfig =
    {
        .pllFllSel = kClockPllFllSelFll,        /* PLLFLLSEL select FLL. */
        .er32kSrc  = kClockEr32kSrcOsc32kClk,   /* ERCLK32K selection, use RTC. */
        .sysdiv   = 0U,         /* system clock is 24 MHZ */
        .busdiv   = 0U,         /* bus clock is 24 MHZ */
        .flashclkmode   = 0U,   /* flash clock is 24 MHZ */
    },
    .oscerConfig =
    {
        .enable       = false,  /* OSCERCLK enable. */
        .enableInStop = false,  /* OSCERCLK enable in STOP mode. */
    }
};

void hardware_init(void) {
  /* enable clock for PORTs */
  CLOCK_SYS_EnablePortClock(PORTE_IDX);
  CLOCK_SYS_EnablePortClock(PORTF_IDX);
  CLOCK_SYS_EnablePortClock(PORTI_IDX);
  CLOCK_SYS_EnablePortClock(PORTL_IDX);

  /* enable XBAR clock */
  CLOCK_SYS_EnableXbarClock(0);

  /* Init board EXTAL 32.768KHz clock */
  BOARD_InitRtcOsc();

  /* configure MCG mode as FEE, FLL 24MHz 1:1:1 */
  CLOCK_SYS_SetConfiguration(&clkFll24MCfg);

  /* init debug uart */
  dbg_uart_init();
  
  configure_xbar_pins(0U);
}

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.4 [05.10]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
