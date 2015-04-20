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
* $FileName: usb_host_hub_sm.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file contains the implementation of hub state machine on host.
*
*END************************************************************************/
#include "usb_host_config.h"
#if USBCFG_HOST_HUB
#include "usb.h"
#include "usb_host_stack_interface.h"
#include "usb_host_hub.h"
#include "usb_host_hub_prv.h"
#include "usb_host_hub_sm.h"
#include "usb_host_dev_mng.h"
#include "usb_host.h"

#define USBCFG_HUB_TASK_PRIORITY    7
#define USBCFG_HUB_TASK_STACKSIZE   3000
#define USB_HUB_TASK_NAME           "HUB Task"

#define USB_HUB_EVENT_CTRL           (0x01)
#define USB_HUB_EVENT_DELETE         (0x02)

#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)          /* USB stack running on SDK */
#if !(USE_RTOS)
#define USB_HUB_TASK_ADDRESS 			   usb_hub_task
#else 
#define USB_HUB_TASK_ADDRESS              usb_hub_task_stun
#endif
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)        /* USB stack running on BM  */
#define USB_HUB_TASK_ADDRESS              usb_hub_task
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)       /* USB stack running on MQX */  
#define USB_HUB_TASK_ADDRESS              usb_hub_task_stun
#endif

extern usb_host_handle usb_host_dev_mng_get_host(usb_device_instance_handle dev_handle);
extern uint8_t usb_host_dev_mng_get_level(usb_device_instance_handle dev_handle);
extern uint8_t usb_host_dev_mng_get_address(usb_device_instance_handle dev_handle);

extern usb_status usb_host_dev_mng_attach(usb_host_handle handle,hub_device_struct_t* hub, uint8_t speed, uint8_t hub_no, uint8_t port_no, uint8_t level, usb_device_instance_handle* dev_handle_ptr);

extern usb_status usb_host_dev_mng_detach(usb_host_handle handle, uint8_t hub_no, uint8_t port_no);

void usb_host_hub_device_sm(void*, void *, uint8_t *, uint32_t, usb_status);
static void usb_host_hub_int_callback(void*, void *, uint8_t *, uint32_t, usb_status);
static void usb_host_hub_rescan(hub_device_struct_t* hub_instance, hub_command_t* hub_com);
static void usb_host_hub_get_bitmap(hub_device_struct_t* hub_instance, hub_command_t* hub_com);


static void usb_host_hub_rescan(hub_device_struct_t* hub_instance, hub_command_t* hub_com)
{
    hub_port_struct_t*            port_status_ptr = NULL;
    uint8_t                        i;

    for (i = 0; i < hub_instance->hub_port_nr; i++)
    {
        port_status_ptr = hub_instance->hub_ports + i;
        if ((port_status_ptr->status & (1 << PORT_CONNECTION)) &&
           !((hub_instance->hub_ports + i)->app_status & HUB_PORT_ATTACHED))
        {
            /* if some device is attached since startup, then start session */
            hub_instance->port_iterator = i;
            hub_instance->state = HUB_GET_PORT_STATUS_ASYNC;
            //(hub_instance->hub_ports + i)->app_status = 0;
            if (USB_OK != usb_class_hub_get_port_status(hub_com, hub_instance->port_iterator + 1, hub_instance->port_status_buffer, 4))
            {
#if _DEBUG
                USB_PRINTF("ERROR usb_class_hub_get_port_status in HUB_GET_PORT_STATUS_PROCESS\r\n");
#endif
            }
            else
            {
#if _DEBUG
                USB_PRINTF("get port %d status 0x%x\r\n", i, port_status_ptr->status);
#endif
                hub_instance->in_control = 1;
            }
            break;
        }
    }

    if (hub_instance->in_control == 0)
    {
        usb_host_hub_get_bitmap(hub_instance, hub_com);
    }
}

static void usb_host_hub_get_bitmap(hub_device_struct_t* hub_instance, hub_command_t* hub_com)
{
    if ((hub_instance->in_recv == 0))
    {
        hub_com->class_ptr      = hub_instance->class_handle;
        hub_com->callback_fn    = usb_host_hub_int_callback;
        hub_com->callback_param = hub_instance;
        if (USB_OK != usb_class_hub_recv_bitmap(hub_com, hub_instance->bit_map_buffer, hub_instance->hub_port_nr / 8 + 1))
        {
#if _DEBUG
            USB_PRINTF("error in hub usb_class_hub_recv_bitmap\r\n");
#endif
            hub_instance->state = HUB_NONE;
        }
        else
        {
            hub_instance->in_recv = 1;
        }
    }
}


