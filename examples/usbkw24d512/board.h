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

#if !defined(__BOARD_H__)
#define __BOARD_H__

#include <stdint.h>
#include "pin_mux.h"
#include "gpio_pins.h"

/* The board name */
#define BOARD_NAME                      "USB-KW24D512"

#define CLOCK_VLPR 1U
#define CLOCK_RUN  2U
#define CLOCK_NUMBER_OF_CONFIGURATIONS 3U

#ifndef CLOCK_INIT_CONFIG
#define CLOCK_INIT_CONFIG CLOCK_RUN
#endif

/* OSC0 configuration. */
#define OSC0_XTAL_FREQ 4000000U
#define OSC0_SC2P_ENABLE_CONFIG  false
#define OSC0_SC4P_ENABLE_CONFIG  false
#define OSC0_SC8P_ENABLE_CONFIG  false
#define OSC0_SC16P_ENABLE_CONFIG false
#define MCG_HGO0   kOscGainLow
#define MCG_RANGE0 kOscRangeVeryHigh
#define MCG_EREFS0 kOscSrcOsc

/* EXTAL0 PTA18 */
#define EXTAL0_PORT   PORTA
#define EXTAL0_PIN    18
#define EXTAL0_PINMUX kPortPinDisabled

/* XTAL0 PTA19 */
#define XTAL0_PORT   PORTA
#define XTAL0_PIN    19
#define XTAL0_PINMUX kPortPinDisabled

/* RTC external clock configuration. */
#define RTC_XTAL_FREQ   32768U
#define RTC_SC2P_ENABLE_CONFIG       false
#define RTC_SC4P_ENABLE_CONFIG       false
#define RTC_SC8P_ENABLE_CONFIG       false
#define RTC_SC16P_ENABLE_CONFIG      false
#define RTC_OSC_ENABLE_CONFIG        true
#define RTC_CLK_OUTPUT_ENABLE_CONFIG true

#define BOARD_RTC_CLK_FREQUENCY     32768U;
/* The USB_CDC to use for debug messages. */
#ifndef BOARD_DEBUG_USBCDC_INSTANCE
    #define BOARD_DEBUG_USBCDC_INSTANCE   1
#endif
#ifndef BOARD_DEBUG_USBCDC_BAUD
    #define BOARD_DEBUG_USBCDC_BAUD       115200
#endif

/* Board does not have UART output port but still need the definitions to pass the build */
#ifdef FSL_RTOS_MQX
/* The UART to use for debug messages. */
#ifndef BOARD_DEBUG_UART_INSTANCE
    #define BOARD_DEBUG_UART_INSTANCE   2
    #define BOARD_DEBUG_UART_BASEADDR   UART2_BASE
#endif
#ifndef BOARD_DEBUG_UART_BAUD
    #define BOARD_DEBUG_UART_BAUD       115200
#endif
#define BOARD_USE_UART
#endif

#define BOARD_USE_DSPI                  (1)

#define BOARD_SW_GPIO               kGpioSW1
#define BOARD_SW_IRQ_NUM            PORTC_IRQn
#define BOARD_SW_IRQ_HANDLER        PORTC_IRQHandler

/* The instances of peripherals used for dac_adc_demo */
#define BOARD_DAC_DEMO_DAC_INSTANCE     0U
#define BOARD_DAC_DEMO_ADC_INSTANCE     0U
#define BOARD_DAC_DEMO_ADC_CHANNEL      23U

/* Board does not have UART output port but still need the definitions to pass the build */
#ifdef FSL_RTOS_MQX
/* The UART to use for debug messages. */
#ifndef BOARD_DEBUG_UART_INSTANCE
    #define BOARD_DEBUG_UART_INSTANCE   2
    #define BOARD_DEBUG_UART_BASEADDR   UART2_BASE
#endif
#ifndef BOARD_DEBUG_UART_BAUD
    #define BOARD_DEBUG_UART_BAUD       115200
#endif
#define BOARD_USE_UART
#endif

/* The Flextimer instance/channel used for board */
#define BOARD_FTM_INSTANCE              0
#define BOARD_FTM_CHANNEL               4

// All LEDs are BLUE LEDs
#define BOARD_GPIO_LED_RED              kGpioLED1
#define BOARD_GPIO_LED_GREEN            kGpioLED2

#define LED1_EN (PORT_HAL_SetMuxMode(PORTD, 4, kPortMuxAsGpio))    /*!< Enable target LED0 */
#define LED2_EN (PORT_HAL_SetMuxMode(PORTD, 5, kPortMuxAsGpio))    /*!< Enable target LED1 */

#define LED1_DIS (PORT_HAL_SetMuxMode(PORTD, 4, kPortPinDisabled)) /*!< Disable target LED1 */
#define LED2_DIS (PORT_HAL_SetMuxMode(PORTD, 5, kPortPinDisabled)) /*!< Disable target LED2 */

#define LED1_OFF (GPIO_DRV_WritePinOutput(ledPins[0].pinName, 1))       /*!< Turn off target LED0 */
#define LED2_OFF (GPIO_DRV_WritePinOutput(ledPins[1].pinName, 1))       /*!< Turn off target LED1 */

#define LED1_ON (GPIO_DRV_WritePinOutput(ledPins[0].pinName, 0))        /*!< Turn on target LED0 */
#define LED2_ON (GPIO_DRV_WritePinOutput(ledPins[1].pinName, 0))        /*!< Turn on target LED1 */

#define LED1_TOGGLE (GPIO_DRV_TogglePinOutput(ledPins[0].pinName))        /*!< Toggle on target LED1 */
#define LED2_TOGGLE (GPIO_DRV_TogglePinOutput(ledPins[1].pinName))        /*!< Toggle on target LED2 */

#define OFF_ALL_LEDS  \
                           LED1_OFF;\
                           LED2_OFF;\

/* The rtc instance used for rtc_func */
#define BOARD_RTC_FUNC_INSTANCE         0

/* ConnSw configuration */
#define gXcvrSpiInstance_c              1

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void hardware_init(void);
void dbg_uart_init(void);
/* Function to initialize clock base on board configuration. */
void BOARD_ClockInit(void);

/* Function to initialize OSC0 base on board configuration. */
void BOARD_InitOsc0(void);

/* Function to initialize RTC external clock base on board configuration. */
void BOARD_InitRtcOsc(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* __BOARD_H__ */
