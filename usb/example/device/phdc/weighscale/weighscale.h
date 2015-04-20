/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013- 2014 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989 - 2008 ARC International;
* All Rights Reserved
*
*************************************************************************** 
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
**************************************************************************
*
* $FileName: weighscale.h$
* $Version :
* $Date    :
*
* Comments:
*
* @brief The file contains Macro's and functions needed by weighscale 
*        application
*
*****************************************************************************/
#ifndef _WEIGHSCALE_H
#define _WEIGHSCALE_H
#include "usb_descriptor.h"
#include "phd_com_model.h"

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define  HIGH_SPEED           (0)

#if HIGH_SPEED
#define CONTROLLER_ID         USB_CONTROLLER_EHCI_0
#else
#define CONTROLLER_ID         USB_CONTROLLER_KHCI_0
#endif
#define ITERATION_COUNT    (14000000)/* this delay depends on SoC used*/
#define SEND_MEASUREMENT   (0x02)
#define DISCONNECT         (0x04) 
#define NUM_OF_MSR_TO_SEND (10)

/*****************************************************************************
 * Types
 *****************************************************************************/
typedef struct _weighscale_variable_struct
{
    uint32_t app_handle;
    uint8_t event;
    phd_measurement_t msr;
    uint8_t num_of_msr_sent;
} weighscale_variable_struct_t;
  
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void APP_init(void);

#endif
/* EOF */
