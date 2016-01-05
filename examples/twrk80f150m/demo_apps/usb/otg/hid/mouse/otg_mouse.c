/**HEADER********************************************************************
 * 
* Copyright (c) 2010, 2013 - 2015 Freescale Semiconductor;
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
 * $FileName: otg_mouse.c$
 * $Version :
 * $Date    :
 *
 * Comments:
 *
 *
 *END************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb.h"
#include "usb_otg_main.h"
#include "host_mouse_api.h"
#include "dev_mouse_api.h"
#include "otg_mouse.h"
#include "usb_host_config.h"
#include "usb_device_config.h"
#include "usb_host_stack_interface.h"
#include "usb_host_hid.h"
#include "host_mouse.h"

#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include "fsl_port_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
  #ifdef BOARD_USE_LPUART
#include "fsl_lpuart_driver.h"
  #else
#include "fsl_uart_driver.h"
  #endif
/*****************************************************************************
 * Constant and Macro's 
 *****************************************************************************/
#if  USBCFG_DEV_COMPOSITE
#error This application requires USBCFG_DEV_COMPOSITE defined zero in usb_device_config.h. Please recompile USB library with this option.
#endif

#define MAIN_TASK                       (10)
/*****************************************************************************
 * Local Types 
 *****************************************************************************/
typedef enum
{
    dev_b = 0,
    dev_a
} dev_type_t;
/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/
extern void OTG_App_Init(void);
extern void DEV_APP_init(void);
extern void HOST_APP_init(void);
extern void DEV_APP_uninit(void);
extern void HOST_APP_uninit(void);
void OTG_App_Periodic_Task(void);
extern void HOST_APP_task_stun(uint32_t param);
extern void DEV_APP_task_stun(uint32_t param);
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && (defined (FSL_RTOS_MQX)))
void Main_Task(uint32_t param);
#endif
/****************************************************************************
 * Global Variables
 ****************************************************************************/
extern device_struct_t g_mouse_hid_device;
extern hid_command_t* g_mouse_hid_com;
usb_otg_handle g_otg_handle;
uint32_t g_otg_state;
dev_type_t g_dev_type; /* dev_type = 0 (device B); dev_type = 1 (device A) */
bool g_sess_vld; /* TRUE if session is valid */
bool g_vbus_err; /* VBUS over current */
os_event_handle g_otg_app_event_handle;

  #ifdef BOARD_USE_LPUART
lpuart_state_t g_uart_state;
  #else
uart_state_t g_uart_state;
  #endif

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
static void App_OtgCallback(usb_otg_handle handle, os_event_handle event);
static void App_HandleUserInput(void);
static void App_PrintMenu(void);
static void App_Print_Event(usb_otg_handle handle, os_event_handle event);
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
static const otg_int_struct_t g_otg_init =
{
    App_OtgCallback,
    HOST_APP_load,
    DEV_APP_load,
    HOST_APP_unload,
    DEV_APP_unload,
};


/* Declare max3353 low level trigger interrupt pin for the usb otg demo */
gpio_input_pin_user_config_t max3353intPin[] =
{
    {
        .pinName = BOARD_MAX3353_GPIO_INT,
        .config.isPullEnable = true,
        .config.pullSelect = kPortPullUp,
        .config.isPassiveFilterEnabled = false,
        .config.interrupt = kPortIntLogicZero /* kPortIntDisabled, kPortIntFallingEdge */
    },
    {
        .pinName = GPIO_PINS_OUT_OF_RANGE,
    }
};

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
/************************************************************************************
 **
 ** Globals
 ************************************************************************************/

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : OTG_App_Load_Unload_Task 
 * Returned Value : none
 * Comments       :
 *    
 *
 *END*--------------------------------------------------------------------*/
