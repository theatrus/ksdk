/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: virtual_com.c$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief  The file emulates a USB PORT as RS232 PORT.
*****************************************************************************/ 

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "virtual_com.h"

#if USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined zero in usb_device_config.h. Please recompile usbd with this option.
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_port_hal.h"

#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#endif

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
extern void Main_Task(uint32_t param);
#define MAIN_TASK       10

TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
   { MAIN_TASK, Main_Task, 2*3000L, 7L, "Main", MQX_AUTO_START_TASK, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L, 0, 0}
};
#endif
/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/
 
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
void TestApp_Init(void);

/****************************************************************************
 * Global Variables
 ****************************************************************************/
extern usb_desc_request_notify_struct_t  desc_callback;

cdc_handle_t   g_app_handle;

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/
 
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void USB_App_Device_Callback(uint8_t event_type, void* val,void* arg);
uint8_t USB_App_Class_Callback(uint8_t event, uint16_t value, uint8_t ** data, uint32_t* size, void* arg); 
void Virtual_Com_App(void);
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
uint8_t g_line_coding[LINE_CODING_SIZE] = 
{
 /*e.g. 0x00,0x10,0x0E,0x00 : 0x000E1000 is 921600 bits per second */
	(LINE_CODE_DTERATE_IFACE>> 0) & 0x000000FF,
	(LINE_CODE_DTERATE_IFACE>> 8) & 0x000000FF,
	(LINE_CODE_DTERATE_IFACE>>16) & 0x000000FF, 		 
	(LINE_CODE_DTERATE_IFACE>>24) & 0x000000FF,
	 LINE_CODE_CHARFORMAT_IFACE,
	 LINE_CODE_PARITYTYPE_IFACE,
	 LINE_CODE_DATABITS_IFACE
};

uint8_t g_abstract_state[COMM_FEATURE_DATA_SIZE] = 
{
	 (STATUS_ABSTRACT_STATE_IFACE>>0) & 0x00FF,
	 (STATUS_ABSTRACT_STATE_IFACE>>8) & 0x00FF																		
};

uint8_t g_country_code[COMM_FEATURE_DATA_SIZE] = 
{
 (COUNTRY_SETTING_IFACE>>0) & 0x00FF,
 (COUNTRY_SETTING_IFACE>>8) & 0x00FF															  
};
static bool start_app = FALSE;
static bool start_transactions = FALSE;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
static uint8_t * g_curr_recv_buf;
static uint8_t * g_curr_send_buf;
#else
static uint8_t g_curr_recv_buf[DATA_BUFF_SIZE];
static uint8_t g_curr_send_buf[DATA_BUFF_SIZE];
#endif
static uint8_t g_recv_size;
static uint8_t g_send_size;
/*****************************************************************************
 * Local Functions
 *****************************************************************************/
 
