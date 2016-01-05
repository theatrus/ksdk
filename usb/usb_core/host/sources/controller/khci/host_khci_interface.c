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
 * $FileName: khci_interface.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *
 *END************************************************************************/
#include "usb_host_config.h"
#if USBCFG_HOST_KHCI
#include "usb.h"
#include "usb_host_stack_interface.h"

#include "usb_host_common.h"
#include "usb_host.h"

#include "khci.h"
#include "khci_prv.h"

#include "usb_host_dev_mng.h"
#include "fsl_usb_khci_hal.h"
#if defined(__cplusplus)
extern const struct usb_host_api_functions_struct _usb_khci_host_api_table =
#else
const struct usb_host_api_functions_struct _usb_khci_host_api_table =
#endif
{
    /* The Host/Device preinit function */
    usb_khci_preinit,

    /* The Host/Device init function */
    usb_khci_init,

    /* The function to shutdown the host/device */
    usb_khci_shutdown,

    /* The function to send data */
    usb_khci_send,

    /* The function to send setup data */
    usb_khci_send_setup,

    /* The function to receive data */
    usb_khci_recv,

    /* The function to cancel the transfer */
    usb_khci_cancel_pipe,

    /* The function for USB bus control */
    usb_khci_bus_control,

    /* The function for alloc bandwidth */
    NULL,

    /* The function for free controller resource */
    NULL,

    /* The function for get frame_number */
    usb_khci_get_frame_number,

    /* The function for get micro frame_number */
    NULL,

    /* The function for open pipe */
    usb_khci_open_pipe,

    /* The function to close pipe */
    usb_khci_close_pipe,

    NULL,

    NULL
};
#endif

