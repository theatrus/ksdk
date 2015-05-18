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
 *
 */

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

// Standard C Included Files
#include <stdio.h>
// SDK Included Files
#include "fsl_debug_console.h"
#include "fsl_os_abstraction.h"
// Application Included Files
#include "demo_state_machine.h"

///////////////////////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////////////////////

dac_converter_config_t *g_dacConfig;
adc16_converter_config_t *g_adcConfig;
adc16_chn_config_t *g_chnConfig;

adc16_calibration_param_t g_adcCalibraitionParam;

demo_state_t g_demoState = kStart;

demo_machine_t g_demoMachine[] =
{
    { kStart       ,  demo_start    },
    { kConfigDevice,  device_config },
    { kSetDAC      ,  dac_set       },
    { kWait        ,  wait_state    },
    { kGetADC      ,  adc_get       },
    { kDeinitDevice,  device_deinit },
    { kEndState    ,  demo_end      },
    { kStop        ,  NULL          }
};

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

uint8_t demo_start(demo_state_t *prevState)
{
    volatile uint8_t msg;

    OSA_Init();

    #if defined(__GNUC__)
    setvbuf(stdin, NULL, _IONBF, 0);
    #endif

    PRINTF("\r\nDAC ADC Demo!\r\n");
    PRINTF("\r\nPlease refer to Kinetis SDK Demo Applications User's Guide document,\r\n");
    PRINTF("\r\nChapter DAC ADC demo, for pins configuration information.\r\n");

    PRINTF("\r\nPress spacebar to start demo.\r\n");

    msg = 'A';

    while(msg != ' ')
    {
        msg = GETCHAR();
    }

    PRINTF("\r\nDemo begin...\r\n" );

    // Set previous state to current state
    *prevState = g_demoState;

    // Set next state
    g_demoState = kConfigDevice;

    return msg;
}

uint8_t device_config(demo_state_t *prevState)
{
    // Allocate memory for configuration structures to avoid memory overwrites
    g_dacConfig = (dac_converter_config_t *)OSA_MemAlloc(sizeof(dac_converter_config_t));
    g_adcConfig = (adc16_converter_config_t *)OSA_MemAlloc(sizeof(adc16_converter_config_t));
    g_chnConfig = (adc16_chn_config_t *)OSA_MemAlloc(sizeof(adc16_chn_config_t));

    //////////////////////////////////////////
    // Configure DAC for software triggering.//
    //////////////////////////////////////////

    // Set configuration for basic operation
    DAC_DRV_StructInitUserConfigNormal(g_dacConfig);

    // Initialize DAC with basic configuration
    DAC_DRV_Init(BOARD_DAC_DEMO_DAC_INSTANCE, g_dacConfig);

    ///////////////////
    // Configure ADC //
    ///////////////////

    // Configure ADC for blocking mode
    ADC16_DRV_StructInitUserConfigDefault(g_adcConfig);
    /* Notice!
    * VREF module's output signal shares pin with Vrefh, which is the
    * default external reference voltage source for ADC module. So user should
    * be careful when connecting external voltage to the Vrefh pin on board.
    * Even in some reference design, the Vrefh pin is float as default.
    * However, the ADC module could have Valt as another selection for ADC
    * reference voltage. Some chips like KL43 and KL27, make the connection
    * of VDDA and Valth inside the chip. On this condition, to use Valt would
    * be safer.
    */
#if (  defined(FRDM_KL43Z)   /* CPU_MKL43Z256VLH4 */ \
    || defined(TWR_KL43Z48M) /* CPU_MKL43Z256VLH4 */ \
    || defined(FRDM_KL27Z)   /* CPU_MKL27Z64VLH4  */ \
    )
    g_adcConfig->refVoltSrc = kAdc16RefVoltSrcOfValt;
#endif
    ADC16_DRV_Init(BOARD_DAC_DEMO_ADC_INSTANCE, g_adcConfig);
    
    // Calibrate ADC
    ADC16_DRV_GetAutoCalibrationParam(BOARD_DAC_DEMO_ADC_INSTANCE, &g_adcCalibraitionParam);
    ADC16_DRV_SetCalibrationParam(BOARD_DAC_DEMO_ADC_INSTANCE, &g_adcCalibraitionParam);

    // Configure ADC for blocking mode
    g_adcConfig->continuousConvEnable = true;
    ADC16_DRV_Init(BOARD_DAC_DEMO_ADC_INSTANCE, g_adcConfig);

    // Trigger channel
    g_chnConfig->chnIdx = (adc16_chn_t)BOARD_DAC_DEMO_ADC_CHANNEL;
    g_chnConfig->diffConvEnable= false;
    g_chnConfig->convCompletedIntEnable = false;
    ADC16_DRV_ConfigConvChn(BOARD_DAC_DEMO_ADC_INSTANCE, 0U, g_chnConfig);

    // Set previous state to current state
    *prevState = g_demoState;

    // Set next state
    g_demoState = kSetDAC;

    return 0;
}

