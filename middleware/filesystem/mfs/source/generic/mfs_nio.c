/*HEADER**********************************************************************
*
* Copyright 2008-2014 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
* Copyright 1989-2008 ARC International
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains adaptation layer for NIO subsystem
*
*
*END************************************************************************/

#include "mfs.h"
#include "mfs_prv.h"

struct init_t
{
    uint32_t part_num;
    int dev_fd; /*[IN] the device on which to install MFS */
};

static int _io_mfs_deinit(void *dev_context, int *error);
static int _io_mfs_open(void *dev_context, const char *open_name_ptr, int flags, void **fp_context, int *error);
static int _io_mfs_close(void *dev_context, void *fp_context, int *error);
static int _io_mfs_read(void *dev_context, void *fp_context, void *buf, size_t nbytes, int *error);
static int _io_mfs_write(void *dev_context, void *fp_context, const void *data_ptr, size_t num, int *error);
static _nio_off_t _io_mfs_lseek(void *dev_context, void *fp_context, _nio_off_t offset, int whence, int *error);
static int _io_mfs_ioctl(void *dev_context, void *fp_context, int *error, unsigned long int request, va_list ap);
static int _io_mfs_init(void *init_data, void **dev_context, int *error);

static const NIO_DEV_FN_STRUCT _nio_mfs = {
    .OPEN = _io_mfs_open,
    .READ = _io_mfs_read,
    .WRITE = _io_mfs_write,
    .LSEEK = _io_mfs_lseek,
    .IOCTL = _io_mfs_ioctl,
    .CLOSE = _io_mfs_close,
    .INIT = _io_mfs_init,
    .DEINIT = _io_mfs_deinit,
};

/*!
 * \brief Initialize the MSDOS File System.
 *
 * \param[in] dev_fd The device on which to install MFS.
 * \param[in] identifier The name that should be given to mfs (ex: "C:", "MFS1:", etc..).
 * \param[in] partition_num The partition number to install MFS on. 0 for no partitions.
 *
 * \return int Error code.
 */
int _io_mfs_install(
    int dev_fd,
    char *identifier,
    uint32_t partition_num)
{
    uint32_t error_code;

    struct init_t mfs_init = {
        .dev_fd = dev_fd,
        .part_num = partition_num,
    };

    if (NULL == _nio_dev_install(identifier,
                                 &_nio_mfs,
                                 (void *)&mfs_init,
                                 NULL))
    {
        error_code = MFS_INSUFFICIENT_MEMORY;
    }
    else
    {
        error_code = MFS_NO_ERROR;
    }

    return error_code;
}

/*!
 * \brief Initialize the MSDOS File System.
 *
 * \param init_data
 * \param dev_context
 *
 * \return int Error code.
 */
static int _io_mfs_init(
    void *init_data,
    void **dev_context,
    int *error)
{
    MFS_DRIVE_STRUCT_PTR drive_ptr;
    struct init_t *mfs_init = (struct init_t *)init_data;
    _mfs_error error_code;

    error_code = MFS_Create_drive(&drive_ptr);
    if (error_code)
    {
        return error_code;
    }

    /* Fill in storage device handle */
    drive_ptr->DEV_FILE_PTR = mfs_init->dev_fd;
    drive_ptr->DRV_NUM = mfs_init->part_num;

    //_mem_set_type(drive_ptr, MEM_TYPE_MFS_DRIVE_STRUCT);
    // Inform Kernel that MFS is installed
    //_mqx_set_io_component_handle(IO_MFS_COMPONENT,(void *)MFS_VERSION);

    *dev_context = (void *)drive_ptr;

    return 0;
}


/*!
 * \brief Initialize the MSDOS File System.
 *
 * \param[in] identifier The name that should be given to mfs (ex: "C:", "MFS1:", etc..).
 *
 * \return int Error code.
 */
int _io_mfs_uninstall(
    char *identifier)
{
    uint32_t retval;

    if (-1 == _nio_dev_uninstall(identifier, NULL))
    {
        retval = MFS_ACCESS_DENIED;
    }
    else
    {
        retval = MFS_NO_ERROR;
    }

    return retval;
}


/*!
 * \brief Uninstalls the MSDOS File System and frees all memory allocated to it.
 *
 * \param[in] dev_context The identifier of the device.
 *
 * \return int Error code.
 */
static int _io_mfs_deinit(
    void *dev_context,
    int *error)
{
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR)dev_context;
    if (MFS_NO_ERROR != MFS_Destroy_drive(drive_ptr))
    {
        if (error)
        {
            *error = NIO_EIO;
        }
        return -1;
    }

    return 0;
}


/*!
 * \brief Opens and initializes MFS driver.
 *
 * \param dev_context
 * \param open_name_ptr
 * \param fflags
 * \param fp_context
 *
 * \return int MFS_NO_ERROR or an error.
 */
