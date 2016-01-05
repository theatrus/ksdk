/*
 * Copyright (c) 2013-2015, Freescale Semiconductor, Inc.
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
#include <string.h>
// SDK Included Files
#include "SLCD_Engine/slcd_engine.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_clock_manager.h"
#include "fsl_slcd_driver.h"
#include "fsl_rnga_driver.h"

#define off(n) ((uint32_t)1 << (n))
#define off1(n) ((uint32_t)1 << (n - 32))
#define RNGA_INSTANCE       0

static uint8_t slcd_lcd_gpio_seg_pin[SLCD_PIN_NUM] =
{
    38, 36, 34, 32, 31, 29, 25, 23, 43, 37,
    35, 33, 50, 30, 45, 24, 26, 28, 44, 59
};

static int32_t slcd_set_reg(int32_t lcd_pin, uint8_t pin_val, int32_t on)
{
    uint32_t i = 0;
    uint8_t bit_val = 0, gpio_pin = 0;

    /* lcd _pin starts from 1 */
    gpio_pin = slcd_lcd_gpio_seg_pin[lcd_pin - 1];

    for (i = 0; i < 8; ++i)
    {
        bit_val = (pin_val >> i) & 0x1;
        if (bit_val)
            SLCD_DRV_SetSegmentEnableCmd(0, gpio_pin, (slcd_phase_index_t)i, on);
    }

    return 0;
}

static int32_t rnga_hw_init(void)
{
    rnga_status_t status;
    rnga_user_config_t rngaConfig;

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

    return 0;
}

static int32_t slcd_hw_init(void)
{
    slcd_clk_config_t slcd_clkCommonConfig =
    {
      .clkSrc              = kSLCDDefaultClk,
#if FSL_FEATURE_SLCD_HAS_MULTI_ALTERNATE_CLOCK_SOURCE
      .altClkSrc           = kSLCDAltClkSrc1,
#endif
      .altClkDiv           = kSLCDAltClkDivFactor1,
      .clkPrescaler        = kSLCDClkPrescaler01,
#if FSL_FEATURE_SLCD_HAS_FAST_FRAME_RATE
      .fastFrameRateEnable = false,
#endif
    };
    slcd_work_mode_t slcd_PowerMode =
    {
        .kSLCDEnableInDozeMode = false,
        .kSLCDEnableInStopMode = false,
    };
    slcd_user_config_t slcd_init =
    {
        .clkConfig       = slcd_clkCommonConfig,
        .powerSupply     = kSLCDPowerInternalVll3AndChargePump,
        .loadAdjust      = kSLCDHighLoadOrSlowestClkSrc,
        .dutyCyc         = kSLCD1Div8DutyCyc,
        .slcdIntEnable   = false,
        .workMode        = slcd_PowerMode,
    };
    slcd_pins_config_t slcd_pinsConfig =
    {
        .slcdLowPinsEnabled           = (off(14)|off(20)|off(22)|off(13)|off(19)|off(28)|off(26)|off(24)|off(31)|off(29)|off(25)|off(23)|off(30)),
        .slcdHighPinsEnabled          = (off1(56)|off1(58)|off1(57)|off1(59)|off1(44)|off1(45)|off1(38)|off1(36)|off1(34)|off1(33)|off1(43)|off1(37)|off1(35)|off1(33)|off1(50)|off1(32)),
        .slcdBackPlaneLowPinsEnabled  = (off(14)|off(20)|off(22)|off(13)|off(19)),
        .slcdBackPlaneHighPinsEnabled = (off1(56)|off1(58)|off1(57)),
    };

    configure_lcd_pins();

    SLCD_DRV_Init(0, &slcd_init);

    SLCD_DRV_SetAllPinsConfig(0, &slcd_pinsConfig);

    return 0;
}

static int32_t slcd_clear_screen(void)
{
    uint32_t pinNum = 0;

    for(pinNum = 0; pinNum < FSL_FEATURE_SLCD_HAS_PIN_NUM; pinNum++)
    {
        SLCD_DRV_SetPinWaveForm(0, pinNum, 0x0);
    }

    SLCD_DRV_SetBackPlanePhase(0, 14, kSLCDPhaseH);
    SLCD_DRV_SetBackPlanePhase(0, 20, kSLCDPhaseG);
    SLCD_DRV_SetBackPlanePhase(0, 22, kSLCDPhaseF);
    SLCD_DRV_SetBackPlanePhase(0, 56, kSLCDPhaseE);
    SLCD_DRV_SetBackPlanePhase(0, 58, kSLCDPhaseD);
    SLCD_DRV_SetBackPlanePhase(0, 13, kSLCDPhaseC);
    SLCD_DRV_SetBackPlanePhase(0, 19, kSLCDPhaseB);
    SLCD_DRV_SetBackPlanePhase(0, 57, kSLCDPhaseA);

    return 0;
}

