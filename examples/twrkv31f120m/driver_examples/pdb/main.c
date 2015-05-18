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
// SDK Included Files
#include "board.h"
#include "fsl_os_abstraction.h"
#include "fsl_pdb_driver.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define PDB_INSTANCE   (0)

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
volatile uint32_t pdbIntCounter = 0;

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief PDB normal timer.
 *
 * This function uses PDB to generate a constant period of time.
 * Each time, PDB expires, interrupt occurres.
 */
int main(void)
{
    pdb_timer_config_t PdbTimerConfig;
    bool pdbFinished = false;

    uint32_t currentCounter = 0;

    // Init hardware
    hardware_init();

    PRINTF("\r\nPDB example \r\n");

    PdbTimerConfig.loadValueMode = kPdbLoadValueImmediately;
    PdbTimerConfig.seqErrIntEnable = false;
    PdbTimerConfig.clkPreDiv = kPdbClkPreDivBy128;
    PdbTimerConfig.clkPreMultFactor = kPdbClkPreMultFactorAs40;
    PdbTimerConfig.triggerInput = kPdbSoftTrigger;
    PdbTimerConfig.continuousModeEnable = true;
    PdbTimerConfig.dmaEnable = false;
    PdbTimerConfig.intEnable = true;

    while(1)
    {
        // Clear counter
        currentCounter = 0;
        pdbIntCounter  = 0;
        pdbFinished    = false;

        // Initialize PDB instance.
        PDB_DRV_Init(PDB_INSTANCE, &PdbTimerConfig);

        PDB_DRV_SetTimerModulusValue(PDB_INSTANCE, 0xFFFU);
        PDB_DRV_SetValueForTimerInterrupt(PDB_INSTANCE, 0xFFFU);
        PDB_DRV_LoadValuesCmd(PDB_INSTANCE);

        // Trigger the PDB.
        PDB_DRV_SoftTriggerCmd(PDB_INSTANCE);

        while (!pdbFinished)
        {
            // Check if interrupt occurred.
            if (pdbIntCounter > currentCounter)
            {
                currentCounter = pdbIntCounter;
                // PDB counter go 10 time before stop.
                PRINTF("PDB ISR No.%d occured !\r\n", currentCounter);
                if (currentCounter == 10)
                {
                    pdbFinished = true;
                }
            }
        }

        // Deinitilize PDB instance
        PDB_DRV_Deinit(PDB_INSTANCE);

        PRINTF("PDB example finished \r\n");

        // Print a note.
        PRINTF(" Press any key to run example again \r\n");
        GETCHAR();
    }

}

/*******************************************************************************
 * EOF
 ******************************************************************************/

