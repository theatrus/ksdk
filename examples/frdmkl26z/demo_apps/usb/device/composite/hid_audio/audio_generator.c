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
 * $FileName: audio_generator.c$
 * $Version :
 * $Date    :
 *
 * Comments:
 *
 * @brief  The file emulates a generator.
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "audio_generator.h"
#include "usb_request.h"

#if USBCFG_AUDIO_CLASS_2_0
#error This application requires USBCFG_AUDIO_CLASS_2_0 defined not zero in usb_device_config.h. Please recompile usbd with this option.
#endif

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
#define MAIN_TASK       10
#if (HIGH_SPEED)
#define AUDIO_ENDPOINT_MAX_PACKET_SIZE  (FS_ISO_OUT_ENDP_PACKET_SIZE > HS_ISO_OUT_ENDP_PACKET_SIZE ? FS_ISO_OUT_ENDP_PACKET_SIZE : HS_ISO_OUT_ENDP_PACKET_SIZE)
#else
#define AUDIO_ENDPOINT_MAX_PACKET_SIZE  (FS_ISO_OUT_ENDP_PACKET_SIZE)
#endif

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void USB_Prepare_Data (void);
extern void Main_Task (uint32_t param);
/****************************************************************************
 * Global Variables
 ****************************************************************************/
extern const unsigned char wav_data[];
extern const uint16_t wav_size;

static uint8_t wav_buff[AUDIO_ENDPOINT_MAX_PACKET_SIZE];

uint32_t audio_position = 0;

audio_handle_t* g_audio_handle;
extern uint16_t g_composite_speed;

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
volatile bool start_app = FALSE;
/*****************************************************************************
 * Local Functions
 *****************************************************************************/

/******************************************************************************
 *
 *   @name        USB_Prepare_Data
 *
 *   @brief       This function prepares data to send
 *
 *   @param       None
 *
 *   @return      None
 *****************************************************************************
 * This function prepare data before sending
 *****************************************************************************/
void USB_Prepare_Data (void)
{
    uint8_t k;
    /* copy data to buffer */
    for (k = 0; k < AUDIO_ENDPOINT_MAX_PACKET_SIZE; k++, audio_position++)
    {
        if (audio_position > wav_size)
        {
            audio_position = 0;
        }
        wav_buff[k] = wav_data[audio_position];
    }
}

/*****************************************************************************
 *  
 *   @name        audio_init
 * 
 *   @brief       This function is the entry for Audio generator
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/
void audio_init (void* param)
{
    g_audio_handle = (audio_handle_t*) param;

    while (TRUE)
    {
        while (!start_app)
        {
            OS_Time_delay(1);
            ;
        }
        start_app = FALSE;
        USB_PRINTF("USB_Prepare_Data \r\n");
        USB_Prepare_Data();
        USB_Class_Audio_Send_Data(*g_audio_handle, AUDIO_ENDPOINT, wav_buff,
        g_composite_speed == USB_SPEED_HIGH ? HS_ISO_OUT_ENDP_PACKET_SIZE : FS_ISO_OUT_ENDP_PACKET_SIZE);
    }
}

/******************************************************************************
 * 
 *    @name        Audio_USB_App_Device_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       handle : handle to Identify the controller
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void Audio_USB_App_Device_Callback (uint8_t event_type, void* val, void* arg)
{
    UNUSED_ARGUMENT(arg)
    UNUSED_ARGUMENT(val)

    if ((event_type == USB_DEV_EVENT_BUS_RESET) || (event_type == USB_DEV_EVENT_CONFIG_CHANGED))
    {
        start_app = FALSE;
        if (USB_OK == USB_Class_Audio_Get_Speed(*g_audio_handle, &g_composite_speed))
        {
            USB_Desc_Set_Speed(*g_audio_handle, g_composite_speed);
        }
    }
    else if (event_type == USB_DEV_EVENT_ENUM_COMPLETE)
    {
        start_app = TRUE;

    }
    else if (event_type == USB_DEV_EVENT_ERROR)
    {
        /* add user code for error handling */
    }
}
/******************************************************************************
 * 
 *    @name        Audio_USB_App_Class_Callback
 *    
 *    @brief       This function handles the callback for Get/Set report req  
 *                  
 *    @param       request  :  request type
 *    @param       value    :  give report type and id
 *    @param       data     :  pointer to the data 
 *    @param       size     :  size of the transfer
 *
 *    @return      status
 *                  USB_OK  :  if successful
 *                  else return error
 *
 *****************************************************************************/
uint8_t Audio_USB_App_Class_Callback
(
    uint8_t request,
    uint16_t value,
    uint8_t ** data,
    uint32_t* size,
    void* arg
) 
{
    uint8_t error = USB_OK;

    if ((request == USB_DEV_EVENT_SEND_COMPLETE) && (value == USB_REQ_VAL_INVALID) && (*size != 0xFFFFFFFF))
    {
        if (arg != NULL)
        {
            USB_Prepare_Data();
            USB_Class_Audio_Send_Data(*g_audio_handle, AUDIO_ENDPOINT, wav_buff,
            g_composite_speed == USB_SPEED_HIGH ? HS_ISO_OUT_ENDP_PACKET_SIZE : FS_ISO_OUT_ENDP_PACKET_SIZE);
        }
        return error;
    }

    error = USB_Class_Get_feature(0x0, value, request, data);
    if (error == USBERR_INVALID_REQ_TYPE)
        error = USB_Class_Set_feature(0x0, value, request, data);

    return error;
}

/* EOF */
