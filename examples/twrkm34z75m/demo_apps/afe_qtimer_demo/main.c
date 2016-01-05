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
//  Includes
///////////////////////////////////////////////////////////////////////////////
// Standard C Included Files
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

// SDK Included Files
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_afe_hal.h"
#include "fsl_vref_hal.h"
#include "fsl_mcg_hal.h"
#include "fsl_osc_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_interrupt_manager.h"
#include "freemaster.h"
#include "fsl_xbar_signals.h"
#include "fsl_xbar_hal.h"
#include "fsl_xbar_driver.h"
#include "fsl_cmp_hal.h"
#include "fsl_quadtmr_hal.h"

//#define Use_FreeMaster  //Default use openSDA debug_console, Freemaster GUI just for option

#define CORE_CLK	72000000
#define BUS_CLK		24000000
#define UART_BAUD_RATE  115200
#define TMRPRCLK		(double)(BUS_CLK/16.0)
#define TMR2FREQ(x) 	(double)(TMRPRCLK/(double)x)

#define afe_freq 12288000


////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////
/* static data definitions */
volatile uint32_t value_ch2 = 0;
volatile uint32_t value_ch3 = 0;
double freq_tmr;
uint16_t tmp16;

void PLL_Enable(void);
void VREF_Config(void);
void AFE_Config(void);
void AFE_TEST_CallbackChn2(void);
void AFE_TEST_CallbackChn3(void);
void AFE_TEST_InstallCallback(uint32_t instance, uint32_t chn, void (*callbackFunc)(void) );
void diagnostic_setup (void);
void CMP1_Config(void);
void QTMER0_Config(void);
void XBAR_Config(void);
static uint8_t TMRCH0_ReadClrCaptReg (uint16_t *ptr);
static int32_t double_to_int(double real, uint32_t *sign_val, uint32_t *int_val, uint32_t *frag_val);


////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/*!
* @brief Main function
*/
int main (void)
{
  uint32_t int_val = 0, frac_val = 0, sign_val = 0;  
  
  hardware_init();
  
  OSA_Init();
  
  PLL_Enable();
  
  /* route internal clock to PTF7 for monitoring  */
  //CLOCK_HAL_SetClkOutSel(SIM, kClockClkoutGatedCoreClk);//Coreclk=72Mhz
  CLOCK_HAL_SetClkOutSel(SIM, kClockClkoutBusFlashClk);//BusFlashClk=24Mhz
  PORT_HAL_SetMuxMode(PORTF,7,kPortMuxAlt3);//PTF7 ALT3 for clockout
  
  VREF_Config();
  
  /* AFE interrupt installcallback */
  AFE_TEST_InstallCallback(0, 2, AFE_TEST_CallbackChn2 );
  AFE_TEST_InstallCallback(0, 3, AFE_TEST_CallbackChn3 );
  
  /* AFE channels init and config*/
  CLOCK_HAL_SetAfeClkSrc(SIM, kClockAfeSrcPllClk);//MCG PLL Clock selected
  AFE_Config();
  
  /* XBAR config */
  XBAR_Config();
  
  /* CMP channel 1 init  */
  CMP1_Config();
  
  /* Q_TIMER0 input capture mode to detect external frequency */
  QTMER0_Config();
  
  diagnostic_setup ();//For debug
  
  AFE_HAL_ChnSwTriggerCmd(AFE, CHN_TRIG_MASK(2) | CHN_TRIG_MASK(3));
  
#ifdef Use_FreeMaster
  FMSTR_Init();
#endif
  
  /* main loop  */
  while (1)
  {
    /* read phase voltage frequency */
    if (TMRCH0_ReadClrCaptReg(&tmp16) == true) 
    { 
      freq_tmr = TMR2FREQ(tmp16);
      double_to_int(freq_tmr, &sign_val, &int_val, & frac_val);
    }
#ifdef Use_FreeMaster    
    FMSTR_Poll();
#else
    PRINTF("value_ch2  = 0x%lx \r\n", value_ch2);
    PRINTF("value_ch3  = 0x%lx \r\n", value_ch3);
    PRINTF("freq_tmr  = %ld.%ld\r\n", int_val,frac_val);
    OSA_TimeDelay(500u);
#endif    
  }
  
  
}

/***************************************************************************//*!
* @brief   Selects PLL 32KHz reference clock and enables PLL module.
* @details This macro selects PLL 32KHz reference clock and enables PLL module.
*          PLL is enabled by setting PLLCLKEN0 bit.
* @param   clk   Select one of the @ref pll_src_list.  
* @note    Implemented as an inline macro.
* @warning FLL or PLL is enabled by this macro (LP bit is deasserted). The 
*          MCGPLLCLK is prevented from coming out of the MCG until it is
*          enabled and S[LOCK0] is set.
* @see     @ref PLL_Disable, @ref SIM_CtrlPLLVLP, @ref PLL_CtrlPLLSTEN 
******************************************************************************/
void PLL_Enable(void)
{
  CLOCK_HAL_SetLowPowerModeCmd(MCG, false);
  CLOCK_HAL_SetPllRefSel0Mode(MCG,kMcgPllRefClkSrcRtc);
  CLOCK_HAL_SetPll0EnableCmd(MCG, true);
  while(!CLOCK_HAL_IsPll0Locked(MCG)) {} // Wait until locked.
}

