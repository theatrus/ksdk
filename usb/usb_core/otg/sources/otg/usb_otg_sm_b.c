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
 * $FileName: usb_otg_sm_b.c$
 * $Version : 
 * $Date    : 
 *
 * Comments : This file contains the implementation of the OTG state machine
 *
 *         
 *****************************************************************************/
#include "usb.h"
#include "usb_device_config.h"
#include "usb_otg_main.h"
#include "usb_otg_private.h"
#include "usb_otg_sm.h"
#include "usb_device_stack_interface.h"
/* Constant Definitions*********************************************************/

/* Type Definitions*********************************************************/

/* Private memory definitions ***********************************************/

/* Private functions prototypes *********************************************/
static void _usb_otg_sm_b_substate_change(usb_otg_handle otg_handle, uint8_t new_state, uint32_t sm_indication);

/* Public functions *********************************************************/

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _usb_otg_sm_b
 * Returned Value   :
 * Comments         : This function handles the substates of the B-state machine
 *    
 *
 *END*----------------------------------------------------------------------*/
void _usb_otg_sm_b
(
    usb_otg_handle otg_handle
    )
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t *otg_status = &usb_otg_struct_ptr->otg_status;

    if (!otg_status->id)
    {
        _usb_otg_id_chg_a(otg_handle);
        return;
    }

    switch(usb_otg_struct_ptr->sub_state)
    {
    case USB_OTG_SM_B_IDLE_SESS_DETECT:
        usb_otg_struct_ptr->hnp_enabled = FALSE;
        if (otg_status->sess_valid)
        {
            _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_PERI_BUS_SUSP_DETECT, OTG_B_PERIPHERAL);
        }
        else
        {
            /* Disable pull-up. The B-Device is now in the B-Idle state */
            _usb_otg_callback_set_dp_pull_up(otg_handle, FALSE);
            if (otg_status->sess_end)
            {
                usb_otg_struct_ptr->sub_state = USB_OTG_SM_B_IDLE_SESS_END_DETECT;

                otg_status->b_timeout = TB_SESSEND_SRP; /* Program the SRP detect timeout as SESSEND SRP */
            }
        }
        break;
    case USB_OTG_SM_B_IDLE_SESS_END_DETECT:
        if (otg_status->sess_valid)
        {
            _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_PERI_BUS_SUSP_DETECT, OTG_B_PERIPHERAL);
        }
        else
        {
            /* Read the Live SE0 bit */
            if ((otg_status->live_se0) && (otg_status->line_stable))
            {
                otg_status->b_timeout_en = TRUE;

                usb_otg_struct_ptr->sub_state = USB_OTG_SM_B_IDLE_SE0_STABLE_WAIT;
            }
        }
        break;
    case USB_OTG_SM_B_IDLE_SE0_STABLE_WAIT:
        if (otg_status->sess_valid)
        {
            _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_PERI_BUS_SUSP_DETECT, OTG_B_PERIPHERAL);
        }
        else
        {
            /* Line state change. restart SE0 detection */
            if (!otg_status->line_stable)
            {
                otg_status->b_timeout_en = TRUE;

                otg_status->b_timeout = TB_SE0_SRP; /* reinitialize the the SE0 detect timer */

                /* Keep the current state */
            }
            else
            {
                if (otg_status->b_timeout == 0)
                {
                    /* The timeout expired during stable SE0 */
                    _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_IDLE_SRP_START_ARMED, OTG_B_IDLE_SRP_READY);
                }
            }
        }
        break;

    case USB_OTG_SM_B_IDLE_SRP_START_ARMED:
        if (otg_status->sess_valid)
        {
            _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_PERI_BUS_SUSP_DETECT, OTG_B_PERIPHERAL);
        }
        else
        {
            /* Line state change. restart SE0 detection */
            if (!otg_status->line_stable)
            {
                otg_status->b_timeout = TB_SE0_SRP; /* reinitialize the the SE0 detect timer */
                usb_otg_struct_ptr->sub_state = USB_OTG_SM_B_IDLE_SESS_END_DETECT;
            }
            else
            {
                if ((usb_otg_struct_ptr->srp_request) || (usb_otg_struct_ptr->power_up))
                {
                    usb_otg_struct_ptr->power_up = FALSE;
                    usb_otg_struct_ptr->srp_request = FALSE;
                    /* Start SRP */
                    /* Start the D+ pulsing timeout */
                    otg_status->b_timeout = TB_DATA_PLS; /* reinitialize the the SE0 detect timer */
                    otg_status->b_timeout_en = TRUE;
                    /* Enable D+ pullup */
                    _usb_otg_callback_set_dp_pull_up(otg_handle, TRUE);
                    usb_otg_struct_ptr->sub_state = USB_OTG_SM_B_SRP_PULSE;
                }
            }
        }
        break;
    case USB_OTG_SM_B_SRP_PULSE:
        if (otg_status->b_timeout == 0)
        {
            /* The timeout expired. Remove the D+ pullup */
            _usb_otg_callback_set_dp_pull_up(otg_handle, FALSE);
            /* Wait for VBUS */
            otg_status->b_timeout = (TB_SRP_FAIL - TB_DATA_PLS);
            otg_status->b_timeout_en = TRUE;

            usb_otg_struct_ptr->sub_state = USB_OTG_SM_B_SRP_VBUS_WAIT;

            /* Signal the event to the application */
            OS_Event_set((usb_otg_struct_ptr->otg_app_event), OTG_B_SRP_INIT);
        }
        break;

    case USB_OTG_SM_B_SRP_VBUS_WAIT:
        if (otg_status->sess_valid)
        {
            _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_PERI_BUS_SUSP_DETECT, OTG_B_PERIPHERAL);
        }
        else
        {
            if (otg_status->b_timeout == 0)
            {
                /* The timeout expired during VBUS wait */

                /* Inform the application about the failed SRP and return to idle state */
                /* and wait for SE0 condition on the bus to be able to restart the SRP */

                usb_otg_struct_ptr->sub_state = USB_OTG_SM_B_IDLE_SE0_STABLE_WAIT;

                otg_status->b_timeout_en = TRUE;

                otg_status->b_timeout = TB_SE0_SRP; /* reinitialize the the SE0 detect timer */

                /* Signal the event to the application */
                OS_Event_set((usb_otg_struct_ptr->otg_app_event), OTG_B_SRP_FAIL);
            }
        }
        break;
    case USB_OTG_SM_B_PERI_BUS_SUSP_DETECT:
        if (!otg_status->sess_valid)
        {
            _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_IDLE_SESS_DETECT, OTG_B_IDLE);
        }
        else
        {
            /* Enable the D+ pull-up. the B device is in the peripheral state */
            _usb_otg_callback_set_dp_pull_up(otg_handle, TRUE);
            usb_otg_struct_ptr->power_up = FALSE;

            /* Start monitoring the data lines. 
             * If the bus has inactivity for more than TB_AIDL_BDIS, then the A is considered disconnected and B can start HNP
             */
            usb_otg_struct_ptr->sub_state = USB_OTG_SM_B_PERI_BUS_SUSP_WAIT;
            otg_status->b_timeout_en = TRUE;
            otg_status->b_timeout = TB_AIDL_BDIS; /* reinitialize the IDLE detect timer */
        }
        break;
    case USB_OTG_SM_B_PERI_BUS_SUSP_WAIT:
        if (!otg_status->sess_valid)
        {
            _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_IDLE_SESS_DETECT, OTG_B_IDLE);
        }
        else
        {
            if (!otg_status->line_stable)
            {
                /* Restart detection */
                otg_status->b_timeout_en = TRUE;
                otg_status->b_timeout = TB_AIDL_BDIS; /* reinitialize the IDLE detect timer */
            }
            else
            {
                if ((usb_otg_struct_ptr->hnp_enabled) && (otg_status->b_timeout == 0))
                {
                    usb_otg_struct_ptr->sub_state = USB_OTG_SM_B_PERI_HNP_ARMED;
                    /* Signal the event to the application */
                    OS_Event_set((usb_otg_struct_ptr->otg_app_event), OTG_B_PERIPHERAL_HNP_READY);
                }
            }
        }
        break;
    case USB_OTG_SM_B_PERI_HNP_ARMED:
        if (!otg_status->sess_valid)
        {
            _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_IDLE_SESS_DETECT, OTG_B_IDLE);
        }
        else
        {
            if ((!otg_status->line_stable) || (!usb_otg_struct_ptr->hnp_enabled))
            {
                _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_PERI_BUS_SUSP_DETECT, OTG_B_PERIPHERAL_HNP_FAIL);
            }
            else
            {
                if ((usb_otg_struct_ptr->bus_request) && (usb_otg_struct_ptr->hnp_enabled))
                {
                    usb_otg_struct_ptr->bus_request = FALSE;

                    /* Clear the Status at the USB device level */
                    usb_device_set_status(usb_otg_struct_ptr->dev_inst_ptr, USB_STATUS_OTG, USB_STATUS_IDLE);
                    /* Start HNP. Turn off Pull-Up on D+ for the Host to detect SE0 */
                    _usb_otg_callback_set_dp_pull_up(otg_handle, FALSE);
                    /* Wait for data line to discharge (25us) */
                    otg_status->b_timeout = 1;
                    otg_status->b_timeout_en = TRUE;
                    usb_otg_struct_ptr->sub_state = USB_OTG_SM_B_PERI_HNP_START;
                    /* Signal the event to the application */
                    OS_Event_set((usb_otg_struct_ptr->otg_app_event), OTG_B_PERIPHERAL_HNP_START);
                }
            }
        }
        break;
    case USB_OTG_SM_B_PERI_HNP_START:
        if (!otg_status->sess_valid)
        {
            _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_IDLE_SESS_DETECT, OTG_B_IDLE);
        }
        else
        {
            if (otg_status->b_timeout == 0)
            {
                /* Line should have discharged by now */
                /* Start the host disconnect detect */
                otg_status->b_timeout = TB_ASE0_BRST;
                otg_status->b_timeout_en = TRUE;
                usb_otg_struct_ptr->sub_state = USB_OTG_SM_B_PERI_HNP_ACONN;
            }
        }
        break;
    case USB_OTG_SM_B_PERI_HNP_ACONN:
        if (!otg_status->sess_valid)
        {
            _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_IDLE_SESS_DETECT, OTG_B_IDLE);
        }
        else
        {
            if (otg_status->b_timeout == 0)
            {
                /* A connect timeout detected. Go back to peripheral state */
                _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_PERI_BUS_SUSP_DETECT, OTG_B_PERIPHERAL_HNP_FAIL);
            }
            else
            {
                /* Check the Data line state */
                if ((!otg_status->live_se0) && (otg_status->live_jstate) && (otg_status->line_stable))
                {
                    /* J-STATE. Host has been released */
                    /* Enter the B-Host state */
                    usb_otg_struct_ptr->hnp_enabled = FALSE;
                    otg_status->a_conn = TRUE;
                    _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_HOST, OTG_B_HOST);
                }
                else
                {
                    if ((!otg_status->live_se0) && (!otg_status->live_jstate) && (otg_status->line_stable))
                    {
                        /* Host has retained the bus */
                        _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_PERI_BUS_SUSP_DETECT, OTG_B_PERIPHERAL_HNP_FAIL);
                    }
                }
            }
        }
        break;
    case USB_OTG_SM_B_HOST:
        if (!otg_status->sess_valid)
        {
            otg_status->a_conn = FALSE;
            _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_IDLE_SESS_DETECT, OTG_B_IDLE);
        }
        else
        {
            if ((otg_status->a_conn == (uint8_t) FALSE) || (usb_otg_struct_ptr->bus_release))
            {
                usb_otg_struct_ptr->bus_release = FALSE;
                otg_status->a_conn = FALSE;
                /* A-device disconnected or B has finished using the bus */
                _usb_otg_sm_b_substate_change(otg_handle, USB_OTG_SM_B_PERI_BUS_SUSP_DETECT, OTG_B_PERIPHERAL);
            }
            else
            {
                if (usb_otg_struct_ptr->dev_inst_ptr != NULL)
                {
                    if (otg_status->hnp_req)
                    {
                        OS_Event_set((usb_otg_struct_ptr->otg_app_event), OTG_B_A_HNP_REQ);
                    }
                    if ((otg_status->hnp_support) && (otg_status->host_req_poll_timer >= T_HOST_REQ_POLL))
                    {
                        otg_status->host_req_poll_timer = 0;
                        _usb_otg_hnp_poll_req(usb_otg_struct_ptr);
                    }
                }
            }
        }
        break;
    default:
        break;
    }
}

