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
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) 
//#include "io_gpio.h"
#include "sgtl5000.h"
#ifdef BSPCFG_ENABLE_SAI
#include "sai_audio.h"
#include "sai.h"
#endif
#endif
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
#include "sgtl5000.h"
#include "soc_i2s.h"
#include "user_config.h"

#if !I2C_ENABLE
#error This application requires I2C_ENABLE defined one in user_config.h.
#endif

#if !I2S_ENABLE
#error This application requires I2S_ENABLE defined one in user_config.h.
#endif
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)

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

#endif


#if USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined zero in usb_device_config.h. Please recompile usbd with this option.
#endif

void APP_task(void);

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
extern void Main_Task(uint32_t param);
extern void Play_Task(uint32_t param);

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    { 10L, Main_Task, 2000L, 7L, "Main Task", MQX_AUTO_START_TASK, 0, 0},
    { 11L, Play_Task,  2000L, 3L, "Play Task", MQX_AUTO_START_TASK, 0, 0},
    { 0L, 0L, 0L, 0L, 0L, 0L, 0, 0}
};
#endif
/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void APP_init(void);

/****************************************************************************
 * Global Variables
 ****************************************************************************/              
extern usb_endpoints_t                      usb_desc_ep;
extern usb_desc_request_notify_struct_t     desc_callback;
audio_handle_t                              g_app_handle;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
os_event_handle                         app_event;
static uint8_t* audio_data_buff0;
static uint8_t* audio_data_buff1;
static volatile uint8_t codecisready = 0;
static uint32_t datasize;
static uint8_t * audio_current_buff;
#endif
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
ksai_info_struct_t                      ksai_info;
ksai_info_struct_t*                     ksai_info_ptr = &ksai_info;

uint8_t                                 audio_data_buff[DATA_BUFF_SIZE*4];
volatile uint32_t                       data_queued = 0;
extern volatile uint8_t                 sai_event;
ksai_info_struct_t    ksai_init = 
{
    /* Selected SAI HW channel */
    0,
    /* The SAI TX channel to initialize */
    0,
    /* The SAI RX channel to initialize */
    0,
    /* Clock setup: sync-async; bitclock: normal-swapped */
    I2S_TX_ASYNCHRONOUS | I2S_TX_BCLK_NORMAL | I2S_RX_SYNCHRONOUS | I2S_RX_BCLK_NORMAL,
    
    /* Default operating mode */
    I2S_TX_MASTER | I2S_RX_MASTER,
    
    /* Number of valid data bits*/
    16,
    
    /* I2S master clock source*/
    I2S_CLK_INT,
}; 
#endif 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Device_Callback(uint8_t event_type, void* val,void* arg);
void USB_Notif_Callback(uint8_t event_type,void* val,void* arg);
uint8_t USB_App_Class_Callback(uint8_t request, uint16_t value, uint8_t ** data, 
    uint32_t* size,void* arg); 

/*****************************************************************************
 * Local Variables 
 *****************************************************************************/

/*****************************************************************************
 * Local Functions
 *****************************************************************************/

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)

 sai_data_format_t sai_format = {0};
 sgtl_handler_t sgtl_codec_handler_t = {0};
 static sound_card_t tx_card;
 /*static sound_card_t rx_card; */
 static sai_data_format_t *format= &sai_format;
 static sai_user_config_t tx_config;
 /* static sai_user_config_t rx_config; */
 static sai_state_t tx_state;
 /* static sai_state_t rx_state; */
 uint8_t audio_data_buff[DATA_BUFF_SIZE*4];
