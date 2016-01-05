/******************************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
* All Rights Reserved
*
*******************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************//*!
*
* @file      M1_StateMachine.c
*
* @author    STSP001
*
* @version   1.0.3.0
*
* @date      Nov-27-2013
*
* @brief     Motor 1 state machine
*
*******************************************************************************
*
* Motor 1 state machine.
*
******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "BLDC_appconfig.h"
#include "peripherals_init.h"
#include "M1_StateMachine.h"
#include "motor_structure.h"
#include "arm_cm0.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

#define MAX_CMT_ERRORS                          3
//! @brief period between measurements (multiples of 1 / PWM modulo frequency)
#define CALIB_ONE_MEASURE_PERIOD_US             200
#define CALIB_MEASURES_CNT                      200     //!< 40 ms
#define CALIB_ONE_MEASURE_PERIOD_TICKS          ((unsigned short) ((float) TIMER_FREQUENCY / 1000000 * CALIB_ONE_MEASURE_PERIOD_US))
#define CURRENT_CONTROLLER_ALIGN_LIM_LOW        FRAC16(0.02)
#define CURRENT_CONTROLLER_ALIGN_LIM_HIGH       FRAC16(0.9)

/*!
 * @brief Run sub-states
 */
typedef enum {
    CALIB         = 0,
    READY         = 1,
    ALIGN         = 2,
    STARTUP       = 3,
    SPIN          = 4,
    FREEWHEEL     = 5,
} SM_RUN_SUBSTATE_T;

typedef void (*PFCN_VOID_MCSTRUCT) (MCSTRUC_BLDC_SNLS_INT_T *);

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

//! @brief User state machine functions
static void M1_StateFault(void);
static void M1_StateInit(void);
static void M1_StateStop(void);
static void M1_StateRun(void);

//! @brief User state-transition functions
static void M1_TransFaultInit(void);
static void M1_TransInitFault(void);
static void M1_TransInitStop(void);
static void M1_TransStopFault(void);
static void M1_TransStopInit(void);
static void M1_TransStopRun(void);
static void M1_TransRunFault(void);
static void M1_TransRunStop(void);

//! @brief User sub-state machine functions
static void M1_StateRunCalib(MCSTRUC_BLDC_SNLS_INT_T *);
static void M1_StateRunReady(MCSTRUC_BLDC_SNLS_INT_T *);
static void M1_StateRunAlign(MCSTRUC_BLDC_SNLS_INT_T *);
static void M1_StateRunStartup(MCSTRUC_BLDC_SNLS_INT_T *);
static void M1_StateRunSpin(MCSTRUC_BLDC_SNLS_INT_T *);
static void M1_StateRunFreewheel(MCSTRUC_BLDC_SNLS_INT_T *);

void TimeEvent(MCSTRUC_BLDC_SNLS_INT_T *);
void FastControlLoop(MCSTRUC_BLDC_SNLS_INT_T *);
void SlowControlLoop(MCSTRUC_BLDC_SNLS_INT_T *);
void BldcCommutation(MCSTRUC_BLDC_SNLS_INT_T *);
void ReadSwFaults(MCSTRUC_BLDC_SNLS_INT_T *);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Global variables
////////////////////////////////////////////////////////////////////////////////
extern volatile UInt16 uw16TriggerErrorCnt;

////////////////////////////////////////////////////////////////////////////////
// Local variables
////////////////////////////////////////////////////////////////////////////////
static int      mintM1_SwitchAppOnOff;
static int      mintM1_Fault;
//! @brief Run sub-state identifier
static SM_RUN_SUBSTATE_T    meM1_StateRun;
MCSTRUC_BLDC_SNLS_INT_T     gsM1_Drive;
//! @brief State machine functions field (in pmem)
static const SM_APP_STATE_FCN_T msSTATE = {M1_StateFault, M1_StateInit, M1_StateStop, M1_StateRun};
//! @brief State-transition functions field (in pmem)
static const SM_APP_TRANS_FCN_T msTRANS = {M1_TransFaultInit, M1_TransInitFault, M1_TransInitStop, M1_TransStopFault, M1_TransStopInit, M1_TransStopRun, M1_TransRunFault, M1_TransRunStop};

//! @brief Sub-state machine functions field (in pmem)
static const PFCN_VOID_MCSTRUCT mM1_STATE_RUN_TABLE[6] = {M1_StateRunCalib, M1_StateRunReady, M1_StateRunAlign, M1_StateRunStartup, M1_StateRunSpin, M1_StateRunFreewheel};

//! @brief State machine structure declaration and initialization */
SM_APP_CTRL_T gsM1_Ctrl =
{
    &msSTATE,       //!< gsM1_Ctrl.psState, User state functions
    &msTRANS,       //!< gsM1_Ctrl.psTrans, User state-transition functions
    SM_CTRL_NONE,   //!< gsM1_Ctrl.uiCtrl, Default no control command
    INIT            //!< gsM1_Ctrl.eState, Default state after reset
};

/*!
 * @brief FAULT state
 */
