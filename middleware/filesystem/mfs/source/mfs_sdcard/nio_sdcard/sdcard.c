/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains the sd card driver functions.
*
*
*END************************************************************************/

#include <mqx.h>
#include <stdint.h>
#include <stdio.h>

#include <nio.h>
#include <ioctl.h>
#include <fcntl.h>
#include <fs_supp.h>

#include <sdcard_prv.h>
#include <sdcard.h>

static int nio_sdcard_open(void *dev_context, const char *dev_name, int flags, void **fp_context, int *error);
static int nio_sdcard_close(void *dev_context, void *fp_context, int *error);
static int nio_sdcard_ioctl(void *dev_context, void *fp_context, int *error, unsigned long int cmd, va_list ap);
static int nio_sdcard_read(void *dev_context, void *fp_context, void *data_ptr, size_t n, int *error);
static int nio_sdcard_write(void *dev_context, void *fp_context, const void *data_ptr, size_t n, int *error);
static int nio_sdcard_deinit(void *dev_context, int *error);
static int nio_sdcard_init(void *init_data, void **dev_context, int *error);
static _nio_off_t nio_sdcard_lseek(void *dev_context, void *fp_context, _nio_off_t offset, int whence, int *error);

const NIO_DEV_FN_STRUCT nio_sdcard_dev_fn = {
    .OPEN = nio_sdcard_open,
    .READ = nio_sdcard_read,
    .WRITE = nio_sdcard_write,
    .LSEEK = nio_sdcard_lseek,
    .IOCTL = nio_sdcard_ioctl,
    .CLOSE = nio_sdcard_close,
    .INIT = nio_sdcard_init,
    .DEINIT = nio_sdcard_deinit,
};

/*!
 * \brief Internal callback of NIO, initialization of the device and allocation of device context
 *
 * \param[in] init_data SDCARD_STRUCT_PTR init structure
 * \param[out] dev_context  SDCARD_STRUCT_PTR containing SDCard context variables
 *
 * \return SDCARD_OK on success, SDCARD_ERR on error
 */
static int nio_sdcard_init(

    void *init_data, void **dev_context, int *error)
{
    SDCARD_STRUCT_PTR sdcard_ptr;
    SDCARD_CONST_INIT_STRUCT_PTR init = ((SDCARD_INIT_STRUCT_PTR)init_data)->const_data;
    int com_device = ((SDCARD_INIT_STRUCT_PTR)init_data)->com_dev;

    if ((NULL == init) || (-1 == com_device))
    {
        if (error)
        {
            *error = SDCARD_ERR;
        }
        return -1;
    }

    sdcard_ptr = (SDCARD_STRUCT_PTR)_mem_alloc_system_zero((_mem_size)sizeof(SDCARD_STRUCT));
    if (sdcard_ptr == NULL)
    {
        if (error)
        {
            *error = SDCARD_ERR;
        }
        return -1;
    }

    _mem_set_type(sdcard_ptr, MEM_TYPE_IO_SDCARD_STRUCT);
    sdcard_ptr->INIT.INIT_FUNC = init->INIT_FUNC;
    sdcard_ptr->INIT.READ_FUNC = init->READ_FUNC;
    sdcard_ptr->INIT.WRITE_FUNC = init->WRITE_FUNC;
    sdcard_ptr->INIT.SIGNALS = init->SIGNALS;
    sdcard_ptr->COM_DEVICE = com_device;

    if (MQX_OK != _lwsem_create(&sdcard_ptr->LWSEM, 1))
    {
        _mem_free(sdcard_ptr);
        if (error)
        {
            *error = SDCARD_ERR;
        }
        return -1;
    }

    *dev_context = sdcard_ptr;

    return 0;
}

/*!
 * \brief Internal callback of NIO, deinitialization of sdcard and its device context
 *
 * \param[in] dev_context  SDCARD_STRUCT_PTR containing SDCard context variables
 *
 * \return SDCARD_OK on success, SDCARD_ERR on error
 */
static int nio_sdcard_deinit(void *dev_context, int *error)
{
    SDCARD_STRUCT_PTR sdcard_ptr = (SDCARD_STRUCT_PTR)dev_context;

    if (NULL == sdcard_ptr)
    {
        if (error)
        {
            *error = SDCARD_ERR;
        }
        return -1;
    }

    _lwsem_destroy(&sdcard_ptr->LWSEM);
    _mem_free(sdcard_ptr);

    return 0;
}

