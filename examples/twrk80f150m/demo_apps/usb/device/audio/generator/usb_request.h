/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
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
 * @file usb_descriptor.h
 *
 * @author
 *
 * @version
 *
 * @date 
 *
 * @brief The file is a header file for USB Descriptors required for Mouse
 *        Application
 *****************************************************************************/

#ifndef _USB_REQUEST_H
#define _USB_REQUEST_H

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_class_audio.h"
#include "usb_device_stack_interface.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/

/******************************************************************************
 * Types
 *****************************************************************************/


/******************************************************************************
 * Global Functions
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t USB_Class_Get_feature
(
   uint32_t handle,
   int32_t cmd, 
   uint8_t in_data,
   uint8_t * * out_buf
);

extern uint8_t USB_Class_Set_feature
(
   uint32_t handle,
   int32_t cmd, 
   uint8_t in_data,
   uint8_t * * in_buf
);

#ifdef __cplusplus
}
#endif

#endif
