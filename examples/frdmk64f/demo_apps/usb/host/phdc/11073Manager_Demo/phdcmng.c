/**HEADER********************************************************************
 * 
 * Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
 * $FileName: phdcmng.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file is an example of device drivers for the PHDC class which intends
 *   to demonstrate the IEEE-11073 Manager functionality.
 *
 *END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"

#include "fsl_device_registers.h"
#include "fsl_clock_manager.h"
#include "fsl_port_hal.h"
#include "board.h"
#include "fsl_debug_console.h"
#include <stdio.h>
#include <stdlib.h>
//#include "fsl_uart_driver.h"

#include "usb_host_phdc.h"
#include "usb_host_hub_sm.h"

#include "ieee11073_phd_types.h"
#include "ieee11073_nom_codes.h"
#include "phdcmng.h"
#include <math.h>

/************************************************************************************
 ** Global variables
 ************************************************************************************/
device_struct_t g_phdc_device = { 0 };
usb_host_handle g_host_handle;
uint8_t g_phdc_interface_number = 0;
usb_device_interface_struct_t* g_phdc_interface_info[USBCFG_HOST_MAX_INTERFACE_PER_CONFIGURATION] = { NULL };
usb_phdc_param_t g_phdc_call_param_recv;
usb_phdc_param_t g_phdc_call_param_send;
os_event_handle g_phdc_usb_event;
/* Application function prototypes */
void APP_init(void);
void APP_task(void);
void phdc_manager_inuse(void);
bool phdc_manager_initialize_device(void);
usb_status usb_host_phdc_manager_event(
    usb_device_instance_handle dev_handle,
    usb_interface_descriptor_handle intf_handle,
    uint32_t event_code
    );
/* Manager APDU handlers */
void phdc_manager_aarq_handler(uint8_t *buffer);
void phdc_manager_prst_handler(uint8_t *buffer);
void phdc_manager_rlrq_handler(uint8_t *buffer);
void phdc_manager_abrt_handler(uint8_t *buffer);
void phdc_manager_prst_mdc_noti_config(apdu_t *apdu, data_apdu_t *dataApdu);
void phdc_manager_prst_mdc_noti_scanrep_fixed(apdu_t *apdu, data_apdu_t *dataApdu);
void phdc_manager_prst_get(apdu_t *apdu, data_apdu_t *dataApdu);
/* Manager configuration and scan handlers */
void phdc_manager_validate_print_fixedscan(observation_scan_fixed_list_t *scanList);
config_object_t* phdc_manager_get_configobj_byhandle(config_object_list_t *confObjList, handle_t obj_handle);
ava_type_t* phdc_manager_get_attribute_byid(config_object_t *confObj, oid_type_t attribute_id);
/* Manager presentation services */
void phdc_manager_handle_metric_nu_fixedscan(config_object_t *confObj, observation_scan_fixed_t* observation);
bool phdc_manager_print_nomenclature(oid_type_t type);
bool phdc_manager_print_partition(nom_partition_t partition);
void phdc_manager_print_float_val(uint8_t *value);
void phdc_manager_print_sfloat_val(uint8_t *value);
void phdc_manager_print_abs_timestamp(uint8_t *value);
void usb_host_phdc_send_callback(void* tr_ptr, void* param, uint8_t* buff_ptr, uint32_t buff_size, usb_status usb_sts);
void usb_host_phdc_recv_callback(void* tr_ptr, void* param, uint8_t* buff_ptr, uint32_t buff_size, usb_status usb_sts);
void usb_host_phdc_ctrl_callback(void* tr_ptr, void* param, uint8_t* buff_ptr, uint32_t buff_size, usb_status usb_sts);
usb_status usb_host_phdc_unsupported_device_event(
    usb_device_instance_handle dev_handle,
    usb_interface_descriptor_handle intf_handle,
    uint32_t event_code
    );
static usb_interface_descriptor_handle phdc_get_interface(void);
/* Table of driver capabilities this application wants to use */
static usb_host_driver_info_t g_driver_info_table[] =
{
    {
        {0x00, 0x00},       /* Vendor ID per USB-IF                               */
        {0x00, 0x00},       /* Product ID per manufacturer                        */
        USB_CLASS_PHDC,     /* Class code (PHDC)                                  */
        0,                  /* Sub-Class code (PHDC does not assign a subclass)   */
        0,                  /* Protocol                         */
        0,                  /* Reserved                         */
        usb_host_phdc_manager_event /* Application call back function   */
    },
    /* USB 1.1 hub */
    {
        {0x00, 0x00},           /* Vendor ID per USB-IF             */
        {0x00, 0x00},           /* Product ID per manufacturer      */
        USB_CLASS_HUB,          /* Class code                       */
        USB_SUBCLASS_HUB_NONE,  /* Sub-Class code                   */
        USB_PROTOCOL_HUB_LS,    /* Protocol                         */
        0,                      /* Reserved                         */
        usb_host_hub_device_event /* Application call back function   */
    },
    {
        { 0x00, 0x00 }, /* All-zero entry terminates        */
        { 0x00, 0x00 }, /* driver info list.                */
        0,
        0,
        0,
        0,
        NULL
    }
};

/* Predefined PHDC Manager messages */
/* association response to send */
uint8_t g_phd_mng_assoc_res[ASSOC_RES_SIZE] =
{
    0xE3, 0x00, /* APDU CHOICE Type (AareApdu) */
    0x00, 0x2C, /* CHOICE.length = 44 */
    0x00, 0x03, /* result = accepted-unknown-config (UPDATED by manager) */
    0x50, 0x79, /* data-proto-id = 20601 */
    0x00, 0x26, /* data-proto-info length = 38 */
    0x80, 0x00, 0x00, 0x00, /* protocolVersion */
    0x80, 0x00, /* encoding rules = MDER */
    0x80, 0x00, 0x00, 0x00, /* nomenclatureVersion */
    0x00, 0x00, 0x00, 0x00, /* functionalUnits */
    0x80, 0x00, 0x00, 0x00, /* systemType = sys-type-manager */
    0x00, 0x08, /* system-id length = 8 and value */
    0x4C, 0x4E, 0x49, 0x41, 0x47, 0x45, 0x4E, 0x54, /* (manufacturer- and device- specific) */
    0x00, 0x00, /* manager's response to config-id is always 0 */
    0x00, 0x00, 0x00, 0x00, /* manager's response to data-req-mode-capab is always 0 */
    0x00, 0x00, 0x00, 0x00 /* optionList.count = 0 | optionList.length = 0 */
};

uint8_t g_phd_mng_config_res[CONFIG_RES_SIZE] =
{
    0xE7, 0x00, /* APDU CHOICE Type (PrstApdu) */
    0x00, 0x16, /* CHOICE.length = 22 */
    0x00, 0x14, /* OCTET STRING.length = 20 */
    0x00, 0x00, /* invoke-id = 0x0000 (updated by mng) */
    0x02, 0x01, /* CHOICE (Remote Operation Response | Confirmed Event Report) */
    0x00, 0x0E, /* CHOICE.length = 14 */
    0x00, 0x00, /* obj-handle = 0 (MDS object) */
    0x00, 0x00, 0x00, 0x00, /* currentTime = 0 */
    0x0D, 0x1C, /* event-type = MDC_NOTI_CONFIG */
    0x00, 0x04, /* event-reply-info.length = 4 */
    0x00, 0x00, /* ConfigReportRsp.config-report-id = 0x0000 (updated by mng) */
    0x00, 0x00 /* ConfigReportRsp.config-result = accepted-config (updated by mng) */
};