/*!
 * \brief Internal callback of NIO, This function opens the SDCard device.
 *
 * \param[in] dev_context  SDCARD_STRUCT_PTR containing SDCard context variables
 * \param[in] dev_name  Name of the device
 * \param[in] flags  The flags to be used during operation
 * \param[out] fp_context  The file context, unused by this driver
 *
 * \return SDCARD_OK on success, SDCARD_ERR on error
 */
static int nio_sdcard_open(
    /* Device context */
    void *dev_context,

    /* Name of the device */
    const char *dev_name,

    /* The flags to be used during operation */
    int flags,

    /* The file context, unused in this driver */
    void **fp_context,

    int *error)
{
    SDCARD_STRUCT_PTR sdcard_ptr = (SDCARD_STRUCT_PTR)dev_context;
    int result = 0;

    if (NULL != sdcard_ptr->INIT.INIT_FUNC)
    {
        _lwsem_wait(&sdcard_ptr->LWSEM);
        if (sdcard_ptr->INIT.INIT_FUNC(sdcard_ptr))
        {
            result = SDCARD_ERR;
        }
        _lwsem_post(&sdcard_ptr->LWSEM);
    }

    sdcard_ptr->LOCATION = 0;
    sdcard_ptr->FLAGS = flags;

    *fp_context = NULL;

    if (result)
    {
        if (error)
        {
            *error = SDCARD_ERR;
        }
        return -1;
    }

    return 0;
}


/*!
 * \brief Internal callback of NIO, This function closes an opened SDCard device.
 *
 * \param[in] dev_context  Containing SDCard context variables
 * \param[in] fp_context  The file context, unused by this driver
 *
 * \return SDCARD_OK on success, SDCARD_ERR on error
 */
static int nio_sdcard_close(
    /* Device context */
    void *dev_context,

    /* File context*/
    void *fp_context,

    int *error)
{
    /* Unused here */
    return 0;
}


/*!
 * \brief Internal callback of NIO. This function performs miscellaneous services for the SDCard I/O device.
 *
 * \param[in] dev_context  SDCARD_STRUCT_PTR containing SDCard context variables
 * \param[in] fp_context  The file context, unused by this driver
 * \param[in] cmd  The command to perform
 * \param[out] ap  Parameters for the command
 *
 * \return SDCARD_OK on success, SDCARD_ERR or NIO_EINVAL on error
 */
static int nio_sdcard_ioctl(
    /* Device context */
    void *dev_context,

    /* File context*/
    void *fp_context,

    /* Error code to fill */
    int *error,

    /* The command to perform */
    unsigned long int cmd,

    /* Parameters for the command */
    va_list ap)
{
    SDCARD_STRUCT_PTR sdcard_ptr = (SDCARD_STRUCT_PTR)dev_context;
    _mem_size_ptr mparam_ptr;
    _mqx_uint_ptr uparam_ptr;
    uint32_t *u32param_ptr;
    void *param_ptr;
    int result = 0;

    param_ptr = va_arg(ap, void *);

    switch (cmd)
    {
        case IO_IOCTL_SET_BLOCK_MODE:
            break;
        case IO_IOCTL_GET_BLOCK_SIZE:
            uparam_ptr = (_mqx_uint_ptr)param_ptr;
            *uparam_ptr = IO_SDCARD_BLOCK_SIZE;
            break;
        case IO_IOCTL_GET_NUM_SECTORS:
            mparam_ptr = (_mem_size_ptr)param_ptr;
            *mparam_ptr = (_mem_size)sdcard_ptr->NUM_BLOCKS;
            break;
        case IO_IOCTL_GET_REQ_ALIGNMENT:
            if (NULL == param_ptr)
            {
                result = NIO_EINVAL;
                break;
            }
            *((uint32_t *)param_ptr) = sdcard_ptr->ALIGNMENT;
            break;
        case IO_IOCTL_SDCARD_GET_CID:
            if (NULL == param_ptr)
            {
                result = NIO_EINVAL;
                break;
            }
            _mem_copy(sdcard_ptr->CID, param_ptr, sizeof(sdcard_ptr->CID));
            break;
        case IO_IOCTL_DEVICE_IDENTIFY:
            u32param_ptr = (uint32_t *)param_ptr;
            u32param_ptr[IO_IOCTL_ID_PHY_ELEMENT] = IO_DEV_TYPE_PHYS_SDCARD;
            u32param_ptr[IO_IOCTL_ID_LOG_ELEMENT] = IO_DEV_TYPE_LOGICAL_MFS;
            u32param_ptr[IO_IOCTL_ID_ATTR_ELEMENT] = IO_SDCARD_ATTRIBS;
            if ((sdcard_ptr->FLAGS & O_ACCMODE) == O_RDONLY)
            {
                u32param_ptr[IO_IOCTL_ID_ATTR_ELEMENT] &= (~IO_DEV_ATTR_WRITE);
            }
            break;
        default:
            result = NIO_EINVAL;
            break;
    }

    if (result)
    {
        if (error)
        {
            *error = SDCARD_ERR;
        }
        return -1;
    }

    return 0;
}

