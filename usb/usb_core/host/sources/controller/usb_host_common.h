/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 Freescale Semiconductor;
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
* $FileName: host_common.h$
* $Version : 3.8.24.0$
* $Date    : Sep-13-2012$
*
* Comments:
*
*   This file contains definitions for USB common utilities.
*
*END************************************************************************/

#ifndef __host_common_h__
#define __host_common_h__

typedef struct pipe_struct
{
   struct pipe_struct *next;
   tr_struct_t*    tr_list_ptr;   /* List of transactions scheduled on this pipe */
   void*           dev_instance;       /* Instance on list of all devices that own pipe. */
   uint32_t        flags;              /* After all data transferred, should we terminate the transfer with a zero length packet if the last packet size == max_packet_size? */
   uint16_t        max_packet_size;    /* Pipe's maximum packet size for sending or receiving. */
   uint16_t        nak_count;          /* The maximum number of NAK responses tolerated. MUST be zero for interrupt. */
                                   /* For USB 1.1, after count NAKs/frame,transaction is deferred to next frame. */
                                   /* For USB 2.0, the host controller will not execute transaction following this many NAKs. */
   uint8_t         interval;           /* Interval for polling pipe for data transfer. */
  

   uint8_t         endpoint_number;    /* The endpoint number */
   uint8_t         direction;          /* Input or Output */
   uint8_t         pipetype;           /* Control, interrupt, bulk or isochronous */
   uint8_t         trs_per_uframe;     /* Number of transaction per frame, only high-speed high-bandwidth pipes. */
   uint8_t         open;               /* Open flag */
   uint8_t         nextdata01;         /* Endpoint data toggling bit */
} pipe_struct_t;

/* Class Callback function storage structure */
typedef struct _device_class_info_struct
{
   uint8_t               class_type;
   uint8_t               sub_class;
   uint8_t               protocol;
} device_class_info_struct_t;

/* List of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

bool  usb_host_driver_info_nonzero(usb_host_driver_info_t* info_ptr);
class_map_t*  usb_host_get_class_map(interface_descriptor_t*  interface_desc_ptr);

usb_status usb_hostdev_validate(usb_device_instance_handle);

      
#ifdef __cplusplus
}
#endif

#endif
/* EOF */
