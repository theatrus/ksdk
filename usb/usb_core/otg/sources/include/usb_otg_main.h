/**HEADER********************************************************************
 * 
 * Copyright (c) 2010, 2015 Freescale Semiconductor;
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
 * $FileName: usb_otg_main.h$
 * $Version : 
 * $Date    : 
 *
 * Comments : This is the header file for the OTG driver
 *
 *         
 *****************************************************************************/

#ifndef USB_OTG_MAIN_H_
#define USB_OTG_MAIN_H_

#include "usb.h"
/* Public constants */

/* Number of available USB device controllers */
#define DEBUG_LOG_TRACE(x) USB_PRINTF("\n%s",x)
#define USB_OTG_DEVSTATE_B                  0x00
#define USB_OTG_DEVSTATE_A                  0x01
/* B state machine indications */
#define OTG_B_IDLE                      ((uint32_t)0x0001)     /* B idle state */
#define OTG_B_IDLE_SRP_READY            ((uint32_t)0x0002)     /* B idle, SRP ready to start */
#define OTG_B_SRP_INIT                  ((uint32_t)0x0004)     /* B srp init state */
#define OTG_B_SRP_FAIL                  ((uint32_t)0x0008)     /* B srp failed to get a response */
#define OTG_B_PERIPHERAL                ((uint32_t)0x0010)     /* B peripheral state */
#define OTG_B_PERIPHERAL_LOAD_ERROR     ((uint32_t)0x0020)     /* B peripheral state (peripheral stack could not be loaded) */
#define OTG_B_PERIPHERAL_HNP_READY      ((uint32_t)0x0040)     /* B peripheral, HNP ready to be performed */
#define OTG_B_PERIPHERAL_HNP_START      ((uint32_t)0x0080)     /* B peripheral, HNP start */
#define OTG_B_PERIPHERAL_HNP_FAIL       ((uint32_t)0x0100)     /* B peripheral, HNP failed */
#define OTG_B_HOST                      ((uint32_t)0x0200)     /* B host state */
#define OTG_B_HOST_LOAD_ERROR           ((uint32_t)0x0400)     /* B host state (host stack could not be loaded) */
#define OTG_B_A_HNP_REQ                 ((uint32_t)0x0800)
#if !defined(FSL_RTOS_FREE_RTOS)
/* A state machine indications */
#define OTG_A_IDLE                      ((uint32_t)0x00010000)
#define OTG_A_WAIT_VRISE                ((uint32_t)0x00020000)
#define OTG_A_WAIT_BCON                 ((uint32_t)0x00040000)
#define OTG_A_HOST                      ((uint32_t)0x00080000)
#define OTG_A_SUSPEND                   ((uint32_t)0x00100000)
#define OTG_A_PERIPHERAL                ((uint32_t)0x00200000)
#define OTG_A_WAIT_VFALL                ((uint32_t)0x00400000)
#define OTG_A_VBUS_ERR                  ((uint32_t)0x00800000)
#define OTG_A_WAIT_VRISE_TMOUT          ((uint32_t)0x01000000)
#define OTG_A_WAIT_BCON_TMOUT           ((uint32_t)0x02000000)
#define OTG_A_B_HNP_REQ                 ((uint32_t)0x04000000)
#define OTG_A_BIDL_ADIS_TMOUT           ((uint32_t)0x08000000)
#define OTG_A_ID_TRUE                   ((uint32_t)0x10000000)
#define OTG_A_HOST_LOAD_ERROR           ((uint32_t)0x20000000)
#define OTG_A_PERIPHERAL_LOAD_ERROR     ((uint32_t)0x40000000)
#define OTG_A_AIDL_BDIS_TMOUT           ((uint32_t)0x80000000)
#define USB_STATUS_OTG                 (0x09)
/* Public types */
#define OTG_STATE_EVENT_MASK            0xFFFFFFFF
#else
/* A state machine indications */
#define OTG_A_IDLE                      ((uint32_t)0x0001)
#define OTG_A_WAIT_VRISE                ((uint32_t)0x0002)
#define OTG_A_WAIT_BCON                 ((uint32_t)0x0004)
#define OTG_A_HOST                      ((uint32_t)0x0008)
#define OTG_A_SUSPEND                   ((uint32_t)0x0010)
#define OTG_A_PERIPHERAL                ((uint32_t)0x0020)
#define OTG_A_WAIT_VFALL                ((uint32_t)0x0040)
#define OTG_A_VBUS_ERR                  ((uint32_t)0x0080)
#define OTG_A_WAIT_VRISE_TMOUT          ((uint32_t)0x0100)
#define OTG_A_WAIT_BCON_TMOUT           ((uint32_t)0x0200)
#define OTG_A_B_HNP_REQ                 ((uint32_t)0x0400)
#define OTG_A_BIDL_ADIS_TMOUT           ((uint32_t)0x0800)
#define OTG_A_ID_TRUE                   ((uint32_t)0x1000)
#define OTG_A_HOST_LOAD_ERROR           ((uint32_t)0x2000)
#define OTG_A_PERIPHERAL_LOAD_ERROR     ((uint32_t)0x4000)
#define OTG_A_AIDL_BDIS_TMOUT           ((uint32_t)0x8000)
#define USB_STATUS_OTG                 (0x09)
/* Public types */
#define OTG_STATE_EVENT_MASK            0xFFFFFF
#endif
typedef os_event_handle otg_event;
/* Function for getting the active interrupts from the external circuit */
typedef uint32_t (*otg_load_usb_stack)(void);
typedef uint32_t (*otg_unload_usb_stack)(void);
typedef void (*otg_event_callback)(usb_otg_handle handle, otg_event event);