uint8_t g_phd_mng_get_mds_attr[GET_MDS_ATTR_SIZE] =
{
    0xE7, 0x00, /* APDU CHOICE Type (PrstApdu) */
    0x00, 0x0E, /* CHOICE.length = 14 */
    0x00, 0x0C, /* OCTET STRING.length = 12 */
    0x34, 0x56, /* invoke-id = 0x3456 (start of DataApdu. MDER encoded.) */
    0x01, 0x03, /* CHOICE (Remote Operation Invoke | Get) */
    0x00, 0x06, /* CHOICE.length = 6 */
    0x00, 0x00, /* handle = 0 (MDS object) */
    0x00, 0x00, /* attribute-id-list.count = 0 (all attributes) */
    0x00, 0x00 /* attribute-id-list.length = 0 */
};

uint8_t g_phd_mng_scan_res[SCAN_RES_SIZE] =
{
    0xE7, 0x00, /* APDU CHOICE Type (PrstApdu) */
    0x00, 0x12, /* CHOICE.length = 18 */
    0x00, 0x10, /* OCTET STRING.length = 16 */
    0x43, 0x21, /* invoke-id = 0x4321 (start of DataApdu. MDER encoded.) */
    0x02, 0x01, /* CHOICE(Remote Operation Response | Confirmed Event Report) */
    0x00, 0x0A, /* CHOICE.length = 10 */
    0x00, 0x00, /* obj-handle = 0 (MDS object) */
    0xFF, 0xFF, 0xFF, 0xFF, /* currentTime = 0 */
    0x0D, 0x1D, /* event-type = MDC_NOTI_SCAN_REPORT_FIXED */
    0x00, 0x00 /* event-reply-info.length = 0 */
};

/* release request to send */
uint8_t g_phd_mng_rel_req[REL_REQ_SIZE] =
{
    0xE4, 0x00, /* APDU CHOICE Type (RlrqApdu) */
    0x00, 0x02, /* CHOICE.length = 2 */
    0x00, 0x00 /* reason = normal */
};

/* release response to the device */
uint8_t g_phd_wsl_rel_res[REL_RES_SIZE] =
{
    0xE5, 0x00, /* APDU CHOICE Type (RlrsApdu) */
    0x00, 0x02, /* CHOICE.length = 2 */
    0x00, 0x00 /* reason = normal */
};

/* abort request to send */
uint8_t g_phd_mng_abrt_req[ABRT_SIZE] =
{
    0xE6, 0x00, /* APDU CHOICE Type (RlrsApdu) */
    0x00, 0x02, /* CHOICE.length = 2 */
    0x00, 0x02 /* reason = normal */
};

static void update_state(void)
{
    if (g_phdc_device.state_change != 0)
    {
        if (g_phdc_device.state_change & USB_STATE_CHANGE_ATTACHED)
        {
            if (g_phdc_device.dev_state == USB_DEVICE_IDLE)
            {
                g_phdc_device.dev_state = USB_DEVICE_ATTACHED;
            }
            g_phdc_device.state_change &= ~(USB_STATE_CHANGE_ATTACHED);
        }
        if (g_phdc_device.state_change & USB_STATE_CHANGE_OPENED)
        {
            if (g_phdc_device.dev_state != USB_DEVICE_DETACHED)
            {
                g_phdc_device.dev_state = USB_DEVICE_INTERFACED;
            }
            g_phdc_device.state_change &= ~(USB_STATE_CHANGE_OPENED);
        }
        if (g_phdc_device.state_change & USB_STATE_CHANGE_DETACHED)
        {
            g_phdc_device.dev_state = USB_DEVICE_DETACHED;
            g_phdc_device.state_change &= ~(USB_STATE_CHANGE_DETACHED);
        }
    }
}

#if defined(FSL_RTOS_MQX)
void Main_Task(uint32_t param);
TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    {   1L, Main_Task, 1024L, MQX_MAIN_TASK_PRIORITY, "Main", MQX_AUTO_START_TASK},
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

    OS_Task_create(Task_Start, NULL, 5L, 4000L, "task_start", NULL);
    OSA_Start();
#if !defined(FSL_RTOS_MQX)
    return 1;
#endif
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_get_interface
 * Returned Value : none
 * Comments       :
 *     Used to get the proper interface if there are multiple interfaces in a device are available
 *     for us.
 *     In this example, we always choose the first interface.
 *     For the customer, a proper way should be implemented as needed.
 *
 *END*--------------------------------------------------------------------*/