#if USEDMA
static edma_state_t edmaState;
static edma_user_config_t edmaUserConfig;
#endif


 /* Open the clock and configure the pin mux for sai, i2c. */
 void hardware_init_codec(uint8_t instance)
 {
     uint8_t i;
     /* Open GPIO clock */
     for (i = 0; i < HW_PORT_INSTANCE_COUNT; i++)
     {
         CLOCK_SYS_EnablePortClock(i);
     }
     CLOCK_SYS_EnableSaiClock(0);
         configure_i2s_pins(instance);
         configure_i2c_pins(0);
 }
 
 /* Initialize the structure information for sai. */
 void init(void)
 {
    hardware_init_codec(0);
    /* Configure the play audio format */
	 //format = (sai_data_format_t *)OSA_MemAllocZero(sizeof(sai_data_format_t));
    format->bits = 16;
    format->sample_rate = 16000;
    format->mclk = 384 * format->sample_rate;
    format->mono_streo = kSaiStreo;
 
    /* SAI configuration */
    tx_config.protocol = kSaiBusI2SLeft;
    tx_config.channel = 0;
    tx_config.slave_master = kSaiMaster;
    tx_config.sync_mode = kSaiModeAsync;
    tx_config.bclk_source = kSaiBclkSourceMclkDiv;
    tx_config.mclk_source = kSaiMclkSourceSysclk;
    tx_config.mclk_divide_enable = true;
    tx_config.watermark = 4;

    tx_card.controller.instance = 0;
    tx_card.controller.fifo_channel = 0;
    tx_card.controller.ops = &g_sai_ops;

#if USEDMA
    tx_card.controller.dma_source = kDmaRequestMux0I2S0Tx;
#endif
    //sgtl_handler_t *codec_handler = (sgtl_handler_t *)OSA_MemAllocZero(sizeof(sgtl_handler_t));
    sgtl_handler_t *codec_handler = &sgtl_codec_handler_t;
    tx_card.codec.handler = codec_handler;
    tx_card.codec.ops = &g_sgtl_ops;
 }

 #endif

 /*****************************************************************************
 *  
 *   @name        APP_init
 * 
 *   @brief       This function is the entry for speaker (or other usuage)
 * 
 *   @param       None
 * 
 *   @return      None
 **
 *****************************************************************************/
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#if defined(FSL_RTOS_MQX)
extern void I2C_DRV_IRQHandler(uint32_t instance);
 void I2C0_MQX_IRQHandler(void)
 {
     I2C_DRV_IRQHandler(0);
 }
 
 void I2S0_TX_MQX_IRQHandler(void)
 {
     SAI_DRV_TxIRQHandler(0U);
 }
 
 void DMA0_MQX_IRQHandler(void)
 {
     EDMA_DRV_IRQHandler(0);
 }
#else
extern void I2C_DRV_IRQHandler(uint32_t instance);
void DMA0_IRQHandler(void)
{
    EDMA_DRV_IRQHandler(0);
}

void I2C0_IRQHandler(void)
{
    I2C_DRV_IRQHandler(0);
}

extern sai_state_t * volatile sai_state_ids[HW_I2S_INSTANCE_COUNT][2];

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
void I2S0_Tx_IRQHandler(void)
{
    SAI_DRV_TxIRQHandler(0U);
}

