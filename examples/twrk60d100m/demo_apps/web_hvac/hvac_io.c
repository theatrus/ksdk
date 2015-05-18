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
#include <string.h>

#include "board.h"

#include "hvac.h"
#include "hvac_public.h"
#include "hvac_private.h"
#include "adc_inf.h"

#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#if ENABLE_ADC
#define ADC_0                   (0U)
#define CHANNEL_0               (0U)
#define ADCR_VDD                (65535U)    // Maximum value when use 16b resolution.
#define V_BG                    (1000U)     // BANDGAP voltage in mV (trim to 1.0V).
#define V_TEMP25                (716U)      // Typical VTEMP25 in mV.
#define M                       (1620U)     // Typical slope: (mV x 1000)/oC.
#define STANDARD_TEMP           (25)

#define UPPER_VALUE_LIMIT       (1U)        // This value/10 is going to be added to current Temp to set the upper boundary.
#define LOWER_VALUE_LIMIT       (1U)        // This Value/10 is going to be subtracted from current Temp to set the lower boundary.
#define UPDATE_BOUNDARIES_TIME  (20U)       // This value indicates the number of cycles needed to update boundaries. To know the Time it will take, multiply this value times LPTMR_COMPARE_VALUE.
#define kAdcChannelTemperature  (26U)       // ADC channel of temperature sensor.
#define kAdcChannelBandgap      (27U)       // ADC channel of BANDGAP.
#endif


///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////

#if ENABLE_ADC
extern void init_trigger_source(uint32_t instance);
#endif

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

char * HVAC_OutputName[] = { "Fan", "Furnace", "A/C", "HeartBeat" };

bool HVAC_OutputState[HVAC_MAX_OUTPUTS] ={0};
static uint32_t AmbientTemperature = 200; // 20.0 degrees celsius, 68.0 degrees fahrenheit

gpio_input_pin_user_config_t *swConfigs = NULL;

#if ENABLE_ADC
extern volatile bool conversionCompleted;
#endif

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief SWs Interrupt service fuction.
 */
void sw_irq_callback(void)
{
    if ((swConfigs[0].pinName != GPIO_PINS_OUT_OF_RANGE) && !GPIO_DRV_ReadPinInput(swConfigs[0].pinName))
    {
        if (HVAC_GetDesiredTemperature() < HVAC_ConvertCelsiusToDisplayTemp(HVAC_TEMP_MAXIMUM - HVAC_TEMP_SW_DELTA))
        {
           HVAC_SetDesiredTemperature(HVAC_GetDesiredTemperature()+HVAC_TEMP_SW_DELTA);
        }
        // Clear SW1 external interrupt flag.
        GPIO_DRV_ClearPinIntFlag(swConfigs[0].pinName);
    }
    else if ((swConfigs[1].pinName != GPIO_PINS_OUT_OF_RANGE) && !GPIO_DRV_ReadPinInput(swConfigs[1].pinName))
    {
        if (HVAC_GetDesiredTemperature() > HVAC_ConvertCelsiusToDisplayTemp(HVAC_TEMP_MINIMUM + HVAC_TEMP_SW_DELTA))
        {
           HVAC_SetDesiredTemperature(HVAC_GetDesiredTemperature()-HVAC_TEMP_SW_DELTA);
        }
        // Clear SW2 external interrupt flag.
        GPIO_DRV_ClearPinIntFlag(swConfigs[1].pinName);
    }
}

/*!
 * @brief Initialize LEDs and SWs for HVAC simulation.
 */
