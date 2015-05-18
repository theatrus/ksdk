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

#ifndef __hvac_private_h__
#define __hvac_private_h__

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include <fsl_os_abstraction.h>

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define HVAC_CONTROL_CYCLE_IN_SECONDS    1

#define HVAC_CONTROL_CYCLE_IN_TICKS      (HVAC_CONTROL_CYCLE_IN_SECONDS*BSP_ALARM_FREQUENCY)

#define HVAC_DEFAULT_TEMP     200   // in 1/10 degree C

#define HVAC_PARAMS_CHANGED  1

#define ENABLE_ADC 1

#define LED_1        1
#define LED_2        1
#define LED_3        1
#define LED_4        1

typedef struct {
   event_flags_t       Event;
   HVAC_Mode_t          HVACMode;
   FAN_Mode_t           FanMode;
   Temperature_Scale_t  TemperatureScale;
   uint32_t              DesiredTemperature;
} HVAC_PARAMS, * HVAC_PARAMS_PTR;

typedef struct  {
   HVAC_Mode_t    HVACState;
   bool        FanOn;
   uint32_t        ActualTemperature;
} HVAC_STATE, * HVAC_STATE_PTR;


extern HVAC_STATE  HVAC_State;
extern HVAC_PARAMS HVAC_Params;

void HVAC_InitializeParameters(void);
bool HVAC_InitializeIO(void);
void HVAC_ResetOutputs(void);
void HVAC_SetOutput(HVAC_Output_t,bool);
void HVAC_InitializeADC(void);
int32_t ReadADC(void);

#endif
