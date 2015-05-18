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
 
/******************************************************************************
*
* freemaster_cfg.h
*
* FreeMASTER Serial Communication Driver configuration file
*
*******************************************************************************/

#ifndef __FREEMASTER_CFG_H
#define __FREEMASTER_CFG_H

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

//! @brief Select interrupt or poll-driven serial communication
#define FMSTR_LONG_INTR    0        //!< Complete message processing in interrupt
#define FMSTR_SHORT_INTR   0        //!< SCI FIFO-queuing done in interrupt
#define FMSTR_POLL_DRIVEN  1        //!< No interrupt needed, polling only

//! @brief Select communication interface (SCI or CAN)
#define FMSTR_SCI_BASE          0x4006B000u     //!< UART1 base
#define FMSTR_SCI_INTERRUPT     29

#define FMSTR_CAN_BASE          0x400A4000      //!< MSCAN1 base on K40
#define FMSTR_CAN_RX_INTERRUPT  38  //!< HC12 only, MSCAN RX interrupt number (use PRM if not defined)
#define FMSTR_CAN_TX_INTERRUPT  39  //!< HC12 only, MSCAN TX interrupt number (use PRM if not defined) 

#define FMSTR_SCI_TWOWIRE_ONLY  1
#define FMSTR_USE_SCI           1   //!< To select SCI communication interface 
#define FMSTR_USE_FLEXCAN       0   //!< To select FlexCAN communication interface

//! @brief Input/output communication buffer size
#define FMSTR_COMM_BUFFER_SIZE  0   //!< Set to 0 for "automatic"

//! @brief Receive FIFO queue size (use with FMSTR_SHORT_INTR only)
#define FMSTR_COMM_RQUEUE_SIZE  32  //!< Set to 0 for "default"

//! @brief Support for Application Commands
#define FMSTR_USE_APPCMD        0   //!< Enable/disable App.Commands support
#define FMSTR_APPCMD_BUFF_SIZE  32  //!< App.Command data buffer size
#define FMSTR_MAX_APPCMD_CALLS  4   //!< How many app.cmd callbacks? (0=disable)

//! @brief Oscilloscope support
#define FMSTR_USE_SCOPE         1   //!< Enable/disable scope support
#define FMSTR_MAX_SCOPE_VARS    8   //!< Max. number of scope variables (2..8)

//! @brief Recorder support
#define FMSTR_USE_RECORDER      1   //!< Enable/disable recorder support
#define FMSTR_MAX_REC_VARS      2   //!< Max. number of recorder variables (2..8)
#define FMSTR_REC_OWNBUFF       0   //!< Use user-allocated rec. buffer (1=yes)

//! @brief Built-in recorder buffer (use when FMSTR_REC_OWNBUFF is 0)
#define FMSTR_REC_BUFF_SIZE     2048    //!< built-in buffer size

//! @brief Recorder time base, specifies how often the recorder is called in the user app.
#define FMSTR_REC_TIMEBASE      FMSTR_REC_BASE_MICROSEC(50) // 0 = "unknown"

//! @brief Target-side address translation (TSA)
#define FMSTR_USE_TSA           0   //!< Enable TSA functionality
#define FMSTR_USE_TSA_SAFETY    0   //!< Enable access to TSA variables only
#define FMSTR_USE_TSA_INROM     0   //!< TSA tables declared as const (put to ROM)

//! @brief Enable/Disable read/write memory commands
#define FMSTR_USE_READMEM       1   //!< Enable read memory commands
#define FMSTR_USE_WRITEMEM      1   //!< Enable write memory commands
#define FMSTR_USE_WRITEMEMMASK  1   //!< Enable write memory bits commands

//! @brief Enable/Disable read/write variable commands (a bit faster than Read Mem)
#define FMSTR_USE_READVAR       0   //!< Enable read variable fast commands
#define FMSTR_USE_WRITEVAR      0   //!< Enable write variable fast commands
#define FMSTR_USE_WRITEVARMASK  0   //!< Enable write variable bits fast commands

#define FMSTR_BUFFER_ACCESS_BY_FUNCT 1

#endif /* __FREEMASTER_CFG_H */

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

