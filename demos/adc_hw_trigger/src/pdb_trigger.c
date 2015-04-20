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

// Standard C Included Files
#include <stdio.h>
// SDK Included Files
#include "adc_hw_trigger.h"
#include "fsl_pdb_driver.h"
#include "fsl_clock_manager.h"

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

extern const uint32_t gSimBaseAddr[]; // driver state store

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @Brief PDB0 IRQ Handler to handle the seq error if any
 */
void PDB0_IRQHandler(void)
{
    // clear the IF
    PDB_DRV_ClearPdbCounterIntFlag(0);
    // clear the EIE
    if (PDB_DRV_GetAdcPreTriggerFlag(0, 0, 0, kPdbAdcPreChnErrFlag))
    {
        PDB_DRV_ClearAdcPreTriggerFlag(0, 0, 0, kPdbAdcPreChnErrFlag);
        printf("Pre-trigger A seq error\r\n");
    }
    if (PDB_DRV_GetAdcPreTriggerFlag(0, 0, 1, kPdbAdcPreChnErrFlag))
    {
        PDB_DRV_ClearAdcPreTriggerFlag(0, 0, 1, kPdbAdcPreChnErrFlag);
        printf("Pre-trigger B seq error\r\n");
    }
}

/*!
 * @Brief enable the trigger source
 */
void init_trigger_source(uint32_t adcInstance)
{
    pdb_adc_pre_trigger_config_t pdbAdcTriggerConfig;
    pdb_user_config_t pdbUserConfig;
    pdb_clk_prescaler_div_mode_t divMode = kPdbClkPreDivBy64;
    uint32_t busClock, modValue;
    uint8_t preDivider = 1 << divMode;

    // get the bus clock freq which for PDB
    CLOCK_SYS_GetFreq(kBusClock, &busClock);

    // calculate for MOD value
    modValue = INPUT_SIGNAL_FREQ*NR_SAMPLES;
    modValue = busClock/preDivider/modValue*4;

    // init the pdb user config structure for
    // enable SW trigger, enable continuous mode,
    // set the correct MOD and DLY
    // Enable the sequency error interrupt.
    PDB_DRV_StructInitUserConfigForSoftTrigger(&pdbUserConfig);
    pdbUserConfig.continuousModeEnable = true;
    pdbUserConfig.pdbModulusValue = modValue;
    pdbUserConfig.intEnable = true;
    pdbUserConfig.seqErrIntEnable = true;
    pdbUserConfig.clkPrescalerDivMode = divMode;
    pdbUserConfig.multFactorMode = kPdbMultFactorAs1;

    // Initialize PDB driver
    PDB_DRV_Init(0, &pdbUserConfig);

    // Configure the PDB channel for ADC_adcInstance
    // disable BACK to BACK mode
    pdbAdcTriggerConfig.backToBackModeEnable = false;

    // enable pretrigger out
    pdbAdcTriggerConfig.preTriggerOutEnable = true;
    pdbAdcTriggerConfig.delayValue = modValue/4;

    // Configure the pre trigger A for ADC
    PDB_DRV_EnableAdcPreTrigger(0, adcInstance, 0, &pdbAdcTriggerConfig);

    // the Pre-trigger A delay is set to 1/4 MOD,
    // the Pre-trigger B delay is set to 3/4 MOD,
    // so the trigger interval between A/B, B/A is same.
    pdbAdcTriggerConfig.delayValue = modValue*3/4;

    // Configure the pre trigger B for ADC
    PDB_DRV_EnableAdcPreTrigger(0, adcInstance, 1, &pdbAdcTriggerConfig);

    // Configure SIM for ADC hw trigger source PDB
    SIM_HAL_SetAdcAlternativeTriggerCmd(gSimBaseAddr[0], adcInstance, false);

    // Trigger the PDB, let it go in continuous mode
    PDB_DRV_SoftTriggerCmd(0);
}

/*!
 * @Brief disable the trigger source
 */
void deinit_trigger_source(uint32_t adcInstance)
{
    PDB_DRV_DisableAdcPreTrigger(0, adcInstance, 0);
    PDB_DRV_DisableAdcPreTrigger(0, adcInstance, 1);
    PDB_DRV_Deinit(0);
}
