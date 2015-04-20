/**HEADER********************************************************************
* 
* Copyright (c) 2014 Freescale Semiconductor;
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
* $FileName: file composite_app.h$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief The file contains function headers needed by application 
*
*****************************************************************************/


#ifndef _COMPOSITE_APP_H
#define _COMPOSITE_APP_H


/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define COMPOSITE_CFG_MAX            2

#define AUDIO_INTERFACE_INDEX        0
#define HID_MOUSE_INTERFACE_INDEX    1

#define  HIGH_SPEED                  (0)

#if HIGH_SPEED
#define CONTROLLER_ID                USB_CONTROLLER_EHCI_0
#else
#define CONTROLLER_ID                USB_CONTROLLER_KHCI_0
#endif
/******************************************************************************
 * Types
 *****************************************************************************/
typedef struct composite_device_struct
{
    composite_handle_t          composite_device;
    audio_handle_t              audio_handle;
    hid_mouse_struct_t          hid_mouse;
    composite_config_struct_t   composite_device_config_callback;
    class_config_struct_t       composite_device_config_list[COMPOSITE_CFG_MAX];
}composite_device_struct_t;

/*****************************************************************************
 * Global variables
 *****************************************************************************/
extern composite_device_struct_t                 g_composite_device;
/*****************************************************************************
 * Global Functions
 *****************************************************************************/
#endif