void I2S0_Rx_IRQHandler(void)
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
#endif
void APP_init(void)
{
    audio_config_struct_t       audio_config;

    USB_PRINTF("audio_speaker_TestApp_Init\n");
    audio_config.audio_application_callback.callback = USB_App_Device_Callback;
    audio_config.audio_application_callback.arg = &g_app_handle;
    audio_config.class_specific_callback.callback = USB_App_Class_Callback;
    audio_config.class_specific_callback.arg = &g_app_handle;
    audio_config.desc_callback_ptr = &desc_callback;
    USB_PRINTF("Audio speaker TestApp\r\n");
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#if defined(FSL_RTOS_FREE_RTOS)
    NVIC_SetPriority(I2C0_IRQn, 2);
    NVIC_SetPriority(I2S0_Tx_IRQn, 3);
#if USEDMA
    NVIC_SetPriority(DMA0_IRQn,4);
#endif
#endif
#if defined(FSL_RTOS_MQX)
    OS_install_isr(I2C0_IRQn, I2C0_MQX_IRQHandler, NULL);
    OS_install_isr(I2S0_Tx_IRQn, I2S0_TX_MQX_IRQHandler, NULL);
#if USEDMA
    OS_install_isr(DMA0_IRQn, DMA0_MQX_IRQHandler, NULL);
#endif
#endif
    init();
#if USEDMA
    EDMA_DRV_Init(&edmaState, &edmaUserConfig);
#endif
    SND_TxInit(&tx_card, &tx_config, NULL, &tx_state);
    SND_TxConfigDataFormat(&tx_card,format);

#endif
    /* Initialize the USB interface */
    USB_Class_Audio_Init(CONTROLLER_ID, &audio_config, &g_app_handle);



#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    app_event = OS_Event_create(0);//LWEVENT_AUTO_CLEAR);
    if (app_event == NULL)
    {
        USB_PRINTF("\n_lwevent_create app_event failed.\n");
        _task_block();
    }
    while (1)
    {
        if (OS_EVENT_OK != OS_Event_wait(app_event, USB_APP_ENUM_COMPLETE_EVENT_MASK, FALSE, 0)) {
            USB_PRINTF("\nOS_Event_wait app_event failed.\n");
            _task_block();
        }
        if (OS_EVENT_OK!= OS_Event_clear(app_event, USB_APP_ENUM_COMPLETE_EVENT_MASK)) 
        {
            USB_PRINTF("\nOS_Event_clear app_event failed.\n");
            _task_block();
        }
       /* Prepare buffer for first isochronous input */
        while (!codecisready)
            OS_Time_delay(1);
        USB_PRINTF("Audio speaker is working ... \r\n");
        /* Prepare buffer for first isochronous input */
        USB_Class_Audio_Recv_Data(g_app_handle,AUDIO_ISOCHRONOUS_ENDPOINT,
                      audio_data_buff0, DATA_BUFF_SIZE);
    }
#endif
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
void USB_App_Device_Callback(uint8_t event_type, void* val, void* arg) 
{
    if(event_type == USB_DEV_EVENT_BUS_RESET) 
    {

    }
    else if(event_type == USB_DEV_EVENT_ENUM_COMPLETE) 
    {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)     
        if (OS_EVENT_OK !=OS_Event_set(app_event, USB_APP_ENUM_COMPLETE_EVENT_MASK))
        {
            USB_PRINTF("OS_Event_set app_event failed.\n");
        }
#endif
        
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#if USEDMA
              edmaUserConfig.chnArbitration = kEDMAChnArbitrationRoundrobin;
              edmaUserConfig.notHaltOnError = false;
#endif

          

     USB_Class_Audio_Recv_Data(g_app_handle,AUDIO_ISOCHRONOUS_ENDPOINT,
                                        audio_data_buff, DATA_BUFF_SIZE);
#endif
        
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        uint32_t    flags = I2S_IO_WRITE;
        uint32_t    fs_freq = 16000;
        uint16_t    clk_mult = 256;
        uint8_t     channels = 2;

        InitCodec();
        sai_init(&ksai_init, flags, fs_freq, clk_mult, channels);
        SetupCodec(fs_freq);
        data_queued   = 0;
        sai_event     = 0;
        USB_Class_Audio_Recv_Data(g_app_handle,AUDIO_ISOCHRONOUS_ENDPOINT,
                                        audio_data_buff, DATA_BUFF_SIZE);
        sai_tx_enable(ksai_info_ptr->tx_channel);
        USB_PRINTF("Audio speaker is working ... \r\n");
#endif
    }
    else if(event_type == USB_DEV_EVENT_ERROR)
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
uint8_t USB_App_Class_Callback
(
    uint8_t request, 
    uint16_t value, 
    uint8_t ** data, 
    uint32_t* size,
    void* arg
) 
{
    uint8_t error = USB_OK;
    audio_app_data_t* data_receive;
    static uint32_t read_data = 0;
    static uint32_t write_data = 0;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
    static uint32_t soundcard_counter = 0;
    snd_state_t tx_status;
#endif

    if((request == USB_DEV_EVENT_DATA_RECEIVED) && (value == USB_REQ_VAL_INVALID) && (arg != NULL))
    {
        data_receive = (audio_app_data_t*)data;
        read_data += data_receive->data_size;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
        if (read_data >= datasize)
        {
            read_data = 0;
            if (OS_EVENT_OK != OS_Event_set(app_event, audio_current_buff == audio_data_buff0 ? USB_APP_BUFFER0_FULL_EVENT_MASK : USB_APP_BUFFER1_FULL_EVENT_MASK))
            {
                //an error occured while setting the lwevent
            }
            /* switch to the other buffer */
            if (audio_current_buff == audio_data_buff0)
            {
                audio_current_buff = audio_data_buff1;
            }
            else
            {
                audio_current_buff = audio_data_buff0;
            }
        }
        /* request next data to the current buffer */
        USB_Class_Audio_Recv_Data(g_app_handle, AUDIO_ISOCHRONOUS_ENDPOINT,
            audio_current_buff + read_data, DATA_BUFF_SIZE - read_data);
       return error;
#endif
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        data_queued += data_receive->data_size;
        if (data_queued >= DATA_BUFF_SIZE*4) 
        {
            data_queued -= data_receive->data_size;
            read_data -= data_receive->data_size;
        }
        if ((data_queued >= 3*DATA_BUFF_SIZE) && (sai_event==0))
        {
            sai_tx_int_enable(ksai_info_ptr->tx_channel);
        }
        if (read_data >= DATA_BUFF_SIZE*4) 
        {
            read_data = 0;
        }
        
        /* request next data to the current buffer */
        USB_Class_Audio_Recv_Data(g_app_handle, AUDIO_ISOCHRONOUS_ENDPOINT,
            audio_data_buff + read_data, AUDIO_ENDPOINT_PACKET_SIZE);
        return error;
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
        if (read_data >= DATA_BUFF_SIZE*4) 
        {
            read_data = 0;
        }

        if (write_data >= DATA_BUFF_SIZE*4) 
        {
            write_data = 0;
        }
        if(first_copy)
        {
            
            first_copy = false;
            SND_GetStatus(&tx_card, &tx_status);
            memcpy(tx_status.input_address, audio_data_buff, tx_status.size);
            SND_TxStart(&tx_card);    
            SND_TxUpdateStatus(&tx_card, AUDIO_BUFFER_BLOCK_SIZE);     

        }
        else
        {

            soundcard_counter++;
            if(soundcard_counter >= AUDIO_BUFFER_BLOCK_SIZE/AUDIO_ENDPOINT_PACKET_SIZE)
            {
             
             SND_WaitEvent(&tx_card);
             SND_GetStatus(&tx_card, &tx_status);
             memcpy(tx_status.input_address, audio_data_buff, tx_status.size);
             SND_TxUpdateStatus(&tx_card,AUDIO_BUFFER_BLOCK_SIZE);
             soundcard_counter = 0;
                }

        }
        
        /* request next data to the current buffer */
        USB_Class_Audio_Recv_Data(g_app_handle, AUDIO_ISOCHRONOUS_ENDPOINT,
            audio_data_buff + AUDIO_ENDPOINT_PACKET_SIZE * soundcard_counter, AUDIO_ENDPOINT_PACKET_SIZE);
                    
        
         return error;
#endif
    }

    error = USB_Class_Get_feature(0x0,value, request, data);
    if(error == USBERR_INVALID_REQ_TYPE)
        error = USB_Class_Set_feature(0x0,value,request,data);
    
    return error; 
}

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
/*Task*----------------------------------------------------------------
* 
* Task Name  : Main_Task
* Returned Value : None
* Comments       :
*     First function called.  Calls the Test_App
*     callback functions.
* 
*END*--------------------------------------------------------------------*/
void Main_Task
(
   uint32_t param
)
{
    UNUSED_ARGUMENT (param)
    APP_init();
    _time_delay(200);
    _task_block();
}

/*Task*----------------------------------------------------------------
* 
* Task Name      : Play_Task
* Returned Value : None
* Comments       :
*     This task writes audio data buffer to I2S device.
* 
*END*--------------------------------------------------------------------*/
void Play_Task
(
   uint32_t param
)
{
   MQX_FILE_PTR              i2s_ptr = NULL;
   AUDIO_DATA_FORMAT         audio_format;
   _mqx_int                  errcode;
   _mqx_int                  freq;
#if BSPCFG_ENABLE_SAI
   I2S_STATISTICS_STRUCT     stats;
#endif

    errcode = InitCodec();
    i2s_ptr = fopen(AUDIO_DEVICE, "w");
    if ((errcode != 0) || (i2s_ptr == NULL))
    {
        USB_PRINTF("Initializing audio codec...[FAIL]\n");
        USB_PRINTF("  Error 0x%X\n", errcode);
        fclose(i2s_ptr);
        _task_block();
    }
    /* Audio format in little endian */
    audio_format.ENDIAN = AUDIO_LITTLE_ENDIAN;
    /* Alignment of input audio data format */
    audio_format.ALIGNMENT = AUDIO_ALIGNMENT_LEFT;
    /* Audio format bits */
    audio_format.BITS = AUDIO_FORMAT_BITS;
    /* Audio format size */
    audio_format.SIZE = AUDIO_FORMAT_SIZE;
    /* Audio format channel number */
    audio_format.CHANNELS = AUDIO_FORMAT_CHANNELS;
#if BSPCFG_ENABLE_SAI
   audio_format.SAMPLE_RATE = AUDIO_FORMAT_SAMPLE_RATE;
   freq = AUDIO_FORMAT_SAMPLE_RATE * AUDIO_I2S_FS_FREQ_DIV;
   ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &freq);
   ioctl(i2s_ptr, IO_IOCTL_AUDIO_SET_TX_DATA_FORMAT, &audio_format);
   /* Setting audio codec for SGTL5000 device */
   SetupCodec(i2s_ptr, &audio_format);
#else
    /* Setting audio format */
    ioctl(i2s_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT, &audio_format);

    /* Setting over sampling clock frequency in Hz */
    freq = AUDIO_FORMAT_SAMPLE_RATE * AUDIO_I2S_FS_FREQ_DIV;
    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &freq);

    /* Setting bit clock frequency in Hz */
    freq = AUDIO_FORMAT_SAMPLE_RATE;
    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &freq);
    ioctl(i2s_ptr, IO_IOCTL_I2S_TX_DUMMY_OFF, NULL);

   /* Setting audio codec for SGTL5000 device */
   SetupCodec(i2s_ptr);
   OS_Time_delay(1);

   datasize = DATA_BUFF_SIZE;
