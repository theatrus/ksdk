/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 * of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
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

#ifndef __MAIN_H__
#define __MAIN_H__
/*Please note that FlexIO pin number defined here should be aligned with hardware_init.c */
#define FLEXIO_INSTANCE                 0
#define CMP_INSTANCE                    0
#define LOOPBACK_TEST                   1

#define FLEXIO_UART_IRDA_RX_PIN         6
#define FLEXIO_UART_IRDA_RX_TIMER       2

#define FLEXIO_UART_IRDA_TX_PIN         7
#define FLEXIO_UART_IRDA_TX_TIMER       3

#define FLEXIO_UART_RX_PIN              5
#define FLEXIO_UART_RX_SHIFTER          0
#define FLEXIO_UART_RX_TIMER            0
#define FLEXIO_UART_RX_PINEN            1

#define FLEXIO_UART_TX_PIN              4
#define FLEXIO_UART_TX_SHIFTER          1
#define FLEXIO_UART_TX_TIMER            1
#define FLEXIO_UART_TX_PINEN            1

#define FLEXIO_UART_BAUDRATE            9600 
#define FLEXIO_UART_NUMOFBITS           8



#endif
