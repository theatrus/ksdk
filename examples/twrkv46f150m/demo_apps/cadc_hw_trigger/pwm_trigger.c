/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
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
#include <stdio.h>
#include "cadc_hw_trigger.h"
#include "fsl_pwm_hal.h"
#include "fsl_pwm_driver.h"
#include "fsl_xbar_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

extern SIM_Type* const gSimBase[];

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*FUNCTION**********************************************************************
 *
 * Function Name : PWM_DRV_Init3PhPwm
 * Description   : Initializes the PWM driver.
 *
 *END**************************************************************************/
static pwm_status_t PWM_DRV_Init3PhPwm(uint8_t instance)
{
    assert(instance < PWM_INSTANCE_COUNT);
    uint16_t deadTimeVal;
    PWM_Type *pwmBase = g_pwmBase[instance];
    pwm_module_setup_t moduleParams;
    pwm_module_signal_setup_t signalParams;

    moduleParams.clkSrc = kClkSrcPwmIPBusClk;
    moduleParams.prescale = kPwmDividedBy1;
    moduleParams.cntrInitSel = kInitSrcLocalSync;
    moduleParams.reloadLogic = kFlexPwmReloadPwmFullCycle;
    moduleParams.reloadFreq = kPwmLoadEvery10Oportunity;
    moduleParams.chnlPairOper = kFlexPwmComplementaryPwmA;
    moduleParams.forceTrig = kForceOutputLocalForce;

    signalParams.pwmPeriod = 1200;
    signalParams.pwmType = kFlexPwmSignedCenterAligned;
    signalParams.pwmAPulseWidth = 10;
    signalParams.pwmBPulseWidth = FLEXPWM_NO_PWM_OUT_SIGNAL;
    signalParams.pwmAPolarity = false;
    signalParams.pwmBPolarity = false;

    /*********** PWMA_SM0 - phase A, configuration ************/
    /* Setup trigger */
    PWM_DRV_SetTriggerVal(0U, kFlexPwmModule0, kFlexPwmVAL4, 0xff80U);
    PWM_DRV_SetTriggerCmd(0U, kFlexPwmModule0, kFlexPwmVAL4, true);
    PWM_DRV_SetTriggerCmd(0U, kFlexPwmModule0, kFlexPwmVAL0, true);

    PWM_DRV_SetupPwm(instance, kFlexPwmModule0, &moduleParams, &signalParams);

    /* Set deadtime count, we set this to about 650ns */
    deadTimeVal = ((uint64_t)CLOCK_SYS_GetPwmFreq(0) * 650) / 1000000000;
    PWM_BWR_DTCNT0_DTCNT0(pwmBase, kFlexPwmModule0, deadTimeVal);
    PWM_BWR_DTCNT1_DTCNT1(pwmBase, kFlexPwmModule0, deadTimeVal);

    /* Disable faults for PWM A & B */
    PWM_BWR_DISMAP_DIS0A(pwmBase, kFlexPwmModule0, 0, 0x0);
    PWM_BWR_DISMAP_DIS0B(pwmBase, kFlexPwmModule0, 0, 0x0);

    /*********** PWMA_SM1 - phase B configuration ************/
    moduleParams.clkSrc = kClkSrcPwm0Clk;
    moduleParams.cntrInitSel = kInitSrcMasterSync;

    /* Setup trigger */
    PWM_DRV_SetTriggerVal(0U, kFlexPwmModule1, kFlexPwmVAL4, 0x0U);
    PWM_DRV_SetTriggerCmd(0U, kFlexPwmModule1, kFlexPwmVAL4, true);

    PWM_DRV_SetupPwm(instance, kFlexPwmModule1, &moduleParams, &signalParams);

    /* Set deadtime count */
    PWM_BWR_DTCNT0_DTCNT0(pwmBase, kFlexPwmModule1, deadTimeVal);
    PWM_BWR_DTCNT1_DTCNT1(pwmBase, kFlexPwmModule1, deadTimeVal);

    /* Disable faults for PWM A & B */
    PWM_BWR_DISMAP_DIS0A(pwmBase, kFlexPwmModule1, 0, 0x0);
    PWM_BWR_DISMAP_DIS0B(pwmBase, kFlexPwmModule1, 0, 0x0);

    /*********** PWMA_SM2 - phase C configuration ************/
    moduleParams.clkSrc = kClkSrcPwm0Clk;
    moduleParams.cntrInitSel = kInitSrcMasterSync;

    /* Setup trigger */
    PWM_DRV_SetTriggerVal(0U, kFlexPwmModule2, kFlexPwmVAL4, 0x0U);
    PWM_DRV_SetTriggerCmd(0U, kFlexPwmModule2, kFlexPwmVAL4, true);

    PWM_DRV_SetupPwm(instance, kFlexPwmModule2, &moduleParams, &signalParams);

    /* Set deadtime count */
    PWM_BWR_DTCNT0_DTCNT0(pwmBase, kFlexPwmModule2, deadTimeVal);
    PWM_BWR_DTCNT1_DTCNT1(pwmBase, kFlexPwmModule2, deadTimeVal);

    /* Disable faults for PWM A & B */
    PWM_BWR_DISMAP_DIS0A(pwmBase, kFlexPwmModule2, 0, 0x0);
    PWM_BWR_DISMAP_DIS0B(pwmBase, kFlexPwmModule2, 0, 0x0);

    return kStatusPwmSuccess;
}

/*!
 * @Brief enable the trigger source of PWM
 */
void init_trigger_source(uint32_t adcInstance)
{
    pwm_status_t PwmStatus;
    uint8_t pwmsubmod = 0;

    xbar_state_t xbarStatePtr;

    PWM_DRV_Init(0U);

    PwmStatus = PWM_DRV_Init3PhPwm(0U);
    if (PwmStatus == kStatusPwmSuccess)
    {
        PRINTF("PWM initialization passed!\r\n");
    }
    else
    {
        PRINTF("PWM initialization failed!\r\n");
    }

    PWM_DRV_CounterStart(0U, 0x7);

    XBAR_DRV_Init(&xbarStatePtr);
    XBAR_DRV_ConfigSignalConnection(kXbaraInputPWM0_TRG0,kXbaraOutputADCA_TRIG);

    // Configure SIM for ADC hw trigger source selection
    SIM_HAL_SetAdcAlternativeTriggerCmd(gSimBase[0], kSimCAdcConvA, kSimAdcTrgenXbarout);
}

/*!
* @Brief disable the trigger source
*/
void deinit_trigger_source(uint32_t adcInstance)
{
    PWM_DRV_Deinit(0U);
}