static void M1_StateFault(void)
{
    // Type the code to do when in the FAULT state
    if (gsM1_Drive.uw16FreewheelPeriodCurrentMs > 0)
        // Wait for free-wheel period
        return;

    // If no fault
    if (!mintM1_Fault)
    {
        // Clear fault command
        gsM1_Ctrl.uiCtrl |= SM_CTRL_FAULT_CLEAR;
    }
}

/*!
 * @brief INIT state
 */
static void M1_StateInit(void)
{
    // Type the code to do when in the INIT state
    meM1_StateRun = READY;
    uw16TriggerErrorCnt = 0;

    gsM1_Drive.uw32McatCurrentScale = FM_I_SCALE;
    gsM1_Drive.uw32McatVoltageScale = FM_U_DCB_SCALE;
    gsM1_Drive.uw32McatSpeedScale   = FM_N_SCALE;

    gsM1_Drive.f16DcBusCurrentOffset = 0;
    gsM1_Drive.f16DcBusCurrentAlign = ALIGN_CURRENT;
    gsM1_Drive.f16DcBusCurrentLimit = I_DCB_LIMIT;
    gsM1_Drive.uw16FreewheelPeriodLongMs  = FREEWHEEL_T_LONG;
    gsM1_Drive.uw16FreewheelPeriodShortMs = FREEWHEEL_T_SHORT;
    gsM1_Drive.uw16HwUpdateRequests = 0;
    gsM1_Drive.uw16FaultStatus = 0;
    gsM1_Drive.f16SpeedRequired = 0;
    gsM1_Drive.f16SpeedMinimal = N_MIN;
    gsM1_Drive.w16CmtSector = 0;

    gsM1_Drive.trCurrentMaFilter.u16NSamples = TORQUE_LOOP_MAF;
    GDFLIB_FilterMAInit_F16(&gsM1_Drive.trCurrentMaFilter);

    // Default ramp setting (overwritten in Demonstration() function)
    gsM1_Drive.f32trSpeedRamp.f32RampDown = SPEED_LOOP_RAMP_DOWN;
    gsM1_Drive.f32trSpeedRamp.f32RampUp   = SPEED_LOOP_RAMP_UP;

    gsM1_Drive.uw16McatAlignmentPeriodMs = ALIGN_DURATION;
    // Current controller parameters
    gsM1_Drive.trCurrentPI.f16PropGain        = TORQUE_LOOP_KP_GAIN;
    gsM1_Drive.trCurrentPI.f16IntegGain       = TORQUE_LOOP_KI_GAIN;
    gsM1_Drive.trCurrentPI.w16PropGainShift   = TORQUE_LOOP_KP_SHIFT;
    gsM1_Drive.trCurrentPI.w16IntegGainShift  = TORQUE_LOOP_KI_SHIFT;
    gsM1_Drive.trCurrentPI.f16UpperLimit      = CTRL_LOOP_LIM_HIGH;
    gsM1_Drive.trCurrentPI.f16LowerLimit      = CTRL_LOOP_LIM_LOW;

    // Speed controller parameters
    gsM1_Drive.trSpeedPI.f16PropGain        = SPEED_LOOP_KP_GAIN;
    gsM1_Drive.trSpeedPI.f16IntegGain       = SPEED_LOOP_KI_GAIN;
    gsM1_Drive.trSpeedPI.w16PropGainShift   = SPEED_LOOP_KP_SHIFT;
    gsM1_Drive.trSpeedPI.w16IntegGainShift  = SPEED_LOOP_KI_SHIFT;
    gsM1_Drive.trSpeedPI.f16UpperLimit      = CTRL_LOOP_LIM_HIGH;
    gsM1_Drive.trSpeedPI.f16LowerLimit      = CTRL_LOOP_LIM_LOW;

    gsM1_Drive.f16McatStartCmtAcceleration  = START_CMT_ACCELER;
    gsM1_Drive.uw16McatStartCmtCounter      = STARTUP_CMT_CNT;
    gsM1_Drive.uw16McatPeriodCmtNext        = STARTUP_CMT_PER;
    gsM1_Drive.uw16McatPeriodToff           = CMT_T_OFF;
    gsM1_Drive.f32IntegralBemfThreshold     = INTEG_TRH;

    gsM1_Drive.uw16CurrentLimiting = 0;

    // INIT_DONE command
    gsM1_Ctrl.uiCtrl |= SM_CTRL_INIT_DONE;
}

/*!
 * @brief STOP state
 */
static void M1_StateStop(void)
{
    // Type the code to do when in the STOP state
    // If the user switches on
    if (mintM1_SwitchAppOnOff)
    {
        // Start command
        gsM1_Ctrl.uiCtrl |= SM_CTRL_START;
    }

    // If a fault occurred
    if (mintM1_Fault)
    {
        // Switches to the FAULT state
        gsM1_Ctrl.uiCtrl |= SM_CTRL_FAULT;
    }
}

/*!
 * @brief RUN state
 */
