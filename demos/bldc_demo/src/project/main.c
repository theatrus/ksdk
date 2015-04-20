/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2011 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
******************************************************************************
*
* @file      main.c
*
* @author    Petr Staszko
*
* @version   1.0.0.0
*
* @date      15.8-2013
*
* @brief     Sensorless BLDC motor control application
*
* @remark
*
*******************************************************************************
*
* The main file of Sensorless BLDC motor control application for Kinetis KV10
*
******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "peripherals_init.h"
#include "freemaster.h"
#include "M1_StateMachine.h" // Motor 1's state machine
#include "motor_structure.h"

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

extern MCSTRUC_BLDC_SNLS_INT_T gsM1_Drive;
volatile UInt16 uw16TriggerErrorCnt;

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

void UpdateBldcHwModules(MCSTRUC_BLDC_SNLS_INT_T *);
extern void ReadSwFaults(MCSTRUC_BLDC_SNLS_INT_T *);
extern void TimeEvent(MCSTRUC_BLDC_SNLS_INT_T *);
extern void FastControlLoop(MCSTRUC_BLDC_SNLS_INT_T *);
extern void SlowControlLoop(MCSTRUC_BLDC_SNLS_INT_T *);
extern void Demonstration(MCSTRUC_BLDC_SNLS_INT_T *);

int main(void);

int main(void)
{
    DisableInterrupts;

    MCU_init();

    FMSTR_Init();
    // Application variables init
    SM_StateMachine(&gsM1_Ctrl);

    // Enable Interrupts globally
    EnableInterrupts;

    M1_SetAppSwitch(1);

    // Infinite loop
    while(1)
    {
        FMSTR_Poll();
    }
}

/*
 * @brief   ADC Callback function
 *
 * Description:
 * Three analogue signals are measured by the ADC module
 *
 *   1) DC Bus current  - triggered by H/W
 *   2) Phase voltage   - triggered by H/W (Back to back PDB mode)
 *   3) DC Bus voltage  - triggered by SW
 *
 * After second ADC sample is converted (2), ADC interrupt service routine
 * is executed.
 * PDB module is disabled (to avoid error due to Back-to-back mode of PDB)
 * ADC is configured to S/W triggered mode
 * Third ADC sample is converted (3) (ADC started by S/W trigger, pool mode)
 * PDB module is enabled
 * ADC is configured to H/W triggered mode
 * ADC channels are set back for next measurements
 *
 */
void ADC0_IRQHandler(void)
{
    GPIO_HAL_SetPinOutput(GPIOD_BASE, 6);
    // Read timer counter and value registers
    gsM1_Drive.uw16TimeCurrent = FTM_HAL_GetCounter(FTM1_BASE);
    gsM1_Drive.uw16TimeCurrentEvent = FTM_HAL_GetChnCountVal(FTM1_BASE, 0);
    // Read ADC results
    // (ADC triggered by H/W trigger from PDB)
    // 15-bit positive number
    gsM1_Drive.f16DcBusCurrentRaw = ADC16_HAL_GetChnConvValueRAW(ADC0_BASE,0 ) << 3;
    // Left justified 12-bit result
    gsM1_Drive.f16PhaseVoltage = ADC16_HAL_GetChnConvValueRAW(ADC0_BASE,1 ) << 3;
    // - Disable PDB module to avoid error due to
    //   Back-to-back mode of PDB
    // - Configure ADC to S/W trigger mode
    // Disable PDB channel 0
    PDB_HAL_SetChnC1Reg(PDB0_BASE, 0, 0);
    // Set ADC to S/W trigger
    ADC16_HAL_SetHwTriggerCmd(ADC0_BASE, false);
    // - Start of conversion by S/W, wait for end
    // - Read ADC result
    ADC16_HAL_ConfigChn(ADC0_BASE, 0,false, false, ADC_CHANNEL_DCBV);
    while (!ADC16_HAL_GetChnConvCompletedCmd(ADC0_BASE, 0) ) {}
    // Left justified 12-bit result
    gsM1_Drive.f16DcbVoltage = ADC16_HAL_GetChnConvValueRAW(ADC0_BASE,0 ) << 3;

    // - Enable PDB module
    // - Configure ADC to H/W trigger mode
    // Enable PDB channel 0   TOS3 EN3 BB2
    PDB_HAL_SetChnC1Reg(PDB0_BASE, 0, 131843);

    ADC16_HAL_SetHwTriggerCmd(ADC0_BASE, true);

    // Configure ADC channel A for next measurement
    ADC16_HAL_ConfigChn(ADC0_BASE, 0,false, false, ADC_CHANNEL_DCBI);

    gsM1_Drive.f16DcBusCurrentNoFiltered = MLIB_ShLSat_F16(gsM1_Drive.f16DcBusCurrentRaw - gsM1_Drive.f16DcBusCurrentOffset, 1);
    // Filtered DC Bus current value
    gsM1_Drive.f16DcBusCurrent = GDFLIB_FilterMA_F16(gsM1_Drive.f16DcBusCurrentNoFiltered, &gsM1_Drive.trCurrentMaFilter);
    gsM1_Drive.f16PhaseBemf = gsM1_Drive.f16PhaseVoltage - (gsM1_Drive.f16DcbVoltage >> 1);

    // Back-EMF voltage (must be located before FastControlLoop, because it is modified in FastControlLoop)
    FMSTR_Recorder();

    FastControlLoop(&gsM1_Drive);

    // Configure ADC channel B for next measurement
    // w16CmtSector may be modified in FastControlLoop(...)
    ADC16_HAL_ConfigChn(ADC0_BASE, 1,true, false, bldcAdcSectorCfg[gsM1_Drive.w16CmtSector]);

    ReadSwFaults(&gsM1_Drive);

    SM_StateMachine(&gsM1_Ctrl);

    UpdateBldcHwModules(&gsM1_Drive);

    GPIO_HAL_ClearPinOutput(GPIOD_BASE,6);

}

