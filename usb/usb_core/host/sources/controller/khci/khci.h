/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 Freescale Semiconductor;
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
 * $FileName: khci.h$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file contains KHCI-specific implementations of USB interfaces
 *
 *END************************************************************************/

#ifndef __khci_h__
#define __khci_h__

/* Basic scheduling packets algorithm, schedules next packet to the beginning of next frame
 */
//#define KHCICFG_BASIC_SCHEDULING
/* Scheduling based on internal "deadtime" register THSLD; you can choose additional value passed to the register
 ** Note that the register value depends on delay on line from host to device and is experimentally set to 0x65,
 ** which should be enough for the absolute majority of cases.
 */
#define KHCICFG_THSLD_DELAY 0x65

/* Prototypes of functions */
usb_status usb_khci_preinit(usb_host_handle upper_layer_handle, usb_host_handle *handle);
usb_status usb_khci_init(uint8_t controller_id, usb_host_handle handle);
usb_status usb_khci_shutdown(usb_host_handle handle);
usb_status usb_khci_send(usb_host_handle handle, pipe_struct_t* pipe_ptr, tr_struct_t* tr_ptr);
usb_status usb_khci_send_setup(usb_host_handle handle, pipe_struct_t* pipe_ptr, tr_struct_t* tr_ptr);
usb_status usb_khci_recv(usb_host_handle handle, pipe_struct_t* pipe_ptr, tr_struct_t* tr_ptr);
//static usb_status _usb_khci_cancel(usb_host_handle handle, pipe_struct_t* pipe_ptr, tr_struct_t* tr_ptr);
usb_status usb_khci_bus_control(usb_host_handle handle, uint8_t bus_control);
uint32_t usb_khci_get_frame_number(usb_host_handle handle);
usb_status usb_khci_open_pipe(usb_host_handle handle, usb_pipe_handle* pipe_handle_ptr, pipe_init_struct_t* pipe_init_ptr);
usb_status usb_khci_close_pipe(usb_host_handle handle, usb_pipe_handle pipe_handle);
usb_status usb_khci_cancel_pipe(usb_host_handle handle, pipe_struct_t* pipe_ptr, tr_struct_t* tr_ptr);

#endif
