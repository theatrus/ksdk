/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2015 Freescale Semiconductor;
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
 * $FileName: usb_otg_khci.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *         
 *****************************************************************************/
#include "usb.h"
#include "usb_otg_main.h"
#include "usb_otg_private.h"
#include "usb_otg_khci.h"

/* Functions and parameters to handles host, device interrupt*/
/* Include file */
#include "usb_device_config.h"
#include "khci_dev_misc.h"
#include "usb_otg_sm.h"
#include "usb_host_config.h"
#include "usb_host_stack_interface.h"
#include "usb_host.h"
#include "fsl_usb_khci_hal.h"

/* Functions */
extern void* bsp_usb_otg_get_init_param(uint8_t controller_id);
extern void _usb_dev_khci_isr(usb_khci_dev_state_struct_t* state_ptr);
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM))             /* USB stack running on MQX */
extern void _usb_host_khci_isr(usb_host_handle handle);
#endif
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
extern void _usb_host_khci_isr(void);
#endif
/* Parameters */
extern usb_host_handle host_handle;
/* global ogt_khci_call_ptr */
usb_otg_khci_call_struct_t * g_otg_khci_call_ptr;

/* Prototypes of functions */

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_khci_isr
 *  Returned Value : None
 *  Comments       :
 *        Service all the interrupts in the kirin usb hardware
 *END*-----------------------------------------------------------------*/
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM))       
static void _usb_otg_khci_isr
(
    void* otg_khci_call_ptr
    )
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
static void _usb_otg_khci_isr
(
void
)
#endif
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = ((usb_otg_khci_call_struct_t *) g_otg_khci_call_ptr)->otg_handle_ptr;
    if (usb_otg_struct_ptr != NULL)
    {
        uint8_t otg_int_stat = (uint8_t) usb_hal_khci_get_otg_interrupt_status(usb_otg_struct_ptr->usbRegBase);
        uint8_t otg_stat = usb_hal_khci_get_otg_status(usb_otg_struct_ptr->usbRegBase);
        usb_otg_status_t *otg_status = &usb_otg_struct_ptr->otg_status;
        /* Handle otg interrupt*/
        if (otg_int_stat != 0)
        {
            /* Clear the OTG relevant interrupts */
            usb_hal_khci_clr_otg_interrupt(usb_otg_struct_ptr->usbRegBase, otg_int_stat);
            if (otg_int_stat & USB_OTGISTAT_ONEMSEC_MASK)
            {
                otg_status->tmr_1ms = TRUE;
                if (otg_status->ms_since_line_changed < 0xffff)
                {
                    otg_status->ms_since_line_changed++;
                }
                if (otg_status->host_req_poll_timer < 0xffff)
                {
                    otg_status->host_req_poll_timer++;
                }
                /* Decrement timeouts */
                if ((otg_status->b_timeout_en) && (otg_status->b_timeout))
                {
                    otg_status->b_timeout--;
                    if (!otg_status->b_timeout)
                    {
                        otg_status->b_timeout_en = FALSE;
                    }
                }
            }
            if ((otg_int_stat & USB_OTGISTAT_LINE_STATE_CHG_MASK) || (!(otg_stat & USB_OTGSTAT_LINESTATESTABLE_MASK)))
            {
                otg_status->ms_since_line_changed = 0;
            }
        }
        /* Check active stack*/
        if (otg_status->active_stack == USB_ACTIVE_STACK_HOST)
        {
            /* Host stack is current active stack */
            /* Call function to handle host interrupt*/
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM))             /* USB stack running on MQX */
            _usb_host_khci_isr (NULL);
#endif
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
            _usb_host_khci_isr();
#endif
        }
        else if (otg_status->active_stack == USB_ACTIVE_STACK_DEVICE)
        {
            /* Device stack is current active stack */
            /* Call function to handle device interrupt*/
            if (usb_otg_struct_ptr->dev_inst_ptr != NULL)
            {
                _usb_dev_khci_isr((usb_khci_dev_state_struct_t*) usb_otg_struct_ptr->dev_inst_ptr);
            }

        }
        else
        {
            /* Clear all pending USB interrupt*/
            usb_hal_khci_clr_all_interrupts(usb_otg_struct_ptr->usbRegBase);
        }
        OS_Event_set(usb_otg_struct_ptr->otg_isr_event, USB_OTG_KHCI_ISR_EVENT);
    }
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_khci_init
 *  Returned Value : status
 *  Comments       :
 *        Initialize khci otg hardware and khci otg interrupt
 *END*-----------------------------------------------------------------*/
