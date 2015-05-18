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
#include "fsl_pmc_hal.h"
#include "fsl_debug_console.h"

void setup_debug_pins(port_mux_t mux)
{
  PORT_HAL_SetMuxMode(PORTA, 0u, mux);
  PORT_HAL_SetMuxMode(PORTA, 3u, mux);
}

void setup_uart_pins(port_mux_t mux)
{
  /* UART1 */
  PORT_HAL_SetMuxMode(PORTA,1u, mux);
  PORT_HAL_SetMuxMode(PORTA,2u, mux);
}

void hardware_init(void)
{

    /* enable clock for PORTs */
    CLOCK_SYS_EnablePortClock(PORTA_IDX);
    CLOCK_SYS_EnablePortClock(PORTC_IDX);
    CLOCK_SYS_EnablePortClock(PORTD_IDX);
    CLOCK_SYS_EnablePortClock(PORTE_IDX);

#if FSL_FEATURE_SIM_OPT_HAS_RTC_CLOCK_OUT_SELECTION
    configure_rtc_pins(0);
#endif

    if(PMC_HAL_GetAckIsolation(PMC_BASE_PTR) != 0)
    {
        PMC_HAL_ClearAckIsolation(PMC_BASE_PTR);
    }

    /* Init board clock */
    BOARD_ClockInit();

    setup_uart_pins(kPortMuxAlt2);

    // Select different clock source for LPSCI. */
#if (CLOCK_INIT_CONFIG == CLOCK_VLPR)
    CLOCK_SYS_SetLpsciSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpsciSrcMcgIrClk);
#else
    CLOCK_SYS_SetLpsciSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpsciSrcPllFllSel);
#endif

    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_LOW_POWER_UART_BAUD, kDebugConsoleLPSCI);
}

void disable_unused_pins(void)
{
  /* Disable debug pins when MCU sleeps */
  setup_debug_pins(kPortMuxAlt4);

  /* Disable uart pins */
  setup_uart_pins(kPortPinDisabled);
}

void enable_unused_pins(void)
{
  /* Enable debug pins when MCU sleeps */
  setup_debug_pins(kPortMuxAlt7);

  /* Enable uart pins */
  setup_uart_pins(kPortMuxAlt2);

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
