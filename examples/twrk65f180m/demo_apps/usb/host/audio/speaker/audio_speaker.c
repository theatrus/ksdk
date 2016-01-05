/**HEADER********************************************************************
 *
 * Copyright (c) 2010, 2013 - 2015 Freescale Semiconductor;
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
 * $FileName: audio_speaker.c$
 * $Version : 3.8.3.0$
 * $Date    : Aug-9-2012$
 *
 * Comments:
 *
 *   This file is an example of device drivers for the Audio host class. This example
 *   demonstrates the audio transfer capability of audio host class with audio devices.
 *
 *END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"

#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
//#include "fsl_port_hal.h"
#include <stdio.h>
#include <stdlib.h>
//#include "fsl_uart_driver.h"
#include "fsl_pit_driver.h"
//#include "fsl_gpio_hal.h"
//#include "fsl_gpio_common.h"
#include "board.h"

#if defined (KL25Z4_SERIES) || defined (KL46Z4_SERIES)
extern void PIT_IRQHandler(void);
#else
extern void PIT0_IRQHandler(void);
#endif

#include "usb_host_audio.h"

#include "usb_host_hub_sm.h"

#include "audio_speaker.h"
#include "hidkeyboard.h"

/***************************************
 **
 ** Macros
 ****************************************/

/***************************************
 **
 ** Global functions
 ****************************************/

/***************************************
 **
 ** Local functions
 ****************************************/
static usb_status check_device_type(usb_audio_ctrl_desc_it_t*, usb_audio_ctrl_desc_ot_t*, char * *, uint8_t *);
static void USB_Prepare_Data(void);

/***************************************
 **
 ** Global variables
 ****************************************/
uint8_t                                      device_direction = USB_AUDIO_DEVICE_DIRECTION_UNDEFINE;
uint8_t                                   wav_buff[MAX_ISO_PACKET_SIZE];
uint32_t                           packet_size; /* number of bytes the host send to the device each mS */
uint8_t                                   resolution_size;
uint8_t                                   audio_state = AUDIO_IDLE;
uint32_t                                   read_data_err = FALSE;
audio_control_device_struct_t      audio_control = { 0 };
audio_control_device_struct_t      audio_stream  = { 0 };
usb_audio_stream_desc_format_type_t*     frm_type_desc = NULL;
usb_audio_ctrl_desc_fu_t*                fu_desc = NULL;
audio_command_t audio_com;
static int16_t                  cur_volume, min_volume, max_volume, res_volume;
//static int16_t                  physic_volume;
int8_t                            host_cur_volume = 5;
int16_t                          device_volume_step;
static uint8_t                   increase_times = 1;
static uint8_t                   decrease_times = 1;
/* Following are buffers for USB, should be aligned at cache boundary */
int8_t *g_cur_mute;
int8_t *g_cur_vol;
int8_t *g_max_vol;
int8_t *g_min_vol;
int8_t *g_res_vol;

volatile uint32_t                          buffer_1_free = 1; /* buffer is free */
volatile uint32_t                          buffer_2_free = 1;
volatile uint8_t                           buffer_read = 1; /* buffer need to be read */
extern uint8_t                             file_open_count;
extern uint8_t                             sd_card_state;
extern uint8_t                             g_interface_number;
//extern volatile USB_KEYBOARD_DEVICE_STRUCT kbd_hid_device;
extern os_event_handle                     usb_audio_fu_request;
extern os_event_handle                     usb_keyboard_event;
extern os_event_handle                     sd_card_event;


extern const unsigned char wav_data[];
extern const uint16_t wav_size;
#if (USE_RTOS)
#define keyboard_task_fuc keyboard_task_stun
#define timer_task_fuc timer_task_stun
#else
#define keyboard_task_fuc keyboard_task
#define timer_task_fuc timer_task
#endif

#define NUMBER_OF_BUFFER  0x5

os_event_handle USB_ctr_Event;
//#define USB_EVENT_CTRL           (0x01)

/***************************************
 **
 ** Local variables
 ****************************************/
static endpoint_descriptor_t* endp;
/* Transfer Types */
static char *TransferType[4] =
{
    "Control",
    "Isochronous",
    "Bulk",
    "Interrupt"
};
/* Sync Types */
static char *SyncType[4] =
{
    "No synchronization",
    "Asynchronous",
    "Adaptive",
    "Synchronous"
};
/* Data Types */
static char *DataType[4] =
{
    "Data endpoint",
    "Feedback endpoint",
    "Implicit feedback",
    "Reserved"
};

/* Input Terminal types */
static char *it_type_string[NUMBER_OF_IT_TYPE] =
{
    "Input Undefined",
    "Microphone",
    "Desktop Microphone",
    "Personal Microphone",
    "Omni directional Microphone",
    "Microphone array",
    "Processing Microphone array",
};

/* Output Terminal types */
static char *ot_type_string[NUMBER_OF_OT_TYPE] =
{
    "Output Undefined",
    "Speaker",
    "Headphones",
    "Head Mounted Display Audio",
    "Desktop Speaker",
    "Room Speaker",
    "Communication Speaker",
    "Low Frequency Effects Speaker",
};
static char *device_string;
uint8_t g_interface_control_number = 0;
uint8_t g_interface_stream_number = 0;
extern uint8_t g_interface_keyboard_number;

usb_device_interface_struct_t* g_interface_control_info[USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];
usb_device_interface_struct_t* g_interface_stream_info[USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION];

/*
 ** Globals
 */

