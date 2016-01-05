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
#ifndef __I2C_RTOS_H__
#define __I2C_RTOS_H__

///////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////
// Standard C Included Files
#include <stdio.h>
// SDK Included Files
#include "fsl_os_abstraction.h"
#include "board.h"
#include "fsl_smc_hal.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions
///////////////////////////////////////////////////////////////////////////////
#define I2C_RTOS_SLAVE_ADDRESS    (0x7F)
#define I2C_RTOS_LIGHT_CMD        1
#define I2C_RTOS_TEMP_CMD         2
#define I2C_RTOS_SLEEP_CMD        3
#define I2C_RTOS_READID_CMD       4

// Definition for boards with only 1 I2C.
#if USE_RTOS
  #ifdef FRDM_K82F
#define I2C_RTOS_MASTER_INSTANCE  (0)
#define I2C_RTOS_SLAVE_INSTANCE   (3U)
  #elif defined(FRDM_K66F)
#define I2C_RTOS_MASTER_INSTANCE  (3U)
#define I2C_RTOS_SLAVE_INSTANCE   (1U)
  #else
#define I2C_RTOS_MASTER_INSTANCE  (0)
#define I2C_RTOS_SLAVE_INSTANCE   (1U)
  #endif // FRDM_K82F
#else
#define I2C_RTOS_SLAVE_INSTANCE   (BOARD_I2C_INSTANCE)
#define I2C_RTOS_MASTER_INSTANCE  (BOARD_I2C_INSTANCE)
#endif

#define I2C_RTOS_MAGIC            0xBB
#define ADC_INSTANCE              HWADC_INSTANCE

#endif /* __I2C_RTOS_H__ */
