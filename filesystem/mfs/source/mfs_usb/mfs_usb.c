/**HEADER********************************************************************
* 
* Copyright (c) 2014 Freescale Semiconductor;
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
* $FileName: msd_diskio.c$
* $Version : 
* $Date    : 
*
* Comments: This file implements low level disk interface module for msd
*
*   
*
*END************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <nio.h>
#include <ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include "mfs_usb.h"


/**************************************************************************
   Micro variables
**************************************************************************/

#define UNUSED(x)                                  (void)x;


/**************************************************************************
*   Local variables
**************************************************************************/
static volatile bool                    g_bCallBack = FALSE;
static volatile usb_status              g_bStatus = USB_OK;


static int _io_usb_mfs_init(void *init_data, void **dev_context);
static int _io_usb_mfs_deinit(void *dev_context);
static int _io_usb_mfs_open(void *dev_context, const char *dev_name, int flags, void **fp_context);
static int _io_usb_mfs_close(void *dev_context, void *fp_context);
static int _io_usb_mfs_read(void *dev_context, void *fp_context, void *data_ptr, size_t num);
static int _io_usb_mfs_write(void *dev_context, void *fp_context, const void *data_ptr, size_t num);
static int _io_usb_mfs_ioctl(void *dev_context, void *fp_context, unsigned long int command, va_list ap);
static _nio_off_t _io_usb_mfs_lseek(void *dev_context, void *fp_context, _nio_off_t offset, int mode);

const NIO_DEV_FN_STRUCT usb_mfs_dev = {
    .OPEN = _io_usb_mfs_open,
    .READ = _io_usb_mfs_read,
    .WRITE = _io_usb_mfs_write,
    .LSEEK = _io_usb_mfs_lseek,
    .IOCTL = _io_usb_mfs_ioctl,
    .CLOSE = _io_usb_mfs_close,
    .INIT = _io_usb_mfs_init,
    .DEINIT = _io_usb_mfs_deinit,
};

