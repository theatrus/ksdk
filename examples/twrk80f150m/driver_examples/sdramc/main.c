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
#include "fsl_device_registers.h"
#include "fsl_sdram_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include "fsl_sdramc_driver.h"
#include "fsl_lmem_cache_driver.h"
///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

#define SDRAM_DATATESTLEN              0x1000  // The length of test data 
#define SDRAM_DATAWRITETIMES           1000 // The data write test times
///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

// Data buffer for SDRAM read and write.
uint32_t sdram_writeBuffer[SDRAM_DATATESTLEN] = {0};
uint32_t sdram_readBuffer[SDRAM_DATATESTLEN] = {0};
////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief Simple example for SDRAM driver
 */

int main(void)
{
    uint32_t index;
    uint32_t datalen = SDRAM_DATATESTLEN;
    uint32_t *sdram = (uint32_t *)SDRAM_START_ADDRESS;   // SDRAM start address
    bool result = true;

    // Initialize hardware
    hardware_init();

    // Initialize OSA layer.
    OSA_Init();

    // Initialize SDRAM
    SDRAM_Init();
    PRINTF("\r\n SDRAM Example Start!\r\n");  
    PRINTF("\r\n SDRAM Memory Write Start, Start Address 0x%x, Data Length %d !\r\n", sdram, datalen); 
    // Prepare data and write to SDRAM
    for(index=0; index < datalen; index++)
    {
        sdram_writeBuffer[index] = index;
        *(uint32_t *)(sdram + index) = sdram_writeBuffer [index];    
    }   
    PRINTF("\r\n SDRAM Write finished!\r\n");  
    
    PRINTF("\r\n SDRAM Read Start, Start Address 0x%x, Data Length %d !\r\n", sdram, datalen); 
    /* Read data from the SDRAM. */
    for(index=0; index < datalen; index++)
    {
        sdram_readBuffer[index] = *(uint32_t *)(sdram + index);
    }
    PRINTF("\r\n SDRAM Read finished!\r\n"); 

    PRINTF("\r\n SDRAM Write Data and Read Data Compare Start!\r\n"); 
    /* Compare the two buffers. */
    while (datalen--)
    {
      if (sdram_writeBuffer[datalen] != sdram_readBuffer[datalen])
      {
         result = false;
         PRINTF("\r\n SDRAM Write Data and Read Data Check Error!\r\n"); 
         break;
      }
    }

#if  (defined TWR_K81F150M) || (defined TWR_K80F150M)
    uint32_t times, timeBefore, timeAfter, timeUseWb, timeUseWt, timeUseNo;
    // SDRAM Time Spend Compare with cache write-back, write-through and non-cacheable
    // SDRAM Cache write back
    LMEM_DRV_SystemCacheEnable(0);
    LMEM_DRV_SystemCacheInvalidateAll(0);
    timeBefore = OSA_TimeGetMsec();
    // Write SDRAM  SDRAM_DATATESTLEN length SDRAM_DATAWRITETIMES times
    for (times = 0; times < SDRAM_DATAWRITETIMES; times++)
    {
        for(index=0; index < SDRAM_DATATESTLEN; index++)
        {
            *(uint32_t *)(sdram + index) = 0xFFFF;    
        }  
    }
    timeAfter = OSA_TimeGetMsec();
    timeUseWb =  timeAfter - timeBefore;

    // SDRAM Cache write through
    LMEM_DRV_SystemCacheDemoteRegion(0, kCacheRegion7, kCacheWriteThrough);
    timeBefore = OSA_TimeGetMsec();
    // Write SDRAM  SDRAM_DATATESTLEN length SDRAM_DATAWRITETIMES times
    for (times = 0; times < SDRAM_DATAWRITETIMES; times++)
    {
        for(index=0; index < SDRAM_DATATESTLEN; index++)
        {
            *(uint32_t *)(sdram + index) = 0xFFFF;    
        }  
    }
    timeAfter = OSA_TimeGetMsec();
    timeUseWt =  timeAfter - timeBefore;

     // SDRAM Non-Cacheable
    LMEM_DRV_SystemCacheDemoteRegion(0, kCacheRegion7, kCacheNonCacheable);
    timeBefore = OSA_TimeGetMsec();
    // Write SDRAM  SDRAM_DATATESTLEN length SDRAM_DATAWRITETIMES times
    for (times = 0; times < SDRAM_DATAWRITETIMES; times++)
    {
        for(index=0; index < SDRAM_DATATESTLEN; index++)
        {
            *(uint32_t *)(sdram + index) = 0xFFFF;    
        }  
    }
    timeAfter = OSA_TimeGetMsec();
    timeUseNo =  timeAfter - timeBefore;
    LMEM_DRV_SystemCacheDisable(0); 

    // For K80 serial board debug console, the sdramc pin is conflict with the uart pin
    // so configure uart pin to enable the log print at the end of the example
    dbg_uart_init();

    PRINTF("\r\n SDRAM Spend %d milliseconds with Cache Write back.\r\n", timeUseWb);
    PRINTF("\r\n SDRAM Spend %d milliseconds with Cache Write through.\r\n", timeUseWt);
    PRINTF("\r\n SDRAM Spend %d milliseconds with no-cacheable.\r\n", timeUseNo);
#endif 
    if(result)
    {
      PRINTF("\r\n SDRAM Write Data and Read Data Succeed.\r\n");    
    }
    else
    {
      PRINTF("\r\n SDRAM Write Data and Read Data Failed.\r\n"); 
    }
    // Print finished message
    PRINTF("\r\n SDRAM Example End.\r\n");
    while (1);
}
///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
