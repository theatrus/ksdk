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
 * $FileName: usb_host_msd_ufi.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file creates the mass storage class UFI command headers
 *
 *END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_dev_mng.h"
#include "usb_host_ch9.h"
#include "usb_host_common.h"
#include "usb_host_msd_bo.h"
#include "usb_host_msd_ufi.h"
#include "usb_host.h"

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_mass_ufi_generic
 * Returned Value : None
 * Comments       :
 *     This function initializes the mass storage class
 *
 *END*--------------------------------------------------------------------*/

usb_status usb_mass_ufi_generic
(
      /* [IN] command object allocated by application*/
      mass_command_struct_t*      cmd_ptr,
      uint8_t                     opcode,
      uint8_t                     lun,
      uint32_t                    lbaddr,
      uint32_t                    blen,

      uint8_t                     cbwflags,

      uint8_t *                   buf,
      uint32_t                    buf_len
)
{ /* Body */
    ufi_cbwcb_extended_struct_t* ufi_ptr = NULL;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_mass_ufi_generic");
#endif

    ufi_ptr = (ufi_cbwcb_extended_struct_t*) &(cmd_ptr->CBW_PTR->CMD_BLOCK);

    /* Construct UFI command buffer */
    ufi_ptr->BUFIOPCODE = opcode;
    ufi_ptr->BUFILUN = lun;
    USB_HOST_TO_BE_UNALIGNED_LONG(lbaddr, ufi_ptr->BUFILOGICALBLOCKADDRESS)
    USB_HOST_TO_BE_UNALIGNED_LONG(blen, ufi_ptr->BLENGTH)

    /* Construct CBW fields (sig and tag will be filled up by class driver)*/
    /* (uint_32_ptr)cmd_ptr->CBW_PTR->DCBWDATATRANSFERLENGTH = USB_HOST_TO_LE_LONG(buf_len); */
    USB_HOST_TO_LE_UNALIGNED_LONG(buf_len, cmd_ptr->CBW_PTR->DCBWDATATRANSFERLENGTH)
    cmd_ptr->CBW_PTR->BMCBWFLAGS = cbwflags;
    TRANSFER_LOW_NIBBLE(cmd_ptr->LUN, cmd_ptr->CBW_PTR->BCBWLUN);
    TRANSFER_LOW_NIBBLE(sizeof(ufi_cbwcb_struct_t) - 2,
        cmd_ptr->CBW_PTR->BCBWCBLENGTH);

    /* Construct Command object */
    cmd_ptr->DATA_BUFFER = buf;
    cmd_ptr->BUFFER_LEN = buf_len;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_mass_ufi_generic, SUCCESSFUL");
#endif

    /* Pass this request to class driver */
    return usb_class_mass_storage_device_command(cmd_ptr);
} /* Endbody */

bool usb_mass_ufi_cancel
(
    mass_command_struct_t* cmd_ptr
    )
{ /* Body */
    return usb_class_mass_storage_device_command(cmd_ptr);
} /* Endbody */
/* EOF */
