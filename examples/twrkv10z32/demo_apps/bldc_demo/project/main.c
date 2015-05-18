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

extern adc16_converter_config_t adcUserConfig;
extern adc16_chn_config_t adcChnConfig;
extern adc16_hw_cmp_config_t adcHwCmpConfig;


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
  gsM1_Drive.uw16TimeCurrent = FTM_HAL_GetCounter(FTM1_BASE_PTR);
  gsM1_Drive.uw16TimeCurrentEvent = FTM_HAL_GetChnCountVal(FTM1_BASE_PTR, 0);

  // Read ADC results
  // (ADC triggered by H/W trigger from PDB)
  gsM1_Drive.f16DcBusCurrentRaw = ADC16_HAL_GetChnConvValue(ADC0_BASE_PTR,0U)<<3;
  gsM1_Drive.f16PhaseVoltage = ADC16_HAL_GetChnConvValue(ADC0_BASE_PTR,1U)<<3;

  // - disable PDB module to avoid error due to Back-to-back mode of PDB
  // - configure ADC to S/W trigger mode
  // disable PDB channel 0
  /* Note! Use feature extension MACRO here is better to save precious ISR time, 
     * through the HAL driver canwork well in most cases.
     * PDB_HAL_SetAdcPreTriggerBackToBackEnable(PDB0_BASE_PTR, 0, (1U<<0)|(1U<<1), false);
     * PDB_HAL_SetAdcPreTriggerOutputEnable(PDB0_BASE_PTR, 0, (1U<<0)|(1U<<1), false);
     * PDB_HAL_SetAdcPreTriggerEnable(PDB0_BASE_PTR, 0, (1U<<0)|(1U<<1), false);
     */
  PDB_WR_C1(PDB0_BASE_PTR, 0U, 0U);

  // set ADC to S/W trigger
  adcUserConfig.hwTriggerEnable = false;
  ADC16_HAL_ConfigConverter(ADC0_BASE_PTR, &adcUserConfig);

  // - start of conversion by S/W, wait for end
  // - read ADC result
  adcChnConfig.chnIdx = (adc16_chn_t)ADC_CHANNEL_DCBV;
  adcChnConfig.convCompletedIntEnable = false;
  adcChnConfig.diffConvEnable = false;
  ADC16_HAL_ConfigChn(ADC0_BASE_PTR, 0U, &adcChnConfig);
  while (!ADC16_HAL_GetChnConvCompletedFlag(ADC0_BASE_PTR,0U) ) {}
  gsM1_Drive.f16DcbVoltage = ADC16_HAL_GetChnConvValue(ADC0_BASE_PTR,0U)<<3; // left justified 12-bit result

  // - enable PDB module
  /* Note, Use feature extension MACRO here is to save precious ISR time, 
     *  through the HAL driver canwork well in most cases.
     * PDB_HAL_SetAdcPreTriggerBackToBackEnable(PDB0_BASE_PTR, 0, (1U<<1), true);
     * PDB_HAL_SetAdcPreTriggerOutputEnable(PDB0_BASE_PTR, 0, (1U<<0)|(1U<<1), true);
     * PDB_HAL_SetAdcPreTriggerEnable(PDB0_BASE_PTR, 0, (1U<<0)|(1U<<1), true);
     */
  PDB_WR_C1(PDB0_BASE_PTR, 0U, PDB_C1_TOS(3) | PDB_C1_EN(3) | PDB_C1_BB(2));

  // - configure ADC to H/W trigger mode
  adcUserConfig.hwTriggerEnable = true;
  ADC16_HAL_ConfigConverter(ADC0_BASE_PTR,&adcUserConfig);
  // Configure ADC channel A for next measurement
  adcChnConfig.chnIdx = (adc16_chn_t)ADC_CHANNEL_DCBI;
  ADC16_HAL_ConfigChn(ADC0_BASE_PTR, 0U, &adcChnConfig);

  gsM1_Drive.f16DcBusCurrentNoFiltered = MLIB_ShLSat_F16(gsM1_Drive.f16DcBusCurrentRaw - gsM1_Drive.f16DcBusCurrentOffset, 1);
  // Filtered DC Bus current value
  gsM1_Drive.f16DcBusCurrent = GDFLIB_FilterMA_F16(gsM1_Drive.f16DcBusCurrentNoFiltered, &gsM1_Drive.trCurrentMaFilter);
  gsM1_Drive.f16PhaseBemf = gsM1_Drive.f16PhaseVoltage - (gsM1_Drive.f16DcbVoltage >> 1);

  FMSTR_Recorder();     // Back-EMF voltage (must be located before FastControlLoop, because it is modified in FastControlLoop)
  
  FastControlLoop(&gsM1_Drive);

  // Configure ADC channel B for next measurement
  // w16CmtSector may be modified in FastControlLoop(...)
  adcChnConfig.chnIdx = (adc16_chn_t)bldcAdcSectorCfg[gsM1_Drive.w16CmtSector];
  adcChnConfig.convCompletedIntEnable = true;
  ADC16_HAL_ConfigChn(ADC0_BASE_PTR, 1U, &adcChnConfig);

  ReadSwFaults(&gsM1_Drive);

  SM_StateMachine(&gsM1_Ctrl);

  UpdateBldcHwModules(&gsM1_Drive);
}

/*
 * @brief   FTM1 interrupt service routine
 * - Forced commutation control
 */
void FTM1_IRQHandler(void)
{
    // Read timer counter and value registers
    gsM1_Drive.uw16TimeCurrent = FTM_HAL_GetCounter(FTM1_BASE_PTR);
    gsM1_Drive.uw16TimeCurrentEvent = FTM_HAL_GetChnCountVal(FTM1_BASE_PTR, 0);

    TimeEvent(&gsM1_Drive);

    SM_StateMachine(&gsM1_Ctrl);

    UpdateBldcHwModules(&gsM1_Drive);

    FTM_HAL_ClearChnEventFlag(FTM1_BASE_PTR, 0);/*Clear flag*/
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
    temp = FTM_HAL_GetChnCountVal(FTM2_BASE_PTR, 0)+SLOW_TIMER_PERIOD;
    FTM_HAL_SetChnCountVal(FTM2_BASE_PTR, 0, temp);
    FTM_HAL_ClearChnEventFlag(FTM2_BASE_PTR, 0);/*Clear flag*/
}

/*
 * @brief   PDB0 interrupt service routine
 * Trigger error clear routine
 */
void PDB0_IRQHandler(void)
{
    if (PDB_HAL_GetAdcPreTriggerSeqErrFlags(PDB0_BASE_PTR, 0, (1U<<0)|(1U<<1) ))
    {
        if (uw16TriggerErrorCnt < 0xffff) uw16TriggerErrorCnt++;
        // Disable PDB
        PDB_HAL_Disable(PDB0_BASE_PTR);
        PDB_HAL_ClearAdcPreTriggerSeqErrFlags(PDB0_BASE_PTR, 0, (1U<<0)|(1U<<1));
        // Enable PDB
        PDB_HAL_Enable(PDB0_BASE_PTR);
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
        GPIO_HAL_ClearPinOutput(GPIOE_BASE_PTR, 24);
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
        GPIO_HAL_SetPinOutput(GPIOE_BASE_PTR, 24);
    }
    else
    {
        GPIO_HAL_ClearPinOutput(GPIOE_BASE_PTR, 24);
    }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

