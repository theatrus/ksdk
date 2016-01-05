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
#include <time.h>
// SDK Included Files
#include "fsl_crc_driver.h"
#include "board.h"
#include "fsl_debug_console.h"
///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define CRC_INSTANCE         (0)

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Calculates checksum number of an array, gererated randomly.
 */
int main(void)
{
    uint32_t i,crcValue, buffSize, temp;
    uint8_t * buffer;
    // Configure CRC.
    crc_user_config_t userConfigPtr;

    // Init hardware
    hardware_init();

    // Init CRC-16-CCITT
    // seed disable
    // polynomial 1000000100001: x^12+x^5+1
    userConfigPtr.crcWidth       = kCrc16Bits;
    userConfigPtr.seed           = 0x0000U;
    userConfigPtr.polynomial     = 0x1021U;
    userConfigPtr.writeTranspose = kCrcTransposeBits;
    userConfigPtr.readTranspose  = kCrcTransposeBits;
    userConfigPtr.complementRead = false;

    // Init CRC.
    CRC_DRV_Init(CRC_INSTANCE, &userConfigPtr);

    PRINTF("\r\nCRC EXAMPLE \r\n");
    while(1)
    {
        // Configure CRC
        CRC_DRV_Configure(CRC_INSTANCE, &userConfigPtr);
        PRINTF("Please input size of buffer: ");
        SCANF("%d", &buffSize);
        PRINTF("%d\r\n", buffSize);
        buffer = (uint8_t*)OSA_MemAlloc(buffSize);
        PRINTF("Please input %d random numbers [0 - 255]:\r\n", buffSize);
        // Randomize buffer.
        for(i = 0; i < buffSize; i++)
        {
            if (0 == (i & 0x0Fu))
            {
                PRINTF("\r\n");
            }
            PRINTF("buffer[%d] = ", i);
            SCANF("%d", &temp);
            buffer[i] = (uint8_t)temp;
            PRINTF("%d\r\n", buffer[i]);
        }

        // Calculates CRC value.
        crcValue = CRC_DRV_GetCrcBlock(CRC_INSTANCE, buffer, buffSize);

        // print calculated checksum number.
        PRINTF("\r\nChecksum value of buffer array 0x%0X\r\n",crcValue);

        // Free buffer
        OSA_MemFree(buffer);

        // Print a note.
        PRINTF("Press any key to continue \r\n");
        GETCHAR();
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/

