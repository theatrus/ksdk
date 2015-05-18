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
 ///////////////////////////////////////////////////////////////////////////////
 // Includes
 ///////////////////////////////////////////////////////////////////////////////

 // Standard C Included Files
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// SDK Included Files
#include "board.h"
#include "fsl_cmp_driver.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define CMP_INSTANCE    BOARD_CMP_INSTANCE    /*! The cmp instance */

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
volatile bool bRisingEvent = false;
volatile bool bFallingEvent = false;

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
* @brief Compare analog input to reference DAC output.
* This function compares analog input (SW1) to reference DAC output to control a LED
* If the analog input higher than DAC output, led is turned on
* otherwise led is turned off
*/
int main(void)
{
    // Configuration for cmp
    cmp_state_t cmpState;
    cmp_comparator_config_t cmpUserConfig;
    cmp_sample_filter_config_t cmpSampleFilterConfig;
    cmp_dac_config_t cmpDacConfig;

    // Initialize hardware
    hardware_init();

    // Initialize & turn-off LED1
    LED1_EN;
    LED1_OFF;

    PRINTF("The demo compares analog input to reference DAC output to control a LED.\n\
    \rLED is turned ON/OFF when analog input is LOWER/HIGHER than DAC output\n\r");

    // Init IO for CMP. Booad don't have pull up resister, so internal resister need to be enabled
#if defined(TWR_K64F120M) ||defined(FRDM_K64F) || defined(TWR_K24F120M) || defined(TWR_K22F120M) || defined(TWR_K21F120M) ||\
    defined(TWR_KV31F120M) || defined(TWR_K21D50M) || defined(TWR_KW24D512)
    PORT_HAL_SetMuxMode(PORTC,6u,kPortMuxAsGpio);
    PORT_HAL_SetPullMode(PORTC,6U,kPortPullUp);
    PORT_HAL_SetPullCmd(PORTC,6U,true);
#endif

    // Enable rising interrupt
    // Enable falling interrupt
    // Init the CMP comparator.
    CMP_DRV_StructInitUserConfigDefault(&cmpUserConfig, (cmp_chn_mux_mode_t)BOARD_CMP_CHANNEL, kCmpInputChnDac);
    cmpUserConfig.risingIntEnable = true;
    cmpUserConfig.fallingIntEnable = true;
    CMP_DRV_Init(CMP_INSTANCE, &cmpState, &cmpUserConfig);

    // Configure the internal DAC when in used.
    cmpDacConfig.dacEnable = true;
    cmpDacConfig.dacValue = 32U; // 0U - 63U
    cmpDacConfig.refVoltSrcMode = kCmpDacRefVoltSrcOf2;
    CMP_DRV_ConfigDacChn(CMP_INSTANCE, &cmpDacConfig);

    // Configure the Sample/Filter Mode.
    cmpSampleFilterConfig.workMode = kCmpContinuousMode;
    CMP_DRV_ConfigSampleFilter(CMP_INSTANCE, &cmpSampleFilterConfig);

    // Start the CMP function.
    CMP_DRV_Start(CMP_INSTANCE);
#if defined(TWR_K65F180M)
    PRINTF("\n\rChange potentiometer position and see status of led\n\n\r");
#else
    PRINTF("\n\rPress %s and see status of led\n\n\r", (uint8_t*)BOARD_CMP_SW_NAME);    
#endif
    while (1)
    {
        // If rising interrupt occurs
        if (bRisingEvent)
        {
            PRINTF("The analog input is HIGHER than DAC output!\r");
            bRisingEvent = false;
            // Turn off led1
            LED1_OFF;
        }
        // If falling interrupt occurs
        if (bFallingEvent)
        {
            PRINTF("The analog input is LOWER  than DAC output!\r");
            bFallingEvent = false;
            // Turn on led1
            LED1_ON;
        }
    }
}

/*!
* @brief The user-defined ISR
*/
void CMP_UserIsr(void)
{
    // If rising interrupt
    if (CMP_DRV_GetFlag(CMP_INSTANCE, kCmpFlagOfCoutRising) )
    {
        if (!bRisingEvent)
        {
            bRisingEvent = true;
        }
    }

    // If falling interrupt
    if (CMP_DRV_GetFlag(CMP_INSTANCE, kCmpFlagOfCoutFalling) )
    {
        if (!bFallingEvent)
        {
            bFallingEvent = true;
        }
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/

