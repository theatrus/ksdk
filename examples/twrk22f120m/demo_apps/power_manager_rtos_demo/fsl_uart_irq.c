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

// SDK Included Files
#include "board.h"
#if (defined BOARD_USE_LPSCI)
#include "fsl_lpsci_driver.h"
#elif (defined BOARD_USE_LPUART)
#include "fsl_lpuart_driver.h"
#else
#include "fsl_uart_driver.h"
#endif // BOARD_USE_LPSCI
// Project Included Files
#include "lpm_rtos.h"

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////
#if (defined BOARD_USE_LPSCI)
extern void LPSCI_DRV_IRQHandler(uint32_t instance);
#elif (defined BOARD_USE_LPUART)
extern void LPUART_DRV_IRQHandler(uint32_t instance);
#else
extern void UART_DRV_IRQHandler(uint32_t instance);
#endif // BOARD_USE_LPSCI

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
void PM_DBG_UART_IRQ_HANDLER(void)
{
#if (defined BOARD_USE_LPSCI)
    LPSCI_DRV_IRQHandler(BOARD_DEBUG_UART_INSTANCE);
#elif (defined BOARD_USE_LPUART)
    LPUART_DRV_IRQHandler(BOARD_DEBUG_UART_INSTANCE);
#else
    UART_DRV_IRQHandler(BOARD_DEBUG_UART_INSTANCE);
#endif // BOARD_USE_LPSCI
}


///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////

