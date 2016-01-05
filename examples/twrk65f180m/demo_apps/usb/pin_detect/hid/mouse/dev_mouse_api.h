/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2015 Freescale Semiconductor;
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
* $FileName: dev_mouse_api.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*
*
*END************************************************************************/
#ifndef __devmouseapi_h__
#define __devmouseapi_h__

#include "usb_types.h"
/***************************************
**
** Application-specific definitions
*/
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && (defined (USE_RTOS)))
#define DEV_APP_TASK_INDEX  (10)
#endif

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

extern usb_status DEV_APP_init(void);
extern void       DEV_APP_uninit(void);
#if  ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && (defined (USE_RTOS)))
extern void       DEV_APP_task_stun( uint32_t param );
#endif

#ifdef __cplusplus
}
#endif


#endif

/* EOF */