/*
 * @brief   FTM1 interrupt service routine
 * - Forced commutation control
 */
void FTM1_IRQHandler(void)
{
    // Read timer counter and value registers
    gsM1_Drive.uw16TimeCurrent = FTM_HAL_GetCounter(FTM1_BASE);
    gsM1_Drive.uw16TimeCurrentEvent = FTM_HAL_GetChnCountVal(FTM1_BASE, 0);

    TimeEvent(&gsM1_Drive);

    SM_StateMachine(&gsM1_Ctrl);

    UpdateBldcHwModules(&gsM1_Drive);

    FTM_HAL_ClearChnEventFlag(FTM1_BASE, 0);/*Clear flag*/
}

/*
 * @brief   FTM2 interrupt service routine
 * - slow control loop
 * - Low priority ISR
 */
void FTM2_IRQHandler(void)
{
    uint32_t temp = 0;
    SlowControlLoop(&gsM1_Drive);

    MC33927_readStatus();

    Demonstration(&gsM1_Drive);                          //only for demo demonstration
    temp = FTM_HAL_GetChnCountVal(FTM2_BASE, 0)+SLOW_TIMER_PERIOD;
    FTM_HAL_SetChnCountVal(FTM2_BASE, 0, temp);
    FTM_HAL_ClearChnEventFlag(FTM2_BASE, 0);/*Clear flag*/
}

/*
 * @brief   PDB0 interrupt service routine
 * Trigger error clear routine
 */
void PDB0_IRQHandler(void)
{
    if (PDB_HAL_GetPreTriggerSeqErrFlag(PDB0_BASE, 0, 0))
    {
        if (uw16TriggerErrorCnt < 0xffff) uw16TriggerErrorCnt++;
        // Disable PDB
        PDB_HAL_Disable(PDB0_BASE);
        PDB_HAL_ClearPreTriggerSeqErrFlag(PDB0_BASE, 0, 0);
        // Enable PDB
        PDB_HAL_Enable(PDB0_BASE);
    }

}

/*
 * @brief Update of W/H modules used by BLDC algorithm based on request flags
 */
void UpdateBldcHwModules(MCSTRUC_BLDC_SNLS_INT_T * sM_Drive)
{
    // Disable PWM outputs
    // If uw16TriggerErrorCnt > 1 then there is probably issue with ADC ISR (it is probably not executed), therefore PWM outputs are disabled to protect output MOSFETs; 1 error is allowed.
    // "uw16TriggerErrorCnt > 1" condition should be deleted in the final application.
    if ((sM_Drive->uw16HwUpdateRequests & UPDATE_PWM_DISABLE_REQ) | (uw16TriggerErrorCnt > 1))
    {
        sM_Drive->uw16HwUpdateRequests &= ~(UPDATE_PWM_DISABLE_REQ | UPDATE_BRAKE_RESISTOR_ON_REQ | UPDATE_DUTYCYCLE_REQ | UPDATE_PWM_CONFIG_REQ | UPDATE_PWM_ALIGNMENT_REQ);
        // Disable PWM outputs
        FTM0_SetPwmOutput(7);
        GPIO_HAL_ClearPinOutput(GPIOE_BASE, 24);
    }

    // Request to set new time event (update timer value register)
    if (sM_Drive->uw16HwUpdateRequests & UPDATE_TIME_EVENT_REQ)
    {
        sM_Drive->uw16HwUpdateRequests &= ~UPDATE_TIME_EVENT_REQ;
        FTM1->CONTROLS[0].CnV = sM_Drive->uw16TimeNextEvent;
    }

    // Update PWM duty cycle if requested
    if (sM_Drive->uw16HwUpdateRequests & UPDATE_DUTYCYCLE_REQ)
    {
        sM_Drive->uw16HwUpdateRequests &= ~UPDATE_DUTYCYCLE_REQ;
        FTM0_SetDutyCycle(sM_Drive->f16DutyCycle);
    }

    // Apply commutation vector (alternatively enable PWM outputs)
    if (sM_Drive->uw16HwUpdateRequests & UPDATE_PWM_CONFIG_REQ)
    {
        sM_Drive->uw16HwUpdateRequests &= ~UPDATE_PWM_CONFIG_REQ;
        // Apply commutation vector
        FTM0_SetPwmOutput(sM_Drive->w16CmtSector);
    }

    // Apply alignment vector
    if (sM_Drive->uw16HwUpdateRequests & UPDATE_PWM_ALIGNMENT_REQ)
    {
        sM_Drive->uw16HwUpdateRequests &= ~UPDATE_PWM_ALIGNMENT_REQ;
        // Apply alignment vector
        FTM0_SetPwmOutput(6);
    }

    // Brake resistor
    if (sM_Drive->uw16HwUpdateRequests & UPDATE_BRAKE_RESISTOR_ON_REQ) {
        GPIO_HAL_SetPinOutput(GPIOE_BASE, 24);
    }
    else
    {
        GPIO_HAL_ClearPinOutput(GPIOE_BASE, 24);
    }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
