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

/* MODULE pin_mux. */

#include "fsl_device_registers.h"
#include "fsl_port_hal.h"
#include "pin_mux.h"


void configure_gpio_pins(uint32_t instance)
{
  switch(instance) {
    case 0:
      /* PORTA_PCR12 */                 /* PTA */
      PORT_HAL_SetMuxMode(PORTA, 4u,kPortMuxAsGpio);
      /* PORTA_PCR13 */
      PORT_HAL_SetMuxMode(PORTA, 5u,kPortMuxAsGpio);
      /* PORTA_PCR12 */                 /* PTA */
      PORT_HAL_SetMuxMode(PORTA, 12u,kPortMuxAsGpio);
      /* PORTA_PCR13 */
      PORT_HAL_SetMuxMode(PORTA, 13u,kPortMuxAsGpio);
      break;

    case 1:                             /* PTB */
      /* PORTB_PCR0 */
      PORT_HAL_SetMuxMode(PORTB, 0u,kPortMuxAsGpio);
      /* PORTB_PCR19 */
      PORT_HAL_SetMuxMode(PORTB, 19u,kPortMuxAsGpio);
      break;

    case 2:                             /* PTC */
      break;

    case 3:                             /* PTD */
      /* PORTD_PCR6 */
      PORT_HAL_SetMuxMode(PORTD,6u,kPortMuxAsGpio);
      /* PORTD_PCR7 */
      PORT_HAL_SetMuxMode(PORTD,6u,kPortMuxAsGpio);
      break;

    case 4:                             /* PTE */
      break;

    default:
      break;
  }
}

void configure_i2c_pins(uint32_t instance)
{
  switch(instance) {
    case 0:                             /* I2C0 */
      /* PORTE_PCR24 */
      PORT_HAL_SetMuxMode(PORTE,24u,kPortMuxAlt5);
      /* PORTE_PCR25 */
      PORT_HAL_SetMuxMode(PORTE,25u,kPortMuxAlt5);
      break;

    case 1:                             /* I2C1 */
      /* PORTE_PCR0 */
      PORT_HAL_SetMuxMode(PORTE,0u,kPortMuxAlt6);
      /* PORTE_PCR1 */
      PORT_HAL_SetMuxMode(PORTE,1u,kPortMuxAlt6);
      break;

    default:
      break;
  }
}

void configure_rtc_pins(uint32_t instance)
{
  /* PORTE_PCR0 */
  PORT_HAL_SetMuxMode(PORTE,0u,kPortMuxAlt4);
}

void configure_spi_pins(uint32_t instance)
{
  switch(instance) {
    case 0:                             /* SPI0 */
      /* PORTD_PCR0 */
      PORT_HAL_SetMuxMode(PORTD,0u,kPortMuxAlt2);
      /* PORTD_PCR1 */
      PORT_HAL_SetMuxMode(PORTD,1u,kPortMuxAlt2);
      /* PORTD_PCR2 */
      PORT_HAL_SetMuxMode(PORTD,2u,kPortMuxAlt2);
      /* PORTD_PCR3 */
      PORT_HAL_SetMuxMode(PORTD,3u,kPortMuxAlt2);
      break;

    case 1:                             /* SPI1 */
      /* PORTD_PCR4 */
      PORT_HAL_SetMuxMode(PORTD,4u,kPortMuxAlt2);
      /* PORTD_PCR5 */
      PORT_HAL_SetMuxMode(PORTD,5u,kPortMuxAlt2);
      /* PORTD_PCR6 */
      PORT_HAL_SetMuxMode(PORTD,6u,kPortMuxAlt2);
      /* PORTD_PCR7 */
      PORT_HAL_SetMuxMode(PORTD,7u,kPortMuxAlt2);
      break;

    default:
      break;
  }
}

void configure_lpuart_pins(uint32_t instance)
{
  switch(instance) {
    case 0:                             /* LPUART0 */
      /* PORTA_PCR1 */
      PORT_HAL_SetMuxMode(PORTA,1u,kPortMuxAlt2);
      /* PORTA_PCR2 */
      PORT_HAL_SetMuxMode(PORTA,2u,kPortMuxAlt2);
      break;
    case 1:                             /* LPUART1 */
      /* PORTC_PCR3 */
      PORT_HAL_SetMuxMode(PORTC,3u,kPortMuxAlt3);
      /* PORTC_PCR4 */
      PORT_HAL_SetMuxMode(PORTC,4u,kPortMuxAlt3);
      break;
    default:
      break;
  }
}


void configure_uart_pins(uint32_t instance)
{
  switch(instance) {
    case 2:                             /* UART2 = UART1_ELEV */
      /* PORTE_PCR22 */
      PORT_HAL_SetMuxMode(PORTE,22u,kPortMuxAlt4);
      /* PORTE_PCR23 */
      PORT_HAL_SetMuxMode(PORTE,23u,kPortMuxAlt4);
      break;
    default:
      break;
  }
}

