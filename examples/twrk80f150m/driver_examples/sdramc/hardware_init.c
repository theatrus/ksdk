/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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

#include "board.h"
#include "pin_mux.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include "fsl_flexbus_hal.h"

extern FB_Type * const g_fbBase[];

void hardware_init(void) {
  FB_Type* fbbase = g_fbBase[0];
  /* enable clock for PORTs */
  CLOCK_SYS_EnablePortClock(PORTA_IDX);
  CLOCK_SYS_EnablePortClock(PORTB_IDX);
  CLOCK_SYS_EnablePortClock(PORTC_IDX);
  CLOCK_SYS_EnablePortClock(PORTD_IDX);

  /* Init board clock */
  BOARD_ClockInit();
  
  CLOCK_HAL_SetClkOutSel(SIM, kClockClkoutSelFlexbusClk);
    
  /* Sets the Flexbus security level*/
  SIM_HAL_SetFlexbusSecurityLevelMode(SIM,kSimFbslLevel3); 

  /* Enable the FB_BE_xx_yy signal in Flexbus */
  CLOCK_SYS_EnableFlexbusClock(0);
  FLEXBUS_HAL_SetMultiplexControlGroup2(fbbase,kFlexbusMultiplexGroup2_FB_BE_31_24);
  FLEXBUS_HAL_SetMultiplexControlGroup3(fbbase,kFlexbusMultiplexGroup3_FB_BE_23_16);
  FLEXBUS_HAL_SetMultiplexControlGroup4(fbbase,kFlexbusMultiplexGroup4_FB_BE_15_8);
  FLEXBUS_HAL_SetMultiplexControlGroup5(fbbase,kFlexbusMultiplexGroup5_FB_BE_7_0);
  /* Init sdram pins*/
  configure_sdram_pins(0);
  /* Init Sdram Clock*/
  CLOCK_SYS_EnableSdramcClock(0);
}

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.4 [05.10]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
