/*******************************************************************************
*
* Copyright [2014-]2014 Freescale Semiconductor, Inc.

*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale License
* distributed with this Material.
* See the LICENSE file distributed for more details.
* 
*
*******************************************************************************/

#include <stdio.h>

#include "board.h"

#include "fsl_interrupt_manager.h"
#include "fsl_clock_manager.h"

#include "fsl_gpio_driver.h"
#include "fsl_enc_driver.h"
#include "fsl_debug_console.h"

/* Number of pulses per round */
#define ENCODER_PULSES          1024
/* Number of Encoder count for both phase A and B */
#define ENCODER_COUNTS          ENCODER_PULSES*2
/* Number of Encoder count for each LED */
#define ENCODER_PULS_ON_LED     (uint32_t) (ENCODER_COUNTS / 6)

uint32_t ledStart = 0U;
void ENC_ISR_Index(void *arg);

int main(void)
{
    uint32_t i = 0U;
    uint32_t led = 0U;
    enc_user_config_t encConfig;
    enc_counter_t counter;

    /* Init hardware and debug console. */
    hardware_init();

    PRINTF("Welcome to Quad ENC demo!\r\n");

    for (i=0; i<6; i++)
        GPIO_DRV_OutputPinInit(&ledPins[i]);

    /* Init Clock and Pin muxing to Quadrature Encoder. */
    SIM_SCGC5 |= SIM_SCGC5_XBARA_MASK;

    /* Initialize Crossbar for Encoder. */
    XBARA_BWR_SEL22_SEL44(XBARA_BASE_PTR, 3U);
    XBARA_BWR_SEL22_SEL45(XBARA_BASE_PTR, 2U);
    XBARA_BWR_SEL23_SEL46(XBARA_BASE_PTR, 11U);
    XBARA_BWR_SEL23_SEL47(XBARA_BASE_PTR, 6U);

    /* Initialize Encoder. */
    ENC_DRV_StructInitUserConfigNormal(&encConfig);
    encConfig.reverseCounting = true;
    if (ENC_DRV_Init(0U, &encConfig) != kStatus_ENC_Success)
        return -1;

    /* Set interrupt from INDEX impulse. */
    ENC_DRV_SetIntMode(0U, kEncIntIndexPulse, true);

    /* Enable Interrupts GLOBALLY */
    INT_SYS_EnableIRQGlobal();

    /* Run ! */
    while(1)
    {
        /* Read counters. */
        ENC_DRV_ReadCounters(0U, &counter);

        /* Calc LED position. */
        if (counter.position < 0)
            counter.position = ENCODER_COUNTS + counter.position;
        led = (uint32_t) (counter.position / ENCODER_PULS_ON_LED);

        /* Wait for INDEX interrupt to align led position. */
        if (ledStart == 0U) led = 0;

        /* Switch LED according to position. */
        switch (led)
        {
            case 0:
                GPIO_DRV_ClearPinOutput(kGpioLED6);
                GPIO_DRV_SetPinOutput(kGpioLED1);
                GPIO_DRV_ClearPinOutput(kGpioLED2);
                break;
            case 1:
                GPIO_DRV_ClearPinOutput(kGpioLED1);
                GPIO_DRV_SetPinOutput(kGpioLED2);
                GPIO_DRV_ClearPinOutput(kGpioLED3);
                break;
            case 2:
                GPIO_DRV_ClearPinOutput(kGpioLED2);
                GPIO_DRV_SetPinOutput(kGpioLED3);
                GPIO_DRV_ClearPinOutput(kGpioLED4);
                break;
            case 3:
                GPIO_DRV_ClearPinOutput(kGpioLED3);
                GPIO_DRV_SetPinOutput(kGpioLED4);
                GPIO_DRV_ClearPinOutput(kGpioLED5);
                break;
            case 4:
                GPIO_DRV_ClearPinOutput(kGpioLED4);
                GPIO_DRV_SetPinOutput(kGpioLED5);
                GPIO_DRV_ClearPinOutput(kGpioLED6);
                break;
            case 5:
                GPIO_DRV_ClearPinOutput(kGpioLED5);
                GPIO_DRV_SetPinOutput(kGpioLED6);
                GPIO_DRV_ClearPinOutput(kGpioLED1);
                break;
            default: ;
                break;
        }
    }
}

void ENC_ISR_Index(void *arg)
{
    ledStart = 1U;
    ENC_DRV_SetIntMode(0U, kEncIntIndexPulse, false); 
    
    PRINTF("INDEX pulse detected!\r\n");
}

void ENC_ISR_Home(void){}

void ENC_ISR_Rollover(void){}

void ENC_ISR_Rollunder(void){}

void ENC_ISR_Compare(void){}

void ENC_ISR_Watchdog(void){}

void ENC_ISR_Simult(void){}