void configure_gpio_i2c_pins(uint32_t instance)
{
  PORT_HAL_SetMuxMode(PORTE,0u,kPortMuxAsGpio);
  PORT_HAL_SetMuxMode(PORTE,1u,kPortMuxAsGpio);
}

void configure_spi_cs0_pins(uint32_t instance)
{
  PORT_HAL_SetMuxMode(PORTD,0u,kPortMuxAsGpio); /* PTD0 */
}

void configure_spi_cs1_pins(uint32_t instance)
{
  PORT_HAL_SetMuxMode(PORTC,4u,kPortMuxAsGpio); /* PTC4 */
}

void configure_tpm_pins(uint32_t instance)
{
  switch(instance) {
    case 0:                             /* TPM0 */
        /* PTC1 TPM0 channel 0 */
        PORT_HAL_SetMuxMode(PORTA, 13u,kPortMuxAlt3);
      break;
    default:
      break;
  }
}

void configure_cmp_pins(uint32_t instance)
{
  switch(instance) {
    case 0:                             /* CMP0 */
        /* PTE29 CMP0 input channel 5 */
        PORT_HAL_SetMuxMode(PORTE,29u,kPortPinDisabled);
        /* PTE0 CMP0 output */
        PORT_HAL_SetMuxMode(PORTE, 0u,kPortMuxAlt5); /* PTE0*/
        break;
    default:
        break;
    }
}

void configure_flexio_pins(uint32_t instance, uint32_t pinIdx)
{
  switch(instance) {
    case 0:
        switch(pinIdx)
				{
          case 0:
            PORT_HAL_SetMuxMode(PORTD,0u,kPortMuxAlt6);
            break;
          case 1:
            PORT_HAL_SetMuxMode(PORTD,1u,kPortMuxAlt6);
            break;
          case 2:
            PORT_HAL_SetMuxMode(PORTD,2u,kPortMuxAlt6);
            break;
          case 3:
            PORT_HAL_SetMuxMode(PORTD,3u,kPortMuxAlt6);
            break;
          case 4:
            PORT_HAL_SetMuxMode(PORTD,4u,kPortMuxAlt6);
            break;
          case 5:
            PORT_HAL_SetMuxMode(PORTD,5u,kPortMuxAlt6);
            break;
          case 6:
            PORT_HAL_SetMuxMode(PORTD,6u,kPortMuxAlt6);
            break;
          case 7:
            PORT_HAL_SetMuxMode(PORTD,7u,kPortMuxAlt6);
            break;
        default:
          break;
			  }
    default:
      break;
  }
}

/* Setup all smartcard interface pins */
void configure_smartcard_pins(uint32_t instance)
{
    /* Card Presense Detect - PTE30 */
    PORT_HAL_SetMuxMode(PORTE,30u,kPortMuxAsGpio);

    /* UART2 */
    /* PORTE_PCR22 - IO*/
    PORT_HAL_SetMuxMode(PORTE,22u,kPortMuxAlt4);
    PORT_HAL_SetPullMode(PORTE,22u,kPortPullUp);
    PORT_HAL_SetPullCmd(PORTE,22u, true);

    /* TPM,A40*/
    /* - Card clock */
    PORT_HAL_SetMuxMode(PORTE,24u,kPortMuxAlt3);
    PORT_HAL_SetDriveStrengthMode(PORTE, 24u, kPortHighDriveStrength);


    /* PHY/Interface Control */
    /* Reset - A38,PTE29*/
    PORT_HAL_SetMuxMode(PORTE,29u,kPortMuxAsGpio);
    PORT_HAL_SetDriveStrengthMode(PORTE, 29u, kPortHighDriveStrength);

    /*Vsel0:B23,PTA5 and Vsel1:A9,PTE2*/
    PORT_HAL_SetMuxMode(PORTB,2u,kPortMuxAsGpio);
    PORT_HAL_SetDriveStrengthMode(PORTB, 2u, kPortHighDriveStrength);
    PORT_HAL_SetMuxMode(PORTB,19u,kPortMuxAsGpio);
    PORT_HAL_SetDriveStrengthMode(PORTB, 19u, kPortHighDriveStrength);

    /*INT pin:B60,PTE24*/
    PORT_HAL_SetMuxMode(PORTC,1u,kPortMuxAsGpio);
    PORT_HAL_SetDriveStrengthMode(PORTC, 1u, kPortHighDriveStrength);

    /*CMDVCC pin:B21,PTE3*/
    PORT_HAL_SetMuxMode(PORTB,0u,kPortMuxAsGpio);
    PORT_HAL_SetDriveStrengthMode(PORTB, 0u, kPortHighDriveStrength);
}

void configure_dac_pins(uint32_t instance)
{
    switch (instance)
    {
        case 0U:
            PORT_HAL_SetMuxMode(PORTE, 30U, kPortPinDisabled);
            break;
        default:
            break;
    }
}
/* END pin_mux. */
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
