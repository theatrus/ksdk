/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 Freescale Semiconductor;
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
* $FileName: audio_speaker.h$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief The file contains Macro's and functions needed by the audio speaker 
*        application
*
*****************************************************************************/

#ifndef _audio_speaker_h
#define _audio_speaker_h 1
#include "usb.h"

#define  HIGH_SPEED           (0)

#if HIGH_SPEED
#define CONTROLLER_ID         USB_CONTROLLER_EHCI_0
#else
#define CONTROLLER_ID         USB_CONTROLLER_KHCI_0
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#if defined(BSPCFG_ENABLE_SAI)
    #if ! BSPCFG_ENABLE_SAI
    #error This application requires BSPCFG_ENABLE_SAI defined non-zero in user_config.h. Please recompile libraries with this option.
    #endif
    #define AUDIO_DEVICE        "sai:"    
    #define CLK_MULT            (384)
    #define DATA_BUFF_SIZE      (2048) /* ((AUDIO_ENDPOINT_PACKET_SIZE) * 8) */
#elif defined(BSPCFG_ENABLE_II2S0)
    #if ! BSPCFG_ENABLE_II2S0
    #error This application requires BSPCFG_ENABLE_II2S0 defined non-zero in user_config.h. Please recompile libraries with this option.
    #endif
    #define AUDIO_DEVICE        "ii2s0:"    
    #define CLK_MULT            (256)
    #define DATA_BUFF_SIZE      (256) /* ((AUDIO_ENDPOINT_PACKET_SIZE) * 8) */
#else
    #error This application requires SAI or I2S audio device.
#endif
#else
#define DATA_BUFF_SIZE          (2048) /* ((AUDIO_ENDPOINT_PACKET_SIZE) * 8) */
#endif
/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define AUDIO_FORMAT_SAMPLE_RATE           (16000)
#define AUDIO_I2S_FS_FREQ_DIV              (CLK_MULT)

#define USB_APP_ENUM_COMPLETE_EVENT_MASK   (0x01 << 0)
#define USB_APP_BUFFER0_FULL_EVENT_MASK    (0x01 << 1)
#define USB_APP_BUFFER1_FULL_EVENT_MASK    (0x01 << 2)

/*****************************************************************************
 * Global variables
 *****************************************************************************/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/

#endif 

/* EOF */