static void M1_StateRun(void)
{
    // Type the code to do when in the RUN state
    // Run sub-state function
    mM1_STATE_RUN_TABLE[meM1_StateRun](&gsM1_Drive);

    // If the user switches off
    if (!mintM1_SwitchAppOnOff)
    {
        // Stop command
        gsM1_Ctrl.uiCtrl |= SM_CTRL_STOP;
    }

    // If a fault occurred
    if (mintM1_Fault)
    {
        // Switches to the FAULT state
        gsM1_Ctrl.uiCtrl |= SM_CTRL_FAULT;
    }
}

/*!
 * @brief FAULT to INIT transition
 */
static void M1_TransFaultInit(void)
{
    // Type the code to do when going from the FAULT to the INIT state
    MC33927_config();   // reconfigure driver - clear all driver fault flags
}

/*!
 * @brief INIT to FAULT transition
 */
static void M1_TransInitFault(void)
{
    // Type the code to do when going from the INIT to the FAULT state
    mintM1_SwitchAppOnOff = 0;
}

/*!
 * @brief INIT to STOP transition
 */
static void M1_TransInitStop(void)
{
    // Type the code to do when going from the INIT to the FAULT state
}

/*!
 * @brief STOP to FAULT transition
 */
static void M1_TransStopFault(void)
{
    // Type the code to do when going from the STOP to the FAULT state
    mintM1_SwitchAppOnOff = 0;
}

/*!
 * @brief STOP to INIT transition
 */
static void M1_TransStopInit(void)
{
    // Type the code to do when going from the STOP to the INIT state
}

/*!
 * @brief STOP to RUN transition
 */
static void M1_TransStopRun(void)
{
    // Type the code to do when going from the STOP to the RUN state

    // Init sub-state when transition to RUN
    meM1_StateRun = CALIB;

    // Skip if calibration will be not executed
    gsM1_Drive.uw16CalibCounter = CALIB_MEASURES_CNT;
    // Period to next ISR
    gsM1_Drive.uw16TimeNextEvent = gsM1_Drive.uw16TimeCurrent + CALIB_ONE_MEASURE_PERIOD_TICKS;
    gsM1_Drive.uw16HwUpdateRequests |= UPDATE_TIME_EVENT_REQ;
    gsM1_Drive.f16DcBusCurrentOffset = 0;
    gsM1_Drive.f32DcBusCurrentAcc = 0;

    // Acknowledge that the system can proceed into the RUN state
    gsM1_Ctrl.uiCtrl |= SM_CTRL_RUN_ACK;
}

/*!
 * @brief RUN to FAULT transition
 */
static void M1_TransRunFault(void)
{
    // Type the code to do when going from the RUN to the FAULT state
    gsM1_Drive.uw16FreewheelPeriodCurrentMs = gsM1_Drive.uw16FreewheelPeriodLongMs;
    mintM1_SwitchAppOnOff = 0;
    // Disable PWM output
    gsM1_Drive.uw16HwUpdateRequests |= UPDATE_PWM_DISABLE_REQ;
    // Disable brake resistor if enabled
    gsM1_Drive.uw16HwUpdateRequests &= ~UPDATE_BRAKE_RESISTOR_ON_REQ;
    gsM1_Drive.uw16CurrentLimiting = 0;
    // Enter Free-wheel state
    meM1_StateRun = FREEWHEEL;
}

/*!
 * @brief RUN to STOP transition
 */
static void M1_TransRunStop(void)
{
    // Type the code to do when going from the RUN to the STOP state
    gsM1_Drive.uw16CurrentLimiting = 0;

    switch (meM1_StateRun)
    {
        case SPIN:
            gsM1_Drive.uw16FreewheelPeriodCurrentMs = gsM1_Drive.uw16FreewheelPeriodLongMs;
            // Disable PWM output
            gsM1_Drive.uw16HwUpdateRequests |= UPDATE_PWM_DISABLE_REQ;
            // Disable brake resistor if enabled
            gsM1_Drive.uw16HwUpdateRequests &= ~UPDATE_BRAKE_RESISTOR_ON_REQ;
            // Enter Free-wheel state
            meM1_StateRun = FREEWHEEL;
            break;

        case STARTUP:
            gsM1_Drive.uw16FreewheelPeriodCurrentMs = gsM1_Drive.uw16FreewheelPeriodShortMs;
            // Disable PWM output
            gsM1_Drive.uw16HwUpdateRequests |= UPDATE_PWM_DISABLE_REQ;
            // Enter Free-wheel state
            meM1_StateRun = FREEWHEEL;
            break;

        case FREEWHEEL:
            // Wait for freewheel period
            if (gsM1_Drive.uw16FreewheelPeriodCurrentMs > 0) break;
            meM1_StateRun = READY;
            // Acknowledge that the system can proceed into the STOP state
            // Enter to Stop state
            gsM1_Ctrl.uiCtrl |= SM_CTRL_STOP_ACK;
            break;

        // any other states (motor is not spinning)
        default:
            gsM1_Drive.uw16HwUpdateRequests |= UPDATE_PWM_DISABLE_REQ;
            meM1_StateRun = READY;
            // Acknowledge that the system can proceed into the STOP state
            // Enter to Stop state
            gsM1_Ctrl.uiCtrl |= SM_CTRL_STOP_ACK;
            break;
    }
}

/*!
 * @brief Fault function
 */
void M1_Fault(int intValue)
{
    mintM1_Fault = intValue;
}

