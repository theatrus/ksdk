/*******************************************************************************
*
* Copyright [2014-]2014 Freescale Semiconductor, Inc.

*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale License
* distributed with this Material.
* See the LICENSE file distributed for more details.
*
*
*******************************************************************************/

#include <stdio.h>
#include "board.h"
#include "fsl_os_abstraction.h"
#include "fsl_debug_console.h"
#include "fsl_interrupt_manager.h"
#include "fsl_clock_manager.h"
#include "fsl_gpio_driver.h"
#include "fsl_pwm_driver.h"
#include "fsl_uart_driver.h"
#include "fsl_port_hal.h"

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

    moduleParams.clkSrc = kClkSrcPwmIPBusClk;  /* Use IP Bus clock as source */
    moduleParams.prescale = kPwmDividedBy1;
    moduleParams.cntrInitSel = kInitSrcLocalSync;
    moduleParams.reloadLogic = kFlexPwmReloadPwmFullCycle;
    moduleParams.reloadFreq = kPwmLoadEvery1Oportunity;
    moduleParams.chnlPairOper = kFlexPwmComplementaryPwmA;
    moduleParams.forceTrig = kForceOutputLocalForce;

    signalParams.pwmPeriod = 1000;
    signalParams.pwmType = kFlexPwmSignedCenterAligned;
    signalParams.pwmAPulseWidth = 10;
    signalParams.pwmBPulseWidth = FLEXPWM_NO_PWM_OUT_SIGNAL;
    signalParams.pwmAPolarity = false;
    signalParams.pwmBPolarity = false;

    /*********** PWMA_SM0 - phase A, configuration ************/
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

    PWM_DRV_SetupPwm(instance, kFlexPwmModule2, &moduleParams, &signalParams);

    /* Set deadtime count */
    PWM_BWR_DTCNT0_DTCNT0(pwmBase, kFlexPwmModule2, deadTimeVal);
    PWM_BWR_DTCNT1_DTCNT1(pwmBase, kFlexPwmModule2, deadTimeVal);

    /* Disable faults for PWM A & B */
    PWM_BWR_DISMAP_DIS0A(pwmBase, kFlexPwmModule2, 0, 0x0);
    PWM_BWR_DISMAP_DIS0B(pwmBase, kFlexPwmModule2, 0, 0x0);

    return kStatusPwmSuccess;
}


/*******************************************************************************
* Variables
******************************************************************************/
int main(void)
{
    static uint16_t delay;
    pwm_status_t PwmStatus;
    uint32_t pwmVal = 10;
    pwm_module_signal_setup_t signalParams;
    uint16_t i;

    signalParams.pwmPeriod = 1000; /* PWM signal period is 1000 microseconds (1KHz) */
    signalParams.pwmType = kFlexPwmSignedCenterAligned;
    signalParams.pwmAPulseWidth = pwmVal; /* PWM pulse width, start with 10 microseconds */
    signalParams.pwmBPulseWidth = FLEXPWM_NO_PWM_OUT_SIGNAL; /* Use only PWM A */
    signalParams.pwmAPolarity = false; /* PWM output is not inverted */
    signalParams.pwmBPolarity = false;

    /* Init hardware, OSA, dbg console */
    hardware_init();
    OSA_Init();

    PRINTF("Welcome to EflexPWM demo!\r\n");

    delay = 0x0fffU;

    PWM_DRV_Init(0U);

    /* call the init function with demo configuration */
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

    while (1U)
    {
        for (i = 0U; i < delay; i++)
        {
            __ASM volatile ("nop");
        }
        pwmVal++;

        if (signalParams.pwmAPulseWidth > signalParams.pwmPeriod)
        {
            pwmVal = 10;
        }

        signalParams.pwmAPulseWidth = pwmVal;
        PWM_DRV_UpdatePwmSignal(0, kFlexPwmModule0, &signalParams);
        signalParams.pwmAPulseWidth = pwmVal >> 2;
        PWM_DRV_UpdatePwmSignal(0, kFlexPwmModule1, &signalParams);
        signalParams.pwmAPulseWidth = pwmVal >> 4;
        PWM_DRV_UpdatePwmSignal(0, kFlexPwmModule2, &signalParams);
    }
}

