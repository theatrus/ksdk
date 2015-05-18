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
#include "user_cfg.h"
#include "flash_demo.h"
#include "fsl_debug_console.h"
#include "fsl_uart_driver.h"
#include <string.h>
#include <stdio.h>

FLASH_SSD_CONFIG flashSSDConfig =
{
    FTFx_REG_BASE,          /* FTFx control register base */
    PFLASH_BLOCK_BASE,      /* base address of PFlash block */
    PBLOCK_SIZE,            /* size of PFlash block */
    DEFLASH_BLOCK_BASE,     /* base address of DFlash block */
    0,                      /* size of DFlash block */
    EERAM_BLOCK_BASE,       /* base address of EERAM block */
    0,                      /* size of EEE block */
    DEBUGENABLE,            /* background debug mode enable bit */
    NULL_CALLBACK           /* pointer to callback function */
};

UINT8 DataArray[PGM_SIZE_BYTE];
UINT8 buffer[BUFFER_SIZE_BYTE];
UINT32 gCallBackCnt; /* global counter in callback(). */
void callback(void);

/*********************************************************************
*
*  Function Name    : main
*  Description      : Main function
*
*  Arguments        : void
*  Return Value     : UNIT32
*
**********************************************************************/
int main(void)
{
    UINT32 ret;          /* Return code from each SSD function */
    UINT32 destination;         /* Address of the target location */
    UINT32 size;
    UINT8  securityStatus;      /* Return protection status */    
    UINT32 FailAddr;
    UINT16 number;      		/* Number of longword or phrase to be program or verify*/
    UINT32 i;
    UINT32 margin_read_level;   /* 0=Normal, 1=User - Margin read levels for reading 1's */
    UINT32 *p_data;
#if (DEBLOCK_SIZE != 0)
    UINT8  protectStatus;           /* Store Protection Status Value of DFLASH or EEPROM */
#endif 
	
#if (ENDIANNESS == LITTLE_ENDIAN)
	#if (PGM_SIZE_BYTE == 4)
	UINT8 unsecure_key[PGM_SIZE_BYTE] = {0xFE, 0xFF, 0xFF, 0xFF};
	#else
	UINT8 unsecure_key[PGM_SIZE_BYTE] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF};
	#endif
#else /* Big Endian */
	UINT8 unsecure_key[PGM_SIZE_BYTE] = {0xFF, 0xFF, 0xFF, 0xFE};
#endif	
    
    /* Initialize callback variable to 0 */
    gCallBackCnt = 0;
    
    /* Disable cache */   
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
    PRINTF("\n\n\n\r*****************************************************************");
    PRINTF("\n\r*\t\tWelcome to the Flash Demo!");
    PRINTF("\n\r*");
    PRINTF("\n\r*  This demo will erase and program different regions of ");
    PRINTF("\n\r*  flash memory, demonstrating flash driver functionality. ");    
    PRINTF("\n\r*");
#if (defined(FLASH_TARGET))
    PRINTF("\n\r*\t- This demo is running from Flash Memory Space -");
#else
    PRINTF("\n\r*\t- This demo is running from SRAM Memory Space -");
#endif
    PRINTF("\n\r*");
        
    /***************************************************************/
    /* Print flash information - PFlash, DFlash, EEE if they exist */
    /***************************************************************/
    PRINTF("\n\r*\tFlash Information: \n\r-----------------------------------------------------------------");
    PRINTF("\n\r*\tTotal Flash Size:\t%d KB, Hex: (0x%x)", (PBLOCK_SIZE/ONE_KB), PBLOCK_SIZE);
    PRINTF("\n\r*\tFlash Sector Size:\t%d KB, Hex: (0x%x)", (FTFx_PSECTOR_SIZE/ONE_KB), FTFx_PSECTOR_SIZE);
    PRINTF("\n\r*");
    
    /*************************************/
    /* Does DFlash exist on this device? */
    /*************************************/
    if (flashSSDConfig.DFlashBlockSize) 
    {   
      PRINTF("\n\r*\tData Flash Size:\t%d KB,\tHex: (0x%x)", (flashSSDConfig.DFlashBlockSize/ONE_KB), flashSSDConfig.DFlashBlockSize); 
      PRINTF("\n\r*\tData Flash Base Address:\t0x%x", flashSSDConfig.DFlashBlockBase);
    }
    else
    {
      PRINTF("\n\r*\tNo D-Flash (FlexNVM) Present on this Device..."); 
    }

    /******************************************/
    /* Does FlexMemory Exist on this device ? */
    /******************************************/
    if (flashSSDConfig.EEEBlockSize) 
    {   
      PRINTF("\n\r*\tEnhanced EEPROM (EEE) Block Size:\t%d KB,\tHex: (0x%x)", (flashSSDConfig.EEEBlockSize/ONE_KB), flashSSDConfig.EEEBlockSize); 
      PRINTF("\n\r*\tEnhanced EEPROM (EEE) Base Address:\t0x%x", flashSSDConfig.EERAMBlockBase);
    }
    else
    {
      PRINTF("\n\r*\tNo Enhanced EEPROM (EEE) Present on this Device..."); 
    }    
    
    /**************************************/
    /* Is Swap Supported on this device ? */
    /**************************************/