/*!
 * @brief Set the app switch function
 */
void M1_SetAppSwitch(int intValue)
{
    mintM1_SwitchAppOnOff = intValue;
}

/*!
 * @brief Read the app switch function
 */
int M1_GetAppSwitch(void)
{
    return (mintM1_SwitchAppOnOff);
}

/*!
 * @brief RUN CALIB sub-state
 * @param sM_Drive BLDC motor variables structure pointer
 */
static void M1_StateRunCalib(MCSTRUC_BLDC_SNLS_INT_T * sM_Drive)
{
    // Type the code to do when in the RUN CALIB sub-state
    if (sM_Drive->uw16CalibCounter > 0) return;

    // To switch to the RUN READY sub-state
    sM_Drive->f16DcBusCurrentOffset = (Frac16) (sM_Drive->f32DcBusCurrentAcc / CALIB_MEASURES_CNT);
    meM1_StateRun = READY;
}

/*!
 * @brief RUN READY sub-state
 * @param sM_Drive BLDC motor variables structure pointer
 */
static void M1_StateRunReady(MCSTRUC_BLDC_SNLS_INT_T * sM_Drive)
{
    // Type the code to do when in the RUN READY sub-state
    if ((sM_Drive->f16SpeedRequired < sM_Drive->f16SpeedMinimal) && (sM_Drive->f16SpeedRequired > (- sM_Drive->f16SpeedMinimal))) return;

    // To switch to the RUN ALIGN sub-state
    if (sM_Drive->f16SpeedRequired > 0) sM_Drive->uw16Direction = 0;
    else sM_Drive->uw16Direction = 1;
    // Should be (deadtime / Modulo) to start generate PWM immediately
    sM_Drive->f16DutyCycle = 0;
    sM_Drive->uw16HwUpdateRequests |= UPDATE_DUTYCYCLE_REQ;
    // Apply alignment vector
    sM_Drive->uw16HwUpdateRequests |= UPDATE_PWM_ALIGNMENT_REQ;
    sM_Drive->uw16TimeNextEvent = sM_Drive->uw16TimeCurrent + TIMER_1MS_CONST;
    sM_Drive->uw16HwUpdateRequests |= UPDATE_TIME_EVENT_REQ;

    sM_Drive->uw16AlignmentPeriodMs = sM_Drive->uw16McatAlignmentPeriodMs;

    sM_Drive->trCurrentPI.f16InK_1           = (Frac16) 0;
    // Calculate internal accumulator to be aligned with actual dutycycle (deadtime)
    sM_Drive->trCurrentPI.f32IntegPartK_1    = MLIB_ConvertPU_F32F16(sM_Drive->f16DutyCycle);
    sM_Drive->trCurrentPI.f16UpperLimit      = CURRENT_CONTROLLER_ALIGN_LIM_HIGH;
    sM_Drive->trCurrentPI.f16LowerLimit      = CURRENT_CONTROLLER_ALIGN_LIM_LOW;
    meM1_StateRun = ALIGN;
}

/*!
 * @brief RUN ALIGN sub-state
 * @param sM_Drive BLDC motor variables structure pointer
 */
static void M1_StateRunAlign(MCSTRUC_BLDC_SNLS_INT_T * sM_Drive)
{
    // Type the code to do when in the RUN ALIGN sub-state

    // To switch to the RUN READY sub-state
    if ((sM_Drive->f16SpeedRequired < sM_Drive->f16SpeedMinimal) && (sM_Drive->f16SpeedRequired > (-sM_Drive->f16SpeedMinimal)))
    {
        // Disable PWM output
        sM_Drive->uw16HwUpdateRequests |= UPDATE_PWM_DISABLE_REQ;
        meM1_StateRun = READY;
        return;
    }

    if (sM_Drive->uw16AlignmentPeriodMs > 0) return;

    // Type the code to do when going from the RUN ALIGN to the RUN STARTUP sub-state
    sM_Drive->uw16PeriodCmtNext = sM_Drive->uw16McatPeriodCmtNext;
    sM_Drive->uw16StartCmtCounter = sM_Drive->uw16McatStartCmtCounter;
    sM_Drive->uw16TimeNextEvent = sM_Drive->uw16TimeCurrent + sM_Drive->uw16PeriodCmtNext;
    sM_Drive->uw16HwUpdateRequests |= UPDATE_TIME_EVENT_REQ;
    if (sM_Drive->uw16Direction == 0) sM_Drive->w16CmtSector = 2;
    else sM_Drive->w16CmtSector = 5;
    // Apply first vector after alignment
    sM_Drive->uw16HwUpdateRequests |= UPDATE_PWM_CONFIG_REQ;
    meM1_StateRun = STARTUP;
}

/*!
 * @brief RUN STARTUP sub-state
 * @param sM_Drive BLDC motor variables structure pointer
 */
