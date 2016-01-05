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

#include <stdio.h>
#include "board.h"
#include "fsl_irtc_driver.h"
#include "fsl_debug_console.h"
#include "fsl_os_abstraction.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
volatile static bool alarmHappen = false;
void alarmISR(void);
/*******************************************************************************
 * Code
 ******************************************************************************/
int main(void)
{
    irtc_datetime_t datetime, alarmDatetime, datetimeGet;

    datetime.year = 2015;
    datetime.month = 1;
    datetime.day = 21;
    datetime.weekDay = 3;
    datetime.hour = 18;
    datetime.minute = 55;
    datetime.second = 30;

    alarmDatetime.year = 2015;
    alarmDatetime.month = 1;
    alarmDatetime.day = 21;
    alarmDatetime.hour = 18;
    alarmDatetime.minute = 55;
    alarmDatetime.second = 33;
    
    hardware_init();
    
    OSA_Init();    

    PRINTF("RTC Example START:\r\n");
    IRTC_DRV_Init(0, &datetime, &alarmDatetime,kIRTCSecMinHourDayMonYear, NULL);
    IRTC_DRV_GetAlarm(0, &datetimeGet);
    PRINTF("\r\nAlarm Time is %d/%d/%d %d:%d:%2d\r\n", datetimeGet.year,
                                datetimeGet.month,
                                datetimeGet.day,
                                datetimeGet.hour,
                                datetimeGet.minute,
                                datetimeGet.second);

    while (1)
    {
        if (!alarmHappen)
        {
            IRTC_DRV_GetDatetime(0, &datetimeGet);
            PRINTF("\r\nCurrent Time is %d/%d/%d %d:%d:%2d", datetimeGet.year,
                                            datetimeGet.month,
                                            datetimeGet.day,
                                            datetimeGet.hour,
                                            datetimeGet.minute,
                                            datetimeGet.second);
            /* Delay 1s to shorten print log */
            OSA_TimeDelay(1000); 
        }
        else
        {
            PRINTF("\r\nRing, ring, ring");
            IRTC_DRV_GetDatetime(0, &datetimeGet);
            PRINTF("\r\nCurrent Time is %d/%d/%d %d:%d:%2d\r\n", datetimeGet.year,
                                            datetimeGet.month,
                                            datetimeGet.day,
                                            datetimeGet.hour,
                                            datetimeGet.minute,
                                            datetimeGet.second);
            PRINTF("\r\nRTC Example END.\r\n");
            break;
        }
    }

    for(;;) {}
}

/*! @brief ISR for irtc alarm */
void alarmISR(void)
{
    if(IRTC_DRV_GetIntStatusFlag(0,kIRTCAlarmIntFlag))
    {
        alarmHappen = true;
        IRTC_DRV_ClearIntStatusFlag(0, kIRTCAlarmIntFlag);
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