static int _io_mfs_open(
    void *dev_context,
    const char *open_name_ptr,
    int fflags,
    void **fp_context,
    int *error)
{
    MFS_DRIVE_STRUCT_PTR drive_ptr;
    MFS_HANDLE_PTR handle = NULL;
    _mfs_error error_code = 0;

    drive_ptr = (MFS_DRIVE_STRUCT_PTR)dev_context;
    *fp_context = NULL;

    if (!drive_ptr->DEV_OPEN)
    {
        /* This means we are opening MFS and not a file . */
        error_code = MFS_Open_Device(drive_ptr);
        if (error_code == MFS_NOT_A_DOS_DISK)
        {
            /*
            ** This error is OK. It just means the disk probably has to be
            ** formatted
            */
            error_code = 0;
        }
        if (error_code == 0)
        {
            drive_ptr->DEV_OPEN = true;
        }
    }

    else
    {
        /* Check for filename */
        open_name_ptr = MFS_Parse_Out_Device_Name((char *)open_name_ptr);
        if (*open_name_ptr == '\0')
        {
            error_code = MFS_PATH_NOT_FOUND;
        }
        else
        {
            /* Open or create the file */
            handle = MFS_Open_file(drive_ptr, (char *)open_name_ptr, fflags, &error_code);

            if (error_code != MFS_NO_ERROR)
            {
                /* There is an error, free the file handle if any. This is to cover potential corner case which should not happen */
                if (handle != NULL)
                {
                    MFS_Close_file(drive_ptr, handle);
                    handle = NULL;
                }
            }
            else if (handle == NULL)
            {
                /* There is no error but the file handle is not NULL. This is to cover potential corner case which should not happen */
                error_code = MFS_INVALID_HANDLE;
            }
            else
            {
                *fp_context = handle;
            }
        }
    }

    if (error_code)
    {
        if (error)
        {
            *error = error_code;
        }
        return -1;
    }

    return 0;
}


/*!
 * \brief Closes MFS driver.
 *
 * \param dev_context
 * \param fp_context
 *
 * \return int Error code.
 */
static int _io_mfs_close(
    void *dev_context,
    void *fp_context,
    int *error)
{
    MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR)fp_context;
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR)dev_context;
    _mfs_error error_code = 0;

    if (handle == NULL)
    {
        /* We are closing the mfs_fd_ptr, and not a normal file */
        error_code = MFS_Close_Device(drive_ptr);
        drive_ptr->DEV_OPEN = false;
    }
    else
    {
        /* We are closing a normal file */
        error_code = MFS_Close_file(drive_ptr, handle);
    }

    if (error_code)
    {
        if (error)
        {
            *error = error_code;
        }
        return -1;
    }

    return 0;
}

/*!
 * \brief Reads data from MFS driver.
 *
 * \param dev_context
 * \param fp_context
 * \param data_ptr
 * \param[in] num
 *
 * \return int Number of characters read.
 */
static int _io_mfs_read(
    void *dev_context,
    void *fp_context,
    void *data_ptr,
    size_t num,
    int *error)
{
    MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR)fp_context;
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR)dev_context;
    int32_t result;
    _mfs_error error_code;

    result = MFS_Read(drive_ptr, handle, num, data_ptr, &error_code);

    if ((error_code) != 0 && (error_code != MFS_EOF))
    {
        if (error)
        {
            *error = error_code;
        }
        return -1;
    }

    return result;
}


/*!
 * \brief Writes data to the fdv_ram device.
 *
 * \param dev_context
 * \param fp_context
 * \param data_ptr
 * \param[in] num
 *
 * \return int Number of characters written.
 */
static int _io_mfs_write(
    void *dev_context,
    void *fp_context,
    const void *data_ptr,
    size_t num,
    int *error)
{
#if !MFSCFG_READ_ONLY

    MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR)fp_context;
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR)dev_context;
    int32_t result;
    _mfs_error error_code;

    if ((data_ptr == NULL) && (num == 0))
    {
        error_code = MFS_Flush_Device(drive_ptr, handle);
    }
    else
    {
        result = MFS_Write(drive_ptr, handle, num, (char *)data_ptr, &error_code);
    }

    if ((error_code) != 0 && (error_code != MFS_EOF))
    {
        if (error)
        {
            *error = error_code;
        }
        return -1;
    }

    return result;

#else  //MFSCFG_READ_ONLY

    return -MFS_OPERATION_NOT_ALLOWED;

#endif  //MFSCFG_READ_ONLY
}


/*!
 * \brief
 *
 * \param dev_context
 * \param fp_context
 * \param offset
 * \param[in] mode
 *
 * \return _nio_off_t The returned value is IO_EOF or a MQX error code.
 */
static _nio_off_t _io_mfs_lseek(
    void *dev_context,
    void *fp_context,
    _nio_off_t offset,
    int mode,
    int *error)
{
    MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR)fp_context;
    _nio_off_t location;

    if (handle == NULL)
    {
        if (error)
        {
            *error = NIO_EINVAL;
        }
        return -1;
    }

    switch (mode)
    {
        case SEEK_SET:
            location = offset;
            break;
        case SEEK_CUR:
            location = handle->LOCATION + offset;
            break;
        case SEEK_END:
            location = handle->DIR_ENTRY->FILE_SIZE + offset;
            break;
        default:
            location = -1; /* Set location to invalid value */
            break;
    }

    /* Check validity of location */
    if (location < 0 || location > MFS_MAX_FILESIZE)
    {
        if (error)
        {
            *error = NIO_EINVAL;
        }
        return -1;
    }
    else
    {
        /* Store valid location */
        handle->LOCATION = location;
    }

    return location;
}


/*!
 * \brief
 *
 * \param dev_context
 * \param fp_context
 * \param[in] cmd
 * \param ap
 *
 * \return int The returned value is IO_EOF or a MQX error code.
 */
static int _io_mfs_ioctl(
    void *dev_context,
    void *fp_context,
    int *error,
    unsigned long int cmd,
    va_list ap)
{
    MFS_DRIVE_STRUCT_PTR drive_ptr = (MFS_DRIVE_STRUCT_PTR)dev_context;
    MFS_HANDLE_PTR handle = (MFS_HANDLE_PTR)fp_context;

    int result;

    result = MFS_vioctl(drive_ptr, handle, cmd, ap);
    if (result != MFS_NO_ERROR)
    {
        if (error)
        {
            *error = result;
        }
        return -1;
    }

    return 0;
}
