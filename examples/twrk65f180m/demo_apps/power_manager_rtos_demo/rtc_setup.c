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
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

// SDK Included Files
#include "board.h"
#include "fsl_clock_manager.h"
#include "lpm_rtos.h"
#include "fsl_debug_console.h"
#if (defined PM_RTOS_DEMO_RTC_FUNC_INSTANCE)
#include "fsl_rtc_driver.h"

// Project Included Files
#include "rtc_setup.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Structures & enumerations
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief RTC init.
 * @param uint32_t instance     : RTC instance
 */
void rtcInit(uint32_t instance, rtc_datetime_t *date)
{
    // select the 1Hz for RTC_CLKOUT
    CLOCK_SYS_SetRtcOutSrc(kClockRtcoutSrc1Hz);

    RTC_DRV_Init(instance);

    RTC_DRV_SetDatetime(instance, date);
}

/*!
 * @brief RTC setting up alarm.
 * @param uint32_t instance     : RTC instance
 * @param uint8_t offsetSec     : offset in seconds to call interrupt.
 */
void rtcSetAlarm(uint32_t instance, uint8_t offsetSec)
{
    uint32_t seconds;
    rtc_datetime_t date;

    if ((offsetSec < 1) || (offsetSec > 60))
    {
        offsetSec = 5;
    }
    // get date time and convert to seconds
    RTC_DRV_GetDatetime(instance, &date);
    // convert to sec and add offset
    RTC_HAL_ConvertDatetimeToSecs(&date, &seconds);
    seconds += offsetSec;
    RTC_HAL_ConvertSecsToDatetime(&seconds, &date);

    // set the date time for alarm
    if (RTC_DRV_SetAlarm(instance, &date, true))
    {
    }
    else
    {
        PRINTF("Failed to set alarm. Alarm time is not in the future\r\n");
        return;
    }
}
#endif
///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////
