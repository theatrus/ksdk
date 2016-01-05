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
#include <string.h>
#include <stdio.h>
#include "flash_demo.h"
#include "fsl_debug_console.h"


/********************************************************/
/*      Global Variables                                */
/********************************************************/
uint8_t DataArray[PGM_SIZE_BYTE];
uint8_t program_buffer[BUFFER_SIZE_BYTE];
uint32_t gCallBackCnt; /* global counter in callback(). */
pFLASHCOMMANDSEQUENCE g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)0xFFFFFFFF;

/* array to copy __Launch_Command func to RAM */
uint16_t __ram_func[LAUNCH_CMD_SIZE/2];
uint16_t __ram_for_callback[CALLBACK_SIZE/2]; /* length of this array depends on total size of the functions need to be copied to RAM*/

/************************************************************************************************/
/************************************************************************************************/
/*                      Flash Standard Software Driver Structure                                */
/************************************************************************************************/
/************************************************************************************************/
FLASH_SSD_CONFIG flashSSDConfig =
{
    FTFx_REG_BASE,          /* FTFx control register base */
    P_FLASH_BASE,           /* base address of PFlash block */
    P_FLASH_SIZE,           /* size of PFlash block */
    FLEXNVM_BASE,           /* base address of DFlash block */
    0,                      /* size of DFlash block */
    EERAM_BASE,             /* base address of EERAM block */
    0,                      /* size of EEE block */
    DEBUGENABLE,            /* background debug mode enable bit */
    NULL_CALLBACK           /* pointer to callback function */
};

/*********************************************************************
*
*  Function Name    : main
*  Description      : Main function
*                     Use Standard Software Drivers (SSD) to modify flash
*
*  Arguments        : void
*  Return Value     : UNIT32
*
**********************************************************************/
int main(void)
{
    uint32_t ret;                  /* Return code from each SSD function */
    uint32_t destination;          /* Address of the target location */
    uint32_t size;
    uint32_t end;
    uint8_t  securityStatus;       /* Return protection status */
    uint16_t number;               /* Number of longword or phrase to be program or verify*/
    uint32_t *p_data;
    uint32_t margin_read_level;    /* 0=normal, 1=user - margin read for reading 1's */

#if (!defined(SWAP_M))
    uint32_t i, FailAddr;
#endif
    gCallBackCnt = 0;

    CACHE_DISABLE

    /* initialize device ports and pins */
    hardware_init();

    /**************************************************************************
    *                               FlashInit()                               *
    * Setup flash SSD structure for device and initialize variables           *
    ***************************************************************************/
    ret = FlashInit(&flashSSDConfig);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }

    /****************************************/
    /* print welcome message for flash demo */
    /****************************************/
    PRINTF("\r\n\r\n\r\n*****************************************************************");
    PRINTF("\r\n*\t\tWelcome to the Flash Demo!");
    PRINTF("\r\n*");
    PRINTF("\r\n*  This demo will erase and program different regions of ");
    PRINTF("\r\n*  flash memory, and perform flash swap if it is supported. ");
    PRINTF("\r\n*");

    /***************************************************************/
    /* Print flash information - PFlash, DFlash, EEE if they exist */
    /***************************************************************/
    PRINTF("\r\n*\tFlash Information: \r\n-----------------------------------------------------------------");
    PRINTF("\r\n*\tTotal Flash Size:\t%d KB, Hex: (0x%x)", (P_FLASH_SIZE/ONE_KB), P_FLASH_SIZE);
    PRINTF("\r\n*\tFlash Sector Size:\t%d KB, Hex: (0x%x)", (FTFx_PSECTOR_SIZE/ONE_KB), FTFx_PSECTOR_SIZE);
    PRINTF("\r\n*");

    /*************************************/
    /* Does DFlash exist on this device? */
    /*************************************/
    if (flashSSDConfig.DFlashSize)
    {
        PRINTF("\r\n*\tData Flash Size:\t%d KB,\tHex: (0x%x)", (int)(flashSSDConfig.DFlashSize/ONE_KB), (unsigned int)flashSSDConfig.DFlashSize);
        PRINTF("\r\n*\tData Flash Base Address:\t0x%x", (unsigned int)flashSSDConfig.DFlashBase);
    }
    else
    {
      PRINTF("\r\n*\tNo D-Flash (FlexNVM) Present on this Device...");
    }

    /******************************************/
    /* Does FlexMemory Exist on this device ? */
    /******************************************/
    if (flashSSDConfig.EEESize)
    {
        PRINTF("\r\n*\tEnhanced EEPROM (EEE) Block Size:\t%d KB,\tHex: (0x%x)", (int)(flashSSDConfig.EEESize/ONE_KB), (unsigned int)flashSSDConfig.EEESize);
        PRINTF("\r\n*\tEnhanced EEPROM (EEE) Base Address:\t0x%x", (unsigned int)flashSSDConfig.EERAMBase);
    }
    else
    {
      PRINTF("\r\n*\tNo Enhanced EEPROM (EEE) Present on this Device...");
    }

    /**************************************/
    /* Is Swap Supported on this device ? */
    /**************************************/
