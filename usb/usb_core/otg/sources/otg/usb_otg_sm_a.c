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
 * $FileName: usb_otg_sm_a.c$
 * $Version : 
 * $Date    : 
 *
 * Comments : This file contains the implementation of the OTG state machine
 *
 *         
 *****************************************************************************/
#include "usb.h"
#include "usb_otg_main.h"
#include "usb_otg_private.h"
#include "usb_otg_sm.h"
/* Constant Definitions*********************************************************/

/* Type Definitions*********************************************************/

/* Private memory definitions ***********************************************/

/* Private functions prototypes *********************************************/
static void _usb_otg_a_change_state(usb_otg_handle otg_handle, uint8_t new_state);

/* Public functions *********************************************************/

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _usb_otg_sm_a
 * Returned Value   :
 * Comments         : This function handles the substates of the B-state machine
 *    
 *
 *END*----------------------------------------------------------------------*/
void _usb_otg_sm_a
(
    usb_otg_handle otg_handle
    )
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t *otg_status = &usb_otg_struct_ptr->otg_status;

    switch(usb_otg_struct_ptr->sub_state)
    {
    case USB_OTG_SM_A_IDLE:
        if ((!otg_status->a_srp_det) && (otg_status->sess_end))
        {
            /* srp detection */
            if (otg_status->a_srp_det_state == srp_not_started)
            {
                if ((otg_status->live_se0) && (otg_status->line_stable))
                {
                    /* se0 detected */
                    otg_status->a_srp_det_state = srp_se0;
                }
            }
            else if (otg_status->a_srp_det_state == srp_se0)
            {
                if (otg_status->line_stable)
                {
                    if (otg_status->live_jstate)
                    {
                        /* j state detected */
                        otg_status->a_srp_det_state = srp_dp_puls;
                        otg_status->host_req_poll_timer = 0;
                    }
                    else if (!otg_status->live_se0)
                    {
                        otg_status->a_srp_det_state = srp_not_started;
                    }
                }
            }
            else/* srp_dp_puls state */
            {
                if (otg_status->line_stable)
                {
                    if (otg_status->live_se0)
                    {
                        otg_status->a_srp_det_state = srp_not_started;
                        otg_status->a_srp_pulse_duration = otg_status->host_req_poll_timer;
                        if ((TB_DATA_PLS_MIN <= otg_status->a_srp_pulse_duration) &&
                        (otg_status->a_srp_pulse_duration <= TB_DATA_PLS_MAX))
                        {
                            /* valid srp pulse detected */
                            otg_status->a_srp_det = TRUE;
                        }
                    }
                    else if (!otg_status->live_jstate)
                    {
                        otg_status->a_srp_det_state = srp_not_started;
                    }
                }
            }
        }
        else /* (otg_status->sess_end) */
        {
            otg_status->a_srp_det_state = srp_not_started;
        }
        if (otg_status->id)
        {
            _usb_otg_id_chg_b(otg_handle);
            OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_ID_TRUE);
            break;
        }
        if (otg_status->a_bus_drop)
        {
            break;
        }
        if ((otg_status->a_bus_req) || (usb_otg_struct_ptr->power_up) || (otg_status->a_srp_det))
        {
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_VRISE);
            usb_otg_struct_ptr->power_up = FALSE;

        }
        break;
    case USB_OTG_SM_A_WAIT_VRISE:
        if ((otg_status->id) || (otg_status->a_bus_drop))
        {
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_VFALL);
            break;
        }
        if (otg_status->vbus_valid)
        {
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_BCON);
        }
        else if (otg_status->b_timeout_en == (uint8_t) FALSE)
        {
            /* TA_VBUS_RISE time expires */
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_VFALL);
            OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_WAIT_VRISE_TMOUT);

        }

        break;
    case USB_OTG_SM_A_WAIT_BCON:
        if ((otg_status->id) || (otg_status->a_bus_drop))
        {
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_VFALL);
            break;
        }
        if (!otg_status->vbus_valid)
        {
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_VBUS_ERR);
            OS_Event_set((usb_otg_struct_ptr->otg_app_event), OTG_A_VBUS_ERR);
            break;
        }
        if ((!otg_status->live_se0) && (otg_status->ms_since_line_changed >= otg_status->b_conn_dbnc_time))
        {
            /* b device has connected */
            otg_status->b_conn = TRUE;
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_HOST);
            OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_HOST);
        }
        else if (otg_status->b_timeout_en == (uint8_t) FALSE)
        {
            /* TA_WAIT_BCON timeout expires */
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_VFALL);
            OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_WAIT_BCON_TMOUT);
        }
        else
        {
            /* if the short de-bounce window expires , long de-bounce time must be used */
            if (otg_status->host_req_poll_timer >= TA_BCON_SDB_WIN)
            {
                otg_status->b_conn_dbnc_time = TA_BCON_LDB;
            }

        }
        break;
    case USB_OTG_SM_A_VBUS_ERR:
        if ((otg_status->id) || (otg_status->a_bus_drop) || (otg_status->a_clr_err))
        {
            otg_status->a_clr_err = 0;
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_VFALL);
        }
        break;
    case USB_OTG_SM_A_WAIT_VFALL:
        if (otg_status->b_timeout_en == (uint8_t) FALSE)
        {
            /* TA_VBUS_FALL timeout expires */
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_IDLE);
        }
        break;
    case USB_OTG_SM_A_HOST:
        if (!otg_status->vbus_valid)
        {
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_VBUS_ERR);
            OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_VBUS_ERR);
            break;
        }
        if ((otg_status->id) || (otg_status->a_bus_drop))
        {
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_VFALL);
            if (otg_status->id)
            {
                OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_ID_TRUE);
            }

            break;
        }
        if (!otg_status->b_conn)
        {
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_BCON);
            break;
        }
        if (otg_status->hnp_req)
        {
            /* B device has requested HNP */
            OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_B_HNP_REQ);
        }
        if (!otg_status->a_bus_req)
        {
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_SUSPEND);
            break;
        }
        if (usb_otg_struct_ptr->dev_inst_ptr != NULL)
        {
            /* A device polls B device for HNP request */
            if ((otg_status->a_set_b_hnp_en) && (otg_status->host_req_poll_timer >= T_HOST_REQ_POLL))
            {
                otg_status->host_req_poll_timer = 0;
                _usb_otg_hnp_poll_req(usb_otg_struct_ptr);
            }
        }
        break;
    case USB_OTG_SM_A_SUSPEND:
        if (!otg_status->vbus_valid)
        {
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_VBUS_ERR);
            OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_VBUS_ERR);
            break;
        }
        if ((otg_status->id) || (otg_status->a_bus_drop))
        {
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_VFALL);
            break;
        }
        if (otg_status->a_set_b_hnp_en)
        {
            if ((otg_status->live_se0) && ((otg_status->line_stable) && (!otg_status->a_bus_req)))
            {
                /* B device  detects bus idle and disconnects as part of the HNP . */
                /* A device must change its state to peripheral. */
                /* DP pullup must be enabled , DP pulldown disabled and DM pulldown maintained .*/
                otg_status->b_conn = FALSE;
                _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_PERIPHERAL);
                _usb_otg_callback_set_dp_pull_up(otg_handle, TRUE);
                _usb_otg_callback_set_pull_downs(otg_handle, OTG_CTRL_PDOWN_DM);

                OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_PERIPHERAL);
                break;
            }
            if (otg_status->b_timeout_en == (uint8_t) FALSE)
            {
                /* TA_AIDL_BDIS timeout expires */
                if (otg_status->a_bus_req)
                {
                    _usb_otg_callback_generate_resume(otg_handle, TRUE);
                    _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_SUSPEND_RESUME);
                }
                else
                {
                    _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_VFALL);
                    OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_AIDL_BDIS_TMOUT);
                }
                break;
            }
        }
        else/* (otg_status->a_set_b_hnp_en) */
        {
            if ((otg_status->live_se0) && (otg_status->line_stable))
            {
                otg_status->b_conn = FALSE;
                _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_BCON);
                break;
            }
            if (otg_status->a_bus_req)
            {
                _usb_otg_callback_generate_resume(otg_handle, TRUE);
                _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_SUSPEND_RESUME);
            }
        }
        break;
    case USB_OTG_SM_A_SUSPEND_RESUME:
        if (!otg_status->vbus_valid)
        {
            _usb_otg_callback_generate_resume(otg_handle, FALSE);
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_VBUS_ERR);
            OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_VBUS_ERR);
            break;
        }
        if ((otg_status->id) || (otg_status->a_bus_drop))
        {
            _usb_otg_callback_generate_resume(otg_handle, FALSE);
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_VFALL);
            break;
        }
        if (otg_status->b_timeout_en == (uint8_t) FALSE)
        {
            _usb_otg_callback_generate_resume(otg_handle, FALSE);
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_HOST);
            OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_HOST);
        }
        break;
    case USB_OTG_SM_A_PERIPHERAL:
        if (!otg_status->vbus_valid)
        {
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_VBUS_ERR);
            OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_VBUS_ERR);
            break;
        }
        if ((otg_status->id) || (otg_status->a_bus_drop))
        {
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_VFALL);
            if (otg_status->id)
            {
                OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_ID_TRUE);
            }
            break;
        }
        if ((otg_status->live_jstate) && (otg_status->ms_since_line_changed >= TA_BIDL_ADIS))
        {
            /* the bus is idle for TA_BIDL_ADIS. This is the last stage of HNP */
            /* A device leaves peripheral state */
            _usb_otg_a_change_state(otg_handle, USB_OTG_SM_A_WAIT_BCON);
            OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_BIDL_ADIS_TMOUT);
        }
        break;
    default:
        break;
    }
}

