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

#if !defined(__FSL_FLEXIO_IRDA_H__)
#define      __FSL_FLEXIO_IRDA_H__

#include <stdint.h>
#include "fsl_flexio_hal.h"

/*!
 * @addtogroup flexio_irda
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
//User should define IRDA_DUTY1 to their expected value or use other definition
//DUTY value never exceed 50%
//Pulse IRDA
#define IRDA_PULSE_DUTY1   3/16
#define IRDA_PULSE_DUTY2   1/16
#define IRDA_PULSE_DUTY3   3/32
#define IRDA_PULSE_DUTY4   1/4
#define IRDA_PULSE_DUTY5   8/16
     
//IRDA mode
#define IRDA_PULSE          1

#if IRDA_PULSE
#define IRDA_DUTY    IRDA_PULSE_DUTY1
#endif


//User should define IRDA PIN mode acccordingly to their board setup
//For loop back test, these two value should be the same
//IRDA_PULSE waveform example at MCU PIN
//REVERSE = 0 waveform
//___________|_||_|_|_||___
//REVERSE = 1 waveform
//__________________________
//           | | | | | | |     
#define IRDA_RXPIN_REVERSE  0
#define IRDA_TXPIN_REVERSE  0



//use CMP0 output or not, define 1 to use CMP0 out as IRDA rx ext trigger
//if using CMP0 output, CMP0 output pulse to FlexIO timer for decoding
//if not using CMP0 output, CMP0 output direct connect to FlexIO simulate UART RX
#define IRDA_RX_CMP0_TRIG    0

#if IRDA_RX_CMP0_TRIG
#define IRDA_RX_CMP0_SOURCE         1
#define IRDA_RX_CMP0_IN_THREAD     2100
#define IRDA_RX_CMP0_DACREF_VOLTAGE 3300
#define IRDA_RX_CMP0_DAC_VALUE (IRDA_RX_CMP0_IN_THREAD*64/IRDA_RX_CMP0_DACREF_VOLTAGE)
#endif
   
typedef struct _flexio_irda_config {
    uint32_t timerIdx;
    uint32_t trigPinIdx;
    uint32_t timerPinIdx;
    uint32_t baudrate;
    uint32_t flexioFrequency;                                   
} flexio_irda_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initialize FlexIO IRDA Tx Encoding
 *
 * @param flexioBase base address for Flexio
 * @param txConfig IRDA TX Encoding configuration data
 */
flexio_status_t FLEXIO_UART_IRDA_ConfigureTxEncoding(FLEXIO_Type * flexioBase,
                                                     flexio_irda_config_t * rxConfig);

/*!
 * @brief Initialize FlexIO IRDA Rx Encoding
 *
 * @param flexioBase base address for Flexio
 * @param rxConfig IRDA RX Decoding configuration data
 */
flexio_status_t FLEXIO_UART_IRDA_ConfigureRxDecoding(FLEXIO_Type * flexioBase,
                                                     flexio_irda_config_t * txConfig);

/*!
 * @brief Initialize FlexIO IRDA Tx/Rx initialization
 *
 * @param flexioBase base address for Flexio
 * @param rxConfig IRDA RX Decoding configuration data
 * @param txConfig IRDA TX Encoding configuration data
 */
flexio_status_t FLEXIO_IRDA_Init               (FLEXIO_Type * flexioBase,
                                                flexio_irda_config_t * rxConfig,
                                                flexio_irda_config_t * txConfig);

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* __FSL_FLEXIO_IRDA_H__*/
/*******************************************************************************
 * EOF
 ******************************************************************************/
