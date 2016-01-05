/**HEADER********************************************************************
 * 
 * Copyright (c) 2008 , 2013 Freescale Semiconductor;
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
 * $FileName: host_mouse_api.h$
 * $Version : 3.8.7.0$
 * $Date    : Jun-7-2012$
 *
 * Comments:
 *
 *   This file contains function prototype and definitions used by the OTG application.
 *
 *END************************************************************************/
#ifndef __host_mouse_api_h__
#define __host_mouse_api_h__
/***************************************
 **
 ** Application-specific definitions
 */
#define HOST_APP_TASK_INDEX (11)

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C"
{
#endif

    extern usb_status HOST_APP_load(void);
    extern usb_status HOST_APP_unload(void);
    extern void HOST_task(uint32_t param);

#ifdef __cplusplus
}
#endif

#endif

/* EOF */
