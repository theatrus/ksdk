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
 * $FileName: usb_host_phdc.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file contains  the USB PHDC class driver implementation
 *
 *END************************************************************************/

#include "usb_host_config.h"
#if USBCFG_HOST_PHDC
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_phdc.h"
#include "usb_host_common.h"
#include "usb_host_dev_mng.h"
#include "usb_host_ch9.h"

/*--------------------------------------------------------------**
 ** This anchor points to the first class/interface in a linked  **
 **   chain of structs, one for each functional phdc interface. **
 **   Items are added by calling "select interface" and removed  **
 **   by "delete interface".  Typically an application will call **
 **   these select in its attach callback routine.  It may later **
 **   call delete, or the call may be automatic during detach.   **
 **--------------------------------------------------------------*/

/* String to give the metadata signature */
static char const msg_preamble_signature[] = "PhdcQoSSignature";

/* Private functions for use within this file only */
static void usb_class_phdc_free_qos_metadata_list(usb_phdc_class_struct_t *phdc_intf);
static bool usb_class_phdc_pipe_transfers_pending(usb_pipe_handle pipe_handle);
static bool usb_class_phdc_interface_transfers_pending(usb_phdc_class_struct_t *phdc_intf);
static void usb_class_phdc_send_data_callback(void* tr_ptr, void* call_param_ptr, uint8_t * data_ptr, uint32_t data_size, usb_status usb_sts);
static void usb_class_phdc_send_control_callback(void* tr_ptr, void* callbackParam, uint8_t * bBuff, uint32_t length, usb_status usb_sts);
static void usb_class_phdc_recv_data_callback(void* tr_ptr, void* param, uint8_t* buff_ptr, uint32_t buff_size, usb_status usb_sts);
static usb_phdc_desc_qos_metadata_list_t*
usb_class_phdc_search_endpoint_type(usb_phdc_desc_qos_metadata_list_t* phdcQosMetaList_ptr, uint8_t ep_type, uint8_t ep_dir);
/* Private macro definitions  */
#define   USB_PHDC_LATECNY_RELIABILITY_MASK   0x3F

/* PUBLIC FUNCTION*--------------------------------------------------------
 * 
 * Function Name  : usb_class_phdc_init
 * Returned Value : none
 * Comments       :
 *     Initialize phdc-class interface info struct.
 * 
 *END*--------------------------------------------------------------------*/
