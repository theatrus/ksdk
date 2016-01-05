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
 * $FileName: audio_speaker.c$
 * $Version : 3.8.5.0$
 * $Date    : Jul-23-2012$
 *
 * Comments:
 *
 * @brief  The file emulates a speaker.
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "audio_speaker.h"
#include "usb_descriptor.h"
#include "usb_request.h"

#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_port_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include "fsl_soundcard.h"
#include "fsl_sgtl5000_driver.h"
#include "fsl_edma_request.h"
#include "fsl_clock_manager.h"
#include "fsl_sai_driver.h"

static bool first_copy = true;
static bool first_time = true;


void APP_task (void);

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void APP_init (void);

/****************************************************************************
 * Global Variables
 ****************************************************************************/
extern usb_endpoints_t usb_desc_ep;
extern usb_desc_request_notify_struct_t desc_callback;
audio_handle_t g_app_handle;
uint16_t g_app_speed;
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Device_Callback (uint8_t event_type, void* val, void* arg);
void USB_Notif_Callback (uint8_t event_type, void* val, void* arg);
uint8_t USB_App_Class_Callback (uint8_t request, uint16_t value, uint8_t ** data,
    uint32_t* size, void* arg);

/*****************************************************************************
 * Local Variables 
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/


sai_data_format_t sai_format = { 0 };
sgtl_handler_t sgtl_codec_handler_t = { 0 };
static sound_card_t tx_card;
/*static sound_card_t rx_card; */
static sai_data_format_t *format = &sai_format;
static sai_user_config_t tx_config;
/* static sai_user_config_t rx_config; */
static sai_state_t tx_state;
/* static sai_state_t rx_state; */
uint8_t audio_data_buff[DATA_BUFF_SIZE * 4];
#if USEDMA
static edma_state_t edmaState;
static edma_user_config_t edmaUserConfig;
#endif

/* Open the clock and configure the pin mux for sai, i2c. */
void hardware_init_codec (uint8_t instance)
{
    /* Open GPIO clock */

    CLOCK_SYS_EnableSaiClock(0);
    configure_i2s_pins(instance);
    configure_i2c_pins(BOARD_SAI_DEMO_I2C_INSTANCE);
}

/* Initialize the structure information for sai. */
void init (void)
{
    hardware_init_codec(0);
    /* Configure the play audio format */
    //format = (sai_data_format_t *)OSA_MemAllocZero(sizeof(sai_data_format_t));
    format->bits = 16;
    format->sample_rate = 16000;
    format->mclk = 384 * format->sample_rate;
    format->mono_stereo = kSaiStereo;

    /* SAI configuration */
    tx_config.protocol = kSaiBusI2SLeft;
    tx_config.channel = 0;
    tx_config.slave_master = kSaiMaster;
    tx_config.sync_mode = kSaiModeAsync;
    tx_config.bclk_source = kSaiBclkSourceMclkDiv;
    tx_config.mclk_source = kSaiMclkSourceSysclk;
    tx_config.watermark = 4;

    tx_card.controller.instance = 0;
    tx_card.controller.fifo_channel = 0;
    tx_card.controller.ops = &g_sai_ops;

#if USEDMA
    tx_card.controller.dma_source = kDmaRequestMux0I2S0Tx;
#endif
    //sgtl_handler_t *codec_handler = (sgtl_handler_t *)OSA_MemAllocZero(sizeof(sgtl_handler_t));
    sgtl_handler_t *codec_handler = &sgtl_codec_handler_t;
    codec_handler->i2c_instance = BOARD_SAI_DEMO_I2C_INSTANCE;
    tx_card.codec.handler = codec_handler;
    tx_card.codec.ops = &g_sgtl_ops;
}


/*****************************************************************************
 *  
 *   @name        APP_init
 * 
 *   @brief       This function is the entry for speaker (or other usage)
 * 
 *   @param       None
 * 
 *   @return      None
 **
 *****************************************************************************/
#if defined(FSL_RTOS_MQX)
extern void I2C_DRV_IRQHandler(uint32_t instance);
void I2C0_MQX_IRQHandler(void)
{
    I2C_DRV_IRQHandler(0);
}

void I2C1_MQX_IRQHandler(void)
{
    I2C_DRV_IRQHandler(1);
}

void I2S0_TX_MQX_IRQHandler(void)
{
    SAI_DRV_TxIRQHandler(0U);
}