#if (defined(SWAP_M))

    PRINTF("\r\n*\tSwap is Supported on this Device...");

#else

    PRINTF("\r\n*\tSwap is NOT Supported on this Device...");

#endif

    PRINTF("\r\n*****************************************************************");

    PRINTF("\r\n\r\n....................Now Running Demo.............................\r\n");

    /*********************************************/
    /* END: print welcome message for flash demo */
    /*********************************************/

    /**************************************************************************
      * Set CallBack to callback function
    ***************************************************************************/
    flashSSDConfig.CallBack = (PCALLBACK)RelocateFunction((uint32_t)__ram_for_callback , CALLBACK_SIZE , (uint32_t)callback);
    g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)RelocateFunction((uint32_t)__ram_func , LAUNCH_CMD_SIZE ,(uint32_t)FlashCommandSequence);

    /**************************************************************************
    *       Erase only select areas because we are running from Flash
    ***************************************************************************/

    /**************************************************************************
    ***************************************************************************
    *       Demo:   FlashEraseSector()  and FlashVerifySection()              *
    ***************************************************************************
    ***************************************************************************/
    /* Debug message for user */
    PRINTF("\r\n\r\n---->Demo: Running FlashEraseSector() and FlashVerifySection()...");

    /************************************************************************/
    /* Erase several sectors on upper pflash block where there is no code */
    /************************************************************************/
    destination = flashSSDConfig.PFlashBase + (flashSSDConfig.PFlashSize - 6*FTFx_PSECTOR_SIZE);
    end = destination + 3*FTFx_PSECTOR_SIZE;    /* erase and program two sectors */
    while ((destination + (FTFx_PSECTOR_SIZE)) < end)
    {
        size = FTFx_PSECTOR_SIZE;
        ret = FlashEraseSector(&flashSSDConfig, destination, size, g_FlashLaunchCommand);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }

        /* Verify section for several sector of PFLASH */
        number = FTFx_PSECTOR_SIZE/FSL_FEATURE_FLASH_PFLASH_SECTION_CMD_ADDRESS_ALIGMENT;
        for(margin_read_level = 0; margin_read_level < 0x2; margin_read_level++)
        {
            ret = FlashVerifySection(&flashSSDConfig, destination, number, margin_read_level, g_FlashLaunchCommand);
            if (FTFx_OK != ret)
            {
                ErrorTrap(ret);
            }
        }

        /* print message for user */
        PRINTF("\r\n\tDemo:  Successfully Erased Sector 0x%x -> 0x%x", (unsigned int)destination, (unsigned int)(destination+size));

        destination += (size);
    }

    /**************************************************************************
    *                          FlashGetSecurityState()                        *
    ***************************************************************************/
    securityStatus = 0x0;
    ret = FlashGetSecurityState(&flashSSDConfig, &securityStatus);

    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }

    /**************************************************
        Message to user on flash security state
        #define FLASH_NOT_SECURE                   0x01
        #define FLASH_SECURE_BACKDOOR_ENABLED      0x02
        #define FLASH_SECURE_BACKDOOR_DISABLED     0x04
    ****************************************************/
    switch(securityStatus)
    {
      case FLASH_NOT_SECURE:
      default:
          PRINTF("\r\n\r\n---->Flash is UNSECURE!");
          break;
      case FLASH_SECURE_BACKDOOR_ENABLED:
          PRINTF("\r\n\r\n---->Flash is SECURE, BACKDOOR is ENABLED!");
          break;
      case FLASH_SECURE_BACKDOOR_DISABLED:
            PRINTF("\r\n\r\n---->Flash is SECURE, BACKDOOR is DISABLED!");
            break;
    }

    /**************************************************************************
     *                          FlashReadResource()                            *
     ***************************************************************************/
     /* Read on P-Flash */
     destination = flashSSDConfig.PFlashBase + PFLASH_IFR; /* Start address of Program Once Field */
     ret = FlashReadResource(&flashSSDConfig, destination, DataArray, 0x0, g_FlashLaunchCommand);

     if (FTFx_OK != ret)
     {
         ErrorTrap(ret);
     }

      /* Message to user */
      p_data = (uint32_t *)&DataArray;
      PRINTF("\r\n\r\n---->Reading flash IFR @ location 0x%x: 0x%x", (unsigned int)destination, (unsigned int)(*p_data));

