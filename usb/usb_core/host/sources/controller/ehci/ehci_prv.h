/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013-2014 Freescale Semiconductor;
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
* $FileName: ehci_prv.h$
* $Version : 3.8.17.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains the internal USB Host specific type definitions
*
*END************************************************************************/
#ifndef __ehci_prv_h__
#define __ehci_prv_h__

//#include "host_cnfg.h"

//#include "ehci_cache.h"
#include "usb_ehci.h"

/***************************************
**
** Code macros
**
*/

#define  BitStuffTime(x)                     (7* 8 * x / 6)
#define  psp_has_data_cache                  0


typedef struct ehci_pipe_struct
{
   pipe_struct_t         common;

   uint32_t              current_nak_count;
   ehci_qh_struct_t*                 qh_for_this_pipe; /* queue head for this pipe */

   /* special fields defined for periodic transfers */
   uint16_t              start_frame;    /*frame number from which this pipe is started to schedule*/
   uint16_t              bwidth;         /* time take by this pipe (for periodic pipes only */
   uint16_t              ss_bwidth;
   uint16_t              cs_bwidth;
   uint16_t              last_frame_index;
   uint8_t               start_uframe;   /*micro frame number from which this pipe is started to schedule*/
   uint8_t               no_of_start_split; /* store the number of start splits (used for iso out )*/
   uint8_t               start_split;    /* store the start split slots for full speed devices*/
   uint8_t               complete_split; /* store the complete split slots for full speed devices*/
   uint8_t               bwidth_slots[8];/*micro frame slots budgeted for transaction */
   uint8_t               actived;
   uint8_t               fsls_bwidth[8]; /* bandwidth for FSLS transaction */
   /* 84 bytes so far add padding to ensure cache alignment*/
#if psp_has_data_cache  // todo check align
   uint8_t               reserved[usb_cache_align(74) - 74]; 
#endif  
    
} ehci_pipe_struct_t;

typedef struct
{
    ehci_pipe_struct_t* pipe_ptr;
    ehci_qh_struct_t*   qh_ptr;
    ehci_qtd_struct_t*  qtd_head;
} qh_node_t;

/* Queue head management data structure */
typedef struct qh_link_node_struct
{
    qh_node_t                          qh_node;
    struct qh_link_node_struct*        next;
} qh_link_node_t;

/* Queue head management data structure */
typedef struct active_qh_mgmt_struct
{
   ehci_qh_struct_t*                  qh_ptr;
   //ehci_qtd_struct_t*                 FIRST_QTD_PTR;
   //int32_t                              TIME;
   //struct active_qh_mgmt_struct       *NEXT_ACTIVE_QH_MGMT_STRUCT_PTR;
} ACTIVE_QH_MGMT_STRUCT, * ACTIVE_QH_MGMT_STRUCT_PTR;

/* ITD,SITD list management data structure (doubly link list )*/
typedef struct list_node_struct
{
    struct list_node_struct *next; /* next member in the list */
    struct list_node_struct *prev; /* previous member in the list */
    void*  member; /* pointer to the currently active ITD or SITD*/
   bool                           next_active; /* is next node a active node */
} list_node_struct_t;



