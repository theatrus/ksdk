/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2004-2011 Freescale Semiconductor
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file   demonstration.c
*
* @brief  Demonstration of the demo
*
* @version 1.0.2.0
*
* @date Nov-27-2013
*
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "gdflib.h"
#include "gmclib.h"
#include "BLDC_appconfig.h"
#include "motor_structure.h"
#include "fsl_gpio_hal.h"

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

#define PUSH_TIME_FILTER                50
#define PUSH_TIME_SHORT                 250
#define PUSH_TIME_LONG                  2000
#define DEMONSTRATION_SPEED_STEP        0.1         //!< 1.0 = MAX motor speed
#define DEMONSTRATION_SPEED_RAMP_GAIN   0.004       //!< max speed ramp in demonstration mode (speed inc/dec in 1 ms in unit of max motor speed)

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////

extern MCSTRUC_BLDC_SNLS_INT_T gsM1_Drive;

static UInt16 uw16Sw1FiltCnt, uw16Sw2FiltCnt;
static UInt16 uw16DemonstrationStatus;
static UInt32 uw32DemoTime;
static UInt16 uw16SwStatus;
static UInt16 uw16Sw1Cnt, uw16Sw2Cnt, uw16SwBothCnt;

////////////////////////////////////////////////////////////////////////////////
// Prototypes
////////////////////////////////////////////////////////////////////////////////

void Demonstration(MCSTRUC_BLDC_SNLS_INT_T *);

/*!
 * @brief Show a few demo speeds and ramps, called every 1 ms
 *
 * @param sM_Drive BLDC motor variables structure pointer
 */
