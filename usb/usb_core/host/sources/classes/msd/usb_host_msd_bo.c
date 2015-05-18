/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 - 2015 Freescale Semiconductor;
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
* $FileName: usb_host_msd_bo.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   Implementation of mass storage class specification.
*
*END************************************************************************/
#include "usb_host_config.h"
#if USBCFG_HOST_MSC
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_dev_mng.h"
#include "usb_host_ch9.h"
#include "usb_host_common.h"
#include "usb_host_msd_bo.h"
#include "usb_host.h"

/* Private functions */
static void usb_class_mass_call_back_cbw(void* tr_ptr, void* user_parm, uint8_t* buffer, uint32_t length_data_transfered, usb_status status);
static void usb_class_mass_call_back_dphase(void* tr_ptr, void* user_parm, uint8_t* buffer, uint32_t length_data_transfered, usb_status status);
static void usb_class_mass_call_back_csw(void* tr_ptr, void* user_parm, uint8_t* buffer, uint32_t length_data_transfered, usb_status status);
static void usb_class_mass_reset_callback(void* tr_ptr, void* user_parm, uint8_t* buffer, uint32_t length_data_transfered, usb_status status);
static void usb_class_mass_ctrl_callback(void* pipe_handle, void* user_param, uint8_t* buffer, uint32_t size, usb_status status);
static usb_status usb_class_mass_pass_on_usb(usb_mass_class_struct_t* mass_class);
static usb_status usb_class_mass_clear_bulk_pipe_on_usb(usb_mass_class_struct_t* mass_class);
static usb_status usb_class_mass_reset_in_pipe(usb_mass_class_struct_t* mass_class);
static usb_status usb_class_mass_reset_out_pipe(usb_mass_class_struct_t* mass_class);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_init
* Returned Value : None
* Comments       :
*     This function initializes the mass storage class
*
*END*--------------------------------------------------------------------*/

