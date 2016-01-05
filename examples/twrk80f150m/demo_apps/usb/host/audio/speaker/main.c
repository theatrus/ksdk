/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 Freescale Semiconductor;
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
 * $FileName: main.c$
 * $Version : 3.8.2.0$
 * $Date    : Aug-21-2012$
 *
 * Comments:
 *
 *   This file is an example of device drivers for the HID class. This example
 *   demonstrates the keyboard functionality. Note that a real keyboard driver also
 *   needs to distinguish between intentionally repeated and unrepeated key presses.
 *   This example simply demonstrates how to receive data from a USB Keyboard. 
 *   Interpretation of data is up to the application and customers can add the code
 *   for it.
 *
 *END************************************************************************/
#include "osadapter_cfg.h"


#include <usb_types.h> 
#include <usb_desc.h>  
#include <usb_misc.h>
#include <usb_opt.h>
#include <soc.h>
#include <osadapter.h>
#include <usb_error.h>
#include <usb_host_hub_sm.h>
#include <usb_host_audio.h>
#include "audio_speaker.h"
#include "hidkeyboard.h"
#include "sd_card.h"
#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

/*
 ** Globals
 */

/* Table of driver capabilities this application wants to use */
static USB_HOST_DRIVER_INFO DriverInfoTable[] =
{
    {
        {0x00, 0x00}, /* Vendor ID per USB-IF             */
        {0x00, 0x00}, /* Product ID per manufacturer      */
        USB_CLASS_AUDIO, /* Class code                       */
        USB_SUBCLASS_AUD_CONTROL, /* Sub-Class code                   */
        0xFF, /* Protocol                         */
        0, /* Reserved                         */
        usb_host_audio_control_event /* Application call back function   */
    },
    {
        {0x00, 0x00}, /* Vendor ID per USB-IF             */
        {0x00, 0x00}, /* Product ID per manufacturer      */
        USB_CLASS_AUDIO, /* Class code                       */
        USB_SUBCLASS_AUD_STREAMING, /* Sub-Class code                   */
        0xFF, /* Protocol                         */
        0, /* Reserved                         */
        usb_host_audio_stream_event /* Application call back function   */
    },
    #if 0
    {
        {   0x00,0x00}, /* Vendor ID per USB-IF             */
        {   0x00,0x00}, /* Product ID per manufacturer      */
        USB_CLASS_HID, /* Class code                       */
        USB_SUBCLASS_HID_NONE, /* Sub-Class code                   */
        USB_PROTOCOL_HID_NONE, /* Protocol                         */
        0, /* Reserved                         */
        usb_host_hid_keyboard_event /* Application call back function   */
    },
#endif
    /* USB 1.1 hub */
    {

    //       {0x00,0x00},                  /* Vendor ID per USB-IF             */
    //       {0x00,0x00},                  /* Product ID per manufacturer      */
    //       USB_CLASS_HUB,                /* Class code                       */
    //       USB_SUBCLASS_HUB_NONE,        /* Sub-Class code                   */
    //       USB_PROTOCOL_HUB_ALL,          /* Protocol                         */
    //      0,                            /* Reserved                         */
    //       usb_host_hub_device_event     /* Application call back function   */
        },
        {
            {0x00, 0x00}, /* All-zero entry terminates        */
            {0x00, 0x00}, /* driver info list.                */
            0,
            0,
            0,
            0,
            NULL
        }
};

usb_host_handle host_handle;

#define MAX_SUPPORTED_USAGE_ID 40

#define MAIN_TASK          (10)
#define AUDIO_TASK         (12)
#define HID_KEYB_TASK      (11)
#define SDCARD_TASK        (13)
#define SHELL_TASK         (14)

extern void Main_Task(uint32_t param);
extern void Audio_Task(uint32_t);
extern void Sdcard_task(uint32_t temp);
extern void Shell_task(uint32_t temp);
extern void Keyboard_Task(uint32_t param);
LWEVENT_STRUCT USB_Audio_FU_Request;
LWEVENT_STRUCT USB_Keyboard_Event;
LWEVENT_STRUCT SD_Card_Event;

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    {MAIN_TASK, Main_Task, 2000L, 8L, "Main", MQX_AUTO_START_TASK},
    {AUDIO_TASK, Audio_Task, 4000L, 10L, "Audio", MQX_USER_TASK},
    //  { HID_KEYB_TASK,  Keyboard_Task,  4000L,  9L,  "Keyboard",  MQX_USER_TASK},
        {SDCARD_TASK, Sdcard_task, 2000L, 11L, "Sdcard", MQX_USER_TASK},
        {SHELL_TASK, Shell_task, 2000L, 12L, "Shell", MQX_USER_TASK},
        {0L, 0L, 0L, 0L, 0L, 0L}
};

extern LWEVENT_STRUCT USB_Keyboard_Event;
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : main (Main_Task if using MQX)
 * Returned Value : none
 * Comments       :
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/
void Main_Task(uint32_t param)
{ /* Body */
    usb_status status = USB_OK;

    /*
     ** It means that we are going to act like host, so we initialize the
     ** host stack. This call will allow USB system to allocate memory for
     ** data structures, it uses later (e.g pipes etc.).
     */
    status = _usb_host_init(0, /*  */
    &host_handle); /* Returned pointer */
    if (status != USB_OK)
    {
        USB_PRINTF("\nUSB Host Initialization failed. STATUS: %x", status);
        fflush (stdout);
    }

    /*
     ** Since we are going to act as the host driver, register the driver
     ** information for wanted class/subclass/protocols
     */
    status = _usb_host_driver_info_register(
        host_handle,
        DriverInfoTable
        );
    if (status != USB_OK)
    {
        USB_PRINTF("\nDriver Registration failed. STATUS: %x", status);
        fflush (stdout);
    }
    _int_enable();

    USB_PRINTF("\n MQX USB Audio Speaker Host Demo\n");
    fflush (stdout);
    /* Create lwevents*/
    if (MQX_OK != _lwevent_create(&USB_Keyboard_Event, LWEVENT_AUTO_CLEAR))
    {
        USB_PRINTF("\n_lwevent_create USB_Keyboard_Event failed.\n");
    }
    if (MQX_OK != _lwevent_create(&USB_Audio_FU_Request, LWEVENT_AUTO_CLEAR))
    {
        USB_PRINTF("\n_lwevent_create USB_Audio_FU_Request failed.\n");
    }
    if (MQX_OK != _lwevent_create(&SD_Card_Event, LWEVENT_AUTO_CLEAR))
    {
        USB_PRINTF("\n_lwevent_create SD_Card_Event failed.\n");
    }
    _task_create(0, AUDIO_TASK, (uint32_t)host_handle);
    _task_create(0, HID_KEYB_TASK, (uint32_t)host_handle);
    _task_create(0, SDCARD_TASK, (uint32_t)host_handle);
    _task_create(0, SHELL_TASK, (uint32_t)host_handle);
    /* The main task has done its job, so exit */
} /* Endbody */

/* EOF */

