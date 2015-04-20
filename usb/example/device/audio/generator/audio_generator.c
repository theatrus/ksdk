/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013- 2014 Freescale Semiconductor;
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

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_port_hal.h"
#include <stdio.h>
#include <stdlib.h>

#endif

#if USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined zero in usb_device_config.h. Please recompile usbd with this option.
#endif

#if USBCFG_AUDIO_CLASS_2_0
#error This application requires USBCFG_AUDIO_CLASS_2_0 defined zero in usb_device_config.h. Please recompile usbd with this option.
#endif

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 #define MAIN_TASK       10
 

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void APP_init(void);
void USB_Prepare_Data(void);
extern void Main_Task(uint32_t param);

/****************************************************************************
 * Global Variables
 ****************************************************************************/              
extern usb_endpoints_t usb_desc_ep;

extern usb_desc_request_notify_struct_t  desc_callback;

extern const unsigned char wav_data[];
extern const uint16_t wav_size;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
static uint8_t *wav_buff;
#else
static uint8_t wav_buff[AUDIO_ENDPOINT_PACKET_SIZE];
#endif
uint32_t audio_position = 0;

audio_handle_t   g_app_handle;

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
extern void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, Main_Task, 2*3000L, 7L, "Main", MQX_AUTO_START_TASK, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L, 0, 0}
};
#endif
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
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
void USB_Prepare_Data(void)
{
   uint8_t k;
   /* copy data to buffer */
   for(k = 0; k < AUDIO_ENDPOINT_PACKET_SIZE; k++, audio_position++)
   {
      if (audio_position > wav_size) {
          audio_position = 0;
      }
      wav_buff[k] = wav_data[audio_position];
   }
}
uint8_t endpoint_memory[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
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
    uint8_t index;

    if((request == USB_DEV_EVENT_SEND_COMPLETE) && (value == USB_REQ_VAL_INVALID))
     {
          if(arg != NULL)
          {
                USB_Prepare_Data();
                USB_Class_Audio_Send_Data(g_app_handle, AUDIO_ENDPOINT, wav_buff, AUDIO_ENDPOINT_PACKET_SIZE);              
          }
          return error;
    }
    else if (request == GET_MEM)
        {
        *data = &endpoint_memory[value];
        }
    else if (request == SET_MEM)
        {
             for(index = 0; index < *size ; index++) 
         {   /* copy the report sent by the host */           
             endpoint_memory[value + index] = *(*data + index);
         }
        *size = 0;  
        }
    error = USB_Class_Get_feature(0x0,value,request, data);
    if(error == USBERR_INVALID_REQ_TYPE)
        error = USB_Class_Set_feature(0x0, value,request,data);
    
    return error; 
}   

 /*****************************************************************************
 *  
 *   @name        TestApp_Init
 * 
 *   @brief       This function is the entry for Audio generator
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/
void APP_init(void)
{
   audio_config_struct_t audio_config;
 //  printf("audio_TestApp_Init\n");
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    wav_buff = OS_Mem_alloc_uncached_align(AUDIO_ENDPOINT_PACKET_SIZE, 32);
    if(wav_buff == NULL)
    {
        USB_PRINTF("OS_Mem_alloc_uncached_align fail in audio generator example \r\n");
        return ;
    }
#endif
   audio_config.audio_application_callback.callback = USB_App_Device_Callback;
   audio_config.audio_application_callback.arg = &g_app_handle;
   audio_config.class_specific_callback.callback = USB_App_Class_Callback;
   audio_config.class_specific_callback.arg = &g_app_handle;
   audio_config.desc_callback_ptr = &desc_callback;

   /* Initialize the USB interface */
   USB_Class_Audio_Init(CONTROLLER_ID, &audio_config, &g_app_handle);


 } 

void APP_task(void)
{
    while(!start_app)
    {
     //OS_Time_delay(1);
    }
    start_app = FALSE;
    USB_PRINTF("USB_Prepare_Data \r\n");   
    USB_Prepare_Data();
    USB_Class_Audio_Send_Data(g_app_handle, AUDIO_ENDPOINT, wav_buff, AUDIO_ENDPOINT_PACKET_SIZE);
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
void USB_App_Device_Callback(uint8_t event_type, void* val,void* arg) 
{
   UNUSED_ARGUMENT (arg)
   UNUSED_ARGUMENT (val)

   if ((event_type == USB_DEV_EVENT_BUS_RESET) || (event_type == USB_DEV_EVENT_CONFIG_CHANGED))
   {
      start_app=FALSE;
   }
   else if(event_type == USB_DEV_EVENT_ENUM_COMPLETE) 
   {
       start_app=TRUE; 
       USB_PRINTF("Audio generator is working ... \r\n");       
   }
   else if(event_type == USB_DEV_EVENT_ERROR)
   {
      /* add user code for error handling */
   }
  
}
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)

/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : Main_Task
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
    for ( ; ; )
    {
        APP_task();

    } 
}
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) 

static void Task_Start(void* param)
{
   
#if USE_RTOS 
  APP_init();
  while(1)
  {
    while(!start_app)
    {
     //OS_Time_delay(1);
    }
    start_app = FALSE;
    USB_PRINTF("USB_Prepare_Data \r\n");   
    USB_Prepare_Data();
    USB_Class_Audio_Send_Data(g_app_handle, AUDIO_ENDPOINT, wav_buff, AUDIO_ENDPOINT_PACKET_SIZE);
    
        //OSA_TaskDestroy(OSA_TaskGetHandler());
  }
#else
       APP_init();
    while(!start_app)
    {
     //OS_Time_delay(1);
    }
    start_app = FALSE;
    USB_PRINTF("USB_Prepare_Data \r\n");   
    USB_Prepare_Data();
    USB_Class_Audio_Send_Data(g_app_handle, AUDIO_ENDPOINT, wav_buff, AUDIO_ENDPOINT_PACKET_SIZE);
#endif
}

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { 1L,     Main_Task,      2500L,  MQX_MAIN_TASK_PRIORITY, "Main",      MQX_AUTO_START_TASK},
   { 0L,     0L,             0L,    0L, 0L,          0L }
};
#endif

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
