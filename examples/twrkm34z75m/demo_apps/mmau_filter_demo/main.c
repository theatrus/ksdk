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

// MMAU callback prototype 
void MMAU_DEMO_IrqHandler(void);
void MMAU_DEMO_InstallCallback(void (*callbackFunc)(void) );

// Input array - sine() signal cycle sampled in 128 points                    
static const frac32_t input[] = 
{
  0,2166732,2753973,1604297,123267,12239,1734704,4097830,5326223,4660528,
  3040692,2258324,3341806,5629024,7363103,7274451,5723831,4376659,4701958,
  6648280,8689462,9199008,7914891,6175140,5711947,7090722,9200346,10253866,
  9398102,7482051,6291825,6941379,8868840,10342644,10021506,8162831,6390892,
  6233630,7746287,9461846,9712938,8134287,5992130,5043834,5955090,7700647,
  8488467,7374730,5114698,3482901,3675087,5231628,6452309,5928792,3814157,
  1685767,1125013,2293441,3788057,3906151,2180158,-200227,-1458963,-832815,
  741949,1473959,331515,-2027032,-3845491,-3851097,-2400263,-1156602,-1591289,
  -3656810,-5828926,-6479050,-5340656,-3748612,-3435831,-4971342,-7246945,
  -8474926,-7797983,-6060970,-5049184,-5879437,-7993407,-9660172,-9535189,
  -7871223,-6290283,-6322041,-8025478,-9935534,-10382821,-8997469,-7042261,
  -6274946,-7364674,-9289208,-10255966,-9317117,-7223680,-5749155,-6092036,
  -7796394,-9162986,-8780521,-6797545,-4789092,-4338241,-5610457,-7205110,
  -7417939,-5777092,-3468902,-2269827,-2946762,-4566901,-5338900,-4227529,
  -1887148,-73038,-60914,-1498499,-2723782,-2263234,-160231
};

// Output array                                                               
static frac32_t output[sizeof(input)/sizeof(input[0])];

// Filter coefficients      
static const frac32_t a[] = 
{ 
  901782070, -1147695792, 654714077,-141089676 
};

static const frac32_t b[] = 
{ 
  45035,180139,268435,
  180139,45035
};

// Filter previous input values
static frac32_t x[] ={0l, 0l, 0l, 0l};

// Filter previous output values                                              
static frac32_t y[] ={0ll, 0ll, 0ll, 0ll};

/*!
 * Brief  The 4th order infinite impulse response filter (IIR) iteration using the following
 * equation: y(n) = b(1)*x(n)+ b(2)*x(n-1)+ b(3)*x(n-2)+ b(4)*x(n-3)+b(5)*x(n-4) 
 * +a(2)*y(n-1)+a(3)*y(n-2)+a(4)*y(n-3)+a(5)*y(n-4)
 * @param  x  - Input fractional value represented in 32-bit fractional format 
 *              "x(n)".
 * @param *pb - Pointer to filter constants "b" represented in 32-bit fractional 
 *              format "b(1) ... b(5)".
 * @param *pa - Pointer to filter constants "a" represented in 32-bit fractional
 *              format "-a(2) ... -a(5)".
 * @param sc  - Filter constants scaling.
 * @param *px - Pointer to previous input values represented in 32-bit fractional
 *              format "x(n) ... x(n-3)".
 * @param *py - Pointer to previous output values represented in 32-bit
*/
static frac32_t MMAU_DEMO_d_iir_4ord (frac32_t x, const frac32_t *pb, const frac32_t *pa, int16_t sc, 
                          frac32_t *px, frac32_t *py)
{
  register frac32_t y;
  
  // actual filter output value calculation with using MMAU instructions      
  MMAU_HAL_d_mul_ll(*(pb  ),      x);                     // acc=    b[0]*x[0]  
  MMAU_HAL_d_mac_ll(*(pb+1),*(px  ));                     // acc=acc+b[1]*x[1]  
  MMAU_HAL_d_mac_ll(*(pb+2),*(px+1));                     // acc=acc+b[2]*x[2] 
  MMAU_HAL_d_mac_ll(*(pb+3),*(px+2));                     // acc=acc+b[3]*x[3] 
  MMAU_HAL_d_mac_ll(*(pb+4),*(px+3));                     // acc=acc+b[4]*x[4] 
  MMAU_HAL_d_mac_ll(*(py  ),*(pa  ));                     // acc=acc+a[2]*y[0]         
  MMAU_HAL_d_mac_ll(*(py+1),*(pa+1));                     // acc=acc+a[3]*y[1]         
  MMAU_HAL_d_mac_ll(*(py+2),*(pa+2));                     // acc=acc+a[4]*y[2]         
  y = (MMAU_HAL_l_mac_ll(*(py+3),*(pa+3)))<<(sc);         // y=(acc+a[5]*y[3])<<sc   
  
  // shifting previous input values                                           
  *(px+3)=*(px+2); *(px+2)=*(px+1); *(px+1)=*(px); *(px)= x;
  
  // shifting previous output values                                          
  *(py+3)=*(py+2); *(py+2)=*(py+1); *(py+1)=*(py); *(py)= y;
  
  return y;
}

int main (void)
{
  register int i;
  
  // Initialize standard SDK demo application pins
  hardware_init();
  PRINTF("\r\n Mmau_filter_demo Begin.\r\n");
  PRINTF("\r\nFirst column is an input signal, second column is an filtered output signal.\r\n");
  
  for(i=0; i < (sizeof(input)/sizeof(input[0])); i++)
  {
    output[i] = MMAU_DEMO_d_iir_4ord (input[i], b, a, 3, (frac32_t*)x, (frac32_t*)y);
    PRINTF("%10ld  -->  %10ld\r\n",input[i],output[i]);
  }
  
  PRINTF("\r\n Mmau_filter_demo End.\r\n");
  while(1);
}

// MMAU error callback function
void MMAU_DEMO_IrqHandler(void)
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