uint8_t dac_set(demo_state_t *prevState)
{
    uint8_t msg;

    PRINTF("\r\n\r\nSelect DAC output level:\r\n\t1. 1.0 V\r\n\t2. 1.5 V\r\n\t3. 2.0 V\r\n\t4. 2.5 V\r\n\t5. 3.0 V\r\n-> ");

    msg = ' ';

    while((msg < '1') || (msg > '5'))
    {
        PRINTF("\b");
        msg = GETCHAR();
        PRINTF("%c", msg);
    }

    switch(msg)
    {
        case '1':
            // Set DAC output
            DAC_DRV_Output(BOARD_DAC_DEMO_DAC_INSTANCE, DAC_1_0_VOLTS);
            break;

        case '2':
            // Set DAC output
            DAC_DRV_Output(BOARD_DAC_DEMO_DAC_INSTANCE, DAC_1_5_VOLTS);
            break;

        case '3':
            // Set DAC output
            DAC_DRV_Output(BOARD_DAC_DEMO_DAC_INSTANCE, DAC_2_0_VOLTS);
            break;

        case '4':
            // Set DAC output
            DAC_DRV_Output(BOARD_DAC_DEMO_DAC_INSTANCE, DAC_2_5_VOLTS);
            break;

        case '5':
            // Set DAC output
            DAC_DRV_Output(BOARD_DAC_DEMO_DAC_INSTANCE, DAC_3_0_VOLTS);
            break;

        default:
            // Set DAC output
            DAC_DRV_Output(BOARD_DAC_DEMO_DAC_INSTANCE, 0U);
            break;
    }

    // Set previous state to current state
    *prevState = g_demoState;

    // Set next state
    g_demoState = kWait;

    return msg;
}

uint8_t wait_state(demo_state_t *prevState)
{
    switch(*prevState)
    {
        case kSetDAC:
            // Wait for 500 us
            OSA_TimeDelay(200);
            // Set next state
            g_demoState = kGetADC;
            break;
        case kGetADC:
            // Wait for 500 us
            OSA_TimeDelay(200);
            // Set next state
            g_demoState = kSetDAC;
            break;
        default:
            // Wait for 500 us
            OSA_TimeDelay(200);
            break;
    }

    return 0;
}

uint8_t adc_get(demo_state_t *prevState)
{
    uint8_t msg;
    uint16_t adcData;
    float voltRead;
    uint8_t voltOne;
    uint8_t voltTenth;
    uint8_t voltHundreth;

    // Get ADC input
    ADC16_DRV_WaitConvDone(BOARD_DAC_DEMO_ADC_INSTANCE, 0U);

    adcData = ADC16_DRV_GetConvValueSigned(BOARD_DAC_DEMO_ADC_INSTANCE, 0U);

    PRINTF("\r\n\r\nADC Value: %d\r\n", adcData);

    // Convert ADC value to a voltage based on 3.3V VREFH on board
    voltRead = (float)adcData * (VREF_BRD / SE_12BIT);

    // Get the firtst digit before decimal place
    voltOne = (uint8_t)voltRead;

    // Get the first digit after the decimal place
    voltTenth = (uint8_t)((voltRead - voltOne) * 10);

    // Get the second digit after the decimal place
    voltHundreth = (uint8_t)((((voltRead - voltOne) * 10) - voltTenth) * 10);

    PRINTF("\r\nADC Voltage: %d.%d%d\r\n", voltOne, voltTenth, voltHundreth);

    // Set previous state to current state
    *prevState = g_demoState;

    // Determine what to do next based on user's request
    PRINTF("\r\n\r\nWhat next?:\r\n\t1. Test another DAC output value.\r\n\t2. Terminate demo.\r\n-> ");

    msg = ' ';

    while((msg < '1') || (msg > '2'))
    {
        PRINTF("\b");
        msg = GETCHAR();
        PRINTF("%c", msg);
    }

    // Set next state
    if(msg == '1')
    {
        g_demoState = kWait;
    }
    else
    {
        g_demoState = kDeinitDevice;

        // Pause the conversion after testing
        ADC16_DRV_PauseConv(BOARD_DAC_DEMO_ADC_INSTANCE, 0U);
    }

    return msg;
}

uint8_t device_deinit(demo_state_t *prevState)
{
    // De initialize DAC & ADC
    DAC_DRV_Deinit(BOARD_DAC_DEMO_DAC_INSTANCE);
    ADC16_DRV_Deinit(BOARD_DAC_DEMO_ADC_INSTANCE);

    // Free allocated memory
    OSA_MemFree(g_dacConfig);
    OSA_MemFree(g_adcConfig);
    OSA_MemFree(g_chnConfig);

    // Set previous state to current state
    *prevState = g_demoState;

    // Set next state
    g_demoState = kEndState;

    return 0;
}

uint8_t demo_end(demo_state_t *prevState)
{
    PRINTF("\r\nDemo terminated. Reset device to begin again.\r\n");

    // Set previous state to current state
    *prevState = g_demoState;

    // Set next state
    g_demoState = kStop;

    return 0;
}