static void M1_StateRunStartup(MCSTRUC_BLDC_SNLS_INT_T * sM_Drive)
{
    // Type the code to do when in the RUN STARTUP sub-state

    // To switch to RUN FREEWHEEL (on startup fail or required speed changed to 0)
    if ((sM_Drive->f16SpeedRequired < sM_Drive->f16SpeedMinimal) && (sM_Drive->f16SpeedRequired > (-sM_Drive->f16SpeedMinimal)))
    {
        sM_Drive->uw16FreewheelPeriodCurrentMs = sM_Drive->uw16FreewheelPeriodShortMs;
        sM_Drive->uw16HwUpdateRequests |= UPDATE_PWM_DISABLE_REQ;
        meM1_StateRun = FREEWHEEL;
        return;
    }

    if (sM_Drive->uw16StartCmtCounter > 0) return;

    // Type the code to do when going from the RUN STARTUP to the RUN SPIN sub-state
    sM_Drive->uw16PeriodToff = MLIB_Mul_F16(sM_Drive->uw16PeriodCmtNext, sM_Drive->uw16McatPeriodToff);

    sM_Drive->uw16PeriodCmt[0] = sM_Drive->uw16PeriodCmtNext;
    sM_Drive->uw16PeriodCmt[1] = sM_Drive->uw16PeriodCmtNext;
    sM_Drive->uw16PeriodCmt[2] = sM_Drive->uw16PeriodCmtNext;
    sM_Drive->uw16PeriodCmt[3] = sM_Drive->uw16PeriodCmtNext;
    sM_Drive->uw16PeriodCmt[4] = sM_Drive->uw16PeriodCmtNext;
    sM_Drive->uw16PeriodCmt[5] = sM_Drive->uw16PeriodCmtNext;
    sM_Drive->f32IntegralBemfSum = 0;

    // Current controller parameters - Spin (close-loop)
    sM_Drive->trCurrentPI.f16UpperLimit      = sM_Drive->trSpeedPI.f16UpperLimit;
    sM_Drive->trCurrentPI.f16LowerLimit      = sM_Drive->trSpeedPI.f16LowerLimit;
    sM_Drive->trCurrentPI.f16InK_1           = (Frac16) 0;
    // Calculate internal accumulator to be aligned with actual dutycycle (already corresponding to actual duty-cycle)
    // sM_Drive->trCurrentPI.f32IntegPartK_1 = MLIB_ConvertPU_F32F16(sM_Drive->f16DutyCycle);

    sM_Drive->trSpeedPI.f16InK_1           = (Frac16) 0;
    // Calculate internal accumulator to be aligned with actual dutycycle
    sM_Drive->trSpeedPI.f32IntegPartK_1    = MLIB_ConvertPU_F32F16(sM_Drive->f16DutyCycle);

    sM_Drive->uw16CmtErrorCnt = 0;

    if (sM_Drive->uw16Direction == 0) sM_Drive->f32trSpeedRamp.f32State = MLIB_ConvertPU_F32F16(N_START_TRH);
    else                              sM_Drive->f32trSpeedRamp.f32State = MLIB_ConvertPU_F32F16(-N_START_TRH);

    meM1_StateRun = SPIN;
}

/*!
 * @brief RUN SPIN sub-state
 * @param sM_Drive BLDC motor variables structure pointer
 */
static void M1_StateRunSpin(MCSTRUC_BLDC_SNLS_INT_T * sM_Drive)
{
    // Type the code to do when in the RUN SPIN sub-state

    // To switch to the RUN FREEWHEEL sub-state

}

/*!
 * @brief RUN FREEWHEEL sub-state
 * @param sM_Drive BLDC motor variables structure pointer
 */
static void M1_StateRunFreewheel(MCSTRUC_BLDC_SNLS_INT_T * sM_Drive)
{
    // Type the code to do when in the RUN FREEWHEEL sub-state
    if (sM_Drive->uw16FreewheelPeriodCurrentMs > 0) return;

    // To switch to the RUN READY sub-state
    meM1_StateRun = READY;
}

/*!
 * @brief Time event function
 * @param sM_Drive BLDC motor variables structure pointer
 */