usb_status usb_class_mass_init
   (           
        /* [IN]  the device handle related to the class driver */
        usb_device_instance_handle      dev_handle,
        /* [IN]  the interface handle related to the class driver */
        usb_interface_descriptor_handle intf_handle,
        /* [OUT] printer call struct pointer */
        usb_class_handle*               class_handle_ptr
   )
{ /* Body */
   usb_mass_class_struct_t *       mass_class = NULL;
   usb_device_interface_struct_t*  pDeviceIntf = NULL;
   endpoint_descriptor_t*          ep_desc = NULL;
   uint8_t                         ep_num;
   usb_status                      status = USB_OK;
   pipe_init_struct_t              pipe_init;
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_init");
   #endif

   mass_class = (usb_mass_class_struct_t *)OS_Mem_alloc_zero(sizeof(usb_mass_class_struct_t));
   if (mass_class == NULL)
   {
       USB_PRINTF("usb_class_mass_init fail on memory allocation\n");
       return USBERR_ERROR;
   }
   
   mass_class->dev_handle  = dev_handle;
   mass_class->intf_handle = intf_handle;
   mass_class->host_handle = usb_host_dev_mng_get_host(mass_class->dev_handle);
   
   pDeviceIntf = (usb_device_interface_struct_t*)intf_handle;
   
   for (ep_num = 0; ep_num < pDeviceIntf->ep_count; ep_num++)
   {
       ep_desc = pDeviceIntf->ep[ep_num].lpEndpointDesc;
       if ((ep_desc->bEndpointAddress & IN_ENDPOINT) && ((ep_desc->bmAttributes & EP_TYPE_MASK) == BULK_ENDPOINT))
       {
           pipe_init.endpoint_number  = (ep_desc->bEndpointAddress & ENDPOINT_MASK);
           pipe_init.direction        = USB_RECV;
           pipe_init.pipetype         = USB_BULK_PIPE;
           pipe_init.max_packet_size  = (uint16_t)(USB_SHORT_UNALIGNED_LE_TO_HOST(ep_desc->wMaxPacketSize) & PACKET_SIZE_MASK);
           pipe_init.interval         = ep_desc->iInterval;
           pipe_init.flags            = 0;
           pipe_init.dev_instance     = mass_class->dev_handle;
           pipe_init.nak_count        = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
           status = usb_host_open_pipe(mass_class->host_handle, &mass_class->bulk_in_pipe, &pipe_init);
           if (status != USB_OK)
           {
               USB_PRINTF("usb_class_msd_init fail to open in pipe\n");
               *class_handle_ptr = (usb_class_handle)mass_class;
               return USBERR_ERROR;
           }
       }
       else if ((!(ep_desc->bEndpointAddress & OUT_ENDPOINT)) && ((ep_desc->bmAttributes & EP_TYPE_MASK) == BULK_ENDPOINT))
       {
           pipe_init.endpoint_number  = (ep_desc->bEndpointAddress & ENDPOINT_MASK);
           pipe_init.direction        = USB_SEND;
           pipe_init.pipetype         = USB_BULK_PIPE;
           pipe_init.max_packet_size  = (uint16_t)(USB_SHORT_UNALIGNED_LE_TO_HOST(ep_desc->wMaxPacketSize) & PACKET_SIZE_MASK);
           pipe_init.interval         = ep_desc->iInterval;
           pipe_init.flags            = 0;
           pipe_init.dev_instance     = mass_class->dev_handle;
           pipe_init.nak_count        = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
           status = usb_host_open_pipe(mass_class->host_handle, &mass_class->bulk_out_pipe, &pipe_init);
           if (status != USB_OK)
           {
               USB_PRINTF("usb_class_msd_init fail to open out pipe\n");
               *class_handle_ptr = (usb_class_handle)mass_class;
               return USBERR_ERROR;
           }
       }          
   }
   
   mass_class->control_pipe = usb_host_dev_mng_get_control_pipe(mass_class->dev_handle);
   
   if ((mass_class->control_pipe) && (mass_class->bulk_in_pipe) && (mass_class->bulk_out_pipe))
   {
      /* Initialize the queue for storing the local copy of interface handles */
      usb_class_mass_q_init(mass_class);
   } /* Endif */

   mass_class->mutex = OS_Mutex_create();
   if (mass_class->mutex == NULL)
   {
      USB_PRINTF("usb_class_msd_init create mutex failed\n");
      *class_handle_ptr = (usb_class_handle)mass_class;
      return USBERR_ALLOC;
   }

   *class_handle_ptr = (usb_class_handle)mass_class;

   /* USB_PRINTF("MSD class driver initialized\n"); */
   
   return USB_OK;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_deinit
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver. It
*     is called in response to a select interface call by application
*
*END*--------------------------------------------------------------------*/
usb_status usb_class_mass_deinit
    (
        /* [IN]  the class driver handle */
        usb_class_handle      handle
     )
{
    usb_mass_class_struct_t *      mass_class = (usb_mass_class_struct_t *)handle;
    usb_status                     status = USB_OK;
    if (mass_class == NULL)
    {
        USB_PRINTF("usb_class_msd_deinit fail\n");
        return USBERR_ERROR;
    }
    
    if (mass_class->bulk_in_pipe != NULL)
    {
        status = usb_host_close_pipe(mass_class->host_handle, mass_class->bulk_in_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_msd_deinit to close pipe\n");
        }
    }
    
    if (mass_class->bulk_out_pipe != NULL)
    {
        status = usb_host_close_pipe(mass_class->host_handle, mass_class->bulk_out_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_msd_deinit to close pipe\n");
        }
    }

    if (mass_class->mutex != NULL)
    {
        OS_Mutex_destroy(mass_class->mutex);
    }
    
    OS_Mem_free(handle);
    /* USB_PRINTF("MSD class driver de-initialized\n"); */
    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_pre_deinit
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver. It
*     is called in response to a select interface call by application
*
*END*--------------------------------------------------------------------*/
usb_status usb_class_mass_pre_deinit
    (
        /* [IN]  the class driver handle */
        usb_class_handle      handle
     )
{
    usb_mass_class_struct_t *      mass_class = (usb_mass_class_struct_t *)handle;
    usb_status                     status = USB_OK;
    if (mass_class == NULL)
    {
        USB_PRINTF("usb_class_msd_pre_deinit fail\n");
        return USBERR_ERROR;
    }
    
    if (mass_class->bulk_in_pipe != NULL)
    {
        status = usb_host_cancel(mass_class->host_handle, mass_class->bulk_in_pipe, NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_host_cancel to close pipe\n");
        }
    }
    
    if (mass_class->bulk_out_pipe != NULL)
    {
        status = usb_host_cancel(mass_class->host_handle, mass_class->bulk_out_pipe, NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_host_cancel to close pipe\n");
        }
    }
    
    /* USB_PRINTF("mass class driver pre_deinit\n"); */
    return USB_OK;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_mass_storage_device_command
* Returned Value : ERROR STATUS error code
*                  USB_OK - means that command has been successfully queued in class
*                  driver queue (or has been passed to USB, if there is not other
*                  command pending)
* Comments       :
*     This routine is called by the protocol layer to execute the command
*     defined in protocol API. It can also be directly called by a user
*     application if they wish to make their own commands (vendor specific) for
*     sending to a mass storage device.
END*--------------------------------------------------------------------*/

usb_status usb_class_mass_storage_device_command
   (
      /* [IN] Command */
      mass_command_struct_t* cmd_ptr
   )
{ /* Body */
   usb_mass_class_struct_t *   mass_class = (usb_mass_class_struct_t *)cmd_ptr->CLASS_PTR;
   int32_t                     temp;
   usb_status                  error = USB_OK;
   cbw_struct_t *              pCbw = (cbw_struct_t *) cmd_ptr->CBW_PTR;
   bool                        empty;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_storage_device_command");
   #endif
   
   if (mass_class == NULL)
   {
      USB_PRINTF("usb_class_mass_storage_device_command fail\n");
      return USBERR_ERROR;
   }

   /* Fill in the remaining CBW fields as per the USB Mass Storage specs */
   *(uint32_t*)&pCbw->DCBWSIGNATURE[0] = USB_HOST_TO_LE_LONG_CONST(CBW_SIGNATURE);
   
   /* CBW is ready so queue it and update status */
   empty = USB_CLASS_MASS_IS_Q_EMPTY(mass_class);
   temp = usb_class_mass_q_insert(mass_class, cmd_ptr);
   if (temp >= 0)
   {
      cmd_ptr->STATUS = STATUS_QUEUED_IN_DRIVER;
      cmd_ptr->CBW_RETRY_COUNT = 0;
      cmd_ptr->DPHASE_RETRY_COUNT = MAX_RETRIAL_ATTEMPTS_IN_DPHASE;
      cmd_ptr->CSW_RETRY_COUNT = 0;
      cmd_ptr->IS_STALL_IN_DPHASE = 0;
      cmd_ptr->BUFFER_SOFAR = 0;
      /* The tag for the command packet is its queue number. */
      *(uint32_t*)&pCbw->DCBWTAG[0] = USB_HOST_TO_LE_LONG(temp);
   
      /*
      ** If queue was empty send CBW to USB immediately, otherwise it will be
      ** taken from queue later by a callback function
      */
      if (empty)
      {
         error = usb_class_mass_pass_on_usb(mass_class);
      } /* Endif */
   }
   else
   {
      error = (usb_status)USB_MASS_QUEUE_FULL;
   } /* Endif */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_storage_device_command, SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,error);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_storage_device_command_cancel
* Returned Value : ERROR STATUS error code
*                  USB_OK - means that command has been successfully de-queued in class
*                  driver queue 

* Comments       :
END*--------------------------------------------------------------------*/

bool usb_class_mass_storage_device_command_cancel
   (
      /* [IN] Command */
      mass_command_struct_t* cmd_ptr
   )
{ /* Body */
   usb_mass_class_struct_t *     mass_class = (usb_mass_class_struct_t *)cmd_ptr->CLASS_PTR;
   bool                          result = FALSE;
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_storage_device_command_cancel");
   #endif
   
   if (mass_class == NULL)
   {
      USB_PRINTF("usb_class_mass_storage_device_command_cancel fail\n");
      return USBERR_ERROR;
   }

   result = usb_class_mass_cancelq(mass_class, cmd_ptr);

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_storage_device_command_cancel, SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,result);
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_call_back_cbw
* Returned Value : None
* Comments       :
*     This routine is called when the CBW phase of the currently pending command
*     has finished.
*
*END*--------------------------------------------------------------------*/

static void usb_class_mass_call_back_cbw
   (
      /* [IN] Pipe on which CBW call was made */
      void*           tr_ptr,

      /* [IN] Pointer to the class instance */
      void*            user_parm,

      /* [IN] Pointer to data buffer */
      uint8_t *        buffer,

      /* [IN] Length of data transfered */
      uint32_t         length_data_transfered,

      /* [IN] Status of the transfer */
      usb_status       status
   )
{ /* Body */
   usb_mass_class_struct_t *        mass_class = NULL;
   mass_command_struct_t*           cmd_ptr = NULL;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_call_back_cbw");
   #endif

   if (user_parm) 
   {
      /* Get the pointer to the pending request */
      mass_class = (usb_mass_class_struct_t *)user_parm;
      usb_class_mass_get_pending_request(mass_class, &cmd_ptr);
   } /* Endif */
   
   /* If there is no user_parm or cmd_ptr we return */
   if (!cmd_ptr) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_call_back_cbw, bad parameter");
      #endif
      return;
   } /* Endif */

   /* Check status of CBW phase and go to the next phase (Data or Status) */
   if (status == USB_OK) 
   {
      /* Verify if the length of the data transferred was correct */
      if (length_data_transfered != CBW_SIZE) 
      {
         if (cmd_ptr->CBW_RETRY_COUNT < MAX_RETRIAL_ATTEMPTS_IN_CBW) 
         {
            cmd_ptr->CBW_RETRY_COUNT++;
            cmd_ptr->RETRY_COUNT++;
            cmd_ptr->STATUS = STATUS_QUEUED_IN_DRIVER;
            cmd_ptr->PREV_STATUS =  cmd_ptr->STATUS; /* preserve the status */
            status = usb_class_mass_reset_recovery_on_usb(mass_class);
         } 
         else 
         {
            /* status = STATUS_CANCELLED; */ /* cannot keep repeating */
            cmd_ptr->PREV_STATUS = STATUS_FAILED_IN_CBW; /* preserve the status */
            status = usb_class_mass_reset_recovery_on_usb(mass_class);
         } /* Endif */
      } 
      else 
      {
         /* Everything is normal, go to next phase of pending request */
         cmd_ptr->STATUS = STATUS_FINISHED_CBW_ON_USB;
         status = usb_class_mass_pass_on_usb(mass_class);
      } /* Endif */
   } 
   else if ((status == USBERR_ENDPOINT_STALLED) || (status == USBERR_TR_FAILED))
   {
      if (cmd_ptr->CBW_RETRY_COUNT < MAX_RETRIAL_ATTEMPTS_IN_CBW) 
      {
         cmd_ptr->CBW_RETRY_COUNT++;
         cmd_ptr->RETRY_COUNT++;
         
         if(USBERR_ENDPOINT_STALLED == status)
         {
            cmd_ptr->STATUS = STATUS_QUEUED_IN_DRIVER; /* this is redundant as clearing pipe will change the status */
            cmd_ptr->PREV_STATUS =  cmd_ptr->STATUS;   /* preserve the status */

            status = usb_class_mass_clear_bulk_pipe_on_usb(mass_class);
         }
         else
         {
            status = usb_class_mass_pass_on_usb(mass_class);
         } /* Endif */
      }
      else
      {
         /* status = STATUS_CANCELLED; */ /* cannot keep repeating */
         cmd_ptr->PREV_STATUS = STATUS_FAILED_IN_CBW; /* preserve the status */
         status = usb_class_mass_reset_recovery_on_usb(mass_class);
      } /* Endif */
   } /* Endif */

   if ((status != USB_OK) && (status != USBERR_ENDPOINT_STALLED) && 
       (status != USBERR_TRANSFER_IN_PROGRESS)) 
   {
      /*
      ** Host must do a reset recovery for the device as per the spec. We also
      ** notify the application of the failure, remove the command from the
      ** queue and move on the the next command queued
      */
      cmd_ptr->STATUS = STATUS_CANCELLED;
      cmd_ptr->CALLBACK((usb_status)USB_MASS_FAILED_IN_COMMAND, mass_class, cmd_ptr, cmd_ptr->TR_BUF_LEN);
      usb_class_mass_deleteq(mass_class);
      status = usb_class_mass_pass_on_usb(mass_class);
   } /* Endif */

   if (usb_host_release_tr(mass_class->host_handle, (tr_struct_t*)tr_ptr) != USB_OK)
   {
       USB_PRINTF("_usb_host_release_tr failed\n");
   }
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_call_back_cbw, SUCCESSFUL");
   #endif
   
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_call_back_dphase
* Returned Value : None
* Comments       :
*     This routine is called by the bus driver when the data phase completes.
*     If status is USB_OK, this routine will pass on the next routine called
*     usb_class_mass_pass_on_usb() which will queue the status phase. If failure
*     the routine will either try to dequeue the packet, or send a Reset device
*     command or will try to de-queue the packet for next phase.
*
*END*--------------------------------------------------------------------*/

static void usb_class_mass_call_back_dphase
   (
      /* [IN] Pipe on which CBW call was made */
      void*             tr_ptr,

      /* [IN] Pointer to the class instance */
      void*             user_parm,

      /* [IN] Pointer to data buffer */
      uint8_t *         buffer,

      /* [IN] Length of data transfered */
      uint32_t          length_data_transfered,

      /* [IN] Status of the transfer */
      usb_status        status
   )
{ /* Body */
   usb_mass_class_struct_t *        mass_class = NULL;
   mass_command_struct_t*           cmd_ptr = NULL;
   usb_status                       return_code;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_call_back_dphase");
   #endif
   
   if (user_parm) 
   {
      /* Get the pointer to the pending request */
      mass_class = (usb_mass_class_struct_t *)user_parm;
      usb_class_mass_get_pending_request(mass_class, &cmd_ptr);
   } /* Endif */

   if (!cmd_ptr) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_call_back_dphase, bad parameter");
      #endif
      return;
   } /* Endif */

   /* Save length transferred during data phase */
   cmd_ptr->TR_BUF_LEN = length_data_transfered;

   /* Test if full or partial data received */
   if ((status == USB_OK) ||
      ((status == USBERR_TR_FAILED) && (buffer != NULL)))
   {
      cmd_ptr->BUFFER_SOFAR += length_data_transfered;
      if((USB_OK == status) && (cmd_ptr->BUFFER_SOFAR < cmd_ptr->BUFFER_LEN))
      {
          cmd_ptr->STATUS = STATUS_FINISHED_CBW_ON_USB;
      }
      else
      {
          /* Everything is OK, update status and go to next phase */
          cmd_ptr->STATUS = STATUS_FINISHED_DPHASE_ON_USB;
      }
      status = usb_class_mass_pass_on_usb(mass_class);
   } 
   else if (status == USBERR_ENDPOINT_STALLED) 
   {
      cmd_ptr->IS_STALL_IN_DPHASE = 1;
      
      if (cmd_ptr->DPHASE_RETRY_COUNT < MAX_RETRIAL_ATTEMPTS_IN_DPHASE) 
      {
         cmd_ptr->DPHASE_RETRY_COUNT++;
         cmd_ptr->RETRY_COUNT++;
         /* cmd_ptr->STATUS = STATUS_QUEUED_IN_DRIVER; */ /* this is redundant as clearing pipe will change the status */
         cmd_ptr->PREV_STATUS = STATUS_QUEUED_IN_DRIVER; /* preserve the status */

         return_code = usb_class_mass_clear_bulk_pipe_on_usb(mass_class);
         
         if (return_code != USB_OK) 
         {
            status = STATUS_CANCELLED;
         }
         else 
         {
            cmd_ptr->PREV_STATUS = STATUS_FINISHED_DPHASE_ON_USB; /* Force the returning status from pipe clear to continue after data phase */
            status = usb_class_mass_pass_on_usb(mass_class);
         }
      } 
      else 
      {
         /* cmd_ptr->STATUS = STATUS_FINISHED_DPHASE_ON_USB; */ /* this is redundant as clearing pipe will change the status */
         cmd_ptr->PREV_STATUS =  STATUS_FINISHED_DPHASE_ON_USB; /* preserve the status */

         return_code = usb_class_mass_clear_bulk_pipe_on_usb(mass_class);
         
         if (return_code != USB_OK)
         {
            status = STATUS_CANCELLED;
         }
      } /* Endif */
   } /* Endif */

   if ((status != USB_OK) && (status != USBERR_ENDPOINT_STALLED) &&
      (status != USBERR_TRANSFER_IN_PROGRESS))
   {
      /* Command didn't succeed. Notify application and cleanup */
      cmd_ptr->STATUS =STATUS_CANCELLED;
      cmd_ptr->CALLBACK((usb_status)USB_MASS_FAILED_IN_DATA, mass_class, cmd_ptr, cmd_ptr->TR_BUF_LEN);
      usb_class_mass_deleteq(mass_class);

      /* Go to the next command, if any */
      status = usb_class_mass_pass_on_usb(mass_class);
   } /* Endif */

   if (usb_host_release_tr(mass_class->host_handle, (tr_struct_t*)tr_ptr) != USB_OK)
   {
       USB_PRINTF("_usb_host_release_tr failed\n");
   }   
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_call_back_dphase, SUCCESSFUL");
   #endif
   
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_call_back_csw
* Returned Value : None
* Comments       : This routine is called by the bus driver when the
*      status phase completes on mass storage device. Please see the specs of
*      USB storage device to check what actions should be taken depending upon
*      the status returned by the device.
*
*END*--------------------------------------------------------------------*/

static void usb_class_mass_call_back_csw
   (
      /* [IN] Pipe on which CBW call was made */
      void*             tr_ptr,

      /* [IN] Pointer to the class instance */
      void*             user_parm,

      /* [IN] Pointer to data buffer */
      uint8_t *         buffer,

      /* [IN] Length of data transfered */
      uint32_t          length_data_transfered,

      /* [IN] Status of the transfer */
      usb_status       status
   )
{ /* Body */
   usb_mass_class_struct_t *      mass_class = NULL;
   mass_command_struct_t*         cmd_ptr = NULL;
   csw_struct_t *                 pCsw = NULL;
   uint32_t                       tmp1;
   uint32_t                       tmp2;
   uint32_t                       tmp3;
   bool                           proc_next = FALSE;
   usb_status                     return_code;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_call_back_csw");
   #endif

   if (user_parm) 
   {
      /* Get the pointer to the pending request */
      mass_class = (usb_mass_class_struct_t *) user_parm;
      usb_class_mass_get_pending_request(mass_class, &cmd_ptr);
   } /* Endif */

   if (cmd_ptr == NULL) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_call_back_csw, bad param");
      #endif
      return;
   } /* Endif */

   /* Finish transaction and queue next command */
   if (status == USB_OK) 
   {
      pCsw = (csw_struct_t *) cmd_ptr->CSW_PTR;
      tmp1 = (uint32_t)USB_LONG_UNALIGNED_LE_TO_HOST(pCsw->DCSWTAG);
      tmp2 = (uint32_t)USB_LONG_UNALIGNED_LE_TO_HOST(cmd_ptr->CBW_PTR->DCBWTAG);
      tmp3 = (uint32_t)USB_LONG_UNALIGNED_LE_TO_HOST(pCsw->DCSWSIGNATURE);

      /* Size must be verified, as well as the signature and the tags */
      if ((length_data_transfered != sizeof(csw_struct_t)) ||
         (tmp3 != CSW_SIGNATURE) || (tmp1 != tmp2))
      {
         if (cmd_ptr->CSW_RETRY_COUNT < MAX_RETRIAL_ATTEMPTS_IN_CSW) 
         {
            cmd_ptr->CSW_RETRY_COUNT++;
            cmd_ptr->RETRY_COUNT++;
            /* cmd_ptr->STATUS = STATUS_FINISHED_DPHASE_ON_USB; */ /* this is redundant as reseting will change the status */
            cmd_ptr->PREV_STATUS = STATUS_FINISHED_DPHASE_ON_USB; /* preserve the status */

            status = usb_class_mass_reset_recovery_on_usb(mass_class);
         } 
         else 
         {
            /* status = STATUS_CANCELLED; */ /* cannot keep repeating */
            cmd_ptr->PREV_STATUS = STATUS_FAILED_IN_CSW; /* preserve the status */
            status = usb_class_mass_reset_recovery_on_usb(mass_class);
         } /* Endif */

         if ((status != USB_OK)  && (status != USBERR_TRANSFER_IN_PROGRESS))
         {
            proc_next = TRUE;
         } /* Endif */
      } 
      else 
      {
         /* Check value of status field in CSW */
         switch (pCsw->BCSWSTATUS) 
         {
            case CSW_STATUS_GOOD:
               /* Command succeed. Notify application and cleanup */
               cmd_ptr->STATUS = STATUS_COMPLETED;
               proc_next = TRUE;
               break;

            case CSW_STATUS_FAILED:
               /* Command failed. Notify application and cleanup */
               cmd_ptr->STATUS = STATUS_COMMAND_FAILED;
               proc_next = TRUE;
               break;

            case CSW_STATUS_PHASE_ERROR:
               /* Command failed. Notify application and cleanup */
               cmd_ptr->STATUS = STATUS_FAILED_IN_CSW;
               cmd_ptr->PREV_STATUS = STATUS_FAILED_IN_CSW; /* preserve the status */
               status = usb_class_mass_reset_recovery_on_usb(mass_class);
               if ((status != USB_OK) && (status != USBERR_TRANSFER_IN_PROGRESS))
               {
                  proc_next = TRUE;
               }
               else
               {
                  proc_next = FALSE;
               }
               break;

            default:
               break;
         } /* Endswitch */
      } /* Endif */
   }
   else if ((status == USBERR_ENDPOINT_STALLED) || (status == USBERR_TR_FAILED))
   {     
      if (cmd_ptr->CSW_RETRY_COUNT < MAX_RETRIAL_ATTEMPTS_IN_CSW) 
      {
         cmd_ptr->CSW_RETRY_COUNT++;
         cmd_ptr->RETRY_COUNT++;
         if(USBERR_ENDPOINT_STALLED == status)
         {
            cmd_ptr->STATUS = STATUS_FINISHED_DPHASE_ON_USB; /* this is redundant as clearing pipe will change the status */
            cmd_ptr->PREV_STATUS = STATUS_FINISHED_DPHASE_ON_USB; /* preserve the status */

            return_code = usb_class_mass_clear_bulk_pipe_on_usb(mass_class);
            if (return_code != USB_OK)
            {
               status = USBERR_TRANSFER_IN_PROGRESS;
            }
         }
         else
         {
            status = usb_class_mass_pass_on_usb(mass_class);
            proc_next = FALSE;
         } /* Endif */
      } 
      else 
      {
         /* status = STATUS_CANCELLED; */ /* cannot keep repeating */
         cmd_ptr->PREV_STATUS = STATUS_FAILED_IN_CSW; /* preserve the status */
         status = usb_class_mass_reset_recovery_on_usb(mass_class);
      } /* Endif */

      if ((status != USB_OK) && (status != USBERR_TRANSFER_IN_PROGRESS) 
       && (status != USBERR_ENDPOINT_STALLED)
         )
      {
         /* Command didn't succeed. Notify application and cleanup */
         cmd_ptr->STATUS = STATUS_CANCELLED;
         proc_next = TRUE;
      } /* Endbody */
   }
   else 
   {
      /* Command didn't succeed. Notify application and cleanup */
      cmd_ptr->STATUS = STATUS_CANCELLED;
      proc_next = TRUE;
   } /* Endif */

   /* Should we cleanup and process the next command? */
   if (proc_next) 
   {
      cmd_ptr->CALLBACK(status, mass_class, cmd_ptr, cmd_ptr->TR_BUF_LEN);
      usb_class_mass_deleteq(mass_class);

      /* Send next command, if any */
      if (!USB_CLASS_MASS_IS_Q_EMPTY(mass_class)) 
      {
         status = usb_class_mass_pass_on_usb(mass_class);
      } /* Endbody */
   } /* Endif */

   if (usb_host_release_tr(mass_class->host_handle, (tr_struct_t*)tr_ptr) != USB_OK)
   {
       USB_PRINTF("_usb_host_release_tr failed\n");
   }
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_call_back_csw, SUCCESSFUL");
   #endif
   
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_pass_on_usb
* Returned Value : ERROR STATUS of the command
* Comments       :
*     This routine gets the pending request from class driver queue and finds
*     what needs to be done by looking at the status (cmd_ptr->STATUS). If a
*     phase has been completed, it will queue the next phase. If there is no
*     pending request in the queue, it will just return.
*     NOTE: This functions should only be called by a callback or within a
*     OS_Lock() OS_Unlock() block!
*
*END*--------------------------------------------------------------------*/

static usb_status usb_class_mass_pass_on_usb
   (
      /* [IN] Interface handle */
      usb_mass_class_struct_t *   mass_class
   )
{ /* Body */
   mass_command_struct_t*      cmd_ptr = NULL;
   tr_struct_t*                tr_ptr;
   usb_status                  status = USB_OK;
   uint8_t                     tmp;
   uint32_t                    data_len;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_pass_on_usb");
   #endif
   
   /* Nothing can be done if there is nothing pending */
   usb_class_mass_get_pending_request(mass_class, &cmd_ptr);
   if (cmd_ptr == NULL) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_pass_on_usb, no matching request");
      #endif
      return (usb_status)USB_MASS_NO_MATCHING_REQUEST;
   } /* Endif */

   /* Determine the appropriate action based on the phase */
   switch (cmd_ptr->STATUS) 
   {
      case STATUS_QUEUED_IN_DRIVER:
         /* means that CBW needs to be sent.*/
         if (usb_host_get_tr(mass_class->host_handle, usb_class_mass_call_back_cbw, mass_class, &tr_ptr) != USB_OK)
         {
            USB_PRINTF("error to get tr\n");
            return USBERR_ERROR;
         }
         tr_ptr->tx_buffer = (uint8_t *)cmd_ptr->CBW_PTR;
         tr_ptr->tx_length = sizeof(cbw_struct_t);

         status = usb_host_send_data(mass_class->host_handle, mass_class->bulk_out_pipe, tr_ptr);
         if (status != USB_OK)
         {
             /* USB_PRINTF("\nError in usb_class_mass_pass_on_usb: %x", status); */
             usb_host_release_tr(mass_class->host_handle, tr_ptr);
             usb_class_mass_deleteq(mass_class);
             return status;
         }
         break;

      case STATUS_FINISHED_CBW_ON_USB:
         /* Determine next phase (DATA or STATUS), length and direction */
         data_len = USB_LONG_UNALIGNED_LE_TO_HOST(cmd_ptr->CBW_PTR->DCBWDATATRANSFERLENGTH);

         if (data_len > 0) 
         {
            /* Common TR setup for IN or OUT direction */
            if (usb_host_get_tr(mass_class->host_handle, usb_class_mass_call_back_dphase, mass_class, &tr_ptr) != USB_OK)
            {
               USB_PRINTF("error to get tr\n");
               return USBERR_ERROR;
            }
            
            tmp = (uint8_t)((cmd_ptr->CBW_PTR->BMCBWFLAGS) & MASK_NON_DIRECTION_BITS);
            switch(tmp)
            {
               case DIRECTION_OUT:
                  /* Make a TR with DATA Phase call back.*/
                  tr_ptr->tx_buffer = (uint8_t *)((uint32_t)cmd_ptr->DATA_BUFFER + cmd_ptr->BUFFER_SOFAR);
                  tr_ptr->tx_length = ((cmd_ptr->BUFFER_LEN - cmd_ptr->BUFFER_SOFAR) > MSD_SEND_MAX_TRANS_LENGTH) ? MSD_SEND_MAX_TRANS_LENGTH : (cmd_ptr->BUFFER_LEN - cmd_ptr->BUFFER_SOFAR);

                  status = usb_host_send_data(mass_class->host_handle, mass_class->bulk_out_pipe, tr_ptr);
                  if (status != USB_OK)
                  {
                      /* USB_PRINTF("\nError in usb_class_mass_pass_on_usb: %x", status); */
                      usb_host_release_tr(mass_class->host_handle, tr_ptr);
                      usb_class_mass_deleteq(mass_class);
                      return status;
                  }
                  break;

               case DIRECTION_IN:
                  /* Make a TR with DATA call back.*/
                  tr_ptr->rx_buffer = (uint8_t *)((uint32_t)cmd_ptr->DATA_BUFFER + cmd_ptr->BUFFER_SOFAR);
                  tr_ptr->rx_length = ((cmd_ptr->BUFFER_LEN - cmd_ptr->BUFFER_SOFAR) > MSD_RECV_MAX_TRANS_LENGTH) ? MSD_RECV_MAX_TRANS_LENGTH : (cmd_ptr->BUFFER_LEN - cmd_ptr->BUFFER_SOFAR);

                  status = usb_host_recv_data(mass_class->host_handle, mass_class->bulk_in_pipe, tr_ptr);
                  if (status != USB_OK)
                  {
                      /* USB_PRINTF("\nError in usb_class_mass_pass_on_usb: %x", status); */
                      usb_host_release_tr(mass_class->host_handle, tr_ptr);
                      usb_class_mass_deleteq(mass_class);
                      return status;
                  }
                  break;

               default:
                  break;
            } /* Endswitch */
            break;
         } /* Endif */

         /*
         ** else case:
         ** No data transfer is expected of the request. Fall through to
         ** STATUS phase
         */

      case STATUS_FINISHED_DPHASE_ON_USB:
         /* Make a TR and send it with STATUS call back */
         if (usb_host_get_tr(mass_class->host_handle, usb_class_mass_call_back_csw, mass_class, &tr_ptr) != USB_OK)
         {
            USB_PRINTF("error to get tr\n");
            return USBERR_ERROR;
         }
         tr_ptr->rx_buffer = (uint8_t *)cmd_ptr->CSW_PTR;
         tr_ptr->rx_length = sizeof(csw_struct_t);

         status = usb_host_recv_data(mass_class->host_handle, mass_class->bulk_in_pipe, tr_ptr);
         if (status != USB_OK)
         {
             /* USB_PRINTF("\nError in usb_class_mass_pass_on_usb: %x", status); */
             usb_host_release_tr(mass_class->host_handle, tr_ptr);
             usb_class_mass_deleteq(mass_class);
             return status;
         }
         break;

      case STATUS_FAILED_IN_CBW:       /* Should never happen */
      case STATUS_FAILED_IN_CSW:       /* Should never happen */
         //cmd_ptr->STATUS = STATUS_CANCELLED;
         cmd_ptr->CALLBACK(cmd_ptr->STATUS, mass_class, cmd_ptr, cmd_ptr->TR_BUF_LEN);
         usb_class_mass_deleteq(mass_class);

         /* Send next command, if any */
         if (!USB_CLASS_MASS_IS_Q_EMPTY(mass_class)) 
         {
            status = usb_class_mass_pass_on_usb(mass_class);
         } /* Endbody */
         break;
      case STATUS_FINISHED_CSW_ON_USB: /* No action */
         break;
      default:
         break;
   } /* Endswitch */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_pass_on_usb, SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,status);
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_getmaxlun_bulkonly
* Returned Value : ERROR STATUS of the command
* Comments       :
*     This is a class specific command. See the documentation of the USB
*     mass storage specification to learn how this command works. This command
*     is used the get the Number of Logical Units on the device. Caller will
*     use the LUN number to direct the commands (as a part of CBW)
*END*--------------------------------------------------------------------*/

usb_status usb_class_mass_getmaxlun_bulkonly
   (
      /* [IN] the class driver handle */
      usb_class_handle           handle,
      uint8_t *                  pLUN,
      tr_callback                callback,
      void*                      callback_param
   )
{ /* Body */
   usb_status                       status = USB_OK; /* USBERR_NO_INTERFACE; */
   usb_mass_class_struct_t *        mass_class = (usb_mass_class_struct_t *)handle;
   tr_struct_t*                     tr_ptr;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_getmaxlun_bulkonly");
   #endif

   if(NULL == mass_class) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_reset_recovery_on_usb,no matching request");
      #endif
      return (usb_status) USB_MASS_NO_MATCHING_REQUEST;
   } /* Endif */

   if (mass_class->dev_handle == NULL)
   {
       #ifdef _HOST_DEBUG_
          DEBUG_LOG_TRACE("_usb_hostdev_cntrl_request, invalid device handle");
       #endif
       return USBERR_DEVICE_NOT_FOUND;
   }
   
   mass_class->ctrl_callback = callback;
   mass_class->ctrl_param = callback_param;

   if (usb_host_get_tr(mass_class->host_handle, usb_class_mass_ctrl_callback, mass_class, &tr_ptr) != USB_OK)
   {
       USB_PRINTF("error to get tr mass\n");
       return USBERR_ERROR;
   }
   
   /* Get the number of logical units on the device */
   tr_ptr->setup_packet.bmrequesttype = REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_IN;
   tr_ptr->setup_packet.brequest = GET_MAX_LUN;
   *(uint16_t*)&tr_ptr->setup_packet.wvalue[0] = USB_HOST_TO_LE_SHORT_CONST(0);
   *(uint16_t*)&tr_ptr->setup_packet.windex[0] = USB_HOST_TO_LE_SHORT(((interface_descriptor_t*)(((usb_device_interface_struct_t *)mass_class->intf_handle)->lpinterfaceDesc))->bInterfaceNumber);
   *(uint16_t*)&tr_ptr->setup_packet.wlength[0] = USB_HOST_TO_LE_SHORT_CONST(1);

   /* Set TR buffer length as required */
   if ((REQ_TYPE_IN & tr_ptr->setup_packet.bmrequesttype) != 0) 
   {
       tr_ptr->rx_buffer = pLUN;
       tr_ptr->rx_length = 1;
   } 
   else 
   {
       tr_ptr->tx_buffer = pLUN;
       tr_ptr->tx_length = 1;
   } /* EndIf */
   
   status = usb_host_send_setup(mass_class->host_handle, mass_class->control_pipe, tr_ptr);
   
    if (status != USB_OK)
    {
        USB_PRINTF("\nError in usb_class_mass_getmaxlun_bulkonly: %x", (unsigned int)status);
        usb_host_release_tr(mass_class->host_handle, tr_ptr);
        return USBERR_ERROR;
    }
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_getmaxlun_bulkonly, SUCCESSFUL");
   #endif

   return USB_log_error(__FILE__,__LINE__,status);
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_getvidpid
* Returned Value : ERROR STATUS of the command
* Comments       :
*     This is a class specific command. See the documentation of the USB
*     mass storage specification to learn how this command works. This command
*     is used the get the Number of Logical Units on the device. Caller will
*     use the LUN number to direct the commands (as a part of CBW)
*END*--------------------------------------------------------------------*/

usb_status usb_class_mass_getvidpid
   (
      /* [IN] the class driver handle */
      usb_class_handle handle,
      /* [OUT] USB device VID */
      uint16_t * vid,
      /* [OUT] USB device PID */
      uint16_t * pid
   )
{ /* Body */
   usb_status                       status = USB_OK;/* USBERR_NO_INTERFACE; */
   usb_mass_class_struct_t *        mass_class = (usb_mass_class_struct_t *)handle;
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_getvidpid");
   #endif

   if(NULL == mass_class) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_reset_recovery_on_usb,no matching request");
      #endif
      return (usb_status) USB_MASS_NO_MATCHING_REQUEST;
   } /* Endif */

   if (mass_class->dev_handle == NULL)
   {
       #ifdef _HOST_DEBUG_
          DEBUG_LOG_TRACE("_usb_hostdev_cntrl_request, invalid device handle");
       #endif
       return USBERR_DEVICE_NOT_FOUND;
   }
   
   *pid = usb_host_dev_mng_get_pid(mass_class->dev_handle);
   *vid = usb_host_dev_mng_get_vid(mass_class->dev_handle);
   
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_getvidpid, SUCCESSFUL");
   #endif

   return USB_log_error(__FILE__,__LINE__,status);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_clear_bulk_pipe_on_usb
* Returned Value : ERROR STATUS of the command
* Comments       :
*     This routine gets the pending request from class driver queue and sends
      the CLEAR_FEATURE command on control pipe. This routine is called when
      status phase of the pending command fails and class driver decides to
      clear the pipes. If there is no pending request in the queue, it will
      just return.
*END*--------------------------------------------------------------------*/

static usb_status usb_class_mass_clear_bulk_pipe_on_usb
   (
      usb_mass_class_struct_t *   mass_class
   )
{ /* Body */
   usb_status                    status = USB_OK;
   pipe_struct_t*                pBulk_pipe = NULL;
   mass_command_struct_t*        cmd_ptr = NULL;
   uint8_t                       direction;
   dev_instance_t*               dev_inst_ptr = NULL;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_clear_bulk_pipe_on_usb");
   #endif
   
   /* Nothing can be done if there is nothing pending */
   usb_class_mass_get_pending_request(mass_class, &cmd_ptr);
   if (cmd_ptr == NULL) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_clear_bulk_pipe_on_usb, no matching request");
      #endif
      return USB_log_error(__FILE__,__LINE__,(usb_status)USB_MASS_NO_MATCHING_REQUEST);
   } /* Endif */

   /* stall in csw */
   if(STATUS_FINISHED_DPHASE_ON_USB == cmd_ptr->STATUS) 
   {
       pBulk_pipe = (pipe_struct_t*)mass_class->bulk_in_pipe;
       direction = REQ_TYPE_IN;
   }
   else if (STATUS_QUEUED_IN_DRIVER == cmd_ptr->STATUS)
   {
       pBulk_pipe = (pipe_struct_t*)mass_class->bulk_out_pipe;
       direction = REQ_TYPE_OUT;
   }
   else
   {
       /* Determine pipe (Bulk IN or Bulk OUT) to clear */
       if ((cmd_ptr->CBW_PTR->BMCBWFLAGS & MASK_NON_DIRECTION_BITS) == DIRECTION_IN) 
       {
           pBulk_pipe = (pipe_struct_t*)mass_class->bulk_in_pipe;
           direction = REQ_TYPE_IN;
       } 
       else 
       {
           pBulk_pipe = (pipe_struct_t*)mass_class->bulk_out_pipe;
           direction = REQ_TYPE_OUT;
       } /* Endif */
   } /* Endif */

   dev_inst_ptr = (dev_instance_t*)mass_class->dev_handle;
   if (NULL == dev_inst_ptr)
   {
       #ifdef _HOST_DEBUG_
          DEBUG_LOG_TRACE("usb_class_mass_clear_bulk_pipe_on_usb, invalid device handle");
       #endif
       return USBERR_DEVICE_NOT_FOUND;
   }

   status = usb_host_register_ch9_callback(mass_class->host_handle,mass_class->dev_handle,
      usb_class_mass_reset_callback, (void*)mass_class);
   
   if (status == USB_OK) 
   {
       /* Send a CLEAR PIPE command */
       cmd_ptr->STATUS = STATUS_CLEAR_BULK_PIPE;
       status = _usb_host_ch9_clear_feature(mass_class->dev_handle,
          REQ_TYPE_ENDPOINT, (uint8_t)(pBulk_pipe->endpoint_number | direction), ENDPOINT_HALT);
   }

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_clear_bulk_pipe_on_usb, SUCCESSFUL");
   #endif
   
   return USB_log_error(__FILE__,__LINE__,status);
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_reset_recovery_on_usb
* Returned Value : ERROR STATUS of the command
* Comments       :
*     This routine gets the pending request from class driver queue and sends
*     the RESET command on control pipe. This routine is called when
*     a phase of the pending command fails and class driver decides to
*     reset the device. If there is no pending request in the queue, it will
*     just return. This routine registers a call back for control pipe commands
*     to ensure that pending command is queued again.
*     NOTE: This functions should only be called by a callback or within a
*     OS_Lock() OS_Unlock() block!
*
*END*--------------------------------------------------------------------*/

usb_status usb_class_mass_reset_recovery_on_usb
   (
      usb_mass_class_struct_t *   mass_class
   )
{ /* Body */
   mass_command_struct_t*         cmd_ptr = NULL;
   usb_status                     status = USB_OK;
   tr_struct_t*                   tr_ptr;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_reset_recovery_on_usb");
   #endif
   
   /* Nothing can be done if there is nothing pending*/
   usb_class_mass_get_pending_request(mass_class,&cmd_ptr);
   if(cmd_ptr == NULL) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_reset_recovery_on_usb,no matching request");
      #endif
      return (usb_status) USB_MASS_NO_MATCHING_REQUEST;
   } /* Endif */

   if (mass_class->dev_handle == NULL)
   {
       #ifdef _HOST_DEBUG_
          DEBUG_LOG_TRACE("_usb_hostdev_cntrl_request, invalid device handle");
       #endif
       return USBERR_DEVICE_NOT_FOUND;
   }

   if (usb_host_get_tr(mass_class->host_handle, usb_class_mass_reset_callback, mass_class, &tr_ptr) != USB_OK)
   {
       USB_PRINTF("error to get tr mass\n");
       return USBERR_ERROR;
   }
   
   /* BULK device mass storage reset */
   tr_ptr->setup_packet.bmrequesttype   = REQ_TYPE_CLASS | REQ_TYPE_INTERFACE | REQ_TYPE_OUT;
   tr_ptr->setup_packet.brequest        = MASS_STORAGE_RESET;
   *(uint16_t*)&tr_ptr->setup_packet.wvalue[0] = USB_HOST_TO_LE_SHORT_CONST(0);
   *(uint16_t*)&tr_ptr->setup_packet.windex[0] = USB_HOST_TO_LE_SHORT(((interface_descriptor_t*)(((usb_device_interface_struct_t *)mass_class->intf_handle)->lpinterfaceDesc))->bInterfaceNumber);
   *(uint16_t*)&tr_ptr->setup_packet.wlength[0] = USB_HOST_TO_LE_SHORT_CONST(0);

   /* Set TR buffer length as required */
   if ((REQ_TYPE_IN & tr_ptr->setup_packet.bmrequesttype) != 0) 
   {
       tr_ptr->rx_buffer = NULL;
       tr_ptr->rx_length = 0;
   } 
   else 
   {
       tr_ptr->tx_buffer = NULL;
       tr_ptr->tx_length = 0;
   } /* EndIf */
   
   status = usb_host_send_setup(mass_class->host_handle, mass_class->control_pipe, tr_ptr);  
   if (status == USB_OK)
   {
       cmd_ptr->STATUS = STATUS_RESET_DEVICE;
   }
   else
   {
       USB_PRINTF("\nError in usb_class_mass_cntrl_common: %x", (unsigned int)status);
       usb_host_release_tr(mass_class->host_handle, tr_ptr);
       return USBERR_ERROR;
   }

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_reset_recovery_on_usb,SUCCESSFUL");
   #endif

   return USB_log_error(__FILE__,__LINE__,status);
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_reset_callback
* Returned Value : None
* Comments       : 
*     Callback used when resetting and clearing endpoints. It is also
*     state machine for reset recovery.
*
*END*--------------------------------------------------------------------*/

static void usb_class_mass_reset_callback
   (
      /* [IN] pointer to pipe */
      void*            tr_ptr,

      /* [IN] user-defined parameter */
      void*           user_parm,

      /* [IN] buffer address */
      uint8_t *         buffer,

      /* [IN] length of data transferred */
      uint32_t           length_data_transfered,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      usb_status        status
   )
{ /* Body */

   usb_mass_class_struct_t *        mass_class = NULL;
   pipe_struct_t*                   pPipe;
   mass_command_struct_t*           cmd_ptr = NULL;
   dev_instance_t*                  dev_inst_ptr = NULL;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_reset_callback");
   #endif

   if (user_parm) 
   {
      /* Get the pointer to the pending request */
      mass_class = (usb_mass_class_struct_t *)user_parm;
      usb_class_mass_get_pending_request(mass_class, &cmd_ptr);
   } /* Endif */

   usb_host_unregister_ch9_callback(mass_class->host_handle, mass_class->dev_handle);

   if (usb_host_release_tr(mass_class->host_handle, (tr_struct_t*)tr_ptr) != USB_OK)
   {
       USB_PRINTF("_usb_host_release_tr failed\n");
   }
   
   if (!cmd_ptr) 
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_class_mass_reset_callback, bad param");
      #endif
      return;
   } /* Endif */

   /* Check the status of the reset and act accordingly */
   if (status == USB_OK) 
   {
      switch (cmd_ptr->STATUS) 
      {
         case STATUS_RESET_DEVICE:
            /* Reset the Bulk IN pipe */
            pPipe = (pipe_struct_t*)mass_class->bulk_in_pipe;
            
            dev_inst_ptr = (dev_instance_t*)mass_class->dev_handle;
            if (NULL == dev_inst_ptr)
            {
                #ifdef _HOST_DEBUG_
                   DEBUG_LOG_TRACE("usb_class_mass_clear_bulk_pipe_on_usb, invalid device handle");
                #endif
                break;
            }
            
            status = usb_host_register_ch9_callback(mass_class->host_handle,mass_class->dev_handle,
               usb_class_mass_reset_callback, (void*)mass_class);
            
            if (status == USB_OK) 
            {
                status = _usb_host_ch9_clear_feature(mass_class->dev_handle,
                        REQ_TYPE_ENDPOINT, (uint8_t)(pPipe->endpoint_number | REQ_TYPE_IN), ENDPOINT_HALT);
                cmd_ptr->STATUS = STATUS_RESET_BULK_IN;
            }
            break;

         case STATUS_RESET_BULK_IN:
            /* Reset the Bulk OUT pipe*/
            pPipe = (pipe_struct_t*)mass_class->bulk_out_pipe;
            
            dev_inst_ptr = (dev_instance_t*)mass_class->dev_handle;
            if (NULL == dev_inst_ptr)
            {
                #ifdef _HOST_DEBUG_
                   DEBUG_LOG_TRACE("usb_class_mass_clear_bulk_pipe_on_usb, invalid device handle");
                #endif
                break;
            }
            status = usb_host_register_ch9_callback(mass_class->host_handle,mass_class->dev_handle,
               usb_class_mass_reset_callback, (void*)mass_class);
            
            if (status == USB_OK) 
            {
                status = _usb_host_ch9_clear_feature(mass_class->dev_handle,
                        REQ_TYPE_ENDPOINT, (uint8_t)(pPipe->endpoint_number | REQ_TYPE_OUT), ENDPOINT_HALT);
                cmd_ptr->STATUS   =   STATUS_RESET_BULK_OUT;
            }
            break;

         case STATUS_RESET_BULK_OUT:
            usb_class_mass_reset_in_pipe(mass_class);
            usb_class_mass_reset_out_pipe(mass_class);
            
            /* this has no significance, just for fixing misra error */
            if (cmd_ptr->STATUS != STATUS_RESET_BULK_OUT)
            {
                break;
            }
         case STATUS_CLEAR_BULK_PIPE:
            /* pPipe = (pipe_struct_t*)mass_class->bulk_in_pipe; */
            /* pPipe->nextdata01 = 0; */ /* reset the NEXTDATA toggle bit */
            cmd_ptr->STATUS = cmd_ptr->PREV_STATUS; /* restore status set prior to call reset function */
            status = usb_class_mass_pass_on_usb(mass_class);
            break;

         default:

            /* we should never appear here, however this case is handled by resetting the device */
            status = usb_class_mass_reset_recovery_on_usb(mass_class);
            break;
      } /* Endswitch */

   } 
   else if (status == USBERR_ENDPOINT_STALLED) 
   {

      cmd_ptr->STATUS = STATUS_CANCELLED;
      cmd_ptr->CALLBACK(status, mass_class, cmd_ptr, cmd_ptr->TR_BUF_LEN);
      usb_class_mass_deleteq(mass_class);

      /* go to the next packet if any */
      usb_class_mass_pass_on_usb(mass_class);
   }
   else
   {
      cmd_ptr->STATUS = STATUS_CANCELLED;
      cmd_ptr->CALLBACK(status, mass_class, cmd_ptr, cmd_ptr->TR_BUF_LEN);
      usb_class_mass_deleteq(mass_class);
      
      /* go to the next packet if any */
      usb_class_mass_pass_on_usb(mass_class);
   }/* Endif */

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_class_mass_reset_callback, SUCCESSFUL");
   #endif
   
} /* Endbody */


static void usb_class_mass_ctrl_callback(void* pipe_handle, void* user_param, uint8_t * buffer, uint32_t size, usb_status status)
{  
   usb_mass_class_struct_t * mass_class = (usb_mass_class_struct_t *)user_param;
   
   if(NULL != mass_class->ctrl_callback)
   {
       mass_class->ctrl_callback(pipe_handle, mass_class->ctrl_param, buffer, size, status);
   }
   
   if ((NULL != mass_class) && (usb_host_release_tr(mass_class->host_handle, (tr_struct_t*)pipe_handle) != USB_OK))
   {
       USB_PRINTF("_usb_host_release_tr failed\n");
   }
}


static usb_status usb_class_mass_reset_in_pipe
    (
    usb_mass_class_struct_t *   mass_class
    )
{
    usb_device_interface_struct_t*  pDeviceIntf = (usb_device_interface_struct_t*)mass_class->intf_handle;
    endpoint_descriptor_t*          ep_desc = NULL;
    uint8_t                         ep_num;
    pipe_init_struct_t              pipe_init;
    usb_status                      status = USB_OK;

    if (mass_class->bulk_in_pipe != NULL)
    {
        status = usb_host_cancel(mass_class->host_handle, mass_class->bulk_in_pipe, NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_host_cancel to close pipe\n");
        }
    }

    if (mass_class->bulk_in_pipe != NULL)
    {
        status = usb_host_close_pipe(mass_class->host_handle, mass_class->bulk_in_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_msd_deinit to close pipe\n");
        }
    }

    mass_class->bulk_in_pipe = NULL;
    for (ep_num = 0; ep_num < pDeviceIntf->ep_count; ep_num++)
    {
        ep_desc = pDeviceIntf->ep[ep_num].lpEndpointDesc;
        if ((ep_desc->bEndpointAddress & IN_ENDPOINT) && ((ep_desc->bmAttributes & EP_TYPE_MASK) == BULK_ENDPOINT))
        {
            pipe_init.endpoint_number  = (ep_desc->bEndpointAddress & ENDPOINT_MASK);
            pipe_init.direction        = USB_RECV;
            pipe_init.pipetype         = USB_BULK_PIPE;
            pipe_init.max_packet_size  = (uint16_t)(USB_SHORT_UNALIGNED_LE_TO_HOST(ep_desc->wMaxPacketSize) & PACKET_SIZE_MASK);
            pipe_init.interval         = ep_desc->iInterval;
            pipe_init.flags            = 0;
            pipe_init.dev_instance     = mass_class->dev_handle;
            pipe_init.nak_count        = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
            status = usb_host_open_pipe(mass_class->host_handle, &mass_class->bulk_in_pipe, &pipe_init);

            if(status == USB_OK)
            {
                break;
            }
        }      
    }

    return status;
}

static usb_status usb_class_mass_reset_out_pipe
    (
    usb_mass_class_struct_t *   mass_class
    )
{
    usb_device_interface_struct_t*  pDeviceIntf = (usb_device_interface_struct_t*)mass_class->intf_handle;
    endpoint_descriptor_t*          ep_desc = NULL;
    uint8_t                         ep_num;
    pipe_init_struct_t              pipe_init;
    usb_status                      status = USB_OK;

    if (mass_class->bulk_out_pipe != NULL)
    {
        status = usb_host_cancel(mass_class->host_handle, mass_class->bulk_out_pipe, NULL);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_host_cancel to close pipe\n");
        }
    }
    
    if (mass_class->bulk_out_pipe != NULL)
    {
        status = usb_host_close_pipe(mass_class->host_handle, mass_class->bulk_out_pipe);
        if (status != USB_OK)
        {
            USB_PRINTF("error in usb_class_msd_deinit to close pipe\n");
        }
    }

    mass_class->bulk_out_pipe = NULL;
    for (ep_num = 0; ep_num < pDeviceIntf->ep_count; ep_num++)
    {
        ep_desc = pDeviceIntf->ep[ep_num].lpEndpointDesc;
        if ((!(ep_desc->bEndpointAddress & OUT_ENDPOINT)) && ((ep_desc->bmAttributes & EP_TYPE_MASK) == BULK_ENDPOINT))
        {
            pipe_init.endpoint_number  = (ep_desc->bEndpointAddress & ENDPOINT_MASK);
            pipe_init.direction        = USB_SEND;
            pipe_init.pipetype         = USB_BULK_PIPE;
            pipe_init.max_packet_size  = (uint16_t)(USB_SHORT_UNALIGNED_LE_TO_HOST(ep_desc->wMaxPacketSize) & PACKET_SIZE_MASK);
            pipe_init.interval         = ep_desc->iInterval;
            pipe_init.flags            = 0;
            pipe_init.dev_instance     = mass_class->dev_handle;
            pipe_init.nak_count        = USBCFG_HOST_DEFAULT_MAX_NAK_COUNT;
            status = usb_host_open_pipe(mass_class->host_handle, &mass_class->bulk_out_pipe, &pipe_init);

            if(status == USB_OK)
            {
                break;
            }
        }      
    }

    return status;
}

#endif