/* OTG initialization structure type */
#ifdef __CC_ARM
#pragma push
#pragma pack(1)
#endif
#ifdef __GNUC__
#pragma pack(push)
#pragma pack(1)
#endif
/*!
 * @brief OTG initialization structure.
 *
 * Define the application callback and the functions to get active interrupts.
 *
 */
typedef struct otg_init_struct
{

    otg_event_callback app_otg_callback; /*!< Application callback*/
    otg_load_usb_stack load_usb_host; /*!< load function for host*/
    otg_load_usb_stack load_usb_device; /*!< load function for device*/
    otg_unload_usb_stack unload_usb_host; /*!< unload function for host*/
    otg_unload_usb_stack unload_usb_device;/*!< unload function for device*/
} otg_int_struct_t;

typedef struct usb_otg_max3353_init_struct
{
    void* int_port;
    uint8_t int_pin;
    uint8_t int_vector;
    uint8_t priority;
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    char* i2c_dev_id;
#else
    uint8_t i2c_channel;
#endif
    uint8_t i2c_vector;
    uint8_t i2c_address;
} usb_otg_max3353_init_struct_t;

typedef union usb_otg_peripheral_union
{
    usb_otg_max3353_init_struct_t max3353_init;
} usb_otg_peripheral_union_t;

typedef enum
{
    USB_STACK_TYPE_DEVICE = 1,
    USB_STACK_TYPE_HOST,
    USB_STACK_TYPE_OTG
} usb_stack_type_t;

enum
{
    USB_ACTIVE_STACK_NONE,
    USB_ACTIVE_STACK_DEVICE,
    USB_ACTIVE_STACK_HOST
};

#define USB_OTG_PERIPHERAL_MAX3353  0

/*!
 * @brief OTG operation interface structure.
 *
 * Define the operation information for OTG.
 *
 */
typedef struct usb_otg_if_struct
{
    const struct usb_otg_callback_functions_struct *otg_if; /*!< Interface for operation*/
    void* otg_init_param;/*!< Initialization parameter*/
    void* otg_handle; /*!< Handle of OTG*/
    usb_stack_type_t stack_type; /*!< Type of stack*/
} usb_otg_if_struct_t;

/* Public functions */

/*!
 * @brief Initializes the internal (on chip) and external  OTG hardware.
 *
 * This function should be called prior to any other function of the OTG API. It verifies the
 * input parameters and if they are correct it allocates memory for the usb_otg_state_struct_t
 * initializes the structure , passes the pointer to this structure to application through the
 * handle parameter, and initializes the internal (on chip) and external  OTG hardware.
 *
 * @param controller_id controller ID
 * KHCI 0 --- 0
 * KHCI 1 --- 1
 * @param init_struct_pt OTG initialization structure
 * @param handle OTG handle
 * @return USB_OK-Success/Others-Fail
 */