void TimeEvent(MCSTRUC_BLDC_SNLS_INT_T * sM_Drive)
{
    // Close-loop control
    if (meM1_StateRun == SPIN)
    {
        // Commutated in ADC ISR just before this ISR
        if (sM_Drive->uw16CommutatedSensorless == 1) return;

        // Check of commutation stability
        if (sM_Drive->uw16CmtErrorCnt > (MAX_CMT_ERRORS * 3))
        {
            // Unstable or very low speed
            meM1_StateRun = FREEWHEEL;
            // Disable PWM output
            sM_Drive->uw16HwUpdateRequests |= UPDATE_PWM_DISABLE_REQ;
            sM_Drive->uw16FreewheelPeriodCurrentMs = sM_Drive->uw16FreewheelPeriodLongMs;
            sM_Drive->f16SpeedMeasured = 0;
            return;
        }
        else
        {
            sM_Drive->uw16CmtErrorCnt += 3;
        }

        sM_Drive->uw16TimeOfCommutationOld = sM_Drive->uw16TimeOfCommutation;
        sM_Drive->uw16TimeOfCommutation = sM_Drive->uw16TimeCurrent;
        sM_Drive->uw16PeriodCmtNext = sM_Drive->uw16TimeOfCommutation - sM_Drive->uw16TimeOfCommutationOld;
        // Save actual commutation period
        sM_Drive->uw16PeriodCmt[sM_Drive->w16CmtSector] = sM_Drive->uw16PeriodCmtNext;

        BldcCommutation(sM_Drive);

        sM_Drive->f32IntegralBemfSum = 0;

        sM_Drive->uw16PeriodToff = MLIB_Mul_F16(sM_Drive->uw16PeriodCmtNext, sM_Drive->uw16McatPeriodToff);
        sM_Drive->uw16PeriodCmtNext = sM_Drive->uw16PeriodCmtNext << 1;
        if (sM_Drive->uw16PeriodCmtNext > 32767) sM_Drive->uw16PeriodCmtNext = 32767;
        // Period to the next commutation ISR
        sM_Drive->uw16TimeNextEvent = sM_Drive->uw16TimeCurrent + sM_Drive->uw16PeriodCmtNext;
        sM_Drive->uw16HwUpdateRequests |= UPDATE_TIME_EVENT_REQ;
        return;
    }

    // Open-loop startup
    if (meM1_StateRun == STARTUP)
    {

        BldcCommutation(sM_Drive);

        sM_Drive->uw16StartCmtCounter--;
        if (sM_Drive->uw16StartCmtCounter > 0)
        {

            sM_Drive->uw16PeriodCmtNext = MLIB_Mul_F16(sM_Drive->uw16PeriodCmtNext, sM_Drive->f16McatStartCmtAcceleration);
            // Period to the next commutation ISR, still open-loop
            sM_Drive->uw16TimeNextEvent = sM_Drive->uw16TimeCurrentEvent + sM_Drive->uw16PeriodCmtNext;
        }
        else
        {
            // Period to the next commutation ISR, safety commutation in close-loop (if not commutated in sensor-less mode)
            sM_Drive->uw16TimeNextEvent = sM_Drive->uw16TimeCurrentEvent + (sM_Drive->uw16PeriodCmtNext << 1);
            sM_Drive->uw16TimeOfCommutation = sM_Drive->uw16TimeCurrent;
            sM_Drive->f32IntegralBemfSum = 0;
        }
        sM_Drive->uw16HwUpdateRequests |= UPDATE_TIME_EVENT_REQ;
        return;
    }

    // Alignment
    if (meM1_StateRun == ALIGN)
    {
        if (sM_Drive->uw16AlignmentPeriodMs > 0)
        {
            sM_Drive->uw16AlignmentPeriodMs--;
            // Period to next ISR (1 ms)
            sM_Drive->uw16TimeNextEvent = sM_Drive->uw16TimeCurrentEvent + TIMER_1MS_CONST;
            sM_Drive->uw16HwUpdateRequests |= UPDATE_TIME_EVENT_REQ;
        }
        return;
    }

    // Calibration
    if (meM1_StateRun == CALIB)
    {
        if (sM_Drive->uw16CalibCounter > 0)
        {
            sM_Drive->uw16CalibCounter--;
            // Period to next ISR
            sM_Drive->uw16TimeNextEvent = sM_Drive->uw16TimeCurrentEvent + CALIB_ONE_MEASURE_PERIOD_TICKS;
            sM_Drive->uw16HwUpdateRequests |= UPDATE_TIME_EVENT_REQ;
            sM_Drive->f32DcBusCurrentAcc += (Frac32) sM_Drive->f16DcBusCurrentRaw;
        }
        return;
    }

    // Free-wheel
    if (meM1_StateRun == FREEWHEEL)
    {
        if (sM_Drive->uw16FreewheelPeriodCurrentMs > 0)
        {
            sM_Drive->uw16FreewheelPeriodCurrentMs--;
            // Period to next ISR (1 ms)
            sM_Drive->uw16TimeNextEvent = sM_Drive->uw16TimeCurrentEvent + TIMER_1MS_CONST; 
            sM_Drive->uw16HwUpdateRequests |= UPDATE_TIME_EVENT_REQ;
        }
    }
}

/*!
 * @brief Fast control loop function
 * @param sM_Drive BLDC motor variables structure pointer
 */
