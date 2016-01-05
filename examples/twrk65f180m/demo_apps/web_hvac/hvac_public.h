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

#ifndef __HVAC_PUBLIC_H__
#define __HVAC_PUBLIC_H__

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

// HVAC user interface
#define HVAC_TEMP_TOLERANCE   0    // in 1/10 degree
#define HVAC_TEMP_SW_DELTA    5    // in 1/10 degree

// ambient temperature change emulation
#define HVAC_TEMP_UPD_DELTA   1    // in 1/10 degree
#define HVAC_TEMP_UPDATE_RATE 1    // in seconds

#define HVAC_TEMP_MINIMUM     0    // in Celsius
#define HVAC_TEMP_MAXIMUM     500  // in Celsius

typedef enum {
   HVAC_FAN_OUTPUT=0,
   HVAC_HEAT_OUTPUT,
   HVAC_COOL_OUTPUT,
   HVAC_ALIVE_OUTPUT,
   HVAC_MAX_OUTPUTS
} HVAC_Output_t;

typedef enum {
   HVAC_TEMP_UP_INPUT=0,
   HVAC_TEMP_DOWN_INPUT,
   HVAC_FAN_ON_INPUT,
   HVAC_HEAT_ON_INPUT,
   HVAC_AC_ON_INPUT,
   HVAC_MAX_INPUTS
} HVAC_Input_t;


typedef enum {
   HVAC_Off,
   HVAC_Cool,
   HVAC_Heat,
   HVAC_Auto
} HVAC_Mode_t;

typedef enum {
   Fan_Automatic,
   Fan_On
} FAN_Mode_t;

typedef enum {
   Fahrenheit,
   Celsius
} Temperature_Scale_t;

extern void HVAC_SetDesiredTemperature(uint32_t);
extern uint32_t HVAC_GetDesiredTemperature(void);

extern void HVAC_SetFanMode(FAN_Mode_t);
extern FAN_Mode_t HVAC_GetFanMode(void);

extern void HVAC_SetHVACMode(HVAC_Mode_t);
extern HVAC_Mode_t HVAC_GetHVACMode(void);
extern char * HVAC_HVACModeName(HVAC_Mode_t mode);

extern uint32_t HVAC_ConvertCelsiusToDisplayTemp(uint32_t temp);
extern uint32_t HVAC_ConvertDisplayTempToCelsius(uint32_t display_temp);
extern void HVAC_SetTemperatureScale(Temperature_Scale_t);
extern Temperature_Scale_t HVAC_GetTemperatureScale(void);
extern char HVAC_GetTemperatureSymbol(void);

extern uint32_t HVAC_GetAmbientTemperature(void);
extern uint32_t HVAC_GetActualTemperature(void);
extern void HVAC_ReadAmbientTemperature(void);

extern bool HVAC_GetOutput(HVAC_Output_t);
extern char * HVAC_GetOutputName(HVAC_Output_t);



#endif