#if (defined(SWAP))
    
    PRINTF("\n\r*\tSwap is Supported on this Device...");

#else
    
    PRINTF("\n\r*\tSwap is NOT Supported on this Device...");

#endif     
    
    PRINTF("\n\r*****************************************************************");
    
    PRINTF("\n\n\r....................Now Running Demo..................\n");   
    
    /*********************************************/
    /* END: print welcome message for flash demo */
    /*********************************************/  
       
    /*************************************/
    /* Set CallBack to callback function */
    /*************************************/
    flashSSDConfig.CallBack = (PCALLBACK)callback;
       
    /**************************************************************************
    *                          FlashEraseAllBlock()                           *
    ***************************************************************************/
    PRINTF("\n\n\r---->Running FlashEraseAllBlock()...");
    ret = FlashEraseAllBlock(&flashSSDConfig, FlashCommandSequence);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    PRINTF("\n\r\tSuccessfully Erased All Blocks, Address Range: 0x%x -> 0x%x", (flashSSDConfig.PFlashBlockBase), (flashSSDConfig.PFlashBlockBase + flashSSDConfig.PFlashBlockSize));
    
    /**************************************************************************
    *                          FlashVerifyAllBlock()                          *
    ***************************************************************************/
    /* Verify erase all blocks with normal and user margin levels */
    /* 0=Normal read level; 1=User Margin read          */
    /* Use Normal read level for regular usage, Margin for diagnostics */    
    PRINTF("\n\n\r---->Running FlashVerifyAllBlock()...");
    for (margin_read_level = 0; margin_read_level < 0x2; margin_read_level++) 
    {
        ret = FlashVerifyAllBlock(&flashSSDConfig, margin_read_level, FlashCommandSequence);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }
    }
    PRINTF("\n\r\tSuccessfully Verified All Blocks, Address Range: 0x%x -> 0x%x", (flashSSDConfig.PFlashBlockBase), (flashSSDConfig.PFlashBlockBase + flashSSDConfig.PFlashBlockSize));
    
    /* Program the Security Byte in the Flash Configuration Field to an unsecure value */
    ret = FlashProgram(&flashSSDConfig, SECURITY_LOCATION, PGM_SIZE_BYTE, unsecure_key, FlashCommandSequence);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    PRINTF("\n\n\r---->Program the Security Byte 0xFE at Address: 0x40C");
    PRINTF("\n\r\tNote:  This step is required or the device will be secured!");
    PRINTF("\n\r\tNote:  See Flash FSEC register description for detail...");
                
    
    /**************************************************************************
    *    FlashProgram() FlashCheckSum() and  FlashProgramCheck()        *
    ***************************************************************************/
    PRINTF("\n\n\r---->Running FlashProgram(), FlashCheckSum() and FlashProgramCheck()...");
    /* Initialize source buffer */
    for (i = 0; i < BUFFER_SIZE_BYTE; i++)
    {
        /* Set source buffer */
        buffer[i] = i;
    }
   
    /* Program some data to location 0x100 in PFlash */
    destination = flashSSDConfig.PFlashBlockBase + BUFFER_SIZE_BYTE;    
    size = BUFFER_SIZE_BYTE;

    ret = FlashProgram(&flashSSDConfig, destination, size, \
                                       buffer, FlashCommandSequence);
    
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    PRINTF("\n\r\tSuccessfully Programmed Flash at Location 0x%x -> 0x%x", destination, (destination + size));
    
    /* Program Check for normal and user margin levels*/
    for (margin_read_level = 1; margin_read_level < 0x2; margin_read_level++)
    {
        ret = FlashProgramCheck(&flashSSDConfig, destination, size, buffer, \
                                    &FailAddr, margin_read_level, FlashCommandSequence);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }
    }
    
    PRINTF("\n\r\tSuccessfully Verified using FlashProgramCheck() at Location 0x%x -> 0x%x", destination, (destination + size));
       
    /*****************************************/
    /* Program to the end location of PFLASH */
    /*****************************************/    
    size = BUFFER_SIZE_BYTE;
    destination = flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize - FTFx_PSECTOR_SIZE);
    
    while ((destination + BYTE2WORD(size)) <= (flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize)))
    {
        ret = FlashProgram(&flashSSDConfig, destination, size, \
                                       buffer, FlashCommandSequence);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }
               
        /* Program Check for normal and user margin levels*/
        for (margin_read_level = 1; margin_read_level < 0x2; margin_read_level++)
        {
            ret = FlashProgramCheck(&flashSSDConfig, destination, size, buffer, \
                                        &FailAddr, margin_read_level, FlashCommandSequence);
            if (FTFx_OK != ret)
            {
                ErrorTrap(ret);
            }
        }
        
        PRINTF("\n\r\tSuccessfully Programmed and Verified Location 0x%x -> 0x%x", destination, (destination + size));
        
        destination += BYTE2WORD(BUFFER_SIZE_BYTE);
    }   
	
