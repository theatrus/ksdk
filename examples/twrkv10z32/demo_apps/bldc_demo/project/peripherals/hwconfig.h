/*****************************************************************************
 * (c) Copyright 2010, Freescale Semiconductor Inc.
 * ALL RIGHTS RESERVED.
 ***************************************************************************//*!
 * @file      hwconfig.h
 * @author    R55013
 * @version   1.0.3.0
 * @date      Nov-27-2013
 * @brief     H/W configuration.
 ******************************************************************************/
#ifndef __HWCONFIG_H
#define __HWCONFIG_H

/******************************************************************************
* Constants
******************************************************************************/
/* 
   [powerstage signal]    [elevator signal]   [cpu signal]
    I_SENSE_DCB               AN6               ADC0_SE4/PTE21
    BEMF_SENSE_A/B/C          AN0/1/2           ADC0_SE6=PTE18 / ADC0_SE9|PTB1 / ADC0_SE7|PTE19
    V_SENSE_DCB               AN4               ADC0_SE5/PTE17
*/

#define ADC_CHANNEL_DCBI        4
#define ADC_CHANNEL_DCBV        5
#define ADC_CHANNEL_PHASEA      6
#define ADC_CHANNEL_PHASEB      9
#define ADC_CHANNEL_PHASEC      7
#define ADC_CHANNEL_DISABLED    31

#define ISR_PRIORITY_ADC0       1       // zero-cross, sensorless control
#define ISR_PRIORITY_SLOW_TIMER 3       // speed control loop (low ISR priority)
#define ISR_PRIORITY_FORCED_CMT 1       // forced commutation (when missed sensorless cmt, open loop, timing)
#define ISR_PRIORITY_PDB0       1       // PDB trigger error clearing

#define CPU_CLOCK               72000000UL
#define BUS_CLOCK               (CPU_CLOCK / 3)
#define TIMER_FREQUENCY         (CPU_CLOCK / 128)       // 585.9375 kHz @ 75 MHz, 562.5 kHz @ 72 MHz
#define TIMER_1MS_CONST         (TIMER_FREQUENCY / 1000)
#define SLOW_TIMER_PERIOD       (CPU_CLOCK / 2000)      // 1 ms period (slow control timer uses divider by 2 and cpu clock)
#define PWM_MODULO              3600                    // 20 kHz = 3750 @ 75 MHz, 3600 @ 72 MHz
#define UART_BAUDRATE           9600

/******************************************************************************
* Types
******************************************************************************/
typedef struct
{
    char mask;
    char swap;
} Pwm_sChannelControl;

/******************************************************************************
* Global variables
******************************************************************************/
extern const unsigned short bldcAdcSectorCfg[8];
extern const Pwm_sChannelControl bldcCommutationTableComp[8];

/******************************************************************************/
#endif /* __HWCONFIG_H */
