/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 - 2015 Freescale Semiconductor;
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
 * $FileName: usb_otg.c$
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

#include "usb_otg.h"

/* Prototypes of functions */
static usb_status _usb_otg_preinit(usb_otg_handle* handle);
static usb_status _usb_otg_init(uint8_t controller_id, usb_otg_handle handle);
static usb_status _usb_otg_shutdown(usb_otg_handle handle);
static usb_status _usb_otg_get_status(usb_otg_handle handle);
static usb_status _usb_otg_set_vbus(usb_otg_handle handle, bool enable);
static usb_status _usb_otg_set_pull_downs(usb_otg_handle handle, uint8_t bitfield);
static usb_status _usb_otg_set_dp_pull_up(usb_otg_handle handle, bool enable);
static usb_status _usb_otg_generate_resume(usb_otg_handle handle, bool enable);

extern uint32_t soc_get_usb_base_address(uint8_t controller_id);

extern const usb_otg_peripheral_functions_struct_t g_usb_otg_max3353_callback_table;

extern const usb_otg_api_functions_struct_t g_usb_otg_callback_table = {
    /* The otg preinit function */
    _usb_otg_preinit,

    /* The otg init function */
    _usb_otg_init,

    /* The function to shutdown the otg */
    _usb_otg_shutdown,

    /* The function to get the otg status */
    _usb_otg_get_status,

    /* The function to control the bus voltage */
    _usb_otg_set_vbus,

    /* The function to control the pull downs */
    _usb_otg_set_pull_downs,

    /* The function to control the dp pull up */
    _usb_otg_set_dp_pull_up,

    /* The function to start/stop resume signalling */
    _usb_otg_generate_resume

};

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_get_peripheral_api
 *  Returned Value :
 *    
 *
 *  Comments       :
 *  
 *
 *END*-----------------------------------------------------------------*/
