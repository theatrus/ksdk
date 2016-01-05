/*
 * Copyright (c) 2013-2015, Freescale Semiconductor, Inc.
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
#include <stdio.h>
// SDK Included Files
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_clock_manager.h"
#include "fsl_slcd_driver.h"

#define SLCD_INSTANCE 0U
volatile uint32_t i, j;
void LCD_IRQHandler(void)
{
    PRINTF("\r\nSLCD enters interrupt");
    SLCD_DRV_SetIntCmd(SLCD_INSTANCE, kSLCDEnableAllInt, false);
}
  
int main(void)
{
    uint32_t pinNum;
    
    slcd_clk_config_t clkCommonConfig = 
    {
      .clkSrc              = kSLCDDefaultClk,
#if FSL_FEATURE_SLCD_HAS_MULTI_ALTERNATE_CLOCK_SOURCE
      .altClkSrc           = kSLCDAltClkSrc1,
#endif
      .altClkDiv           = kSLCDAltClkDivFactor1,
      .clkPrescaler        = kSLCDClkPrescaler01,
#if FSL_FEATURE_SLCD_HAS_FAST_FRAME_RATE
      .fastFrameRateEnable = false,
#endif
    };
    slcd_work_mode_t lowPowerMode = 
    {
        .kSLCDEnableInDozeMode = false,
        .kSLCDEnableInStopMode = false,
    };
    
    slcd_user_config_t init = 
    {
        .clkConfig       = clkCommonConfig,
        .powerSupply     = kSLCDPowerInternalVll3AndChargePump,
        .loadAdjust      = kSLCDHighLoadOrSlowestClkSrc,
        .dutyCyc         = kSLCD1Div8DutyCyc,
        .slcdIntEnable   = true,
        .workMode        = lowPowerMode,
    };
    slcd_pins_config_t pinsConfig =
    {
        .slcdLowPinsEnabled           = 0xF7D86000,
        .slcdHighPinsEnabled          = 0x0F04387F,
        .slcdBackPlaneLowPinsEnabled  = 0x00586000,
        .slcdBackPlaneHighPinsEnabled = 0x07000000,
    };
    
    slcd_fault_detect_config_t faultDetectConfig =
    {
        .faultDetectCompleteIntEnabled = true,
        .faultDetectBackPlaneEnabled   = true,
        .faultDetectPinIndex           = 0,
        .prescaler                     = kSLCDFaultSampleFreq1BusClk,
        .winWidth                      = kSLCDFaultDetectWinWidth4SampleClk,
    };
    slcd_blink_config_t blinkConfig =
    {
        .blinkRate = kSLCDBlinkRate03,
        .blinkMode = kSLCDAltDisplay,
    };
    
    // Init hardware.
    hardware_init();
    
    OSA_Init();
    
    SLCD_DRV_Init(SLCD_INSTANCE, &init);
    
    SLCD_DRV_SetAllPinsConfig(SLCD_INSTANCE, &pinsConfig);
    
    for(pinNum = 0; pinNum < FSL_FEATURE_SLCD_HAS_PIN_NUM; pinNum++)
    {
        SLCD_DRV_SetPinWaveForm(SLCD_INSTANCE, pinNum, 0xFF);
    }

    SLCD_DRV_SetBackPlanePhase(SLCD_INSTANCE, 14, kSLCDPhaseA);
    SLCD_DRV_SetBackPlanePhase(SLCD_INSTANCE, 20, kSLCDPhaseB);
    SLCD_DRV_SetBackPlanePhase(SLCD_INSTANCE, 22, kSLCDPhaseC);
    SLCD_DRV_SetBackPlanePhase(SLCD_INSTANCE, 56, kSLCDPhaseD);
    SLCD_DRV_SetBackPlanePhase(SLCD_INSTANCE, 58, kSLCDPhaseE);
    SLCD_DRV_SetBackPlanePhase(SLCD_INSTANCE, 13, kSLCDPhaseF);
    SLCD_DRV_SetBackPlanePhase(SLCD_INSTANCE, 19, kSLCDPhaseG);
    SLCD_DRV_SetBackPlanePhase(SLCD_INSTANCE, 57, kSLCDPhaseH);

    
    SLCD_DRV_Start(SLCD_INSTANCE);
   
    SLCD_DRV_SetIntCmd(SLCD_INSTANCE, kSLCDEnableAllInt, true);
    
    SLCD_DRV_StartFaultDetection(SLCD_INSTANCE, &faultDetectConfig);  /*!< SLCD starts fault detection and caused an LCD interrupt later */

    SLCD_DRV_StartBlinkingMode(SLCD_INSTANCE, &blinkConfig);
    
    OSA_TimeDelay(5000);
   
    SLCD_DRV_StopBlinkingMode(SLCD_INSTANCE);
    
    SLCD_DRV_SetBlankDisplayModeCmd(SLCD_INSTANCE, true);             /*!< SLCD starts blank display mode */
    SLCD_DRV_SetSegmentEnableCmd(SLCD_INSTANCE, 20, kSLCDPhaseA, false);
    SLCD_DRV_SetSegmentEnableCmd(SLCD_INSTANCE, 20, kSLCDPhaseB, false);
    SLCD_DRV_SetSegmentEnableCmd(SLCD_INSTANCE, 20, kSLCDPhaseC, false);
    SLCD_DRV_SetSegmentEnableCmd(SLCD_INSTANCE, 20, kSLCDPhaseD, false);
    SLCD_DRV_SetSegmentEnableCmd(SLCD_INSTANCE, 20, kSLCDPhaseE, false);
    SLCD_DRV_SetSegmentEnableCmd(SLCD_INSTANCE, 20, kSLCDPhaseF, false);
    SLCD_DRV_SetSegmentEnableCmd(SLCD_INSTANCE, 20, kSLCDPhaseG, false);
    SLCD_DRV_SetSegmentEnableCmd(SLCD_INSTANCE, 20, kSLCDPhaseH, false);

    OSA_TimeDelay(5000);
    SLCD_DRV_SetBlankDisplayModeCmd(SLCD_INSTANCE, false);              /*!< SLCD stops blank display mode */

    while (1)
    {
    }
}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