/*!
 * \brief Internal callback of NIO. Standard seek function for a seekable device.
 *
 * \param[in] dev_context  SDCARD_STRUCT_PTR containing SDCard context variables
 * \param[in] fp_context  The file context, unused by this driver
 * \param[in] offset Offset to apply
 * \param[out] whence Reference point of the seek operation (SEEK_SET, SEEK_CUR, SEEK_END)
 *
 * \return Location in bytes on success, -NIO_EINVAL on error
 */
static _nio_off_t nio_sdcard_lseek(void *dev_context, void *fp_context, _nio_off_t offset, int whence, int *error)
{
    SDCARD_STRUCT_PTR sdcard_ptr = (SDCARD_STRUCT_PTR)dev_context;
    int result = 0;

    _lwsem_wait(&sdcard_ptr->LWSEM);
    switch (whence)
    {
        case SEEK_SET:
            sdcard_ptr->LOCATION = offset;
            break;

        case SEEK_CUR:
            sdcard_ptr->LOCATION += offset;
            break;

        case SEEK_END:
            sdcard_ptr->LOCATION = (((_nio_off_t)sdcard_ptr->NUM_BLOCKS) << IO_SDCARD_BLOCK_SIZE_POWER) + offset;
            break;

        default:
            result = NIO_EINVAL;
            break;
    }
    _lwsem_post(&sdcard_ptr->LWSEM);

    if (result)
    {
        if (error)
        {
            *error = result;
        }
        return -1;
    }

    return (sdcard_ptr->LOCATION);
}


/*!
 * \brief Internal callback of NIO. Reads the data into provided buffer.
 *
 * \param[in] dev_context  SDCARD_STRUCT_PTR containing SDCard context variables
 * \param[in] fp_context  The file context, unused by this driver
 * \param[out] data_ptr  Where the characters are to be stored
 * \param[in] num  The number of bytes to read
 *
 * \return Returns the number of bytes received on success, SDCARD_ERR, -NIO_ENXIO or -NIO_EIO on error (negative)
 */
static int nio_sdcard_read(
    /* Device context */
    void *dev_context,

    /* File context */
    void *fp_context,

    /* [OUT] Where the characters are to be stored */
    void *data_ptr,

    /* [IN] The number of bytes to read */
    size_t num,

    int *error)
{
    SDCARD_STRUCT_PTR sdcard_ptr = (SDCARD_STRUCT_PTR)dev_context;
    int result;

    if ((NULL == sdcard_ptr->INIT.READ_FUNC) || ((sdcard_ptr->FLAGS & O_ACCMODE) == O_WRONLY))
    {
        if (error)
        {
            *error = NIO_ENXIO;
        }
        return -1;
    }

    if (num == 0)
    {
        return 0;
    }

    if (((sdcard_ptr->LOCATION + num) >> IO_SDCARD_BLOCK_SIZE_POWER) > sdcard_ptr->NUM_BLOCKS)
    {
        num = sdcard_ptr->NUM_BLOCKS - (sdcard_ptr->LOCATION >> IO_SDCARD_BLOCK_SIZE_POWER);
    }
    else if ((sdcard_ptr->LOCATION >> IO_SDCARD_BLOCK_SIZE_POWER) >= sdcard_ptr->NUM_BLOCKS)
    {
        if (error)
        {
            *error = NIO_ENXIO;
        }
        return -1;
    }

    _lwsem_wait(&sdcard_ptr->LWSEM);
    result = sdcard_ptr->INIT.READ_FUNC(sdcard_ptr, (unsigned char *)data_ptr, sdcard_ptr->LOCATION >> IO_SDCARD_BLOCK_SIZE_POWER, num >> IO_SDCARD_BLOCK_SIZE_POWER);
    _lwsem_post(&sdcard_ptr->LWSEM);

    if (result > 0)
    {
        sdcard_ptr->LOCATION += result;
    }

    if (result != num)
    {
        if (error)
        {
            *error = NIO_ENXIO;
        }
        return -1;
    }

    return result;
}

