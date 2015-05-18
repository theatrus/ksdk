/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
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
 
/**********************************************************************/
// File Name: {FM_project_loc}/../../src/projects/kv10/BLDC_Sensorless/BLDC_appconfig.h 
//
// Date:  18. February, 2014
//
// Automatically generated file for static configuration of the BLDC application
/**********************************************************************/

#ifndef __BLDC_CONFIG_SETUP_H
#define __BLDC_CONFIG_SETUP_H


//Motor Parameters                      
//----------------------------------------------------------------------
//Pole-pair number                      = 2 [-]
//Back-EMF constant                     = 0.028648 [V.sec/rad]
//Phase current nominal                 FRAC16(0.208750000000)
//Phase voltage nominal                 FRAC16(0.661157024793)
//----------------------------------------------------------------------

//Application scales                    
#define I_MAX                           (8.0)
#define U_DCB_MAX                       (36.3)
#define N_MAX                           (5000.0)
#define I_DCB_OVERCURRENT               FRAC16(0.875000000000)
#define U_DCB_UNDERVOLTAGE              FRAC16(0.220385674931)
#define U_DCB_OVERVOLTAGE               FRAC16(0.826446280992)
#define I_DCB_LIMIT                     FRAC16(0.375000000000)
#define U_DCB_TRIP                      FRAC16(0.771349862259)
#define N_NOM                           FRAC16(0.800000000000)
#define I_PH_NOM                        FRAC16(0.208750000000)
#define U_PH_NOM                        FRAC16(0.661157024793)
//Mechanical Alignemnt                  
#define ALIGN_CURRENT                   FRAC16(0.250000000000)
#define ALIGN_DURATION                  (400)

//BLDC Control Loop                     
//----------------------------------------------------------------------
//Loop sample time                      = 0.001 [sec]
//----------------------------------------------------------------------
//Control loop limits                   
#define CTRL_LOOP_LIM_HIGH              FRAC16(0.900000000000)
#define CTRL_LOOP_LIM_LOW               FRAC16(0.0)

//Speed Controller - Parallel type      
#define SPEED_LOOP_KP_GAIN              FRAC16(0.502654824574)
#define SPEED_LOOP_KP_SHIFT             (-7)
#define SPEED_LOOP_KI_GAIN              FRAC16(0.502654824574)
#define SPEED_LOOP_KI_SHIFT             (-7)

//Speed ramp increments                 
#define SPEED_LOOP_RAMP_UP              FRAC32(0.004000000000)
#define SPEED_LOOP_RAMP_DOWN            FRAC32(0.004000000000)

//Torque Controller - Parallel type     
#define TORQUE_LOOP_KP_GAIN             FRAC16(0.502126721763)
#define TORQUE_LOOP_KP_SHIFT            (-7)
#define TORQUE_LOOP_KI_GAIN             FRAC16(0.502126721763)
#define TORQUE_LOOP_KI_SHIFT            (-7)
#define TORQUE_LOOP_MAF                 (5)

//Sensoroless Control Module            
//----------------------------------------------------------------------
//Timer Frequency                       = 562500 [Hz]
//----------------------------------------------------------------------
#define N_MIN                           FRAC16(0.050000000000)
#define N_START_TRH                     FRAC16(0.072000000000)
#define STARTUP_CMT_CNT                 (8)
#define STARTUP_CMT_PER                 (22500)
#define CMT_T_OFF                       FRAC16(0.220000000000)
#define FREEWHEEL_T_LONG                (1000)
#define FREEWHEEL_T_SHORT               (500)
#define SPEED_SCALE_CONST               (3375)
#define CMT_PER_MIN                     (563)
#define START_CMT_ACCELER               FRAC16(0.859750628053)
#define INTEG_TRH                       (40622)

//FreeMASTER Scale Variables            
//----------------------------------------------------------------------
//Note: Scaled at input = 1000          
//----------------------------------------------------------------------
#define FM_I_SCALE                      (8000)
#define FM_U_DCB_SCALE                  (36300)
#define FM_N_SCALE                      (5000000)

#endif
/**********************************************************************/
/**********************************************************************/