static int32_t slcd_basic_test(tSLCD_Engine *slcd_engine)
{
    uint32_t pinNum = 0, i = 0, j = 0;
    slcd_blink_config_t slcd_blinkConfig =
    {
        .blinkRate = kSLCDBlinkRate03,
        .blinkMode = kSLCDAltDisplay,
    };

    for(pinNum = 0; pinNum < FSL_FEATURE_SLCD_HAS_PIN_NUM; pinNum++)
    {
        SLCD_DRV_SetPinWaveForm(0, pinNum, 0xFF);
    }

    SLCD_DRV_SetBackPlanePhase(0, 14, kSLCDPhaseH);
    SLCD_DRV_SetBackPlanePhase(0, 20, kSLCDPhaseG);
    SLCD_DRV_SetBackPlanePhase(0, 22, kSLCDPhaseF);
    SLCD_DRV_SetBackPlanePhase(0, 56, kSLCDPhaseE);
    SLCD_DRV_SetBackPlanePhase(0, 58, kSLCDPhaseD);
    SLCD_DRV_SetBackPlanePhase(0, 13, kSLCDPhaseC);
    SLCD_DRV_SetBackPlanePhase(0, 19, kSLCDPhaseB);
    SLCD_DRV_SetBackPlanePhase(0, 57, kSLCDPhaseA);

    PRINTF("---------- Start basic SLCD test -------------\r\n");

    SLCD_DRV_Start(0);

    SLCD_DRV_StartBlinkingMode(0, &slcd_blinkConfig);

    OSA_TimeDelay(5000);

    SLCD_DRV_StopBlinkingMode(0);

    slcd_clear_screen();

    for (i = 0; i < NUM_POSEND; ++i)
    {
        for (j = 0; j < 10; ++j)
        {
            SLCD_Engine_Show_Num(slcd_engine, j, i, 1);
            OSA_TimeDelay(150);
            SLCD_Engine_Show_Num(slcd_engine, j, i, 0);
        }
    }

    for (i = 0; i < ICON_END; ++i)
    {
        SLCD_Engine_Show_Icon(slcd_engine, i, 1);
        OSA_TimeDelay(150);
        SLCD_Engine_Show_Icon(slcd_engine, i, 0);
    }
    OSA_TimeDelay(1000);

    return 0;
}

static int32_t slcd_show_victory(tSLCD_Engine *slcd_engine)
{
    SLCD_Engine_Show_Icon(slcd_engine, ICON_S3, 1);
    SLCD_Engine_Show_Icon(slcd_engine, ICON_S6, 1);
    SLCD_Engine_Show_Icon(slcd_engine, ICON_S7, 1);
    SLCD_Engine_Show_Icon(slcd_engine, ICON_S9, 1);
    SLCD_Engine_Show_Icon(slcd_engine, ICON_S11, 1);
    SLCD_Engine_Show_Icon(slcd_engine, ICON_S12, 1);
    SLCD_Engine_Show_Icon(slcd_engine, ICON_S13, 1);

    return 0;
}

static int32_t slcd_show_game_start(tSLCD_Engine *slcd_engine)
{
    SLCD_Engine_Show_Num(slcd_engine, 1, NUM_POS6, 0);
    SLCD_Engine_Show_Num(slcd_engine, 1, NUM_POS6, 1);
    OSA_TimeDelay(1000);
    SLCD_Engine_Show_Num(slcd_engine, 2, NUM_POS6, 0);
    SLCD_Engine_Show_Num(slcd_engine, 2, NUM_POS6, 1);
    OSA_TimeDelay(1000);
    SLCD_Engine_Show_Num(slcd_engine, 3, NUM_POS6, 0);
    SLCD_Engine_Show_Num(slcd_engine, 3, NUM_POS6, 1);
    OSA_TimeDelay(1000);
    SLCD_Engine_Show_Num(slcd_engine, 1, NUM_POS6, 0);
    /* Show Let */
    /* Clear number */
    /* Show G at num pos 8 */
    SLCD_Engine_Show_Num(slcd_engine, 0, NUM_POS5, 0);
    SLCD_Engine_Show_Icon(slcd_engine, ICON_5A, 1);
    SLCD_Engine_Show_Icon(slcd_engine, ICON_5C, 1);
    SLCD_Engine_Show_Icon(slcd_engine, ICON_5D, 1);
    SLCD_Engine_Show_Icon(slcd_engine, ICON_5E, 1);
    SLCD_Engine_Show_Icon(slcd_engine, ICON_5F, 1);

    /* Show O at num pos 9 */
    SLCD_Engine_Show_Num(slcd_engine, 0, NUM_POS6, 0);
    SLCD_Engine_Show_Num(slcd_engine, 0, NUM_POS6, 1);
    OSA_TimeDelay(1000);

    return 0;
}