static void _usb_otg_get_peripheral_api(uint8_t controller_id, const usb_otg_peripheral_functions_struct_t * * controller_api_ptr)
{
    if (controller_id == USB_OTG_PERIPHERAL_MAX3353)
    {
        *controller_api_ptr = (const usb_otg_peripheral_functions_struct_t *) &g_usb_otg_max3353_callback_table;
    }
    else
    {
        *controller_api_ptr = NULL;
    }
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_preinit
 *  Returned Value : 
 *  Comments       :
 *       
 *END*-----------------------------------------------------------------*/
static usb_status _usb_otg_preinit
(
    usb_otg_handle* handle
    )
{
    usb_otg_struct_t * otg_ptr;
    void * khci_call_struct_ptr;

    otg_ptr = (usb_otg_struct_t *) OS_Mem_alloc_zero(sizeof(usb_otg_struct_t));

    if (otg_ptr == NULL)
    {
        return USBERR_ALLOC;
    }

    _usb_otg_get_peripheral_api(USB_OTG_PERIPHERAL_ID, &otg_ptr->peripheral_api_list_ptr);
    if (otg_ptr->peripheral_api_list_ptr == NULL)
    {
        OS_Mem_free(otg_ptr);
        return USBERR_ERROR;
    }

    khci_call_struct_ptr = (void*) OS_Mem_alloc_zero(sizeof(usb_otg_khci_call_struct_t));
    if (khci_call_struct_ptr == NULL)
    {
        OS_Mem_free(otg_ptr);
        otg_ptr = NULL;
        return USBERR_ALLOC;
    }

    otg_ptr->khci_call_ptr = (usb_otg_khci_call_struct_t *) khci_call_struct_ptr;

    if (NULL != otg_ptr->peripheral_api_list_ptr->otg_peripheral_preinit)
    {
        otg_ptr->peripheral_api_list_ptr->otg_peripheral_preinit(otg_ptr, &otg_ptr->peripheral_call_ptr);
        if (khci_call_struct_ptr == NULL)
        {
            OS_Mem_free(khci_call_struct_ptr);
            OS_Mem_free(otg_ptr);
            otg_ptr = NULL;
            return USBERR_ALLOC;
        }
    }

    *handle = otg_ptr;

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_init
 *  Returned Value : 
 *  Comments       :
 *       
 *END*-----------------------------------------------------------------*/
static usb_status _usb_otg_init
(
    uint8_t controller_id,
    usb_otg_handle handle
    )
{
    /* Initialize the OTG controller */
    usb_status error;
    usb_otg_struct_t * otg_handle_ptr = (usb_otg_struct_t *) handle;
    usb_otg_state_struct_t * otg_state_handle_ptr = &(otg_handle_ptr->g);

    otg_handle_ptr->khci_call_ptr->otg_handle_ptr = otg_state_handle_ptr;
    otg_state_handle_ptr->usbRegBase = soc_get_usb_base_address(controller_id);
    error = _usb_otg_khci_init(controller_id, otg_handle_ptr->khci_call_ptr);
    if (error != USB_OK)
    {
        return error;
    }

    if (NULL != otg_handle_ptr->peripheral_api_list_ptr->otg_peripheral_init)
    {
        error = otg_handle_ptr->peripheral_api_list_ptr->otg_peripheral_init(otg_handle_ptr->peripheral_call_ptr);
        if (error != USB_OK)
        {
            return error;
        }
    }
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_shutdown
 *  Returned Value : 0 successful
 *  Comments       :
 *       
 *END*-----------------------------------------------------------------*/
static usb_status _usb_otg_shutdown
(
    usb_otg_handle handle
    )
{
    usb_status error;
    usb_otg_struct_t * otg_handle_ptr = (usb_otg_struct_t *) handle;
    error = _usb_otg_khci_shut_down(otg_handle_ptr->khci_call_ptr);
    if (error != USB_OK)
    {
        return error;
    }

    if (NULL != otg_handle_ptr->peripheral_api_list_ptr->otg_peripheral_shutdown)
    {
        error = otg_handle_ptr->peripheral_api_list_ptr->otg_peripheral_shutdown(otg_handle_ptr->peripheral_call_ptr);
        if (error != USB_OK)
        {
            return error;
        }
    }
    OS_Mem_free(otg_handle_ptr->khci_call_ptr);
    otg_handle_ptr->khci_call_ptr = NULL;
    otg_handle_ptr->peripheral_call_ptr = NULL;
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_get_status
 *  Returned Value : 0 successful
 *  Comments       :
 *       
 *END*-----------------------------------------------------------------*/
static usb_status _usb_otg_get_status
(
    usb_otg_handle handle
    )
{
    usb_otg_struct_t * otg_handle_ptr = (usb_otg_struct_t *) handle;
    usb_otg_state_struct_t * usb_otg_struct_ptr = (usb_otg_state_struct_t *) handle;

    /* *** Handle the events *** */
    if (OS_Event_check_bit(usb_otg_struct_ptr->otg_isr_event, USB_OTG_MAX3353_ISR_EVENT))
    {
        OS_Event_clear(usb_otg_struct_ptr->otg_isr_event, USB_OTG_MAX3353_ISR_EVENT);
        if (NULL != otg_handle_ptr->peripheral_api_list_ptr->otg_peripheral_get_status)
        {
            otg_handle_ptr->peripheral_api_list_ptr->otg_peripheral_get_status(otg_handle_ptr->peripheral_call_ptr);
        }
    }
    if (OS_Event_check_bit(usb_otg_struct_ptr->otg_isr_event, USB_OTG_KHCI_ISR_EVENT))
    {
        OS_Event_clear(usb_otg_struct_ptr->otg_isr_event, USB_OTG_KHCI_ISR_EVENT);
        _usb_otg_khci_get_status(otg_handle_ptr->khci_call_ptr);
    }
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_set_vbus
 *  Returned Value : none
 *  Comments       :
 *        
 *END*-----------------------------------------------------------------*/
static usb_status _usb_otg_set_vbus
(
    usb_otg_handle handle,
    bool enable
    )
{
    usb_otg_struct_t * otg_handle_ptr = (usb_otg_struct_t *) handle;
    if (NULL != otg_handle_ptr->peripheral_api_list_ptr->otg_peripheral_set_vbus)
    {
        return otg_handle_ptr->peripheral_api_list_ptr->otg_peripheral_set_vbus(otg_handle_ptr->peripheral_call_ptr, enable);
    }

    return USBERR_ERROR;
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_set_pull_downs
 *  Returned Value : none
 *  Comments       :
 *        
 *END*-----------------------------------------------------------------*/
static usb_status _usb_otg_set_pull_downs
(
    usb_otg_handle handle,
    uint8_t bitfield
    )
{
    usb_status error;
    usb_otg_struct_t * otg_handle_ptr = (usb_otg_struct_t *) handle;
    error = _usb_otg_khci_set_pull_downs(otg_handle_ptr->khci_call_ptr, bitfield);
    if (error != USB_OK)
    {
        return error;
    }
    if (NULL != otg_handle_ptr->peripheral_api_list_ptr->otg_peripheral_set_pulldowns)
    {
        return otg_handle_ptr->peripheral_api_list_ptr->otg_peripheral_set_pulldowns(otg_handle_ptr->peripheral_call_ptr, bitfield);
    }
    return USBERR_ERROR;

}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_set_pull_downs
 *  Returned Value : none
 *  Comments       :
 *        
 *END*-----------------------------------------------------------------*/
static usb_status _usb_otg_set_dp_pull_up
(
    usb_otg_handle handle,
    bool enable
    )
{
    usb_otg_struct_t * otg_handle_ptr = (usb_otg_struct_t *) handle;
    return _usb_otg_khci_set_dp_pull_up(otg_handle_ptr->khci_call_ptr, enable);
}
/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : _usb_otg_generate_resume
 *  Returned Value : none
 *  Comments       :
 *        
 *END*-----------------------------------------------------------------*/
static usb_status _usb_otg_generate_resume
(
    usb_otg_handle handle,
    bool enable
    )
{
    usb_otg_struct_t * otg_handle_ptr = (usb_otg_struct_t *) handle;
    return _usb_otg_khci_generate_resume(otg_handle_ptr->khci_call_ptr, enable);
}