extern usb_status usb_otg_init(uint8_t controller_id, otg_int_struct_t * init_struct_ptr, usb_otg_handle * handle);

/*!
 * @brief Stop the OTG controller.
 *
 * The function is used to stop the OTG controller.
 *
 * @param handle OTG handle
 * @return USB_OK-Success/Others-Fail
 */
extern usb_status usb_otg_shut_down(usb_otg_handle otg_handle);

/*!
 * @brief Start the SRP protocol from the B-Device side.
 *
 * The USB OTG session request function is used to start the SRP protocol from the
 * B-Device side with the target of requesting the Vbus from the A-Device.
 * The function is used to send a SRP signal and can only be called by the B role device.
 
 * @param handle OTG handle
 * @return USB_OK-Success/Others-Fail
 */
extern usb_status usb_otg_session_request(usb_otg_handle handle);

/*!
 * @brief Start the HNP protocol from the B-Device side.
 *
 * The USB OTG bus request function is used to start the HNP protocol from the B-Device
 * side with the target of obtaining the bus control from the A-Device (B-Peripheral will
 * become a B-Host and A-Host will become A-Peripheral). This request can only be
 * initiated from the B-device state.
 *
 * @param handle OTG handle
 * @return USB_OK-Success/Others-Fail
 */
extern usb_status usb_otg_bus_request(usb_otg_handle handle);

/*!
 * @brief End a Host session on the B-device.
 *
 * The USB OTG bus request function is used to end a Host session on the B-device.
 * This request is only accepted if the device is in the B-host state and results in the
 * B device releasing the bus and returning in the B-peripheral state.
 *
 * @param handle OTG handle
 * @return USB_OK-Success/Others-Fail
 */
extern usb_status usb_otg_bus_release(usb_otg_handle handle);

/*!
 * @brief Set USB Bus drop status.
 *
 * The function is used to set USB Bus drop status and can only be called by the A
 * role device.
 *
 * @param handle OTG handle
 * @return USB_OK-Success/Others-Fail
 */
extern usb_status usb_otg_set_a_bus_req(usb_otg_handle otg_handle, bool a_bus_req);

/*!
 * @brief Get USB Bus request status.
 *
 * The function is used to get USB Bus request status and can only be called by the A
 * role device.
 *
 * @param handle OTG handle
 * @param a_bus_req Request USB Bus status
 * @return USB_OK-Success/Others-Fail
 */
extern usb_status usb_otg_get_a_bus_req(usb_otg_handle otg_handle, bool* a_bus_req);

/*!
 * @brief Set USB Bus drop status.
 *
 * The function is used to set USB Bus drop status and can only be called by the A role
 * device.
 *
 * @param handle OTG handle
 * @param a_bus_drop set USB Bus drop status
 * @return USB_OK-Success/Others-Fail
 */
extern usb_status usb_otg_set_a_bus_drop(usb_otg_handle otg_handle, bool a_bus_drop);

/*!
 * @brief Get USB Bus drop status .
 *
 * The function is used to get USB Bus drop status and can only be called by the A role
 * device.
 *
 * @param handle OTG handle
 * @param a_bus_drop USB Bus drop status
 * @return USB_OK-Success/Others-Fail
 */
extern usb_status usb_otg_get_a_bus_drop(usb_otg_handle otg_handle, bool* a_bus_drop);

/*!
 * @brief Clear an error.
 *
 * The function is used to clear an error and can only be called by the A role device.
 *
 * @param handle OTG handle
 * @return USB_OK-Success/Others-Fail
 */
extern usb_status usb_otg_set_a_clear_err(usb_otg_handle otg_handle);

/*!
 * @brief Get the device role state.
 *
 * The function is used to get the device role state.
 *
 * @param handle OTG handle
 * @return USB_OK-Success/Others-Fail
 */
extern uint8_t usb_otg_get_state(usb_otg_handle otg_handle);

#ifdef __CC_ARM
#pragma pop
#endif
#ifdef __GNUC__
#pragma pack(pop)
#endif
#endif /* USB_OTG_MAIN_H_ */
