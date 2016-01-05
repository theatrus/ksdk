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

/***************************************************************************//*!
*
* @file   freemaster_cfg.h
*
* @brief  FreeMASTER Serial Communication Driver configuration file
*
*******************************************************************************/
#ifndef __FREEMASTER_CFG_H
#define __FREEMASTER_CFG_H

/******************************************************************************
* Select interrupt or poll-driven serial communication
*******************************************************************************/
#define FMSTR_LONG_INTR        0    /* complete msg processing in interrupt   */
#define FMSTR_SHORT_INTR       0    /* SCI FIFO-queuing done in interrupt     */
#define FMSTR_POLL_DRIVEN      1    /* no interrupt needed, polling only      */

/******************************************************************************
* Select SCI communication interface
*******************************************************************************/
//#define FMSTR_SCI_BASE 0x4006A000   /* UART0 base on MKM34Z7                 */
//#define FMSTR_SCI_BASE 0x4006B000   /* UART1 base on MKM34Z7                 */
#define FMSTR_SCI_BASE 0x4006C000     /* UART2 base on MKM34Z7                 */
//#define FMSTR_SCI_BASE 0x4006D000   /* UART3 base on MKM34Z7                 */

/******************************************************************************
* Input/output communication buffer size
*******************************************************************************/
#define FMSTR_COMM_BUFFER_SIZE 200  /* set to 0 for "automatic"               */

/******************************************************************************
* Receive FIFO queue size (use with FMSTR_SHORT_INTR only)
*******************************************************************************/
#define FMSTR_COMM_RQUEUE_SIZE 32   /* set to 0 for "default"                 */

/******************************************************************************
* Support for Application Commands 
*******************************************************************************/
#define FMSTR_USE_APPCMD       0    /* enable/disable App.Commands support    */
#define FMSTR_APPCMD_BUFF_SIZE 32   /* App.Command data buffer size           */
#define FMSTR_MAX_APPCMD_CALLS 4    /* num. of app.cmd callbacks? (0=disable) */

/******************************************************************************
* Oscilloscope support
*******************************************************************************/
#define FMSTR_USE_SCOPE        1    /* enable/disable scope support           */
#define FMSTR_MAX_SCOPE_VARS   8    /* max. number of scope variables (2..8)  */

/******************************************************************************
* Recorder support
*******************************************************************************/
#define FMSTR_USE_RECORDER     1    /* enable/disable recorder support        */
#define FMSTR_MAX_REC_VARS     8    /* max. num. of recorder variables (2..8) */

/* built-in recorder buffer (use when FMSTR_REC_OWNBUFF is 0)                 */
#define FMSTR_REC_BUFF_SIZE    8192 /* built-in buffer size                   */

/* recorder time base, specifies how often the recorder is called             */
#define FMSTR_REC_TIMEBASE     FMSTR_REC_BASE_MILLISEC(0) /* 0 = "unknown"    */

#define FMSTR_REC_FLOAT_TRIG   0    /* enable/disable floating point trigger  */

/******************************************************************************
* Target-side address translation (TSA)
*******************************************************************************/
#define FMSTR_USE_TSA          0    /* enable TSA functionality               */
#define FMSTR_USE_TSA_SAFETY   0    /* enable access to TSA variables only    */
#define FMSTR_USE_TSA_INROM    0    /* TSA tables as const (put to ROM)       */

/******************************************************************************
* Enable the byte access to communication buffer. All Cortex M0-based devices 
* require this option to be set in order to avoid misaligned access to integer 
* parameters which is unsupported on this platform.
*******************************************************************************/
#define FMSTR_BYTE_BUFFER_ACCESS 1

#endif /* __FREEMASTER_CFG_H */

