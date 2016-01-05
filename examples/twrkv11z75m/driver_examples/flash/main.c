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
// Includes
///////////////////////////////////////////////////////////////////////////////
// Standard C Included Files
#include <string.h>
#include <stdio.h>
// SDK Included Files
#include "board.h"
#include "main.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
uint8_t buffer[BUFFER_SIZE_BYTE]; /*! Buffer for program */
pFLASHCOMMANDSEQUENCE g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)0xFFFFFFFF;

// Array to copy __Launch_Command func to RAM.
uint16_t ramFunc[LAUNCH_CMD_SIZE/2];

// Flash Standard Software Driver Structure.
FLASH_SSD_CONFIG flashSSDConfig =
{
    FTFx_REG_BASE,          /*! FTFx control register base */
    P_FLASH_BASE,           /*! Base address of PFlash block */
    P_FLASH_SIZE,           /*! Size of PFlash block */
    FLEXNVM_BASE,           /*! Base address of DFlash block */
    0,                      /*! Size of DFlash block */
    EERAM_BASE,             /*! Base address of EERAM block */
    0,                      /*! Size of EEE block */
    DEBUGENABLE,            /*! Background debug mode enable bit */
    NULL_CALLBACK           /*! Pointer to callback function */
};

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Use Standard Software Drivers (SSD) to modify flash.
 *
 * This function uses SSD to demonstrate flash mode:
 *      + Check flash information.
 *      + Erase a sector and verify.
 *      + Program a sector and verify.
 */
