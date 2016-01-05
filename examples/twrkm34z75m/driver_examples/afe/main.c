/*
 * Copyright (c) 2014 - 2015, Freescale Semiconductor, Inc.
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
//  Includes
///////////////////////////////////////////////////////////////////////////////

// Standard C Included Files
#include <stdio.h>
#include <stdlib.h>

// SDK Included Files
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_afe_driver.h"
#include "fsl_vref_hal.h"
#include "fsl_mcg_hal.h"
#include "fsl_osc_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_os_abstraction.h"
////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void configurate_vref(void);
void configurate_fll_osc_init(void);

static volatile int32_t result0, result1;

/*!
 * @brief Main function
 */
int main (void)
{
    uint32_t afeInstance = 0;
    
     /* Initialize standard SDK demo application pins */
    hardware_init();
    
    /* Clock and OSC init */
    configurate_fll_osc_init();
    
    // Init OSA layer.
    OSA_Init();
    
    /* VREF init */
    configurate_vref();        

    /* Call this function to initialize the console UART. This function 
       enables the use of STDIO functions (PRINTF, SCANF, etc.)*/
    dbg_uart_init();
    
    /* Print the initial banner */
    PRINTF("AFE Example:\r\nAn 8MHz external crystal is used as a clock source for AFE in this example.\r\n\r\n");
    PRINTF("\r\nSingle and polling mode is selected.\r\n\r\n");
  
    
    afe_user_config_t afeExampleStruct;
    afeExampleStruct.clkDividerMode = kAfeClkDividerInputOf2;
    afeExampleStruct.clkSrcMode = kAfeClkSrcClk1;
    afeExampleStruct.lowPowerEnable = false;
    afeExampleStruct.resultFormat = kAfeResultFormatRight;
    afeExampleStruct.startupCnt =80; /* startupCnt = (Clk_freq/Clk_div)*20e-6 */
    
    afe_chn_config_t afeChnExampleStruct;
    afeChnExampleStruct.chnMode = kAfeNormal;
    afeChnExampleStruct.continuousConvEnable = false;
    afeChnExampleStruct.delay = 0;
    afeChnExampleStruct.chnEvent = kAfeNoneReq;
    afeChnExampleStruct.hwTriggerEnable = false;
    afeChnExampleStruct.pgaGainSel = kAfePgaDisable;
    afeChnExampleStruct.decimOSR = kAfeDecimOsrOf64;
    
    /* Initiate and configurate the AFE module */
    AFE_DRV_Init(afeInstance, &afeExampleStruct);
      
    /* Initiate and configurate the AFE channel 0 */
    AFE_DRV_ChnInit(afeInstance, 0, &afeChnExampleStruct);
        
    /* AFE delay assertion and AFE enabling */
    AFE_DRV_AssertDelayOk(afeInstance);
    AFE_DRV_Enable(afeInstance, true);

    AFE_DRV_SoftTriggerConv(afeInstance, CHN_TRIG_MASK(0));
   /*
    *Note: 
    * Please connect DC power output positive signal to pin EXT_SD_ADP0, 
    * negative signal to pin EXT_SD_ADM0. The measurable output voltage range 
    * is from 0 and 500mV.
    * Adjust the output power value, then see the printed log to observe
    * results;
    */
    
    while(1)
    {
        if(AFE_DRV_GetChnFlag(afeInstance,0,kAfeConvCompleteFlag))
        {
            AFE_DRV_WaitConvDone(afeInstance,0);
            result0 = AFE_DRV_GetChnConvVal(afeInstance,0);
            PRINTF("result0  = %d  0x%x \r\n", result0, result0); 
            OSA_TimeDelay(500u);
            AFE_DRV_SoftTriggerConv(afeInstance, CHN_TRIG_MASK(0));
        }
    }
}

void configurate_vref(void){
    
    CLOCK_SYS_EnableVrefClock(0);  
    
    VREF_HAL_SetChopOscillatorCmd(VREF, true);
    VREF_HAL_SetBufferMode(VREF, kVrefModeHighPowerBuffer);
    VREF_HAL_SetVoltageReference(VREF,kVrefReferenceInternal);
    VREF_HAL_SetInternalRegulatorCmd(VREF, true);
    VREF_HAL_SetLowReferenceTrimVal(VREF,3);
    VREF_HAL_SetTrimVal(VREF,46);
    VREF_HAL_SetLowReference(VREF, true);

    VREF_HAL_Enable(VREF);
    VREF_HAL_WaitVoltageStable(VREF);

}

void configurate_fll_osc_init(void){
    
    OSC_HAL_SetExternalRefClkCmd(OSC, true);
    CLOCK_HAL_SetAfeClkSrc(SIM, kClockAfeSrcOscClk);
    CLOCK_HAL_SetFeeMode(MCG,
                         kMcgOscselOsc,
                         3,
                         kMcgDmx32Default,
                         kMcgDcoRangeSelHigh,
                         NULL);
}