#if (DEBLOCK_SIZE != 0)
    PRINTF("\n\n\r---->Running D-Flash Programming...");
    /* Program to the DFLASH block*/
    size = BUFFER_SIZE_BYTE;
    destination = flashSSDConfig.DFlashBlockBase + BYTE2WORD(flashSSDConfig.DFlashBlockSize - 4*FTFx_DSECTOR_SIZE) ;

    while ((destination + BYTE2WORD(size)) < (flashSSDConfig.DFlashBlockBase + BYTE2WORD(flashSSDConfig.DFlashBlockSize)))
    {
        ret = FlashProgram(&flashSSDConfig, destination, size, \
                                       buffer, FlashCommandSequence);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }
               
        /* Program Check for user margin levels*/
        for (margin_read_level = 1; margin_read_level < 0x2; margin_read_level++)
        {
            ret = FlashProgramCheck(&flashSSDConfig, destination, size, buffer, \
                                        &FailAddr, i, FlashCommandSequence);
            if (FTFx_OK != ret)
            {
                ErrorTrap(ret);
            }
        }
        
        PRINTF("\n\r\tSuccessfully Programmed and Verified D-Flash Location 0x%x -> 0x%x", destination, (destination + size));
        
        destination += BYTE2WORD(BUFFER_SIZE_BYTE);
    }
#endif /* DEBLOCK_SIZE */

#if ((!(defined(FTFA_M))) || (defined(BLOCK_COMMANDS)))
    /**************************************************************************
    *                     FlashEraseBlock() and FlashVerifyBlock()            *
    ***************************************************************************/
    PRINTF("\n\n\r---->Running FlashEraseBlock() and FlashVerifyBlock()...");
    /* Erase for each individual Pflash block */
    destination = flashSSDConfig.PFlashBlockBase;
    for (i = 0; i < PBLOCK_NUM; i ++)
    {
        ret = FlashEraseBlock(&flashSSDConfig, destination, FlashCommandSequence);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }
        /* verification for normal and user margin levels */
        for (margin_read_level = 0; margin_read_level < 0x2; margin_read_level++)
        {
            ret = FlashVerifyBlock(&flashSSDConfig, destination, margin_read_level, FlashCommandSequence);
            if (FTFx_OK != ret)
            {
                ErrorTrap(ret);
            }
        }
        
        PRINTF("\n\r\tSuccessfully Erased and Verified Location 0x%x -> 0x%x", destination, (destination + flashSSDConfig.PFlashBlockSize/PBLOCK_NUM));
        
        destination += BYTE2WORD(flashSSDConfig.PFlashBlockSize/PBLOCK_NUM);
    }
    
    /* Program the Security Byte in the Flash Configuration Field to an unsecure value */
    ret = FlashProgram(&flashSSDConfig, SECURITY_LOCATION, PGM_SIZE_BYTE, unsecure_key, FlashCommandSequence);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }  
    PRINTF("\n\n\r---->Program the Security Byte 0xFE at Address: 0x40C");
    