static int msd_disk_ioctl (mass_command_struct_t *dev, uint8_t cmd, void* buff);
static void command_callback(usb_status status, void *p1, void *p2, uint32_t buffer_length);

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_install
* Returned Value   : uint32_t - a task error code or MQX_OK
* Comments         :
*    Install a  USB-MFS mass storage device driver.
*
*END*----------------------------------------------------------------------*/
int _io_usb_mfs_install
   (
      /* [IN] A string that identifies the device for fopen */
      char                          *identifier,

      /* [IN] Logical unit number which driver need to install */
      mfsmsd_device_t               *mass_device
   )
{ /* Body */
   /* we rely that the init_data will be passed to init function in this task */
   if (NULL == _nio_dev_install(identifier, &usb_mfs_dev, (void *)mass_device)) {
      return -1;
   }

   return 0;
} /* Endbody */

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_init
* Returned Value   : uint_32 error code
* Comments  :   Uninstalls the MSDOS File System and frees all memory allocated
*               to it. NIO driver init API
*
*END*---------------------------------------------------------------------*/
int _io_usb_mfs_init
   (
      void *init_data,
      void **dev_context
   )
{
    cbw_struct_t * cbw_ptr; 
    csw_struct_t * csw_ptr;
    struct mfsmsd_device *mfsmsd = (struct mfsmsd_device *)init_data;
    struct mfs_device *mfs_dev;
    

    mfs_dev = (struct mfs_device *)malloc(sizeof(*mfs_dev));
    if (NULL == mfs_dev)
    {
        return -ENOMEM;
    }
    
    cbw_ptr = (cbw_struct_t *) malloc(sizeof(*cbw_ptr));      
    if (NULL == cbw_ptr)
    {
        free(mfs_dev);
        return -ENOMEM;
    }
    

    csw_ptr = (csw_struct_t *) malloc(sizeof(*csw_ptr));
    if (NULL == csw_ptr)
    {
        free(mfs_dev);
        free(cbw_ptr);
        return -ENOMEM;
    }
   
    
    memset(cbw_ptr, 0, sizeof(cbw_struct_t));
    memset(csw_ptr, 0, sizeof(csw_struct_t));
    /* Store the address of CBW and CSW */
    mfs_dev->msd_dev.CBW_PTR = cbw_ptr;
    mfs_dev->msd_dev.CSW_PTR = csw_ptr;
    /* Init SCSI command object */
    mfs_dev->msd_dev.LUN      = mfsmsd->lun;
    mfs_dev->msd_dev.CLASS_PTR = (void *)mfsmsd->class_handle;
    mfs_dev->msd_dev.CALLBACK = command_callback;

    //TODO: make this not ioctl call, but call of static functions
    msd_disk_ioctl(&mfs_dev->msd_dev, UFI_TEST_UNIT_READY_CMD, NULL);      
    msd_disk_ioctl(&mfs_dev->msd_dev, UFI_REQUEST_SENSE_CMD, NULL);
    msd_disk_ioctl(&mfs_dev->msd_dev, UFI_INQUIRY_CMD, NULL);      
    msd_disk_ioctl(&mfs_dev->msd_dev, UFI_REQUEST_SENSE_CMD, NULL);
    msd_disk_ioctl(&mfs_dev->msd_dev, UFI_READ_CAPACITY_CMD, NULL);
    msd_disk_ioctl(&mfs_dev->msd_dev, UFI_REQUEST_SENSE_CMD, NULL);

    msd_disk_ioctl(&mfs_dev->msd_dev, GET_SECTOR_COUNT, &mfs_dev->BCOUNT);
    msd_disk_ioctl(&mfs_dev->msd_dev, GET_SECTOR_SIZE, &mfs_dev->BLENGTH);

   *dev_context = mfs_dev;

   return 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_deinit
* Returned Value   : uint32_t error code
* Comments  :   Uninstalls the MSDOS File System and frees all memory allocated
*               to it. NIO driver deinit API
*
*END*---------------------------------------------------------------------*/
int _io_usb_mfs_deinit
   (
       /* [IN] The device to uninstall */
      void *dev_context
   )
{
    struct mfs_device *mfs_dev = (struct mfs_device *)dev_context;

    assert(NULL != mfs_dev);
    assert(NULL != mfs_dev->msd_dev.CBW_PTR);
    assert(NULL != mfs_dev->msd_dev.CSW_PTR);
    free(mfs_dev->msd_dev.CBW_PTR);
    free(mfs_dev->msd_dev.CSW_PTR);
    free(mfs_dev);

    return 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_open
* Returned Value   : An error code or MQX_OK
* Comments         : Opens and initializes a USB-MFS mass storage device driver.
*                    Mass storage device should be initialized to prior to this
*                    this call.
*END*----------------------------------------------------------------------*/
int _io_usb_mfs_open
   (
      void *dev_context,
      const char *dev_name,
      int flags,
      void **fp_context
   )
{ /* Body */
   struct file_data *fd = (struct file_data *)malloc(sizeof(*fd));

   if (NULL == fd)
   {
      return -ENOMEM;
   }
   
   fd->LOCATION = 0;
   
   *fp_context = fd;
  
   return 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_close
* Returned Value   : ERROR CODE
* Comments         : Closes the USB mass storage link driver
*
*END*----------------------------------------------------------------------*/
int _io_usb_mfs_close
   (
      void *dev_context,

      void *fp_context
   )
{ /* Body */
   assert(NULL != fp_context);

   free(fp_context);

   return 0;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _io_usb_mfs_read
* Returned Value : 
* Comments       : Read Sector(s), NIO driver read API 
*
*END*--------------------------------------------------------------------*/
int _io_usb_mfs_read
   (
      void *dev_context,

      void *fp_context,

      void *data_ptr,

      size_t data_len
   )
{
    struct mfs_device *mfs_dev = (struct mfs_device *)dev_context;
    mass_command_struct_t *msd_dev = &mfs_dev->msd_dev;
    struct file_data *fi = (struct file_data *)fp_context;
    int             res = -1;
    usb_status      status = USB_OK;
    uint32_t        i = MAX_RETRY_TIMES;
    uint32_t        sector_count;
    uint32_t        sector_index;

    /* Parameter sanity check */
    if (!data_len || (data_len % mfs_dev->BLENGTH) || (fi->LOCATION % mfs_dev->BLENGTH))
    {
        return -EINVAL;
    }

    /* Conversion of byte offset and length to sector index and count */
    sector_count = data_len / mfs_dev->BLENGTH;
    sector_index = fi->LOCATION / mfs_dev->BLENGTH;

    /* Device size check */
    if (sector_index + sector_count > mfs_dev->BCOUNT)
    {
        return -EINVAL;
    }

    for( ; i > 0; i--)
    {
        /* disk_ioctl(pdrv, UFI_REQUEST_SENSE_CMD, NULL); */
        /* Send read_10 SCSI command */
        g_bCallBack = FALSE;
        status = usb_mass_ufi_read_10(msd_dev, sector_index, data_ptr, data_len, sector_count);
        if (status != USB_OK)
        {
            res = -EIO;
            break;
        }
        else
        {
            while(!g_bCallBack)    /* Wait till command comes back */
            {
              #if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
                      Poll();
              #elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
                          OSA_PollAllOtherTasks();
              #endif
            }

            if(msd_dev->IS_STALL_IN_DPHASE > 0)
            {
                continue;
            }

            if (!g_bStatus)
            {
                res = data_len;
                fi->LOCATION += data_len;
                break;
            }
            else
            {
                res = -EBUSY;
                break;
            }
        }
        /* disk_ioctl(pdrv, UFI_REQUEST_SENSE_CMD, NULL); */
    }

    return res;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _io_usb_mfs_write
* Returned Value : 
* Comments       : Write Sector(s), NIO driver write API
*
*END*--------------------------------------------------------------------*/

int _io_usb_mfs_write
   (
      void *dev_context,

      void *fp_context,

      const void *data_ptr,

      size_t data_len
   )
{
    struct mfs_device *mfs_dev = (struct mfs_device *)dev_context;
    mass_command_struct_t *msd_dev = &mfs_dev->msd_dev;
    struct file_data *fi = (struct file_data *)fp_context;
    int             res = -1;
    usb_status      status = USB_OK;
    uint32_t        i = MAX_RETRY_TIMES;
    uint32_t        sector_count;
    uint32_t        sector_index;

    /* Parameter sanity check */
    if (!data_len || (data_len % mfs_dev->BLENGTH) || (fi->LOCATION % mfs_dev->BLENGTH))
    {
        return -EINVAL;
    }

    /* Conversion of byte offset and length to sector index and count */
    sector_count = data_len / mfs_dev->BLENGTH;
    sector_index = fi->LOCATION / mfs_dev->BLENGTH;

    /* Device size check */
    if (sector_index + sector_count > mfs_dev->BCOUNT)
    {
        return -EINVAL;
    }

    for(; i > 0; i--)
    {
        /* disk_ioctl(pdrv, UFI_REQUEST_SENSE_CMD, NULL); */

        /* Send Write_10 SCSI command */
        g_bCallBack = FALSE;
        status = usb_mass_ufi_write_10(msd_dev, sector_index, (uint8_t*)data_ptr, data_len, sector_count);
        if (status != USB_OK)
        {
            res = -EIO;
        }
        else
        {
            while(!g_bCallBack)    /* Wait till command comes back */
            {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
            OSA_PollAllOtherTasks();
#endif
            }

            if (msd_dev->IS_STALL_IN_DPHASE > 0)
            {
                continue;
            }

            if (!g_bStatus)
            {
                res = data_len;
                fi->LOCATION += data_len;
                break;
            }
            else
            {
                res = -EBUSY;
                break;
            }
        }
        /* disk_ioctl(pdrv, UFI_REQUEST_SENSE_CMD, NULL); */
    }

    return res;
}

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_ioctl
* Returned Value   : int32_t
* Comments         :
*    Returns result of ioctl operation. NIO driver ioctl API
*
*END*-------------------------------------------------------------------------*/
int _io_usb_mfs_ioctl
   (
      void *dev_context,

      void *fp_context,

      unsigned long int command,

      va_list ap
   )
{ /* Body */
    struct mfs_device *mfs_dev = (struct mfs_device *)dev_context;
    uint32_t                      *param_ptr;
    int                           result = 0;

    switch (command) {
      case IO_IOCTL_GET_NUM_SECTORS:
         param_ptr = va_arg(ap, uint32_t *);
         *param_ptr = mfs_dev->BCOUNT;
         break;

      case IO_IOCTL_GET_BLOCK_SIZE:
         param_ptr = va_arg(ap, uint32_t *);
         *param_ptr = mfs_dev->BLENGTH;
         break;

      case IO_IOCTL_DEVICE_IDENTIFY:
         param_ptr = va_arg(ap, uint32_t *);
         param_ptr[0] = IO_DEV_TYPE_PHYS_USB_MFS;
         param_ptr[1] = IO_DEV_TYPE_LOGICAL_MFS;
         param_ptr[2] = IO_DEV_ATTR_ERASE | IO_DEV_ATTR_POLL
                         | IO_DEV_ATTR_READ | IO_DEV_ATTR_REMOVE
                         | IO_DEV_ATTR_SEEK | IO_DEV_ATTR_WRITE;
         break;
/*
      case IO_IOCTL_GET_VENDOR_INFO:
         param_ptr = va_arg(ap, uint32_t *);
         *param_ptr = (uint32_t)&(msd_dev->INQUIRY_DATA.BVID);
         break;

      case IO_IOCTL_GET_PRODUCT_ID:
         param_ptr = va_arg(ap, uint32_t *);
         *param_ptr = (uint32_t)&(msd_dev->INQUIRY_DATA.BPID);
         break;

      case IO_IOCTL_GET_PRODUCT_REV:
         param_ptr = va_arg(ap, uint32_t *);
         *param_ptr = (uint32_t)&(msd_dev->INQUIRY_DATA.BPRODUCT_REV);
         break;

      case IO_IOCTL_DEVICE_STOP:
         result = _io_usb_mfs_ioctl_stop(info_ptr);
         break;
*/

      default:
         result = -EINVAL;
         break;
   } /* Endswitch */

   return result;
} /* Endbody */



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : msd_disk_ioctl
* Returned Value : 
* Comments       : The disk_ioctl function controls device specified features
*                  and miscellaneous functions other than disk read/write 
*
*END*--------------------------------------------------------------------*/
static int msd_disk_ioctl (
    /* MSD command structure */
    mass_command_struct_t *dev,
    /* [IN] Control command code */
    uint8_t cmd, 
    /* [IN/OUT] Parameter or data buffer */   
    void* buff
)
{
    int                                                  res = -1;
    usb_status                                           status = USB_OK;
    /* TODO: move the data into stack and make the call blocking so the stack is valid along the response */
    static mass_storage_read_capacity_cmd_struct_t       read_capacity;
    static capacity_list_struct_t                        capacity_list;
    static inquiry_data_format_struct_t                  inquiry;
    static req_sense_data_format_struct_t                req_sense;
    uint32_t *                                           param_ptr = buff;

    switch (cmd)
    {
    case UFI_TEST_UNIT_READY_CMD:
        /* Send test unit ready SCSI command */
        g_bCallBack = FALSE;
        status = usb_mass_ufi_test_unit_ready(dev);
        if (status != USB_OK)
        {
            res = -EBUSY;
        }
        else
        {
            /* Wait till command comes back */
            while(!g_bCallBack)    
            {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
            OSA_PollAllOtherTasks();
#endif
            }
            
            if (!g_bStatus) 
            {                                
                res = 0;  
            }
            else 
            {              
                res = -EBUSY;
            }         
        }                  
        break;
    case UFI_READ_CAPACITY_CMD:
        /* Send read_capacity SCSI command */
        g_bCallBack = FALSE;
        status = usb_mass_ufi_read_capacity(dev, (uint8_t *)&read_capacity,
                                            sizeof(mass_storage_read_capacity_cmd_struct_t));
        if (status != USB_OK)
        {
            res = -EIO;
        }
        else
        {
            /* Wait till command comes back */
            while(!g_bCallBack)    
            {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
            OSA_PollAllOtherTasks();
#endif
            }     
            
            if (!g_bStatus) 
            {
                res = 0;   
            }
            else 
            {
                res = -EBUSY;
            }         
        }                  
        break;
    case UFI_READ_FORMAT_CAPACITY_CMD:
        /* Send read_capacity SCSI command */
        g_bCallBack = FALSE;
        status = usb_mass_ufi_format_capacity(dev, (uint8_t *)&capacity_list,\
            sizeof(capacity_list_struct_t));
        if (status != USB_OK)
        {
            res = -EIO;   
        }
        else
        {
            /* Wait till command comes back */
            while(!g_bCallBack)    
            {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
            OSA_PollAllOtherTasks();
#endif
            }
            
            if (!g_bStatus) 
            {
                res = 0;    
            }
            else 
            {
                res = -EBUSY;
            }         
        }                  
        break;
    case UFI_INQUIRY_CMD:
        /* Send read_capacity SCSI command */
        g_bCallBack = FALSE;
        status = usb_mass_ufi_inquiry(dev, (uint8_t *) &inquiry,
                                      sizeof(inquiry_data_format_struct_t));
        if (status != USB_OK)
        {
            res = -EBUSY;
        }
        else
        {
            /* Wait till command comes back */
            while(!g_bCallBack)    
            {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
            OSA_PollAllOtherTasks();
#endif
            }
            
            if (!g_bStatus) 
            {
                inquiry.BVID[7] = 0;
                inquiry.BPID[15] = 0;
                inquiry.BPRODUCT_REV[3] = 0;
                res = 0;
            }
            else 
            {
                res = -EBUSY;
            }
        }                  
        break;

    case UFI_REQUEST_SENSE_CMD:
        /* Send read_capacity SCSI command */
        g_bCallBack = FALSE;
        status = usb_mass_ufi_request_sense(dev, &req_sense,
                                            sizeof(req_sense_data_format_struct_t));
        if (status != USB_OK)
        {
            res = -EIO;
        }
        else
        {
            /* Wait till command comes back */
            while(!g_bCallBack)
            {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
            OSA_PollAllOtherTasks();
#endif
            }
            
            if (!g_bStatus)
            {
                res = 0; 
            }
            else 
            {
                res = -EBUSY;
            }
        }          
        break;
    case GET_SECTOR_COUNT:
    case GET_SECTOR_SIZE:
        /* Send read_capacity SCSI command */
        g_bCallBack = FALSE;
        status = usb_mass_ufi_read_capacity(dev, (uint8_t *)&read_capacity,\
            sizeof(mass_storage_read_capacity_cmd_struct_t));
        if (status != USB_OK)
        {
            res = -EIO;
        }
        else
        {
            /* Wait till command comes back */
            while(!g_bCallBack)
            {
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_BM)
        Poll();
#elif ((OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK) && !(USE_RTOS))
            OSA_PollAllOtherTasks();
#endif
            }
            
            if (!g_bStatus)
            {
                res = 0;    
            }
            else
            {
                res = -EBUSY;
            } 
        }
        
        if (!buff) {
            res = -EINVAL;
        }
        /* Get number of sectors on the disk (DWORD) */
        else if (GET_SECTOR_COUNT == cmd)
        {
            *(uint32_t *)buff = SWAP4BYTE_CONST(*((uint32_t *)(read_capacity.BLLBA))) + 1;
        }
        /* Get the sector size in byte */
        else
        {
            *(uint32_t *)buff = SWAP4BYTE_CONST(*((uint32_t *)(read_capacity.BLENGTH)));
        }
        break;          
    case GET_BLOCK_SIZE:
        if(!buff) {
            res = -EINVAL;
        }
        else {
            *(uint32_t*)buff = ERASE_BLOCK_SIZE;
            res = 0;
        }
        break;
    case CTRL_SYNC:
        res = 0;    
        break;
    case GET_VENDOR_INFO:
        *param_ptr = (uint32_t)&inquiry.BVID;
        break;
    case GET_PRODUCT_ID:
        *param_ptr = (uint32_t)&inquiry.BPID;
        break;
    case GET_PRODUCT_REV:
        *param_ptr = (uint32_t)&inquiry.BPRODUCT_REV;
        break;
    default:
        res = -EINVAL;
        break;
    }  

    return res;
}

/*FUNCTION*---------------------------------------------------------------------
*
* Function Name    : _io_usb_mfs_lseek
* Returned Value   : int_32
* Comments         :
*    Returns result of ioctl operation.
*
*END*-------------------------------------------------------------------------*/
_nio_off_t _io_usb_mfs_lseek
   (
       void *dev_context,
       void *fp_context,
       _nio_off_t offset,
       int mode
   )
{ /* Body */
    struct mfs_device *mfs_dev = (struct mfs_device *)dev_context;
    struct file_data *fi = (struct file_data *)fp_context;
    _nio_off_t location;

    switch (mode) {
        case SEEK_SET:
            location = offset;
            break;
        case SEEK_CUR:
            location = fi->LOCATION + offset;
            break;
        case SEEK_END:
            location = ((uint64_t)mfs_dev->BCOUNT * mfs_dev->BLENGTH) + offset;
            break;
        default:
            location = -1; /* Set location to invalid value */
            break;
    }

    /* Check validity of location */
    if (location < 0) {
        location = -EINVAL;
    }
    else {
        /* Store valid location */
        fi->LOCATION = location;
    }

    return location;
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : command_callback
* Returned Value : None
* Comments       : Called on completion of a control-pipe transaction.
*
*END*--------------------------------------------------------------------*/
static void command_callback
   (
      /* [IN] Status of this command */
      usb_status status,

      /* [IN] void * to USB_MASS_BULK_ONLY_REQUEST_STRUCT*/
      void *  p1,

      /* [IN] void * to the command object*/
      void *  p2,

      /* [IN] Length of data transmitted */
      uint32_t buffer_length
   )
{ /* Body */

   UNUSED(p1)
   UNUSED(p2)
   UNUSED(buffer_length)

   g_bCallBack = TRUE;
   g_bStatus = status;

} /* Endbody */
