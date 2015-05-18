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
#include "board.h"
#include "fsl_dac_driver.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define DAC_INSTANCE          BOARD_DAC_DEMO_DAC_INSTANCE

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief DAC example.
 *
 * This function uses terminal to enter DAC value and
 * convert this value to DAC output.
 */
int main(void)
{
    dac_converter_config_t dacUserConfig;
    uint32_t ch;

    // Initialize hardware
    hardware_init();

    PRINTF("\r\nDAC Example.\r\n");

    // Fill the structure with configuration of software trigger.
    DAC_DRV_StructInitUserConfigNormal(&dacUserConfig);
    // Initialize the DAC Converter.
    DAC_DRV_Init(DAC_INSTANCE, &dacUserConfig);

    while(1)
    {
        ch = 0xFFFFu;
        PRINTF("Enter the value for DAC input from 0 to 4095 \r\n ");
        while (ch > 4095u)
        {
            SCANF("%d", &ch);
        }

        PRINTF("Check oscilloscope to see DAC output of %d\r\n",ch);
        DAC_DRV_Output(DAC_INSTANCE, (uint16_t)ch);
    }
}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
