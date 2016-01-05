/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013- 2014 Freescale Semiconductor;
 * All Rights Reserved
 *
 * Copyright (c) 1989 - 2008 ARC International;
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
 * $FileName: weighscale.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 * @brief when the device is connected using continua manager it comes in 
 *        operating state and after waiting for some time we send weighscale 
 *        readings
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "weighscale.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_port_hal.h"
#include <stdio.h>
#include <stdlib.h>
#endif

/****************************************************************************
 * Global Variables
 ****************************************************************************/
/* Add all the variables needed for phd_com_model.c to this structure */
weighscale_variable_struct_t g_weighscale;
uint32_t iteration = 0;
/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
static void Weights_App_Device_Callback(uint32_t handle, uint8_t event_type);
static void Send_Weights(void);

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
/******************************************************************************
 * 
 *    @name        Weights_App_Device_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       handle : handle to Identify the controller
 *    @param       event_type : value of the event    
 * 
 *    @return      None
 *
 *****************************************************************************
 * This function is called from the lower layer whenever an event occurs.
 * This sets a variable according to the event_type
 *****************************************************************************/
static void Weights_App_Device_Callback
(
    uint32_t handle,
    uint8_t event_type
    )
{
    UNUSED_ARGUMENT (handle)
    /* when event_type is APP_PHD_INITIALISED(transport connected, enumeration
     complete), execution will go in the else part and event will become 
     APP_PHD_INITIALISED */
    if (g_weighscale.event == APP_PHD_INITIALISED)
    {/* if transport is connected, enumeration is complete */
        if (event_type == USB_PHD_CONNECTED_TO_HOST)
        { /* attributes sent (response to get_attributes) and the device is 
         ready to send measurements */
            g_weighscale.event = USB_PHD_CONNECTED_TO_HOST;
        }
    }
    else
    {
        g_weighscale.event = event_type;
    }
}

/******************************************************************************
 *  
 *   @name        Send_Weights
 * 
 *   @brief       This function is used to send the association request after 
 *                the device is connected
 * 
 *   @param       None
 * 
 *   @return      None
 *
 *****************************************************************************
 * This function sends the measurement data or the dissociation request 
 * depending on the key pressed
 *****************************************************************************/
static void Send_Weights(void)
{
    PHD_Send_Measurements_to_Manager(g_weighscale.app_handle, &g_weighscale.msr);
}

/******************************************************************************
 *  
 *   @name        APP_init
 * 
 *   @brief       This function is the entry for the app (or other usage)
 * 
 *   @param       None
 * 
 *   @return      None
 *
 *****************************************************************************
 * This function starts the PHDC (weighing scale) application                
 *****************************************************************************/
void APP_init(void)
{
    /* Initialize Global Variable Structure */
    OS_Mem_zero(&g_weighscale, sizeof(weighscale_variable_struct_t));
    g_weighscale.event = APP_PHD_UNINITIALISED;
    /* initialize measurements */
    g_weighscale.msr.msr_time.century = 0x20;
    g_weighscale.msr.msr_time.year = 9;
    g_weighscale.msr.msr_time.month = 4;
    g_weighscale.msr.msr_time.day = 9;
    g_weighscale.msr.msr_time.hour = 3;
    g_weighscale.msr.msr_time.minute = 5;
    g_weighscale.msr.msr_time.second = 0;
    g_weighscale.msr.msr_time.sec_fractions = 0;

#if (ENDIANNESS == LITTLE_ENDIAN)
    g_weighscale.msr.bmi[0] = 200;
    g_weighscale.msr.bmi[1] = 205;
    g_weighscale.msr.weight[0] = 95;
    g_weighscale.msr.weight[1] = 54;
#else
    g_weighscale.msr.bmi[1]= 200;
    g_weighscale.msr.bmi[0]= 205;
    g_weighscale.msr.weight[1]= 95;
    g_weighscale.msr.weight[0]= 54;
#endif

    /* Initialize the USB interface */
    (uint8_t) PHD_Transport_Init((uint32_t *) &g_weighscale.app_handle, Weights_App_Device_Callback);
}

/******************************************************************************
 *  
 *   @name        APP_task
 * 
 *   @brief       
 * 
 *   @param       None
 * 
 *   @return      None
 *
 *****************************************************************************
 * 
 *****************************************************************************/
void APP_task()
{
    static volatile uint32_t delay_count;
    switch(g_weighscale.event)
    {
    case APP_PHD_INITIALISED:
        /* enters here for the first time when enum is complete and 
         event is APP_PHD_INITIALISED till the response to 
         get_attribute is not sent (see func USB_App_Callback in the
         same file) */
        /* wait till the host is ready to receive the association
         request */
        if(iteration == 0)
        {
            iteration = ITERATION_COUNT;
            PHD_Connect_to_Manager(g_weighscale.app_handle);
        }
        else if (iteration > 0)
        {
            iteration--;
        }
        break;

    case USB_PHD_CONNECTED_TO_HOST:
        { /* enters here for the first time when the device 
     is ready to send measurements */
        delay_count = ITERATION_COUNT;/* this delay depends on SoC used*/
        while (delay_count--)
        {
        };
        g_weighscale.msr.msr_time.second++;
        g_weighscale.msr.msr_time.second %= 60;
        g_weighscale.msr.weight[0]++;
        g_weighscale.msr.weight[1]++;
        g_weighscale.msr.weight[0] %= 1000;
        g_weighscale.msr.weight[1] %= 1000;
        g_weighscale.msr.bmi[0]++;
        g_weighscale.msr.bmi[1]++;
        g_weighscale.msr.bmi[0] %= 500;
        g_weighscale.msr.bmi[1] %= 500;
    }
        Send_Weights();
        break;

    case USB_PHD_DISCONNECTED_FROM_HOST:
        iteration = 0;
        break;

    case USB_PHD_MEASUREMENT_SENT:
        /* enters here each time we receive a response to the 
         measurements sent */
        g_weighscale.event = USB_PHD_CONNECTED_TO_HOST;
        g_weighscale.num_of_msr_sent++;
        break;

    default:
        break;
    }
    USB_PHDC_Periodic_Task();
}

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    {   1L, Main_Task, 512L, MQX_MAIN_TASK_PRIORITY, "Main", MQX_AUTO_START_TASK},
    {   0L, 0L, 0L, 0L, 0L, 0L}
};
#endif

static void Task_Start(void *arg)
{
#if (USE_RTOS)
    APP_init();

    /*
     ** Infinite loop, waiting for events requiring action
     */
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

#if defined(__CC_ARM) || defined(__GNUC__)
int main(void)
#else
void main(void)
#endif

#endif
{
    hardware_init();
    OSA_Init();
    dbg_uart_init();

#if !(USE_RTOS)
    APP_init();
#endif

    OS_Task_create(Task_Start, NULL, 4L, 3000L, "task_start", NULL);
    OSA_Start();
#if (!defined(FSL_RTOS_MQX))&(defined(__CC_ARM) || defined(__GNUC__))
    return 1;
#endif
}
#endif
/* EOF */