#if (FSL_FEATURE_EDMA_MODULE_CHANNEL <= 16)
void DMA0_MQX_IRQHandler(void)
{
    EDMA_DRV_IRQHandler(0);
}
#else
void DMA0_DMA16_MQX_IRQHandler(void)
{
    EDMA_DRV_IRQHandler(0);
}
#endif
#else
extern void I2C_DRV_IRQHandler (uint32_t instance);
#if (FSL_FEATURE_EDMA_MODULE_CHANNEL <= 16)
void DMA0_IRQHandler (void)
{
    EDMA_DRV_IRQHandler(0);
}
#else
void DMA0_DMA16_IRQHandler(void)
{
    EDMA_DRV_IRQHandler(0);
}
#endif

void I2C0_IRQHandler (void)
{
    I2C_DRV_IRQHandler(0);
}

void I2C1_IRQHandler (void)
{
    I2C_DRV_IRQHandler(1);
}

extern sai_state_t * volatile sai_state_ids[I2S_INSTANCE_COUNT][2];

/*************************************************************************
 * Code
 ************************************************************************/
/* I2S IRQ handler with the same name in startup code */

#if (FSL_FEATURE_SAI_INT_SOURCE_NUM == 1)

void I2S0_IRQHandler(void)
{
    if (sai_state_ids[0][1] != NULL)
    {
        SAI_DRV_RxIRQHandler(0U);
    }
    if (sai_state_ids[0][0] != NULL)
    {
        SAI_DRV_TxIRQHandler(0U);
    }
}

#else
void I2S0_Tx_IRQHandler (void)
{
    SAI_DRV_TxIRQHandler(0U);
}

void I2S0_Rx_IRQHandler (void)
{
    SAI_DRV_RxIRQHandler(0U);
}

#if defined (K70F12_SERIES)
void I2S1_Tx_IRQHandler(void)
{
    SAI_DRV_TxIRQHandler(1U);
}

void I2S1_Rx_IRQHandler(void)
{
    SAI_DRV_RxIRQHandler(1U);
}
#endif /*defined K70F12_SERIES */
#endif /* FSL_FEATURE_SAI_INT_SPURCE_NUM */
#endif
void APP_init (void)
{
    audio_config_struct_t audio_config;

    USB_PRINTF("audio_speaker_TestApp_Init\n");
    audio_config.audio_application_callback.callback = USB_App_Device_Callback;
    audio_config.audio_application_callback.arg = &g_app_handle;
    audio_config.class_specific_callback.callback = USB_App_Class_Callback;
    audio_config.class_specific_callback.arg = &g_app_handle;
    audio_config.board_init_callback.callback = usb_device_board_init;
    audio_config.board_init_callback.arg = CONTROLLER_ID;
    audio_config.desc_callback_ptr = &desc_callback;
    USB_PRINTF("Audio speaker TestApp\r\n");

    g_app_speed = USB_SPEED_FULL;
#if defined(FSL_RTOS_FREE_RTOS)
	#if (BOARD_SAI_DEMO_I2C_INSTANCE == 0)
       NVIC_SetPriority(I2C0_IRQn, 2);
	#elif (BOARD_SAI_DEMO_I2C_INSTANCE == 1)
	   NVIC_SetPriority(I2C1_IRQn, 2);
	#endif
    NVIC_SetPriority(I2S0_Tx_IRQn, 3);
#if USEDMA
    #if (FSL_FEATURE_EDMA_MODULE_CHANNEL <= 16)
        NVIC_SetPriority(DMA0_IRQn,4);
    #else
        NVIC_SetPriority(DMA0_DMA16_IRQn,4);
    #endif
#endif
#endif
#if defined(FSL_RTOS_MQX)
    OS_install_isr(I2C0_IRQn, I2C0_MQX_IRQHandler, NULL);
    OS_install_isr(I2C1_IRQn, I2C1_MQX_IRQHandler, NULL);
    OS_install_isr(I2S0_Tx_IRQn, I2S0_TX_MQX_IRQHandler, NULL);
#if USEDMA
    #if (FSL_FEATURE_EDMA_MODULE_CHANNEL <= 16)
        OS_install_isr(DMA0_IRQn, DMA0_MQX_IRQHandler, NULL);
    #else
        OS_install_isr(DMA0_DMA16_IRQn, DMA0_DMA16_MQX_IRQHandler, NULL);   
    #endif
#endif
#endif
    init();
#if USEDMA
    EDMA_DRV_Init(&edmaState, &edmaUserConfig);
#endif
    SND_TxInit(&tx_card, &tx_config, NULL, &tx_state);
    SND_TxConfigDataFormat(&tx_card, format);

    /* Initialize the USB interface */
    USB_Class_Audio_Init(CONTROLLER_ID, &audio_config, &g_app_handle);

}

