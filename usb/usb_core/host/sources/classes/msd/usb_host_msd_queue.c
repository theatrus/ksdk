/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
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
 * $FileName: usb_host_msd_queue.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   Implements the queue system for the mass storage class.
 *
 *END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_dev_mng.h"
#include "usb_host_ch9.h"
#include "usb_host_common.h"
#include "usb_host_msd_bo.h"
#include "usb_host.h"

#define MASSQ_NEXT(index)   (uint8_t)( index ==(QUEUE_SIZE - 1)?0:index+1)

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_mass_q_init
 * Returned Value : None
 * Comments       :
 *     This function initializes a mass storage class queue.
 *
 *END*--------------------------------------------------------------------*/

void usb_class_mass_q_init
(
    /* [IN] interface structure pointer */
    usb_mass_class_struct_t * msd_class_ptr
    )
{ /* Body */
    uint32_t i = 0;
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_mass_q_init");
#endif

    msd_class_ptr->queue.FIRST = 0;
    msd_class_ptr->queue.LAST = 0;
    msd_class_ptr->queue.COUNT = 0;
    for (i = 0; i < QUEUE_SIZE; i++)
    {
        msd_class_ptr->queue.ELEMENTS[i] = NULL;
    }
    /* _mem_zero(&msd_class_ptr->queue, sizeof(msd_class_ptr->queue)); */
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_mass_q_init, SUCCESSFUL");
#endif

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_mass_q_insert
 * Returned Value : Position at which insertion took place in the queue.
 * Comments       :
 *     This function is called by class driver for inserting a command in the
 *     queue.
 *END*--------------------------------------------------------------------*/

int32_t usb_class_mass_q_insert
(
    /* [IN] interface structure pointer */
    usb_mass_class_struct_t * mass_class,

    /* [IN] command object to be inserted in the queue*/
    mass_command_struct_t* pCmd
    )
{ /* Body */
    mass_queue_struct_t * q = &mass_class->queue;
    int32_t tmp = -1;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_mass_q_insert");
#endif

    /*
     ** Insert into queue, update LAST, check if full and return queue position.
     ** If queue is full -1 will be returned
     */
    USB_Host_MSD_lock();
    if (q->COUNT < QUEUE_SIZE)
    {
        q->ELEMENTS[q->LAST] = pCmd;
        tmp = q->LAST;
        q->LAST = MASSQ_NEXT(q->LAST);
        q->COUNT++;
    } /* Endif */
    USB_Host_MSD_unlock();

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_mass_q_insert, SUCCESSFUL");
#endif

    return tmp;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_mass_deleteq
 * Returned Value : None
 * Comments       :
 *     This routine deletes the pending request in the queue
 *END*--------------------------------------------------------------------*/

void usb_class_mass_deleteq
(
    /* [IN] interface structure pointer */
    usb_mass_class_struct_t * mass_class
    )
{ /* Body */
    mass_queue_struct_t * q = &mass_class->queue;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_mass_deleteq");
#endif

    /* Remove current command and increment FIRST modulo the q size */
    USB_Host_MSD_lock();
    if (q->COUNT)
    {
        q->ELEMENTS[q->FIRST] = NULL;
        q->FIRST = MASSQ_NEXT(q->FIRST);
        /*   q->AVAILABLE = TRUE; */
        q->COUNT--;
    }
    USB_Host_MSD_unlock();

    /* if (q->COUNT >1) {
     USB_PRINTF("\nMASS q size now %d", q->COUNT );
     }
     */

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_mass_deleteq, SUCCESSFUL");
#endif

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_mass_get_pending_request
 * Returned Value : None
 * Comments       :
 *     This routine fetches the pointer to the first (pending) request in
 *     the queue, or NULL if there is no pending requests.
 *END*--------------------------------------------------------------------*/

void usb_class_mass_get_pending_request
(
    /* [IN] interface structure pointer */
    usb_mass_class_struct_t * mass_class,

    /* [OUT] pointer to pointer which will hold the pending request */
    mass_command_struct_t* * cmd_ptr_ptr
    )
{ /* Body */
    mass_queue_struct_t * q = &mass_class->queue;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_mass_get_pending_request");
#endif

    USB_Host_MSD_lock();
    if (q->COUNT)
    {
        *cmd_ptr_ptr = (mass_command_struct_t*) q->ELEMENTS[q->FIRST];
    }
    else
    {
        *cmd_ptr_ptr = NULL;
    } /* Endif */
    USB_Host_MSD_unlock();

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_mass_get_pending_request, SUCCESSFUL");
#endif

    return;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_mass_cancelq
 * Returned Value : None
 * Comments       :
 *     This routine cancels the given request in the queue
 *END*--------------------------------------------------------------------*/

bool usb_class_mass_cancelq
(
    /* [IN] interface structure pointer */
    usb_mass_class_struct_t * mass_class,

    /* [IN] command object to be inserted in the queue*/
    mass_command_struct_t * pCmd
    )
{ /* Body */
    mass_queue_struct_t * q = &mass_class->queue;
    uint32_t i;
    uint32_t index;
    bool result = FALSE;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_mass_cancelq");
#endif

    /* Remove given command - leave q size the same*/
    USB_Host_MSD_lock();
    if (q->COUNT)
    {
        index = q->FIRST;
        for (i = 0; i < q->COUNT; i++)
        {
            if (q->ELEMENTS[index] == pCmd)
            {
                q->ELEMENTS[index] = NULL;
                result = TRUE;
                /* USB_PRINTF("\nMASS q entry cancelled!!!" ); */
                break;
            }
            index = MASSQ_NEXT(index);
        }
    }
    USB_Host_MSD_unlock();

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE(result?"usb_class_mass_cancelq, SUCCESSFUL":"usb_class_mass_cancelq, FAILED");
#endif

    return result;
} /* Endbody */

/* EOF */