void FastControlLoop(MCSTRUC_BLDC_SNLS_INT_T * sM_Drive)
{
    // Process Back-EMF voltage
    if (meM1_StateRun != SPIN) return;

    if (sM_Drive->f16DcbVoltage > U_DCB_TRIP)
        // Enable brake resistor
        sM_Drive->uw16HwUpdateRequests |= UPDATE_BRAKE_RESISTOR_ON_REQ;
    else
        // Disable brake resistor
        sM_Drive->uw16HwUpdateRequests &= ~UPDATE_BRAKE_RESISTOR_ON_REQ;

    sM_Drive->uw16CommutatedSensorless = 0;

    // Check Toff period after commutation event
    if ((unsigned short int) (sM_Drive->uw16TimeCurrent - sM_Drive->uw16TimeOfCommutation) < sM_Drive->uw16PeriodToff) return;

    // Mirror Back-EMF voltage according to the sector (change falling Back-EMF to rising Back-EMF)
    if (!((sM_Drive->w16CmtSector ^ sM_Drive->uw16Direction) & 1))
    {
        sM_Drive->f16PhaseBemf = - sM_Drive->f16PhaseBemf;
    }

    // Integrate if positive bemf voltage
    if (sM_Drive->f16PhaseBemf > 0) {
        if (sM_Drive->f32IntegralBemfSum == 0) sM_Drive->f16DcBusCurrentZC = sM_Drive->f16DcBusCurrent;
        sM_Drive->f32IntegralBemfSum += (Frac32) sM_Drive->f16PhaseBemf;
    }

    // Check integral of Back-EMF voltage
    if (sM_Drive->f32IntegralBemfSum >= sM_Drive->f32IntegralBemfThreshold)
    {
        if ((unsigned short int) (sM_Drive->uw16TimeCurrent - sM_Drive->uw16TimeOfCommutation) < (CMT_PER_MIN >> 1))
        {
            meM1_StateRun = FREEWHEEL;
            sM_Drive->uw16FreewheelPeriodCurrentMs = sM_Drive->uw16FreewheelPeriodLongMs;
            // Disable PWM output
            sM_Drive->uw16HwUpdateRequests |= UPDATE_PWM_DISABLE_REQ;
            return;
        }

        sM_Drive->uw16TimeOfCommutationOld = sM_Drive->uw16TimeOfCommutation;
        sM_Drive->uw16TimeOfCommutation = sM_Drive->uw16TimeCurrent;
        sM_Drive->uw16PeriodCmtNext = sM_Drive->uw16TimeOfCommutation - sM_Drive->uw16TimeOfCommutationOld;
        // Save actual commutation period
        sM_Drive->uw16PeriodCmt[sM_Drive->w16CmtSector] = sM_Drive->uw16PeriodCmtNext;

        BldcCommutation(sM_Drive);

        sM_Drive->uw16PeriodToff = MLIB_Mul_F16(sM_Drive->uw16PeriodCmtNext, sM_Drive->uw16McatPeriodToff);
        // Double of actual commutation period
        sM_Drive->uw16PeriodCmtNext = sM_Drive->uw16PeriodCmtNext << 1;
        if (sM_Drive->uw16PeriodCmtNext > 32767) sM_Drive->uw16PeriodCmtNext = 32767;
        // Period to the next commutation ISR
        sM_Drive->uw16TimeNextEvent = sM_Drive->uw16TimeCurrent + sM_Drive->uw16PeriodCmtNext;
        sM_Drive->uw16HwUpdateRequests |= UPDATE_TIME_EVENT_REQ;

        sM_Drive->f32IntegralBemfSum = 0;

        if (sM_Drive->uw16CmtErrorCnt > 0) sM_Drive->uw16CmtErrorCnt--;

        sM_Drive->uw16CommutatedSensorless = 1;

    }
}

/*!
 * @brief Slow control loop function
 * @param sM_Drive BLDC motor variables structure pointer
 */

