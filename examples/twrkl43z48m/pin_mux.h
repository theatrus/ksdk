/*
 * Copyright (c) 2013-2014, Freescale Semiconductor, Inc.
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

#ifndef pin_mux_H_
#define pin_mux_H_

/* MODULE pin_mux. */

/*
** ===================================================================
**     Method      :  pin_mux_GPIO (component PinSettings)
*/
/*!
**     @brief
**         GPIO method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         PTA, PTB, PTC, PTD, PTE
**         peripherals.
**     @param
**         uint32_t instance - GPIO instance number 0..4
*/
/* ===================================================================*/
void configure_gpio_pins(uint32_t instance);
/*
** ===================================================================
**     Method      :  pin_mux_I2C (component PinSettings)
*/
/*!
**     @brief
**         I2C method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         I2C0, I2C1, I2C2
**         peripherals.
**     @param
**         uint32_t instance - I2C instance number 0..2
*/
/* ===================================================================*/
void configure_i2c_pins(uint32_t instance);
/*
** ===================================================================
**     Method      :  pin_mux_RTC (component PinSettings)
*/
/*!
**     @brief
**         RTC method sets registers according routing settings. Call
**         this method code to route desired pins into RTC periphery.
**     @param
**         uint32_t instance - RTC instance number (0 is expected)
*/
/* ===================================================================*/
void configure_rtc_pins(uint32_t instance);
/*
** ===================================================================
**     Method      :  pin_mux_SPI (component PinSettings)
*/
/*!
**     @brief
**         SPI method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         SPI0, SPI1, SPI2
**         peripherals.
**     @param
**         uint32_t instance - SPI instance number 0..2
*/
/* ===================================================================*/
void configure_spi_pins(uint32_t instance);
/*
** ===================================================================
**     Method      :  pin_mux_UART (component PinSettings)
*/
/*!
**     @brief
**         UART method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         UART0, UART1, UART2, UART3, UART4, UART5
**         peripherals.
**     @param
**         uint32_t instance - UART instance number 0..5
*/
/* ===================================================================*/
void configure_lpuart_pins(uint32_t instance);
void configure_uart_pins(uint32_t instance);
void configure_gpio_i2c_pins(uint32_t instance);
void configure_spi_cs0_pins(uint32_t instance);
void configure_spi_cs1_pins(uint32_t instance);
void configure_tpm_pins(uint32_t instance);
void configure_cmp_pins(uint32_t instance);
void configure_flexio_pins(uint32_t instance, uint32_t pinIdx);
void configure_smartcard_pins(uint32_t instance);
void configure_dac_pins(uint32_t instance);
/* END pin_mux. */
#endif /* #ifndef __pins_H_ */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
