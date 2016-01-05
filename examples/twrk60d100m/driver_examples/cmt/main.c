/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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
#include "fsl_cmt_driver.h"
#include "fsl_os_abstraction.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define DATA_BYTENUM               2  /*! The number of bytes to be transmitted */
#define CARRIERGENERATE_HIGHCOUNT  1  /*! The count of carrier and generator high time period */
#define CARRIERGENERATE_LOWCOUNT   1  /*! The count of carrier and generator low time period */
#define MODULATEBITONE_MARKCOUNT   9  /*! The count of modulator mark period for bit one */
#define MODULATEBITONE_SPACECOUNT  0  /*! The count of modulator space period for bit one */
#define MODULATEBITZERO_MARKCOUNT  0  /*! The count of modulator mark period for bit zero */
#define MODULATEBITZERO_SPACECOUNT 9  /*! The count of modulator space period for bit zero */

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

// Data for modulation.
uint8_t data[ DATA_BYTENUM ] = {0x15, 0x24};
// Data structure for modulator
cmt_modulate_data_t  modData = {
       .data = data,
       .len  = CMT_MODULATEUNIT8BITS * DATA_BYTENUM,
       .markOne = MODULATEBITONE_MARKCOUNT,
       .spaceOne = MODULATEBITONE_SPACECOUNT,
       .markZero = MODULATEBITZERO_MARKCOUNT,
       .spaceZero = MODULATEBITZERO_SPACECOUNT
   };

semaphore_t sema;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Simple example for CMT driver
 *          Measure the modulated data using oscilloscope on Jumper 2.
 */

int main(void)
{
    cmt_base_config_t cmtInit = {0};    // CMT Initialize structure
    cmt_modulate_config_t modulateConfig = {(cmt_mode_t)0, 0};  // CMT carrier generator and modulation configuration
    
    osa_status_t syncStatus;

    // Initialize hardware
    hardware_init();

    // Initialize OSA layer.
    OSA_Init();

    // Initialize CMT module
    cmtInit.isInterruptEnabled = true;
    cmtInit.isIroEnabled       = true;
    cmtInit.iroPolarity        = kCMTIROActiveHigh;
    cmtInit.cmtDivider         = kCMTSecClkDiv1;
    CMT_DRV_Init(0, &cmtInit); 

    PRINTF("\r\n CMT Initialize finished!\r\n"); 

    // Create a semaphore to notify the data modulation transaction is finished 
    OSA_SemaCreate(&sema, 0);

    // Initialize CMT Carrier and Modulation mode for data modulation
    modulateConfig.mode  = kCMTTimeMode;
    modulateConfig.highCount1   =   CARRIERGENERATE_HIGHCOUNT;
    modulateConfig.lowCount1    =   CARRIERGENERATE_LOWCOUNT;
    modulateConfig.markCount    =   MODULATEBITONE_MARKCOUNT;
    modulateConfig.spaceCount   =   MODULATEBITONE_SPACECOUNT;
    
    // Start data modulation with LSB mode for byte data transmit
    CMT_DRV_StartCarrierModulate(0,  &modulateConfig, &modData, false);

    PRINTF("\r\n Start carrier modulation ....\r\n");

    while (1){

        // Wait until transfer is complete
        do
        {
            syncStatus = OSA_SemaWait(&sema, OSA_WAIT_FOREVER);
        }while(syncStatus == kStatus_OSA_Idle);
        
        // Print the data finish
        PRINTF("\r\n %d bit data carrier modulation finished. Stop carrier modulation!\r\n", modData.lenDone);
        return 0;
    }
}
///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