typedef struct  usb_ehci_host_state_structure
{
   uint32_t                            controller_id;
   uint32_t                            usbRegBase;
   void*                               upper_layer_handle;
   void*                               dev_ptr;
   pipe_struct_t*                      pipe_descriptor_base_ptr;
   uint32_t                            frame_list_size;
#if EHCI_BANDWIDTH_RECORD_ENABLE
   uint8_t*                            periodic_frame_list_bw_ptr;
#endif
   void                               *xtd_struct_base_addr;
   void                               *periodic_list_base_addr;
   ehci_qh_struct_t*                   async_list_base_addr;
   ehci_qh_struct_t*                   qh_base_ptr; 
   uint32_t                            qh_entries;
   ehci_qh_struct_t*                   qh_head;
   ehci_qh_struct_t*                  qh_tail;
   ehci_qh_struct_t*                  dummy_qh;

   ehci_qh_struct_t*                   active_async_list_ptr;
   ehci_qh_struct_t*                  active_async_list_tail_ptr;
   qh_link_node_t*                     active_qh_node_list;
   ehci_qh_struct_t*                  active_interrupt_periodic_list_ptr;
   ehci_qh_struct_t*                  active_interrupt_periodic_list_tail_ptr;
   ehci_qtd_struct_t*                 qtd_base_ptr;
   ehci_qtd_struct_t*                 qtd_head;
   ehci_qtd_struct_t*                 qtd_tail;
   uint32_t                            qtd_entries;
   
    list_node_struct_t* active_iso_itd_periodic_list_head_ptr;
    list_node_struct_t* active_iso_itd_periodic_list_tail_ptr;
   ehci_itd_struct_t*                 itd_base_ptr;
   ehci_itd_struct_t*                 itd_head;
   ehci_itd_struct_t*                 itd_tail;
   uint32_t                            itd_entries;

    list_node_struct_t* active_iso_sitd_periodic_list_head_ptr;
    list_node_struct_t* active_iso_sitd_periodic_list_tail_ptr;
   ehci_sitd_struct_t*                sitd_base_ptr;
   ehci_sitd_struct_t*                 sitd_head;
   ehci_sitd_struct_t*               sitd_tail;
   uint32_t                            sitd_entries;
   uint32_t                            reset_recovery_timer;
   uint32_t                            port_num;
   os_mutex_handle                     mutex;
   os_event_handle                     ehci_event_ptr;
   bool                                full_speed_iso_queue_active; 
   bool                                high_speed_iso_queue_active;
   bool                                periodic_list_initialized;
   bool                                itd_list_initialized;
   bool                                sitd_list_initialized;
   bool                                is_resetting;
   uint8_t                             uframe_count;
   uint8_t                             temp_speed;
   
   uint8_t                             devices_inserted;
   uint8_t                             devices_attached;
   
} usb_ehci_host_state_struct_t;
#define USB_EHCI_Host_lock()                OS_Mutex_lock(((usb_ehci_host_state_struct_t*)usb_host_ptr)->mutex)
#define USB_EHCI_Host_unlock()              OS_Mutex_unlock(((usb_ehci_host_state_struct_t*)usb_host_ptr)->mutex)

#define EHCI_GET_TYPE(data_struct_ptr) \
   (EHCI_MEM_READ(*((volatile uint32_t *)data_struct_ptr)) & EHCI_ELEMENT_TYPE_MASK)

#define EHCI_ITD_QADD(head, tail, ITD)      \
   if ((head) == NULL) {         \
      (head) = (ITD);            \
   } else {                      \
      (tail)->scratch_ptr = (ITD);   \
   } /* Endif */                 \
   (tail) = (ITD);               \
   (ITD)->scratch_ptr = NULL
   
