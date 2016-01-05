/******************************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* @file      motor_structure.h
*
* @author    stsp001
* 
* @version   1.0.3.0
* 
* @date      Nov-27-2013
* 
* @brief     Motor control structure
*
*******************************************************************************
*
* Motor control structure.
*
******************************************************************************/
#ifndef _MCSTRUC_H_
#define _MCSTRUC_H_

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "SWLIBS_Defines.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

//! @brief Request to set new time event (update timer value register)
#define UPDATE_TIME_EVENT_REQ           0x0001
//! @brief Duty-cycle update request
#define UPDATE_DUTYCYCLE_REQ            0x0002
//! @brief Apply commutation vector (alternatively enable PWM outputs)
#define UPDATE_PWM_CONFIG_REQ           0x0004
//! @brief Disable PWM outputs
#define UPDATE_PWM_DISABLE_REQ          0x0008
//! @brief Apply alignment vector
#define UPDATE_PWM_ALIGNMENT_REQ        0x0010
//! @brief Connect or disconnect brake resistor
#define UPDATE_BRAKE_RESISTOR_ON_REQ    0x0020

#define FAULT_UNDERVOLTAGE_F            0x0001
#define FAULT_OVERVOLTAGE_F             0x0002
#define FAULT_OVERCURRENT_F             0x0004

/*!
 * @brief BLDC Sensor-less with BEMF integration method
 */
typedef struct
{
    GFLIB_CONTROLLER_PIAW_P_T_F16       trSpeedPI;
    GFLIB_CONTROLLER_PIAW_P_T_F16       trCurrentPI;
    GDFLIB_FILTER_MA_T_F16              trCurrentMaFilter;
    GFLIB_RAMP_T_F32                    f32trSpeedRamp;
  
    UInt16                              uw16PeriodCmt[6];
    
    Frac32                              f32DcBusCurrentAcc;
    Frac32                              f32IntegralBemfSum;
    Frac32                              f32IntegralBemfThreshold;
    UInt32                              uw32Period6CmtSum;
    
    UInt32                              uw32McatCurrentScale;   //!< Added to be modifiable by MCAT
    UInt32                              uw32McatVoltageScale;   //!< Added to be modifiable by MCAT
    UInt32                              uw32McatSpeedScale;     //!< Added to be modifiable by MCAT
    
    Frac16                              f16SpeedMeasured;
    Frac16                              f16SpeedRequired;
    Frac16                              f16SpeedMinimal;
    Frac16                              f16SpeedRampOutput;
    Frac16                              f16SpeedPiErr;
    Frac16                              f16SpeedPiOutput;
    Frac16                              f16DcBusCurrentNoFiltered;
    Frac16                              f16DcBusCurrent;
    Frac16                              f16DcBusCurrentRaw;
    Frac16                              f16DcBusCurrentOffset;
    Frac16                              f16DcBusCurrentAlign;
    Frac16                              f16DcBusCurrentLimit;
    Frac16                              f16DcBusCurrentZC;
    Frac16                              f16CurrentPiOutput;
    Frac16                              f16CurrentPiErr;
    
    
    Frac16                              f16PhaseVoltage;
    Frac16                              f16PhaseBemf;
    Frac16                              f16DcbVoltage;
    Frac16                              f16DutyCycle;

    
    UInt16                              uw16TimeCurrent;
    UInt16                              uw16TimeCurrentEvent;
    UInt16                              uw16TimeNextEvent;
    UInt16                              uw16TimeOfCommutation;
    UInt16                              uw16TimeOfCommutationOld;
    UInt16                              uw16PeriodCmtNext;
    UInt16                              uw16PeriodToff;
    
    
    UInt16                              uw16FreewheelPeriodCurrentMs;
    UInt16                              uw16FreewheelPeriodLongMs;
    UInt16                              uw16FreewheelPeriodShortMs;
    
    UInt16                              uw16AlignmentPeriodMs;
    
    UInt16                              uw16StartCmtCounter;

    Int16                               w16CmtSector;
    UInt16                              uw16CmtErrorCnt;
    UInt16                              uw16CalibCounter;

    UInt16                              uw16Direction;
    UInt16                              uw16CommutatedSensorless;
    UInt16                              uw16CurrentLimiting;
    
    UInt16                              uw16HwUpdateRequests;    
    
    UInt16                              uw16FaultStatus;

    UInt16                              uw16McatAlignmentPeriodMs;  //!< Added to be modifiable by MCAT
    Frac16                              f16McatStartCmtAcceleration;
    UInt16                              uw16McatStartCmtCounter;
    UInt16                              uw16McatPeriodCmtNext;
    UInt16                              uw16McatPeriodToff;
} MCSTRUC_BLDC_SNLS_INT_T;

#endif /* _MCSTRUC_H_ */

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

