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
#include "fsl_os_abstraction.h"
#include "fsl_tsi_driver.h"
#include "fsl_debug_console.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define TSI_THRESHOLD_SAMPLING      (100u)
#define TSI_INSTANCE                0

///////////////////////////////////////////////////////////////////////////////
// Code
///////////////////////////////////////////////////////////////////////////////
/*!
 * @brief TSI control a LED.
 *
 * This function demonstrates how to use touch sensor interface.
 * When touching platform, a LED is turned on.
 */
int main(void)
{
    uint16_t measureResult[BOARD_TSI_ELECTRODE_CNT];
    uint8_t tsiChn[BOARD_TSI_ELECTRODE_CNT];
    uint32_t i, j, avgUntouch = 0; /*! Average TSI value of untouched state */
    uint32_t sumUntouch=0;
    uint32_t avgMeasure;
    tsi_status_t result;
#if (FSL_FEATURE_TSI_VERSION == 2)
    // Set up the HW configuration for normal mode of TSI
    static const tsi_config_t tsiHwConfig =
    {
        .ps       = kTsiElecOscPrescaler_2div,       /*! Prescaler */
        .extchrg  = kTsiExtOscChargeCurrent_10uA,    /*! Electrode charge current */
        .refchrg  = kTsiRefOscChargeCurrent_10uA,    /*! Reference charge current */
        .nscn     = kTsiConsecutiveScansNumber_8time,/*! Number of scans. */
        .lpclks   = kTsiLowPowerInterval_100ms,      /*! Low power clock. */
        .amclks   = kTsiActiveClkSource_BusClock,    /*! Active mode clock source. */
        .ampsc    = kTsiActiveModePrescaler_8div,    /*! Active mode prescaler. */
        .lpscnitv = kTsiLowPowerInterval_100ms,      /*! Low power scan interval. */
        .thresh   = 100u,                            /*! High byte of threshold. */
        .thresl   = 200u,                            /*! Low byte of threshold. */
    };
#elif (FSL_FEATURE_TSI_VERSION == 4)
    // Set up the HW configuration for normal mode of TSI
    static const tsi_config_t tsiHwConfig =
    {
        .ps      = kTsiElecOscPrescaler_2div,        /*! Prescaler */
        .extchrg = kTsiExtOscChargeCurrent_8uA,      /*! Electrode charge current */
        .refchrg = kTsiRefOscChargeCurrent_8uA,      /*! Reference charge current */
        .nscn    = kTsiConsecutiveScansNumber_8time, /*! Number of scans. */
        .mode    = kTsiAnalogModeSel_Capacitive,     /*! TSI analog modes in a TSI instance */
        .dvolt   = kTsiOscVolRails_Dv_103,
        .thresh   = 100,                             /*! High byte of threshold. */
        .thresl   = 200,                             /*! Low byte of threshold. */
    };
#endif

    tsi_state_t myTsiState;

    // Set up the configuration of initialization structure
    const tsi_user_config_t tsiConfig =
    {
        .config        = (tsi_config_t*)&tsiHwConfig,
        .pCallBackFunc = NULL,
        .usrData       = 0,
    };

    // Get TSI channel.
    tsiChn[0] = BOARD_TSI_ELECTRODE_1;
#if (BOARD_TSI_ELECTRODE_CNT > 1)
    tsiChn[1] = BOARD_TSI_ELECTRODE_2;
#endif
#if (BOARD_TSI_ELECTRODE_CNT > 2)
    tsiChn[2] = BOARD_TSI_ELECTRODE_3;
#endif
#if (BOARD_TSI_ELECTRODE_CNT > 3)
    tsiChn[3] = BOARD_TSI_ELECTRODE_4;
#endif

    // Initialize hardware
    hardware_init();

    // Initialize the OS abstraction layer
    OSA_Init();

    // Init LED1 & turn off it.
    LED1_EN;
    LED1_OFF;

    PRINTF("\r\n Touch Sensing input example ");

    // Driver initialization
    result = TSI_DRV_Init(TSI_INSTANCE, &myTsiState, &tsiConfig);
    if(result != kStatus_TSI_Success)
    {
        PRINTF("\r\nThe initialization of TSI driver failed ");
        return -1;
    }

    // Enable electrodes for normal mode
    for(i = 0; i < BOARD_TSI_ELECTRODE_CNT; i++)
    {
        result = TSI_DRV_EnableElectrode(TSI_INSTANCE, tsiChn[i], true);
        if(result != kStatus_TSI_Success)
        {
            PRINTF("\r\nThe initialization of TSI channel %d failed \r\n", tsiChn[i]);
            return -1;
        }
    }
    // Measures average value of untouched state.
    result = TSI_DRV_MeasureBlocking(TSI_INSTANCE);
    if(result != kStatus_TSI_Success)
    {
        PRINTF("\r\nThe measure of TSI failed. ");
        return -1;
    }

    // Measures average value in untouched mode.
    for(i = 0; i<TSI_THRESHOLD_SAMPLING; i++)
    {
        for(j = 0; j < BOARD_TSI_ELECTRODE_CNT; j++)
        {
            result = TSI_DRV_GetCounter(TSI_INSTANCE, tsiChn[j], &measureResult[j]);
            if(result != kStatus_TSI_Success)
            {
                PRINTF("\r\nThe read of TSI channel %d failed.", tsiChn[j]);
                return -1;
            }
            // Calculates sum of average values.
            sumUntouch += measureResult[j];
        }
    }
    // Calculates average value afer 100 times measurement.
    avgUntouch = sumUntouch/(TSI_THRESHOLD_SAMPLING * BOARD_TSI_ELECTRODE_CNT);

    // Print a note
    PRINTF("\r\nTouching for turning led on \r\n");
    while(1)
    {
        result = TSI_DRV_MeasureBlocking(TSI_INSTANCE);
        if(result != kStatus_TSI_Success)
        {
            PRINTF("\r\nThe measure of TSI failed.");
            return -1;
        }

        // Init average measurement.
        avgMeasure = 0;
        for(i = 0; i < BOARD_TSI_ELECTRODE_CNT; i++)
        {
            result = TSI_DRV_GetCounter(TSI_INSTANCE, tsiChn[i], &measureResult[i]);
            if(result != kStatus_TSI_Success)
            {
                PRINTF("\r\nThe read of TSI channel %d failed.", tsiChn[i]);
                return -1;
            }
            avgMeasure += measureResult[i];
        }
        // Calculates average measurement.
        avgMeasure /=BOARD_TSI_ELECTRODE_CNT;

        // Check if it's the touched state.
        // Add 10 to remove noise
        if(avgMeasure > avgUntouch+10)
        {
            LED1_ON;
        }
        else
        {
            LED1_OFF;
        }
        // Measures each 100ms.
        OSA_TimeDelay(100u);
    }
}
///////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////

