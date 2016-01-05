/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
 * All Rights Reserved
 *
 * Copyright (c) 1989-2008 ARC International;
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
 * $FileName: otg_mouse.h$
 * $Version :
 * $Date    :
 *
 * Comments:
 *
 * 
 *
 *END************************************************************************/
#ifndef _OTG_MOUSE_H
#define _OTG_MOUSE_H

#define MAX3353_INT_LEVEL                   (4)

/* struct contains max3353 init params */
#define MAX3353_I2C_ADDRESS                 (0x2C)                              /* I2C Address */

#define OTG_LOAD_DEVICE                     ((uint32_t)0x1000)
#define OTG_UNLOAD_DEVICE                   ((uint32_t)0x2000)
#define OTG_LOAD_HOST                       ((uint32_t)0x4000)
#define OTG_UNLOAD_HOST                     ((uint32_t)0x8000)
#if defined (FSL_RTOS_FREE_RTOS)
#define OTG_A_B_STATE_EVENT_MARK            ((uint32_t)0xFF)
#else
#define OTG_A_B_STATE_EVENT_MARK            ((uint32_t)0xFFFF0FFF)
#endif
#define OTG_LOAD_UNLOAD_EVENT_MARK          ((uint32_t)0x0000F000)
#define USBCFG_DEFAULT_OTG_CONTROLLER 0
#endif
/* EOF */