static int32_t slcd_show_number(tSLCD_Engine *slcd_engine, int32_t num)
{
    uint32_t thousand_val = 0, hundred_val = 0, decimal_val = 0, single_val = 0;

    if ((num < 0) || (num > 9999))
    {
        return -1;
    }

    SLCD_Engine_Show_Num(slcd_engine, 8, NUM_POS3, 0);
    SLCD_Engine_Show_Num(slcd_engine, 8, NUM_POS4, 0);
    SLCD_Engine_Show_Num(slcd_engine, 8, NUM_POS5, 0);
    SLCD_Engine_Show_Num(slcd_engine, 8, NUM_POS6, 0);

    thousand_val = num / 1000;
    hundred_val  = (num % 1000) / 100;
    decimal_val  = (num % 100) / 10;
    single_val   = (num % 10);

    if (num >= 1000)
    {
        SLCD_Engine_Show_Num(slcd_engine, thousand_val, NUM_POS3, 0);
        SLCD_Engine_Show_Num(slcd_engine, thousand_val, NUM_POS3, 1);
    }

    if (num >= 100)
    {
        SLCD_Engine_Show_Num(slcd_engine, hundred_val, NUM_POS4, 0);
        SLCD_Engine_Show_Num(slcd_engine, hundred_val, NUM_POS4, 1);
    }

    if (num >= 10)
    {
        SLCD_Engine_Show_Num(slcd_engine, decimal_val, NUM_POS5, 0);
        SLCD_Engine_Show_Num(slcd_engine, decimal_val, NUM_POS5, 1);
    }

    SLCD_Engine_Show_Num(slcd_engine, single_val, NUM_POS6, 0);
    SLCD_Engine_Show_Num(slcd_engine, single_val, NUM_POS6, 1);

    return 0;
}

static int32_t slcd_guess_num(tSLCD_Engine *slcd_engine)
{
    int32_t input_num = 0;
    int32_t target_num = 0;

    while (1)
    {
        int32_t play_again = 0;
        uint32_t randout = 0;

        PRINTF("-------------- SLCD Guess Num Game --------------\r\n");
        PRINTF("The number intput and final number will be shown on SLCD.\r\n");
        PRINTF("Please check SLCD for these numbers.\r\n");
        PRINTF("Let's play:\r\n");

        RNGA_DRV_GetRandomData(RNGA_INSTANCE, &randout, sizeof(uint32_t));

        target_num = randout % 10000;

        slcd_clear_screen();

        slcd_show_game_start(slcd_engine);

        while (1)
        {
            uint8_t input_num_size = 4;
            char ch_in = 0;
            PRINTF("Please guess the number I want(0 - 9999), Press 'enter' to end: ");
            input_num = 0;
            /* Check for enter key */
            while (1)
            {
                if (('\r' == ch_in) && (input_num_size < 4))
                    break;

                ch_in = GETCHAR();
                if ((ch_in >= '0') && (ch_in <= '9') && (input_num_size > 0))
                {
                    PUTCHAR(ch_in);
                    input_num = (input_num * 10) + (ch_in - '0');
                    input_num_size--;
                }
            }
            PRINTF("\r\n");

            if ((input_num < 0) || (input_num > 9999))
            {
            	PRINTF("Input number out of range! Should be in the range of 0-9999.\r\n");
            }
            else
            {
                slcd_show_number(slcd_engine, input_num);
                if (input_num == target_num)
                {
                    ch_in = 0;

                    slcd_show_victory(slcd_engine);
                    PRINTF("Great, %d, you have GOT it!\r\n", input_num);
                    while (1)
                    {
                    	PRINTF("Play again? Input 'Y' or 'N':\r\n");
                        ch_in = GETCHAR();
                        PUTCHAR(ch_in);
                        PRINTF("\r\n");
                        if ((ch_in == 'Y') || (ch_in == 'y'))
                        {
                            play_again = 1;
                            break;
                        }
                        else if ((ch_in == 'N') || (ch_in == 'n'))
                        {
                            play_again = 0;
                            break;
                        }
                        else
                        {
                        	PRINTF("Wrong input!Please input again.\r\n");
                            continue;
                        }
                    }
                    break;
                }
                else
                {
                	PRINTF("The input number %d is %s than what I want. Guess again!\r\n",
                           input_num, (input_num > target_num) ? "bigger" : "smaller");
                }
            }
        }

        if (play_again == 0)
        {
            slcd_clear_screen();
            PRINTF("Bye bye!\r\n");
            break;
        }
    }

    return 0;
}

int main(void)
{
    tSLCD_Engine slcd_engine;

    /* Init hardware. */
    hardware_init();

    OSA_Init();

    rnga_hw_init();

    slcd_hw_init();

    memset(&slcd_engine, 0, sizeof(tSLCD_Engine));
    SLCD_Engine_Init(&slcd_engine, slcd_set_reg);

    slcd_basic_test(&slcd_engine);

    slcd_guess_num(&slcd_engine);

    while (1)
    {
    }
}

///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