#define EHCI_ITD_QGET(head, tail, ITD)      \
   (ITD) = (head);               \
   if (head) {                   \
      (head) = (ehci_itd_struct_t*)((head)->scratch_ptr);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

#define EHCI_ACTIVE_QUEUE_ADD_NODE(tail,member_ptr)     \
   if (tail->prev != NULL) {                             \
       tail->prev->next_active = TRUE;                   \
   }                                                     \
   tail->member =  member_ptr;                                  \
   tail->next_active = FALSE;                            \
   tail = tail->next                   

#define EHCI_QUEUE_FREE_NODE(head,tail,node_ptr) \
   if(node_ptr->prev != NULL) {                             \
     node_ptr->prev->next = node_ptr->next;                 \
     node_ptr->prev->next_active = node_ptr->next_active;   \
     node_ptr->next->prev = node_ptr->prev;                 \
   } else  {                                                \
     head = node_ptr->next;                                 \
     head->prev = NULL;                                     \
   }                                                        \
   node_ptr->next = tail->next;                             \
   node_ptr->prev = tail;                                   \
   node_ptr->next_active = FALSE;                           \
   node_ptr->member = NULL;                                 \
   if(tail->next != NULL)   {                               \
      tail->next->prev = node_ptr;                          \
   }                                                        \
   tail->next = node_ptr;                                   \
   tail->next_active = FALSE;                             

#define EHCI_SITD_QADD(head, tail, SITD)      \
   if ((head) == NULL) {         \
      (head) = (SITD);            \
   } else {                      \
      (tail)->scratch_ptr = (SITD);   \
   } /* Endif */                 \
   (tail) =  (SITD);               \
   (SITD)->scratch_ptr = NULL
   
#define EHCI_SITD_QGET(head, tail, SITD)      \
   (SITD) = (head);               \
   if (head) {                   \
      (head) = (ehci_sitd_struct_t*)((head)->scratch_ptr);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */
   
#define EHCI_QTD_QADD(head, tail, QTD)      \
   if ((head) == NULL) {         \
      (head) = (QTD);            \
   } else {                      \
      (tail)->next = (QTD);   \
   } /* Endif */                 \
   (tail) = (QTD);               \
   (QTD)->next = NULL
   
#define EHCI_QTD_QGET(head, tail, QTD)      \
   (QTD) = (head);               \
   if (head) {                   \
      (head) = (ehci_qtd_struct_t*)((head)->next);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

#define EHCI_QH_QADD(head, tail, QH)      \
   if ((head) == NULL) {         \
      (head) =  (QH);            \
   } else {                      \
      (tail)->next =  (QH);   \
   } /* Endif */                 \
   (tail) = (QH);               \
   (QH)->next = NULL
   
#define EHCI_QH_QGET(head, tail, QH)      \
   (QH) = (head);               \
   if (head) {                   \
      (head) = (ehci_qh_struct_t*)((head)->next);  \
      if ((head) == NULL) {      \
         (tail) = NULL;          \
      } /* Endif */              \
   } /* Endif */

#ifdef __cplusplus
extern "C"
{
#endif   
   
extern usb_status _usb_ehci_calculate_time (uint8_t speed, uint8_t pipetype, uint8_t direction, uint32_t bytes);
extern uint32_t usb_ehci_get_frame_number (usb_host_handle handle);
extern uint32_t usb_ehci_get_micro_frame_number (usb_host_handle handle);
extern usb_status _usb_ehci_allocate_bandwidth (usb_host_handle handle, pipe_struct_t* pipe_ptr);

usb_status _usb_ehci_fsls_allocate_bandwidth(usb_host_handle handle, pipe_struct_t* pipe_ptr);
usb_status _usb_ehci_fsls_sum_bandwidth
(
    usb_host_handle                        handle,
    ehci_pipe_struct_t*                    pipe_descr_ptr,
    uint32_t*                              uframe_bandwidth,
    uint32_t                               frame
);


extern void unlink_periodic_data_structure_from_frame (volatile uint32_t * prev_transfer_data_struct_ptr, volatile uint32_t * transfer_data_struct_ptr);
extern void reclaim_band_width (usb_host_handle handle, uint32_t frame_number, volatile uint32_t * transfer_data_struct_ptr, ehci_pipe_struct_t* pipe_descr_ptr);
extern usb_status usb_ehci_cancel_transfer (usb_host_handle handle, pipe_struct_t* pipe_ptr, tr_struct_t* current_pipe_tr_struct_ptr);
extern uint32_t _usb_ehci_add_interrupt_xfer_to_periodic_list (usb_host_handle handle, ehci_pipe_struct_t* pipe_descr_ptr, tr_struct_t* pipe_tr_ptr);
extern void _usb_ehci_close_interrupt_pipe (usb_host_handle handle, ehci_pipe_struct_t* pipe_descr_ptr);
extern usb_status _usb_ehci_link_structure_in_periodic_list (usb_host_handle handle, ehci_pipe_struct_t* pipe_descr_ptr, uint32_t * struct_to_link_list, uint32_t no_of_structs);
extern usb_status _usb_ehci_add_ITD (usb_host_handle handle, ehci_pipe_struct_t* pipe_descr_ptr, tr_struct_t* pipe_tr_ptr);
extern usb_status _usb_ehci_add_SITD (usb_host_handle handle, ehci_pipe_struct_t* pipe_descr_ptr, tr_struct_t* pipe_tr_ptr);
extern uint32_t _usb_ehci_add_isochronous_xfer_to_periodic_schedule_list (usb_host_handle handle, ehci_pipe_struct_t* pipe_descr_ptr, tr_struct_t* pipe_tr_ptr);
extern void _usb_ehci_free_ITD (usb_host_handle handle, ehci_itd_struct_t* itd_ptr);
extern void _usb_ehci_free_SITD (usb_host_handle handle, ehci_sitd_struct_t* sitd_ptr);
extern void _usb_ehci_close_isochronous_pipe (usb_host_handle handle, ehci_pipe_struct_t* pipe_descr_ptr);
usb_status usb_ehci_send_data (usb_host_handle handle, pipe_struct_t* pipe_descr_ptr, tr_struct_t* current_pipe_tr_struct_ptr);
usb_status usb_ehci_send_setup (usb_host_handle handle, pipe_struct_t* pipe_descr_ptr, tr_struct_t* current_pipe_tr_struct_ptr);
usb_status usb_ehci_recv_data (usb_host_handle handle, pipe_struct_t* pipe_descr_ptr, tr_struct_t* current_pipe_tr_struct_ptr);
usb_status _usb_ehci_queue_pkts ( usb_host_handle handle, ehci_pipe_struct_t* pipe_descr_ptr, tr_struct_t* current_pipe_tr_struct_ptr);
void _usb_ehci_init_qh (usb_host_handle handle, ehci_pipe_struct_t* pipe_descr_ptr, ehci_qh_struct_t* qh_ptr);
void _usb_ehci_init_qtd (usb_host_handle handle, ehci_qtd_struct_t* qtd_ptr, uint8_t*  buffer_start_address, uint32_t token);
uint32_t _usb_ehci_init_qtd_link (usb_host_handle handle, ehci_pipe_struct_t* pipe_descr_ptr, tr_struct_t* current_pipe_tr_struct_ptr);
uint32_t _usb_ehci_add_xfer_to_asynch_schedule_list (usb_host_handle handle, ehci_pipe_struct_t* pipe_descr_ptr, tr_struct_t* current_pipe_tr_struct_ptr);
bool _usb_ehci_process_port_change (usb_host_handle handle);
void _usb_ehci_reset_and_enable_port (usb_host_handle handle, uint8_t port_number);
void _usb_host_process_reset_recovery_done (usb_host_handle handle);

void _usb_ehci_process_tr_complete(usb_host_handle handle);

void _usb_ehci_isr
   (
      /* [IN] the USB Host state structure */
      void
   );

usb_status usb_ehci_preinit (usb_host_handle upper_layer_handle, usb_host_handle *handle);
usb_status usb_ehci_init (uint8_t controller_id, usb_host_handle handle);
//void _usb_ehci_free_qtd (usb_host_handle, void *);
//void _usb_ehci_free_qh (usb_host_handle, void *);
usb_status _usb_ehci_free_resources (usb_host_handle handle, pipe_struct_t* pipe_ptr);
void _usb_host_delay (usb_host_handle handle, uint32_t delay);

usb_status usb_ehci_open_pipe(usb_host_handle handle, usb_pipe_handle * pipe_handle_ptr, pipe_init_struct_t* pipe_init_ptr);
usb_status usb_ehci_close_pipe(usb_host_handle handle, usb_pipe_handle pipe_handle);

void _usb_ehci_process_timer(usb_host_handle handle);
usb_status usb_ehci_update_dev_address(usb_host_handle handle, pipe_struct_t* pipe_ptr);
extern usb_status usb_ehci_shutdown (usb_host_handle handle);
extern usb_status usb_ehci_bus_control (usb_host_handle handle, uint8_t bControl);
extern void _usb_ehci_bus_suspend (usb_host_handle handle);
extern void _usb_ehci_bus_resume (usb_host_handle handle);


/* Prototypes */

void _usb_ehci_process_qh_list_tr_complete(usb_host_handle handle, ehci_qh_struct_t* active_qh_node_list);
void _usb_ehci_process_qh_interrupt_tr_complete(usb_host_handle handle, ehci_qh_struct_t* active_list_member_ptr);
void _usb_ehci_process_itd_tr_complete(usb_host_handle handle);
void _usb_ehci_process_sitd_tr_complete(usb_host_handle handle);

#ifdef __cplusplus
}
#endif

#endif
