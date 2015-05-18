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

#include <assert.h>
#include "fsl_gpio_driver.h"
#include "board.h"

extern void sdhc_cd_irqhandler(void);

/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined (KL25Z4_SERIES) || defined (K64F12_SERIES) || defined (K22F51212_SERIES)|| \
    defined (KV31F51212_SERIES) || defined (K22F25612_SERIES) || defined (K60D10_SERIES) || \
    defined (K22F12810_SERIES) || defined (KV31F25612_SERIES) || defined (KV31F12810_SERIES)
/*!
 * @brief gpio IRQ handler with the same name in startup code
 */
void PORTA_IRQHandler(void)
{
    /* Clear interrupt flag.*/
    if(PORT_HAL_GetPortIntFlag(PORTA) == (1<<SDCARD_CARD_DETECTION_GPIO_PIN))
    {
        sdhc_cd_irqhandler();
    }
    PORT_HAL_ClearPortIntFlag(PORTA);
}

/*!
 * @brief gpio IRQ handler with the same name in startup code
 */
void PORTD_IRQHandler(void)
{
    if(PORT_HAL_GetPortIntFlag(PORTD) == (1<<SDCARD_CARD_DETECTION_GPIO_PIN))
    {
        sdhc_cd_irqhandler();
    }
    /* Clear interrupt flag.*/
    PORT_HAL_ClearPortIntFlag(PORTD);
}

#if defined (K64F12_SERIES) || defined (K22F51212_SERIES) || defined (K60D10_SERIES) ||\
    defined (KV31F51212_SERIES) || defined (K22F25612_SERIES) || defined (K22F12810_SERIES) || \
    defined (KV31F25612_SERIES) || defined (KV31F12810_SERIES)
/*!
 * @brief gpio IRQ handler with the same name in startup code
 */
void PORTB_IRQHandler(void)
{
    if(PORT_HAL_GetPortIntFlag(PORTB) == (1<<SDCARD_CARD_DETECTION_GPIO_PIN))
    {
        sdhc_cd_irqhandler();
    }
    /* Clear interrupt flag.*/
    PORT_HAL_ClearPortIntFlag(PORTB);
}

/*!
 * @brief gpio IRQ handler with the same name in startup code
 */
void PORTC_IRQHandler(void)
{
    if(PORT_HAL_GetPortIntFlag(PORTC) == (1 << SDCARD_CARD_DETECTION_GPIO_PIN))
    {
        sdhc_cd_irqhandler();
    }
  /* Clear interrupt flag.*/
    PORT_HAL_ClearPortIntFlag(PORTC);
}

/*!
 * @brief gpio IRQ handler with the same name in startup code
 */
void PORTE_IRQHandler(void)
{
    if(PORT_HAL_GetPortIntFlag(PORTE) == (1<<SDCARD_CARD_DETECTION_GPIO_PIN))
    {
        sdhc_cd_irqhandler();
    }
  /* Clear interrupt flag.*/
    PORT_HAL_ClearPortIntFlag(PORTE);
}

#endif

#endif

/*! @} */

/*******************************************************************************
 * EOF
 ******************************************************************************/

