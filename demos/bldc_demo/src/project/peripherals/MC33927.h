/******************************************************************************
* 
* Copyright (c) 2009 Freescale Semiconductor;
* All Rights Reserved                       
*
******************************************************************************* 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************//*!
*
* @file      MC33927.h
*
* @author    R61928
* 
* @version   1.0.1.0
* 
* @date      Sep-16-2013
* 
* @brief     MC33927 routines on the board 00336_01
*
*******************************************************************************
*
* MC33927 routines on the board 00336_01 plugged on 00315_01.
*
******************************************************************************/
#ifndef _MC33927_H_
#define _MC33927_H_

#ifndef byte
#define byte unsigned char
#endif
/******************************************************************************
* Includes
******************************************************************************/

/******************************************************************************
* Constants
******************************************************************************/
/*******************************************************************************
*                  MC33927 predriver register constants definition
*******************************************************************************/

#define STATUS_REGISTER_0		          0x00
#define STATUS_REGISTER_1		          0x01
#define STATUS_REGISTER_2		          0x02
#define STATUS_REGISTER_3		          0x03

#define MODE_COMMAND                      0x40
#define MODE_COMMAND_MASK                 0x0B

#define MC33927_MASK0        			          0x20
#define MC33927_MASK1        			          0x30

#define CLINT0        			          0x60
#define CLINT1        			          0x70

#define ZERO_DEADTIME                     0x80
#define DEADTIME_CALIBRATION              0x81
/******************************************************************************
* Macros 
******************************************************************************/

/******************************************************************************
* Types
******************************************************************************/
/* MC33927 latches structure */
typedef union
{
    struct
    {
        byte TLim: 1; 
        byte Desat : 1;
        byte LowVLS: 1;
        byte Overcurrent : 1;
        byte PhaseError : 1; 
        byte FramingError : 1;
        byte WriteError : 1;
        byte ResetEvent	: 1;
    } B;
    byte W8;
} MC33927_LATCH_T;

/* MC33927 modes structure */
typedef union
{
    struct
    {
        byte Lock: 1; 
        byte FullOnMode : 1;
        byte Bit2 : 1;
        byte DeadtimeCalibration : 1;
        byte CalibrationOverflow : 1; 
        byte ZeroDeadtime : 1;
        byte DesatMode : 1;
        byte Bit7 : 1;
    } B;
    byte W8;
} MC33927_MODE_T;

/* MC33927 commands structure */
typedef union
{
    struct
    {
        byte Lock : 1; 
        byte FullOnMode : 1;
        byte Bit2: 1;
        byte DesatFaultMode : 1;
        byte Bit4 : 1; 
        byte Bit5 : 1;
        byte Bit6 : 1;
        byte Bit7 : 1;
    } B;
    byte W8;
} MC33927_MODE_COMMAND_T;


/******************************************************************************
* Global variables
******************************************************************************/
   
/******************************************************************************
* Global functions
******************************************************************************/
/* Get Status Register 0 */
extern void MC33927_GetSR0(MC33927_LATCH_T * pudtSR0);

/* Get Status Register 1 */
extern void MC33927_GetSR1(MC33927_MODE_T * pudtSR1);

/* Get Status Register 2 */
extern void MC33927_GetSR2(MC33927_LATCH_T * pudtSR2);

/* Get Status Register 3 */
extern byte MC33927_GetSR3(void);

/* Set modes */
extern void MC33927_ModeCommand(MC33927_MODE_COMMAND_T * pudtModeCommand);

/* Mask interrupts */
extern void MC33927_MaskInterrupts(MC33927_LATCH_T * pudtInterruptMask);

/* Clear flags */
extern void MC33927_ClearFlags(MC33927_LATCH_T * pudtClearFlags);

/* Set zero deadtime */
extern void MC33927_ZeroDeadtime(void);

/* Calibrate deadtime */
extern void MC33927_DeadtimeCalibration(void);

/******************************************************************************
* Inline functions
******************************************************************************/

#endif /* _MC33927_H_ */
