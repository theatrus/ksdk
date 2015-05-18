/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013- 2014 Freescale Semiconductor;
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
 * $FileName: usb_host_hub.h$
 * $Version : 3.8.11.0$
 * $Date    : Jun-7-2012$
 *
 * Comments:
 *
 *   This file defines a structure(s) for hub class driver.
 *
 *END************************************************************************/
#ifndef __usb_host_hub_h__
#define __usb_host_hub_h__

#if 0
#define  USB_CLASS_HUB              9
#define  USB_SUBCLASS_HUB_NONE      0
#define  USB_PROTOCOL_HUB_LS        0
#define  USB_PROTOCOL_HUB_FS        0
#define  USB_PROTOCOL_HUB_HS_SINGLE 1
#define  USB_PROTOCOL_HUB_HS_MULTI  2
#define  USB_PROTOCOL_HUB_ALL       0xFF
#endif

/*
 ** HUB Class Interface structure. This structure will be passed to
 ** all commands to this class driver.
 */
typedef struct
{
    /* Each class must start with a GENERAL_CLASS struct */

    usb_host_handle host_handle;
    usb_device_instance_handle dev_handle;
    usb_interface_descriptor_handle intf_handle;

    /* Pipes */
    usb_pipe_handle control_pipe;
    usb_pipe_handle interrupt_pipe;

    /* Higher level callback and parameter */
    tr_callback ctrl_callback;
    void* ctrl_param;

    /* Higher level callback and parameter */
    tr_callback interrupt_callback;
    void* interrupt_param;

    /* Is command used? (only 1 command can be issued at one time) */
    bool in_setup;
    /* Only 1 interrupt pipe can be opened at one time */
    bool in_interrupt;
    //uint32_t                                   control_buf_size;
    //uint32_t                                   interrupt_buf_size;
    /* control pipe buffer pointer, allocated in memlist */
    //uint8_t *                                control_buf;
    /* interrupt pipe buffer pointer, allocated in memlist */
    //uint8_t *                                interrupt_buf;
    uint8_t level;

} usb_hub_class_struct_t;

typedef struct
{
    usb_class_handle class_ptr;
    tr_callback callback_fn;
    void* callback_param;
} hub_command_t;

/* Class specific functions exported by hub class driver */
#ifdef __cplusplus
extern "C"
{
#endif

extern usb_status usb_class_hub_init
(
    usb_device_instance_handle      dev_handle,
    usb_interface_descriptor_handle intf_handle,
    usb_class_handle*               class_handle_ptr
);
extern usb_status usb_class_hub_deinit(usb_class_handle handle);
extern usb_status usb_class_hub_pre_deinit(usb_class_handle handle);

#ifdef __cplusplus
}
#endif

#endif