//static void usb_host_hub_control_callback(void*, void *, pointer, uint32_t, usb_status);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hub_get_instance
* Returned Value : None
* Comments       :
*     called when a hub has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
usb_status usb_host_hub_get_instance
    (
         /* [IN] pointer to device instance */
         usb_device_instance_handle      dev_handle,

         /* [IN] pointer to interface descriptor */
         usb_interface_descriptor_handle intf_handle,

         /* [OUT] pointer to CLASS_CALL_STRUCT to be filled in */
         hub_device_struct_t* *          hub_instance_ptr
    )
{
    hub_device_struct_t*          instance_ptr = NULL;
    dev_instance_t*               dev_instance_ptr = (dev_instance_t*)dev_handle;
    usb_host_state_struct_t*      usb_host_ptr = (usb_host_state_struct_t*)dev_instance_ptr->host;
    
    USB_HUB_lock();
    
    instance_ptr = usb_host_ptr->hub_link;
    while (instance_ptr != NULL)
    {
        if ((instance_ptr->dev_handle != dev_handle) ||
           (instance_ptr->intf_handle != intf_handle))
        {
            instance_ptr = instance_ptr->next;
        }
        else
        {
            *hub_instance_ptr = instance_ptr;
            USB_HUB_unlock();
            return USB_OK;
        }
    }
    USB_HUB_unlock();
    *hub_instance_ptr = NULL;
    return USBERR_ERROR;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hub_create_instance
* Returned Value : None
* Comments       :
*     called when a hub has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
static hub_device_struct_t* usb_host_hub_create_instance
    (
         /* [IN] pointer to device instance */
         usb_device_instance_handle      dev_handle,

         /* [IN] pointer to interface descriptor */
         usb_interface_descriptor_handle intf_handle
    )
{
    hub_device_struct_t* hub_instance = NULL;

    hub_instance = (hub_device_struct_t*)OS_Mem_alloc_zero(sizeof(hub_device_struct_t));
    if (hub_instance == NULL)
    {
#if _DEBUG
        USB_PRINTF("OS_Mem_alloc_zero error in usb_host_hub_create_instance\r\n");
#endif
        return NULL;
    }
    
    hub_instance->hub_descriptor_buffer = (uint8_t *)OS_Mem_alloc_uncached_zero(7 + MAX_HUB_PORT_NUMBER / 8 + 1);
    if (hub_instance->hub_descriptor_buffer == NULL)
    {
#if _DEBUG
        USB_PRINTF("failed to get memory for hub descriptor\r\n");
#endif
    }
    
    hub_instance->port_status_buffer = (uint8_t *)OS_Mem_alloc_uncached_zero(4);
    if (hub_instance->port_status_buffer == NULL)
    {
#if _DEBUG
        USB_PRINTF("failed to get memory for port status\r\n");
#endif
    }
    
    hub_instance->bit_map_buffer = (uint8_t *)OS_Mem_alloc_uncached_zero(MAX_HUB_PORT_NUMBER / 8 + 1);
    if (hub_instance->bit_map_buffer == NULL)
    {
#if _DEBUG
        USB_PRINTF("failed to get memory for bit map\r\n");
#endif
    }
    
    hub_instance->dev_handle = dev_handle;
    hub_instance->intf_handle = intf_handle;
    hub_instance->in_recv  = 0;
    hub_instance->host_handle = usb_host_dev_mng_get_host(dev_handle);
    hub_instance->hub_level = usb_host_dev_mng_get_level(dev_handle);
	hub_instance->speed = usb_host_dev_mng_get_speed(dev_handle);
	hub_instance->hs_hub_no = usb_host_dev_mng_get_hubno(dev_handle);
	hub_instance->hs_port_no = usb_host_dev_mng_get_portno(dev_handle);
	if (((usb_host_dev_mng_get_hub_speed(dev_handle) != USB_SPEED_HIGH) 
		&& (hub_instance->speed != USB_SPEED_HIGH)) && (hub_instance->hub_level 
		> 2))
	{
		hub_instance->hs_hub_no = usb_host_dev_mng_get_hs_hub_no(dev_handle);
		hub_instance->hs_port_no = usb_host_dev_mng_get_hs_port_no(dev_handle);
	}
#if _DEBUG
	USB_PRINTF("### Level :%d Speed:%d  %d  %d \r\n",hub_instance->hub_level, hub_instance->speed,hub_instance->hs_hub_no,hub_instance->hs_port_no);
#endif   
    return hub_instance;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hub_destroy_instance
* Returned Value : None
* Comments       :
*     called when a hub has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
usb_status usb_host_hub_destroy_instance
    (
         hub_device_struct_t* instance_ptr
    )
{
    if (instance_ptr == NULL)
    {
        return USBERR_ERROR;
    }
    if (instance_ptr->hub_ports != NULL)
    {
        OS_Mem_free(instance_ptr->hub_ports);
        instance_ptr->hub_ports = NULL;
    }
    if (instance_ptr->hub_descriptor_buffer != NULL)
    {
        OS_Mem_free(instance_ptr->hub_descriptor_buffer);
        instance_ptr->hub_descriptor_buffer = NULL;
    }
    
    if (instance_ptr->port_status_buffer != NULL)
    {
        OS_Mem_free(instance_ptr->port_status_buffer);
        instance_ptr->port_status_buffer = NULL;
    }

    if (instance_ptr->bit_map_buffer != NULL)
    {
        OS_Mem_free(instance_ptr->bit_map_buffer);
        instance_ptr->bit_map_buffer = NULL;
    }

    OS_Mem_free(instance_ptr);
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_task
*  Returned Value : none
*  Comments       :
*        KHCI task
*END*-----------------------------------------------------------------*/
static void usb_hub_task(void* param)
{
    hub_device_struct_t*          instance_ptr;
    hub_device_struct_t*          pre;
    usb_host_state_struct_t*      usb_host_ptr = (usb_host_state_struct_t*)param;

    if(OS_Sem_wait(usb_host_ptr->hub_sem, 0) != OS_EVENT_OK)
    {
        return;
    }

    USB_HUB_lock();
    pre = instance_ptr = usb_host_ptr->hub_link;
    
    while (instance_ptr != NULL)
    {
        if (instance_ptr->to_be_deleted != 1)
        {
            pre = instance_ptr;
            instance_ptr = instance_ptr->next; 
        }
        else
        {
            if ((instance_ptr->in_control == 0) &&
                (instance_ptr->in_recv == 0))
            {
                if (instance_ptr == usb_host_ptr->hub_link)
                {
                    usb_host_ptr->hub_link = instance_ptr->next;
                }
                else
                {
                    pre->next = instance_ptr->next;
                }
                if (USB_OK != usb_host_close_dev_interface(instance_ptr->host_handle, instance_ptr->dev_handle, instance_ptr->intf_handle, instance_ptr->class_handle))
                {
#if _DEBUG
                    USB_PRINTF("error in hub usb_host_close_dev_interface\r\n");
#endif
                }
                USB_PRINTF("level %d hub disconnected\r\n", instance_ptr->hub_level);
                usb_host_hub_destroy_instance(instance_ptr);
                break;
            }
            else
            {
                pre = instance_ptr;
                instance_ptr = instance_ptr->next; 
            }
        }
    }
    USB_HUB_unlock();
    return ;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_task_stun
*  Returned Value : none
*  Comments       :
*        KHCI task
*END*-----------------------------------------------------------------*/
#if ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) || ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && (USE_RTOS))) 
static void usb_hub_task_stun(void* hub_instance)
{
    while(1)
    {
        usb_hub_task(hub_instance);
    }
}

#endif

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hub_device_event
* Returned Value : None
* Comments       :
*     called when a hub has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_hub_device_event
   (
      /* [IN] pointer to device instance */
      usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint32_t                          event_code
   )
{
    hub_device_struct_t*          hub_instance;
    hub_device_struct_t*          temp;
    hub_command_t                 hub_com;
    uint8_t                       i;
    dev_instance_t*               dev_instance_ptr = (dev_instance_t*)dev_handle;
    usb_host_state_struct_t*      usb_host_ptr = (usb_host_state_struct_t*)dev_instance_ptr->host;

    switch (event_code) {
        case USB_ATTACH_EVENT:
            /* Drop through into attach, same processing */
          
            /* Find device in g_hub_link. */
            USB_HUB_lock();
            hub_instance = usb_host_ptr->hub_link;
            while(hub_instance != NULL)
            {
                if(hub_instance->dev_handle == dev_handle)
                {
                    break;
                }
                hub_instance = hub_instance->next;
            }
            
            if (hub_instance != NULL)
            {
                USB_HUB_unlock();
                break;
            }
            USB_HUB_unlock();
            
            /* Create 'unsafe' application struct */
            hub_instance = usb_host_hub_create_instance(dev_handle, intf_handle);
            
            if (NULL != hub_instance)
            {
                hub_instance->state     = HUB_IDLE;
                
                if (usb_host_ptr->hub_task == 0xFFFFFFFF)
                {
                    usb_host_ptr->hub_task = OS_Task_create(USB_HUB_TASK_ADDRESS, (void*)usb_host_ptr, (uint32_t)USBCFG_HUB_TASK_PRIORITY, USBCFG_HUB_TASK_STACKSIZE, USB_HUB_TASK_NAME, NULL);
                    if (usb_host_ptr->hub_task == (uint32_t)OS_TASK_ERROR)
                    {
#if _DEBUG
                        USB_PRINTF("failed to create hub task\r\n");
#endif
                    }
                }
                
                USB_HUB_lock();
                if (usb_host_ptr->hub_link == NULL)
                {
                    usb_host_ptr->hub_link = hub_instance;
                }
                else
                {
                    temp = usb_host_ptr->hub_link;
                    while (temp->next != NULL)
                    {
                        temp = temp->next;
                    }
                    temp->next = hub_instance;
                }
                USB_HUB_unlock();
            }
            break;
        case USB_CONFIG_EVENT:
            /* Find device in g_hub_link. */
            USB_HUB_lock();
            hub_instance = usb_host_ptr->hub_link;
            while(hub_instance != NULL)
            {
                if(hub_instance->dev_handle == dev_handle)
                {
                    break;
                }
                hub_instance = hub_instance->next;
            }
            USB_HUB_unlock();

            if((hub_instance != NULL) && (hub_instance->opened_interface == 0))
            {
                
                if (USB_OK != usb_host_open_dev_interface(hub_instance->host_handle, dev_handle, intf_handle, (class_handle*)&hub_instance->class_handle))
                {
#if _DEBUG                
                    USB_PRINTF("error in hub usb_host_open_dev_interface\r\n");
#endif
                }
                else
                {
                    hub_instance->opened_interface++;
#if _DEBUG
                    USB_PRINTF("usb_host_open_dev_interface\r\n");
#endif                    
                }
            }
            break;
            
        case USB_INTF_OPENED_EVENT:
#if _DEBUG
            USB_PRINTF("USB_INTF_OPENED_EVENT\r\n");
#endif
            if (USB_OK != usb_host_hub_get_instance(dev_handle, intf_handle, &hub_instance))
            {
#if _DEBUG            
                USB_PRINTF("ERROR usb_host_hub_get_instance in USB_INTF_OPENED_EVENT\r\n");
#endif
                break;
            }
            else
            {
                USB_PRINTF("level %d hub connected\r\n", hub_instance->hub_level);
            }
            /* set we are in process of getting hub descriptor */
            //hub_instance->state    = HUB_BEGIN_GET_DESCRIPTOR_TINY_PROCESS;
            hub_instance->state    = HUB_GET_DESCRIPTOR_TINY_PROCESS;
            hub_com.class_ptr      = hub_instance->class_handle;
            hub_com.callback_fn    = usb_host_hub_device_sm;
            hub_com.callback_param = hub_instance;
            /* here, we should retrieve information from the hub */
            if (USB_OK != usb_class_hub_get_descriptor(&hub_com, hub_instance->hub_descriptor_buffer, 7))
            {
#if _DEBUG            
                USB_PRINTF("error in hub usb_class_hub_get_descriptor\r\n");
#endif
                hub_instance->state = HUB_NONE;
            }
            else
            {
                hub_instance->in_control = 1;
            }
            break;

        case USB_DETACH_EVENT:
            if (USB_OK != usb_host_hub_get_instance(dev_handle, intf_handle, &hub_instance))
            {
#if _DEBUG
                USB_PRINTF("ERROR usb_host_hub_get_instance in USB_DETACH_EVENT\r\n");
#endif
                break;
            }

            for (i = 0; i < hub_instance->hub_port_nr; i++)
            {
                if ((hub_instance->hub_ports + i)->app_status & HUB_PORT_ATTACHED)
                {
                    usb_host_dev_mng_detach(hub_instance->host_handle,
                                            usb_host_dev_mng_get_address(hub_instance->dev_handle),
                                            i + 1);
                }
            }

            USB_HUB_lock();
            hub_instance->to_be_deleted = 1;
            USB_HUB_unlock();

            OS_Sem_post(usb_host_ptr->hub_sem);

            break;

        default:
            break;
   } /* EndSwitch */

} /* Enbbody */




/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hub_device_sm
* Returned Value : None
* Comments       :
*     called when a hub changes state; sm = state machine
*END*--------------------------------------------------------------------*/
void usb_host_hub_device_sm
   (
         /* [IN] structure with USB pipe information on the interface */
         void*           pipe,

         /* [IN] parameters */
         void*           param,

         /* [IN] buffer of data from IN stage */
         uint8_t*          buffer,

         /* [IN] length of data from IN stage */
         uint32_t           len,

         /* [IN] status of transaction */
         usb_status        status
   )
{
    register hub_device_struct_t* hub_instance = (hub_device_struct_t*) param;
    hub_command_t                 hub_com;
    usb_device_instance_handle    dev_handle;
    hub_descriptor_struct_t*      hub_desc = NULL;
    hub_port_struct_t*            port_status_ptr = NULL;
    uint8_t                        speed;
    uint8_t                        i;
    uint32_t                       stat;
    dev_instance_t*               dev_instance_ptr = (dev_instance_t*)hub_instance->dev_handle;
    usb_host_state_struct_t*      usb_host_ptr = (usb_host_state_struct_t*)dev_instance_ptr->host;

    hub_instance->in_control = 0;

    if (hub_instance->to_be_deleted == 1)
    {
        OS_Sem_post(usb_host_ptr->hub_sem);
        return;
    }

    hub_com.class_ptr      = hub_instance->class_handle;
    hub_com.callback_fn    = usb_host_hub_device_sm;
    hub_com.callback_param = hub_instance;
    
    if (status != USB_OK)
    {
#if _DEBUG    
        USB_PRINTF("last request failed 0x%x\r\n", hub_instance->state);
#endif
        usb_host_hub_rescan(hub_instance, &hub_com);
        return;
    }

    switch (hub_instance->state)
    {
        case HUB_IDLE:
        case HUB_BEGIN_GET_DESCRIPTOR_TINY_PROCESS:
        case HUB_BEGIN_GET_DESCRIPTOR_PROCESS:
        case HUB_BEGIN_SET_PORT_FEATURE_PROCESS:
        case HUB_BEGIN_CLEAR_PORT_FEATURE_PROCESS:
        case HUB_BEGIN_GET_PORT_STATUS_PROCESS:
        case HUB_BEGIN_GET_PORT_STATUS_ASYNC:
        case HUB_BEGIN_GET_STATUS_ASYNC:   
            break;
        case HUB_GET_DESCRIPTOR_TINY_PROCESS: 
            /* here we get number of ports from USB data */
            hub_instance->hub_port_nr = ((hub_descriptor_struct_t*)hub_instance->hub_descriptor_buffer)->bnrports;
            if (hub_instance->hub_port_nr > MAX_HUB_PORT_NUMBER)
            {
#if _DEBUG
                USB_PRINTF("the port number exceeds the MAX_HUB_PORT_NUMBER\r\n");
#endif
                break;
            }
            //hub_instance->state = HUB_BEGIN_GET_DESCRIPTOR_PROCESS;
            hub_instance->state = HUB_GET_DESCRIPTOR_PROCESS;
            if (USB_OK != usb_class_hub_get_descriptor(&hub_com, hub_instance->hub_descriptor_buffer, 7 + hub_instance->hub_port_nr / 8 + 1))
            {
#if _DEBUG            
                USB_PRINTF("error in hub usb_class_hub_get_descriptor\r\n");
#endif                
                hub_instance->state = HUB_NONE;
            }
            else
            {
                hub_instance->in_control = 1;
            }
            break;

        case HUB_GET_DESCRIPTOR_PROCESS: 
            {
                /* here, we get information from the hub and fill info in hub_instance */
                hub_desc = (hub_descriptor_struct_t*)hub_instance->hub_descriptor_buffer;
                
                hub_instance->hub_ports = (hub_port_struct_t*)OS_Mem_alloc_zero(hub_instance->hub_port_nr * sizeof(hub_port_struct_t));
            
                for (i = 0; i < hub_instance->hub_port_nr; i++) 
                {
                    port_status_ptr = hub_instance->hub_ports + i;
                    /* get REMOVABLE bit from the desriptor for appropriate installed port */
                    port_status_ptr->app_status = hub_desc->deviceremovable[(i + 1) / 8] & (0x01 << ((i + 1) % 8)) ? HUB_PORT_REMOVABLE : 0;
                }
            }

            /* pass fluently to HUB_SET_PORT_FEATURE_PROCESS */
            //hub_instance->state = HUB_BEGIN_SET_PORT_FEATURE_PROCESS;
            hub_instance->state = HUB_SET_PORT_FEATURE_PROCESS;
            hub_instance->port_iterator = 0;

        case HUB_SET_PORT_FEATURE_PROCESS: 
            if (hub_instance->port_iterator < hub_instance->hub_port_nr) 
            {
                //hub_instance->state = HUB_BEGIN_SET_PORT_FEATURE_PROCESS;
                hub_instance->state = HUB_SET_PORT_FEATURE_PROCESS;
                
                if (USB_OK != usb_class_hub_set_port_feature(&hub_com, ++hub_instance->port_iterator, PORT_POWER))
                {
#if _DEBUG                
                    USB_PRINTF("error in hub usb_class_hub_set_port_feature\r\n");
#endif
                    hub_instance->state = HUB_NONE;
                }
                else
                {
                    hub_instance->in_control = 1;
                }
                break;
            }

            /* pass fluently to HUB_CLEAR_PORT_FEATURE_PROCESS */
            //hub_instance->state = HUB_BEGIN_CLEAR_PORT_FEATURE_PROCESS;
            hub_instance->state = HUB_CLEAR_PORT_FEATURE_PROCESS;
            hub_instance->port_iterator = 0;
        case HUB_CLEAR_PORT_FEATURE_PROCESS: 
            if (hub_instance->port_iterator < hub_instance->hub_port_nr) 
            {
                //hub_instance->state = HUB_BEGIN_CLEAR_PORT_FEATURE_PROCESS;
                hub_instance->state = HUB_CLEAR_PORT_FEATURE_PROCESS;
                
                if (USB_OK != usb_class_hub_clear_port_feature(&hub_com, ++hub_instance->port_iterator, C_PORT_CONNECTION))
                {
#if _DEBUG                
                    USB_PRINTF("error in hub usb_class_hub_clear_port_feature\r\n");
#endif
                    hub_instance->state = HUB_NONE;
                }
                else
                {
                    hub_instance->in_control = 1;
                }
                break;
            }

            /* pass fluently to HUB_GET_PORT_STATUS_PROCESS */
            //hub_instance->state = HUB_BEGIN_GET_PORT_STATUS_PROCESS;
            hub_instance->state = HUB_GET_PORT_STATUS_PROCESS;
            hub_instance->port_iterator = 0;
        case HUB_GET_PORT_STATUS_PROCESS: 
            if (hub_instance->port_iterator)
            {
                port_status_ptr = hub_instance->hub_ports + hub_instance->port_iterator - 1;
                port_status_ptr->status = USB_SHORT_LE_TO_HOST(*(uint32_t*)hub_instance->port_status_buffer);
            }
            if (hub_instance->port_iterator < hub_instance->hub_port_nr)
            {
                //hub_instance->state = HUB_BEGIN_GET_PORT_STATUS_PROCESS;
                hub_instance->state = HUB_GET_PORT_STATUS_PROCESS;
                
                if (USB_OK != usb_class_hub_get_port_status(&hub_com, ++hub_instance->port_iterator, hub_instance->port_status_buffer, 4))
                {
#if _DEBUG
                    USB_PRINTF("error in hub usb_class_hub_get_port_status\r\n");
#endif                    
                    hub_instance->state = HUB_NONE;
                }
                else
                {
                    hub_instance->in_control = 1;
                }
                break;
            }
            usb_host_hub_rescan(hub_instance, &hub_com);

            break;
            
        case HUB_BEGIN_PORT_RESET:
            (hub_instance->hub_ports + hub_instance->port_iterator)->app_status |= HUB_PORT_BEGINRESET;
            hub_instance->state = HUB_GET_PORT_STATUS_ASYNC;
            if (USB_OK != usb_class_hub_get_port_status(&hub_com, hub_instance->port_iterator + 1, hub_instance->port_status_buffer, 4))
            {
#if _DEBUG            
                USB_PRINTF("usb_class_hub_get_port_status failed in usb_host_hub_int_callback\r\n");
#endif
                hub_instance->state = HUB_NONE;
            }
            else
            {
                hub_instance->in_control = 1;
            }
            break;

        case HUB_RESET_DEVICE_PORT_PROCESS:
            hub_instance->state = HUB_BEGIN_PORT_RESET;
            
            if (USB_OK != usb_class_hub_set_port_feature(&hub_com, hub_instance->port_iterator + 1, PORT_RESET))
            {
#if _DEBUG            
                USB_PRINTF("error in usb_class_hub_set_port_feature of PORT_RESET\r\n");
#endif
            }
            else
            {
                hub_instance->in_control = 1;
            }
            break;

        case HUB_ADDRESS_DEVICE_PORT_PROCESS:
            /* compute speed */
            if ((hub_instance->hub_ports + hub_instance->port_iterator)->status & (1 << PORT_HIGH_SPEED))
                speed = USB_SPEED_HIGH;
            else if ((hub_instance->hub_ports + hub_instance->port_iterator)->status & (1 << PORT_LOW_SPEED))
                speed = USB_SPEED_LOW;
            else
                speed = USB_SPEED_FULL;
            /* FIXME: accessing intf_handle directly without validation to get its host handle is not good method */
            usb_host_dev_mng_attach(
                hub_instance->host_handle,
                hub_instance,
                speed, /* port speed */
                usb_host_dev_mng_get_address(hub_instance->dev_handle), /* hub address */
                hub_instance->port_iterator + 1, /* hub port */
                hub_instance->hub_level + 1, /* hub level */
                &dev_handle
                );
           (hub_instance->hub_ports + hub_instance->port_iterator )->app_status |= HUB_PORT_ATTACHED;
            hub_instance->state = HUB_NONE;
            usb_host_hub_rescan(hub_instance, &hub_com);

            break;

        case HUB_DETACH_DEVICE_PORT_PROCESS:
            /* FIXME: accessing intf_handle directly without validation to get its host handle is not good method */
            usb_host_dev_mng_detach(
                hub_instance->host_handle,
                usb_host_dev_mng_get_address(hub_instance->dev_handle), /* hub address */
                hub_instance->port_iterator + 1 /* hub port */
                );

            /* reset the app status */
            (hub_instance->hub_ports + hub_instance->port_iterator)->app_status = 0x00;
             hub_instance->state = HUB_NONE;
            usb_host_hub_rescan(hub_instance, &hub_com);

            break;
            
        case HUB_GET_PORT_STATUS_ASYNC:
            stat = USB_LONG_LE_TO_HOST(*(uint32_t*)(hub_instance->port_status_buffer));
#if _DEBUG            
            USB_PRINTF("stat 0x%x 0x%x\r\n", stat, (hub_instance->hub_ports + hub_instance->port_iterator)->app_status);
#endif
            /* register the current status of the port */
            (hub_instance->hub_ports + hub_instance->port_iterator)->status = stat;

            if ((hub_instance->hub_ports + hub_instance->port_iterator)->app_status & HUB_PORT_BEGINRESET)
            {
                if ((1 << C_PORT_CONNECTION) & stat)
                {
                    hub_instance->state = HUB_NONE;
                    if (USB_OK != usb_class_hub_clear_port_feature(&hub_com, hub_instance->port_iterator + 1, C_PORT_CONNECTION))
                    {
#if _DEBUG                    
                        USB_PRINTF("error in hub usb_class_hub_clear_port_feature C_PORT_CONNECTION\r\n");
#endif
                        hub_instance->state = HUB_NONE;
                    }
                    else
                    {
                        hub_instance->in_control = 1;
                    }
                    (hub_instance->hub_ports + hub_instance->port_iterator)->app_status &= ~HUB_PORT_BEGINRESET;
                }
                else if(!((1 << PORT_CONNECTION) & stat))
                {
                    hub_instance->state = HUB_NONE;
                    usb_host_hub_rescan(hub_instance, &hub_com);
                }
                else if (!((1 << C_PORT_RESET) & stat))
                {
                    hub_instance->state = HUB_GET_PORT_STATUS_ASYNC;
                    if (USB_OK != usb_class_hub_get_port_status(&hub_com, hub_instance->port_iterator + 1, hub_instance->port_status_buffer, 4))
                    {
#if _DEBUG                    
                        USB_PRINTF("usb_class_hub_get_port_status failed in HUB_WAIT_FOR_PORT_RESET\r\n");
#endif
                        hub_instance->state = HUB_NONE;
                    }
                    else
                    {
                        hub_instance->in_control = 1;
                    }
                }
                else
                {
                    hub_instance->state = HUB_NONE;
                }
            }
            else
            {
                /* let's see what happened */
                if (((1 << C_PORT_CONNECTION) & stat))
                {
                    /* get if a device on port was attached or detached */
                    if ((hub_instance->hub_ports + hub_instance->port_iterator)->app_status & HUB_PORT_ATTACHED)
                    {
                        if((1 << PORT_CONNECTION) & stat)
                        {
                            hub_instance->state = HUB_NONE;
                        }
                        else
                        {
                            //hub_instance->state = HUB_BEGIN_DETACH_DEVICE_PORT_PROCESS;
                            hub_instance->state = HUB_DETACH_DEVICE_PORT_PROCESS;
                        }
                    }
                    else 
                    {
                        if((1 << PORT_CONNECTION) & stat)
                        {
                            //hub_instance->state = HUB_BEGIN_RESET_DEVICE_PORT_PROCESS;
                            hub_instance->state = HUB_RESET_DEVICE_PORT_PROCESS;
                        }
                        else
                        {
                            hub_instance->state = HUB_NONE;
                        }
                    }
                    
                    if (USB_OK != usb_class_hub_clear_port_feature(&hub_com, hub_instance->port_iterator + 1, C_PORT_CONNECTION))
                    {
#if _DEBUG                    
                        USB_PRINTF("error in hub usb_class_hub_clear_port_feature C_PORT_CONNECTION\r\n");
#endif
                        hub_instance->state = HUB_NONE;
                    }
                    else
                    {
                        hub_instance->in_control = 1;
                    }
                    break;
                }
                else if((1 << PORT_CONNECTION) & stat)
                {
                   /* get if a device on port was attached or detached */
                    if ((hub_instance->hub_ports + hub_instance->port_iterator)->app_status & HUB_PORT_ATTACHED)
                    {
                        //hub_instance->state = HUB_BEGIN_DETACH_DEVICE_PORT_PROCESS;
                        hub_instance->state = HUB_DETACH_DEVICE_PORT_PROCESS;
                        if((1 << C_PORT_ENABLE) & stat)
                        {
                            //if((1 << PORT_ENABLE) & stat)
                            {
                                hub_instance->state = HUB_NONE;
                            }
                            if (USB_OK != usb_class_hub_clear_port_feature(&hub_com, hub_instance->port_iterator + 1, C_PORT_ENABLE))
                            {
#if _DEBUG                
                                USB_PRINTF("error in hub usb_class_hub_clear_port_feature C_PORT_ENABLE\r\n");
#endif
                                hub_instance->state = HUB_NONE;
                            }
                            else
                            {
                                hub_instance->in_control = 1;
                            }
                            break;
                        }
                    }
                    else
                    {
                        hub_instance->state = HUB_RESET_DEVICE_PORT_PROCESS;
                    }

					
#if 0            
                    if (USB_OK != usb_class_hub_clear_port_feature(&hub_com, hub_instance->port_iterator + 1, PORT_CONNECTION))
                    {
#if _DEBUG                    
                        USB_PRINTF("error in hub usb_class_hub_clear_port_feature PORT_CONNECTION\r\n");
#endif                        
                        hub_instance->state = HUB_NONE;
                    }
                    else
                    {
                        hub_instance->in_control = 1;
                    }
#endif
                    /* compute speed */
                    if (USB_OK != usb_class_hub_set_port_feature(&hub_com, hub_instance->port_iterator + 1, PORT_RESET))
                    {
#if _DEBUG            
                      USB_PRINTF("error in usb_class_hub_set_port_feature of PORT_RESET\r\n");
#endif
                     }
                    else
                    {    
                      hub_instance->in_control = 1;
                     }

                 break;
                }
                else
                {
                    hub_instance->state = HUB_NONE;
                }
            }
            
            if ((1 << C_PORT_RESET) & stat)
            {
                //hub_instance->state = HUB_BEGIN_ADDRESS_DEVICE_PORT_PROCESS;
                hub_instance->state = HUB_ADDRESS_DEVICE_PORT_PROCESS;
                (hub_instance->hub_ports + hub_instance->port_iterator)->app_status &= ~(HUB_PORT_BEGINRESET);
                (hub_instance->hub_ports + hub_instance->port_iterator)->app_status |= HUB_PORT_ATTACHED;
                if (USB_OK != usb_class_hub_clear_port_feature(&hub_com, hub_instance->port_iterator + 1, C_PORT_RESET))
                {
#if _DEBUG                
                    USB_PRINTF("error in hub usb_class_hub_clear_port_feature C_PORT_RESET2222\r\n");
#endif                    
                    hub_instance->state = HUB_NONE;
                }
                else
                {
                    hub_instance->in_control = 1;
                }
                
                break;
            }
            else if ((1 << C_PORT_ENABLE) & stat)
            {
                /* unexpected event (error), just ignore the port */
                //hub_instance->state = HUB_BEGIN_DETACH_DEVICE_PORT_PROCESS;
                hub_instance->state = HUB_DETACH_DEVICE_PORT_PROCESS;
                if (USB_OK != usb_class_hub_clear_port_feature(&hub_com, hub_instance->port_iterator + 1, C_PORT_ENABLE))
                {
#if _DEBUG                
                    USB_PRINTF("error in hub usb_class_hub_clear_port_feature C_PORT_ENABLE\r\n");
#endif
                    hub_instance->state = HUB_NONE;
                }
                else
                {
                    hub_instance->in_control = 1;
                }
                //usb_host_hub_get_bitmap(hub_instance, &hub_com);
                break;
            }
            else if ((1 << C_PORT_OVER_CURRENT) & stat) 
            {
                /* unexpected event (error), just ignore the port */
                hub_instance->state = HUB_NONE;
                if (USB_OK != usb_class_hub_clear_port_feature(&hub_com, hub_instance->port_iterator + 1, C_PORT_OVER_CURRENT))
                {
#if _DEBUG                
                    USB_PRINTF("error in hub usb_class_hub_clear_port_feature C_PORT_OVER_CURRENT\r\n");
#endif
                    break;
                }
                else
                {
                    hub_instance->in_control = 1;
                }
                //usb_host_hub_get_bitmap(hub_instance, &hub_com);
                break;
            }
            else if ((1 << C_PORT_POWER) & stat) 
            {
                /* unexpected event (error), just ignore the port */
                hub_instance->state = HUB_NONE;
                if (USB_OK != usb_class_hub_clear_port_feature(&hub_com, hub_instance->port_iterator + 1, C_PORT_POWER))
                {
#if _DEBUG                
                    USB_PRINTF("error in hub usb_class_hub_clear_port_feature C_PORT_POWER\r\n");
#endif

                }
                else
                {
                    hub_instance->in_control = 1;
                }
                //usb_host_hub_get_bitmap(hub_instance, &hub_com);
                break;
            }
            /* FIXME: handle more events */
            else
            {
                //hub_instance->state = HUB_NONE;
                usb_host_hub_get_bitmap(hub_instance, &hub_com);
            }

            break;

        case HUB_GET_STATUS_ASYNC:
        {
            hub_status_struct_t* hub_stat = (hub_status_struct_t*) hub_instance->port_status_buffer;
            uint32_t change = USB_SHORT_LE_TO_HOST(hub_stat->whubchange);

            if ((1 << C_HUB_LOCAL_POWER) & change) 
            {
                hub_instance->state = HUB_NONE;
                if (USB_OK != usb_class_hub_clear_port_feature(&hub_com, hub_instance->port_iterator + 1, C_HUB_LOCAL_POWER))
                {
#if _DEBUG                
                    USB_PRINTF("error in hub usb_class_hub_clear_port_feature C_HUB_LOCAL_POWER\r\n");
#endif
                    break;
                }
                else
                {
                    hub_instance->in_control = 1;
                }
                usb_host_hub_get_bitmap(hub_instance, &hub_com);
            }
            else if ((1 << C_HUB_OVER_CURRENT) & change) 
            {
                hub_instance->state = HUB_NONE;
                if (USB_OK != usb_class_hub_clear_port_feature(&hub_com, hub_instance->port_iterator + 1, C_HUB_OVER_CURRENT))
                {
#if _DEBUG
                    USB_PRINTF("error in hub usb_class_hub_clear_port_feature C_HUB_OVER_CURRENT\r\n");
#endif
                    break;
                }
                else
                {
                    hub_instance->in_control = 1;
                }
                usb_host_hub_get_bitmap(hub_instance, &hub_com);
            }
            else
            {
                hub_instance->state = HUB_NONE;
                usb_host_hub_get_bitmap(hub_instance, &hub_com);
            }
            break;
        }

        default:
            usb_host_hub_rescan(hub_instance, &hub_com);
            break;

    } /* EndSwitch */
    return;
} /* EndBody */



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_hub_int_callback
* Returned Value : None
* Comments       :
*     Called on interrupt endpoint data reception
*END*--------------------------------------------------------------------*/

static void usb_host_hub_int_callback
   (
      /* [IN] pointer to pipe */
      void*           tr,

      /* [IN] user-defined parameter */
      void*           param,

      /* [IN] buffer address */
      uint8_t*          buffer,

      /* [IN] length of data transferred */
      uint32_t           len,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      usb_status        status
   )
{ /* Body */
    hub_device_struct_t*          hub_instance = (hub_device_struct_t*) param;
    hub_command_t                 hub_com;
    int32_t                       i, j, port = 0;
    uint8_t *                     port_pattern= (uint8_t *) buffer;
    dev_instance_t*               dev_instance_ptr = (dev_instance_t*)hub_instance->dev_handle;
    usb_host_state_struct_t*      usb_host_ptr = (usb_host_state_struct_t*)dev_instance_ptr->host;

#if _DEBUG
    //USB_PRINTF("%d bm 0x%x 0x%x\r\n", hub_instance->hub_level, port_pattern[0], hub_instance->state);
#endif
    hub_instance->in_recv = 0;
    
    if (hub_instance->to_be_deleted == 1)
    {
        OS_Sem_post(usb_host_ptr->hub_sem);
        return;
    }

    if (status != USB_OK)
    {
#if _DEBUG    
        USB_PRINTF("can't get level %d hub bit map\r\n", hub_instance->hub_level);
#endif
        return ;
    }
    
    hub_com.class_ptr      = hub_instance->class_handle;
    hub_com.callback_fn    = usb_host_hub_device_sm;
    hub_com.callback_param = hub_instance;
   
    /* find which port changed its state */
    for (i = 0; i < len; i++) 
    {
        if (!port_pattern[i])
            continue;
        port = i * 8;
        for (j = 0; j < 8; j++, port++) 
        {
            if (port_pattern[i] & 0x01)
                break;
            port_pattern[i] >>= 1;
        }
        break;
    }

    /* The port number, which changed status, is written in variable "port"
    ** Note, that if there are more ports, which changed its status, these will
    ** be invoked later (in next interrupt)
    */

    if (port == 0)
    {
        hub_instance->state = HUB_GET_STATUS_ASYNC;
        if (USB_OK != usb_class_hub_get_status(&hub_com, hub_instance->port_status_buffer, 4))
        {
#if _DEBUG        
            USB_PRINTF("usb_class_hub_get_status failed in usb_host_hub_int_callback\r\n");
#endif            
            hub_instance->state = HUB_NONE;
        }
        else
        {
#if _DEBUG        
            USB_PRINTF("try to get status\r\n");
#endif            
            hub_instance->in_control = 1;
        }
    }
    else {

        if((hub_instance->state != HUB_GET_PORT_STATUS_ASYNC )&&(hub_instance->in_control != 1))
        {
            hub_instance->state = HUB_GET_PORT_STATUS_ASYNC;
            hub_instance->port_iterator = port - 1;
            if (USB_OK != usb_class_hub_get_port_status(&hub_com, port, hub_instance->port_status_buffer, 4))
            {
#if _DEBUG            
                USB_PRINTF("usb_class_hub_get_port_status failed in usb_host_hub_int_callback\r\n");
#endif                
                hub_instance->state = HUB_NONE;
            }
            else
            {
                hub_instance->in_control = 1;
            }
        }
    }
} /*EndBody */


void usb_host_hub_Port_Reset(hub_device_struct_t* hub_instance,uint8_t port)
{
    hub_command_t                   hub_com;

    hub_com.class_ptr      = hub_instance->class_handle;
    hub_com.callback_fn    = usb_host_hub_device_sm;
    hub_com.callback_param = hub_instance;
    
    hub_instance->state = HUB_GET_PORT_STATUS_ASYNC;

    usb_class_hub_set_port_feature(&hub_com, port, PORT_RESET);
}

#endif