#endif
    
#if (DEBLOCK_SIZE != 0)
    
    PRINTF("\n\n\r---->Running D-Flash Erase and Verify...");
    
    /* On DFLASH */
    destination = flashSSDConfig.DFlashBlockBase;
	for (i = 0; i < DBLOCK_NUM; i ++)
    {
		ret = FlashEraseBlock(&flashSSDConfig, destination, FlashCommandSequence);
		if (FTFx_OK != ret)
		{
			ErrorTrap(ret);
		}

		/* verification for normal and user margin levels */
		for (margin_read_level = 0; margin_read_level < 0x2; margin_read_level ++)
		{
			ret = FlashVerifyBlock(&flashSSDConfig, destination, margin_read_level, FlashCommandSequence);
			if (FTFx_OK != ret)
			{
				ErrorTrap(ret);
			}
		}
        
        PRINTF("\n\r\tSuccessfully Erased D-Flash Block 0x%x -> 0x%x", destination, (destination + flashSSDConfig.DFlashBlockSize);
        
		destination += BYTE2WORD(flashSSDConfig.DFlashBlockSize/DBLOCK_NUM);
    }
#endif /* DEBLOCK_SIZE */    

#ifndef FTFA_M
    /**************************************************************************
    *                          FlashProgramSection()                          *
    ***************************************************************************/
    PRINTF("\n\n\r---->Running FlashProgramSection using Acceleration RAM...");
    
    /* Write some values to Acceleration RAM */
    /* Note:  only 1K out of 4K of Acceleration RAM used */
    /* for ProgramSection command */
    for (i=0; i < 0x100; i+=4)
    {
        WRITE32(flashSSDConfig.EERAMBlockBase + i,0x11223344);
    }
 
    /* Program the values to each individual Pflash block*/
    for (i = 0; i < PBLOCK_NUM; i ++)
    {
        number = 0x2;
        destination = flashSSDConfig.PFlashBlockBase + BYTE2WORD(i*flashSSDConfig.PFlashBlockSize/PBLOCK_NUM);
        ret = FlashProgramSection(&flashSSDConfig, destination, number, FlashCommandSequence);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }
        
        PRINTF("\n\r\tSuccessfully Programmed Flash using EERAM 0x%x", destination);
        
        destination = flashSSDConfig.PFlashBlockBase + BYTE2WORD(i*flashSSDConfig.PFlashBlockSize/PBLOCK_NUM + flashSSDConfig.PFlashBlockSize/PBLOCK_NUM - number*PPGMSEC_ALIGN_SIZE);
        ret = FlashProgramSection(&flashSSDConfig, destination, number, FlashCommandSequence);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }
        
        PRINTF("\n\r\tSuccessfully Programmed Flash Location 0x%x using Acceleration RAM", destination);
    }  
#endif
    
#if (DEBLOCK_SIZE != 0)
    
    PRINTF("\n\n\r---->Running D-Flash Programming...");
    /* Program the values to the 1st sector of DFLASH */
    number = FTFx_DSECTOR_SIZE/(DPGMSEC_ALIGN_SIZE*4);
    destination = flashSSDConfig.DFlashBlockBase;  
    ret = FlashProgramSection(&flashSSDConfig, destination, number, FlashCommandSequence);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    PRINTF("\n\r\tSuccessfully Programmed D-Flash Block at 0x%x", destination);
    
    /* Program the values to the last sector of DFLASH */
    number = FTFx_DSECTOR_SIZE/(DPGMSEC_ALIGN_SIZE*4);
    destination = flashSSDConfig.DFlashBlockBase + BYTE2WORD((UINT32)(flashSSDConfig.DFlashBlockSize - number*DPGMSEC_ALIGN_SIZE));  
    ret = FlashProgramSection(&flashSSDConfig, destination, number, FlashCommandSequence);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    PRINTF("\n\r\tSuccessfully Programmed D-Flash Block at 0x%x", destination);
    
