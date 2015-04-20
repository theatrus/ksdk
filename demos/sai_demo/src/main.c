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
 *
 */

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

// Standard C Included Files
#include <stdio.h>
#include <string.h>
// SDK Included Files
#include "fsl_sgtl5000_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_device_registers.h"
#include "fsl_soundcard.h"
#include "board.h"
// Application Included Files
#include "audio.h"
#include "equalizer.h"
#include "pcm_data.h"
#include "terminal_menu.h" 

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////

void pin_setup(void);
void user_menu(void);
void playback_set(uint8_t *menuData);

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

extern menu_machine_t g_menuMachine[];
extern menu_state_t g_menuState;

#if !(defined(CPU_MK22FN128VDC10) || defined(CPU_MK22FN256VDC12))
float32_t g_dspStore[2 * AUDIO_BUFFER_BLOCK_SIZE];
float32_t g_dspResult[AUDIO_BUFFER_BLOCK_SIZE];
float32_t g_dspInput[AUDIO_BUFFER_BLOCK_SIZE];
#endif

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

void pin_setup(void)
{
    configure_i2s_pins(0);
}

int main (void)
{ 
    hardware_init();

    // Initialize UART terminal.
    dbg_uart_init();

    // Configure I2C pins
    configure_i2c_pins(BOARD_SAI_DEMO_I2C_INSTANCE);

    // Configure SAI pins.
    pin_setup();

    // Initialize OSA
    OSA_Init();

    // Print welcome message.
    menu_start();

    while(1)
    {
        user_menu(); // User menu, viewable through serial terminal.
    }
}

void user_menu(void)
{
    uint8_t menuMsg[3];
    uint8_t count;

    count = 0;
    while(g_menuState < kMenuEndState)
    {
        menuMsg[count] = (*g_menuMachine[g_menuState].menu_func)();
        count++;
    }

    playback_set(menuMsg);

    g_menuState = kMenuSelectPlay;

}

void playback_set(uint8_t *menuData)
{

    switch(menuData[0])
    {
        case '1':
            switch(menuData[1])
            {
            case '1':
#if __FPU_PRESENT
                stream_audio(kFFT, menuData[2]);
#else
                printf("No FPU, this feature do not support.!\n\r ");
#endif
                break;

            case '2':
                stream_audio(kNoDSP, menuData[2]);
                break;

            default:
                __asm("NOP");
                break;
            }
            break;

        case '2':
            switch(menuData[1])
            {
            case '1':
                play_wav((uint32_t *)music, menuData[2]);
                break;

            default:
                __asm("NOP");
                break;
            }
            break;

        default:
            __asm("NOP");
            break;
    }

}