/* Private functions *********************************************************/

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _usb_otg_a_change_state
 * Returned Value   :
 * Comments         : This function handles the actions performed at A substate change
 *    
 *
 *END*----------------------------------------------------------------------*/
static void _usb_otg_a_change_state
(
    usb_otg_handle otg_handle,
    uint8_t new_state
    )
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t *otg_status = &usb_otg_struct_ptr->otg_status;

    if (usb_otg_struct_ptr->sub_state == USB_OTG_SM_A_HOST)
    {
        _usb_otg_unload_active(otg_handle);
    }
    if (usb_otg_struct_ptr->sub_state == USB_OTG_SM_A_PERIPHERAL)
    {

        _usb_otg_unload_active(otg_handle);
        _usb_otg_callback_set_dp_pull_up(otg_handle, FALSE);
        _usb_otg_callback_set_pull_downs(otg_handle, OTG_CTRL_PDOWN_DP | OTG_CTRL_PDOWN_DM);
    }

    switch(new_state)
    {
    case USB_OTG_SM_A_IDLE:
        usb_otg_struct_ptr->sub_state = USB_OTG_SM_A_IDLE;
        otg_status->a_srp_det_state = srp_not_started;
        otg_status->b_timeout_en = FALSE;
        otg_status->b_timeout = 0;
        OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_IDLE);
        break;
    case USB_OTG_SM_A_WAIT_VRISE:
        usb_otg_struct_ptr->sub_state = USB_OTG_SM_A_WAIT_VRISE;
        _usb_otg_callback_set_vbus(otg_handle, TRUE);
        otg_status->b_timeout_en = FALSE;
        otg_status->b_timeout = TA_VBUS_RISE;
        otg_status->b_timeout_en = TRUE;
        OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_WAIT_VRISE);
        break;
    case USB_OTG_SM_A_WAIT_BCON:
        if (usb_otg_struct_ptr->sub_state == USB_OTG_SM_A_WAIT_VRISE)
        {
            otg_status->b_conn_dbnc_time = TA_BCON_LDB;
        }
        else
        {
            otg_status->b_conn_dbnc_time = TA_BCON_SDB;
        }
        usb_otg_struct_ptr->sub_state = USB_OTG_SM_A_WAIT_BCON;
        otg_status->a_set_b_hnp_en = 0;
        otg_status->srp_support = 0;
        otg_status->hnp_support = 0;
        otg_status->host_req_poll_timer = 0;
        otg_status->b_timeout_en = FALSE;
        otg_status->b_timeout = TA_WAIT_BCON;
        otg_status->b_timeout_en = TRUE;
        OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_WAIT_BCON);
        break;
    case USB_OTG_SM_A_VBUS_ERR:
        usb_otg_struct_ptr->sub_state = USB_OTG_SM_A_VBUS_ERR;
        _usb_otg_callback_set_vbus(otg_handle, FALSE);
        otg_status->b_conn = FALSE;
        otg_status->b_timeout_en = FALSE;
        otg_status->b_timeout = 0;
        OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_VBUS_ERR);
        break;
    case USB_OTG_SM_A_WAIT_VFALL:
        usb_otg_struct_ptr->sub_state = USB_OTG_SM_A_WAIT_VFALL;
        _usb_otg_callback_set_vbus(otg_handle, FALSE);
        otg_status->b_conn = FALSE;
        otg_status->a_srp_det = FALSE;
        otg_status->a_srp_det_state = srp_not_started;
        otg_status->b_timeout_en = FALSE;
        otg_status->b_timeout = TA_VBUS_FALL;
        otg_status->b_timeout_en = TRUE;
        OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_WAIT_VFALL);
        break;
    case USB_OTG_SM_A_HOST:
        usb_otg_struct_ptr->sub_state = USB_OTG_SM_A_HOST;
        otg_status->hnp_req = 0;
        otg_status->srp_support = 0;
        otg_status->hnp_support = 0;
        otg_status->a_set_b_hnp_en = 0;
        otg_status->b_timeout_en = FALSE;
        otg_status->b_timeout = 0;
        {
            uint32_t error;
            error = _usb_otg_load_host(otg_handle);
            if (error == USB_OK)
            {
                OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_HOST);
            }
            else
            {
                OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_HOST_LOAD_ERROR);
            }
        }
        break;
    case USB_OTG_SM_A_SUSPEND:
        usb_otg_struct_ptr->sub_state = USB_OTG_SM_A_SUSPEND;
        otg_status->b_timeout_en = FALSE;
        otg_status->b_timeout = 0;
        if (otg_status->a_set_b_hnp_en)
        {
            otg_status->b_timeout = TA_AIDL_BDIS;
            otg_status->b_timeout_en = TRUE;
        }
        OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_SUSPEND);
        break;
    case USB_OTG_SM_A_SUSPEND_RESUME:
        usb_otg_struct_ptr->sub_state = USB_OTG_SM_A_SUSPEND_RESUME;
        otg_status->b_timeout_en = FALSE;
        otg_status->b_timeout = TA_SUSPEND_RESUME;
        otg_status->b_timeout_en = TRUE;
        break;
    case USB_OTG_SM_A_PERIPHERAL:
        usb_otg_struct_ptr->sub_state = USB_OTG_SM_A_PERIPHERAL;
        otg_status->b_timeout_en = FALSE;
        otg_status->b_timeout = 0;
        {
            uint32_t error;
            error = _usb_otg_load_device(otg_handle);
            if (error == USB_OK)
            {
                OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_PERIPHERAL);
            }
            else
            {
                OS_Event_set(usb_otg_struct_ptr->otg_app_event, OTG_A_PERIPHERAL_LOAD_ERROR);
            }
        }
        break;
    default:
        break;
    }
}
