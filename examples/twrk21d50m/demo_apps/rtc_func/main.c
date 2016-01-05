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

// SDK Included Files
#include "fsl_rtc_driver.h"
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
//  Definitions
///////////////////////////////////////////////////////////////////////////////

static volatile uint8_t gAlarmPending = 0;
static volatile bool gSecsFlag;

///////////////////////////////////////////////////////////////////////////////
//  Variables
///////////////////////////////////////////////////////////////////////////////

static const char gStrMenu[] = "\r\n"
    "Please choose the sub demo to run:\r\n"
    "1) Get current date time.\r\n"
    "2) Set current date time.\r\n"
    "NOTE: Accuracy maybe affected by a few seconds if RTC counter clock is not 32KHz.\r\n"
    "3) Alarm trigger show.\r\n"
    "NOTE: Alarm will be delayed if RTC counter clock is not 32KHz.\r\n"
    "4) Second interrupt show (demo for 20s).\r\n"
    "NOTE: Interrupt will be delayed if RTC counter clock is not 32KHz.\r\n"
    "5) Set RTC compensation.\r\n";

static const char gStrNewline[] = "\r\n";
static const char gStrInvalid[] = "Invalid input format\r\n";

///////////////////////////////////////////////////////////////////////////////
//  Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief override the RTC IRQ handler
 */
void RTC_IRQHandler(void)
{
    if (RTC_DRV_IsAlarmPending(0))
    {
        gAlarmPending = 1;
        // disable alarm interrupt
        RTC_DRV_SetAlarmIntCmd(0, false);
    }
}

/*!
 * @brief override the RTC Second IRQ handler
 */
void RTC_Seconds_IRQHandler(void)
{
    gSecsFlag = true;
}

/*!
 * @brief set alarm command.
 *
 * This function set the alarm which will be
 * trigerred x secs later. The alarm trigger
 * will print a notification on the console.
 */
static void cmd_alarm(uint8_t offsetSec)
{
    rtc_datetime_t date;
    uint32_t seconds;

    if ((offsetSec < 1) || (offsetSec > 9))
    {
        PRINTF(gStrInvalid);
        return;
    }
    // get date time and convert to seconds
    RTC_DRV_GetDatetime(0, &date);

    // convert to sec and add offset
    RTC_HAL_ConvertDatetimeToSecs(&date, &seconds);
    seconds += offsetSec;
    RTC_HAL_ConvertSecsToDatetime(&seconds, &date);

    // set the datetime for alarm
    if (RTC_DRV_SetAlarm(0, &date, true))
    {
        /* Alarm was successfully set, wait for alarm interrupt */
        while (!gAlarmPending)
        {
         ;
        }
    }
    else
    {
        PRINTF("Failed to set alarm. Alarm time is not in the future\r\n");
        return;
    }
    // interrupt done
    RTC_DRV_GetDatetime(0, &date);
    PRINTF("Triggered Alarm: %02d:%02d:%02d\r\n",
            date.hour, date.minute, date.second);

    gAlarmPending = 0;

    // disable the alarm interrupt and clear TAF
    RTC_DRV_SetAlarm(0, &date, false);
}

/*!
 * @brief get the current date time.
 *
 * This function get the current date time
 */
static void cmd_get_datetime(void)
{
    rtc_datetime_t date;
    RTC_DRV_GetDatetime(0, &date);
    PRINTF("Current datetime: %04hd-%02hd-%02hd %02hd:%02hd:%02hd\r\n",
           date.year, date.month, date.day,
           date.hour, date.minute, date.second);
}

/*!
 * @brief run the digital clock in 20s.
 *
 * This function show the digital clock on console
 */
static void cmd_seconds(void)
{
    uint32_t count = 0;
    rtc_datetime_t date;
    char sourceBuff[] = "\r10:10:00";

    gSecsFlag = false;
    // enable Sec interrupt
    RTC_DRV_SetSecsIntCmd(0, true);

    /* If the RTC counter clock is not 32KHz, the seconds interrupt will be delayed and
     * will not occur every second. For eg: if the RTC counter clock is 1KHz, the seconds
     * interrupt will occur after 32 seconds
     */
    while (count < 20U)
    {
        // If seconds interrupt ocurred, print new time
        if (gSecsFlag)
        {
            // Build up the word
            gSecsFlag = false;
            count ++;
            RTC_DRV_GetDatetime(0, &date);

            sourceBuff[1] = ((date.hour/10)+0x30);
            sourceBuff[2] = ((date.hour%10)+0x30);
            sourceBuff[3] = (date.second & 0x01) ? ':':' ';
            sourceBuff[4] = ((date.minute/10) +0x30);
            sourceBuff[5] = ((date.minute%10) +0x30);
            sourceBuff[6] = (date.second & 0x01) ? ':':' ';
            sourceBuff[7] = ((date.second/10) +0x30);
            sourceBuff[8] = ((date.second%10) +0x30);
            // print the time
            PRINTF(sourceBuff);
        }
    }
    // disable Sec interrupt
    RTC_DRV_SetSecsIntCmd(0, false);
    PRINTF(gStrNewline);
}