usb_status _usb_otg_khci_init
(
    uint8_t controller_id,
    usb_otg_khci_call_struct_t * otg_khci_call_ptr
    )
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = ((usb_otg_khci_call_struct_t *) otg_khci_call_ptr)->otg_handle_ptr;
    usb_hal_khci_clr_all_interrupts(usb_otg_struct_ptr->usbRegBase);
    usb_hal_khci_clr_all_otg_interrupts(usb_otg_struct_ptr->usbRegBase);
    /* Enable the OTG mode */
    usb_hal_khci_enable_otg(usb_otg_struct_ptr->usbRegBase);
    usb_hal_khci_clr_usbtrc0(usb_otg_struct_ptr->usbRegBase);
    /* Enable the OTG Interrupts */
    usb_hal_khci_enable_otg_interrupts(usb_otg_struct_ptr->usbRegBase, USB_OTGICR_ONEMSECEN_MASK | USB_OTGICR_LINESTATEEN_MASK);
    otg_khci_call_ptr->init_param_ptr = (usb_khci_otg_int_struct_t*) bsp_usb_otg_get_init_param(controller_id);
    g_otg_khci_call_ptr = otg_khci_call_ptr;
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)) 
    if (!OS_install_isr(otg_khci_call_ptr->init_param_ptr->vector, (void (*)(void *))_usb_otg_khci_isr, (void*)otg_khci_call_ptr))
    {
        return USBERR_INSTALL_ISR;
    }
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
    OS_install_isr(otg_khci_call_ptr->init_param_ptr->vector,(void(*)(void))_usb_otg_khci_isr,(void*)otg_khci_call_ptr);
#endif
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_khci_shut_down
 *  Returned Value : status
 *  Comments       :
 *        Initialize khci otg hardware and khci otg interrupt
 *END*-----------------------------------------------------------------*/
usb_status _usb_otg_khci_shut_down
(
    usb_otg_khci_call_struct_t * otg_khci_call_ptr
) 
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = ((usb_otg_khci_call_struct_t *) otg_khci_call_ptr)->otg_handle_ptr;
    usb_hal_khci_clr_all_interrupts(usb_otg_struct_ptr->usbRegBase);
    usb_hal_khci_clr_all_otg_interrupts(usb_otg_struct_ptr->usbRegBase);
    /* disable the OTG Interrupts */
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_khci_shut_down
 *  Returned Value : status
 *  Comments       :
 *        update the OTG status indicators associated with the KHCI hardware   
 *END*-----------------------------------------------------------------*/
usb_status _usb_otg_khci_get_status
(
    usb_otg_khci_call_struct_t * otg_khci_call_ptr
    )
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = otg_khci_call_ptr->otg_handle_ptr;
    usb_otg_status_t * otg_status_ptr = &usb_otg_struct_ptr->otg_status;

    /* Update status */
    otg_status_ptr->line_stable = usb_hal_khci_is_line_stable(usb_otg_struct_ptr->usbRegBase);
    if (!otg_status_ptr->line_stable)
    {
        otg_status_ptr->ms_since_line_changed = 0;
    }
    if (otg_status_ptr->line_stable)
    {
        otg_status_ptr->live_se0 = usb_hal_khci_get_se0_status(usb_otg_struct_ptr->usbRegBase);
        if (otg_status_ptr->live_se0)
        {
            otg_status_ptr->live_jstate = 0;
        }
        else
        {
            otg_status_ptr->live_jstate = !usb_hal_khci_get_line_status(usb_otg_struct_ptr->usbRegBase);
        }
    }
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_khci_set_pull_downs
 *  Returned Value : status
 *  Comments       :
 *        update the OTG status indicators associated with the KHCI hardware   
 *END*-----------------------------------------------------------------*/
usb_status _usb_otg_khci_set_pull_downs
(
    usb_otg_khci_call_struct_t * otg_khci_call_ptr,
    uint8_t bitfield
    )
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = otg_khci_call_ptr->otg_handle_ptr;
    usb_hal_khci_set_pull_downs(usb_otg_struct_ptr->usbRegBase, bitfield);
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_khci_set_pull_downs
 *  Returned Value : status
 *  Comments       :
 *        update the OTG status indicators associated with the KHCI hardware   
 *END*-----------------------------------------------------------------*/
usb_status _usb_otg_khci_set_dp_pull_up
(
    usb_otg_khci_call_struct_t * otg_khci_call_ptr,
    bool enable
    )
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = otg_khci_call_ptr->otg_handle_ptr;
    if (enable)
    {
        usb_hal_khci_enable_dp_pull_up(usb_otg_struct_ptr->usbRegBase);
    }
    else
    {
        usb_hal_khci_disable_dp_pull_up(usb_otg_struct_ptr->usbRegBase);
    }
    return USB_OK;
}
/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_khci_set_pull_downs
 *  Returned Value : status
 *  Comments       :
 *        update the OTG status indicators associated with the KHCI hardware   
 *END*-----------------------------------------------------------------*/
usb_status _usb_otg_khci_generate_resume
(
    usb_otg_khci_call_struct_t * otg_khci_call_ptr,
    bool enable
    )
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = otg_khci_call_ptr->otg_handle_ptr;
    if (enable)
    {
        usb_hal_khci_start_resume(usb_otg_struct_ptr->usbRegBase);
    }
    else
    {
        usb_hal_khci_stop_resume(usb_otg_struct_ptr->usbRegBase);
    }
    return USB_OK;
}