/* Table of driver capabilities this application wants to use */
static usb_host_driver_info_t DriverInfoTable[] =
{
    {
        {0x00,0x00},                  /* Vendor ID per USB-IF             */
        {0x00,0x00},                  /* Product ID per manufacturer      */
        USB_CLASS_AUDIO,              /* Class code                       */
        USB_SUBCLASS_AUD_CONTROL,     /* Sub-Class code                   */
        0xFF,                         /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_audio_control_event  /* Application call back function   */
    },
    {
        {0x00,0x00},                  /* Vendor ID per USB-IF             */
        {0x00,0x00},                  /* Product ID per manufacturer      */
        USB_CLASS_AUDIO,              /* Class code                       */
        USB_SUBCLASS_AUD_STREAMING,   /* Sub-Class code                   */
        0xFF,                         /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_audio_stream_event   /* Application call back function   */
    },  
    {
         {0x00,0x00},                  /* Vendor ID per USB-IF             */
         {0x00,0x00},                  /* Product ID per manufacturer      */
         USB_CLASS_HID,                /* Class code                       */
         USB_SUBCLASS_HID_NONE,        /* Sub-Class code                   */
         USB_PROTOCOL_HID_NONE,        /* Protocol                         */
         0,                            /* Reserved                         */
         usb_host_hid_keyboard_event   /* Application call back function   */
    },
    /* USB 1.1 hub */
    {

       {0x00,0x00},                    /* Vendor ID per USB-IF             */
        {0x00,0x00},                   /* Product ID per manufacturer      */
       USB_CLASS_HUB,                  /* Class code                       */
       USB_SUBCLASS_HUB_NONE,          /* Sub-Class code                   */
       USB_PROTOCOL_HUB_ALL,           /* Protocol                         */
      0,                               /* Reserved                         */
       usb_host_hub_device_event       /* Application call back function   */
    },
    {
        {0x00,0x00},                   /* All-zero entry terminates        */
        {0x00,0x00},                   /* driver info list.                */
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
#define TIMER_TASK         (15)

extern void main_task(uint32_t param);
extern void audio_task(uint32_t);
extern void sdcard_task(uint32_t temp);
extern void shell_task(uint32_t temp);
extern void keyboard_task(uint32_t param);
extern void keyboard_task_stun(uint32_t param);
extern void timer_task(uint32_t param);
extern void timer_task_stun(uint32_t param);
extern usb_status usb_class_audio_get_alternative_interface_descriptors_and_sync
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,

    usb_device_interface_struct_t* default_intf_handle,

    /* [IN] pointer to interface descriptor */
    usb_device_interface_struct_t* intf_handle,

    void* alternative_interface_descriptor
    );

os_event_handle usb_audio_fu_request;
os_event_handle usb_keyboard_event;
os_event_handle sd_card_event;
os_event_handle usb_timer_event;
#define timer_out_event           0x01
static int errcount = 0;
#define AUDIO_SPEAKER_FREQUENCY (1000) /* Frequency in Hz*/
/*FUNCTION*---------------------------------------------------------------
 *
 * Function Name : audio_timer_isr
 * Comments  : Callback called by hwtimer every elapsed period
 *
 *END*----------------------------------------------------------------------*/
void audio_timer_isr(uint32_t channel)
{
    OS_Event_set(usb_timer_event, timer_out_event);

}

#if defined (KL25Z4_SERIES) || defined (KL46Z4_SERIES)
void PIT_IRQHandler(void)
{
    uint32_t i;
    for(i=0; i < FSL_FEATURE_PIT_TIMER_COUNT; i++)
    {
        if (PIT_HAL_IsIntPending(g_pitBase[0], i))
        {
            /* Clear interrupt flag.*/
            PIT_HAL_ClearIntFlag(g_pitBase[0], i);
        }
    }
    audio_timer_isr(0);
}
#else
void PIT0_IRQHandler(void)
{
    /* Clear interrupt flag.*/
    PIT_HAL_ClearIntFlag(g_pitBase[0], 0U);
    audio_timer_isr(0);
}
#endif

/* Test single timer period and interrupt.*/
void pit_single_timer_init()
{
    /* Change the timer period here in unit of microseconds.*/
    uint32_t timerPeriod = 1000;

    /* Structure of initialize PIT channel 0 */
    pit_user_config_t
    pitTestInit =
    {
        .isInterruptEnabled = true,
        .periodUs = timerPeriod
    };

#if defined (KL25Z4_SERIES) || defined (KL46Z4_SERIES)
    NVIC_SetPriority(PIT_IRQn, 4);

    OS_install_isr(PIT_IRQn, PIT_IRQHandler, NULL);
#else 
    NVIC_SetPriority(PIT0_IRQn, 4);

    OS_install_isr(PIT0_IRQn, PIT0_IRQHandler, NULL);
#endif

    /* Init pit module and enable run in debug */
    PIT_DRV_Init(0, true);

    /* Initialize PIT timer 0 */
    PIT_DRV_InitChannel(0, 0, &pitTestInit);

    /*Register pit isr callback function.*/
    //PIT_DRV_InstallCallback(0, 0, audio_timer_isr);
}


void timer_task(uint32_t param)
{
    static uint16_t index = 0;
    if (OS_Event_check_bit(usb_timer_event, timer_out_event))
    {
        OS_Event_clear(usb_timer_event, timer_out_event);
        if (g_interface_stream_number == 0)
            return;
        audio_com.callback_fn = usb_host_audio_tr_callback;
        audio_com.callback_param = 0;
        {
            if (USB_OK != usb_class_audio_send_data(&audio_com, (unsigned char
                *)&wav_buff[packet_size * index], packet_size))
                errcount++;
            index++;
            if (index == NUMBER_OF_BUFFER)
                index = 0;
        }
        //audio_stream.dev_state = USB_DEVICE_IDLE;
    }
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_khci_task_stun
 *  Returned Value : none
 *  Comments       :
 *        KHCI task
 *END*-----------------------------------------------------------------*/
void timer_task_stun(uint32_t param)
{
    while (1)
    {
        OS_Event_wait(usb_timer_event, timer_out_event, FALSE, 0);
        timer_task(param);
}
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : main (Main_Task if using MQX)
 * Returned Value : none
 * Comments       :
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/
void APP_init(void)
{ /* Body */
    usb_status status = USB_OK;
    uint32_t task_id;
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
    usb_int_dis();
#endif 
    /*
     ** It means that we are going to act like host, so we initialize the
     ** host stack. This call will allow USB system to allocate memory for
     ** data structures, it uses later (e.g pipes etc.).
     */
    status = usb_host_init(CONTROLLER_ID,usb_host_board_init, /*  */
    &host_handle); /* Returned pointer */
    if (status != USB_OK)
    {
        USB_PRINTF("\n\rUSB Host Initialization failed. STATUS: %x", status);

    }
    /*
     ** Since we are going to act as the host driver, register the driver
     ** information for wanted class/subclass/protocols
     */
    status = usb_host_register_driver_info(
        host_handle,
        DriverInfoTable
        );
    if (status != USB_OK)
    {
        USB_PRINTF("\n\rDriver Registration failed. STATUS: %x", status);
    }

    /* Create lwevents*/
    usb_keyboard_event = OS_Event_create(0);/* manually clear */
    usb_audio_fu_request = OS_Event_create(0);/* manually clear */
#if (defined _MCF51MM256_H) || (defined _MCF51JE256_H)
    usb_int_en();
#endif     
    if (NULL == (g_cur_mute = (int8_t *)OS_Mem_alloc_uncached_zero(4)))
    {
        USB_PRINTF("\r\n memory allocation failed.\r\n");
        return;
    }
    if (NULL == (g_cur_mute = (int8_t *)OS_Mem_alloc_uncached_zero(4)))
    {
        USB_PRINTF("\r\n memory allocation failed.\r\n");
        return;
    }
    if (NULL == (g_cur_vol = (int8_t *)OS_Mem_alloc_uncached_zero(4)))
    {
        USB_PRINTF("\r\n memory allocation failed.\r\n");
        return;
    }
    if (NULL == (g_max_vol = (int8_t *)OS_Mem_alloc_uncached_zero(4)))
    {
        USB_PRINTF("\r\n memory allocation failed.\r\n");
        return;
    }
    if (NULL == (g_min_vol = (int8_t *)OS_Mem_alloc_uncached_zero(4)))
    {
        USB_PRINTF("\r\n memory allocation failed.\r\n");
        return;
    }
    if (NULL == (g_res_vol = (int8_t *)OS_Mem_alloc_uncached_zero(4)))
    {
        USB_PRINTF("\r\n memory allocation failed.\r\n");
        return;
    }
    if (NULL == (endp = (endpoint_descriptor_t*)OS_Mem_alloc_uncached_zero(sizeof(endpoint_descriptor_t))))
    {
        USB_PRINTF("\nMemory allocation for current sampling failed.\n");
        return;
    }

    USB_ctr_Event = OS_Event_create(0);
    usb_timer_event = OS_Event_create(0);

    task_id = OS_Task_create((task_start_t)keyboard_task_fuc, (void*)host_handle, (uint32_t)5, 4000, (char*)"Keyboard", NULL);
    if (task_id == 0)
    {
        return;
    }

    task_id = OS_Task_create((task_start_t)timer_task_fuc, (void*)host_handle, (uint32_t)4, 2000, (char*)"timer", NULL);
    if (task_id == 0)
    {
        return;
    }
    pit_single_timer_init();

    USB_PRINTF("USB Audio Speaker Host Demo\n\r");
    USB_PRINTF("\r\n");
    USB_PRINTF("!!Please select the device audio speaker key map MACRO in the hidkeyboard.c and recompile the example, the default device is EDIFIER R18USB.\r\n");
    /* The main task has done its job, so exit */
} /* Endbody */
typedef enum
{
    AUDIO_GET_MIN_VOLUME,
    AUDIO_GET_MAX_VOLUME,
    AUDIO_GET_RES_VOLUME,
    AUDIO_CONFIG_CHANNEL,
    AUDIO_CONFIG_CHANNEL1_VOL,
    AUDIO_CONFIG_CHANNEL2_VOL,
    AUDIO_INCREASE_VOLUME,
    AUDIO_DECREASE_VOLUME,
    AUDIO_DONE,
    AUDIO_NONE
} audio_control_state_t;
static audio_control_state_t audio_statue = AUDIO_GET_MIN_VOLUME;
static feature_control_struct_t control_feature;
uint32_t freq;
void APP_task()
{
    usb_status status = USB_OK;

    uint8_t bsamfreqtype_index;
    if (OS_Event_check_bit(USB_ctr_Event, USB_EVENT_CTRL))
    {
        OS_Event_clear(USB_ctr_Event, USB_EVENT_CTRL);
        if (g_interface_control_number == 0)
            return;
        status = usb_host_open_dev_interface(host_handle, audio_control.dev_handle, audio_control.intf_handle, (usb_class_handle *)&audio_control.class_handle);
        if (status != USB_OK)
        {
            USB_PRINTF("\n\rError in _usb_host_open_dev_interface: %x\n\r", status);
            return;
        } /* Endif */
        audio_stream.dev_state = USB_DEVICE_INTERFACED;
    }
    if (OS_Event_check_bit(USB_ctr_Event, USB_EVENT_CTRL_DETACH))
    {
        OS_Event_clear(USB_ctr_Event, USB_EVENT_CTRL_DETACH);
        g_interface_control_number = 0;
        status = usb_host_close_dev_interface(host_handle, audio_control.dev_handle, audio_control.intf_handle, audio_control.class_handle);
        if (status != USB_OK)
        {
            USB_PRINTF("\n\rError in _usb_host_close_dev_interface: %x\n\r", status);
        } /* Endif */
        audio_control.dev_handle = NULL;
        audio_control.intf_handle = NULL;
        audio_control.dev_state = USB_DEVICE_DETACHED;

        device_direction = USB_AUDIO_DEVICE_DIRECTION_UNDEFINE;

    }
    if (OS_Event_check_bit(USB_ctr_Event, USB_EVENT_STREAM_DETACH))
    {
        OS_Event_clear(USB_ctr_Event, USB_EVENT_STREAM_DETACH);

        g_interface_stream_number = 0;
        status = usb_host_close_dev_interface(host_handle, audio_stream.dev_handle, audio_stream.intf_handle, audio_stream.class_handle);
        if (status != USB_OK)
        {
            USB_PRINTF("error in _usb_hostdev_close_interface %x\n\r", status);
        }
        audio_stream.dev_handle = NULL;
        audio_stream.intf_handle = NULL;
        audio_stream.dev_state = USB_DEVICE_DETACHED;
        PIT_DRV_StopTimer(0,0);

    }
    if (USB_DEVICE_INUSE == audio_stream.dev_state)
    {
        switch(audio_statue)
        {
        case AUDIO_INCREASE_VOLUME:
            if (OS_Event_check_bit(usb_audio_fu_request, FU_VOLUME_MASK))
            {
                OS_Event_clear(usb_audio_fu_request, FU_VOLUME_MASK);
                if (increase_times <= frm_type_desc->bnrchannels)
                    audio_increase_volume_command(increase_times);
                else
                {
                    increase_times = 1;
                    audio_statue = AUDIO_GET_MIN_VOLUME;

                }
            }
            break;
        case AUDIO_DECREASE_VOLUME:
            if (OS_Event_check_bit(usb_audio_fu_request, FU_VOLUME_MASK))
            {
                OS_Event_clear(usb_audio_fu_request, FU_VOLUME_MASK);
                if (decrease_times <= frm_type_desc->bnrchannels)
                    audio_decrease_volume_command(decrease_times);
                else
                {
                    decrease_times = 1;
                    audio_statue = AUDIO_GET_MIN_VOLUME;

                }
            }
            break;
        default:
            break;
        }
    }

    if (USB_DEVICE_INTERFACED == audio_stream.dev_state)
    {

        /* Get min volume */
        switch(audio_statue)
        {
        case AUDIO_GET_MIN_VOLUME:
            OS_Event_clear(usb_audio_fu_request, FU_VOLUME_MASK);
            audio_com.class_control_handle = (usb_class_handle)audio_control.class_handle;
            audio_com.callback_fn = usb_host_audio_request_ctrl_callback;
            control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
            audio_com.callback_param = (void *)&control_feature;
            if (USB_OK == usb_class_audio_feature_command(&audio_com, 1, (void *)g_min_vol, USB_AUDIO_GET_MIN_VOLUME))
                audio_statue = AUDIO_GET_MAX_VOLUME;
            else
                audio_statue = AUDIO_DONE;
            USB_PRINTF("AUDIO_GET_MIN_VOLUME\n\r");
            break;

        case AUDIO_GET_MAX_VOLUME:
            if (OS_Event_check_bit(usb_audio_fu_request, FU_VOLUME_MASK))
            {
                OS_Event_clear(usb_audio_fu_request, FU_VOLUME_MASK);
                /* Get max volume */
                if (USB_OK == usb_class_audio_feature_command(&audio_com, 1, (void *)g_max_vol, USB_AUDIO_GET_MAX_VOLUME))
                    audio_statue = AUDIO_GET_RES_VOLUME;
                else
                    audio_statue = AUDIO_DONE;
                USB_PRINTF("AUDIO_GET_MAX_VOLUME\n\r");
            }
            break;

        case AUDIO_GET_RES_VOLUME:
            /* Get res volume */
            if (OS_Event_check_bit(usb_audio_fu_request, FU_VOLUME_MASK))
            {
                OS_Event_clear(usb_audio_fu_request, FU_VOLUME_MASK);
                if (USB_OK == usb_class_audio_feature_command(&audio_com, 1, (void *)g_res_vol, USB_AUDIO_GET_RES_VOLUME))
                    audio_statue = AUDIO_CONFIG_CHANNEL;
                else
                    audio_statue = AUDIO_DONE;
            }
            break;

        case AUDIO_CONFIG_CHANNEL:
            if (OS_Event_check_bit(usb_audio_fu_request, FU_VOLUME_MASK))
            {
                OS_Event_clear(usb_audio_fu_request, FU_VOLUME_MASK);
                min_volume = (g_min_vol[1] << 8) | (g_min_vol[0]);
                max_volume = (g_max_vol[1] << 8) | (g_max_vol[0]);
                res_volume = (g_res_vol[1] << 8) | (g_res_vol[0]);
                USB_PRINTF("  %x  %x  Res %x \r\n", (uint32_t) * (uint32_t *)g_min_vol, (uint32_t) * (uint32_t *)g_max_vol, res_volume);
                control_feature.FU = USB_AUDIO_CTRL_FU_MUTE;
                /* Synchronize host volume and device volume */
                device_volume_step = (int16_t)(((int16_t)(max_volume) - (int16_t)(min_volume)) / (HOST_MAX_VOLUME - HOST_MIN_VOLUME));
                cur_volume = (int16_t)(min_volume + device_volume_step * host_cur_volume);
                /* Calculate physical volume(dB) */
                //physic_volume = ((int16_t)(cur_volume)*39)/10000;
                g_cur_vol[0] = (int8_t)((int16_t)(cur_volume) & 0x00FF);
                g_cur_vol[1] = (int8_t)((int16_t)(cur_volume) >> 8);
                //config_channel(fu_desc,frm_type_desc->bnrchannels);
                g_cur_mute[0] = FALSE;
                usb_class_audio_feature_command(&audio_com, 0, (void *)g_cur_mute, USB_AUDIO_SET_CUR_MUTE);
                audio_statue = AUDIO_CONFIG_CHANNEL1_VOL;
            }
            break;

        case AUDIO_CONFIG_CHANNEL1_VOL:
            if (OS_Event_check_bit(usb_audio_fu_request, FU_MUTE_MASK))
            {
                OS_Event_clear(usb_audio_fu_request, FU_MUTE_MASK);
                control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
                audio_com.callback_param = (void *)&control_feature;
                usb_class_audio_feature_command(&audio_com, 1, (void *)g_cur_vol, USB_AUDIO_SET_CUR_VOLUME);
                audio_statue = AUDIO_CONFIG_CHANNEL2_VOL;
            }
            break;

        case AUDIO_CONFIG_CHANNEL2_VOL:
            if (OS_Event_check_bit(usb_audio_fu_request, FU_VOLUME_MASK))
            {
                OS_Event_clear(usb_audio_fu_request, FU_VOLUME_MASK);
                control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
                audio_com.callback_param = (void *)&control_feature;
                usb_class_audio_feature_command(&audio_com, 2, (void *)g_cur_vol, USB_AUDIO_SET_CUR_VOLUME);
                audio_statue = AUDIO_DONE;
            }
            break;

        case AUDIO_INCREASE_VOLUME:
            audio_statue = AUDIO_DONE;
            break;

        case AUDIO_DECREASE_VOLUME:
            audio_statue = AUDIO_DONE;
            break;

        case AUDIO_DONE:
            /* Audio device information */
            USB_PRINTF("Audio device information:\r\n");
            USB_PRINTF("   - Device type    : %s\n\r", device_string);
            for (bsamfreqtype_index = 0; bsamfreqtype_index < frm_type_desc->bsamfreqtype; bsamfreqtype_index++)
            {
                USB_PRINTF("   - Frequency device support      : %d Hz\n\r", ((frm_type_desc->tsamfreq[bsamfreqtype_index][2]) << 16) |
                ((frm_type_desc->tsamfreq[bsamfreqtype_index][1]) << 8) |
                ((frm_type_desc->tsamfreq[bsamfreqtype_index][0]) << 0));
            }
            USB_PRINTF("   - Bit resolution : %d bits\n\r", frm_type_desc->bbitresolution);
            USB_PRINTF("   - Number of channels : %d channels\n\r", frm_type_desc->bnrchannels);
            USB_PRINTF("   - Transfer type : %s\n\r", TransferType[(endp->bmAttributes) & EP_TYPE_MASK]);
            USB_PRINTF("   - Sync type : %s\n\r", SyncType[(endp->bmAttributes >> 2) & EP_TYPE_MASK]);
            USB_PRINTF("   - Usage type : %s\n\r", DataType[(endp->bmAttributes >> 4) & EP_TYPE_MASK]);
            if (device_direction == USB_AUDIO_DEVICE_DIRECTION_OUT)
            {
                USB_PRINTF("The device is unsupported!\r\n");
            }
            else
            {
                USB_PRINTF("This audio device supports play audio files with these properties:\r\n");
                USB_PRINTF("   - Sample rate    :\r\n");
                for (bsamfreqtype_index = 0; bsamfreqtype_index < frm_type_desc->bsamfreqtype; bsamfreqtype_index++)
                {
                    USB_PRINTF("                    : %d Hz\n\r", ((frm_type_desc->tsamfreq[bsamfreqtype_index][2]) << 16) |
                    ((frm_type_desc->tsamfreq[bsamfreqtype_index][1]) << 8) |
                    ((frm_type_desc->tsamfreq[bsamfreqtype_index][0]) << 0));
                }
                USB_PRINTF("   - Sample size    : %d bits\n\r", frm_type_desc->bbitresolution);
                USB_PRINTF("   - Number of channels : %d channels\n\r", frm_type_desc->bnrchannels);
                USB_PRINTF("USB Speaker example will loop playback %dk_%dbit_%dch format aduio.\r\n",
                (((frm_type_desc->tsamfreq[0][2]) << 16) |
                ((frm_type_desc->tsamfreq[0][1]) << 8) |
                ((frm_type_desc->tsamfreq[0][0]) << 0))/1000,
                frm_type_desc->bbitresolution,
                frm_type_desc->bnrchannels);
            }
            freq = ((frm_type_desc->tsamfreq[0][2]) << 16) |
            ((frm_type_desc->tsamfreq[0][1]) << 8) |
            ((frm_type_desc->tsamfreq[0][0]) << 0);
            usb_class_audio_endpoint_command(&audio_com, &freq, USB_AUDIO_SET_CUR_SAMPLING_FREQ);
            /* After get information of audio interface, make HID interface is ready to use */
            audio_stream.dev_state = USB_DEVICE_INUSE;
            packet_size = USB_Audio_Get_Packet_Size(frm_type_desc, 0);
            USB_Prepare_Data();
            PIT_DRV_StartTimer(0, 0);

            audio_statue = AUDIO_GET_MIN_VOLUME;
            break;

        default:
            break;

        }
    }
}
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : main (Main_Task if using MQX)
 * Returned Value : none
 * Comments       :
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/

void main_task(uint32_t param)
{
    APP_init();

    /*
     ** Infinite loop, waiting for events requiring action
     */
    for (;;)
    {
        APP_task();

    } /* Endfor */
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : USB_Prepare_Data
 * Returned Value : None
 * Comments       :
 *    This function prepares data to send.
 *
 *END*--------------------------------------------------------------------*/
static void USB_Prepare_Data(void)
{
    uint32_t resolution_size = packet_size >> 5;
    static uint32_t audio_position = 0;
    static uint8_t index = 0;
    uint8_t k, j=0;
    /* copy data to buffer */

    for(k = 0; k < 32; k++, audio_position++)
    {
        for(j = 0; j < resolution_size; j++)
        wav_buff[index*packet_size+j*32+k] = wav_data[audio_position];

    }
    index ++;
    if(index == NUMBER_OF_BUFFER)
    index = 0;
    if(audio_position >= 112000)
    audio_position = 0;
}


/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : check_device_type
 * Returned Value : None
 * Comments       :
 *    This function check whether the attached device is out-device or in-device.
 *
 *END*--------------------------------------------------------------------*/
usb_status check_device_type
(
    /* [IN] Input terminal descriptor */
    usb_audio_ctrl_desc_it_t* it_desc,

    /* [IN] Output terminal descriptor */
    usb_audio_ctrl_desc_ot_t* ot_desc,

    /* [OUT] Terminal type name */
    char * *device_type,

    /* [OUT] device direction */
    uint8_t *direction
)
{
    unsigned char it_type_high, it_type_low, ot_type_low;

    it_type_high = it_desc->wterminaltype[0];
    it_type_low = it_desc->wterminaltype[1];
    ot_type_low = ot_desc->wterminaltype[1];

    /* Input terminal associates with audio streaming */
    if (USB_TERMINAL_TYPE == it_type_low)
    {
        *direction = USB_AUDIO_DEVICE_DIRECTION_IN;
    }
    /* Input terminal type */
    else if (INPUT_TERMINAL_TYPE == it_type_low)
    {
        /* get type device name */
        *device_type = it_type_string[it_type_high];
    }
    else
    {
        return USBERR_ERROR;
    }

    /* Output terminal associates with audio streaming */
    if (USB_TERMINAL_TYPE == ot_type_low)
    {
        if (USB_AUDIO_DEVICE_DIRECTION_IN == (*direction))
        {
            *direction = USB_AUDIO_DEVICE_DIRECTION_UNDEFINE;
            return USBERR_ERROR;
        }
        else
        {
            *direction = USB_AUDIO_DEVICE_DIRECTION_OUT;
        }
    }
    /* Output terminal type */
    else if (OUTPUT_TERMINAL_TYPE == ot_type_low)
    {
        /* get type device name */
        *device_type = ot_type_string[it_type_high];
    }
    else
    {
        return USBERR_ERROR;
    }
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : USB_Audio_Get_Packet_Size
 * Returned Value : None
 * Comments       :
 *     This function gets the packet size to send to the device each mS.
 *
 *END*--------------------------------------------------------------------*/
uint32_t USB_Audio_Get_Packet_Size
(
    /* [IN] Point to format type descriptor */
    usb_audio_stream_desc_format_type_t* format_type_desc,
    uint8_t bsamfreqtype_index
)
{
    uint32_t packet_size_tmp;
    /* calculate packet size to send to the device each mS.*/
    /* packet_size = (sample frequency (Hz) /1000) * (bit resolution/8) * number of channels */
    packet_size_tmp = ((((format_type_desc->tsamfreq[bsamfreqtype_index][2]) << 16) |
    ((format_type_desc->tsamfreq[bsamfreqtype_index][1]) << 8) |
    ((format_type_desc->tsamfreq[bsamfreqtype_index][0]) << 0))
    * (format_type_desc->bbitresolution / 8)
    * (format_type_desc->bnrchannels) / 1000);
    return (packet_size_tmp);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_audio_request_ctrl_callback
 * Returned Value : None
 * Comments       :
 *     Called when a mute request is sent successfully.
 *
 *END*--------------------------------------------------------------------*/
void usb_host_audio_request_ctrl_callback
(
    /* [IN] pointer to pipe */
    usb_pipe_handle pipe_handle,

    /* [IN] user-defined parameter */
    void *user_parm,

    /* [IN] buffer address */
    unsigned char *buffer,

    /* [IN] length of data transferred */
    uint32_t buflen,

    /* [IN] status, hopefully USB_OK or USB_DONE */
    uint32_t status
)
{ /* Body */
    static feature_control_struct_t* control_feature_ptr;
    control_feature_ptr = (feature_control_struct_t*)user_parm;

    switch(control_feature_ptr->FU)
    {
    case USB_AUDIO_CTRL_FU_MUTE:
        {
        OS_Event_set(usb_audio_fu_request, FU_MUTE_MASK);
        break;
    }
    case USB_AUDIO_CTRL_FU_VOLUME:
        {
        OS_Event_set(usb_audio_fu_request, FU_VOLUME_MASK);
        break;
    }
    default:
        break;
    }
    //fflush(stdout);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_audio_tr_callback
 * Returned Value : None
 * Comments       :
 *     Called when a ISO packet is sent/received successfully.
 *
 *END*--------------------------------------------------------------------*/
void usb_host_audio_tr_callback(
    /* [IN] pointer to pipe */
    usb_pipe_handle pipe_handle,

    /* [IN] user-defined parameter */
    void *user_parm,

    /* [IN] buffer address */
    unsigned char *buffer,

    /* [IN] length of data transferred */
    uint32_t buflen,

    /* [IN] status, hopefully USB_OK or USB_DONE */
    uint32_t status
)
{

    USB_Prepare_Data();
    if (status == USB_OK)
        audio_stream.dev_state = USB_DEVICE_INUSE;
}

usb_interface_descriptor_handle audio_control_get_interface()
{
    return (usb_interface_descriptor_handle)(g_interface_control_info[0]);
}

usb_interface_descriptor_handle audio_stream_get_interface()
{
    return (usb_interface_descriptor_handle)(g_interface_stream_info[0]);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_audio_control_event
 * Returned Value : usb_status
 * Comments       :
 *     Called when control interface has been attached, detached, etc.
 *END*--------------------------------------------------------------------*/
usb_status usb_host_audio_control_event
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,

    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,

    /* [IN] code number for event causing callback */
    uint32_t event_code
)
{
    usb_device_interface_struct_t* pHostIntf = (usb_device_interface_struct_t*)intf_handle;
    interface_descriptor_t* intf_ptr = pHostIntf->lpinterfaceDesc;

    switch(event_code)
    {
    case USB_CONFIG_EVENT:
        if ((audio_control.dev_state == USB_DEVICE_ATTACHED))
        {
            audio_control.dev_handle = dev_handle;
            audio_control.intf_handle = audio_control_get_interface();
            audio_control.dev_state = USB_DEVICE_CONFIGURED;
        }
        else
        {
            USB_PRINTF("Audio device already attached\n\r");
            //fflush(stdout);
        }
        USB_PRINTF("----- Audio control interface: USB_CONFIG_EVENT -----\r\n");
        break;
        /* Drop through into attach, same processing */
    case USB_ATTACH_EVENT:
        /* initialize new interface members and select this interface */
        g_interface_control_info[g_interface_control_number] = pHostIntf;
        g_interface_control_number++;
        audio_control.dev_state = USB_DEVICE_ATTACHED;
        USB_PRINTF("----- Audio control interface: attach event -----\r\n");
        //fflush(stdout);
        USB_PRINTF("State = attached");
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d\n\r", intf_ptr->bInterfaceProtocol);
        break;

    case USB_INTF_OPENED_EVENT:
        {
        USB_PRINTF("----- Audio control interface: interface event -----\r\n");
        usb_audio_ctrl_desc_header_t* header_desc = NULL;
        usb_audio_ctrl_desc_it_t* it_desc = NULL;
        usb_audio_ctrl_desc_ot_t* ot_desc = NULL;

        /* finds all the descriptors in the configuration */
        if (USB_OK != usb_class_audio_control_get_descriptors(dev_handle,
            intf_handle,
            &header_desc,
            &it_desc,
            &ot_desc,
            &fu_desc))
        {
            ;  //break;
        };

        /* set all info got from descriptors to the class interface struct */
        usb_class_audio_control_set_descriptors(audio_control.class_handle,
            header_desc, it_desc, ot_desc, fu_desc);

        if (USB_OK != check_device_type(it_desc, ot_desc, &device_string, &device_direction))
        {
            break;
        }
        device_direction = USB_AUDIO_DEVICE_DIRECTION_IN;
        if (g_interface_number != 0)
        {
            OS_Event_set(usb_keyboard_event, USB_Keyboard_Event_CTRL);
        }
        audio_stream.dev_state = USB_DEVICE_INUSE;
    }
        break;

    case USB_DETACH_EVENT:
        USB_PRINTF("----- Audio control interface: detach event -----\r\n");
        USB_PRINTF("State = detached");
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d\n\r", intf_ptr->bInterfaceProtocol);

        OS_Event_set(USB_ctr_Event, USB_EVENT_CTRL_DETACH);

        break;

    default:
        USB_PRINTF("Audio Device: unknown control event\n\r");
        //fflush(stdout);
        break;
    }
    
    return USB_OK;
    //fflush(stdout);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_audio_stream_event
 * Returned Value : usb_status
 * Comments       :
 *     Called when stream interface has been attached, detached, etc.
 *END*--------------------------------------------------------------------*/
usb_status usb_host_audio_stream_event
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,

    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,

    /* [IN] code number for event causing callback */
    uint32_t event_code
)
{ /* Body */
    usb_device_interface_struct_t* pHostIntf = (usb_device_interface_struct_t*)intf_handle;
    interface_descriptor_t* intf_ptr = pHostIntf->lpinterfaceDesc;
    switch(event_code)
    {
    case USB_CONFIG_EVENT:
        {
        usb_audio_stream_desc_spepific_as_if_t* as_itf_desc = NULL;
        usb_audio_stream_desc_specific_iso_endp_t* iso_endp_spec_desc = NULL;
        uint32_t param1 = 0;
        uint32_t param2 = 0;
        void* alternative_interface_descriptor = NULL;
        if (intf_handle != audio_stream_get_interface())
            return USB_OK;

        if (pHostIntf->alternate_setting_num != 0)
        {
            g_interface_stream_info[g_interface_stream_number] = (usb_device_interface_struct_t*)OS_Mem_alloc_uncached_zero(sizeof(usb_device_interface_struct_t));
            param1 = (uint32_t)((uint32_t)(g_interface_stream_info[0]->lpinterfaceDesc->bInterfaceNumber << 8) | (0x00000001));
            usb_host_get_dev_descriptor(intf_handle, USB_DESC_TYPE_IF, &param1, &param2, &alternative_interface_descriptor);
                          usb_class_audio_get_alternative_interface_descriptors_and_sync(dev_handle,(usb_device_interface_struct_t*)intf_handle, g_interface_stream_info[g_interface_stream_number],alternative_interface_descriptor);

            if ((audio_stream.dev_state == USB_DEVICE_IDLE) || (audio_stream.dev_state == USB_DEVICE_DETACHED))
            {
                audio_stream.dev_handle = dev_handle;
                audio_stream.intf_handle = intf_handle;
                audio_stream.dev_state = USB_DEVICE_CONFIGURED;
            }
            else
            {
                USB_PRINTF("Audio device already attached\n\r");
            }

            /* finds all the descriptors in the configuration */
            if (USB_OK != usb_class_audio_stream_get_descriptors(dev_handle,
                g_interface_stream_info[g_interface_stream_number],
                &as_itf_desc,
                &frm_type_desc,
                &iso_endp_spec_desc))
            {
                USB_PRINTF("usb_class_audio_stream_get_descriptors fail!\r\n");
                break;
            };

            endp->bmAttributes = iso_endp_spec_desc->bmattributes;
            /* initialize new interface members and select this interface */
            if (USB_OK != usb_host_open_dev_interface(host_handle, dev_handle,
                intf_handle, (usb_class_handle *)&audio_stream.class_handle))
            {

                USB_PRINTF("----- Audio stream interface: _usb_host_open_dev_interface fail\n\r");
            }
            audio_com.class_stream_handle = (usb_class_handle)audio_stream.class_handle;
            /* set all info got from descriptors to the class interface struct */
            usb_class_audio_stream_set_descriptors(audio_stream.class_handle,
                as_itf_desc, frm_type_desc, iso_endp_spec_desc);
            USB_PRINTF("----- Audio stream interface: USB_CONFIG_EVENT -----\r\n");
        }
        //g_interface_stream_number++;
    }
        break;
        /* Drop through into attach, same processing */
    case USB_ATTACH_EVENT:
        g_interface_stream_info[g_interface_stream_number] = pHostIntf;
        g_interface_stream_number++;
        USB_PRINTF("----- Audio stream interface: attach event -----\r\n");
        //fflush(stdout);
        USB_PRINTF("State = attached");
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d\n\r", intf_ptr->bInterfaceProtocol);
        USB_PRINTF("  Class = %d", intf_ptr->bAlternateSetting);
        //fflush(stdout);
        break;

    case USB_INTF_OPENED_EVENT:
        OS_Event_set(USB_ctr_Event, USB_EVENT_CTRL);
        break;

    case USB_DETACH_EVENT:
        if (intf_handle != audio_stream_get_interface())
            return USB_OK;
        audio_stream.dev_handle = dev_handle;
        audio_stream.intf_handle = intf_handle;
        OS_Event_set(USB_ctr_Event, USB_EVENT_STREAM_DETACH);
        OS_Mem_free(g_interface_stream_info[g_interface_stream_number]);
        g_interface_stream_number = 0;
        USB_PRINTF("----- Audio stream interface: detach event-----\r\n");
        break;

    default:
        USB_PRINTF("Audio device: unknown data event\n\r");
        //fflush(stdout);
        break;
    } /* EndSwitch */
    
    return USB_OK;
} /* Endbody */

/******************************************************************************
 *   @name        config_channel
 *
 *   @brief       Config volume for all channels of speaker
 *
 *   @param       None
 *
 *   @return      None
 *
 ******************************************************************************
 * This function is used to unmute and set volume of audio speaker
 *****************************************************************************/
void config_channel(usb_audio_ctrl_desc_fu_t* fu_ptr, uint8_t num_channel)
{
    uint8_t i;
    for (i = 0; i <= num_channel; i++)
    {
        audio_com.class_control_handle = (usb_class_handle)audio_control.class_handle;
        audio_com.callback_fn = usb_host_audio_request_ctrl_callback;
        control_feature.FU = USB_AUDIO_CTRL_FU_MUTE;
        audio_com.callback_param = (void *)&control_feature;
        g_cur_mute[0] = FALSE;
        usb_class_audio_feature_command(&audio_com, i, (void *)g_cur_mute, USB_AUDIO_SET_CUR_MUTE);
        control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
        audio_com.callback_param = (void *)&control_feature;
        usb_class_audio_feature_command(&audio_com, i, (void *)g_cur_vol, USB_AUDIO_SET_CUR_VOLUME);
    }
}

/******************************************************************************
 *   @name        audio_mute_command
 *
 *   @brief       Mute/unmute audio speaker
 *
 *   @param       None
 *
 *   @return      None
 *
 ******************************************************************************
 * This function is used to mute/unmute audio speaker
 *****************************************************************************/
void audio_mute_command(void)
{
    //feature_control_struct_t control_feature;
    if ((audio_stream.dev_state > USB_DEVICE_DETACHED) || (device_direction != USB_AUDIO_DEVICE_DIRECTION_IN))
    {
        USB_PRINTF("  err: Audio Speaker is not connected\n\r");
    }
    g_cur_mute[0] = !g_cur_mute[0];
    if (g_cur_mute[0])
    {
        //USB_PRINTF("Mute ...\r\n");
    }
    else
    {
        //USB_PRINTF("UnMute ...\r\n");
    }
    /* Send set mute request */
    control_feature.FU = USB_AUDIO_CTRL_FU_MUTE;
    audio_com.class_control_handle = (usb_class_handle)audio_control.class_handle;
    audio_com.callback_fn = usb_host_audio_request_ctrl_callback;
    audio_com.callback_param = &control_feature;
    usb_class_audio_feature_command(&audio_com, 0, (void *)g_cur_mute, USB_AUDIO_SET_CUR_MUTE);
    usb_class_audio_feature_command(&audio_com, 2, (void *)g_cur_mute, USB_AUDIO_SET_CUR_MUTE);
}

/******************************************************************************
 *   @name        audio_increase_volume_command
 *
 *   @brief       Increase audio speaker
 *
 *   @param       None
 *
 *   @return      None
 *
 ******************************************************************************
 * This function is used to increase volume of audio speaker
 *****************************************************************************/
void audio_increase_volume_command(uint8_t channel)
{
    //feature_control_struct_t control_feature;
    static uint32_t i;
    uint8_t max_audio_channel;
    max_audio_channel = frm_type_desc->bnrchannels;
    if (channel > max_audio_channel)
    {
        USB_PRINTF("  err: Channel number larger than max channel\n\r");
        return;
    }
    if ((audio_stream.dev_state > USB_DEVICE_DETACHED) || (device_direction != USB_AUDIO_DEVICE_DIRECTION_IN))
    {
        USB_PRINTF("  err: Audio Speaker is not connected\n\r");
        return;
    }
    if (channel == 1)
    {

        /* Send set mute request */

        if ((host_cur_volume + HOST_VOLUME_STEP) > HOST_MAX_VOLUME)
        {
            host_cur_volume = HOST_MAX_VOLUME;
            i = 0;
        }
        else
        {
            host_cur_volume += HOST_VOLUME_STEP;
            i = 1;
        }
        cur_volume += (int16_t)(i * HOST_VOLUME_STEP * device_volume_step);
        g_cur_vol[0] = (int8_t)((int16_t)(cur_volume) & 0x00FF);
        g_cur_vol[1] = (int8_t)((int16_t)(cur_volume) >> 8);

        audio_stream.dev_state = USB_DEVICE_INUSE;

    }
    audio_com.class_control_handle = (usb_class_handle)audio_control.class_handle;
    audio_com.callback_fn = usb_host_audio_request_ctrl_callback;
    control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
    audio_com.callback_param = &control_feature;

    usb_class_audio_feature_command(&audio_com, channel, (void *)g_cur_vol, USB_AUDIO_SET_CUR_VOLUME);

    increase_times++;
    audio_statue = AUDIO_INCREASE_VOLUME;

    //physic_volume = ((int16_t)(cur_volume)*39)/10000;
    //USB_PRINTF("\n\rCurrent physical volume: %d dB\n\r",physic_volume);

}

/******************************************************************************
 *   @name        audio_decrease_volume_command
 *
 *   @brief       Decrease audio speaker
 *
 *   @param       None
 *
 *   @return      None
 *
 ******************************************************************************
 * This function is used to decrease volume of audio speaker
 *****************************************************************************/
void audio_decrease_volume_command(uint8_t channel)
{
    //feature_control_struct_t control_feature;
    static uint32_t i;
    uint8_t max_audio_channel;
    max_audio_channel = frm_type_desc->bnrchannels;
    if (channel > max_audio_channel)
    {
        USB_PRINTF("  err: Channel number larger than max channel\n\r");
        return;
    }
    if ((audio_stream.dev_state > USB_DEVICE_DETACHED) || (device_direction != USB_AUDIO_DEVICE_DIRECTION_IN))
    {
        USB_PRINTF(" err: Audio Speaker is not connected\n\r");
        return;
    }
    if (channel == 1)
    {

        /* Send set mute request */
        audio_com.class_control_handle = (usb_class_handle)audio_control.class_handle;

        if (host_cur_volume < (HOST_VOLUME_STEP + HOST_MIN_VOLUME))
        {
            host_cur_volume = HOST_MIN_VOLUME;
            i = 0;
        }
        else
        {
            host_cur_volume -= HOST_VOLUME_STEP;
            i = 1;
        }
        cur_volume -= (int16_t)(i * HOST_VOLUME_STEP * device_volume_step);
        g_cur_vol[0] = (int8_t)((int16_t)(cur_volume) & 0x00FF);
        g_cur_vol[1] = (int8_t)((int16_t)(cur_volume) >> 8);

        audio_stream.dev_state = USB_DEVICE_INUSE;
    }
    audio_com.callback_fn = usb_host_audio_request_ctrl_callback;
    control_feature.FU = USB_AUDIO_CTRL_FU_VOLUME;
    audio_com.callback_param = &control_feature;
    // for (i=0 ; i<= max_audio_channel ; i++)
    //{
    usb_class_audio_feature_command(&audio_com, channel, (void *)g_cur_vol, USB_AUDIO_SET_CUR_VOLUME);
    //}

    decrease_times++;
    audio_statue = AUDIO_DECREASE_VOLUME;

    //physic_volume = ((int16_t)(cur_volume)*39)/10000;
    //USB_PRINTF("\n\rCurrent physical volume: %d dB\n\r",physic_volume);
}



#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    {   1L, Main_Task, 2500L, MQX_MAIN_TASK_PRIORITY, "Main", MQX_AUTO_START_TASK},
    {   0L, 0L, 0L, 0L, 0L, 0L}
};
#endif

static void Task_Start(void *arg)
{
#if (USE_RTOS)
    APP_init();

    for (;; )
    {
#endif
    APP_task();
#if (USE_RTOS)
} /* Endfor */
#endif
}

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param)
#else
int main(void)
#endif
{
    hardware_init();
    OSA_Init();
    dbg_uart_init();

#if !(USE_RTOS)
    APP_init();
#endif

    OS_Task_create(Task_Start, NULL, 6L, 3000L, "task_start", NULL);
    OSA_Start();
#if !defined(FSL_RTOS_MQX)
    return 1;
#endif
}

/* EOF */
