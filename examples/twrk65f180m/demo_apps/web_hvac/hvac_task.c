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

#include <stdio.h>

#include "board.h"
#include "fsl_os_abstraction.h"

#include "hvac.h"
#include "hvac_public.h"
#include "hvac_private.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

HVAC_STATE  HVAC_State =  {HVAC_Off};

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief The HVAC task
 */
void HVAC_Task(task_param_t pvParameters)
{
   HVAC_Mode_t mode;

   // Initialize operatiing parameters to default values
   HVAC_InitializeParameters();

   // Configure and reset outputs
   HVAC_InitializeIO();

   while (TRUE) {
      // Read current temperature
      HVAC_ReadAmbientTemperature();
      HVAC_State.ActualTemperature = HVAC_GetAmbientTemperature();
      OSA_TimeDelay(1500);

      // Examine current parameters and set state accordingly
      HVAC_State.HVACState = HVAC_Off;
      HVAC_State.FanOn = FALSE;

      mode = HVAC_GetHVACMode();

      if (mode == HVAC_Cool || mode == HVAC_Auto)
      {
         if (HVAC_State.ActualTemperature > (HVAC_Params.DesiredTemperature+HVAC_TEMP_TOLERANCE))
         {
            HVAC_State.HVACState = HVAC_Cool;
            HVAC_State.FanOn = TRUE;
         }
      }

      if (mode == HVAC_Heat || mode == HVAC_Auto)
      {
         if (HVAC_State.ActualTemperature < (HVAC_Params.DesiredTemperature-HVAC_TEMP_TOLERANCE))
         {
            HVAC_State.HVACState = HVAC_Heat;
            HVAC_State.FanOn = TRUE;
         }
      }

      if (HVAC_GetFanMode() == Fan_On) {
         HVAC_State.FanOn = TRUE;
      }

      // Set outputs to reflect new state
      HVAC_SetOutput(HVAC_FAN_OUTPUT, HVAC_State.FanOn);
      HVAC_SetOutput(HVAC_HEAT_OUTPUT,  HVAC_State.HVACState == HVAC_Heat);
      HVAC_SetOutput(HVAC_COOL_OUTPUT,  HVAC_State.HVACState == HVAC_Cool);
   }
}

/*!
 * @brief The heart beat task
 */
void HeartBeat_Task(task_param_t pvParameters)
{
    // initialize IO before starting this task

    int32_t delay, value = 0;

#if ENABLE_ADC
    HVAC_InitializeADC();
#endif
    while (TRUE) {
#if ENABLE_ADC
        delay = 100 + (1000 * ReadADC() / 0x0fff);
#else
        delay = 100;
#endif
        OSA_TimeDelay(delay);
        HVAC_SetOutput(HVAC_ALIVE_OUTPUT, value);

        value ^= 1;  // toggle next value
    }

}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
