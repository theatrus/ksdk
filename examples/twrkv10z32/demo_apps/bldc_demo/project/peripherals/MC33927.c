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
* @file      MC33927.c
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

/******************************************************************************
* Includes
******************************************************************************/
#include "MC33927.h"

/*****************************************************************************
* External objects
******************************************************************************/
extern byte SPI_Send(byte);

/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Constants and macros
******************************************************************************/



/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/

/******************************************************************************
* Local functions
******************************************************************************/

/******************************************************************************
* Global functions
******************************************************************************/


/***************************************************************************//*!
*
* @brief  Reads Status Register 0 from MC33927
*
* @param  ptr			MC33927_LATCH_T * pudtSR0
*						  - pointer to the Status Register 0 structure
*
* @return N/A
*		
* @remarks 	The function sends the command to read Status Register 0 from
*           MC33927.
*
****************************************************************************/
void MC33927_GetSR0(MC33927_LATCH_T * pudtSR0)
{
    SPI_Send(STATUS_REGISTER_0);
    pudtSR0 -> W8 = (byte)SPI_Send(STATUS_REGISTER_0);
}

/***************************************************************************//*!
*
* @brief  Reads Status Register 1 from MC33927
*
* @param  ptr			MC33927_LATCH_T * pudtSR1
*						  - pointer to the Status Register 1 structure
*
* @return N/A
*		
* @remarks 	The function sends the command to read Status Register 1 from
*           MC33927.
*
****************************************************************************/
void MC33927_GetSR1(MC33927_MODE_T * pudtSR1)
{
    SPI_Send(STATUS_REGISTER_1);
    pudtSR1 -> W8 = (byte)SPI_Send(STATUS_REGISTER_0);
}

/***************************************************************************//*!
*
* @brief  Reads Status Register 2 from MC33927
*
* @param  ptr			MC33927_LATCH_T * pudtSR2
*						  - pointer to the Status Register 2 structure
*
* @return N/A
*		
* @remarks 	The function sends the command to read Status Register 2 from
*           MC33927.
*
****************************************************************************/
void MC33927_GetSR2(MC33927_LATCH_T * pudtSR2)
{
    SPI_Send(STATUS_REGISTER_2);
    pudtSR2 -> W8 = (byte)SPI_Send(STATUS_REGISTER_0);
}

/***************************************************************************//*!
*
* @brief  Reads Status Register 3 from MC33927
*
*
* @return deadtime value
*		
* @remarks 	The function sends the command to read Status Register 3 from
*           MC33927. The return of this function is the deadtime
*           8-bit value, 255 corresponds to 15us.
*
****************************************************************************/
byte MC33927_GetSR3(void)
{
    SPI_Send(STATUS_REGISTER_3);
    return((byte)SPI_Send(STATUS_REGISTER_0));
}


/***************************************************************************//*!
*
* @brief  Set modes on MC33927
*
* @param  ptr			MC33927_MODE_COMMAND_T * pudtModeCommand
*						  - pointer to the mode command structure
*
* @return N/A
*		
* @remarks 	The function sets the mode bits MC33927 corresponding to the
*           structure the pudtModeCommand pointer points to.
*
****************************************************************************/
void MC33927_ModeCommand(MC33927_MODE_COMMAND_T * pudtModeCommand)
{
    SPI_Send((pudtModeCommand -> W8 & MODE_COMMAND_MASK) | MODE_COMMAND);
}

/***************************************************************************//*!
*
* @brief  Set modes on MC33927
*
* @param  ptr			MC33927_MODE_COMMAND_T * pudtModeCommand
*						  - pointer to the mode command structure
*
* @return N/A
*		
* @remarks 	The function sets the mode bits of MASK0 and MASK1 on MC33927
*           corresponding to the structure the pudtInterruptMask pointer points to.
*
****************************************************************************/
void MC33927_MaskInterrupts(MC33927_LATCH_T * pudtInterruptMask)
{
    SPI_Send((pudtInterruptMask -> W8 & 0x0F) | MC33927_MASK0);
    SPI_Send(((pudtInterruptMask -> W8 >> 4) & 0x0F) | MC33927_MASK1);
}

/***************************************************************************//*!
*
* @brief  Clear flags on MC33927
*
* @param  ptr			MC33927_MODE_COMMAND_T * pudtClearFlags
*						  - pointer to the clear flags structure
*
* @return N/A
*		
* @remarks 	The function clears the flag bits on MC33927 corresponding to
*           the structure the pudtClearFlags pointer points to.
*
****************************************************************************/
void MC33927_ClearFlags(MC33927_LATCH_T * pudtClearFlags)
{
    SPI_Send((pudtClearFlags -> W8 & 0x0F) | CLINT0);
    SPI_Send(((pudtClearFlags -> W8 >> 4) & 0x0F) | CLINT1);
}

/***************************************************************************//*!
*
* @brief  Set zero deadtime on MC33927
*
* @remarks 	The function sets the zero deadtime on MC33927. Be sure a deadtime
*           is properly generated by the PWM peripheral of the processor.
*
****************************************************************************/
void MC33927_ZeroDeadtime(void)
{
    SPI_Send(ZERO_DEADTIME);
}

/***************************************************************************//*!
*
* @brief  Enter the deadtime calibration mode on MC33927
*
* @remarks 	The function sets MC33927 to enter the deadtime calibration mode.
*           The driver will ignore any commands sent to it after this function called.
*           To calibrate the deadtime follow these steps:
*           1. Call this function
*           2. Set the /CS pin of MC33927 to logical low
*           3. Apply delay loop 16x longer than the desired deadtime
*           4. Set the /CS pin of MC33927 to logical high
*
*           Now the deadtime is set. After reset the deadtime is set to 15us!
*           
****************************************************************************/
void MC33927_DeadtimeCalibration(void)
{
    SPI_Send(DEADTIME_CALIBRATION);
}