void OTG_App_Load_Unload_Task(os_event_handle event)
{
    if (OS_Event_check_bit(event, OTG_UNLOAD_DEVICE))
    {
        OS_Lock();
        DEV_APP_uninit();
        OS_Event_clear(event, OTG_UNLOAD_DEVICE);
        OS_Unlock();
    }
    if (OS_Event_check_bit(event, OTG_UNLOAD_HOST))
    {
        OS_Lock();
        HOST_APP_uninit();
        OS_Event_clear(event, OTG_UNLOAD_HOST);
        OS_Unlock();
    }
    if (OS_Event_check_bit(event, OTG_LOAD_DEVICE))
    {
        OS_Lock();
        DEV_APP_init();
        OS_Event_clear(event, OTG_LOAD_DEVICE);
        OS_Unlock();
    }
    if (OS_Event_check_bit(event, OTG_LOAD_HOST))
    {
        OS_Lock();
        HOST_APP_init();
        OS_Event_clear(g_otg_app_event_handle, OTG_LOAD_HOST);
        OS_Unlock();
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : OTG_App_Periodic_Task 
 * Returned Value : none
 * Comments       :
 *    
 *
 *END*--------------------------------------------------------------------*/
void APP_task()
{
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)&& USE_RTOS)
    OS_Event_wait(g_otg_app_event_handle, OTG_LOAD_UNLOAD_EVENT_MARK, FALSE, 10);
#endif  

    if (OS_Event_check_bit(g_otg_app_event_handle, OTG_LOAD_UNLOAD_EVENT_MARK))
    {
        OTG_App_Load_Unload_Task(g_otg_app_event_handle);
    }
    App_HandleUserInput();
}

void uart_rx_tx_hander()
{
#ifdef __cplusplus
    extern "C"
    {
#endif
  #ifdef BOARD_USE_LPUART
    extern void LPUART_DRV_IRQHandler(uint32_t instance);
  #else
        extern void UART_DRV_IRQHandler(uint32_t instance);
#endif
#ifdef __cplusplus
    }
#endif
  #ifdef BOARD_USE_LPUART
    LPUART_DRV_IRQHandler (BOARD_DEBUG_UART_INSTANCE);
  #else
    UART_DRV_IRQHandler (BOARD_DEBUG_UART_INSTANCE);
  #endif
}


/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : max3353_set_pin_int
 * Returned Value   : none
 * Comments         :
 *    This function enables/disables the pin interrupt associated with the max3353 interrupt pin 
 *
 *END*----------------------------------------------------------------------*/
