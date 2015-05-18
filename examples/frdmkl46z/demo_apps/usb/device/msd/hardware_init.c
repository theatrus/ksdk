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
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "disk.h"

void hardware_init(void)
{

    uint8_t i;

    /* enable clock for PORTs */
    for (i = 0; i < PORT_INSTANCE_COUNT; i++)
    {
        CLOCK_SYS_EnablePortClock(i);
    }

#if (SD_CARD_APP)
#if ((defined TWR_K64F120M) || (defined FRDM_K64F) || (defined TWR_K60D100M) || (defined TWR_K21F120M) || (defined TWR_K65F180M))
    /* configure detect pin as gpio (alt1) */
    uint32_t port = GPIO_EXTRACT_PORT(sdhcCdPin[0].pinName);
    uint32_t pin = GPIO_EXTRACT_PIN(sdhcCdPin[0].pinName);
    PORT_Type * portBase = g_portBase[port];
    PORT_HAL_SetMuxMode(portBase, pin, kPortMuxAsGpio);

    configure_sdhc_pins(BOARD_SDHC_INSTANCE);
#endif
#endif
    /* Init board clock */
    BOARD_ClockInit();
    dbg_uart_init();
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