static usb_interface_descriptor_handle phdc_get_interface(void)
{
    return (usb_interface_descriptor_handle)(g_phdc_interface_info[0]);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_phdc_unsupported_device_event
 * Returned Value : usb_status
 * Comments       :
 *     Called when unsupported device has been attached.
 *END*--------------------------------------------------------------------*/
usb_status usb_host_phdc_unsupported_device_event
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,
    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,
    /* [IN] code number for event causing callback */
    uint32_t event_code
    )
{
    usb_device_interface_struct_t* pDeviceIntf;
    interface_descriptor_t* intf_ptr;
    if (USB_ATTACH_INTF_NOT_SUPPORT == event_code)
    {
        pDeviceIntf = (usb_device_interface_struct_t*) intf_handle;
        intf_ptr = pDeviceIntf->lpinterfaceDesc;
        USB_PRINTF("----- Unsupported Interface of attached Device -----\n");
        USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
        USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
    }
    else if (USB_ATTACH_DEVICE_NOT_SUPPORT == event_code)
    {
        USB_PRINTF("----- Unsupported Device attached -----\n");
    }
    
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : ApplicationInit
 * Returned Value : none
 * Comments       :
 *     Execution starts here
 *
 *END*--------------------------------------------------------------------*/
void APP_init(void)
{
    /*
     ** It means that we are going to act like host, so we initialize the
     ** host stack. This call will allow USB system to allocate memory for
     ** data structures, it uses later (e.g pipes etc.).
     */
    usb_status status = USB_OK;

    status = usb_host_init(CONTROLLER_ID, usb_host_board_init,/* Use value in header file */
    &g_host_handle); /* Returned pointer */

    if (status != USB_OK)
    {
        USB_PRINTF("\n\rUSB Host Initialization failed! STATUS: 0x%x", (unsigned int) status);
        return;
    }

    /*
     ** since we are going to act as the host driver, register the driver
     ** information for wanted class/subclass/protocols
     */
    status = usb_host_register_driver_info(g_host_handle, (void *) g_driver_info_table);
    if (status != USB_OK)
    {
        USB_PRINTF("\n\rDriver Registration failed! STATUS: 0x%x", (unsigned int) status);
        return;
    }
    status = usb_host_register_unsupported_device_notify(g_host_handle, usb_host_phdc_unsupported_device_event);

    if (status != USB_OK)
    {
        USB_PRINTF("\nUSB Initialization driver info failed! STATUS: 0x%x", status);
        return;
    }
    g_phdc_usb_event = OS_Event_create(0);/* manually clear */

    if (g_phdc_usb_event == NULL)
    {
        USB_PRINTF("\nOS_Event_create failed!\n");
        return;
    }

    USB_PRINTF("\n\r****************************************************************************");
    USB_PRINTF("\n\rUSB PHDC Manager Demo");
    USB_PRINTF("\n\rWaiting for PHDC Agent to be attached...");
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : ApplicationTask
 * Returned Value : None
 * Comments       :
 *     Application function 
 *END*--------------------------------------------------------------------*/
void APP_task(void)
{
    usb_status status = USB_OK;
    if (OS_Event_wait(g_phdc_usb_event, USB_EVENT_DATA_ERR | USB_EVENT_DATA_OK | USB_EVENT_DATA_CORRUPTED | USB_EVENT_CTRL, FALSE, 0) == OS_EVENT_OK)
    {
        if (OS_Event_check_bit(g_phdc_usb_event, USB_EVENT_CTRL))
        {
            /* update state for not app_task context */
            update_state();
            OS_Event_clear(g_phdc_usb_event, USB_EVENT_CTRL);
        }
        switch(g_phdc_device.dev_state)
        {
        case USB_DEVICE_IDLE:
            break;
        case USB_DEVICE_ATTACHED:
            USB_PRINTF(" PHDC device attached");
            g_phdc_device.dev_state = USB_DEVICE_SET_INTERFACE_STARTED;

            status = usb_host_open_dev_interface(g_host_handle, g_phdc_device.dev_handle, g_phdc_device.intf_handle, (usb_class_handle*) &g_phdc_device.class_handle);
            if (status != USB_OK)
            {
                USB_PRINTF("\n\rError in _usb_hostdev_select_interface: %x", (unsigned int) status);
                return;
            }
            break;
        case USB_DEVICE_SET_INTERFACE_STARTED:
            break;

        case USB_DEVICE_INTERFACED:
            USB_PRINTF("\n\rPHDC device interfaced and ready");
            /* Advance to the INUSE state to process the PHDC messages */
            if (phdc_manager_initialize_device())
            {
                g_phdc_device.dev_state = USB_DEVICE_INUSE;
            }
            break;

        case USB_DEVICE_INUSE:
            if (OS_Event_check_bit(g_phdc_usb_event, USB_EVENT_DATA_CORRUPTED | USB_EVENT_DATA_ERR))
            {
                /* Schedule a new RX */
                g_phdc_call_param_recv.class_ptr = (usb_class_handle*) g_phdc_device.class_handle;
                g_phdc_call_param_recv.qos = 0xFE;
                g_phdc_call_param_recv.callback_fn = usb_host_phdc_recv_callback;
                (usb_status)usb_class_phdc_recv_data(&g_phdc_call_param_recv, (uint8_t*)g_phdc_device.phd_buffer, (uint32_t)APDU_MAX_BUFFER_SIZE);
            }
            if (OS_Event_check_bit(g_phdc_usb_event, USB_EVENT_DATA_OK))
            {
                phdc_manager_inuse();
            }
            break;

        case USB_DEVICE_DETACHED:
            USB_PRINTF("\n\rGoing to idle state");
            status = usb_host_close_dev_interface(g_host_handle, g_phdc_device.dev_handle, g_phdc_device.intf_handle, g_phdc_device.class_handle);
            if (status != USB_OK)
            {
                USB_PRINTF("error in _usb_hostdev_close_interface %x\n", status);
            }
            if (g_phdc_device.phd_buffer != NULL)
            {
                OS_Mem_free(g_phdc_device.phd_buffer);
                g_phdc_device.phd_buffer = NULL;
            }
            if (g_phdc_device.confObjList != NULL)
            {
                OS_Mem_free(g_phdc_device.confObjList);
                g_phdc_device.confObjList = NULL;
            }
            g_phdc_device.dev_state = USB_DEVICE_IDLE;
            break;
        default:
            break;
        }
        if (OS_Event_check_bit(g_phdc_usb_event, USB_EVENT_DATA_ERR))
        {
            OS_Event_clear(g_phdc_usb_event, USB_EVENT_DATA_ERR);
        }
        if (OS_Event_check_bit(g_phdc_usb_event, USB_EVENT_DATA_CORRUPTED))
        {
            OS_Event_clear(g_phdc_usb_event, USB_EVENT_DATA_CORRUPTED);
        }
        if (OS_Event_check_bit(g_phdc_usb_event, USB_EVENT_DATA_OK))
        {
            OS_Event_clear(g_phdc_usb_event, USB_EVENT_DATA_OK);
        }
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_inuse_loop
 * Returned Value : None
 * Comments       :
 *     This function handles all the manager events and actions during the usage time
 *END*--------------------------------------------------------------------*/
void phdc_manager_inuse(void)
{
    apdu_t *apdu;
    if (g_phdc_device.phd_recv_size)
    {
        apdu = (apdu_t*) (g_phdc_device.phd_buffer);

        /* APDU processing */
        switch(g_phdc_device.phd_manager_state)
        {
        case PHD_MNG_UNASSOCIATED:
            /* Expecting only an RxAssocRequest message (AarqApdu) */
            if (apdu->choice == USB_SHORT_BE_TO_HOST(AARQ_CHOSEN))
            {
                phdc_manager_aarq_handler(g_phdc_device.phd_buffer);
            }
            break;
        case PHD_MNG_WAIT_CONFIG:
            case PHD_MNG_OPERATING:
            /* Handle the messages received in this state */
            if (apdu->choice == USB_SHORT_BE_TO_HOST(PRST_CHOSEN))
            {
                phdc_manager_prst_handler(g_phdc_device.phd_buffer);
            }
            else
            {
                if (apdu->choice == USB_SHORT_BE_TO_HOST(ABRT_CHOSEN))
                {
                    phdc_manager_abrt_handler(g_phdc_device.phd_buffer);
                }
                else
                {
                    if (apdu->choice == USB_SHORT_BE_TO_HOST(RLRQ_CHOSEN))
                    {
                        phdc_manager_rlrq_handler(g_phdc_device.phd_buffer);
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    /* Schedule a new RX */
    g_phdc_call_param_recv.class_ptr = (usb_class_handle*) g_phdc_device.class_handle;
    g_phdc_call_param_recv.qos = 0xFE;
    g_phdc_call_param_recv.callback_fn = usb_host_phdc_recv_callback;   
    (usb_status)usb_class_phdc_recv_data(&g_phdc_call_param_recv, (uint8_t*)g_phdc_device.phd_buffer, (uint32_t)APDU_MAX_BUFFER_SIZE);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_aarq_handler
 * Returned Value : None
 * Comments       :
 *     This function checks the received association request message
 *END*--------------------------------------------------------------------*/
void phdc_manager_aarq_handler
(
    uint8_t *buffer
    )
{
    apdu_t *apdu = (apdu_t *) buffer;

    aare_apdu_t *pAare;
    associate_result_t assocResult = REJECTED_UNKNOWN;
    aarq_apdu_t *pAarq = &(apdu->u.aarq);

    if (pAarq->assoc_version != USB_LONG_BE_TO_HOST(ASSOC_VERSION1))
    {
        assocResult = REJECTED_UNSUPPORTED_ASSOC_VERSION;
    }
    else
    {
        if (pAarq->data_proto_list.value[0].data_proto_id != USB_SHORT_BE_TO_HOST(DATA_PROTO_ID_20601))
        {
            assocResult = REJECTED_NO_COMMON_PROTOCOL;
        }
        else
        {
            assocResult = ACCEPTED_UNKNOWN_CONFIG;
            /* Advance the manager state */
            USB_PRINTF("\n\r11073 MNG Demo: Received a valid association request.");
            g_phdc_device.phd_manager_state = PHD_MNG_WAIT_CONFIG;
        }
    }

    /* Send back the response */
    /* APDU now pointing to the response */
    apdu = (apdu_t*) &g_phd_mng_assoc_res[0];

    /* Update the association result in the message template */
    pAare = &(apdu->u.aare);
    pAare->result = USB_SHORT_BE_TO_HOST(assocResult);

    /* Send data */
    g_phdc_call_param_send.class_ptr = g_phdc_device.class_handle;
    g_phdc_call_param_send.metadata = FALSE;
    g_phdc_call_param_send.callback_fn = usb_host_phdc_send_callback;

    USB_PRINTF("\n\r11073 MNG Demo: Send back association response. Waiting for config...");
    usb_class_phdc_send_data(&g_phdc_call_param_send, (uint8_t*)&g_phd_mng_assoc_res[0], (uint32_t)ASSOC_RES_SIZE);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_prst_handler
 * Returned Value : None
 * Comments       :
 *     This function handles the PRST APDU message
 *END*--------------------------------------------------------------------*/
void phdc_manager_prst_handler
(
    uint8_t *buffer
    )
{
    apdu_t *apdu = (apdu_t *) buffer;
    prst_apdu_t *pPrst = (prst_apdu_t *) &(apdu->u.prst);
    data_apdu_t *dataApdu = (data_apdu_t *) &(pPrst->value[0]);

    /* Check the choice and obj-handle */
    if ((dataApdu->choice.choice == ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN) &&
    (dataApdu->choice.u.roiv_cmipConfirmedEventReport.obj_handle == 0) /* MDS Object */
    )
    {
        switch(USB_SHORT_BE_TO_HOST(dataApdu->choice.u.roiv_cmipConfirmedEventReport.event_type))
        {
        case MDC_NOTI_CONFIG:
            phdc_manager_prst_mdc_noti_config(apdu, dataApdu);
            break;
        case MDC_NOTI_SCAN_REPORT_FIXED:
            phdc_manager_prst_mdc_noti_scanrep_fixed(apdu, dataApdu);
            break;
        default:
            break;
        }
    }

    if ((dataApdu->choice.choice == USB_SHORT_BE_TO_HOST(RORS_CMIP_GET_CHOSEN)) &&
    (dataApdu->choice.u.rors_cmipGet.obj_handle == 0) /* MDS Object */
    )
    {
        phdc_manager_prst_get(apdu, dataApdu);
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_rlrq_handler
 * Returned Value : None
 * Comments       :
 *     This function handles the RLRQ APDU message
 *END*--------------------------------------------------------------------*/
void phdc_manager_rlrq_handler
(
    uint8_t *buffer
    )
{
    apdu_t *apdu = (apdu_t *) buffer;
    rlrq_apdu_t *pRlrq = (rlrq_apdu_t *) &(apdu->u.prst);

    USB_PRINTF("\n\r11073 MNG Demo: Received a Release request. Reason = %d", pRlrq->reason);
    if (pRlrq->reason == RELEASE_REQUEST_REASON_NORMAL)
    {
        USB_PRINTF(" (REASON_NORMAL)");
    }

    if (g_phdc_device.confObjList != NULL)
    {
        OS_Mem_free(g_phdc_device.confObjList);
        g_phdc_device.confObjList = NULL;
        USB_PRINTF("\n\r11073 MNG Demo: Device configuration memory is now deallocated");
    }

    USB_PRINTF("\n\r11073 MNG Demo: Manager state is now UNASSOCIATED");
    /* Advance the manager state */
    g_phdc_device.phd_manager_state = PHD_MNG_UNASSOCIATED;

    /* Send data -> Release Response */
    g_phdc_call_param_send.class_ptr = g_phdc_device.class_handle;
    g_phdc_call_param_send.metadata = FALSE;
    g_phdc_call_param_send.callback_fn = usb_host_phdc_send_callback;
    usb_class_phdc_send_data(&g_phdc_call_param_send, (uint8_t*) &g_phd_wsl_rel_res[0], (uint32_t)REL_RES_SIZE);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_abrt_handler
 * Returned Value : None
 * Comments       :
 *     This function handles the RLRQ APDU message
 *END*--------------------------------------------------------------------*/
void phdc_manager_abrt_handler
(
    uint8_t *buffer
    )
{
    apdu_t *apdu = (apdu_t *) buffer;
    abrt_apdu_t *pAbrt = (abrt_apdu_t *) &(apdu->u.prst);

    USB_PRINTF("\n\r11073 MNG Demo:Received an Abort request. Reason = %d", pAbrt->reason);

    if (g_phdc_device.confObjList != NULL)
    {
        OS_Mem_free(g_phdc_device.confObjList);
        g_phdc_device.confObjList = NULL;
        USB_PRINTF("\n\r11073 MNG Demo: Device configuration memory is now deallocated");
    }

    USB_PRINTF("\n\r11073 MNG Demo: Manager state is now UNASSOCIATED");
    /* Advance the manager state */
    g_phdc_device.phd_manager_state = PHD_MNG_UNASSOCIATED;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_prst_mdc_noti_config
 * Returned Value : None
 * Comments       :
 *     This function handles the MDC_NOTI_CONFIG event types
 *END*--------------------------------------------------------------------*/
void phdc_manager_prst_mdc_noti_config
(
    apdu_t *apdu,
    data_apdu_t *dataApdu
    )
{
    config_result_t configResult = UNSUPPORTED_CONFIG;
    invoke_id_type_t configInvokeID = 0;
    config_id_t configReportId = 0;
    uint16_t i;
    uint16_t j;
    uint16_t configOffset = 0;
    uint16_t attrOffset;
    uint16_t configListSize;
    config_report_t *configReport = (config_report_t *) &(dataApdu->choice.u.roiv_cmipConfirmedEventReport.event_info.value[0]);
    config_object_list_t *confObjList = &(configReport->config_obj_list);
    prst_apdu_t *pPrst;
    data_apdu_t *respDataApdu;
    config_report_rsp_t *reportResp;
    usb_phdc_param_t *phdc_call_param_send;

    configReport->config_report_id = USB_SHORT_BE_TO_HOST(configReport->config_report_id);
    confObjList->count = USB_SHORT_BE_TO_HOST(confObjList->count);
    USB_PRINTF("\n\r11073 MNG Demo: Received a configuration event report.");
    USB_PRINTF("\n\r11073 MNG Demo: -------------------------------------------");
    USB_PRINTF("\n\r11073 MNG Demo: Configuration Report Id: %d.", configReport->config_report_id);
    USB_PRINTF("\n\r11073 MNG Demo: Number of Agent Measurements Objects: %d.", confObjList->count);

    apdu->length = USB_SHORT_BE_TO_HOST(apdu->length);
    if ((apdu->length + sizeof(apdu->choice) + sizeof(apdu->length)) < APDU_MAX_BUFFER_SIZE)
    {
        configReportId = configReport->config_report_id;
        configInvokeID = dataApdu->invoke_id;
        confObjList->length = USB_SHORT_BE_TO_HOST(confObjList->length);
        /* Save the config obj list in the device structure */
        configListSize = (uint16_t)(confObjList->length + sizeof(confObjList->count) + sizeof(confObjList->length));

        if (g_phdc_device.confObjList != NULL)
        {
            OS_Mem_free(g_phdc_device.confObjList);
        }
        g_phdc_device.confObjList = (config_object_list_t*) OS_Mem_alloc_uncached_zero(configListSize);
        if (g_phdc_device.confObjList != NULL)
        {
            /* Store the Configuration report in the application */
            OS_Mem_copy((void*) confObjList, (void*) g_phdc_device.confObjList, configListSize);

            for (i = 0; i < confObjList->count; i++)
            {
                config_object_t *confObj = (config_object_t *) (((uint8_t*) &confObjList->value[0]) + configOffset);
                confObj->obj_handle = USB_SHORT_BE_TO_HOST(confObj->obj_handle);
                confObj->obj_class = USB_SHORT_BE_TO_HOST(confObj->obj_class);
                confObj->attributes.count = USB_SHORT_BE_TO_HOST(confObj->attributes.count);
                USB_PRINTF("\n\r11073 MNG Demo:  > Object Handle %d: Class = %d  Num Attributes = %d.", confObj->obj_handle, confObj->obj_class, confObj->attributes.count);
                attrOffset = 0;
                for (j = 0; j < confObj->attributes.count; j++)
                {
                    ava_type_t *attr = (ava_type_t *) ((uint8_t*) (&confObj->attributes.value[0]) + attrOffset);
                    USB_PRINTF("\n\r11073 MNG Demo:  > > Attribute%d: Id = ", j);
                    if (!phdc_manager_print_nomenclature(attr->attribute_id))
                    {
                        USB_PRINTF("%d (unsupported ASCII nomenclature", USB_SHORT_BE_TO_HOST(attr->attribute_id));
                    }

                    attr->attribute_value.length = USB_SHORT_BE_TO_HOST(attr->attribute_value.length);
                    /* Go to the next attribute in the list */
                    attrOffset += sizeof(attr->attribute_id) +
                    sizeof(attr->attribute_value.length) +
                    attr->attribute_value.length;
                }
                /* Go to the next config object in the list */
                confObj->attributes.length = USB_SHORT_BE_TO_HOST(confObj->attributes.length);
                configOffset += sizeof(confObj->obj_class) +
                sizeof(confObj->obj_handle) +
                sizeof(confObj->attributes.count) +
                sizeof(confObj->attributes.length) +
                confObj->attributes.length;
            }
            configResult = ACCEPTED_CONFIG;
            /* Advance the manager state */
            g_phdc_device.phd_manager_state = PHD_MNG_OPERATING;
        }
        else
        {
            USB_PRINTF("\n\r11073 MNG Demo: Cannot allocate memory to store the configuration");
        }
    }
    else
    {
        /* Advance the manager state */
        g_phdc_device.phd_manager_state = PHD_MNG_UNASSOCIATED;
        USB_PRINTF("\n\r11073 APDU length (%d) exceeds allocated RX buffer length (%d)", apdu->length + sizeof(apdu->choice) + sizeof(apdu->length), APDU_MAX_BUFFER_SIZE);
    }

    USB_PRINTF("\n\r11073 MNG Demo: -------------------------------------------");
    if (configResult == ACCEPTED_CONFIG)
    {
        USB_PRINTF("\n\r11073 MNG Demo: Configuration Accepted.");
    }
    else
    {
        USB_PRINTF("\n\r11073 MNG Demo: Configuration NOT Accepted.");
    }

    /* Send back the configuration response */
    /* Update the predefined config response message fields */
    apdu = (apdu_t *) &g_phd_mng_config_res[0];
    pPrst = (prst_apdu_t *) &(apdu->u.prst);
    respDataApdu = (data_apdu_t *) &(pPrst->value[0]);

    respDataApdu->invoke_id = configInvokeID;
    reportResp = (config_report_rsp_t *) &(respDataApdu->choice.u.roiv_cmipConfirmedEventReport.event_info.value[0]);
    reportResp->config_report_id = USB_SHORT_BE_TO_HOST(configReportId);
    reportResp->config_result = configResult;

    /* Send data -> Configuration Response */
    g_phdc_call_param_send.class_ptr = g_phdc_device.class_handle;
    g_phdc_call_param_send.metadata = FALSE;
    g_phdc_call_param_send.callback_fn = usb_host_phdc_send_callback;
    USB_PRINTF("\n\r11073 MNG Demo: Send back configuration response.");
    usb_class_phdc_send_data(&g_phdc_call_param_send, (uint8_t*)&g_phd_mng_config_res[0], (uint32_t)CONFIG_RES_SIZE);

    if (g_phdc_device.phd_manager_state == PHD_MNG_OPERATING)
    {
        USB_PRINTF("\n\r11073 MNG Demo: The MANAGER is now in the OPERATING state. Ready to receive measurements.");
        /* Send data -> Get MDS Attributes */
        g_phdc_call_param_send.class_ptr = g_phdc_device.class_handle;
        g_phdc_call_param_send.metadata = FALSE;
        g_phdc_call_param_send.callback_fn = usb_host_phdc_send_callback;
        USB_PRINTF("\n\r11073 MNG Demo: Send ROIGET MDS Attributes(all) request.");
        usb_class_phdc_send_data(&g_phdc_call_param_send, (uint8_t*)&g_phd_mng_get_mds_attr[0], (uint32_t)GET_MDS_ATTR_SIZE);
    }
    else
    {
        /* configuration was not accepted. The manager is in the UNASSOCIATED state */
        USB_PRINTF("\n\r11073 MNG Demo: The MANAGER is now in the UNASSOCIATED state.");
        /* Send abort */
        g_phdc_call_param_send.class_ptr = g_phdc_device.class_handle;
        g_phdc_call_param_send.metadata = FALSE;
        g_phdc_call_param_send.callback_fn = usb_host_phdc_send_callback;
        USB_PRINTF("\n\r11073 MNG Demo: Send Abort message to the device.");
        usb_class_phdc_send_data(&g_phdc_call_param_send, (uint8_t*)&g_phd_mng_abrt_req[0], (uint32_t)ABRT_SIZE);
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_prst_mdc_noti_scanrep_fixed
 * Returned Value : None
 * Comments       :
 *     This function handles the MDC_NOTI_SCAN_REPORT event types
 *END*--------------------------------------------------------------------*/
void phdc_manager_prst_mdc_noti_scanrep_fixed
(
    apdu_t *apdu,
    data_apdu_t *dataApdu
    )
{
    scan_report_info_fixed_t *scan_rep;
    prst_apdu_t *pPrst;
    data_apdu_t *respDataApdu;
    usb_phdc_param_t *phdc_call_param_send;

    USB_PRINTF("\n\r11073 MNG Demo: Received a Scan Report (Fixed) event.");
    USB_PRINTF("\n\r11073 MNG Demo: -------------------------------------------");

    scan_rep = (scan_report_info_fixed_t *) &(dataApdu->choice.u.roiv_cmipConfirmedEventReport.event_info.value[0]);
    scan_rep->scan_report_no = USB_SHORT_BE_TO_HOST(scan_rep->scan_report_no);
    scan_rep->obs_scan_fixed.count = USB_SHORT_BE_TO_HOST(scan_rep->obs_scan_fixed.count);
    USB_PRINTF("\n\r11073 MNG Demo: Scan Report Num: %d  Num Observations: %d", scan_rep->scan_report_no, scan_rep->obs_scan_fixed.count);

    phdc_manager_validate_print_fixedscan(&(scan_rep->obs_scan_fixed));

    /* Update the predefined scan response message fields */
    apdu = (apdu_t *) &g_phd_mng_scan_res[0];
    pPrst = (prst_apdu_t *) &(apdu->u.prst);
    respDataApdu = (data_apdu_t *) &(pPrst->value[0]);
    respDataApdu->invoke_id = dataApdu->invoke_id;

    /* Send data -> Scan Response */
    g_phdc_call_param_send.class_ptr = g_phdc_device.class_handle;
    g_phdc_call_param_send.metadata = FALSE;
    g_phdc_call_param_send.callback_fn = usb_host_phdc_send_callback;
    USB_PRINTF("\n\r11073 MNG Demo: -------------------------------------------");
    USB_PRINTF("\n\r11073 MNG Demo: Send back measurements confirmation.");
    usb_class_phdc_send_data(&g_phdc_call_param_send, (uint8_t*)&g_phd_mng_scan_res[0], (uint32_t)SCAN_RES_SIZE);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_prst_get
 * Returned Value : None
 * Comments       :
 *     This function handles the GET prst choice
 *END*--------------------------------------------------------------------*/
void phdc_manager_prst_get
(
    apdu_t *apdu,
    data_apdu_t *dataApdu
    )
{
    attribute_list_t *attribute_list = &(dataApdu->choice.u.rors_cmipGet.attribute_list);
    uint16_t i;
    uint16_t j;
    uint16_t length;
    uint16_t genOffset;
    uint16_t attrOffset = 0;
    type_ver_list_t *type_ver_list;
    attribute_list->count = USB_SHORT_BE_TO_HOST(attribute_list->count);

    //  UNUSED_ARGUMENT(apdu);

    USB_PRINTF("\n\r11073 MNG Demo: Received a GET Response.");
    USB_PRINTF("\n\r11073 MNG Demo: -------------------------------------------");
    USB_PRINTF("\n\r11073 MNG Demo: Number of attributes = %d", attribute_list->count);

    for (i = 0; i < attribute_list->count; i++)
    {
        ava_type_t *attr = (ava_type_t *) ((uint8_t*) (&attribute_list->value[0]) + attrOffset);
        attr->attribute_value.length = USB_SHORT_BE_TO_HOST(attr->attribute_value.length);
        switch(USB_SHORT_BE_TO_HOST(attr->attribute_id))
        {
        case MDC_ATTR_SYS_TYPE_SPEC_LIST:
            type_ver_list = (type_ver_list_t*) &attr->attribute_value.value[0];
            type_ver_list->count = USB_SHORT_BE_TO_HOST(type_ver_list->count);
            for (j = 0; j < type_ver_list->count; j++)
            {
                int8_t *typeString = NULL;
                type_ver_t *type_ver = (type_ver_t *) &type_ver_list->value[j];
                switch(USB_SHORT_BE_TO_HOST(type_ver->type))
                {
                case MDC_DEV_SPEC_PROFILE_PULS_OXIM:
                    typeString = (int8_t*) "Pulse Oximeter";
                    break;
                case MDC_DEV_SPEC_PROFILE_BP:
                    typeString = (int8_t*) "Blood Presure";
                    break;
                case MDC_DEV_SPEC_PROFILE_TEMP:
                    typeString = (int8_t*) "Thermometer";
                    break;
                case MDC_DEV_SPEC_PROFILE_SCALE:
                    typeString = (int8_t*) "Scale";
                    break;
                case MDC_DEV_SPEC_PROFILE_GLUCOSE:
                    typeString = (int8_t*) "Glucose Meter";
                    break;
                case MDC_DEV_SPEC_PROFILE_HF_CARDIO:
                    typeString = (int8_t*) "HF Cardio";
                    break;
                case MDC_DEV_SPEC_PROFILE_HF_STRENGTH:
                    typeString = (int8_t*) "HF Strength";
                    break;
                default:
                    break;
                }
                if (typeString != NULL)
                {
                    type_ver->type = USB_SHORT_BE_TO_HOST(type_ver->type);
                    type_ver->version = USB_SHORT_BE_TO_HOST(type_ver->version);
                    USB_PRINTF("\n\r11073 MNG Demo: Type = %d (%s)  Version = %d", type_ver->type, typeString, type_ver->version);
                }
                else
                {
                    USB_PRINTF("\n\r11073 MNG Demo: Type = %d  Version = %d", type_ver->type, type_ver->version);
                }
            }
            break;
        case MDC_ATTR_ID_MODEL:
            length = attr->attribute_value.length;
            genOffset = 0;
            USB_PRINTF("\n\r11073 MNG Demo: Model: ");
            while (length - genOffset)
            {
                any_t *str = (any_t *) ((uint8_t*) &attr->attribute_value.value[0] + genOffset);
                str->length = USB_SHORT_BE_TO_HOST(str->length);
                for (j = 0; j < str->length; j++)
                {
                    char tempString[2];
                    tempString[0] = (char) str->value[j];
                    tempString[1] = '\0';
                    USB_PRINTF("%s", tempString);
                }
                USB_PRINTF("  ");
                genOffset += sizeof(str->length) + str->length;
            }
            break;
        default:
            break;
        }
        /* Go to the next attribute in the list */
        attrOffset += sizeof(attr->attribute_id) +
        sizeof(attr->attribute_value.length) +
        attr->attribute_value.length;
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_validate_print_scan
 * Returned Value : None
 * Comments       :
 *     This function validates the received measurements 
 *     based on the existing device configuration
 *END*--------------------------------------------------------------------*/
void phdc_manager_validate_print_fixedscan
(
    observation_scan_fixed_list_t *scanList
    )
{
    observation_scan_fixed_t *observation;
    uint16_t i;
    uint16_t obsListOffset = 0;
    config_object_t *confObj;

    scanList->count = scanList->count;
    for (i = 0; i < scanList->count; i++)
    {
        observation = (observation_scan_fixed_t *) ((uint8_t*) &(scanList->value[0]) + obsListOffset);
        observation->obj_handle = USB_SHORT_BE_TO_HOST(observation->obj_handle);
        confObj = phdc_manager_get_configobj_byhandle(g_phdc_device.confObjList, observation->obj_handle);

        if (confObj != NULL)
        {
            if (confObj->obj_class == USB_SHORT_BE_TO_HOST(MDC_MOC_VMO_METRIC_NU))
            {
                phdc_manager_handle_metric_nu_fixedscan(confObj, observation);
            }
            else
            {
                USB_PRINTF("\n\r11073 MNG Demo: Usupported object class to print the observation scan : %d", confObj->obj_class);
            }
        }
        else
        {
            USB_PRINTF("\n\r11073 MNG Demo: Unknown object handle for observation scan : %d", observation->obj_handle);
        }
        observation->obs_val_data.length = USB_SHORT_BE_TO_HOST(observation->obs_val_data.length);
        obsListOffset += sizeof(observation->obj_handle) +
        sizeof(observation->obs_val_data.length) +
        observation->obs_val_data.length;
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_handle_metric_nu_fixedscan
 * Returned Value : None
 * Comments       :
 *     This function handles the observation data for the METRIC_NU class object
 *END*--------------------------------------------------------------------*/
void phdc_manager_handle_metric_nu_fixedscan
(
    config_object_t *confObj,
    observation_scan_fixed_t* observation
    )
{
    ava_type_t *attr_val_map, *gen_attr, *unit_code_attr;
    attr_val_map_t *val_map;
    uint16_t i, attrOffset = 0, obs_offset = 0, count, attribute_len;

    /* Search for the MDC_ATTR_ATTRIBUTE_VAL_MAP attribute in the object configuration
     * to see what type of measurements are we expecting. If this attribute does not exists 
     * in the configuration  object, then the measurements cannot be decoded.
     */
    attr_val_map = phdc_manager_get_attribute_byid(confObj, MDC_ATTR_ATTRIBUTE_VAL_MAP);
    if (attr_val_map != NULL)
    {
        /* Print the Type of the object. Get the MDC_ATTR_ID_TYPE attribute */
        gen_attr = phdc_manager_get_attribute_byid(confObj, MDC_ATTR_ID_TYPE);
        if (gen_attr != NULL)
        {
            /* Decode the attribute value - TYPE */
            type_t *type = (type_t *) &(gen_attr->attribute_value.value[0]);

            USB_PRINTF("\n\r11073 MNG Demo: Observation for object type: ");
            phdc_manager_print_nomenclature(type->code);
            USB_PRINTF(". Partition: ");
            phdc_manager_print_partition(type->partition);
        }

        /* Search for the measurement unit MDC_ATTR_UNIT_CODE */
        unit_code_attr = phdc_manager_get_attribute_byid(confObj, MDC_ATTR_UNIT_CODE);

        /* Decode the Value map to see what attributes are getting values from the observation scan */
        val_map = (attr_val_map_t *) &(attr_val_map->attribute_value.value[0]);
        count = USB_SHORT_BE_TO_HOST(val_map->count);

        for (i = 0; i < count; i++)
        {
            attr_val_map_entry_t *attr = (attr_val_map_entry_t *) ((uint8_t*) (&val_map->value[0]) + attrOffset);

            switch(USB_SHORT_BE_TO_HOST(attr->attribute_id))
            {
            case MDC_ATTR_NU_VAL_OBS_SIMP:
                /* SimpleNuObsValue -> Print the observation based on the FLOAT type */
                USB_PRINTF("\n\r11073 MNG Demo: Observation Value = ");
                phdc_manager_print_float_val(&observation->obs_val_data.value[obs_offset]);
                /* print also the unit code */
                if (unit_code_attr != NULL)
                {
                    USB_PRINTF(" ");
                    phdc_manager_print_nomenclature(*(oid_type_t*) &(unit_code_attr->attribute_value.value[0]));
                }
                break;
            case MDC_ATTR_NU_VAL_OBS_BASIC:
                /* BasicNuObsValue -> Print the observation based on the SFLOAT type */
                USB_PRINTF("\n\r11073 MNG Demo: Observation Value = ");
                phdc_manager_print_sfloat_val(&observation->obs_val_data.value[obs_offset]);
                /* print also the unit code */
                if (unit_code_attr != NULL)
                {
                    USB_PRINTF(" ");
                    phdc_manager_print_nomenclature(*(oid_type_t*) &(unit_code_attr->attribute_value.value[0]));
                }
                break;
            case MDC_ATTR_TIME_STAMP_ABS:
                /* AbsoluteTime */
                USB_PRINTF("\n\r11073 MNG Demo: Absolute Timestamp = ");
                phdc_manager_print_abs_timestamp(&observation->obs_val_data.value[obs_offset]);
                break;
            }
            attribute_len = USB_SHORT_BE_TO_HOST(attr->attribute_len);
            obs_offset += attribute_len;

            /* Go to the next attribute in the list */
            attrOffset += sizeof(attr->attribute_id) +
            sizeof(attr->attribute_len);
        }
    }
    else
    {
        USB_PRINTF("\n\r11073 MNG Demo: Missing MDC_ATTR_ATTRIBUTE_VAL_MAP attribute to print the fixed observation");
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_print_float_val
 * Returned Value : None
 * Comments       :
 *     This function prints an ascii representation of a Floating point number
 *      according to the medical device numeric format.
 *END*--------------------------------------------------------------------*/
void phdc_manager_print_float_val
(
    uint8_t *value
    )
{
    /* The FLOAT-Type has 32-bits representation */
    /* The resulted number can be calculated with the following formula: 
     *   F = mantisa * 10^exponent
     */
    uint32_t temp;
    uint32_t rawVal;
    int32_t mantisa;
    int8_t exponent;
    float decValue;

    memcpy(&temp, (uint8_t*) value, sizeof(temp));
    rawVal = USB_LONG_BE_TO_HOST(temp);
    mantisa = (((int32_t)(rawVal & 0xFFFFFF) << 8)) >> 8; /* Shifting left and right to propagate the sign bit */
    exponent = (int8_t)((int32_t)(rawVal & 0xFF000000) >> 24); /* int32 cast to ensure the sign bit is propagated */
    decValue = (float) (mantisa * pow(10, (double) exponent));
    USB_PRINTF("%f", decValue);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_print_sfloat_val
 * Returned Value : None
 * Comments       :
 *     This function prints an ascii representation of a S-Floating point number
 *      according to the medical device numeric format.
 *END*--------------------------------------------------------------------*/
void phdc_manager_print_sfloat_val
(
    uint8_t *value
    )
{
    /* The SFLOAT-Type has 16-bits representation */
    /* The resulted number can be calculated with the following formula: 
     *   F = mantisa * 10^exponent
     */
    uint16_t rawVal = USB_SHORT_BE_TO_HOST(*(uint16_t*) value);
    int16_t mantisa = (int16_t)((((int16_t)(rawVal & 0xFFF) << 4)) >> 4); /* Shifting left and right to propagate the sign bit */
    int8_t exponent = (int8_t)((int16_t)(rawVal & 0xF000) >> 12); /* int cast to ensure the sign bit is propagated */
    float decValue;
    decValue = (float) (mantisa * pow(10, (double) exponent));
    USB_PRINTF("%f", decValue);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_print_abs_timestamp
 * Returned Value : None
 * Comments       :
 *     This function prints an ascii representation of the Absolute Time stamp
 *      according to the medical device numeric format.
 *END*--------------------------------------------------------------------*/
void phdc_manager_print_abs_timestamp
(
    uint8_t *value
    )
{
    USB_PRINTF("%02x%02x-%02x-%02x  %02x:%02x", value[0], value[1], value[2], value[3], value[4], value[5]);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_print_nomenclature
 * Returned Value : None
 * Comments       :
 *     This function prints the nomenclature ASCII string based on the OID_Type
 *END*--------------------------------------------------------------------*/
bool phdc_manager_print_nomenclature(oid_type_t type)
{
    uint16_t i;
    for (i = 0; i < g_nom_ascii_count; i++)
    {
        if ((g_nomenclature_ascii_table[i].type == USB_SHORT_BE_TO_HOST(type)) && (g_nomenclature_ascii_table[i].ascii_string != NULL))
        {
            USB_PRINTF("%s", (uint8_t*) g_nomenclature_ascii_table[i].ascii_string);
            return TRUE;
        }
    }
    return FALSE;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_print_partition
 * Returned Value : None
 * Comments       :
 *     This function prints the partition ASCII string based on the OID_Type
 *END*--------------------------------------------------------------------*/
bool phdc_manager_print_partition
(
    nom_partition_t partition
    )
{
    uint16_t i;
    for (i = 0; i < g_partition_ascii_count; i++)
    {
        if ((g_partition_ascii_table[i].partition == USB_SHORT_BE_TO_HOST(partition)) && (g_partition_ascii_table[i].ascii_string != NULL))
        {
            USB_PRINTF("%s", (uint8_t*) g_partition_ascii_table[i].ascii_string);
            return TRUE;
        }
    }
    return FALSE;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_get_configobj_byhandle
 * Returned Value : None
 * Comments       :
 *     This function returns a configuration object from the provided ConfigObjectList
 *     using the requested obj_handle.
 *     If the obj_handle is not found in the list, this function returns NULL;
 *END*--------------------------------------------------------------------*/
config_object_t* phdc_manager_get_configobj_byhandle
(
    config_object_list_t *confObjList,
    handle_t obj_handle
    )
{
    config_object_t *confObjRet = NULL;
    uint16_t i;
    uint16_t configOffset = 0;
    uint16_t length;
    uint16_t obj_handle_tmp;

    if (confObjList != NULL)
    {
        for (i = 0; i < confObjList->count; i++)
        {
            config_object_t *confObj = (config_object_t *) (((uint8_t*) &confObjList->value[0]) + configOffset);
            obj_handle_tmp = USB_SHORT_BE_TO_HOST(confObj->obj_handle);
            if (obj_handle_tmp == obj_handle)
            {
                confObjRet = confObj;
                break;
            }
            length = USB_SHORT_BE_TO_HOST(confObj->attributes.length);
            /* Go to the next config object in the list */
            configOffset += sizeof(confObj->obj_class) +
            sizeof(confObj->obj_handle) +
            sizeof(confObj->attributes.count) +
            sizeof(confObj->attributes.length) +
            length;
        }
    }
    return confObjRet;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_get_attribute_byid
 * Returned Value : None
 * Comments       :
 *     This function returns the searched attribute from the configuration object
 *     If the attribute_id is not found in the config object, this function returns NULL;
 *END*--------------------------------------------------------------------*/
ava_type_t* phdc_manager_get_attribute_byid
(
    config_object_t *confObj,
    oid_type_t attribute_id
    )
{
    ava_type_t *attrObjRet = NULL;
    uint16_t i;
    uint16_t attrOffset = 0;
    uint16_t length;
    uint16_t count;

    count = USB_SHORT_BE_TO_HOST(confObj->attributes.count);
    for (i = 0; i < count; i++)
    {
        ava_type_t *attr = (ava_type_t *) ((uint8_t*) (&confObj->attributes.value[0]) + attrOffset);

        if (attr->attribute_id == USB_SHORT_BE_TO_HOST(attribute_id))
        {
            attrObjRet = attr;
            break;
        }

        length = USB_SHORT_BE_TO_HOST(attr->attribute_value.length);
        /* Go to the next attribute in the list */
        attrOffset += sizeof(attr->attribute_id) +
        sizeof(attr->attribute_value.length) +
        length;
    }
    return attrObjRet;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_phdc_manager_event
 * Returned Value : usb_status
 * Comments       :
 *     Called when PHDC device has been attached, detached, etc.
 *END*--------------------------------------------------------------------*/
usb_status usb_host_phdc_manager_event
(
    /* [IN] pointer to device instance */
    usb_device_instance_handle dev_handle,
    /* [IN] pointer to interface descriptor */
    usb_interface_descriptor_handle intf_handle,
    /* [IN] code number for event causing callback */
    uint32_t event_code
    )
{
    usb_device_interface_struct_t* pHostIntf = (usb_device_interface_struct_t*) intf_handle;
    interface_descriptor_t* intf_ptr = pHostIntf->lpinterfaceDesc;

    switch(event_code)
    {
    case USB_ATTACH_EVENT:
        g_phdc_interface_info[g_phdc_interface_number] = pHostIntf;
        g_phdc_interface_number++;
        USB_PRINTF("\n\r----- Attach Event -----");
        USB_PRINTF("  Interface Number = %d", intf_ptr->bInterfaceNumber);
        USB_PRINTF("  Alternate Setting = %d", intf_ptr->bAlternateSetting);
        USB_PRINTF("  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
        break;
    case USB_CONFIG_EVENT:
        if (g_phdc_device.dev_state == USB_DEVICE_IDLE)
        {
            g_phdc_device.dev_handle = dev_handle;
            g_phdc_device.intf_handle = phdc_get_interface();
            g_phdc_device.state_change |= USB_STATE_CHANGE_ATTACHED;

            /* notify application that status has changed */
            OS_Event_set(g_phdc_usb_event, USB_EVENT_CTRL);
        }
        else
        {
            USB_PRINTF("\n\rPHDC device already attached");
        }
        break;

    case USB_INTF_OPENED_EVENT:
        USB_PRINTF("\n\r----- Interfaced Event -----");
        g_phdc_device.state_change |= USB_STATE_CHANGE_OPENED;

        /* notify application that status has changed */
        OS_Event_set(g_phdc_usb_event, USB_EVENT_CTRL);
        break;

    case USB_DETACH_EVENT:
        /* Use only the interface with desired protocol */
        USB_PRINTF("\n\r----- Detach Event -----");
        USB_PRINTF("\n\rState = %d", (int32_t) g_phdc_device.dev_state);
        USB_PRINTF("\n\r  Class = %d", intf_ptr->bInterfaceClass);
        USB_PRINTF("\n\r  SubClass = %d", intf_ptr->bInterfaceSubClass);
        USB_PRINTF("\n\r  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
        g_phdc_interface_number = 0;
        g_phdc_device.state_change |= USB_STATE_CHANGE_DETACHED;

        /* notify application that status has changed */
        OS_Event_set(g_phdc_usb_event, USB_EVENT_CTRL);
        break;
    }
    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : phdc_manager_initialize_device
 * Returned Value : None
 * Comments       :
 *     Called after a PHDC device was interfaced. Fills the application-specific
 *     fields from the phdc_device structure, and sets the PHDC callbacks.
 *END*--------------------------------------------------------------------*/
bool phdc_manager_initialize_device(void)
{
    g_phdc_device.phd_buffer = (uint8_t*) OS_Mem_alloc_uncached_zero(APDU_MAX_BUFFER_SIZE);
    g_phdc_device.phd_recv_size = 0; /* Expecting an association request */
    g_phdc_device.phd_manager_state = PHD_MNG_UNASSOCIATED;
    if (g_phdc_device.phd_buffer != NULL)
    {
        g_phdc_call_param_recv.class_ptr = (usb_class_handle*) g_phdc_device.class_handle;
        g_phdc_call_param_recv.qos = 0xFE;
        g_phdc_call_param_recv.callback_fn = usb_host_phdc_recv_callback;   
        (usb_status)usb_class_phdc_recv_data(&g_phdc_call_param_recv, (uint8_t*)g_phdc_device.phd_buffer, (uint32_t)APDU_MAX_BUFFER_SIZE);
        return TRUE;
    }
    else
    {
        USB_PRINTF(" ERROR allocating memory");
        return FALSE;
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_phdc_send_callback
 * Returned Value : None
 * Comments       :
 *     Called when PHDC device has completed a send transfer.
 *END*--------------------------------------------------------------------*/
void usb_host_phdc_send_callback
(
/* [IN] Unused */
void* tr_ptr,
/* [IN] parameter specified by higher level */
void* param,
/* [IN] pointer to buffer containing data (Rx) */
uint8_t* buff_ptr,
/* [IN] length of data transferred */
uint32_t buff_size,
/* [IN] status, preferably USB_OK or USB_DONE */
usb_status usb_sts
)
{
    /* Free the memory allocated for call_param */
    //OS_Mem_free(call_param);
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_phdc_recv_callback
 * Returned Value : None
 * Comments       :
 *     Called when PHDC device has completed a receive transfer.
 *END*--------------------------------------------------------------------*/
void usb_host_phdc_recv_callback
(
/* [IN] Unused */
void* tr_ptr,
/* [IN] parameter specified by higher level */
void* param,
/* [IN] pointer to buffer containing data (Rx) */
uint8_t* buff_ptr,
/* [IN] length of data transferred */
uint32_t buff_size,
/* [IN] status, preferably USB_OK or USB_DONE */
usb_status usb_sts
)
{
    g_phdc_device.phd_recv_size = (uint8_t)buff_size;

    if (usb_sts == USB_OK)
    {
        OS_Event_set(g_phdc_usb_event, USB_EVENT_DATA_OK);
    }
    else if (usb_sts == USBERR_TR_CANCEL)
    {
        OS_Event_set(g_phdc_usb_event, USB_EVENT_DATA_CORRUPTED);
    }
    else
    {
        OS_Event_set(g_phdc_usb_event, USB_EVENT_DATA_ERR);
    }
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_phdc_recv_callback
 * Returned Value : None
 * Comments       :
 *     Called when PHDC device has completed a control transfer.
 *END*--------------------------------------------------------------------*/
void usb_host_phdc_ctrl_callback
(
/* [IN] Unused */
void* tr_ptr,
/* [IN] parameter specified by higher level */
void* param,
/* [IN] pointer to buffer containing data (Rx) */
uint8_t* buff_ptr,
/* [IN] length of data transferred */
uint32_t buff_size,
/* [IN] status, preferably USB_OK or USB_DONE */
usb_status usb_sts
)
{
    /* Free the memory allocated for call_param */
    //OS_Mem_free(call_param);
}
/* EOF */
