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
 * $FileName: disk.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 * @brief  RAM Disk has been emulated via this Mass Storage Demo
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"

#include "disk.h"
#include "usb_class_msc.h"
#if SD_CARD_APP
#include "SD_esdhc_kinetis.h"
#endif

/*****************************************************************************
 * Constant and Macro's - None
 *****************************************************************************/

/*****************************************************************************
 * Global Functions Prototypes
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/
/* Add all the variables needed for disk.c to this structure */
disk_struct_t* g_msc_disk_ptr;

#if SD_CARD_APP
#define USE_SDHC_PROTOCOL    (1)
#define USE_SPI_PROTOCOL     (0)
#endif

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/
void Disk_USB_App_Device_Callback(uint8_t event_type, void* val, void* arg);
uint8_t Disk_USB_App_Class_Callback
(uint8_t event_type,
    uint16_t value,
    uint8_t ** data,
    uint32_t* size,
    void* arg
) ;
void Disk_App(void);
/*****************************************************************************
 * Local Variables 
 *****************************************************************************/
#if SD_CARD_APP
uint8_t *g_msc_bulk_out_buff;
uint8_t *g_msc_bulk_in_buff;
#endif
/*****************************************************************************
 * Local Functions
 *****************************************************************************/

/******************************************************************************
 * 
 *    @name       Disk_App
 *    
 *    @brief      
 *                  
 *    @param      None
 * 
 *    @return     None
 *    
 *****************************************************************************/
void Disk_App(void)
{
    /* User Code */
    return;
}

/******************************************************************************
 * 
 *    @name        Disk_USB_App_Device_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       pointer : 
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
void Disk_USB_App_Device_Callback(uint8_t event_type, void* val, void* arg)
{
    UNUSED_ARGUMENT (arg)
    UNUSED_ARGUMENT (val)
    if (event_type == USB_DEV_EVENT_BUS_RESET)
    {
        g_msc_disk_ptr->start_app = FALSE;
        if (USB_OK == USB_Class_MSC_Get_Speed(g_msc_disk_ptr->app_handle, &g_msc_disk_ptr->speed))
        {
            USB_Desc_Set_Speed(g_msc_disk_ptr->app_handle, g_msc_disk_ptr->speed);
        }
    }
    else if (event_type == USB_DEV_EVENT_ENUM_COMPLETE)
    {
        g_msc_disk_ptr->start_app = TRUE;
    }
    else if (event_type == USB_DEV_EVENT_ERROR)
    {
        /* add user code for error handling */
    }
    else if (event_type == USB_MSC_DEVICE_GET_SEND_BUFF_INFO)
    {
        if (NULL != val)
        {
#if SD_CARD_APP
            *((uint32_t *)val) = (uint32_t)MSD_SEND_BUFFER_SIZE;
#elif RAM_DISK_APP
            *((uint32_t *)val) = (uint32_t)DISK_SIZE;
#endif
        }
    }
    else if (event_type == USB_MSC_DEVICE_GET_RECV_BUFF_INFO)
    {
        if (NULL != val)
        {
#if SD_CARD_APP
            *((uint32_t *)val) = (uint32_t)MSD_RECV_BUFFER_SIZE;
#elif RAM_DISK_APP
            *((uint32_t *)val) = (uint32_t)DISK_SIZE;
#endif
        }
    }

    return;
}

/******************************************************************************
 * 
 *    @name        Disk_USB_App_Class_Callback
 *    
 *    @brief       This function handles the callback  
 *                  
 *    @param       pointer : 
 *    @param       event_type : value of the event
 *    @param       val : gives the configuration value 
 * 
 *    @return      None
 *
 *****************************************************************************/