/* Private functions ********************************************************/

/*FUNCTION*-------------------------------------------------------------------
 *
 * Function Name    : _usb_otg_sm_b_substate_change
 * Returned Value   :
 * Comments         : This function handles the actions performed at B substate change
 *    
 *
 *END*------------------------------------------------------------------------*/
static void _usb_otg_sm_b_substate_change
(
    usb_otg_handle otg_handle,
    uint8_t new_state,
    uint32_t sm_indication
    )
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) otg_handle;
    usb_otg_status_t *otg_status = &usb_otg_struct_ptr->otg_status;

    switch(new_state)
    {
    case USB_OTG_SM_B_IDLE_SESS_DETECT:
        _usb_otg_callback_set_dp_pull_up(otg_handle, FALSE);
        /* Unload the active USB stack if any */
        _usb_otg_unload_active(otg_handle);
        _usb_otg_callback_set_pull_downs(otg_handle, 0);
        break;
    case USB_OTG_SM_B_PERI_BUS_SUSP_DETECT:
        _usb_otg_callback_set_dp_pull_up(otg_handle, TRUE);
        if (sm_indication == OTG_B_PERIPHERAL)
        {
            /* Unload the active USB stack if any. Could be that the Host stack is active */
            _usb_otg_unload_active(otg_handle);
            /* Load the Peripheral stack */
            _usb_otg_callback_set_pull_downs(otg_handle, OTG_CTRL_PDOWN_DM);
            if (_usb_otg_load_device(otg_handle) != USB_OK)
            {
                sm_indication = OTG_B_PERIPHERAL_LOAD_ERROR;
            }
        }
        break;
    case USB_OTG_SM_B_HOST:
        otg_status->hnp_req = 0;
        otg_status->srp_support = 0;
        otg_status->hnp_support = 0;
        /* Unload the active USB stack if any. Most probably the peripheral stack is active */
        _usb_otg_unload_active(otg_handle);
        _usb_otg_callback_set_pull_downs(otg_handle, OTG_CTRL_PDOWN_DP | OTG_CTRL_PDOWN_DM);
        /* Load the Host stack */
        if (_usb_otg_load_host(otg_handle) != USB_OK)
        {
            sm_indication = OTG_B_HOST_LOAD_ERROR;
        }
        break;
    default:
        break;
    }
    usb_otg_struct_ptr->sub_state = new_state;
    OS_Event_set((usb_otg_struct_ptr->otg_app_event), sm_indication);
}
