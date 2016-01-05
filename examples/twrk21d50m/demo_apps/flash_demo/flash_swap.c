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

/* include the header files */

#include <stdio.h>
#include "flash_demo.h"
#include "fsl_debug_console.h"
#if (defined(SWAP_M))
extern FLASH_SSD_CONFIG flashSSDConfig;
extern pFLASHCOMMANDSEQUENCE g_FlashLaunchCommand;

#if (PGM_SIZE_BYTE == 8)
    uint8_t unsecure_key[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF};
#else
    uint8_t unsecure_key[4] = {0xFE, 0xFF, 0xFF, 0xFF};
#endif
uint8_t pgmData[8] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

static uint32_t ProgramApplicationCode(void);

bool SwapCallback(uint8_t currentSwapMode);

/**************************************************************************
    Flash Swap Example Code
IMPORTANT NOTE:
    This demo will operate on flash region from address PSWAP_INDICATOR_ADDR to
    to the end of PFlash. So please do NOT put any program and data in the region
 **************************************************************************/
uint32_t flash_swap(void)
{
    uint32_t ret;
    uint8_t currentSwapMode, currentSwapBlock, nextSwapBlock;

    /* If standalone, FlashInit() is required here */
    /**************************************************************************
    *                          Get current swap state                         *
    ***************************************************************************/
    /* Important note:  Use g_FlashLaunchCommand instead of FlashCommandSequence to run CCIF from SRAM!  */
    /*                  This allows swap application to execute from Flash memory space while CCIF flag is written from SRAM! */
    ret = PFlashSwapCtl(&flashSSDConfig, PSWAP_INDICATOR_ADDR, FTFx_SWAP_REPORT_STATUS, &currentSwapMode, \
                    &currentSwapBlock, &nextSwapBlock, g_FlashLaunchCommand);

    if (FTFx_OK != ret)
    {
        return ret;
    }

    if (currentSwapMode == FTFx_SWAP_UNINIT)
    {
        /* Erase the sectors that containning swap indicator in the upper Pflash halves to make sure them all 0xFF */
        ret = FlashEraseSector(&flashSSDConfig,PSWAP_INDICATOR_ADDR + P_FLASH_SIZE/2, P_SECTOR_SIZE,g_FlashLaunchCommand);
        ret |= FlashEraseSector(&flashSSDConfig,PSWAP_INDICATOR_ADDR, P_SECTOR_SIZE,g_FlashLaunchCommand);

        if (FTFx_OK != ret)
        {
            return ret;
        }
        /**************************************************************************
        *      Call PFlashSwapCtl() to stop at each immediate state                *
        ***************************************************************************/

        /* after this call, swap state = UPDATE_ERS */
        ret = PFlashSwapCtl(&flashSSDConfig, PSWAP_INDICATOR_ADDR, FTFx_SWAP_SET_INDICATOR_ADDR, &currentSwapMode, &currentSwapBlock, &nextSwapBlock, g_FlashLaunchCommand);
        if ((FTFx_OK != ret)||(currentSwapMode!= FTFx_SWAP_UPDATE_ERASED))
        {
            return ret;
        }

        /* Erase non active swap indicator and program test data to lower/upper half */
        ret = ProgramApplicationCode();
        if (FTFx_OK != ret)
        {
            return ret;
        }

        /* Progress to swap complete mode now that our indicator address is set, and we have programmed some data */
        ret = PFlashSwapCtl(&flashSSDConfig, PSWAP_INDICATOR_ADDR, FTFx_SWAP_SET_IN_COMPLETE, &currentSwapMode, \
                        &currentSwapBlock, &nextSwapBlock, g_FlashLaunchCommand);

        if ((FTFx_OK != ret)||(currentSwapMode!= FTFx_SWAP_COMPLETE))
        {
            return ret;
        }

        /************************************************************************************/
        /* Need to reset here to complete swap process                                       */
        /************************************************************************************/
    }
    else /* swap system is in READY state */
    {
        /**************************************************************************
        *      Call PFlashSwap()                                                   *
        *       Swap is already initialized, meaning the swap indicator locations  *
        *       have been assigned and swap is ready to be executed                *
        ***************************************************************************/
        ret = PFlashSwap(&flashSSDConfig, PSWAP_INDICATOR_ADDR, SwapCallback, g_FlashLaunchCommand);
        if (FTFx_OK != ret)
        {
            return ret;
        }
        ret = PFlashSwapCtl(&flashSSDConfig, PSWAP_INDICATOR_ADDR, FTFx_SWAP_REPORT_STATUS, &currentSwapMode, \
                        &currentSwapBlock, &nextSwapBlock, g_FlashLaunchCommand);
        if ((FTFx_OK != ret)||(currentSwapMode != FTFx_SWAP_COMPLETE))
        {
            return ret;
        }
    }

    /*********************************************************************************************************/
    return(FTFx_OK);
}

