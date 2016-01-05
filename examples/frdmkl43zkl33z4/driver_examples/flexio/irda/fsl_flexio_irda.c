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

#include <assert.h>
#include "fsl_flexio_hal.h"
#include "fsl_flexio_uart_hal.h"
#include "fsl_flexio_irda.h"

/*FUNCTION****************************************************************
 *
 * Function Name: FLEXIO_IRDA_ConfigureRxDecoding
 * Description: Configure IRDA RX related timer
 *
 *END*********************************************************************/
flexio_status_t FLEXIO_IRDA_ConfigureRxDecoding(FLEXIO_Type * flexioBase,
                                                flexio_irda_config_t * rxConfig)
{
    flexio_timer_config_t   mFlexioTimerConfigStruct;
    uint32_t divider;

    /* Configure the timer for decoding. */
#if IRDA_RX_CMP0_TRIG
    mFlexioTimerConfigStruct.trgsel = (IRDA_RX_CMP0_SOURCE);
    mFlexioTimerConfigStruct.trgsrc = kFlexioTimerTriggerSourceExternal;
#else
    mFlexioTimerConfigStruct.trgsel = FLEXIO_HAL_TIMER_TRIGGER_SEL_PININPUT(rxConfig->trigPinIdx);
    mFlexioTimerConfigStruct.trgsrc = kFlexioTimerTriggerSourceInternal;
#endif
    mFlexioTimerConfigStruct.trgpol = (flexio_timer_trigger_polarity_t)IRDA_RXPIN_REVERSE;
    mFlexioTimerConfigStruct.pincfg = kFlexioPinConfigOutput;    
    mFlexioTimerConfigStruct.pinsel = rxConfig->timerPinIdx;    
    mFlexioTimerConfigStruct.pinpol = kFlexioPinActiveLow;
    mFlexioTimerConfigStruct.timod  = kFlexioTimerModeDual8BitBaudBit; 
    mFlexioTimerConfigStruct.timout = kFlexioTimerOutputOneNotAffectedByReset;
    mFlexioTimerConfigStruct.timdec = kFlexioTimerDecSrcOnFlexIOClockShiftTimerOutput;
    mFlexioTimerConfigStruct.timrst = kFlexioTimerResetOnTimerTriggerRisingEdge;
    mFlexioTimerConfigStruct.timdis = kFlexioTimerDisableOnTimerCompare;
    mFlexioTimerConfigStruct.timena = kFlexioTimerEnableOnTriggerHigh;
    mFlexioTimerConfigStruct.tstop  = kFlexioTimerStopBitDisabled;
    mFlexioTimerConfigStruct.tstart = kFlexioTimerStartBitDisabled;
    divider = (rxConfig->flexioFrequency / rxConfig->baudrate)+2;
    mFlexioTimerConfigStruct.timcmp = divider;
    FLEXIO_HAL_ConfigureTimer(flexioBase, rxConfig->timerIdx, &mFlexioTimerConfigStruct);
    return kStatus_FLEXIO_Success;
}


/*FUNCTION****************************************************************
 *
 * Function Name: FLEXIO_IRDA_ConfigureTxEncoding
 * Description: Configure IRDA TX related timer
 *
 *END*********************************************************************/
flexio_status_t FLEXIO_IRDA_ConfigureTxEncoding(FLEXIO_Type * flexioBase,
                                                     flexio_irda_config_t * txConfig)
{
    flexio_timer_config_t   mFlexioTimerConfigStruct;
    uint32_t divider, timPwm;
    uint16_t pwmHighTime, pwmLowTime;
#if IRDA_PULSE    
    divider = ((txConfig->flexioFrequency / txConfig->baudrate)-2)/2;
    if(divider>255)
    {
        divider = 255;
    }
    pwmHighTime = (divider*IRDA_DUTY*2) - 1;
    pwmLowTime = divider - pwmHighTime;
    timPwm  = ((pwmLowTime)<<8)|((pwmHighTime));
    mFlexioTimerConfigStruct.timcmp = timPwm;
#endif
    
    /* Configure the timer for encoding */
    mFlexioTimerConfigStruct.trgsel = FLEXIO_HAL_TIMER_TRIGGER_SEL_PININPUT(txConfig->trigPinIdx);
    mFlexioTimerConfigStruct.trgpol = kFlexioTimerTriggerPolarityActiveLow;
    mFlexioTimerConfigStruct.trgsrc = kFlexioTimerTriggerSourceInternal;
    mFlexioTimerConfigStruct.pincfg = kFlexioPinConfigOutput;
    mFlexioTimerConfigStruct.pinsel = txConfig->timerPinIdx;
    mFlexioTimerConfigStruct.pinpol = (flexio_pin_polarity_t) IRDA_TXPIN_REVERSE;
    mFlexioTimerConfigStruct.timod  = kFlexioTimerModeDual8BitPWM; 
    mFlexioTimerConfigStruct.timout = kFlexioTimerOutputOneNotAffectedByReset;
    mFlexioTimerConfigStruct.timdec = kFlexioTimerDecSrcOnFlexIOClockShiftTimerOutput;
    mFlexioTimerConfigStruct.timrst = kFlexioTimerResetNever;
    mFlexioTimerConfigStruct.timdis = kFlexioTimerDisableOnTimerCompare;
    mFlexioTimerConfigStruct.timena = kFlexioTimerEnableOnTriggerHigh;
    mFlexioTimerConfigStruct.tstop  = kFlexioTimerStopBitEnableOnTimerDisable;
    mFlexioTimerConfigStruct.tstart = kFlexioTimerStartBitDisabled;
    FLEXIO_HAL_ConfigureTimer(flexioBase, txConfig->timerIdx, &mFlexioTimerConfigStruct);
    return kStatus_FLEXIO_Success;
}

flexio_status_t FLEXIO_IRDA_Init(FLEXIO_Type * flexioBase,
                                 flexio_irda_config_t * rxConfig,
                                 flexio_irda_config_t * txConfig)
{
    flexio_status_t flexio_status;
    flexio_status = FLEXIO_IRDA_ConfigureTxEncoding(flexioBase, txConfig);
    if(flexio_status)
    {
        return flexio_status;
    }
    flexio_status = FLEXIO_IRDA_ConfigureRxDecoding(flexioBase, rxConfig);
    return flexio_status;
}

                                                     

