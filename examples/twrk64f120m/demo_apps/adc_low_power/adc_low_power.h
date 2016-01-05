/*******************************************************************************
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.                           *
 * All rights reserved.                                                        *
 *                                                                             *
 * Redistribution and use in source and binary forms, with or without modifi-  *
 * cation, are permitted provided that the following conditions are met:       *
 *                                                                             *
 * o Redistributions of source code must retain the above copyright notice,    *
 * this list of conditions and the following disclaimer.                       *
 *                                                                             *
 * o Redistributions in binary form must reproduce the above copyright notice, *
 * this list of conditions and the following disclaimer in the documentation   *
 * and/or other materials provided with the distribution.                      *
 *                                                                             *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its    *
 *   contributors may be used to endorse or promote products derived from this *
 *   software without specific prior written permission.                       *
 *                                                                             *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" *
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  *
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   *
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         *
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        *
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    *
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     *
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     *
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  *
 * POSSIBILITY OF SUCH DAMAGE.                                                 *
 *                                                                             *
 *******************************************************************************/


#ifndef __LOWPOWER_ADC_H__
#define __LOWPOWER_ADC_H__

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

/*!
* @brief Boundaries struct
*/
typedef struct lowPowerAdcBoundaries
{
    int32_t upperBoundary;
    int32_t lowerBoundary;
} lowPowerAdcBoundaries_t;

/*!
 * @brief Low Power Timer Interrupt handler. Clear LPT Compare flag.
 */
void LowPowerTimerIRQHandler(void);

/*!
 * @brief ADC Interrupt handler. Get current ADC value and set conversionCompleted flag.
 */
void ADC1IRQHandler(void);

/*!
 * @brief Initialize Low Power Timer. Use 1 kHz LPO with no preescaler and enable LPT interrupt.
 */
void InitLowPowerTmr(void);

/*!
 * @brief Calculate current temperature.
 *
 * @return uint32_t Returns current temperature.
 */
int32_t GetCurrentTempValue(void);

/*!
 * @brief Calculate current temperature.
 *
 * @param updateBoundariesCounter Indicate number of values into tempArray.
 *
 * @param tempArray Store temperature value.
 *
 * @return lowPowerAdcBoundaries_t Returns upper and lower temperature boundaries.
 */
lowPowerAdcBoundaries_t TempSensorCalibration(uint32_t updateBoundariesCounter,
                                                     int32_t *tempArray);

/*!
 * @brief User-defined function to install callback.
 */
void ADC_TEST_InstallCallback(uint32_t instance, uint32_t chnGroup, void (*callbackFunc)(void) );

/*!
 * @brief User-defined function to read conversion value in ADC ISR. 
 */
uint16_t ADC_TEST_GetConvValueRAWInt(uint32_t instance, uint32_t chnGroup);

#endif // __LOWPOWER_ADC_H__