void VREF_Config(void)
{
  vref_user_config_t  vref_user_struct;
  /* VREF module initialized  */
  CLOCK_SYS_EnableVrefClock(0);  
  
  VREF_HAL_Init(VREF);
  vref_user_struct.chopOscEnable = true;
  vref_user_struct.trimValue = 46;
  vref_user_struct.regulatorEnable = true;
  vref_user_struct.soccEnable = true;
  vref_user_struct.bufferMode = kVrefModeHighPowerBuffer;
  VREF_HAL_Configure(VREF, &vref_user_struct);
  
  VREF_HAL_SetVoltageReference(VREF,kVrefReferenceInternal);
  VREF_HAL_SetLowReferenceTrimVal(VREF,3);
  VREF_HAL_SetLowReference(VREF, true);
  VREF_HAL_WaitVoltageStable(VREF);
  
  /* VREF module initialized in SIM module for AFE,SAR,CMP*/
  SIM_HAL_SetVrefBuffPowerDownCmd(SIM, true);//buffer enable
  CLOCK_HAL_SetVrefBuffInputSel(SIM,kSimVrefBuffInputSel0);//Internal Reference selected as Buffer Input
  SIM_HAL_SetVrefBuffOutCmd(SIM, true); // 1.2V out on PAD
}

void AFE_Config(void)
{
  afe_chn_set_t  afe_chn_struct;
  afe_converter_config_t afe_converter_struct;
  
  /* AFE module channels initialized  */
  CLOCK_SYS_EnableAfeClock(0);  
  AFE_HAL_Init(AFE);
  AFE_HAL_ChnInit(AFE, 2);
  AFE_HAL_ChnInit(AFE, 3);
  
  afe_chn_struct.hwTriggerEnable = false;
  afe_chn_struct.continuousConvEnable = true;
  afe_chn_struct.bypassEnable = false;
  afe_chn_struct.pgaGainSel = kAfePgaGainBy1;
  afe_chn_struct.pgaEnable = false;
  afe_chn_struct.decimOSR = kAfeDecimOsrOf64;
  afe_chn_struct.modulatorEnable = true;
  afe_chn_struct.decimFilterEnable = true;
  afe_chn_struct.decimNegedgeEnable = false;
  afe_chn_struct.externClockEnable = false;
  
  AFE_HAL_ConfigChn(AFE, 2, &afe_chn_struct);
  AFE_HAL_ConfigChn(AFE, 3, &afe_chn_struct);
  
  /*AFE interrupt enable */	
  AFE_HAL_SetIntEnableCmd(AFE, 2, true);
  AFE_HAL_SetIntEnableCmd(AFE, 3, true);
  INT_SYS_EnableIRQ(AFE_CH2_IRQn );
  INT_SYS_EnableIRQ(AFE_CH3_IRQn );
  
  /*Configure the AFE converter*/
  afe_converter_struct.startupCnt = ((uint8_t)((afe_freq/(1<<kAfeClkDividerInputOf2))*20e-6)) ;
  afe_converter_struct.resultFormat = kAfeResultFormatRight;
  afe_converter_struct.delayOk = true;
  afe_converter_struct.swReset = true;
  afe_converter_struct.lowPowerEnable = false;
  afe_converter_struct.swTriggChn0 = false;
  afe_converter_struct.swTriggChn1 = false;
  afe_converter_struct.swTriggChn2 = false;
  afe_converter_struct.swTriggChn3 = false;
  afe_converter_struct.masterEnable = true;
  
  AFE_HAL_ConfigConverter(AFE, &afe_converter_struct);
  AFE_HAL_SetClkDividerMode(AFE,kAfeClkDividerInputOf2);
  AFE_HAL_SetClkSourceMode(AFE,kAfeClkSrcClk1);//AFE_PLL_CLK
}

void AFE_TEST_CallbackChn2(void){
  value_ch2 = AFE_HAL_GetResult(AFE, 2);
}

void AFE_TEST_CallbackChn3(void){
  value_ch3 = AFE_HAL_GetResult(AFE, 3);
}
void diagnostic_setup (void)
{
#if 0//For debug
  PORT_HAL_SetMuxMode(PORTL,2,kPortMuxAlt2);//PTL2 ALT2 as XBAR_OUT10
  CLOCK_HAL_SetXbarOutSel(SIM, kClockXbarOutBusFlashClk);
  XBAR_DRV_ConfigSignalConnection(kXbaraInputClock_Output, kXbaraOutputXB_OUT10);//Clock Output to XBAR_OUT[10]
  PORT_HAL_SetMuxMode(PORTL,2,kPortMuxAlt2);//PTL2 ALT2 as XBAR_OUT10
  XBAR_DRV_ConfigSignalConnection(kXbaraInputCMP1_Output, kXbaraOutputXB_OUT10);//Clock Output to XBAR_OUT[10]
#endif
}