bool HVAC_InitializeIO(void)
{
    uint32_t i = 0, swCount;
    // Table to save port IRQ enum numbers defined in CMSIS files.
    const IRQn_Type g_portIrqId[PORT_INSTANCE_COUNT] = PORT_IRQS;
    uint32_t port;

    // Get the size of switchPins structure
    for (swCount = 0;; swCount++)
    {
        if (switchPins[swCount].pinName == GPIO_PINS_OUT_OF_RANGE)
        {
            swCount += 1;
            break;
        }
    }
    swConfigs = (gpio_input_pin_user_config_t *)malloc(sizeof(gpio_input_pin_user_config_t)*swCount);
    if(swConfigs == NULL)
    {
        PRINTF("Failed to allocate memory!!\r\n");
        return 0;
    }
    memcpy(swConfigs, &switchPins, sizeof(gpio_input_pin_user_config_t)*swCount);
    // Enable external interrupt.
    while (swConfigs[i].pinName != GPIO_PINS_OUT_OF_RANGE)
    {
        swConfigs[i].config.interrupt = kPortIntFallingEdge;
        i++;
    }
    // Initialize SWs and LEDs
    GPIO_DRV_Init(swConfigs, ledPins);

    while (i > 0)
    {
        i--;
        // Configure switch interrupt
        port = GPIO_EXTRACT_PORT(swConfigs[i].pinName);
        NVIC_SetPriority(g_portIrqId[port], 6U);
        OSA_InstallIntHandler(g_portIrqId[port], sw_irq_callback);
    }

    return 0;
}

/*!
 * @brief Reset output.
 */
void HVAC_ResetOutputs(void)
{
   uint32_t i;

   for (i=0;i<HVAC_MAX_OUTPUTS;i++) {
      HVAC_SetOutput((HVAC_Output_t)i,FALSE);
   }
}

/*!
 * @brief Set output.
 */
void HVAC_SetOutput(HVAC_Output_t signal,bool state)
{
   if (HVAC_OutputState[signal] != state) {
      HVAC_OutputState[signal] = state;
      switch (signal) {
#if LED_1
          case HVAC_FAN_OUTPUT:
              (state) ? LED1_ON:LED1_OFF;
               break;
#endif
#if LED_2
          case HVAC_HEAT_OUTPUT:
              (state) ? LED2_ON:LED2_OFF;
               break;
#endif
#if LED_3
          case HVAC_COOL_OUTPUT:
               (state) ? LED3_ON:LED3_OFF;
                break;
#endif
#if LED_4
          case HVAC_ALIVE_OUTPUT:
               (state) ? LED4_ON:LED4_OFF;
               break;
#endif
          default:
               break;
         }
   }
}

/*!
 * @brief Get the output state.
 */
bool HVAC_GetOutput(HVAC_Output_t signal)
{
   return HVAC_OutputState[signal];
}

/*!
 * @brief Get output name.
 */
char * HVAC_GetOutputName(HVAC_Output_t signal)
{
   return HVAC_OutputName[signal];
}

/*!
 * @brief Get ambient temperature.
 */
uint32_t HVAC_GetAmbientTemperature(void)
{
   return AmbientTemperature;
}

/*!
 * @brief Get ambient temperature.
 */
void HVAC_ReadAmbientTemperature(void)
{
    if (HVAC_GetOutput(HVAC_HEAT_OUTPUT)) {
       AmbientTemperature += HVAC_TEMP_UPD_DELTA;
    } else if (HVAC_GetOutput(HVAC_COOL_OUTPUT)) {
       AmbientTemperature -= HVAC_TEMP_UPD_DELTA;
    }
}

#if ENABLE_ADC

/*!
 * @brief Setup ADC module to read in accelerometer and potentiometer values
 */
void HVAC_InitializeADC(void) {
    int32_t temp_dummy = 0;
    uint32_t updateBoundariesCounter = 0;
    int32_t tempArray[UPDATE_BOUNDARIES_TIME * 2];
    // Calibrate param Temperature sensor
    calibrateParams();
    if (init_adc(ADC_0))
    {
        PRINTF("Failed to do the ADC init\n");
    }
    // setup the HW trigger source
    init_trigger_source(ADC_0);

    // Warm up microcontroller and allow to set first boundaries
    while(updateBoundariesCounter < (UPDATE_BOUNDARIES_TIME * 2))
    {
        while(!conversionCompleted);
        temp_dummy = GetCurrentTempValue();
        tempArray[updateBoundariesCounter] = temp_dummy;
        updateBoundariesCounter++;
        conversionCompleted = false;
    }
    // Temp Sensor Calibration
    TempSensorCalibration(updateBoundariesCounter, tempArray);
    updateBoundariesCounter = 0;
}

/*!
 * @brief Read in ADC value on the channel given
 */
int32_t ReadADC(void) {
  return (int32_t) GetCurrentTempValue();
}

#endif // ENABLE_ADC

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