#endif /* DEBLOCK_SIZE */    
    
    /**************************************************************************
    *                          FlashReadResource()                            *
    ***************************************************************************/
    /* Read on P-Flash */
    destination = flashSSDConfig.PFlashBlockBase + PFLASH_IFR; /* Start address of Program Once Field */
    ret = FlashReadResource(&flashSSDConfig, destination, DataArray, 0x0, FlashCommandSequence);

    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    p_data = (UINT32 *)&DataArray;
    PRINTF("\n\r\tSuccessfully Read PFLASH IFR 0x%x:  0x%x", destination, *p_data);
    
#if (DEBLOCK_SIZE != 0)
    /* Read on D-Flash */
    destination = flashSSDConfig.DFlashBlockBase + DFLASH_IFR;
    ret = FlashReadResource(&flashSSDConfig, destination, DataArray, 0x0, FlashCommandSequence);

    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    p_data = (UINT32 *)&DataArray;
    PRINTF("\n\r\tSuccessfully Read D-Flash IFR 0x%x:  0x%x", destination, *p_data);
#endif /* DEBLOCK_SIZE != 0 */
    
    /**************************************************************************
    *                          FlashGetSecurityState()                        *
    ***************************************************************************/
    securityStatus = 0x0;
    ret = FlashGetSecurityState(&flashSSDConfig, &securityStatus);

    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    /* Message to user on flash security state 
    #define FLASH_NOT_SECURE                   0x01
    #define FLASH_SECURE_BACKDOOR_ENABLED      0x02
    #define FLASH_SECURE_BACKDOOR_DISABLED     0x04
    */
    switch(securityStatus)
    {
      case 1:
      default:
          PRINTF("\n\n\r---->Flash is UNSECURE!");
          break;
      case 2:
          PRINTF("\n\n\r---->Flash is SECURE, BACKDOOR is ENABLED!");
          break;
      case 3:
        PRINTF("\n\n\r---->Flash is SECURE, BACKDOOR is DISABLED!");
        break;
    }
    
    /**************************************************************************
    *               FlashEraseSector()  and FlashVerifySection()              *
    ***************************************************************************/
    PRINTF("\n\n\r---->Running FlashEraseSector() and FlashVerifySection()...");
    /* Erase serveral sectors on Pflash with different size*/
    i = 1;
    destination = flashSSDConfig.PFlashBlockBase;
    while ((destination + BYTE2WORD(i*FTFx_PSECTOR_SIZE)) < (flashSSDConfig.PFlashBlockBase + BYTE2WORD(flashSSDConfig.PFlashBlockSize)))
    {
        size = i*FTFx_PSECTOR_SIZE;
        ret = FlashEraseSector(&flashSSDConfig, destination, size, \
                                     FlashCommandSequence);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }

        /* Verify section for several sector of PFLASH */
        number = FTFx_PSECTOR_SIZE/PRD1SEC_ALIGN_SIZE;
        for(margin_read_level = 0; margin_read_level < 0x2; margin_read_level++)
        {
            ret = FlashVerifySection(&flashSSDConfig, destination, number, \
                                            margin_read_level, FlashCommandSequence);
            if (FTFx_OK != ret)
            {
                ErrorTrap(ret);
            }
        }
        
        PRINTF("\n\r\tSuccessfully Read PFLASH Location 0x%x -> 0x%x", destination, (destination+number));
        
        destination += BYTE2WORD(flashSSDConfig.PFlashBlockSize/PBLOCK_NUM - size);
        i++;
    }
    
    /* Program the Security Byte in the Flash Configuration Field to an unsecure value */
    ret = FlashProgram(&flashSSDConfig, SECURITY_LOCATION, PGM_SIZE_BYTE, unsecure_key, FlashCommandSequence);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    PRINTF("\n\n\r---->Program the Security Byte 0xFE at Address: 0x40C");
    