/******************************************************************************
 * 
 *    @name        USB_App_Device_Callback
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
void USB_App_Device_Callback (uint8_t event_type, void* val, void* arg)
{
    if (event_type == USB_DEV_EVENT_BUS_RESET)
    {
        if (USB_OK == USB_Class_Audio_Get_Speed(g_app_handle, &g_app_speed))
        {
            USB_Desc_Set_Speed(g_app_handle, g_app_speed);
        }
    }
    else if (event_type == USB_DEV_EVENT_ENUM_COMPLETE)
    {

#if USEDMA
        edmaUserConfig.chnArbitration = kEDMAChnArbitrationRoundrobin;
        edmaUserConfig.notHaltOnError = false;
#endif

        USB_Class_Audio_Recv_Data(g_app_handle, AUDIO_ISOCHRONOUS_ENDPOINT,
        audio_data_buff, DATA_BUFF_SIZE);

    }
    else if (event_type == USB_DEV_EVENT_ERROR)
    {
        /* add user code for error handling */
    }
    return;
}

/******************************************************************************
 * 
 *    @name        USB_App_Class_Callback
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
uint8_t USB_App_Class_Callback (uint8_t request, uint16_t value, uint8_t ** data,
    uint32_t* size, void* arg)
{
    uint8_t error = USB_OK;
    audio_app_data_t* data_receive;
    uint16_t ep_packet_size;
    static uint32_t soundcard_counter = 0;
    snd_state_t tx_status;

    if ((request == USB_DEV_EVENT_DATA_RECEIVED)&& (value == USB_REQ_VAL_INVALID) && (arg != NULL) && (*size != 0xFFFFFFFF))
    {
        data_receive = (audio_app_data_t*) data;
        if(data_receive->data_size != 64)
        {
          first_copy = true;
          first_time = true;
        }
        ep_packet_size = (
        g_app_speed == USB_SPEED_HIGH ? FS_ISO_OUT_ENDP_PACKET_SIZE :HS_ISO_OUT_ENDP_PACKET_SIZE);

        if (first_copy)
        {

            first_copy = false;
            SND_GetStatus(&tx_card, &tx_status);
            memcpy(tx_status.input_address, audio_data_buff, tx_status.size);
            //SND_TxStart(&tx_card);
            SND_TxUpdateStatus(&tx_card, AUDIO_BUFFER_BLOCK_SIZE);

        }
        else
        {

            soundcard_counter++;
            if (soundcard_counter >= AUDIO_BUFFER_BLOCK_SIZE / ep_packet_size)
            {
                if (first_time)
                {
                   first_time = false;
                   SND_GetStatus(&tx_card, &tx_status);
                   memcpy(tx_status.input_address, audio_data_buff, tx_status.size);
                   SND_TxStart(&tx_card);
                   SND_TxUpdateStatus(&tx_card, AUDIO_BUFFER_BLOCK_SIZE);
                   soundcard_counter = 0;
                   
                }

                else
                {
                SND_WaitEvent(&tx_card);
                SND_GetStatus(&tx_card, &tx_status);
                if(tx_status.full_block == 0)
                {
                   first_copy = true;
                   first_time = true;
                }
                memcpy(tx_status.input_address, audio_data_buff,tx_status.size);
                SND_TxUpdateStatus(&tx_card, AUDIO_BUFFER_BLOCK_SIZE);
                soundcard_counter = 0;
                }
            }

        }

        /* request next data to the current buffer */
        USB_Class_Audio_Recv_Data(g_app_handle, AUDIO_ISOCHRONOUS_ENDPOINT,
        audio_data_buff + ep_packet_size * soundcard_counter,ep_packet_size);

        return error;
    }

    error = USB_Class_Get_feature(0x0, value, request, data);
    if (error == USBERR_INVALID_REQ_TYPE)
        error = USB_Class_Set_feature(0x0, value, request, data);

    return error;
}

void APP_task (void)
{
}

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    {   1L, Main_Task, 2500L, MQX_MAIN_TASK_PRIORITY, "Main", MQX_AUTO_START_TASK},
    {   0L, 0L, 0L, 0L, 0L, 0L}
};
#endif

static void Task_Start (void *arg)
{
    APP_init();
    OSA_TaskDestroy(OSA_TaskGetHandler());
}

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param)
#else

#if defined(__CC_ARM) || defined(__GNUC__)
int main (void)
#else
void main(void)
#endif

#endif
{
    hardware_init();
    OSA_Init();
    dbg_uart_init();

    OS_Task_create(Task_Start, NULL, 4L, 3000L, "task_start", NULL);

    OSA_Start();
#if (!defined(FSL_RTOS_MQX))&(defined(__CC_ARM) || defined(__GNUC__))
    return 1;
#endif
}

/* EOF */