void CMP1_Config(void)
{
  cmp_comparator_config_t cmp_comparator_struct;
  cmp_dac_config_t cmp_dac_struct;
  cmp_sample_filter_config_t cmp_sample_filter_struct;
  CLOCK_SYS_EnableCmpClock(0);
  CMP_HAL_Init(CMP1);
  
  /*Configure the CMP's comparator function.*/
  cmp_comparator_struct.hystersisMode = kCmpHystersisOfLevel0;
  cmp_comparator_struct.pinoutEnable = false;
  cmp_comparator_struct.pinoutUnfilteredEnable = false;
  cmp_comparator_struct.invertEnable = false;
  cmp_comparator_struct.highSpeedEnable = true;
  cmp_comparator_struct.dmaEnable = false;
  cmp_comparator_struct.risingIntEnable = true;
  cmp_comparator_struct.fallingIntEnable = false;
  cmp_comparator_struct.plusChnMux = kCmpInputChn2;//IN2_SDADP2
  cmp_comparator_struct.minusChnMux = kCmpInputChn3;//IN3_SDADM2
  cmp_comparator_struct.triggerEnable = false;
  CMP_HAL_ConfigComparator(CMP1, &cmp_comparator_struct);
  
  /* Configure the CMP's DAC function.*/
  cmp_dac_struct.dacEnable = true;
  cmp_dac_struct.refVoltSrcMode = kCmpDacRefVoltSrcOf1;
  cmp_dac_struct.dacValue = 32;
  CMP_HAL_ConfigDacChn(CMP1, &cmp_dac_struct);
  
  /*Configure the CMP's sample or filter function.*/
  cmp_sample_filter_struct.workMode = kCmpSampleWithFilteredMode; //CR0[FILTER_CNT]>1;FPR[FILT_PER]>0
  cmp_sample_filter_struct.useExtSampleOrWindow = false;
  //cmp_sample_filter_struct.filterClkDiv = 0x40;//CMP_FPR_FILT_PER
  cmp_sample_filter_struct.filterClkDiv = 0xf0;//CMP_FPR_FILT_PER
  cmp_sample_filter_struct.filterCount = kCmpFilterCountSampleOf4;//CMP_CR0_FILTER_CNT
  CMP_HAL_ConfigSampleFilter(CMP1, &cmp_sample_filter_struct);
  
  /*Enable CMP1*/
  CMP_HAL_Enable(CMP1);
  
}

void QTMER0_Config(void)
{
  CLOCK_SYS_EnableQuadTmrClock(0);
  QUADTMR_HAL_Init(TMR0);
  
  /*FUNCTION**********************************************************************
  *
  * Function Name : QUADTMR_HAL_SetupInputCapture
  * Description   : Allows the user to count the source clock cycles until a capture event arrives.
  * The count will be stored in the capture register.
  *
  *END**************************************************************************/
  QUADTMR_HAL_SetupInputCapture(TMR0, 
                                kQuadTmrClkDiv16,
                                kQuadTmr0InpPin,
                                true,
                                kQuadTmrRisingEdge, 
                                false);
  
  QUADTMR_HAL_SetComp1Val(TMR0,0);
  QUADTMR_HAL_SetComp1PreloadVal(TMR0,0);
  QUADTMR_HAL_SetComp2Val(TMR0,0);
  QUADTMR_HAL_SetComp2PreloadVal(TMR0,0);
  QUADTMR_HAL_SetLoadVal(TMR0,0);
  
}

void XBAR_Config(void)
{
  xbar_state_t xbarStateStruct;
  /* Init the XBAR module*/
  XBAR_DRV_Init(&xbarStateStruct);
  xbar_control_config_t xbarConfigStruct;
  xbarConfigStruct.activeEdge = kXbarEdgeNone;
  xbarConfigStruct.intDmaReq = kXbarReqDis;
  XBAR_DRV_ConfigOutControl(0, &xbarConfigStruct);
  
  /* Configurate interperipheral signal connections. */       
  XBAR_DRV_ConfigSignalConnection(kXbaraInputCMP1_Output, kXbaraOutputTMR_CH0_SecInput);
  
}


static uint8_t TMRCH0_ReadClrCaptReg (uint16_t *ptr)
{
  if (TMR_BRD_SCTRL_IEF(TMR0))  
  { 
    *ptr = QUADTMR_HAL_GetCaptureVal(TMR0); 
    TMR_BWR_SCTRL_IEF(TMR0,0);  
    return  true; 
  }
  else                                                      
    return false;
}

static int32_t double_to_int(double real, uint32_t *sign_val, uint32_t *int_val, uint32_t *frag_val)
{
  if (real < 0)
  {
    *sign_val = 1;
    real = fabs(real);
  }
  
  *int_val = (uint32_t)real;
  real -= *int_val;
  real *= 1000;
  *frag_val = (uint32_t)real;
  
  return 0;
}