#if (DEBLOCK_SIZE != 0)    
    
    PRINTF("\n\n\r---->Running D-Flash FlashEraseSector() and FlashVerifySection()...");
    /* Erase several sectors on Dflash with different size*/
    i = 1;
    destination = flashSSDConfig.DFlashBlockBase;
    while ((destination + BYTE2WORD(i*FTFx_DSECTOR_SIZE)) < (flashSSDConfig.DFlashBlockBase + BYTE2WORD(flashSSDConfig.DFlashBlockSize)))
    {
        size = i*FTFx_DSECTOR_SIZE;
        ret = FlashEraseSector(&flashSSDConfig, destination, size, \
                                     FlashCommandSequence);
        if (FTFx_OK != ret)
        {
            ErrorTrap(ret);
        }

        /* Verify section for several sector of DFLASH */
        number = FTFx_DSECTOR_SIZE/DRD1SEC_ALIGN_SIZE;
        for(margin_read_level = 0; margin_read_level < 0x2; margin_read_level ++)
        {
            ret = FlashVerifySection(&flashSSDConfig, destination, number, \
                                            margin_read_level, FlashCommandSequence);
            if (FTFx_OK != ret)
            {
                ErrorTrap(ret);
            }
        }
        
        PRINTF("\n\r\tSuccessfully Programmed and Verified D-Flash at Location 0x%x", destination, (destination+size));
        
        destination += BYTE2WORD(DEBLOCK_SIZE - size);
        i++;
    }    
#endif /* DEBLOCK_SIZE */    
    
#if (DEBLOCK_SIZE != 0)
        
    /* Message to user */
    PRINTF("\n\r---->Running DFlash and EEE Demo... ");    
    
    /**************************************************************************
    *                            DEFlashPartition()                           *
    ***************************************************************************/
    ret = DEFlashPartition(&flashSSDConfig, \
                                    EEE_DATA_SIZE_CODE, \
                                    DE_PARTITION_CODE, \
                                    FlashCommandSequence);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }  
    
    /* Call FlashInit again to get the new Flash configuration */
    ret = FlashInit(&flashSSDConfig);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    
    /**************************************************************************
    *                               EEEWrite()                                *
    ***************************************************************************/
    destination = flashSSDConfig.EERAMBlockBase;
    size = FTFx_WORD_SIZE;
    ret = EEEWrite(&flashSSDConfig, destination, size, buffer);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    /**************************************************************************
    *                            SetEEEEnable()                               *
    ***************************************************************************/
    ret = SetEEEEnable(&flashSSDConfig, RAM_ENABLE , FlashCommandSequence);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }    
    /* Erase all block */
    ret = FlashEraseAllBlock(&flashSSDConfig, FlashCommandSequence);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    /* Program the Security Byte in the Flash Configuration Field to an unsecure value */
	ret = FlashProgram(&flashSSDConfig, SECURITY_LOCATION, PGM_SIZE_BYTE, unsecure_key, FlashCommandSequence);
	if (FTFx_OK != ret)
	{
	  ErrorTrap(ret);
	} 
    ret = FlashInit(&flashSSDConfig);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
       
    /**************************************************************************
    *                          DFlashGetProtection()                          *
    ***************************************************************************/
    ret = DFlashGetProtection(&flashSSDConfig, &protectStatus);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    
    /**************************************************************************
    *                          DFlashSetProtection()                          *
    ***************************************************************************/
    protectStatus = 0xAA;
    ret = DFlashSetProtection(&flashSSDConfig, protectStatus);
    if (FTFx_OK != ret)
    {
        ErrorTrap(ret);
    }
    
    /* Call DFlashGetProtection to verify the set step */
    ret = DFlashGetProtection(&flashSSDConfig, &protectStatus);
    if (FTFx_OK != ret || 0xAA != protectStatus)
    {
        ErrorTrap(ret);
    }
    
    /* Message to user */
    PRINTF("\n\r\tDFlash & EEE Demo Complete!");    
    
#endif /* DEBLOCK_SIZE */    
       
    /* Message to user */
    PRINTF("\n\n\n\r--------------------------------");
    PRINTF("\n\r----- Flash Demo Complete! -----");
    PRINTF("\n\r--------------------------------");           
        
    while(1);
}  

/*********************************************************************
*
*  Function Name    : ErrorTrap
*  Description      : Gets called when an error occurs.
*  Arguments        : UINT32
*  Return Value     :
*
*********************************************************************/
void ErrorTrap(UINT32 ret)
{
  PRINTF("\n\n\n\t---- HALTED DUE TO FLASH ERROR! ----");  
  while (1)
    {
        ;
    }
}

void callback(void)
{
  /* just increase this variable to observer that this callback() func has been involked */
   gCallBackCnt ++;
}
               

              
/* end of file */
