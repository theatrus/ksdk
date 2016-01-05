/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "fsl_device_registers.h"
#include "board.h"
#include "fsl_debug_console.h"

#include "fsl_clock_manager.h"
#include "fsl_mmau_driver.h"

/* MMAU callback prototype  */
void MMAU_EXAMPLE_IrqHandler (void);
void MMAU_EXAMPLE_InstallCallback(void (*callbackFunc)(void) );

static const frac64_t a[] = 
{ 
   4611686018427390000ll, 
  -7585919437318840000ll, 
   3743501193243610000ll, 
   -879687520293320000ll,   
    120585664192670000ll,  
    -10819389109330000ll, 
       684506989590000ll 
};

static frac32_t sine (frac32_t x)
{
  if      (x >  1073741824l) { x =  2147483648l-x; }
  else if (x < -1073741824l) { x = -2147483648l-x; }
  MMAU_HAL_d_mul_dl (a[6],x);                         /* acc=    x*a[6]  */     
  MMAU_HAL_d_maca_dl(a[5],x);                         /* acc=acc*x+a[5]  */
  MMAU_HAL_d_mula_l (     x);                         /* acc=acc*x       */
  MMAU_HAL_d_maca_dl(a[4],x);                         /* acc=acc*x+a[4]  */
  MMAU_HAL_d_mula_l (     x);                         /* acc=acc*x       */
  MMAU_HAL_d_maca_dl(a[3],x);                         /* acc=acc*x+a[3]  */
  MMAU_HAL_d_mula_l (     x);                         /* acc=acc*x       */ 
  MMAU_HAL_d_maca_dl(a[2],x);                         /* acc=acc*x+a[2]  */
  MMAU_HAL_d_mula_l (     x);                         /* acc=acc*x       */
  MMAU_HAL_d_maca_dl(a[1],x);                         /* acc=acc*x+a[1]  */ 
  MMAU_HAL_d_mula_l (     x);                         /* acc=acc*x       */
  MMAU_HAL_d_maca_dl(a[0],x);                         /* acc=acc*x+a[0]  */
  MMAU_HAL_d_mula_l (     x);                         /* acc=acc*x       */
  return (MMAU_HAL_l_mula_l(1686629713))<<3;          /* acc=acc*2*pi    */
}

static volatile frac32_t result;


int main(void)
{
    //frac32_t i;

    mmau_user_config_t mmauConfigStr;
    hardware_init();
    
    PRINTF("\r\nMMAU example : start\n");
    
    mmauConfigStr.mmauAccess = false;
    mmauConfigStr.mmauDmaReq = false;
    mmauConfigStr.mmauIntReqMask = INT_MMAU_QIE|INT_MMAU_VIE|INT_MMAU_DZIE;

    MMAU_EXAMPLE_InstallCallback(MMAU_EXAMPLE_IrqHandler);
    
    MMAU_DRV_Init(&mmauConfigStr);
       
    /* compute sine function for various angles */
    result = sine((frac32_t)2147483648);
    PRINTF("\r\n Sin(pi)      = %d in frac32 format\n", result);
    result = sine((frac32_t)1717986918);
    PRINTF("\r\n Sin(0.8*pi)  = %d in frac32 format\n", result);
    result = sine((frac32_t)1288490189);
    PRINTF("\r\n Sin(0.6*pi)  = %d in frac32 format\n", result);
    result = sine((frac32_t)858993459);
    PRINTF("\r\n Sin(0.4*pi)  = %d in frac32 format\n", result);
    result = sine((frac32_t)429496729);
    PRINTF("\r\n Sin(0.2*pi)  = %d in frac32 format\n", result);
    result = sine(0);
    PRINTF("\r\n Sin(0)       = %d in frac32 format\n", result);
    result = sine((frac32_t)2576980377);
    PRINTF("\r\n Sin(-0.2*pi) = %d in frac32 format\n", result);
    result = sine((frac32_t)3006477107);
    PRINTF("\r\n Sin(-0.4*pi) = %d in frac32 format\n", result);
    result = sine((frac32_t)3435973837);
    PRINTF("\r\n Sin(-0.6*pi) = %d in frac32 format\n", result);
    result = sine((frac32_t)3865470566);
    PRINTF("\r\n Sin(-0.8*pi) = %d in frac32 format\n", result);
    result = sine((frac32_t)-2147483648);
    PRINTF("\r\n Sin(-pi)     = %d in frac32 format\n", result);
      
    PRINTF("\r\nMMAU example : end\n");
    while(1);
}

/* MMAU callback function */
void MMAU_EXAMPLE_IrqHandler (void)
{
    uint32_t state = MMAU_HAL_GetStateCmd(MMAU);
    if (state & (1 << MMAU_CSR_DZIF_SHIFT))
    {
        PRINTF("\r\nInterrupt occurs due to divide by zero \n"); 
    }
    if (state & (1 << MMAU_CSR_VIF_SHIFT))
    {
        PRINTF("\r\nInterrupt occurs due to multiplication or divide overflow \n");  
    }
    if (state & (1 << MMAU_CSR_QIF_SHIFT))
    {
        PRINTF("\r\nInterrupt occurs due to accumulator overflow \n");
    }  
    MMAU_HAL_ConfigStateCmd(MMAU, state);
}

