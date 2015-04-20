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
// Application Included Files
#include "terminal_menu.h"

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

menu_state_t g_menuState = kMenuStart;

menu_machine_t g_menuMachine[] = 
{
    { kMenuStart,        menu_start         },
    { kMenuSelectPlay,   menu_player_select },
    { kMenuSelectWav,    menu_wav_select    },
    { kMenuSelectDSP,    menu_dsp_select    },
    { kMenuSetVolume,    menu_set_volume    }
};

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

uint8_t menu_start(void)
{
    volatile uint8_t msg;

    #if defined(__GNUC__)
    setvbuf(stdin, NULL, _IONBF, 0);
    #endif

    printf("\r\nAudio Demo!\r\n");
    
    printf("\r\nPress spacebar to start demo.\r\n");

    msg = 'A';

    while(msg != ' ')
    {
        msg = getchar();
    }

    printf("\r\nDemo begin...\r\n" );

    g_menuState = kMenuSelectPlay;

    return msg;
}

uint8_t menu_player_select(void)
{
    uint8_t msg;

    printf("\r\n\r\nSelect player:\r\n\t1. Line-In Playback\r\n\t2. Wav File Playback\r\n-> ");

    msg = ' ';

    while((msg < '1') || (msg > '2'))
    {
        printf("\b");
        msg = getchar();
        printf("%c", msg);
    }

    if(msg == '1')
    {
        g_menuState = kMenuSelectDSP;
    }
    else
    {
        g_menuState = kMenuSelectWav;
    }

    return msg;
}

uint8_t menu_wav_select(void)
{
    uint8_t msg;

    printf("\r\n\r\nSelect wav file:\r\n\t1. Audio Demo\r\n-> ");

    msg = ' ';
    
    while((msg < '1') || (msg > '1'))
    {
        printf("\b");
        msg = getchar();
        printf("%c", msg);
    }

    g_menuState = kMenuSetVolume;

    return msg;
}

uint8_t menu_dsp_select(void)
{
    uint8_t msg;

#if !(defined(CPU_MK22FN128VDC10) || defined(CPU_MK22FN256VDC12))
    printf("\r\n\r\nSelect filter:\r\n\t1. FFT - Find Fundamental Frequency\r\n\t2. None\r\n-> ");

    msg = ' ';

    while((msg < '1') || (msg > '2'))
    {
        printf("\b");
        msg = getchar();
        printf("%c", msg);
    }
#else
    printf("\r\n\r\nSelect filter:\r\n\t1. None\r\n->");

    msg = ' ';

    while((msg < '1') || (msg > '1'))
    {
        printf("\b");
        msg = getchar();
        printf("%c", msg);
    }
#endif

    g_menuState = kMenuSetVolume;

    return msg;
}

uint8_t menu_set_volume(void)
{
    uint8_t msg;
    uint8_t vol;

    printf("\r\n\r\nChoose headphone dB level:\r\n\t1. +3.0 dB\r\n\t2. 0.0 dB\r\n\t3. -3.0 dB\r\n\t4. -6.0 dB\r\n\t5. -12.0 dB\r\n\t6. -24.0 dB\r\n\t7. -48.0 dB\r\n-> ");

    msg = ' ';

    while((msg < '1') || (msg > '7'))
    {
        printf("\b");
        msg = getchar();
        printf("%c", msg);
    }

    vol = msg - 0x30;

    g_menuState = kMenuEndState;

    return vol;
}