void max3353_set_pin_int
(
    bool level,
    bool enable
)
{
    if (enable)
    {
        if (level)/* interrupt is triggered  by low level */
        {
            max3353intPin[0].config.interrupt = kPortIntLogicZero;
            GPIO_DRV_Init(max3353intPin, NULL);
        }
        else/* interrupt is triggered by falling edge */
        {
            max3353intPin[0].config.interrupt = kPortIntFallingEdge;
            GPIO_DRV_Init(max3353intPin, NULL);
        }
    }
    else
    {
        max3353intPin[0].config.interrupt = kPortIntDisabled;
        GPIO_DRV_Init(max3353intPin, NULL);
    }
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : otg_max3353_pin_int_clear
 * Returned Value   : none
 * Comments         :
 *    This function clears the pin interrupt flag associated with the max3353 interrupt pin 
 *
 *END*----------------------------------------------------------------------*/
void max3353_clear_pin_int_flag(void)
{
    GPIO_DRV_ClearPinIntFlag(BOARD_MAX3353_GPIO_INT);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : OTG_App_Init
 * Returned Value : none
 * Comments       :
 *
 *
 *END*--------------------------------------------------------------------*/
void APP_init()
{
    usb_otg_max3353_init_struct_t otg_max3353_init_param =
    {
        (void*) BOARD_MAX3353_INT_PORT,
        max3353_set_pin_int,
        max3353_clear_pin_int_flag,
        BOARD_MAX3353_INT_PIN,
        BOARD_MAX3353_INT_VECTOR,
        MAX3353_INT_LEVEL,
        BOARD_MAX3353_I2C_INSTANCE,
        BOARD_MAX3353_I2C_VECTOR,
        MAX3353_I2C_ADDRESS,
    };
    
    /* Initialize the current platform. Call for the _bsp_platform_init which is specific to each processor family */
    OS_Lock();
    g_otg_handle = NULL;
    g_otg_state = 0;
    g_dev_type = dev_b;
    g_sess_vld = FALSE;
    g_vbus_err = FALSE;

  #ifdef BOARD_USE_LPUART
    lpuart_user_config_t   usrt_config =
    {
        .clockSource = BOARD_LPUART_CLOCK_SOURCE,
        .baudRate = BOARD_DEBUG_UART_BAUD,
        .parityMode = kLpuartParityDisabled,
        .stopBitCount = kLpuartOneStopBit,
        .bitCountPerChar = kLpuart8BitsPerChar,
    };
    OS_install_isr((IRQn_Type) g_lpuartRxTxIrqId[BOARD_DEBUG_UART_INSTANCE], uart_rx_tx_hander, NULL);
    if (kStatus_LPUART_Initialized == LPUART_DRV_Init(BOARD_DEBUG_UART_INSTANCE, &g_uart_state, &usrt_config))
    {
        LPUART_DRV_Deinit(BOARD_DEBUG_UART_INSTANCE);
        LPUART_DRV_Init(BOARD_DEBUG_UART_INSTANCE, &g_uart_state, &usrt_config);
    }
  #else
    uart_user_config_t   usrt_config = 
    {
        .baudRate = BOARD_DEBUG_UART_BAUD,
        .parityMode = kUartParityDisabled,
        .stopBitCount = kUartOneStopBit,
        .bitCountPerChar = kUart8BitsPerChar,
    };
    OS_install_isr((IRQn_Type) g_uartRxTxIrqId[BOARD_DEBUG_UART_INSTANCE], uart_rx_tx_hander, NULL);
    if (kStatus_UART_Initialized == UART_DRV_Init(BOARD_DEBUG_UART_INSTANCE, &g_uart_state, &usrt_config))
    {
        UART_DRV_Deinit(BOARD_DEBUG_UART_INSTANCE);
        UART_DRV_Init(BOARD_DEBUG_UART_INSTANCE, &g_uart_state, &usrt_config);
    }
  #endif
    g_otg_app_event_handle = OS_Event_create(0);
    usb_otg_set_peripheral_init_param(USB_OTG_PERIPHERAL_MAX3353, (usb_otg_peripheral_union_t*)&otg_max3353_init_param);
    configure_i2c_pins(BOARD_MAX3353_I2C_INSTANCE);
    USB_PRINTF("\n\r otg module is initilalizing:");
    usb_otg_init(USBCFG_DEFAULT_OTG_CONTROLLER, (otg_int_struct_t *) &g_otg_init, usb_otg_board_init, &g_otg_handle);
    USB_PRINTF("\n\rPress P to print the menu:");
  #ifdef BOARD_USE_LPUART
    OS_intr_init(g_lpuartRxTxIrqId[BOARD_DEBUG_UART_INSTANCE], 5, 0, TRUE);
  #else
    OS_intr_init(g_uartRxTxIrqId[BOARD_DEBUG_UART_INSTANCE], 5, 0, TRUE);
  #endif
    OS_Unlock();
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : App_OtgCallback
 * Returned Value : none
 * Comments       :
 *
 *
 *END*--------------------------------------------------------------------*/
void App_OtgCallback(usb_otg_handle handle, os_event_handle event)
{
    uint8_t device_state;
    device_state = usb_otg_get_state(handle);
    if (device_state == USB_OTG_DEVSTATE_B)
    {
        if (OS_Event_check_bit(event, OTG_B_IDLE))
        {
            g_dev_type = dev_b; /* Device type: B */
            g_sess_vld = FALSE; /* session not valid */
            g_otg_state = OTG_B_IDLE;
        }
        if (OS_Event_check_bit(event, OTG_B_IDLE_SRP_READY))
        {
            g_otg_state = OTG_B_IDLE_SRP_READY;
        }
        if (OS_Event_check_bit(event, OTG_B_PERIPHERAL))
        {
            g_otg_state = OTG_B_PERIPHERAL;
            if (g_sess_vld == FALSE)
            {
                g_sess_vld = TRUE; /* session valid */
            }
        }
        if (OS_Event_check_bit(event, OTG_B_HOST))
        {
            g_otg_state = OTG_B_HOST;
        }
        if (OS_Event_check_bit(event, OTG_B_A_HNP_REQ))
        {
            (void) usb_otg_bus_release(g_otg_handle);
        }
    }
    else if (device_state == USB_OTG_DEVSTATE_A)
    {
        if (OS_Event_check_bit(event, OTG_A_WAIT_BCON_TMOUT))
        {
            usb_otg_set_a_bus_req(g_otg_handle, FALSE);
        }
        if (OS_Event_check_bit(event, OTG_A_BIDL_ADIS_TMOUT))
        {
            usb_otg_set_a_bus_req(g_otg_handle, TRUE);
        }
        if (OS_Event_check_bit(event, OTG_A_B_HNP_REQ))
        {
            usb_otg_set_a_bus_req(handle, FALSE);
        }
        if (OS_Event_check_bit(event, OTG_A_IDLE))
        {
            g_dev_type = dev_a; /* Device type: A */
            g_sess_vld = FALSE;
            g_otg_state = OTG_A_IDLE;
        }
        if (OS_Event_check_bit(event, OTG_A_WAIT_VRISE))
        {
            g_otg_state = OTG_A_WAIT_VRISE;
            usb_otg_set_a_bus_req(handle, TRUE);
        }
        if (OS_Event_check_bit(event, OTG_A_WAIT_BCON))
        {
            g_sess_vld = TRUE;
            g_otg_state = OTG_A_WAIT_BCON;
        }
        if (OS_Event_check_bit(event, OTG_A_HOST))
        {
            g_otg_state = OTG_A_HOST;
        }
        if (OS_Event_check_bit(event, OTG_A_SUSPEND))
        {
            g_otg_state = OTG_A_SUSPEND;
        }
        if (OS_Event_check_bit(event, OTG_A_PERIPHERAL))
        {
            g_otg_state = OTG_A_PERIPHERAL;
        }
        if (OS_Event_check_bit(event, OTG_A_WAIT_VFALL))
        {
            if (g_vbus_err == TRUE)
            {
                g_vbus_err = FALSE;
            }
            g_otg_state = OTG_A_WAIT_VFALL;
        }
        if (OS_Event_check_bit(event, OTG_A_VBUS_ERR))
        {
            g_vbus_err = TRUE;
            g_otg_state = OTG_A_VBUS_ERR;
        }
    }
    /* print the states */
    App_Print_Event(handle, event);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : App_HandleUserInput
 * Returned Value : none
 * Comments       :
 *
 *
 *END*--------------------------------------------------------------------*/

static void App_HandleUserInput(void)
{
    uint8_t character = 0;
  #ifdef BOARD_USE_LPUART
    uint32_t recLength = 0;
    lpuart_status_t ret = kStatus_LPUART_Success;
    static uint8_t rec_char = 0;
    ret = LPUART_DRV_GetReceiveStatus(BOARD_DEBUG_UART_INSTANCE, &recLength);

    if (kStatus_LPUART_Success == ret)
    {
        character = rec_char;
        rec_char = 0;
        LPUART_DRV_ReceiveData(BOARD_DEBUG_UART_INSTANCE, &rec_char, 1);
    }
    if(character != (int8_t)0x00)
    {
        USB_PRINTF("\n\r %c", character);
  #else
    uint32_t recLength = 0;
    uart_status_t ret = kStatus_UART_Success;
    static uint8_t rec_char = 0;
    ret = UART_DRV_GetReceiveStatus(BOARD_DEBUG_UART_INSTANCE, &recLength);

    if (kStatus_UART_Success == ret)
    {
        character = rec_char;
        rec_char = 0;
        UART_DRV_ReceiveData(BOARD_DEBUG_UART_INSTANCE, &rec_char, 1);
    }
    if(character != (int8_t)0x00)
    {
        USB_PRINTF("\n\r %c", character);
  #endif
        switch(character)
        {

        case '1':
            if (usb_otg_session_request(g_otg_handle) == USB_OK)
            {
                USB_PRINTF("\n\rSRP request");
            }
            else
            {
                USB_PRINTF("\n\rError Requesting SRP");
            }
            break;
        case '2':
            if (usb_otg_bus_request(g_otg_handle) == USB_OK)
            {
                USB_PRINTF("\n\rB bus request");
            }
            else
            {
                USB_PRINTF("\n\rError requesting the bus");
            }
            break;

        case '3':
            if (usb_otg_bus_release(g_otg_handle) == USB_OK)
            {
                USB_PRINTF("\n\rB bus release");
            }
            else
            {
                USB_PRINTF("\n\rError releasing the bus");
            }
            break;
        case '4':
            usb_otg_set_a_bus_req(g_otg_handle, TRUE);
            USB_PRINTF("\n\rA bus request");
            break;
        case '5':
            usb_otg_set_a_bus_req(g_otg_handle, FALSE);
            USB_PRINTF("\n\rA bus release");
            break;
        case '6':
            usb_otg_set_a_bus_drop(g_otg_handle, TRUE);
            USB_PRINTF("\n\rA set a bus drop true");
            break;
        case '7':
            usb_otg_set_a_bus_drop(g_otg_handle, FALSE);
            USB_PRINTF("\n\rA set a bus drop false");
            break;
        case '8':
            usb_otg_set_a_clear_err(g_otg_handle);
            break;
        case 'p':
        case 'P': App_PrintMenu();
            break;
        default:
            break;
        }
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : App_PrintMenu
 * Returned Value : none
 * Comments       :
 *
 *
 *END*--------------------------------------------------------------------*/
static void App_PrintMenu(void)
{
    bool a_bus_req;
    bool a_bus_drop;
    USB_PRINTF("\n\r  OTG App User Input Menu");
    if (g_dev_type == dev_a)
    {
        if (g_vbus_err == FALSE)
        {
            if (g_sess_vld == TRUE)
            {
                usb_otg_get_a_bus_req(g_otg_handle, &a_bus_req);
                if (FALSE == a_bus_req)
                {
                    USB_PRINTF("\n\r      4. A bus request ");
                }
                else
                {
                    USB_PRINTF("\n\r      5. A bus release ");
                }
                USB_PRINTF("\n\r      6. A set a bus drop true (session end)");
            }
            else /* session not valid */
            {
                usb_otg_get_a_bus_req(g_otg_handle, &a_bus_req);
                if (FALSE == a_bus_req)
                {
                    USB_PRINTF("\n\r      4. A bus request ");
                }
                else
                {
                    USB_PRINTF("\n\r      5. A bus release ");
                }
                usb_otg_get_a_bus_drop(g_otg_handle, &a_bus_drop);
                if (TRUE == a_bus_drop)
                {
                    USB_PRINTF("\n\r      7. A set a bus drop false");
                }
            }
        }
        else /* no VBUS error */
        {
            USB_PRINTF("\n\r      6. A set a bus drop true (session end)");
            USB_PRINTF("\n\r      8. A clear error");
        }
    }
    else if (g_dev_type == dev_b)
    {
        if (g_sess_vld == TRUE)
        {
            if (g_otg_state == OTG_B_PERIPHERAL)
            {
                USB_PRINTF("\n\r      2. B bus request (HNP start)");
            }
            if (g_otg_state == OTG_B_HOST)
            {
                USB_PRINTF("\n\r      3. B bus release");
            }
        }
        else
        {
            USB_PRINTF("\n\r      1. B session request (SRP start)");
        }
    }
}
/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : App_Print_Event
 * Returned Value : none
 * Comments       :
 *
 *
 *END*--------------------------------------------------------------------*/
static void App_Print_Event(usb_otg_handle handle, os_event_handle event)
{
    uint8_t device_state;
    device_state = usb_otg_get_state(handle);
    if (device_state == USB_OTG_DEVSTATE_B)
    {
        if (OS_Event_check_bit(event, OTG_B_IDLE))
        {
            USB_PRINTF("\n\r>B: OTG state change to B idle");
        }
        if (OS_Event_check_bit(event, OTG_B_IDLE_SRP_READY))
        {
            USB_PRINTF("\n\r>B: OTG is ready to initialize SRP");

        }
        if (OS_Event_check_bit(event, OTG_B_SRP_INIT))
        {
            USB_PRINTF("\n\r>B: OTG has initialized SRP");
        }
        if (OS_Event_check_bit(event, OTG_B_SRP_FAIL))
        {
            USB_PRINTF("\n\r>B: OTG SRP failed to get a response from the Host");
        }
        if (OS_Event_check_bit(event, OTG_B_PERIPHERAL))
        {
            USB_PRINTF("\n\r>B: OTG state change to B peripheral.");
            USB_PRINTF("\n\r>B: USB peripheral stack initialized.");
            App_PrintMenu();
        }
        if (OS_Event_check_bit(event, OTG_B_PERIPHERAL_LOAD_ERROR))
        {
            USB_PRINTF("\n\r>B: OTG state change to B peripheral.");
            USB_PRINTF("\n\r>B: USB peripheral stack initialization failed.");
        }
        if (OS_Event_check_bit(event, OTG_B_PERIPHERAL_HNP_READY))
        {
            USB_PRINTF("\n\r>B: OTG is ready to initialize HNP.");
        }
        if (OS_Event_check_bit(event, OTG_B_PERIPHERAL_HNP_START))
        {
            USB_PRINTF("\n\r>B: OTG has initialized the HNP to request the bus from Host");
        }
        if (OS_Event_check_bit(event, OTG_B_PERIPHERAL_HNP_FAIL))
        {
            USB_PRINTF("\n\r>B: HNP failed. OTG is back into peripheral state");
        }
        if (OS_Event_check_bit(event, OTG_B_HOST))
        {
            USB_PRINTF("\n\r>B: OTG is in the Host state");
            USB_PRINTF("\n\r>B: USB host stack initialized.");

        }
        if (OS_Event_check_bit(event, OTG_B_HOST_LOAD_ERROR))
        {
            USB_PRINTF("\n\r>B: OTG is in the Host state");
            USB_PRINTF("\n\r>B: USB host stack initialization failed.");
        }
        if (OS_Event_check_bit(event, OTG_B_A_HNP_REQ))
        {

            USB_PRINTF("\n\r>B: OTG_B_A_HNP_REQ");

        }
    }
    else if (device_state == USB_OTG_DEVSTATE_A)
    {
        if (OS_Event_check_bit(event, OTG_A_WAIT_BCON_TMOUT))
        {
            USB_PRINTF("\n\r>A: OTG_A_WAIT_BCON_TMOUT");
        }
        if (OS_Event_check_bit(event, OTG_A_BIDL_ADIS_TMOUT))
        {
            USB_PRINTF("\n\r>A: OTG_A_BIDL_ADIS_TMOUT");
        }
        if (OS_Event_check_bit(event, OTG_A_AIDL_BDIS_TMOUT))
        {
            USB_PRINTF("\n\r>A: OTG_A_AIDL_BDIS_TMOUT");
        }
        if (OS_Event_check_bit(event, OTG_A_ID_TRUE))
        {
            USB_PRINTF("\n\r>A: ID = TRUE ");
        }
        if (OS_Event_check_bit(event, OTG_A_WAIT_VRISE_TMOUT))
        {
            USB_PRINTF("\n\r>A: VBUS rise failed");
        }
        if (OS_Event_check_bit(event, OTG_A_B_HNP_REQ))
        {
            USB_PRINTF("\n\r>A: OTG_A_B_HNP_REQ");
        }
        if (OS_Event_check_bit(event, OTG_A_IDLE))
        {
            USB_PRINTF("\n\r>A: OTG state change to A_IDLE");
        }
        if (OS_Event_check_bit(event, OTG_A_WAIT_VRISE))
        {
            USB_PRINTF("\n\r>A: OTG state change to A_WAIT_VRISE");
        }
        if (OS_Event_check_bit(event, OTG_A_WAIT_BCON))
        {
            USB_PRINTF("\n\r>A: OTG state change to A_WAIT_BCON");
        }
        if (OS_Event_check_bit(event, OTG_A_HOST))
        {
            USB_PRINTF("\n\r>A: OTG state change to OTG_A_HOST");
            USB_PRINTF("\n\r>A: USB host stack initialized.");
        }
        if (OS_Event_check_bit(event, OTG_A_HOST_LOAD_ERROR))
        {
            USB_PRINTF("\n\r>A: OTG state change to OTG_A_HOST");
            USB_PRINTF("\n\r>A: USB host stack initialization failed.");
        }
        if (OS_Event_check_bit(event, OTG_A_SUSPEND))
        {
            USB_PRINTF("\n\r>A: OTG state change to A_SUSPEND");
        }
        if (OS_Event_check_bit(event, OTG_A_PERIPHERAL))
        {
            USB_PRINTF("\n\r>A: OTG state change to A_PERIPHERAL    ");
            USB_PRINTF("\n\r>A: USB peripheral stack initialized.");
            App_PrintMenu();
        }
        if (OS_Event_check_bit(event, OTG_A_PERIPHERAL_LOAD_ERROR))
        {
            USB_PRINTF("\n\r>A: USB peripheral stack initialization failed.");
            USB_PRINTF("\n\r>A: OTG state change to A_PERIPHERAL    ");
        }
        if (OS_Event_check_bit(event, OTG_A_WAIT_VFALL))
        {
            USB_PRINTF("\n\r>A: OTG state change to OTG_A_WAIT_VFALL");
        }
        if (OS_Event_check_bit(event, OTG_A_VBUS_ERR))
        {
            USB_PRINTF("\n\r>A: VBUS falls below VBUS_Valid treshold");
            USB_PRINTF("\n\r>A: OTG state change to A_VBUS_ERR");
        }
    }
    OS_Event_clear(event, OTG_A_B_STATE_EVENT_MARK);
}


#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    {   1L, Main_Task, 3000L, MQX_MAIN_TASK_PRIORITY, "Main", MQX_AUTO_START_TASK},
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

    OS_Task_create(Task_Start, NULL, 10L, 3000L, "task_start", NULL);
    OSA_Start();
#if !defined(FSL_RTOS_MQX)
    return 1;
#endif
}