/*!
 * \brief Internal callback of NIO. Writes the provided data buffer to the device.
 *
 * \param[in] dev_context  SDCARD_STRUCT_PTR containing SDCard context variables
 * \param[in] fp_context  The file context, unused by this driver
 * \param[in] data_ptr  Where the characters are to be taken from
 * \param[in] num  The number of bytes to write
 *
 * \return Returns number of bytes transmitted on success, SDCARD_ERR, -NIO_ENOSPC or -NIO_EIO on error (negative)
 */
static int nio_sdcard_write(
    /* Device context */
    void *dev_context,

    /* File context */
    void *fp_context,

    /* [OUT] Where the characters are to be read */
    void const *data_ptr,

    /* [IN] The number of blocks to read */
    size_t num,

    int *error)
{
    SDCARD_STRUCT_PTR sdcard_ptr = (SDCARD_STRUCT_PTR)dev_context;
    int result;

    if ((NULL == sdcard_ptr->INIT.WRITE_FUNC) || ((sdcard_ptr->FLAGS & O_ACCMODE) == O_RDONLY))
    {
        /* Read-only access! */
        if (error)
        {
            *error = NIO_ENOSPC;
        }
        return -1;
    }

    if (num == 0)
    {
        return 0;
    }


    if (((sdcard_ptr->LOCATION + num) >> IO_SDCARD_BLOCK_SIZE_POWER) > sdcard_ptr->NUM_BLOCKS)
    {
        num = sdcard_ptr->NUM_BLOCKS - (sdcard_ptr->LOCATION >> IO_SDCARD_BLOCK_SIZE_POWER);
    }
    else if ((sdcard_ptr->LOCATION >> IO_SDCARD_BLOCK_SIZE_POWER) >= sdcard_ptr->NUM_BLOCKS)
    {
        if (error)
        {
            *error = NIO_ENOSPC;
        }
        return -1;
    }

    _lwsem_wait(&sdcard_ptr->LWSEM);
    result = sdcard_ptr->INIT.WRITE_FUNC(sdcard_ptr, (unsigned char *)data_ptr, sdcard_ptr->LOCATION >> IO_SDCARD_BLOCK_SIZE_POWER, num >> IO_SDCARD_BLOCK_SIZE_POWER);
    _lwsem_post(&sdcard_ptr->LWSEM);

    if (result > 0)
    {
        sdcard_ptr->LOCATION += result;
    }

    if (result != num)
    {
        if (error)
        {
            *error = NIO_EIO;
        }
        return -1;
    }

    return result;
}


/*!
 * \brief Extracts capacity information from CSD
 *
 * \param[in] csd  Pointer to CSD
 *
 * \return Capacity of storage device in 512B blocks
 */
uint32_t _io_sdcard_csd_capacity(
    /* [IN] pointer to CSD */
    uint8_t *csd)
{
    uint32_t capacity, c_size, c_size_mult, read_bl_len;

    /* check CSD version */
    if (0 == (csd[0] & 0xC0))
    {
        read_bl_len = csd[5] & 0x0F;
        c_size = csd[6] & 0x03;
        c_size = (c_size << 8) | csd[7];
        c_size = (c_size << 2) | (csd[8] >> 6);
        c_size_mult = ((csd[9] & 0x03) << 1) | (csd[10] >> 7);
        capacity = (c_size + 1) * (1 << (c_size_mult + 2)) * (1 << (read_bl_len - 9));
    }
    else
    {
        c_size = csd[7] & 0x3F;
        c_size = (c_size << 8) | csd[8];
        c_size = (c_size << 8) | csd[9];
        capacity = (c_size + 1) << 10;
    }

    return capacity;
}


/*!
 * \brief Extracts maximum supported data transfer rate over single line from CSD.
 *
 * \param[in] csd  Pointer to CSD
 *
 * \return Maximum supported data transfer rate
 */
uint32_t _io_sdcard_csd_baudrate(
    /* [IN] pointer to CSD */
    uint8_t *csd)
{
    static const uint32_t unit_tab[] = { 10000, 100000, 1000000, 10000000 };
    static const uint8_t value_tab[] = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };

    uint32_t value, unit;

    unit = (csd[3] & 0x07);
    value = (csd[3] & 0x78) >> 3;

    if (unit > 3)
        return 0; /* undefined */

    return value_tab[value] * unit_tab[unit];
}

/* EOF */
