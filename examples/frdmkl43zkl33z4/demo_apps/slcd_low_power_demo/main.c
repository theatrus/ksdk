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
#include "fsl_slcd_driver.h"
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "s401m16kr.h"
#include "fsl_power_manager.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define DEMO_POWER_MODE     POWER_MODE_VLPS
#define RTC_INSTANCE        BOARD_RTC_FUNC_INSTANCE
#define SLCD_INSTANCE       (0)
#define POWER_MODE_VLPS     (0)
#define POWER_MODE_LLS      (1)

///////////////////////////////////////////////////////////////////////////////
// Prototypes
///////////////////////////////////////////////////////////////////////////////
power_manager_error_code_t callback0(power_manager_notify_struct_t * notify,
                                     power_manager_callback_data_t * dataPtr);

///////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////
static power_manager_user_config_t vlpsConfig;
static power_manager_user_config_t vllsConfig;
static power_manager_callback_user_config_t  callbackCfg0 = { callback0,  kPowerManagerCallbackBeforeAfter,  NULL };
static power_manager_callback_user_config_t * callbacks[] = { &callbackCfg0 };
static const power_manager_user_config_t *powerConfigs[] =
{
    &vlpsConfig,
    &vllsConfig,
};
///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////

/*!
 * @brief init slcd
 */
static void slcd_init(void)
{
    slcd_work_mode_t lowPowerMode =
    {
        .kSLCDEnableInDozeMode = false,
        .kSLCDEnableInStopMode = false,
    };
    slcd_user_config_t init =
    {
        .powerSupply     = kSLCDPowerInternalVll3AndChargePump,
        .loadAdjust      = kSLCDHighLoadOrSlowestClkSrc,
        .dutyCyc         = kSLCD1Div4DutyCyc,
        .trim            = kSLCDRegulatedVolatgeTrim08,
        .workMode        = lowPowerMode,
    };

    slcd_pins_config_t pinsConfig =
    {
        .slcdLowPinsEnabled           = (1<<14)|(1<<15)|(1<<20)|(1<<24)|(1<<26)|(1<<27),
        .slcdHighPinsEnabled          = (1<<(59-32))|(1<<(60-32))|(1<<(40-32))|(1<<(42-32))|(1<<(43-32))|(1<<(44-32)),
        .slcdBackPlaneLowPinsEnabled  = (1<<14)|(1<<15),
        .slcdBackPlaneHighPinsEnabled = (1<<(59-32))|(1<<(60-32)),
    };

    SLCD_DRV_Init(SLCD_INSTANCE, &init);

    SLCD_DRV_SetAllPinsConfig(SLCD_INSTANCE, &pinsConfig);

    SLCD_DRV_SetBackPlanePhase(SLCD_INSTANCE, 59, kSLCDPhaseA);
    SLCD_DRV_SetBackPlanePhase(SLCD_INSTANCE, 60, kSLCDPhaseB);
    SLCD_DRV_SetBackPlanePhase(SLCD_INSTANCE, 14, kSLCDPhaseC);
    SLCD_DRV_SetBackPlanePhase(SLCD_INSTANCE, 15, kSLCDPhaseD);

    SLCD_DRV_Start(SLCD_INSTANCE);
}

/*!
 * @brief Power manager user callback
 */
power_manager_error_code_t callback0(power_manager_notify_struct_t * notify,
                                     power_manager_callback_data_t * dataPtr)
{
    return kPowerManagerSuccess;
}

/*!
 * @brief init power manager system
 */
static void low_power_init(void)
{
    vlpsConfig.mode = kPowerManagerVlps;
    vllsConfig.mode = kPowerManagerLls;
    POWER_SYS_Init(powerConfigs , 2, callbacks, 1);
}

/*!
 * @brief slcd_low_power demo.
 */
int main(void)
{
    rtc_datetime_t date;

    OSA_Init();
    hardware_init();
    PRINTF("slcd_rtc_demo\r\n");

    if((RCM_SRS0 & RCM_SRS0_WAKEUP_MASK) !=  0)
    {
        PRINTF("Wakeup from VLLSx mode\r\n");
    }

    // RTC init
    RTC_DRV_Init(RTC_INSTANCE);

    /* Need to check this here as the RTC_DRV_Init() may have issued a software reset on the
     * module clearing all prior RTC OSC related setup */
    BOARD_InitRtcOsc();

    date.year   = 2014U;
    date.month  = 12U;
    date.day    = 25U;
    date.hour   = 19U;
    date.minute = 0;
    date.second = 0;
    RTC_DRV_SetDatetime(0, &date);

    slcd_init();
    low_power_init();

    // open RTC trigger source
    LLWU_HAL_SetInternalModuleCmd(LLWU, kLlwuWakeupModule7, true);

    RTC_DRV_SetSecsIntCmd(RTC_INSTANCE, true);
#if DEMO_POWER_MODE == POWER_MODE_VLPS
    PRINTF("Will enter power_mode:%d VLPS\r\n", DEMO_POWER_MODE);
#else
    PRINTF("Will enter power_mode:%d LLS\r\n", DEMO_POWER_MODE);
#endif
    OSA_TimeDelay(10);
    while(1)
    {
        POWER_SYS_SetMode(DEMO_POWER_MODE, kPowerManagerPolicyForcible);
        OSA_TimeDelay(1);
    }
}

/*!
 * @brief RTC second interrupt
 */
void RTC_Seconds_IRQHandler(void)
{
    rtc_datetime_t date;
    RTC_DRV_GetDatetime(RTC_INSTANCE, &date);
//    PRINTF("%02d-%02d-%02d\r\n", date.hour, date.minute, date.second);

    SLCD_DispNum(0, date.minute/10);
    SLCD_DispNum(1, date.minute%10);
    SLCD_DispNum(2, date.second/10);
    SLCD_DispNum(3, date.second%10);
    SLCD_DispDot(3, (date.second & 0x01));
}

/*******************************************************************************
 * EOF
 *******************************************************************************/
