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
#include "s401m16kr.h"
#include "fsl_slcd_driver.h"

/*******************************************************************************
 * Defination
 ******************************************************************************/

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

#define SEGA    (0x08)
#define SEGB    (0x04)
#define SEGC    (0x02)
#define SEGD    (0x01)
#define SEGE    (0x02)
#define SEGF    (0x08)
#define SEGG    (0x04)
#define SEG_    (0x00)

typedef struct
{
    uint8_t pinCount;
    uint8_t  mask[2];
}ASCIIPinMaskType;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Variables
 ******************************************************************************/

static const ASCIIPinMaskType ASCIITable[] = 
{
  {2,{SEGD + SEGE + SEGF + SEG_, SEGA + SEGB + SEGC}}, /* 0 */
  {2,{SEG_ + SEG_ + SEG_ + SEG_, SEG_ + SEGB + SEGC}}, /* 1 */
  {2,{SEGD + SEGE + SEG_ + SEGG, SEGA + SEGB + SEG_}}, /* 2 */
  {2,{SEGD + SEG_ + SEG_ + SEGG, SEGA + SEGB + SEGC}}, /* 3 */
  {2,{SEG_ + SEG_ + SEGF + SEGG, SEG_ + SEGB + SEGC}}, /* 4 */
  {2,{SEGD + SEG_ + SEGF + SEGG, SEGA + SEG_ + SEGC}}, /* 5*/
  {2,{SEGD + SEGE + SEGF + SEGG, SEGA + SEG_ + SEGC}}, /* 6 */
  {2,{SEG_ + SEG_ + SEG_ + SEG_, SEGA + SEGB + SEGC}}, /* 7 */
  {2,{SEGD + SEGE + SEGF + SEGG, SEGA + SEGB + SEGC}}, /* 8 */
  {2,{SEGD + SEG_ + SEGF + SEGG, SEGA + SEGB + SEGC}}, /* 9 */
};

static const uint8_t Pos2PinTable[4][2] = 
{
  {20, 24},
  {26, 27},
  {40, 42},
  {43, 44},
};

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief show an number in SLCD.
 *
 * This function show an number in SLCD at a certain positation.
 */
void SLCD_DispNum(uint8_t pos, char num)
{
    int max,i;
    max = ASCIITable[num].pinCount;
    for(i = 0; i < max; i++)
    {
        SLCD_DRV_SetPinWaveForm(0, Pos2PinTable[pos][i], ASCIITable[num].mask[i]);
    }
}

/*!
 * @brief show an dot in SLCD.
 *
 * This function show an number in dot at a certain positation.
 */

void SLCD_WritePin(uint32_t lcdPin, uint8_t index, bool status)
{
    (status)?
        (LCD_WF8B(lcdPin) |= (1 << index)):
        (LCD_WF8B(lcdPin) &= ~(1 << index));
}

void SLCD_DispDot(uint8_t pos, bool status)
{
    SLCD_WritePin(Pos2PinTable[pos][1], 0, status);
}
/******************************************************************************
 * EOF
 ******************************************************************************/