int main(void)
{
    uint32_t result;               /*! Return code from each SSD function */
    uint32_t destAdrss;            /*! Address of the target location */
    uint8_t  securityStatus = 0;   /*! Return protection status */
    uint16_t number;               /*! Number of longword or phrase to be verified */
    uint32_t marginReadLevel;      /*! 0=normal, 1=user, 2=factory - margin read for reading */
    uint32_t i, failAddr;

    // Init hardware
    hardware_init();

    // Setup flash SSD structure for device and initialize variables.
    result = FlashInit(&flashSSDConfig);
    if (FTFx_OK != result)
    {
        error_trap();
    }
    // print welcome message
    PRINTF("\r\n Flash Example Start \r\n");
    // Print flash information - PFlash.
    PRINTF("\r\n Flash Information: ");
    PRINTF("\r\n Total Flash Size:\t%d KB, Hex: (0x%x)", (P_FLASH_SIZE/ONE_KB), P_FLASH_SIZE);
    PRINTF("\r\n Flash Sector Size:\t%d KB, Hex: (0x%x) ", (FTFx_PSECTOR_SIZE/ONE_KB), FTFx_PSECTOR_SIZE);
    // Check if DFlash exist on this device.
    if (flashSSDConfig.DFlashSize)
    {
        PRINTF("\r\n Data Flash Size:\t%d KB,\tHex: (0x%x)", (int)(flashSSDConfig.DFlashSize/ONE_KB), (unsigned int)flashSSDConfig.DFlashSize);
        PRINTF("\r\n Data Flash Base Address:\t0x%x", (unsigned int)flashSSDConfig.DFlashBase);
    }
    else
    {
      PRINTF("\r\n There is no D-Flash (FlexNVM) on this Device.");
    }
    // Check if FlexMemory exist on this device.
    if (flashSSDConfig.EEESize)
    {
        PRINTF("\r\n Enhanced EEPROM (EEE) Block Size:\t%d KB,\tHex: (0x%x)", (int)(flashSSDConfig.EEESize/ONE_KB), (unsigned int)flashSSDConfig.EEESize);
        PRINTF("\r\n Enhanced EEPROM (EEE) Base Address:\t0x%x", (unsigned int)flashSSDConfig.EERAMBase);
    }
    else
    {
      PRINTF("\r\n There is no Enhanced EEPROM (EEE) on this Device.");
    }

    // Check security status.
    result = FlashGetSecurityState(&flashSSDConfig, &securityStatus);
    if (FTFx_OK != result)
    {
        error_trap();
    }
    // Print security status.
    switch(securityStatus)
    {
        case FLASH_NOT_SECURE:
            PRINTF("\r\n Flash is UNSECURE!");
            break;
        case FLASH_SECURE_BACKDOOR_ENABLED:
            PRINTF("\r\n Flash is SECURE, BACKDOOR is ENABLED!");
            break;
        case FLASH_SECURE_BACKDOOR_DISABLED:
            PRINTF("\r\n Flash is SECURE, BACKDOOR is DISABLED!");
            break;
        default:
            break;
    }
    PRINTF("\r\n");

    // Debug message for user.
    // Erase several sectors on upper pflash block where there is no code
    // because we are running from Flash.
    PRINTF("\r\n Erase a sector of flash");

    // Set command to RAM.
    g_FlashLaunchCommand = (pFLASHCOMMANDSEQUENCE)RelocateFunction((uint32_t)ramFunc , LAUNCH_CMD_SIZE ,(uint32_t)FlashCommandSequence);

    // Erase a sector from destAdrss.
    destAdrss = flashSSDConfig.PFlashBase + (flashSSDConfig.PFlashSize - 6*FTFx_PSECTOR_SIZE);
    result = FlashEraseSector(&flashSSDConfig, destAdrss, FTFx_PSECTOR_SIZE, g_FlashLaunchCommand);
    if (FTFx_OK != result)
    {
        error_trap();
    }

    // Verify sector if it's been erased.
    // Number of long to verify.
    number = FTFx_PSECTOR_SIZE/FSL_FEATURE_FLASH_PFLASH_SECTION_CMD_ADDRESS_ALIGMENT;
    for(marginReadLevel = 0; marginReadLevel <=0x2; marginReadLevel++)
    {
        result = FlashVerifySection(&flashSSDConfig, destAdrss, number, marginReadLevel, g_FlashLaunchCommand);
        if (FTFx_OK != result)
        {
            error_trap();
        }
    }
    // Print message for user.
    PRINTF("\r\n Successfully Erased Sector 0x%x -> 0x%x\r\n", (unsigned int)destAdrss, (unsigned int)(destAdrss+FTFx_PSECTOR_SIZE));

    // Print message for user.
    PRINTF("\r\n Program a buffer to a sector of flash ");
    // Prepare buffer.
    for (i = 0; i < BUFFER_SIZE_BYTE; i++)
    {
        buffer[i] = i;
    }
    destAdrss = flashSSDConfig.PFlashBase + (flashSSDConfig.PFlashSize - 6*FTFx_PSECTOR_SIZE);
    result = FlashProgram(&flashSSDConfig, destAdrss, BUFFER_SIZE_BYTE, \
                                   buffer, g_FlashLaunchCommand);
    if (FTFx_OK != result)
    {
        error_trap();
    }

    // Program Check user margin levels
    for (marginReadLevel = 1; marginReadLevel <=0x2; marginReadLevel++)
    {
        result = FlashProgramCheck(&flashSSDConfig, destAdrss, BUFFER_SIZE_BYTE, buffer, \
                                    &failAddr, marginReadLevel, g_FlashLaunchCommand);
        if (FTFx_OK != result)
        {
            error_trap();
        }
    }
    PRINTF("\r\n Successfully Programmed and Verified Location 0x%x -> 0x%x \r\n", (unsigned int)destAdrss, (unsigned int)(destAdrss + BUFFER_SIZE_BYTE));

    // Print finished message.
    PRINTF("\r\n Flash Example End \r\n");
    while(1);
}

/*
* @brief Gets called when an error occurs.
* Print error message and trap forever.
*/
void error_trap(void)
{
    PRINTF("\r\n\r\n\r\n\t---- HALTED DUE TO FLASH ERROR! ----");
    while (1)
    {}
}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////