void SlowControlLoop(MCSTRUC_BLDC_SNLS_INT_T * sM_Drive)
{
    Frac16 f16SpeedMeasuredU;

    if (meM1_StateRun == SPIN)
    {
        // Calculate average speed
        sM_Drive->uw32Period6CmtSum =  ((uint32) sM_Drive->uw16PeriodCmt[0]) + ((uint32) sM_Drive->uw16PeriodCmt[1]);
        sM_Drive->uw32Period6CmtSum += ((uint32) sM_Drive->uw16PeriodCmt[2]) + ((uint32) sM_Drive->uw16PeriodCmt[3]);
        sM_Drive->uw32Period6CmtSum += ((uint32) sM_Drive->uw16PeriodCmt[4]) + ((uint32) sM_Drive->uw16PeriodCmt[5]);
        f16SpeedMeasuredU = (MMDVSQ_HAL_DivUQ(MMDVSQ_BASE_PTR, SPEED_SCALE_CONST*32768, sM_Drive->uw32Period6CmtSum));
        sM_Drive->f16SpeedRampOutput = MLIB_ConvertPU_F16F32(GFLIB_Ramp_F32(MLIB_ConvertPU_F32F16(sM_Drive->f16SpeedRequired), &sM_Drive->f32trSpeedRamp));
        // Process direction of rotation
        if (sM_Drive->uw16Direction == 0)
        {
            sM_Drive->f16SpeedMeasured = f16SpeedMeasuredU;
            sM_Drive->f16SpeedPiErr = MLIB_SubSat_F16(sM_Drive->f16SpeedRampOutput, sM_Drive->f16SpeedMeasured);
        }
        else
        {
            sM_Drive->f16SpeedMeasured = - f16SpeedMeasuredU;
            sM_Drive->f16SpeedPiErr = MLIB_SubSat_F16(sM_Drive->f16SpeedMeasured, sM_Drive->f16SpeedRampOutput);
        }

        // Check minimal speed boundary
        if ((f16SpeedMeasuredU < sM_Drive->f16SpeedMinimal) |
                ((sM_Drive->f16SpeedRampOutput < sM_Drive->f16SpeedMinimal) && (sM_Drive->f16SpeedRampOutput > (- sM_Drive->f16SpeedMinimal)))
                )
        {
            sM_Drive->uw16FreewheelPeriodCurrentMs = sM_Drive->uw16FreewheelPeriodLongMs;
            meM1_StateRun = FREEWHEEL;
            // Disable interrupts, because this ISR has lower priority than other ISRs that can access uw16HwUpdateRequests
            DisableInterrupts;
            sM_Drive->uw16HwUpdateRequests |= UPDATE_PWM_DISABLE_REQ;
            EnableInterrupts;
            sM_Drive->f16SpeedMeasured = 0;
            return;
        }

        // Calculate Speed PI controller
        sM_Drive->f16SpeedPiOutput = GFLIB_ControllerPIpAW_F16(sM_Drive->f16SpeedPiErr, &sM_Drive->trSpeedPI);

        // Calculate Current/Torque PI controller
        sM_Drive->f16CurrentPiErr = MLIB_SubSat_F16(sM_Drive->f16DcBusCurrentLimit, sM_Drive->f16DcBusCurrent);
        sM_Drive->f16CurrentPiOutput = GFLIB_ControllerPIpAW_F16(sM_Drive->f16CurrentPiErr, &sM_Drive->trCurrentPI);

        if (sM_Drive->f16CurrentPiOutput >= sM_Drive->f16SpeedPiOutput)
        {
            sM_Drive->trCurrentPI.f32IntegPartK_1 = MLIB_ConvertPU_F32F16(sM_Drive->f16DutyCycle);
            sM_Drive->trCurrentPI.f16InK_1 = 0;
            sM_Drive->f16DutyCycle = sM_Drive->f16SpeedPiOutput;
            sM_Drive->uw16CurrentLimiting = 0;
        }
        else
        {    // Current limiting
            sM_Drive->trSpeedPI.f32IntegPartK_1 = MLIB_ConvertPU_F32F16(sM_Drive->f16DutyCycle);
            sM_Drive->trSpeedPI.f16InK_1 = 0;
            sM_Drive->f16DutyCycle = sM_Drive->f16CurrentPiOutput;
            sM_Drive->uw16CurrentLimiting = 1;
        }
    }
    else if (meM1_StateRun == ALIGN)    // Alignment only
    {
        // DC Bus current not Filtered
        sM_Drive->f16CurrentPiErr = MLIB_SubSat_F16(sM_Drive->f16DcBusCurrentAlign, sM_Drive->f16DcBusCurrentNoFiltered);
        // DC Bus current Filtered
        // sM_Drive->f16CurrentPiErr = MLIB_SubSat_F16(sM_Drive->f16DcBusCurrentAlign, sM_Drive->f16DcBusCurrent);
        sM_Drive->f16CurrentPiOutput = GFLIB_ControllerPIpAW_F16(sM_Drive->f16CurrentPiErr, &sM_Drive->trCurrentPI);
        sM_Drive->f16DutyCycle = sM_Drive->f16CurrentPiOutput;
    }

    // Disable interrupts, because this ISR has lower priority than other ISRs that can access uw16HwUpdateRequests
    DisableInterrupts;
    sM_Drive->uw16HwUpdateRequests |= UPDATE_DUTYCYCLE_REQ;
    EnableInterrupts;
}

/*!
 * @brief BLDC commutation function
 * @param sM_Drive BLDC motor variables structure pointer
 */
void BldcCommutation(MCSTRUC_BLDC_SNLS_INT_T * sM_Drive)
{

    if (sM_Drive->uw16Direction == 0)
    {
        // Clockwise
        sM_Drive->w16CmtSector++;
        if (sM_Drive->w16CmtSector > 5) sM_Drive->w16CmtSector = 0;
    }
    else
    {
        // counter-clockwise
        sM_Drive->w16CmtSector--;
        if (sM_Drive->w16CmtSector < 0) sM_Drive->w16CmtSector = 5;
    }

    sM_Drive->uw16HwUpdateRequests |= UPDATE_PWM_CONFIG_REQ;
}

/*!
 * @brief Read faults by S/W
 * @param sM_Drive BLDC motor variables structure pointer
 */
void ReadSwFaults(MCSTRUC_BLDC_SNLS_INT_T * sM_Drive)
{
    sM_Drive->uw16FaultStatus = 0;

    if (sM_Drive->f16DcbVoltage > U_DCB_OVERVOLTAGE)
    {
      sM_Drive->uw16FaultStatus |= FAULT_OVERVOLTAGE_F;
    }

    if (sM_Drive->f16DcbVoltage < U_DCB_UNDERVOLTAGE)
    {
      sM_Drive->uw16FaultStatus |= FAULT_UNDERVOLTAGE_F;
    }

    if ((meM1_StateRun == ALIGN) || (meM1_StateRun == STARTUP) || (meM1_StateRun == SPIN))
    {
        if ((sM_Drive->f16DcBusCurrent > I_DCB_OVERCURRENT) || (sM_Drive->f16DcBusCurrent < (-I_DCB_OVERCURRENT)))
        sM_Drive->uw16FaultStatus |= FAULT_OVERCURRENT_F;
    }
    mintM1_Fault |= sM_Drive->uw16FaultStatus;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
