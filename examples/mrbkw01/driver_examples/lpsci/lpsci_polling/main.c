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
//  Includes
///////////////////////////////////////////////////////////////////////////////

#include "board.h"
#include "fsl_clock_manager.h"
#include "fsl_lpsci_hal.h"

///////////////////////////////////////////////////////////////////////////////
//  Consts
///////////////////////////////////////////////////////////////////////////////

const uint8_t buffStart[] = "\n\r++++++++++++++ LPSCI Polling Example ++++++++++++++++++\n\r";
const uint8_t bufferData1[] = "\n\rType characters from keyboard, the board will receive and then echo them to terminal screen\n\r";


///////////////////////////////////////////////////////////////////////////////
//  Code
///////////////////////////////////////////////////////////////////////////////
/*
 * This example will show transmit/receive LPSCI's driver, the efficiency of the
 * transmit/receive drivers with using polling. Transfer data between
 * board and PC. Board will transfer and receive characters with PC through
 * LPSCI interface. Type characters from keyboard, the board will receive and
 * then echo them to terminal screen. Look for intructions output to the terminal.
 */

/*!
 * @brief Check send/receive polling functionality
 *
 */
int main(void)
{
    uint8_t rxChar = 0;
    uint32_t byteCountBuff = 0;

    // Declare config sturcuture to initialize a uart instance
    UART0_Type* baseAddr = BOARD_DEBUG_UART_BASEADDR;
    uint32_t lpsciSourceClock;

    // Enable clock for PORTs, setup board clock source
    hardware_init();

    // Set the LPSCI clock source selection
#if defined(KL02Z4_SERIES)
    CLOCK_SYS_SetLpsciSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpsciSrcFll);
#else
    CLOCK_SYS_SetLpsciSrc(BOARD_DEBUG_UART_INSTANCE, kClockLpsciSrcPllFllSel);
#endif

    // Enable the clock for LPSCI module
    CLOCK_SYS_EnableLpsciClock(BOARD_DEBUG_UART_INSTANCE);

    // Gets the clock frequency for LPSCI module
    lpsciSourceClock = CLOCK_SYS_GetLpsciFreq(BOARD_DEBUG_UART_INSTANCE);

    // Initialize LPSCI baud rate, bit count, parity and stop bit
    LPSCI_HAL_SetBaudRate(baseAddr, lpsciSourceClock, BOARD_DEBUG_UART_BAUD);
    LPSCI_HAL_SetBitCountPerChar(baseAddr, kLpsci8BitsPerChar);
    LPSCI_HAL_SetParityMode(baseAddr, kLpsciParityDisabled);

#if FSL_FEATURE_LPSCI_HAS_STOP_BIT_CONFIG_SUPPORT
    LPSCI_HAL_SetStopBitCount(baseAddr, kLpsciOneStopBit);
#endif

    // Finally, enable the LPSCI transmitter and receiver
    LPSCI_HAL_EnableTransmitter(baseAddr);
    LPSCI_HAL_EnableReceiver(baseAddr);

    // Inform to start polling example
    byteCountBuff = sizeof(buffStart);
    LPSCI_HAL_SendDataPolling(baseAddr, buffStart, byteCountBuff);

    // Inform user of what to do
    byteCountBuff = sizeof(bufferData1);
    LPSCI_HAL_SendDataPolling(baseAddr, bufferData1, byteCountBuff);

    while(1)
    {
        // Wait to receive input data
        if (kStatus_LPSCI_Success == LPSCI_HAL_ReceiveDataPolling(baseAddr, &rxChar,1))
        {
            // Echo received character
            LPSCI_HAL_SendDataPolling(baseAddr, &rxChar, 1);
        }
    }

}

/*******************************************************************************
 * EOF
 ******************************************************************************/