/**************************************************************************//*!
 *
 * @name  USB_Get_Line_Coding
 *
 * @brief The function returns the Line Coding/Configuration
 *
 * @param handle:        handle     
 * @param interface:     interface number     
 * @param coding_data:   output line coding data     
 *
 * @return USB_OK                              When Success
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Get_Line_Coding(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t * *coding_data)
{   
    UNUSED_ARGUMENT(handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get line coding data*/
        *coding_data = g_line_coding;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Set_Line_Coding
 *
 * @brief The function sets the Line Coding/Configuration
 *
 * @param handle: handle     
 * @param interface:     interface number     
 * @param coding_data:   output line coding data     
 *
 * @return USB_OK                              When Success
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Set_Line_Coding(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t * *coding_data)
{   
    uint8_t count;

    UNUSED_ARGUMENT(handle)
    
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set line coding data*/
        for (count = 0; count < LINE_CODING_SIZE; count++) 
        {          
            g_line_coding[count] = *((*coding_data+USB_SETUP_PKT_SIZE) + count);
        }
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Get_Abstract_State
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (ABSTRACT_STATE)
 * @param handle:        handle
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Success
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Get_Abstract_State(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t * *feature_data)
{   
    UNUSED_ARGUMENT(handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get line coding data*/
        *feature_data = g_abstract_state;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Get_Country_Setting
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (COUNTRY_CODE)
 * @param handle:        handle     
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Success
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Get_Country_Setting(uint32_t handle, 
                                    uint8_t interface, 
                                    uint8_t * *feature_data)
{   
    UNUSED_ARGUMENT(handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get line coding data*/
        *feature_data = g_country_code;
        return USB_OK;  
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Set_Abstract_State
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (ABSTRACT_STATE)
 * @param handle:        handle     
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Success
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Set_Abstract_State(uint32_t handle, 
                                uint8_t interface, 
                                uint8_t * *feature_data)
{   
    uint8_t count;
    UNUSED_ARGUMENT(handle)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set Abstract State Feature*/
        for (count = 0; count < COMM_FEATURE_DATA_SIZE; count++) 
        {          
            g_abstract_state[count] = *(*feature_data + count);
        }
        return USB_OK; 
    }
    
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Set_Country_Setting
 *
 * @brief The function gets the current setting for communication feature
 *                                                  (COUNTRY_CODE)
 * @param handle: handle     
 * @param interface:     interface number     
 * @param feature_data:   output comm feature data     
 *
 * @return USB_OK                              When Success
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************/
uint8_t USB_Set_Country_Setting(uint32_t handle, 
                                    uint8_t interface, 
                                    uint8_t * *feature_data)
{   
    uint8_t count;
    UNUSED_ARGUMENT (handle)
    
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        for (count = 0; count < COMM_FEATURE_DATA_SIZE; count++) 
        {          
            g_country_code[count] = *(*feature_data + count);
        }
        return USB_OK; 
    }
    
    return USBERR_INVALID_REQ_TYPE;
}
/*****************************************************************************
*  
*	@name		 APP_init
* 
*	@brief		 This function do initialization for APP.
* 
*	@param		 None
* 
*	@return 	 None
**				  
*****************************************************************************/
void APP_init()
{
    cdc_config_struct_t cdc_config;
    cdc_config.cdc_application_callback.callback = USB_App_Device_Callback;
    cdc_config.cdc_application_callback.arg = &g_app_handle;
    cdc_config.vendor_req_callback.callback = NULL;
    cdc_config.vendor_req_callback.arg = NULL;
    cdc_config.class_specific_callback.callback = USB_App_Class_Callback;
    cdc_config.class_specific_callback.arg = &g_app_handle;
    cdc_config.desc_callback_ptr =  &desc_callback;
    /* Always happen in control endpoint hence hard coded in Class layer*/
    
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
	g_curr_recv_buf = OS_Mem_alloc_uncached_align(DATA_BUFF_SIZE, 32);
	g_curr_send_buf = OS_Mem_alloc_uncached_align(DATA_BUFF_SIZE, 32);
#endif
    /* Initialize the USB interface */
    USB_Class_CDC_Init(CONTROLLER_ID, &cdc_config, &g_app_handle);
    g_recv_size = 0;
    g_send_size= 0;    
}

/*****************************************************************************
*  
*	@name		 APP_task
* 
*	@brief		 This function runs APP task.
*	@param		 None
* 
*	@return 	 None
**				  
*****************************************************************************/
void APP_task()
{
    while (TRUE) 
    {
        /* call the periodic task function */      
        USB_CDC_Periodic_Task();           

       /*check whether enumeration is complete or not */
        if((start_app==TRUE) && (start_transactions==TRUE))
        {        
            Virtual_Com_App(); 
        }            
    }/* Endwhile */   
}

/******************************************************************************
 * 
 *    @name       Virtual_Com_App
 *    
 *    @brief      
 *                  
 *    @param      None
 * 
 *    @return     None
 *    
 *****************************************************************************/
void Virtual_Com_App(void)
{
    /* User Code */ 
    if(g_recv_size) 
    {
        int32_t i;
        
        /* Copy Buffer to Send Buff */
        for (i = 0; i < g_recv_size; i++)
        {
            //USB_PRINTF("Copied: %c\n", g_curr_recv_buf[i]);
        	g_curr_send_buf[g_send_size++] = g_curr_recv_buf[i];
        }
        g_recv_size = 0;
    }
    
    if(g_send_size) 
    {
        uint8_t error;
        uint8_t size = g_send_size;
        g_send_size = 0;

        error = USB_Class_CDC_Send_Data(g_app_handle, DIC_BULK_IN_ENDPOINT,
        	g_curr_send_buf, size);

        if(error != USB_OK) 
        {
            /* Failure to send Data Handling code here */
        } 
    }
    return;
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
    uint32_t handle;
    handle = *((uint32_t *)arg);
    if(event_type == USB_DEV_EVENT_BUS_RESET)
    {
        start_app=FALSE;    
    }
    else if(event_type == USB_DEV_EVENT_CONFIG_CHANGED)
    {
        /* Schedule buffer for receive */
        USB_Class_CDC_Recv_Data(handle, DIC_BULK_OUT_ENDPOINT, g_curr_recv_buf, DIC_BULK_OUT_ENDP_PACKET_SIZE);
        start_app=TRUE;
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
    uint8_t event, 
    uint16_t value, 
    uint8_t ** data, 
    uint32_t* size,
    void* arg
) 
{
    cdc_handle_t handle;
    uint8_t error = USB_OK;
    handle = *((cdc_handle_t *)arg);
    switch(event)
    {
		case GET_LINE_CODING:
			error = USB_Get_Line_Coding(handle, value, data);
		break;
		case GET_ABSTRACT_STATE:
			error = USB_Get_Abstract_State(handle, value, data);
		break;
		case GET_COUNTRY_SETTING:
			error = USB_Get_Country_Setting(handle, value, data);
		break;
		case SET_LINE_CODING:
			error = USB_Set_Line_Coding(handle, value, data);
		break;
		case SET_ABSTRACT_STATE:
			error = USB_Set_Abstract_State(handle, value, data);
		break;
		case SET_COUNTRY_SETTING:
			error = USB_Set_Country_Setting(handle, value, data);
		break;
		case USB_APP_CDC_DTE_ACTIVATED:
			if(start_app == TRUE)
			{
				start_transactions = TRUE; 
			}
		break;
		case USB_APP_CDC_DTE_DEACTIVATED:
			if(start_app == TRUE)
			{
				start_transactions = FALSE; 
			}
		break;
		case USB_DEV_EVENT_DATA_RECEIVED:
		{
            if((start_app == TRUE) && (start_transactions == TRUE))
            {
				g_recv_size = *size;
				if(!g_recv_size)
				{
					 /* Schedule buffer for next receive event */
					 USB_Class_CDC_Recv_Data(handle, DIC_BULK_OUT_ENDPOINT, g_curr_recv_buf, DIC_BULK_OUT_ENDP_PACKET_SIZE); 
				}
				
            }
        }
		break;
		case USB_DEV_EVENT_SEND_COMPLETE:
		{
	        if ((size != NULL) && (*size != 0) && !(*size % DIC_BULK_IN_ENDP_PACKET_SIZE))
	        {
				/* If the last packet is the size of endpoint, then send also zero-ended packet,
				** meaning that we want to inform the host that we do not have any additional
				** data, so it can flush the output.
		             */
	            USB_Class_CDC_Send_Data(g_app_handle, DIC_BULK_IN_ENDPOINT, NULL, 0);
	        } else if((start_app == TRUE) && (start_transactions == TRUE))
            {
				 if((*data != NULL) || ((*data == NULL) && (*size == 0)))
				 {
					 /* User: add your own code for send complete event */ 
					 /* Schedule buffer for next receive event */
					 USB_Class_CDC_Recv_Data(handle, DIC_BULK_OUT_ENDPOINT, g_curr_recv_buf, DIC_BULK_OUT_ENDP_PACKET_SIZE); 
				 }
            }
		}
		break;
        default:
        error = USBERR_INVALID_REQ_TYPE;
    }

    return error;
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
	APP_task();
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
#if (USE_RTOS)
    while (TRUE) 
    {
   	#endif
        /* call the periodic task function */      
        USB_CDC_Periodic_Task();           
    
       /*check whether enumeration is complete or not */
        if((start_app==TRUE) && (start_transactions==TRUE))
{
            Virtual_Com_App(); 
    }
#if (USE_RTOS)
    }/* Endwhile */   
#endif
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

    APP_init();

    OS_Task_create(Task_Start, NULL, 9L, 1000L, "task_start", NULL);

    OSA_Start();
#if (!defined(FSL_RTOS_MQX))&(defined(__CC_ARM) || defined(__GNUC__))
    return 1;
#endif
}
#endif
    
    

/* EOF */
