/* ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename    : hardware_init.h
**     Project     : SDK
**     Processor   : MK22FN512VDC12
**     Component   : PinSettings
**     Version     : Component 01.002, Driver 1.1, CPU db: 3.00.000
**     Compiler    : IAR ARM C Compiler
**     Date/Time   : 2013-12-17, 13:31, # CodeGen: 1
**     Abstract    :
**
**     Settings    :
**
**     Contents    :
**         GPIO                - void hardware_init_GPIO(uint32_t instance);
**         I2C                 - void hardware_init_I2C(uint32_t instance);
**         I2S                 - void hardware_init_I2S(uint32_t instance);
**         UART                - void hardware_init_UART(uint32_t instance);
**
**     Copyright : 1997 - 2013 Freescale Semiconductor, Inc. All Rights Reserved.
**     SOURCE DISTRIBUTION PERMISSIBLE as directed in End User License Agreement.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/
/*!
** @file hardware_init.h
** @version 1.1
** @brief
**
*/         
/*!
**  @addtogroup hardware_init_module hardware_init module documentation
**  @{
*/         

#ifndef pin_mux_H_
#define pin_mux_H_

/* MODULE pin_mux. */


/*
** ===================================================================
**     Method      :  configure_gpio_pins (component PinSettings)
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
**     Method      :  configure_i2c_pins (component PinSettings)
*/
/*!
**     @brief
**         I2C method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         I2C0, I2C1
**         peripherals.
**     @param
**         uint32_t instance - I2C instance number 0..1
*/
/* ===================================================================*/
void configure_i2c_pins(uint32_t instance);
/*
** ===================================================================
**     Method      :  configure_i2s_pins (component PinSettings)
*/
/*!
**     @brief
**         I2S method sets registers according routing settings. Call
**         this method code to route desired pins into I2S0 periphery.
**     @param
**         uint32_t instance - I2S instance number (0 is expected)
*/
/* ===================================================================*/
void configure_i2s_pins(uint32_t instance);
/*
** ===================================================================
**     Method      :  configure_uart_pins (component PinSettings)
*/
/*!
**     @brief
**         UART method sets registers according routing settings. Call
**         this method code to route desired pins into:
**         UART0, UART1, UART2, UART3
**         peripherals.
**     @param
**         uint32_t instance - UART instance number 0..3
*/
/* ===================================================================*/
void configure_uart_pins(uint32_t instance);

void configure_spi_cs0_pins(uint32_t instance);

void configure_spi_cs1_pins(uint32_t instance);

void configure_gpio_pins(uint32_t instance);

void configure_spi_pins(uint32_t instance);

void configure_ftm_pins(uint32_t instance);

void configure_gpio_i2c_pins(uint32_t instance);

void configure_gpio_uart_pins(uint32_t instance);

void configure_rtc_pins(uint32_t instance);

void configure_ftm_pins(uint32_t instance);

void configure_cmp_pins(uint32_t instance);

void configure_sdcard_spi_pins(uint32_t instance);

void configure_smartcard_pins(uint32_t instance);

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
