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
#include <stdio.h>
// Standard C Included Files
#include "board.h"
#include "fsl_rtc_driver.h"
#include "fsl_os_abstraction.h"
#include "fsl_debug_console.h"
///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define RTC_INSTANCE      BOARD_RTC_FUNC_INSTANCE

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
volatile bool busyWait;

/*!
* @brief ISR for Alarm interrupt
*
* This function change state of busyWait.
*/
void RTC_IRQHandler(void)
{
    if (RTC_DRV_IsAlarmPending(RTC_INSTANCE))
    {
        busyWait = false;
        // Disable second interrupt
        RTC_DRV_SetAlarmIntCmd(RTC_INSTANCE, false);
    }
}
/*!
* @brief RTC in alarm mode.
*
* This function demostrates how to use RTC as an alarm clock.
*/
int main(void)
{
    uint32_t sec;
    uint32_t currSeconds;
    rtc_datetime_t date;

    // Init hardware.
    hardware_init();

    PRINTF("RTC example: set up time to wake up an alarm\r\n");

    // Init RTC
    RTC_DRV_Init(RTC_INSTANCE);
    // Set a start date time and start RT.
    date.year   = 2014U;
    date.month  = 12U;
    date.day    = 25U;
    date.hour   = 19U;
    date.minute = 0;
    date.second = 0;

    // Set RTC time to default
    RTC_DRV_SetDatetime(RTC_INSTANCE, &date);

    while (1)
    {
        busyWait = true;
        // Get date time.
        RTC_DRV_GetDatetime(RTC_INSTANCE, &date);

        // print default time
        PRINTF("Current datetime: %04hd-%02hd-%02hd %02hd:%02hd:%02hd\r\n",
                        date.year, date.month, date.day,
                        date.hour, date.minute, date.second);

        // Get alarm time from user
        sec = 0;
        PRINTF("Please input the number of second to wait for alarm \r\n");
        PRINTF("The second must be positive value\r\n");
        while (sec < 1)
        {
            SCANF("%d",&sec);
        }

        // Get current date time.
        RTC_DRV_GetDatetime(RTC_INSTANCE, &date);
        // Convert current date time to seconds
        RTC_HAL_ConvertDatetimeToSecs(&date, &currSeconds);

        // Add sec to currSeconds
        currSeconds += sec;
        // Convert sec to date type
        RTC_HAL_ConvertSecsToDatetime(&currSeconds, &date);

        // Set alarm time
        if(!RTC_DRV_SetAlarm(RTC_INSTANCE, &date, true))
        {
            PRINTF("Failed to set alarm. \r\n");
            continue;
        }
        // Print alarm time
        PRINTF("Alarm will be occured at: %04hd-%02hd-%02hd %02hd:%02hd:%02hd\r\n",
                        date.year, date.month, date.day,
                        date.hour, date.minute, date.second);

        // Wait until alarm occures
        while(busyWait)
        {}

        PRINTF("\r\n Alarm occured !!!! ");
    }

}

/*******************************************************************************
 * EOF
 *******************************************************************************/