/*!
 * @brief demo the compansation of RTC
 *
 * This function set the compansation value
 * and it's interval value. Demo the compansation
 * result by the RTC_CLKOUT pin.
 */
static void cmd_comp(uint32_t cycles, uint32_t interval)
{
    uint8_t value;

    if ((cycles <= 32896U) && (cycles >= 32641U) && (interval < 256U))
    {
        // set compensation interval and cycles
        value = (uint8_t)((32896U - cycles) + 0x80U);
        RTC_DRV_SetTimeCompensation(0, value, interval);
    }
    else
    {
        PRINTF(gStrInvalid);
    }
}

/*!
 * @brief receive the console input and echo
 *
 */
static void recv_from_console(char *buf, uint32_t size)
{
    uint32_t i;

    for (i = 0; i < size; i++)
    {
        buf[i] = GETCHAR();
        PUTCHAR(buf[i]);
    }
}

/*!
 * @brief main demo function.
 */
int main(void)
{
    rtc_datetime_t date;

    // init hardware and debug uart
    hardware_init();

    PRINTF("\r\nRTC Demo running...\r\n");

#if FSL_FEATURE_SIM_OPT_HAS_RTC_CLOCK_OUT_SELECTION
    // select the 1Hz for RTC_CLKOUT
    CLOCK_SYS_SetRtcOutSrc(kClockRtcoutSrc1Hz);
#endif

    RTC_DRV_Init(0);

    /* Enable the RTC Clock output */
    RTC_HAL_SetClockOutCmd(RTC_BASE_PTR, true);

    /* Need to check this here as the RTC_DRV_Init() may have issued a software reset on the
     * module clearing all prior RTC OSC related setup */
    BOARD_InitRtcOsc();

    // Set a start date time and start RTC
    date.year = 2014U;
    date.month = 4U;
    date.day = 30U;
    date.hour = 14U;
    date.minute = 0U;
    date.second = 0U;
    RTC_DRV_SetDatetime(0, &date);

    // start loop
    while (1)
    {
        uint8_t index;
        uint8_t secs;
        char recvBuf[20];
        uint32_t cycles, result;

        // print the user information
        PRINTF(gStrMenu);
        PRINTF("\r\nSelect:");
        // get user input
        index = GETCHAR();
        PUTCHAR(index);
        PRINTF(gStrNewline);

        switch (index)
        {
        case '1':
            cmd_get_datetime();
            break;
        case '2':
            PRINTF("Input date time like: \"2014-04-22 16:40:00\"\r\n");
            recv_from_console(recvBuf, 19);
            result = sscanf(recvBuf, "%04hd-%02hd-%02hd %02hd:%02hd:%02hhd",
                       &date.year, &date.month, &date.day,
                           &date.hour, &date.minute, &date.second);
            PRINTF(gStrNewline);
            if (result != 6)
            {
                PRINTF(gStrInvalid);
                break;
            }
            if (!RTC_DRV_SetDatetime(0, &date))
            {
                PRINTF(gStrInvalid);
            }
            cmd_get_datetime();
            break;
        case '3':
            PRINTF("Input the alarm seconds from now on (1s~9s):");
            secs = GETCHAR();
            PUTCHAR(secs);
            PRINTF(gStrNewline);
            secs -= '0';
            cmd_alarm(secs);
            break;
        case '4':
            cmd_seconds();
            break;
        case '5':
            PRINTF("Compensation cycles (32641-32896):");
            recv_from_console(recvBuf, 5);
            sscanf(recvBuf, "%05u", (unsigned int *)&cycles);
            PRINTF(gStrNewline);
            cmd_comp(cycles, 1);
            break;
        default:
            break;
        }
    }
}