uint8_t Disk_USB_App_Class_Callback
(uint8_t event_type,
    uint16_t value,
    uint8_t ** data,
    uint32_t* size,
    void* arg
) 
{
    lba_app_struct_t* lba_data_ptr;
    uint8_t * prevent_removal_ptr;
    //uint8_t * load_eject_start_ptr = NULL;   
    device_lba_info_struct_t* device_lba_info_ptr;
    uint8_t error = USB_OK;

    //UNUSED_ARGUMENT (arg)

    switch(event_type)
    {
    case USB_DEV_EVENT_DATA_RECEIVED:
        /* Add User defined code -- if required*/
        lba_data_ptr = (lba_app_struct_t*) size;

#if RAM_DISK_APP
#elif SD_CARD_APP
        SD_Write_Block(lba_data_ptr);
#endif
        break;
    case USB_DEV_EVENT_SEND_COMPLETE:
        /* Add User defined code -- if required*/
        lba_data_ptr = (lba_app_struct_t*) size;
        /* read data from mass storage device to driver buffer */
#if RAM_DISK_APP
        if(data != NULL)
        {
            *data = g_msc_disk_ptr->storage_disk + lba_data_ptr->offset;
        }
#elif SD_CARD_APP
#endif
        break;
    case USB_MSC_START_STOP_EJECT_MEDIA:
        /*    Code to be added by user for starting, stopping or 
         ejecting the disk drive. e.g. starting/stopping the motor in 
         case of CD/DVD*/
        break;
    case USB_MSC_DEVICE_READ_REQUEST:
        /* copy data from storage device before sending it on USB Bus 
         (Called before calling send_data on BULK IN endpoints)*/
        lba_data_ptr = (lba_app_struct_t*) size;
        /* read data from mass storage device to driver buffer */
#if RAM_DISK_APP
        if(data != NULL)
        {
            *data = g_msc_disk_ptr->storage_disk + lba_data_ptr->offset;
        }
#elif SD_CARD_APP
        if(data != NULL)
        {
            *data = g_msc_bulk_in_buff;
        }
        lba_data_ptr->buff_ptr = g_msc_bulk_in_buff;
        SD_Read_Block(lba_data_ptr);
#endif         
        break;
    case USB_MSC_DEVICE_WRITE_REQUEST:
        /* copy data from USb buffer to Storage device 
         (Called before after recv_data on BULK OUT endpoints)*/
        lba_data_ptr = (lba_app_struct_t*) size;
        /* read data from driver buffer to mass storage device */
#if RAM_DISK_APP
        if(data != NULL)
        {
            *data = g_msc_disk_ptr->storage_disk + lba_data_ptr->offset;
        }
#elif SD_CARD_APP
        if(data != NULL)
        {
            *data = g_msc_bulk_out_buff;
        }
#endif                          
        break;
    case USB_MSC_DEVICE_FORMAT_COMPLETE:
        break;
    case USB_MSC_DEVICE_REMOVAL_REQUEST:
        prevent_removal_ptr = (uint8_t *) size;
        if (SUPPORT_DISK_LOCKING_MECHANISM)
        {
            g_msc_disk_ptr->disk_lock = *prevent_removal_ptr;
        }
        else if ((!SUPPORT_DISK_LOCKING_MECHANISM) && (!(*prevent_removal_ptr)))
        {
            /*there is no support for disk locking and removal of medium is enabled*/
            /* code to be added here for this condition, if required */
        }
        break;
    case USB_MSC_DEVICE_GET_INFO:
        device_lba_info_ptr = (device_lba_info_struct_t*) size;
#if RAM_DISK_APP
        device_lba_info_ptr->total_lba_device_supports = TOTAL_LOGICAL_ADDRESS_BLOCKS;
        device_lba_info_ptr->length_of_each_lab_of_device = LENGTH_OF_EACH_LAB;
#elif SD_CARD_APP
        SD_Card_Info(&device_lba_info_ptr->total_lba_device_supports,
        &device_lba_info_ptr->length_of_each_lab_of_device);
#endif
        device_lba_info_ptr->num_lun_supported = LOGICAL_UNIT_SUPPORTED;
        break;
    default:
        break;
    }

    return error;
}

/******************************************************************************
 *  
 *   @name        msc_disk_preinit
 * 
 *   @brief       This function pre-initializes the App.
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/

void msc_disk_preinit(void)
{

#if SD_CARD_APP
#if (defined _MK_xxx_H_)
#if USE_SDHC_PROTOCOL
    GPIOE_PDIR |= 1 << 28;
    PORTE_PCR28 |= PORT_PCR_MUX(1);
    GPIOE_PDDR &= ~((uint32_t)1 << 28);
    PORTE_PCR28 |= PORT_PCR_PE_MASK|PORT_PCR_PS_MASK;
#endif // USE_SDHC_PROTOCOL

    _SD_DE; /* Card detection */
#endif
#if (defined _MK_xxx_H_) ||  defined(MCU_mcf51jf128)
    while(SD_DE&kSD_Desert)
    {
    } /* SD Card inserted */
#else
    while(SD_DE == kSD_Desert)
    {
    } /* SD Card inserted */
#endif
    if(!SD_Init()) return; /* Initialize SD_CARD and SPI Interface */
#endif
}

/******************************************************************************
 *  
 *   @name        msc_disk_init
 * 
 *   @brief       This function initializes the App.
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/

void msc_disk_init(void *param)
{
    g_msc_disk_ptr = (disk_struct_t*) param;
    g_msc_disk_ptr->speed = USB_SPEED_FULL;
}
/******************************************************************************
 *  
 *   @name        Disk_TestApp_Task
 * 
 *   @brief       This function runs the App task.
 * 
 *   @param       None
 * 
 *   @return      None
 **                
 *****************************************************************************/

void msc_disk_task(void)
{
    /* call the periodic task function */
    USB_MSC_Periodic_Task();

    /*check whether enumeration is complete or not */
    if (g_msc_disk_ptr->start_app == (uint32_t) TRUE)
    {
        Disk_App();
    }
}

/* EOF */