usb_status usb_class_phdc_init
(
/* [IN]  the device handle related to the class driver */
usb_device_instance_handle dev_handle,
/* [IN]  the interface handle related to the class driver */
usb_interface_descriptor_handle intf_handle,
/* [OUT] printer call struct pointer */
usb_class_handle* class_handle_ptr
)
{ /* Body */
    usb_phdc_class_struct_t* phdc_class = NULL;
    usb_device_interface_struct_t* pDeviceIntf = NULL;
    endpoint_descriptor_t* ep_desc = NULL;
    uint8_t ep_num;
    usb_status status = USB_OK;
    pipe_init_struct_t pipe_init;
    usb_phdc_class_type_t *phdcClassDesc = NULL;
    descriptor_union_t ptr1;
    uint16_t length;

    phdc_class = (usb_phdc_class_struct_t*)OS_Mem_alloc_zero(sizeof(usb_phdc_class_struct_t));
    if (phdc_class == NULL)
    {
        USB_PRINTF("usb_class_phdc_init fail on memory allocation\n");
        return USBERR_ERROR;
    }

    phdc_class->dev_handle = dev_handle;
    phdc_class->intf_handle = intf_handle;
    phdc_class->host_handle = usb_host_dev_mng_get_host(phdc_class->dev_handle);

    pDeviceIntf = (usb_device_interface_struct_t*)intf_handle;
    phdc_class->mutex = OS_Mutex_create();

    for (ep_num = 0; ep_num < pDeviceIntf->ep_count; ep_num++)
    {
        ep_desc = pDeviceIntf->ep[ep_num].lpEndpointDesc;
        /* init interrupt in pipe */
        if ((((ep_desc->bEndpointAddress & 0x80u)>> 7) == 0x01) && ((ep_desc->bmAttributes & EP_TYPE_MASK) == IRRPT_ENDPOINT))
        {
            pipe_init.endpoint_number = (ep_desc->bEndpointAddress & ENDPOINT_MASK);
            pipe_init.direction = USB_RECV;
            pipe_init.pipetype = USB_INTERRUPT_PIPE;
            pipe_init.max_packet_size = (uint16_t)(USB_SHORT_UNALIGNED_LE_TO_HOST(ep_desc->wMaxPacketSize) & PACKET_SIZE_MASK);
            pipe_init.interval = ep_desc->iInterval;
            pipe_init.flags = 0;
            pipe_init.dev_instance = phdc_class->dev_handle;
            pipe_init.nak_count = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
            status = usb_host_open_pipe(phdc_class->host_handle, &phdc_class->int_in_pipe, &pipe_init);
            if (status != USB_OK)
            {
                USB_PRINTF("usb_class_phdc_init fail to open in pipe\n");
                *class_handle_ptr = (usb_class_handle)phdc_class;
                return USBERR_ERROR;
            }
        }
        /* init bulk in pipe */
        if ((((ep_desc->bEndpointAddress & 0x80u)>> 7) == 0x01) && ((ep_desc->bmAttributes & EP_TYPE_MASK) == BULK_ENDPOINT))
        {
            pipe_init.endpoint_number  = (ep_desc->bEndpointAddress & ENDPOINT_MASK);
            pipe_init.direction        = USB_RECV;
            pipe_init.pipetype         = USB_BULK_PIPE;
            pipe_init.max_packet_size  = (uint16_t)(USB_SHORT_UNALIGNED_LE_TO_HOST(ep_desc->wMaxPacketSize) & PACKET_SIZE_MASK);
            pipe_init.interval         = ep_desc->iInterval;
            pipe_init.flags            = 0;
            pipe_init.dev_instance     = phdc_class->dev_handle;
            pipe_init.nak_count        = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
            status = usb_host_open_pipe(phdc_class->host_handle, &phdc_class->bulk_in_pipe, &pipe_init);
            if (status != USB_OK)
            {
                USB_PRINTF("usb_class_phdc_init fail to open in pipe\n");
                *class_handle_ptr = (usb_class_handle)phdc_class;
                return USBERR_ERROR;
            }
        }
        /* init bulk in pipe */
        if ((((ep_desc->bEndpointAddress & 0x80u)>> 7) == 0x00) && ((ep_desc->bmAttributes & EP_TYPE_MASK) == BULK_ENDPOINT))
        {
            pipe_init.endpoint_number  = (ep_desc->bEndpointAddress & ENDPOINT_MASK);
            pipe_init.direction        = USB_SEND;
            pipe_init.pipetype         = USB_BULK_PIPE;
            pipe_init.max_packet_size  = (uint16_t)(USB_SHORT_UNALIGNED_LE_TO_HOST(ep_desc->wMaxPacketSize) & PACKET_SIZE_MASK);
            pipe_init.interval         = ep_desc->iInterval;
            pipe_init.flags            = 0;
            pipe_init.dev_instance     = phdc_class->dev_handle;
            pipe_init.nak_count        = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
            status = usb_host_open_pipe(phdc_class->host_handle, &phdc_class->bulk_out_pipe, &pipe_init);
            if (status != USB_OK)
            {
                USB_PRINTF("usb_class_phdc_init fail to open in pipe\n");
                *class_handle_ptr = (usb_class_handle)phdc_class;
                return USBERR_ERROR;
            }
        }
    }

    ptr1.pntr = (void*)pDeviceIntf->interfaceEx;
    length = 0;

    /* Get function extension descriptor */
    do
    {
        if (PHDC_11073PHD_FUNCTION_DESCRIPTOR == ptr1.cfig->bDescriptorType)
        {
            phdc_class->fcn_ext_desc = (usb_phdc_desc_fcn_ext_t *)(ptr1.pntr);
            break;
        }
        length += ptr1.cfig->bLength;
        ptr1.word += ptr1.cfig->bLength;
    }while (length < pDeviceIntf->interfaceExlength);

    if ((phdc_class->fcn_ext_desc == NULL) ||
    (phdc_class->fcn_ext_desc->blength != (4 + 2 * phdc_class->fcn_ext_desc->bnumdevspecs))
    )
    {
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_class_phdc_init, error class init. function extension descriptor failed validity check");
#endif  
        return USBERR_ERROR;
    }

    ptr1.pntr = (void*)pDeviceIntf->interfaceEx;
    length = 0;

    /* Get function extension descriptor */
    do
    {
        if (USB_PHDC_CLASSFUNCTION_DESCRIPTOR == ptr1.cfig->bDescriptorType)
        {
            phdcClassDesc = (usb_phdc_class_type_t *)(ptr1.pntr);
            break;
        }
        length += ptr1.cfig->bLength;
        ptr1.word += ptr1.cfig->bLength;
    }while (length < pDeviceIntf->interfaceExlength);

    if (phdcClassDesc == NULL)
    {
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_class_phdc_init, error class init at class descriptor initialization");
#endif  
        return USBERR_ERROR;
    }

    if (phdcClassDesc->bmcapability == 0x01)
    {
        phdc_class->preamble_capability = TRUE;
    }
    else
    {
        phdc_class->preamble_capability = FALSE;
    }

    phdc_class->phdc_data_code = phdcClassDesc->bphdcdatacode;

    /* Initialize the QoSMetadata List of descriptors */
    {
        uint8_t endpointCount = 0;
        usb_phdc_desc_qos_metadata_list_t *qosMetadata = NULL;
        usb_phdc_desc_qos_metadata_list_t *qosMetadataListTail = NULL;
        bool noMoreEndpoints = FALSE;
        length += ptr1.cfig->bLength;
        ptr1.word += ptr1.cfig->bLength;
        length += ptr1.cfig->bLength;
        ptr1.word += ptr1.cfig->bLength;

        do
        {
            /* Allocate memory for the structure member */
            qosMetadata = (usb_phdc_desc_qos_metadata_list_t *)OS_Mem_alloc_uncached_zero(sizeof(usb_phdc_desc_qos_metadata_list_t));
            if (qosMetadata == NULL)
            {
#ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_class_phdc_init, memory allocation failed to build the qos metdata descriptor list");
#endif  
                return USBERR_ERROR;
            }
            /* Initialize the fields in the current list member */
            qosMetadata->pnext = NULL;
            if (USB_ENDPOINT_DESCRIPTOR == ptr1.cfig->bDescriptorType)
            {
                qosMetadata->pendpointdesc = (usb_phdc_desc_endpoint_t*)(ptr1.pntr);
            }
            length += ptr1.cfig->bLength;
            ptr1.word += ptr1.cfig->bLength;
            if (USB_PHDC_QOS_DESCRIPTOR == ptr1.cfig->bDescriptorType)
            {
                qosMetadata->pqosdesc = (usb_phdc_desc_qos_t*)(ptr1.pntr);
            }

            length += ptr1.cfig->bLength;
            ptr1.word += ptr1.cfig->bLength;
            if ((qosMetadata->pendpointdesc == NULL) || (qosMetadata->pqosdesc == NULL))
            {
                /* deallocate the memory for the qosMetadata pointer */
                OS_Mem_free(qosMetadata);

                /* PHDC is expecting minimum 2 endpoint descriptors (Bulk In and Bulk Out) */
                if (endpointCount < 2)
                {
#ifdef _HOST_DEBUG_
                    DEBUG_LOG_TRACE("usb_class_phdc_init, Less than 2 endpoint (or QoS) descriptors received");
#endif  
                    return USBERR_ERROR;
                }
                /* End the loop */
                noMoreEndpoints = TRUE;
            }

            /* First element in the list?   */
            if (endpointCount == 0)
            {
                /* Save the reference in the interface struct */
                phdc_class->qos_metadata_list = qosMetadata;
                qosMetadataListTail = phdc_class->qos_metadata_list; /* Initialize the tail, pointing to the only element in the list */
            }
            else
            {
                if (!noMoreEndpoints)
                {
                    /* Link it on the last element in the list */
                    qosMetadataListTail->pnext = qosMetadata;
                    /* Increment the tail */
                    qosMetadataListTail = qosMetadataListTail->pnext;
                }
            }

            if (!noMoreEndpoints)
            {
                /* Check the Qos descriptor fields */
                if (qosMetadata->pqosdesc != NULL)
                {
                    if (qosMetadata->pqosdesc->bqosencodingversion != 0x01)
                    {
                        /* The encoding of the descriptor latency reliability is not supported, Ignore this descriptor */
                        qosMetadata->pqosdesc = NULL;
                    }
                    else
                    {
                        if (qosMetadata->pendpointdesc != NULL)
                        {
                            switch (qosMetadata->pendpointdesc->bmattributes & 0x03)
                            {
                                case 0x02: /* Bulk endpoint */
                                if (qosMetadata->pqosdesc->bmlatencyreliability & 0x01)
                                {
                                    qosMetadata->pqosdesc = NULL; /* Ignore this descriptor */
                                }
                                else
                                {
                                    if (phdc_class->preamble_capability == FALSE)
                                    {
                                        /* This interface does not support metadata. Only one of the b1-b7 bits can be set */
                                        if (qosMetadata->pqosdesc->bmlatencyreliability & (uint8_t)(qosMetadata->pqosdesc->bmlatencyreliability - 1))
                                        {
                                            qosMetadata->pqosdesc = NULL; /* Ignore this descriptor */
                                        }
                                    }
                                }
                                break;
                                case 0x03: /* Interrupt endpoint */
                                if (qosMetadata->pqosdesc->bmlatencyreliability != 0x01) /* for this endpoint only the b0 should be set in the QoS bin.*/
                                {
                                    qosMetadata->pqosdesc = NULL; /* Ignore this descriptor */
                                }
                                break;
                                default: /* The endpoint is not supported */
                                qosMetadata->pqosdesc = NULL; /* Ignore this descriptor */
                                break;
                            }
                        }
                    }
                }
                if (PHDC_METADATA_DESCRIPTOR == ptr1.cfig->bDescriptorType)
                {
                    qosMetadata->pmetadatadesc = (usb_phdc_desc_metadata_t *)(ptr1.pntr);
                    length += ptr1.cfig->bLength;
                    ptr1.word += ptr1.cfig->bLength;
                }
                endpointCount++;
            } /* if(!noMoreEndpoints) */
        }while (!noMoreEndpoints);
    }/* End block: Initialize the QoSMetadata List of descriptors */

    *class_handle_ptr = (usb_class_handle)phdc_class;
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_phdc_init, Init SUCCESSFUL");
#endif 

    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 * 
 * Function Name  : usb_phdc_send_data
 * Returned Value : error code
 * Comments       :
 *     Send data to a phdc device through a bulk-out pipe.
 * 
 *END*--------------------------------------------------------------------*/
usb_status usb_class_phdc_send_data
(
/* [IN] phdc specific generic parameter structure pointer */
usb_phdc_param_t* call_param_ptr
)
{ /* Body */
    usb_phdc_class_struct_t* phdc_class;
    tr_struct_t* tr_ptr;
    usb_status status;
    usb_phdc_desc_qos_metadata_list_t* qosListParser = NULL;
    uint8_t numTransfers;
    uint8_t latencyReliability;

    if (NULL == call_param_ptr)
    {
        status = USBERR_ERROR;
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_phdc_send_data, NULL pointer parameter");
#endif
        return status;
    }

    if ((NULL == call_param_ptr->class_ptr) || (NULL == call_param_ptr->buff_ptr) )
    {
        status = USBERR_ERROR;
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_phdc_send_data, NULL pointer parameter");
#endif
        return status;
    }

    if (0 == call_param_ptr->buff_size)
    {
        status = USBERR_ERROR;
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_phdc_send_data, data size is zero");
#endif
        return status;
    }

    phdc_class = (usb_phdc_class_struct_t*)call_param_ptr->class_ptr;
    /* check for pending transfers */
    if (TRUE == phdc_class->set_clear_request_pending)
    {
        status = USBERR_TRANSFER_IN_PROGRESS;
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_phdc_send_data, control transfer in progress");
#endif
        return status;
    }

    if ((phdc_class->preamble_capability) && (phdc_class->device_feature_set))
    {
        /* meta-data and QOS checking */
        if (TRUE == call_param_ptr->metadata)
        {
            latencyReliability = ((usb_phdc_metadat_prreamble_t *)call_param_ptr->buff_ptr)->bm_latency_reliability;
            if (((latencyReliability & latencyReliability) - 1) || (0 == latencyReliability) || (latencyReliability & 0xC0))
            {
                status = USBERR_INVALID_BMREQ_TYPE;
#ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_phdc_send_data, invalid LatencyReliability");
#endif
                return status;
            }

            /* Check if 'qos' bin is supported by the Bulk OUT endpoint's PHDC QoS Descriptor */
            /* Search in the usb_phdc_desc_qos_metadata_list_t the endpoint number corresponding to the Bulk OUT */
            qosListParser = usb_class_phdc_search_endpoint_type(phdc_class -> qos_metadata_list, ENDPOINT_BULK, ENDPOINT_DIR_OUT);
            /* Compare the qos bin with 'bmLatencyReliability' field from the corresponding QoS Descriptor */
            if (qosListParser == NULL)
            {
                status = USBERR_ERROR;
#ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_phdc_send_data, the required qos is not supported by Bulk OUT pipe");
#endif
                return status;
            }

            /* Check if the requested QoS is supported */
            if (0 == (qosListParser -> pqosdesc -> bmlatencyreliability & latencyReliability))
            {
                status = USBERR_ERROR;
#ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_phdc_send_data, the required qos is not supported by Bulk OUT pipe");
#endif
                return status;
            }

            numTransfers = ((usb_phdc_metadat_prreamble_t *)call_param_ptr->buff_ptr)->b_num_transfers;
            if (0 == numTransfers)
            {
                status = USBERR_ERROR;
#ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_phdc_send_data, numTransfers must be greater than zero");
#endif
                return status;
            }
            phdc_class->num_transf_bulk_out = numTransfers;
        }
        else /* if not a meta-data send request */
        {
            if (0 == phdc_class->num_transf_bulk_out )
            {
                status = USBERR_ERROR;
#ifdef _HOST_DEBUG_
                DEBUG_LOG_TRACE("usb_phdc_send_data, meta-data expected");
#endif
                return status;
            }
        }
    }

    if (phdc_class->dev_handle == NULL)
    {
        return USBERR_ERROR;
    }

    if (usb_host_get_tr(phdc_class->host_handle, usb_class_phdc_send_data_callback, call_param_ptr, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("error to get tr\n");
        return USBERR_ERROR;
    }

    tr_ptr->tx_buffer = call_param_ptr->buff_ptr;
    tr_ptr->tx_length = call_param_ptr->buff_size;
    call_param_ptr->tr_index = tr_ptr->tr_index;
    call_param_ptr->tr_pipe_handle = phdc_class->bulk_out_pipe;

    status = usb_host_send_data(phdc_class->host_handle, phdc_class->bulk_out_pipe, tr_ptr);
    if (status != USB_OK)
    {
        USB_PRINTF("\nError in usb_phdc_send_data: %x", (unsigned int)status);
        usb_host_release_tr(phdc_class->host_handle, tr_ptr);
        return USBERR_ERROR;
    }

    /* decrement the numTransfers for bulkOut pipe */
    if ((phdc_class->num_transf_bulk_out != 0) && (FALSE == call_param_ptr->metadata))
    {
        phdc_class->num_transf_bulk_out--;
    }

    return USB_OK;
} /* Endbody */

/* PUBLIC FUNCTION*--------------------------------------------------------
 * 
 * Function Name  : usb_class_phdc_set_callbacks
 * Returned Value : none
 * Comments       :
 *     Initializes the application callbacks for the current PHDC interface
 * 
 *END*--------------------------------------------------------------------*/
usb_status usb_class_phdc_set_callbacks
(
/* [IN]  the class driver handle */
usb_class_handle handle,
/* [IN] phdc application-registered send Callback */
phdc_callback sendCallback,
/* [IN] phdc application-registered receive Callback */
phdc_callback recvCallback,
/* [IN] phdc application-registered control Callback */
phdc_callback ctrlCallback
)
{
    usb_phdc_class_struct_t* phdc_class = (usb_phdc_class_struct_t*)handle;
    usb_status returnStatus = USBERR_NO_INTERFACE;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_phdc_set_callbacks");
#endif

    if (phdc_class == NULL)
    {
        USB_PRINTF("usb_class_phdc_set_callbacks fail\n");
        return USBERR_ERROR;
    }

    if (!usb_class_phdc_interface_transfers_pending(phdc_class))
    {
        /* No transfers are pending in the interface. Set the callbacks */
        phdc_class->send_callback = sendCallback;
        phdc_class->recv_callback = recvCallback;
        phdc_class->ctrl_callback = ctrlCallback;
        returnStatus = USB_OK;
    }

    return returnStatus;
}

/* PUBLIC FUNCTION*--------------------------------------------------------
 * 
 * Function Name  : usb_class_phdc_uninit
 * Returned Value : none
 * Comments       :
 *     Uninitialize phdc-class interface info struct.
 * 
 *END*--------------------------------------------------------------------*/
usb_status usb_class_phdc_deinit
(
/* [IN]  the class driver handle */
usb_class_handle handle
)
{
    usb_phdc_class_struct_t* phdc_class = (usb_phdc_class_struct_t*)handle;
    usb_status status = USB_OK;

    if (phdc_class == NULL)
    {
        USB_PRINTF("usb_class_phdc_deinit fail\n");
        return USBERR_ERROR;
    }

    /* Close Bulk In Pipe */
    if (phdc_class->bulk_in_pipe != NULL)
    {
        status = usb_host_close_pipe(phdc_class->host_handle, phdc_class->bulk_in_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_phdc_deinit to close pipe\n");
            return USBERR_ERROR;
        }
    }
    /* Close Bulk OUT Pipe */
    if (phdc_class->bulk_out_pipe != NULL)
    {
        status = usb_host_close_pipe(phdc_class->host_handle, phdc_class->bulk_out_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_phdc_deinit to close pipe\n");
            return USBERR_ERROR;
        }
    }

    /* Close Interrupt In Pipe */
    if (phdc_class->int_in_pipe != NULL)
    {
        status = usb_host_close_pipe(phdc_class->host_handle, phdc_class->int_in_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_phdc_deinit to close pipe\n");
            return USBERR_ERROR;
        }
    }

    if (phdc_class->mutex != NULL)
    {
        OS_Mutex_destroy(phdc_class->mutex);
    }

    usb_class_phdc_free_qos_metadata_list(phdc_class);

    OS_Mem_free(handle);
    return status;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_phdc_pre_deinit
 * Returned Value : None
 * Comments       :
 *     This function is called by common class to initialize the class driver. It
 *     is called in response to a select interface call by application
 *
 *END*--------------------------------------------------------------------*/
usb_status usb_class_phdc_pre_deinit
(
/* [IN]  the class driver handle */
usb_class_handle handle
)
{
    usb_phdc_class_struct_t* phdc_class = (usb_phdc_class_struct_t*)handle;
    usb_status status = USB_OK;

    if (phdc_class == NULL)
    {
        USB_PRINTF("_usb_host_cancel_call_interface fail\n");
        return USBERR_ERROR;
    }

    if (phdc_class->bulk_in_pipe != NULL)
    {
        status = usb_host_cancel(phdc_class->host_handle, phdc_class->bulk_in_pipe, NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in _usb_host_cancel_call_interface to close pipe\n");
        }
    }
    if (phdc_class->bulk_out_pipe != NULL)
    {
        status = usb_host_cancel(phdc_class->host_handle, phdc_class->bulk_out_pipe, NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in _usb_host_cancel_call_interface to close pipe\n");
        }
    }

    if (phdc_class->int_in_pipe != NULL)
    {
        status = usb_host_cancel(phdc_class->host_handle, phdc_class->int_in_pipe, NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in _usb_host_cancel_call_interface to close pipe\n");
        }
    }

    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 * 
 * Function Name  : usb_class_phdc_recv_data
 * Returned Value : none
 * Comments       :
 *     Receive printer data from either a bulk-in or an interrupt pipe.
 * 
 *END*--------------------------------------------------------------------*/
usb_status usb_class_phdc_recv_data
(
usb_phdc_param_t* call_param_ptr
)
{ /* Body */
    usb_phdc_class_struct_t* phdc_class;
    tr_struct_t* tr_ptr;
    usb_status status;
    usb_pipe_handle pipe;
    usb_phdc_desc_qos_metadata_list_t* phdcQosMetadataListParser = NULL;

    if (NULL == call_param_ptr)
    {
        status = USBERR_ERROR;
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_phdc_send_data, NULL pointer parameter");
#endif
        return status;
    }

    if ((NULL == call_param_ptr->class_ptr) || (NULL == call_param_ptr->buff_ptr) )
    {
        status = USBERR_ERROR;
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_phdc_send_data, NULL pointer parameter");
#endif
        return status;
    }

    if (0 == call_param_ptr->buff_size)
    {
        status = USBERR_ERROR;
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_phdc_send_data, data size is zero");
#endif
        return status;
    }

    phdc_class = (usb_phdc_class_struct_t*)call_param_ptr->class_ptr;

    /* Checks if QoS 'points' to the interrupt pipe and this pipe exists OR 'qos' contains valid value */
    if (call_param_ptr ->qos & 0x01)
    {
        if ((call_param_ptr->qos & 0xFE) || (NULL == phdc_class->int_in_pipe))
        {
#ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_phdc_recv_data, ERROR: input parameter 'qos' wrong");
#endif
            return USBERR_ERROR;
        }
    }

    /* Get the pipe handle used for receive data (parameter passed in _usb_host_recv_data()), depending on the qos bin */
    pipe = (call_param_ptr->qos & 0x01) ? (phdc_class->int_in_pipe) : (phdc_class->bulk_in_pipe);

    /* Checks if pipe selection is Bulk IN */
    if (pipe == phdc_class->bulk_in_pipe)
    {
        /* Check if 'qos' bin is supported by the Bulk IN endpoint's PHDC QoS Descriptor */
        /* Search in the usb_phdc_desc_qos_metadata_list_t the endpoint number corresponding to the Bulk IN */
        phdcQosMetadataListParser = usb_class_phdc_search_endpoint_type(phdc_class ->qos_metadata_list, ENDPOINT_BULK, ENDPOINT_DIR_IN);

        /* Compare the qos bin with 'bmLatencyReliability' field from the corresponding QoS Descriptor */
        if ((phdcQosMetadataListParser == NULL) || (!(call_param_ptr->qos & phdcQosMetadataListParser->pqosdesc->bmlatencyreliability)))
        {
#ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_phdc_recv_data, ERROR: required qos is not supported by Bulk IN pipe");
#endif  
            return USBERR_ERROR;
        }

        /* Checks if any SET_FEATURE/CLEAR_FEATURE class command is pending; if it is, refuses the current transaction */
        if (TRUE == phdc_class->set_clear_request_pending)
        {
#ifdef _HOST_DEBUG_
            DEBUG_LOG_TRACE("usb_phdc_recv_data, ERROR: transfer in progress");
#endif
            return USBERR_TRANSFER_IN_PROGRESS;
        }
    }

    if (usb_host_get_tr(phdc_class->host_handle, usb_class_phdc_recv_data_callback, call_param_ptr, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("error to get tr\n");
        return USBERR_ERROR;
    }
    tr_ptr->rx_buffer = call_param_ptr->buff_ptr;
    tr_ptr->rx_length = call_param_ptr->buff_size;

    /* Save the transfer index and pipe handle */
    call_param_ptr->tr_index = tr_ptr->tr_index;
    call_param_ptr->tr_pipe_handle = pipe;

    /* Require Host API to post transaction */
    status = usb_host_recv_data(phdc_class->host_handle, pipe, tr_ptr);

    if (status != USB_OK)
    {
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("\nError in usb_phdc_recv_data");
#endif
        usb_host_release_tr(phdc_class->host_handle, tr_ptr);
        return USBERR_ERROR;
    }
    return status;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 * 
 * Function Name  : phdc_receive_data_callback
 * Returned Value : none
 * Comments       :
 *     Internal PHDC callback function when a posted receive transfer has been finished.
 * 
 *END*--------------------------------------------------------------------*/
static void usb_class_phdc_recv_data_callback
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
{ /* Body */
    usb_phdc_param_t* callback_param_ptr = (usb_phdc_param_t*)param;
    usb_phdc_class_struct_t* phdc_class = (usb_phdc_class_struct_t*)(callback_param_ptr->class_ptr);
    dev_instance_t* dev = (dev_instance_t*)(phdc_class->dev_handle);
    pipe_struct_t* pipe_desc_ptr = (pipe_struct_t*) phdc_class->bulk_in_pipe;
    usb_phdc_metadat_prreamble_t* pMetaDataPreamble = NULL;
    uint8_t metadata_packet;
    usb_status status;
    usb_pipe_handle pipe = callback_param_ptr->tr_pipe_handle;

    if (usb_host_release_tr(phdc_class->host_handle, (tr_struct_t*)tr_ptr) != USB_OK)
    {
        USB_PRINTF("_usb_host_release_tr failed\n");
    }
    if (usb_sts != USB_STATUS_IDLE)
    {
        /* ERROR: error returned USB status  */
        callback_param_ptr ->usb_phdc_status = USB_PHDC_ERR;
    }
    else
    {
        if (pipe == phdc_class->int_in_pipe)
        {
            /* Pipe is Interrupt IN */
            /* Packet received is assumed to be data (no metadata preamble check) */
            callback_param_ptr ->usb_phdc_status = USB_PHDC_RX_OK;
        }
        else
        {
            /* Pipe is Bulk IN */
            /* Checks for the preamble capability feature and activation */
            if ((phdc_class->preamble_capability == TRUE) && (phdc_class->device_feature_set == TRUE))
            {
                /* Preamble feature supported and also enabled */
                /* Checks if packet expected is metadata preamble or not */
                if (phdc_class->num_transf_bulk_in)
                {
                    /* Packet expected: message data */
                    /* Packet received is assumed to be data (no metadata preamble check) */
                    callback_param_ptr->usb_phdc_status = USB_PHDC_RX_OK;
                    /* Decrements the number of data transfer left until a new metadata is expected */
                    phdc_class->num_transf_bulk_in--;
                }
                else
                {
                    /* Packet expected: message data */
                    /* 
                     Compare the first 16 bytes of the received data with the preamble signature 
                     in order to decide if metadata preamble has been received; if metadata_pachet = 0 
                     the incoming packet is a metadata.                    
                     */
                    metadata_packet = (uint8_t) strncmp((const char *)buff_ptr, msg_preamble_signature, METADATA_PREAMBLE_SIGNATURE_SIZE);
                    /* Checks the incoming packet */
                    if (!metadata_packet)
                    {
                        /* Metadata preamble received */
                        /* Metadata expected and also received */
                        /* Parse the buffer as it would be a metadata preamble */
                        /* Checks for correctness of the rest of fields in preamble metadata (bNumTransfers, bQosEncodingVersion and bmLatencyReliability) */
                        pMetaDataPreamble = (usb_phdc_metadat_prreamble_t*)buff_ptr;
                        if (
                        (!(pMetaDataPreamble->b_num_transfers)) || /* bNumTrensfer should not be 0 */
                        (pMetaDataPreamble->b_qos_encoding_version != 0x01) || /* if 'bQoSEncodingVersion' isn't 0x01 it would be considered that 'bmLatencyReliability' is not valid */
                        (pMetaDataPreamble->bm_latency_reliability & 0x01) || /* 'bmLatencyReliability' bitmap shouldn't contains a '1' in the bit 0 position */
                        (!(pMetaDataPreamble->bm_latency_reliability)) || /* 'bmLatencyReliability' should not be 0 */
                        ((pMetaDataPreamble->bm_latency_reliability & pMetaDataPreamble ->bm_latency_reliability) - 1) /* 'bmLatencyReliability' bitmap shall contains only one bit set */
                        )
                        {
                            /* ERROR: metadata preamble expected and received BUT invalid 'bNumTransfer' or 'bmLatencyReliability' values */
                            callback_param_ptr ->usb_phdc_status = USB_PHDC_RX_ERR_METADATA_EXPECTED;

                            /* Performs a SetFeature(Endpoint_Halt) command  followed by a ClearFeature(Endpoint_Halt) */
                            /* Prototype:  usb_status  _usb_host_ch9_set/clear_feature(dev_handle, req_type, intf_endpt,feature) where: */
                            /* REQ_TYPE_ENDPOINT = 0x02, ENDPOINT_HALT = 0x01 defined in "host_ch9.h" */
                            status = _usb_host_ch9_set_feature(dev, REQ_TYPE_ENDPOINT, (uint8_t)(pipe_desc_ptr->endpoint_number | pipe_desc_ptr->direction), ENDPOINT_HALT);

                            if (USB_OK != status)
                            {
                                callback_param_ptr ->usb_phdc_status = USB_PHDC_ERR_ENDP_CLEAR_STALL;
                            }
                            else
                            {
                                status = _usb_host_ch9_clear_feature(dev, REQ_TYPE_ENDPOINT, (uint8_t)(pipe_desc_ptr->endpoint_number | pipe_desc_ptr->direction), ENDPOINT_HALT);
                                if (USB_OK != status)
                                {
                                    callback_param_ptr->usb_phdc_status = USB_PHDC_ERR_ENDP_CLEAR_STALL;
                                }
                                else
                                {
                                    /* Reset the 'numTransfBulkIn' parameter in order to expect a new metadata preamble */
                                    phdc_class->num_transf_bulk_in = 0;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if ((phdc_class->preamble_capability == FALSE) && (phdc_class->device_feature_set == FALSE))
                {
                    /* Neither preamble capability supported nor enabled */
                    /* Packet received is assumed to be data (no metadata preamble check) */
                    callback_param_ptr->usb_phdc_status = USB_PHDC_RX_OK;
                }
            }
        }
    }
    /* Populates the 'usb_status and buff_size' in the usb_phdc_param_t */
    callback_param_ptr ->status = usb_sts;
    callback_param_ptr ->buff_size = buff_size;

    /* Call the application callback function unless the callback pointer is NULL */
    if (NULL != phdc_class->recv_callback)
    {
        phdc_class->recv_callback(callback_param_ptr);
    }
} /* EndBody */

/* PUBLIC FUNCTION*--------------------------------------------------------
 * 
 * Function Name  : usb_class_phdc_search_endpoint_type
 * Returned Value : usb_status
 * Comments: This function search in the usb_phdc_desc_qos_metadata_list_t the endpoint
 *           corresponding to the type given as argument 
 * 
 *END*--------------------------------------------------------------------*/
static usb_phdc_desc_qos_metadata_list_t* usb_class_phdc_search_endpoint_type
(
usb_phdc_desc_qos_metadata_list_t* phdcQosMetaList_ptr,
uint8_t ep_type,
uint8_t ep_dir
)
{ /* Body */
    /* used to search a specific QoS descriptor in the linked list of Endpoint-QoS-Metadata descriptors */
    usb_phdc_desc_qos_metadata_list_t* phdcQosMetadataListParser = phdcQosMetaList_ptr;

    while (phdcQosMetadataListParser -> pnext != NULL)
    {
        /* Check if the endpoint is Bulk IN */
        if (
        (((uint8_t)(phdcQosMetadataListParser ->pendpointdesc ->bmattributes) & ENDPOINT_TRANSFER_TYPE) == ep_type) && /* Endpoint type */
        (((uint8_t)(phdcQosMetadataListParser ->pendpointdesc ->bendpointaddress) & ENDPOINT_DIRECTION) == ep_dir) /* Endpoint direction */
        )
        {
            return phdcQosMetadataListParser;
        }

        /* Points to the next element in the list */
        phdcQosMetadataListParser = phdcQosMetadataListParser -> pnext;
    }
    return NULL;
} /* Endbody */

/* PUBLIC FUNCTION*--------------------------------------------------------
 * 
 * Function Name  : usb_class_phdc_send_control_request
 * Returned Value : usb_status
 * Comments: This function implements class specific request: PHDC_GET_STATUS_BREQ, PHDC_SET_FEATURE_BREQ and      :
 * PHDC_CLEAR_FEATURE_BREQ    
 * 
 *END*--------------------------------------------------------------------*/
usb_status usb_class_phdc_send_control_request
(
usb_phdc_param_t* call_param_ptr
)
{ /* Body */
    usb_status                      status = USBERR_NO_INTERFACE;
    usb_phdc_class_struct_t*        phdc_class = NULL;
    usb_pipe_handle                 pipe_handle;
    tr_struct_t*                    tr_ptr; 
    usb_device_interface_struct_t*  pDeviceIntf = NULL;
    interface_descriptor_t*         intf = NULL;
    uint16_t                        targetInterface;
    uint8_t                         reqType; 

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_phdc_send_control_request");
#endif
    /* parameter checking*/
    if(call_param_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    /*make sure that no USB event occurs until the function ends*/

    phdc_class = (usb_phdc_class_struct_t*)call_param_ptr->class_ptr;
    /* make sure that the interface is a valid one*/
    if (phdc_class == NULL)
    {
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_class_phdc_send_control_request, error status");
#endif
        return USBERR_ERROR;
    } /* Endif */

    pDeviceIntf = (usb_device_interface_struct_t*)phdc_class->intf_handle;
    intf = pDeviceIntf->lpinterfaceDesc;
    /* read interface number from the interface descriptor*/
    targetInterface = (uint16_t)(intf->bInterfaceNumber);

    pipe_handle = usb_host_dev_mng_get_control_pipe(phdc_class->dev_handle);

    if (usb_host_get_tr(phdc_class->host_handle, usb_class_phdc_send_control_callback, call_param_ptr, &tr_ptr) != USB_OK)
    {
        USB_PRINTF("error to get tr hid\n");
        return USBERR_ERROR;
    }
    reqType = call_param_ptr->classRequestType;
    switch (reqType)
    {
        case PHDC_GET_STATUS_BREQ:
        /* status bytes are to be received from the device, so, make sure that buff_ptr isn't NULL */
        if (call_param_ptr->buff_ptr == NULL)
        {
            return USBERR_ERROR;
        }
        /* populate the usb_setup_t structure for the status request */
        tr_ptr->setup_packet.bmrequesttype = PHDC_GET_STATUS_BMREQTYPE;
        tr_ptr->setup_packet.brequest = PHDC_GET_STATUS_BREQ;
        *(uint16_t*)&tr_ptr->setup_packet.wvalue[0] = 0;
        *(uint16_t*)&tr_ptr->setup_packet.windex[0] = USB_HOST_TO_LE_SHORT(targetInterface);
        *(uint16_t*)&tr_ptr->setup_packet.wlength[0] = USB_HOST_TO_LE_SHORT(2);
        break;
        case PHDC_SET_FEATURE_BREQ:
        case PHDC_CLEAR_FEATURE_BREQ:
        /* make sure that the device supports metadata message preambule feature */
        if (phdc_class->preamble_capability == 0)
        {
            return USBERR_INVALID_REQ_TYPE;
        }
        /* make sure that no other set/clear feature request is in progress*/
        if (phdc_class->set_clear_request_pending)
        {
            return USBERR_TRANSFER_IN_PROGRESS;
        }
        /* To avoid synchronization issues with preamble */
        /* make sure that no send or receive transactions are pending  */
        if (usb_class_phdc_interface_transfers_pending(phdc_class))
        {
            return USBERR_TRANSFER_IN_PROGRESS;
        }
        /* populate the usb_setup_t structure for the set/clear feature request */
        if (reqType == PHDC_SET_FEATURE_BREQ)
        {
            tr_ptr->setup_packet.bmrequesttype = PHDC_SET_FEATURE_BMREQTYPE;
            tr_ptr->setup_packet.brequest = PHDC_SET_FEATURE_BREQ;
        }
        else
        {
            tr_ptr->setup_packet.bmrequesttype = PHDC_CLEAR_FEATURE_BMREQTYPE;
            tr_ptr->setup_packet.brequest = PHDC_CLEAR_FEATURE_BREQ;
        }
        tr_ptr->setup_packet.wvalue[0] = PHDC_FEATURE_METADATA;
        tr_ptr->setup_packet.wvalue[1] = PHDC_QoS_ENCODING_VERSION;
        *(uint16_t*)&tr_ptr->setup_packet.windex[0] = USB_HOST_TO_LE_SHORT(targetInterface);
        *(uint16_t*)&tr_ptr->setup_packet.wlength[0] = 0;

        /* set the setClearRequestPending flag to indicate that  */
        /* a set/clear feature request is in progress.This flag  */
        /* will be cleared in the phdc send control request callback */
        /* or later in this function if _usb_hostdev_cntrl_request returns error*/
        phdc_class->set_clear_request_pending = TRUE;
        break;
        default:
        return USBERR_INVALID_REQ_TYPE;
    }
    /* Set TR buffer length as required */
    if ((REQ_TYPE_IN & tr_ptr->setup_packet.bmrequesttype) != 0)
    {
        tr_ptr->rx_buffer = call_param_ptr->buff_ptr;
        tr_ptr->rx_length = call_param_ptr->buff_size;
    }
    else
    {
        tr_ptr->tx_buffer = call_param_ptr->buff_ptr;
        tr_ptr->tx_length = call_param_ptr->buff_size;
    }

    status = usb_host_send_setup(phdc_class->host_handle, pipe_handle, tr_ptr);

    if (status != USB_OK)
    {
        if ((reqType == PHDC_SET_FEATURE_BREQ) || (reqType == PHDC_CLEAR_FEATURE_BREQ))
        {
            phdc_class->set_clear_request_pending = FALSE;
        }
    }
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_phdc_send_control_request, SUCCESSFUL");
#endif
    return status;
} /* Endbody */

/* FUNCTION*--------------------------------------------------------
 * 
 * Function Name  : usb_class_phdc_send_data_callback
 * Returned Value : void
 * Comments: this function is called by USB host API
 * 
 *END*--------------------------------------------------------------------*/
static void usb_class_phdc_send_data_callback
(
void* tr_ptr,
void* call_param_ptr,
uint8_t * data_ptr,
uint32_t data_size,
usb_status usb_sts
)
{
    pipe_struct_t* pipe_ptr;
    usb_phdc_class_struct_t* class_interface_handle;
    usb_phdc_param_t* param_ptr = (usb_phdc_param_t*)call_param_ptr;
    usb_status status;
    usb_phdc_class_struct_t* phdc_class = (usb_phdc_class_struct_t*)(param_ptr->class_ptr);

    if (usb_host_release_tr(phdc_class->host_handle, (tr_struct_t*)tr_ptr) != USB_OK)
    {
        USB_PRINTF("_usb_host_release_tr failed\n");
    }

    /* exit if input pointer is NULL */
    if (NULL == call_param_ptr)
    {
        return;
    }

    param_ptr = (usb_phdc_param_t*)call_param_ptr;
    pipe_ptr = (pipe_struct_t*)param_ptr->tr_pipe_handle;
    /* populate the usb_phdc_param_t structure using the data provided by the host API */
    param_ptr->status = usb_sts;
    class_interface_handle = (usb_phdc_class_struct_t*)param_ptr->class_ptr;

    /* verify the USB error code and generate the PHDC specific error code */
    if (USB_OK == usb_sts)
    {
        param_ptr->usb_phdc_status = USB_PHDC_TX_OK;
    }
    else
    {
        param_ptr->usb_phdc_status = USB_PHDC_ERR;
        /* if deivce's endpoint is stalled , try to clear this status */
        if (USBERR_ENDPOINT_STALLED == usb_sts)
        {
            /* if the 'clear feature' operation fails, exit with error code */
            status = _usb_host_ch9_clear_feature
            ( class_interface_handle->dev_handle,
            REQ_TYPE_ENDPOINT,
            pipe_ptr->endpoint_number | REQ_TYPE_OUT,
            ENDPOINT_HALT );
            if (status != USB_OK)
            {
                param_ptr->usb_phdc_status = USB_PHDC_ERR_ENDP_CLEAR_STALL;
            }
        }
    }

    /* verify the callback pointer and if != NULL, launch the application's callback function */
    if (NULL != class_interface_handle->send_callback)
    {
        class_interface_handle->send_callback(param_ptr);
    }
}

/* FUNCTION*--------------------------------------------------------
 * 
 * Function Name  : usb_class_phdc_interface_transfers_pending
 * Returned Value : TRUE  = the provided interface has active transfers
 *                 FALSE = the provided interface does not have active transfers
 * Comments:  this function checks if there are active transfers at the PHDC interface level
 * it checks all the pipes for transfers using the usb_class_phdc_pipe_transfers_pending function
 * WARNING: This function presumes that USB interrupts are disabled
 * 
 *END*--------------------------------------------------------------------*/
static bool usb_class_phdc_interface_transfers_pending
(
usb_phdc_class_struct_t *phdc_intf
)
{
    bool returnValue = FALSE;

    if (phdc_intf != NULL)
    {
        returnValue = usb_class_phdc_pipe_transfers_pending(phdc_intf->control_pipe);
        if (returnValue == FALSE)
        {
            returnValue = usb_class_phdc_pipe_transfers_pending(phdc_intf->bulk_in_pipe);
            if (returnValue == FALSE)
            {
                returnValue = usb_class_phdc_pipe_transfers_pending(phdc_intf->bulk_out_pipe);
                if (returnValue == FALSE)
                {
                    returnValue = usb_class_phdc_pipe_transfers_pending(phdc_intf->int_in_pipe);
                }
            }
        }
    }
    return returnValue;
}

/* FUNCTION*--------------------------------------------------------
 * 
 * Function Name  : usb_class_phdc_pipe_transfers_pending
 * Returned Value : TRUE = the provided pipe has active transfers
 *                  FALSE = the provided pipe does not have active transfers
 * Comments: this function checks if there are active transfers on a pipe
 * 
 *END*--------------------------------------------------------------------*/
static bool usb_class_phdc_pipe_transfers_pending
(
usb_pipe_handle pipe_handle
)
{
    pipe_struct_t* pipe_ptr = (pipe_struct_t*) pipe_handle;
    bool returnValue = FALSE;

    if (pipe_ptr != NULL)
    {
        /* Search the pipe active transfer list */
        tr_struct_t* tr_ptr = pipe_ptr->tr_list_ptr;
        while ((tr_ptr != NULL) && (tr_ptr->tr_index == 0))
        {
            if (pipe_ptr->tr_list_ptr != tr_ptr->next)
            {
                tr_ptr = tr_ptr->next;
            }
            else
            {
                tr_ptr = NULL; /* End of the list reached */
            }
        }
        if (tr_ptr != NULL)
        {
            /* TR_INDEX found to be != 0. This pipe has queued transfers */
            returnValue = TRUE;
        }
    }
    return returnValue;
}

/* FUNCTION*--------------------------------------------------------
 * 
 * Function Name  : usb_class_phdc_free_qos_metadata_list
 * Returned Value : void
 * Comments: this function frees QoS Metadata List
 * 
 *END*--------------------------------------------------------------------*/
static void usb_class_phdc_free_qos_metadata_list
(
usb_phdc_class_struct_t *phdc_intf
)
{
    /* Check for allocated memory inside the interface structure */
    usb_phdc_desc_qos_metadata_list_t *qosMetadataTail = phdc_intf->qos_metadata_list;

    while (qosMetadataTail != NULL)
    {
        /* Ok we got a location to be freed. Save this pointer location */
        usb_phdc_desc_qos_metadata_list_t *qosMetadataLast = qosMetadataTail;
        /* Increment the tail for the next iteration */
        qosMetadataTail = qosMetadataTail->pnext;
        /* Free the saved location */
        OS_Mem_free(qosMetadataLast);
    }

    /* Clear the interface allocated memory */
    OS_Mem_zero (phdc_intf, sizeof(usb_phdc_class_struct_t));
}

/* FUNCTION*--------------------------------------------------------
 * 
 * Function Name  : usb_class_phdc_send_control_callback
 * Returned Value : void
 * Comments: this function will be called when a phdc control request completes
 * 
 *END*--------------------------------------------------------------------*/
static void usb_class_phdc_send_control_callback
(
void* tr_ptr,
void* callbackParam,
uint8_t * bBuff,
uint32_t length,
usb_status usb_sts
)
{
    usb_phdc_class_struct_t* intf_ptr;
    usb_phdc_param_t * call_param_ptr;
    uint8_t reqType;
    uint8_t phdc_status = USB_PHDC_ERR;
    usb_status status;
    usb_phdc_class_struct_t* phdc_class;
    pipe_struct_t* pipeHandle;

    /* parameter checking*/
    if (callbackParam != NULL)
    {
        call_param_ptr = (usb_phdc_param_t *)callbackParam;
        phdc_class = (usb_phdc_class_struct_t*)(call_param_ptr->class_ptr);
        if (usb_host_release_tr(phdc_class->host_handle, (tr_struct_t*)tr_ptr) != USB_OK)
        {
            USB_PRINTF("_usb_host_release_tr failed\n");
        }

        pipeHandle = (pipe_struct_t*)call_param_ptr->tr_pipe_handle;
        /*make sure that no USB event occurs until the function ends*/
        intf_ptr = (usb_phdc_class_struct_t*)call_param_ptr->class_ptr;
        /* make sure that the interface is a valid one*/
        reqType = call_param_ptr->classRequestType;
        if ((reqType == PHDC_SET_FEATURE_BREQ) || (reqType == PHDC_CLEAR_FEATURE_BREQ))
        {
            /*clear the setClearRequestPending to enable another set/clear feature requests*/
            intf_ptr->set_clear_request_pending = FALSE;
        }
        /* update the status of the deviceFeatureSet flag and the phdc status */
        /* if the _usb_hostdev_cntrl_request completes successfully */
        if (usb_sts == USB_OK)
        {
            phdc_status = USB_PHDC_CTRL_OK;
            if (reqType == PHDC_SET_FEATURE_BREQ )
            {
                intf_ptr->device_feature_set = TRUE;
            }
            else if(reqType == PHDC_CLEAR_FEATURE_BREQ )
            {
                intf_ptr->device_feature_set = FALSE;
            }
        }
        /* clear the stall condition if the control endpoint is stalled  */
        else if(usb_sts == USBERR_ENDPOINT_STALLED)
        {
            status = _usb_host_ch9_clear_feature(intf_ptr->dev_handle,
            REQ_TYPE_ENDPOINT, ((pipe_struct_t*)pipeHandle)->endpoint_number , ENDPOINT_HALT);
            if(status != USB_OK)
            {
                phdc_status = USB_PHDC_ERR_ENDP_CLEAR_STALL;
            }
        }
        /* if a user control request callback was installed update the usb_phdc_param_t */
        /* structure and launch the callback*/
        if (*intf_ptr->ctrl_callback != NULL)
        {
            call_param_ptr->usb_phdc_status = phdc_status;
            call_param_ptr->status = usb_sts;
            if (usb_sts == USB_OK)
            {
                call_param_ptr->buff_size = length;
            }
            (*intf_ptr->ctrl_callback)(call_param_ptr);
        }
    }
}
#endif
