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
//  Includes
///////////////////////////////////////////////////////////////////////////////

// Standard C Included Files
#include <stdio.h>
// SDK Included Files
#include "board.h"
#include "fsl_rnga_driver.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define RNGA_INSTANCE       0

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Gererate 10 random numbers and then print to terminal.
 */

int main(void)
{
    uint32_t i;
    uint32_t randout;
    rnga_status_t status;
    rnga_user_config_t rngaConfig;

    // Init hardware
    hardware_init();

    PRINTF("RNGA Peripheral Driver Example\r\n");
    PRINTF("Generate 10 random numbers\n\r");

    // Initialize RNGA
    rngaConfig.isIntMasked         = true;
    rngaConfig.highAssuranceEnable = true;

    status = RNGA_DRV_Init(RNGA_INSTANCE, &rngaConfig);
    if (kStatus_RNGA_Success != status)
    {
        PRINTF("RNGA initialize failed!\r\n");

        // Disable RNGA.
        RNGA_DRV_Deinit(RNGA_INSTANCE);
        return -1;
    }

    while(1)
    {
        PRINTF("Generate 10 number: \r\n");

        // Generate 10 random numbers.
        for(i = 0; i < 10; i++)
        {
            status = RNGA_DRV_GetRandomData(RNGA_INSTANCE, &randout);
            // Check if randomization is successful.
            if (kStatus_RNGA_Success == status)
            {
                PRINTF("Get random data No.%d: %08x.\r\n", i, randout);
            }
            else
            {
                PRINTF("No.%d random data generating failed!\r\n", i);
            }
        }

        // Print a note.
        PRINTF(" Press any key to continue \r\n");
        GETCHAR();
    }
}

/*******************************************************************************
 * EOF
 *******************************************************************************/