#if (defined(SWAP_M))

    /* program application data if we have not yet initialized the device */
      /************************************************************************
      *
      *
      *       Copy (program) Lower block to Upper block so we can swap
      *
      *       Example:
      *           K64F:   Swap is supported
      *                   Lower block: 0x0000 -> 0x8000
      *                   Upper block: 0x8000 -> 0x1_0000
      *           K22F:   Swap is NOT supported
      *
      *       Details:
      *
      *     Swap allows either half of program flash to exist at relative
      *     address 0x0000.  So, different applicaton code can run out of reset,
      *     following a swap.  The Swap command must run from SRAM to avoid
      *     read-while-write errors when running from Flash.
      *     The application can still run from Flash, but launching the
      *     command (clearing CCIF bit) must be executed from SRAM.
      *
      */
      /************************************************************************/
    /* Message to user */
    PRINTF("\r\n\r\n................ Swapping Flash Blocks! ..........................\r\n");
    PRINTF("\r\n\r\n---->Application after the last reset...");
    print_swap_application_data();
    /* Run Swap */
    ret = flash_swap();
    if (FTFx_OK == ret)
    {
    PRINTF("\r\n\r\n---->Flash Swap Demo Success!<----");
    print_swap_application_data();
    PRINTF("\r\n\r\n---->Application data will swap locations after next reset...");
    }
    else
    {
          PRINTF("\r\n\r\n....Flash Swap Demo Failed!  Check hardware and/or software!....");
          ErrorTrap(ret);
    }
#else  /* defined(SWAP_M) */

    /********************************************************************
    *   For devices without SWAP, program some data for demo purposes
    *********************************************************************/
    destination = flashSSDConfig.PFlashBase + (flashSSDConfig.PFlashSize - 6*FTFx_PSECTOR_SIZE);
    end = flashSSDConfig.PFlashBase + (flashSSDConfig.PFlashSize - 4*FTFx_PSECTOR_SIZE);
    for (i = 0; i < BUFFER_SIZE_BYTE; i++)
    {
        /* Set source buffer */
        program_buffer[i] = i;
    }
    size = BUFFER_SIZE_BYTE;

    /* message for user */
    PRINTF("\r\n\r\n---->Running FlashProgram() and FlashProgramCheck()...");

    while ((destination + (size)) < end)
    {
        ret = FlashProgram(&flashSSDConfig, destination, size, \
                                       program_buffer, g_FlashLaunchCommand);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }

        /* Program Check user margin levels*/
        for (margin_read_level = 1; margin_read_level < 0x2; margin_read_level++)
        {
            ret = FlashProgramCheck(&flashSSDConfig, destination, size, program_buffer, \
                                        &FailAddr, margin_read_level, g_FlashLaunchCommand);
            if (FTFx_OK != ret)
            {
                ErrorTrap(ret);
            }
        }

        PRINTF("\r\n\tSuccessfully Programmed and Verified Location 0x%x -> 0x%x", (unsigned int)destination, (unsigned int)(destination + size));

        destination += (BUFFER_SIZE_BYTE);
    }

#endif /* defined(SWAP) */

    /* Message to user */
    PRINTF("\r\n\r\n\r\n*****************************************************************");
    PRINTF("\r\n            Flash Demo Complete!            ");
    PRINTF("\r\n*****************************************************************");

    while(1);
}

/*********************************************************************
*
*  Function Name    : ErrorTrap
*  Description      : Gets called when an error occurs.
*  Arguments        : uint32_t
*  Return Value     :
*
*********************************************************************/
void ErrorTrap(uint32_t ret)
{
    PRINTF("\r\n\r\n\r\n\t---- HALTED DUE TO FLASH ERROR! ----");
    while (1)
    {
        ;
    }
}

/*********************************************************************
*
*  Function Name    : callback
*  Description      : callback function for flash operations
*  Arguments        : none
*  Return Value     :
*
*********************************************************************/
void callback(void)
{
 /* Should not use global variable for functions which need to be
  * relocated such as callback function. If used, some compiler
  * such as KEIL will be failed in all flash functions and
  * the bus error will be triggered.
  */
}

/*******/
/* EOF */
/*******/
