/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
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
 * $FileName: ehci_interface.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *
 *END************************************************************************/

#include "usb_host_config.h"
#if USBCFG_HOST_EHCI
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_common.h"
#include "usb_host.h"
#include "ehci_prv.h"
#include "fsl_usb_ehci_hal.h"
#if defined(__cplusplus)
extern const struct usb_host_api_functions_struct _usb_ehci_host_api_table =
#else
const struct usb_host_api_functions_struct _usb_ehci_host_api_table =
#endif
{
    /* The Host/Device init function */
    usb_ehci_preinit,

    /* The Host/Device init function */
    usb_ehci_init,

    /* The function to shutdown the host/device */
    usb_ehci_shutdown,

    /* The function to send data */
    usb_ehci_send_data,

    /* The function to send setup data */
    usb_ehci_send_setup,

    /* The function to receive data */
    usb_ehci_recv_data,

    /* The function to cancel the transfer */
    usb_ehci_cancel_transfer,

    /* The function for USB bus control */
    usb_ehci_bus_control,

    //_usb_ehci_allocate_bandwidth,
    NULL,

    //_usb_ehci_free_resources,
    NULL,

    usb_ehci_get_frame_number,

    usb_ehci_get_micro_frame_number,

    /* The function to open a pipe */
    usb_ehci_open_pipe,

    /* The function to update the maximum packet size */
    usb_ehci_close_pipe,

    /* The function to update the device address */
    NULL,

    usb_ehci_update_dev_address
};
#endif
