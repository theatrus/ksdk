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

HVAC_PARAMS HVAC_Params = {0};

char * HVACModeName[] = {"Off", "Cool", "Heat", "Auto"};

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Initialize parameters for HVAC.
 */
void HVAC_InitializeParameters(void)
{
   HVAC_Params.HVACMode = HVAC_Auto;
   HVAC_Params.FanMode = Fan_Automatic;
   HVAC_Params.TemperatureScale = Celsius;
   HVAC_Params.DesiredTemperature = HVAC_DEFAULT_TEMP;
}

/*!
 * @brief Get HVAC mode.
 */
char * HVAC_HVACModeName(HVAC_Mode_t mode)
{
   return HVACModeName[mode];
}

/*!
 * @brief Convert temperature to display.
 */
uint32_t HVAC_ConvertCelsiusToDisplayTemp(uint32_t temp)
{
   uint32_t  display_temp;

   if (HVAC_Params.TemperatureScale ==  Celsius) {
      display_temp = temp;
   } else {
      display_temp = temp*9/5+320;
   }

   return display_temp;
}

/*!
 * @brief Convert temperature.
 */
uint32_t HVAC_ConvertDisplayTempToCelsius(uint32_t display_temp)
{
   uint32_t  temp;

   if (HVAC_Params.TemperatureScale ==  Celsius) {
      temp = display_temp;
   } else {
      temp = (display_temp-320)*5/9;
   }

   return temp;
}

/*!
 * @brief Get desired temperature.
 */
uint32_t HVAC_GetDesiredTemperature(void) {
   return HVAC_ConvertCelsiusToDisplayTemp(HVAC_Params.DesiredTemperature);
}

/*!
 * @brief Set desired temperature.
 */
void HVAC_SetDesiredTemperature(uint32_t temp)
{
   HVAC_Params.DesiredTemperature = HVAC_ConvertDisplayTempToCelsius(temp);
}

/*!
 * @brief Get fan mode.
 */
FAN_Mode_t HVAC_GetFanMode(void) {
   return HVAC_Params.FanMode;
}

/*!
 * @brief get fan mode.
 */
void HVAC_SetFanMode(FAN_Mode_t mode)
{
   HVAC_Params.FanMode = mode;
}

/*!
 * @brief Get HVAC mode.
 */
HVAC_Mode_t HVAC_GetHVACMode(void)
{
   return HVAC_Params.HVACMode;
}

/*!
 * @brief Set HVAC mode.
 */
void HVAC_SetHVACMode(HVAC_Mode_t mode)
{
   HVAC_Params.HVACMode = mode;
}

/*!
 * @brief Get actual temperature.
 */
uint32_t HVAC_GetActualTemperature(void) {
   return HVAC_ConvertCelsiusToDisplayTemp(HVAC_State.ActualTemperature);
}

/*!
 * @brief Get temperature scale.
 */
Temperature_Scale_t HVAC_GetTemperatureScale(void)
{
   return HVAC_Params.TemperatureScale;
}

/*!
 * @brief Set temperature scale.
 */
void HVAC_SetTemperatureScale(Temperature_Scale_t scale)
{
   HVAC_Params.TemperatureScale = scale;
}

/*!
 * @brief Get temperature symbol.
 */
char HVAC_GetTemperatureSymbol(void)
{
   return (HVAC_Params.TemperatureScale==Celsius)?'C':'F';
}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