/*******************************************************************************************/
/* This is setting up a small amount of data in upper block and lower block to */
/* show example of how the data gets mapped after a reset following a Swap.  */
/* When Swap is complete and the part is reset, the memory map of the first P-Flash half */
/* and the second P-Flash half are swapped */
/* For example, Data that resides at 0x7F000 will be put at 0xFF000, vice versa */
/*******************************************************************************************/
static uint32_t ProgramApplicationCode(void)
{
    uint32_t ret = FTFx_OK;
    uint32_t dest;
    uint32_t size;
    uint8_t i;

    /* Erase upper half of Pflash */
    for (i = P_BLOCK_NUM/2 ; i < P_BLOCK_NUM; i++)
    {
        dest = P_FLASH_BASE + i*(P_FLASH_SIZE/P_BLOCK_NUM);
        ret |= FlashEraseBlock(&flashSSDConfig, dest, g_FlashLaunchCommand);
        if (FTFx_OK != ret)
        {
            PRINTF("\r\nFlash Erase Upper Block Error, Address: 0x%x", (int)dest);
            return ret;
        }
    }

    /*******************************************************************
        Program upper block with exact same data from lower half except
        the last sector ( for swap inficator).
        This includes security information, for use when we swap blocks
    *******************************************************************/
    dest = P_FLASH_BASE + P_FLASH_SIZE/2;
    size = P_FLASH_SIZE/2 - 2*P_SECTOR_SIZE;
    ret = FlashProgram(&flashSSDConfig, dest, size, P_FLASH_BASE, g_FlashLaunchCommand);
    if (FTFx_OK != ret)
    {
      PRINTF("\r\nFlashProgram Error, Pflash half program, Address: 0x%x", (int)dest);
      return ret;
    }
    /* program data to upper Pflash half for verification */
    ret |= FlashProgram(&flashSSDConfig, PSWAP_UPPERDATA_ADDR, PGM_SIZE_BYTE, pgmData, g_FlashLaunchCommand);
    /* Program data to lower Pflash half*/
    for (i = 0; i < PGM_SIZE_BYTE; i ++)
    {
        pgmData[i] = (i+0x10);
    }
    ret |= FlashEraseSector(&flashSSDConfig,PSWAP_LOWERDATA_ADDR, P_SECTOR_SIZE,g_FlashLaunchCommand);
    ret |= FlashProgram(&flashSSDConfig, PSWAP_LOWERDATA_ADDR, PGM_SIZE_BYTE, pgmData, g_FlashLaunchCommand);
    return (ret);
}
/******************************************************************************/
/* Function: SwapCallback() */
/******************************************************************************/
bool SwapCallback(uint8_t currentSwapMode)
{
    uint32_t destination, ret = FTFx_OK;

    switch (currentSwapMode)
    {
        case FTFx_SWAP_UNINIT:
            /* Put your application-specific code here */
            PRINTF("\r\n\tSwap Callback -> Swap flash is uninitialization status!");
            break;

        case FTFx_SWAP_READY:
            /* Put your application-specific code here */
            PRINTF("\r\n\tSwap Callback -> Swap flash is initialization status!");
            break;

        case FTFx_SWAP_UPDATE:
            /* Put your application-specific code here */
            PRINTF("\r\n\tSwap Callback -> Swap flash is update status!");
            /**************************************************************************/
            /* erase swap indicator address so we can move to update-erased state */
            /*************************************************************************/
            destination = PSWAP_INDICATOR_ADDR + P_FLASH_SIZE/2;
            ret = FlashEraseSector(&flashSSDConfig, destination, FTFx_PSECTOR_SIZE, g_FlashLaunchCommand);
            if (FTFx_OK != ret)
            {
                return ret;
            }
            break;

        case FTFx_SWAP_UPDATE_ERASED:
            /* Put your application-specific code here */
            PRINTF("\r\n\tSwap Callback -> swap flash is update erased status!");

            /* Erase non active swap indicator and Program example application code to lower and upper location */
            /* In a typical user mode, it may be desired to only update the upper half, then swap to run the new code */
            /* The flash block will be swapped after the next reset */
            ret = ProgramApplicationCode();
            break;

        case FTFx_SWAP_COMPLETE:
            /* Put your application-specific code here */
            PRINTF("\r\n\tSwap Callback -> swap flash is complete status!");
            break;
        default:
            break;
    }
    /* Return TRUE to continue swapping and FALSE to stop swapping */
    if (ret == FTFx_OK) return TRUE;
    else return FALSE;
}

/******************************************************************************/
/* Function: print_swap_application_data() */
/******************************************************************************/
/* Two locations are programmed with application data to show how the memory locations get swapped */
void print_swap_application_data()
{
    if (SWAP_STATUS_BIT)
    {
        /* Swap status bit is set, upper half mapped to 0x0000 - printout */

        PRINTF("\r\n\r\n\t++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        PRINTF("\r\n\tSwap Status Bit: 1, UPPER half resides at location 0x0000_0000!");
        PRINTF("\r\n\t++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    }
    else
    {
        /* Swap status bit is clear, lower half mapped to 0x0000 - printout  */
        PRINTF("\r\n\r\n\t---------------------------------------------------------------");
        PRINTF("\r\n\tSwap Status Bit: 0, LOWER half resides at location 0x0000_0000!");
        PRINTF("\r\n\t---------------------------------------------------------------");
    }
    PRINTF("\r\n\t\tP-Flash Lower Test Data @(0x%x): 0x%x", PSWAP_LOWERDATA_ADDR, (unsigned int)READ32(PSWAP_LOWERDATA_ADDR));
    PRINTF("\r\n\t\tP-Flash Upper Test Data @(0x%x): 0x%x", PSWAP_UPPERDATA_ADDR, (unsigned int)READ32(PSWAP_UPPERDATA_ADDR));
}

#endif /* #if (defined(SWAP)) */

/**************************************************************************
    EOF
 ***************************************************************************/
