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
 * $FileName: usb_otg_sm_main.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file contains the implementation of the OTG state machine
 *         
 *****************************************************************************/
#include "usb.h"
#include "usb_otg_main.h"
#include "usb_otg_sm.h"
#include "usb_otg_private.h"
#include "usb_otg.h"
#include "usb_host_ch9.h"
/* Constant Definitions*********************************************************/

/* Type Definitions*********************************************************/

/* Private memory definitions ***********************************************/

/* Private functions prototypes *********************************************/

/* Public functions *********************************************************/

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _usb_otg_sm
 * Returned Value   :
 * Comments         : Handles the changes in OTG status
 *    
 *
 *END*----------------------------------------------------------------------*/
void _usb_otg_sm
(
    usb_otg_handle otg_handle
)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;

    /* Check the device number */
    if (otg_handle != NULL)
    {
        usb_otg_status_t *otg_status = &usb_otg_struct_ptr->otg_status;
        if (usb_otg_struct_ptr->device_state == USB_OTG_DEVSTATE_A)
        {
            _usb_otg_sm_a(otg_handle);
        }
        else
        {
            if (usb_otg_struct_ptr->device_state == USB_OTG_DEVSTATE_B)
            {
                _usb_otg_sm_b(otg_handle);
            }
            else
            {
                /* State is undefined, this point is reached after initialization */
                /* ID change events are handled here */
                if (otg_status->id)
                {
                    _usb_otg_id_chg_b(otg_handle);
                }
                else
                {
                    _usb_otg_id_chg_a(otg_handle);
                }
            }
        }
    }
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _usb_otg_id_chg_a
 * Returned Value   :
 * Comments         : Handles the changes in OTG status for the A-device
 *    
 *
 *END*----------------------------------------------------------------------*/
void _usb_otg_id_chg_a
(
    usb_otg_handle otg_handle
)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t *otg_status = &usb_otg_struct_ptr->otg_status;

    /* Initialize the state machine */
    _usb_otg_callback_set_pull_downs(otg_handle, OTG_CTRL_PDOWN_DP | OTG_CTRL_PDOWN_DM);
    usb_otg_struct_ptr->device_state = USB_OTG_DEVSTATE_A;
    usb_otg_struct_ptr->sub_state = USB_OTG_SM_A_IDLE;
    otg_status->a_bus_req = TRUE;
    otg_status->a_srp_det = FALSE;
    otg_status->a_srp_det_state = srp_not_started;
    otg_status->b_timeout_en = FALSE;
    otg_status->b_timeout = 0;
    /* Signal the event to the application */
    OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_IDLE);
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _usb_otg_id_chg_b
 * Returned Value   :
 * Comments         : Handles the changes in OTG status for the B-device
 *    
 *
 *END*----------------------------------------------------------------------*/
void _usb_otg_id_chg_b
(
    usb_otg_handle otg_handle
    )
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;

    /* Initialize the state machine */
    usb_otg_struct_ptr->device_state = USB_OTG_DEVSTATE_B;
    usb_otg_struct_ptr->sub_state = USB_OTG_SM_B_IDLE_SESS_DETECT;
    /* Signal the event to the application */
    OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_B_IDLE);
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _usb_otg_load_host
 * Returned Value   :
 * Comments         : 
 *    
 *
 *END*----------------------------------------------------------------------*/
usb_status _usb_otg_load_host
(
    usb_otg_handle otg_handle
)
{
    usb_status status;
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;

    status = usb_otg_struct_ptr->init_struct->load_usb_host();
    if (status != USB_OK)
    {
#if _DEBUG
        USB_PRINTF("\n HOST STACK LOAD ERROR");
#endif  
    }
    return status;
}
/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _usb_otg_load_device
 * Returned Value   :
 * Comments         : 
 *    
 *
 *END*----------------------------------------------------------------------*/
usb_status _usb_otg_load_device
(
    usb_otg_handle otg_handle
)
{
    usb_status status;
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    status = usb_otg_struct_ptr->init_struct->load_usb_device();
    if (status != USB_OK)
    {
#if _DEBUG
        USB_PRINTF ("\n DEVICE STACK LOAD ERROR");
#endif  
    }
    return status;
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _usb_otg_unload_host
 * Returned Value   :
 * Comments         : 
 *    
 *
 *END*----------------------------------------------------------------------*/
void _usb_otg_unload_host
(
    usb_otg_handle otg_handle
    )
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_struct_ptr->init_struct->unload_usb_host();
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _usb_otg_unload_device
 * Returned Value   :
 * Comments         : 
 *    
 *
 *END*----------------------------------------------------------------------*/
void _usb_otg_unload_device
(
    usb_otg_handle otg_handle
)
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_struct_ptr->init_struct->unload_usb_device();
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _usb_otg_unload_active
 * Returned Value   :
 * Comments         : 
 *    
 *
 *END*----------------------------------------------------------------------*/
void _usb_otg_unload_active
(
    usb_otg_handle otg_handle
    )
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t * otg_status_ptr = &usb_otg_struct_ptr->otg_status;

    if (otg_status_ptr->active_stack == USB_ACTIVE_STACK_HOST)
    {
        usb_otg_struct_ptr->init_struct->unload_usb_host();
    }
    else if (otg_status_ptr->active_stack == USB_ACTIVE_STACK_DEVICE)
    {
        usb_otg_struct_ptr->init_struct->unload_usb_device();
    }
}

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _usb_otg_hnp_poll_req
 * Returned Value   : HNP poll request status
 * Comments         : This function is called from the otg stack by the host device to check
 *                    if the peripheral device wants to become host.   
 *                    The function initiates a get status request with the otg status selector in the
 *                    wIndex field.
 *END*----------------------------------------------------------------------*/
uint32_t _usb_otg_hnp_poll_req
(
    usb_otg_handle handle
)
{
    usb_device_handle dev_inst_ptr;
    usb_otg_state_struct_t * usb_otg_struct_ptr;
    usb_otg_status_t * otg_status;
    uint32_t status = USB_OK;

    if (handle == NULL)
    {
        return USBERR_ERROR;
    }
    usb_otg_struct_ptr = (usb_otg_state_struct_t *) handle;
    dev_inst_ptr = usb_otg_struct_ptr->dev_inst_ptr;
    if ((void*) dev_inst_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    otg_status = &usb_otg_struct_ptr->otg_status;
    OS_Lock();
    status = _usb_host_ch9_get_status(dev_inst_ptr, 0, OTG_STATUS_SELECTOR, (uint8_t *) &otg_status->hnp_req);
    OS_Unlock();
    return status;
}