#endif

#if BSPCFG_ENABLE_SAI
   ioctl(i2s_ptr, IO_IOCTL_I2S_GET_TX_STATISTICS, &stats);
   datasize = stats.SIZE;
#endif
   codecisready = 1;

   if (NULL == (audio_data_buff0 = OS_Mem_alloc_uncached_align(datasize, 32)))
   {
        USB_PRINTF("\nMemory allocation for audio_data_buff0 failed.\n");
        _task_block();
   }
   if (NULL == (audio_data_buff1 = OS_Mem_alloc_uncached_align(datasize, 32)))
   {
       USB_PRINTF("\nMemory allocation for audio_data_buff1 failed.\n");
       _task_block();
   }
   audio_current_buff = audio_data_buff0;
   OS_Time_delay(1);
#if BSPCFG_ENABLE_SAI
   ioctl(i2s_ptr, IO_IOCTL_I2S_START_TX, NULL);
#endif

   while (TRUE)
   {
       if (OS_EVENT_OK != OS_Event_wait(app_event, USB_APP_BUFFER0_FULL_EVENT_MASK | USB_APP_BUFFER1_FULL_EVENT_MASK, FALSE, 0))
       {
           USB_PRINTF("\n_lwevent_wait_ticks app_event failed.\n");
           fclose(i2s_ptr);
           _task_block();
       }
#if BSPCFG_ENABLE_SAI
       ioctl(i2s_ptr, IO_IOCTL_I2S_WAIT_TX_EVENT, NULL);
#endif
       if (OS_Event_check_bit(app_event, USB_APP_BUFFER0_FULL_EVENT_MASK))
       {
#if BSPCFG_ENABLE_SAI
           ioctl(i2s_ptr, IO_IOCTL_I2S_GET_TX_STATISTICS, &stats);
           OS_Mem_copy(audio_data_buff0, stats.IN_BUFFER, datasize);
           ioctl(i2s_ptr, IO_IOCTL_I2S_UPDATE_TX_STATUS, &datasize);
#else
           write(i2s_ptr, audio_data_buff0, datasize);
#endif
       }
       else if (OS_Event_check_bit(app_event, USB_APP_BUFFER1_FULL_EVENT_MASK))
       {
#if BSPCFG_ENABLE_SAI
           ioctl(i2s_ptr, IO_IOCTL_I2S_GET_TX_STATISTICS, &stats);
           OS_Mem_copy(audio_data_buff1,stats.IN_BUFFER,datasize);
           ioctl(i2s_ptr, IO_IOCTL_I2S_UPDATE_TX_STATUS, &datasize);
#else
           write(i2s_ptr, audio_data_buff1, datasize);
#endif
       }

        if (OS_EVENT_OK != OS_Event_clear(app_event, USB_APP_BUFFER0_FULL_EVENT_MASK | USB_APP_BUFFER1_FULL_EVENT_MASK))
        {
            USB_PRINTF("\n_lwevent_clear app_event failed.\n");
            _task_block();
        }
    }/* End while */

}
#endif
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
void APP_task
(
   void
)
{
}
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { 1L,     Main_Task,      2500L,  MQX_MAIN_TASK_PRIORITY, "Main",      MQX_AUTO_START_TASK},
   { 0L,     0L,             0L,    0L, 0L,          0L }
};
#endif

static void Task_Start(void *arg)
{
    APP_init();
    OSA_TaskDestroy(OSA_TaskGetHandler());
}

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param)
#else

#if defined(__CC_ARM) || defined(__GNUC__)
int main(void)
#else
void main(void)
#endif

#endif
{
    OSA_Init();
    hardware_init();
    dbg_uart_init();

    OS_Task_create(Task_Start, NULL, 9L, 3000L, "task_start", NULL);
    
    OSA_Start();
#if (!defined(FSL_RTOS_MQX))&(defined(__CC_ARM) || defined(__GNUC__))
    return 1;
#endif
}
#endif

/* EOF */