void Demonstration(MCSTRUC_BLDC_SNLS_INT_T * sM_Drive)
{
    Frac16 f16RequiredSpeedNew, f16RequiredSpeedOld;

    // Push buttons signal filtering
    if ((!((GPIOA->PDIR >> 4) & 1)))    // STATUS_SW1
    {
        if (uw16Sw1FiltCnt <= PUSH_TIME_FILTER) uw16Sw1FiltCnt++;
        else uw16SwStatus |= 1;
    }
    else
    {
        if (0 < uw16Sw1FiltCnt) uw16Sw1FiltCnt--;
        else uw16SwStatus &= ~1;
    }

    if ((!((GPIOB->PDIR >> 0) & 1)))    // STATUS_SW2
    {
        if (uw16Sw2FiltCnt <= PUSH_TIME_FILTER) uw16Sw2FiltCnt++;
        else uw16SwStatus |= 2;
    }
    else
    {
        if (0 < uw16Sw2FiltCnt) uw16Sw2FiltCnt--;
        else uw16SwStatus &= ~2;
    }

    // Push buttons processing
    switch (uw16SwStatus)
    {
        case 0:
            uw16Sw1Cnt = 0;
            uw16Sw2Cnt = 0;
            uw16SwBothCnt = 0;
            break;

        case 1:       // Switch 1 only
            if (0 < uw16Sw1Cnt)
            {
                uw16Sw1Cnt--;
            }
            else
            {
                uw16Sw1Cnt = PUSH_TIME_SHORT;
                // Increase speed
                if (uw16DemonstrationStatus == 0)
                {
                    f16RequiredSpeedOld = sM_Drive->f16SpeedRequired;
                    f16RequiredSpeedNew = f16RequiredSpeedOld;
                    f16RequiredSpeedNew = MLIB_AddSat_F16(f16RequiredSpeedNew, FRAC16(DEMONSTRATION_SPEED_STEP));
                    sM_Drive->f16SpeedRequired = f16RequiredSpeedNew;
                }
            }
            uw16Sw2Cnt = 0;
            uw16SwBothCnt = 0;
            break;

        case 2:       // Switch 2 only
            if (0 < uw16Sw2Cnt)
            {
                uw16Sw2Cnt--;
            }
            else
            {
                uw16Sw2Cnt = PUSH_TIME_SHORT;
                // Decrease speed
                if (uw16DemonstrationStatus == 0)
                {
                    f16RequiredSpeedOld = sM_Drive->f16SpeedRequired;
                    f16RequiredSpeedNew = f16RequiredSpeedOld;
                    f16RequiredSpeedNew = MLIB_SubSat_F16(f16RequiredSpeedNew, FRAC16(DEMONSTRATION_SPEED_STEP));
                    sM_Drive->f16SpeedRequired = f16RequiredSpeedNew;
                }
            }

            uw16Sw1Cnt = 0;
            uw16SwBothCnt = 0;
            break;

        case 3:       // Both Switches
            if (uw16SwBothCnt < 0x7fff) uw16SwBothCnt++;
            if (uw16SwBothCnt == PUSH_TIME_LONG)
            {
                if (uw16DemonstrationStatus == 0)
                {
                    uw16DemonstrationStatus = 1;
                    uw32DemoTime = 0;
                }
                else
                {
                    uw16DemonstrationStatus = 0;
                    // Set default ramp
                    sM_Drive->f32trSpeedRamp.f32RampDown = SPEED_LOOP_RAMP_DOWN;
                    sM_Drive->f32trSpeedRamp.f32RampUp   = SPEED_LOOP_RAMP_UP;
                    GPIO_HAL_SetPinOutput(GPIOD_BASE_PTR, 6);//DEMO_LED_OFF;
                }
            }
            uw16Sw1Cnt = 0;
            uw16Sw2Cnt = 0;
            break;
    }

    // Demonstration mode
    if (uw16DemonstrationStatus == 1)
    {
        uw32DemoTime++;

        if (uw32DemoTime & 128) GPIO_HAL_SetPinOutput(GPIOD_BASE_PTR, 6);   //DEMO_LED_OFF;
        else                    GPIO_HAL_ClearPinOutput(GPIOD_BASE_PTR, 6); //DEMO_LED_ON;

        switch (uw32DemoTime)
        {
            case 500:
                sM_Drive->f32trSpeedRamp.f32RampDown = FRAC32(0.25 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f32trSpeedRamp.f32RampUp   = FRAC32(0.25 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f16SpeedRequired = FRAC16(0.6);
                break;

            case 3000:
                sM_Drive->f32trSpeedRamp.f32RampDown = FRAC32(1.0 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f32trSpeedRamp.f32RampUp   = FRAC32(1.0 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f16SpeedRequired = FRAC16(-0.8);
                break;

            case 5500:
                sM_Drive->f16SpeedRequired = FRAC16(0.8);
                break;

            case 7500:
                sM_Drive->f16SpeedRequired = FRAC16(0.2);
                break;

            case 11000:
                sM_Drive->f16SpeedRequired = FRAC16(0.8);
                break;

            case 13000:
                sM_Drive->f16SpeedRequired = FRAC16(0.4);
                break;

            case 15000:
                sM_Drive->f32trSpeedRamp.f32RampDown = FRAC32(0.025 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f32trSpeedRamp.f32RampUp   = FRAC32(0.025 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f16SpeedRequired = FRAC16(0.1);
                break;

            case 25000:
                sM_Drive->f32trSpeedRamp.f32RampDown = FRAC32(0.0125 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f32trSpeedRamp.f32RampUp   = FRAC32(0.0125 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f16SpeedRequired = FRAC16(0.8);
                break;

            case 28000:
                sM_Drive->f32trSpeedRamp.f32RampDown = FRAC32(0.0625 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f32trSpeedRamp.f32RampUp   = FRAC32(0.0625 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f16SpeedRequired = FRAC16(-0.8);
                break;

            case 30500:
                sM_Drive->f32trSpeedRamp.f32RampDown = FRAC32(0.125 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f32trSpeedRamp.f32RampUp   = FRAC32(0.125 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f16SpeedRequired = FRAC16(0.8);
                break;

            case 34000:
                sM_Drive->f16SpeedRequired = FRAC16(0.1);
                break;

            case 50000:
                sM_Drive->f32trSpeedRamp.f32RampDown = FRAC32(0.25 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f32trSpeedRamp.f32RampUp   = FRAC32(0.25 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f16SpeedRequired = FRAC16(-0.18);
                break;

            case 54000:
                sM_Drive->f16SpeedRequired = FRAC16(-0.22);
                break;

            case 58000:
                sM_Drive->f16SpeedRequired = FRAC16(-0.18);
                break;

            case 62000:
                sM_Drive->f16SpeedRequired = FRAC16(-0.2);
                break;

            case 66000:
                sM_Drive->f16SpeedRequired = FRAC16(-0.22);
                break;

            case 70000:
                sM_Drive->f16SpeedRequired = FRAC16(-0.2);
                break;

            case 74000:
                sM_Drive->f32trSpeedRamp.f32RampDown = FRAC32(1.0 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f32trSpeedRamp.f32RampUp   = FRAC32(1.0 * DEMONSTRATION_SPEED_RAMP_GAIN);
                sM_Drive->f16SpeedRequired = FRAC16(-0.18);
                break;

            case 78000:
                sM_Drive->f16SpeedRequired = FRAC16(-0.2);
                break;

            case 85000:
                sM_Drive->f16SpeedRequired = FRAC16(-0.1);
                break;

            case 90000:
                sM_Drive->f16SpeedRequired = FRAC16(-0.8);
                break;

            case 92000:
                sM_Drive->f16SpeedRequired = FRAC16(-0.2);
                break;
            case 95000:
                sM_Drive->f16SpeedRequired = FRAC16(-0.8);
                break;

            case 98000:
                sM_Drive->f16SpeedRequired = FRAC16(-0.2);
                break;

            case 108000:
                uw32DemoTime = 0;
                break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
