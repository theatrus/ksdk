/*HEADER**********************************************************************
*
* Copyright 2010 Freescale Semiconductor, Inc.
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
*   This file contains the SD card driver functions.
*
*
*END************************************************************************/


#include <mqx.h>

#include <stdio.h>
#include <stdint.h>
#include <nio.h>
#include <ioctl.h>
#include <fs_supp.h>
#include <unistd.h>

#include <sdcard.h>
#include <sdcard_prv.h>
#include <sdcard_esdhc.h>
#include <esdhc.h>

const SDCARD_CONST_INIT_STRUCT sdcard_esdhc_init_data = { _io_sdcard_esdhc_init, _io_sdcard_esdhc_read_block, _io_sdcard_esdhc_write_block, ESDHC_BUS_WIDTH_4BIT };


/*!
 * \brief Reformats R2 (CID,CSD) as read from ESDHC registers to a byte array.
 *
 * \param[in] r Registers
 * \param[out] r2 Byte array
 *
 * \return None
 */
static void _io_sdcard_esdhc_r2_format(uint32_t r[4], uint8_t r2[16])
{
    int i;
    uint32_t tmp = 0; /* initialization required to avoid compilation warning */

    i = 15;
    while (i)
    {
        if ((i % 4) == 3)
        {
            tmp = r[3 - i / 4];
        }
        r2[--i] = tmp & 0xff;
        tmp >>= 8;
    }
    r2[15] = 0;
}

/*!
 * \brief Configures SDCARD for high speed mode
 *
 * \param[in] sdcard_ptr SDCard descriptor structure
 * \param[in] baudrate Desired baudrate in Hz
 *
 * \return SDCARD_OK on success, SDCARD_ERR on error, -NIO_ENODEV if device does not support high speed mode
 */
static int32_t _set_sd_high_speed_mode(SDCARD_STRUCT_PTR sdcard_ptr, uint32_t baudrate)
{
    ESDHC_COMMAND_STRUCT command;
    uint32_t param;
    uint8_t cmd6_data[64];

#define SFS_FUNC_GROUP1_BIT 400
#define SFS_FUNC_GROUP2_BIT 416
#define SFS_FUNC_GROUP3_BIT 432
#define SFS_FUNC_GROUP4_BIT 448
#define SFS_FUNC_GROUP5_BIT 464
#define SFS_FUNC_GROUP6_BIT 480


#define SFS_FUNC_GROUP1_FUNC 376

#define SFS_GET_BYTE_CNT(bit) (63 - (bit) / 8)

    //  set BLKCNT field to 1 (block), set BLKSIZE field to 64 (bytes);
    //  send CMD6, with argument 0xFFFFF1 and read 64 bytes of data accompanying the R1 response;
    //  wait data transfer done bit is set;

    /* Check the card capability of bus speed*/
    command.COMMAND = ESDHC_CREATE_CMD(6, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, ESDHC_COMMAND_DATACMD_FLAG | ESDHC_COMMAND_DATA_READ_FLAG);
    command.ARGUMENT = 0xFFFFF1;
    command.BLOCKS = 1;
    command.BLOCKSIZE = 64;

    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
    {
        return SDCARD_ERR;
    }

    if (64 != read(sdcard_ptr->COM_DEVICE, cmd6_data, 64))
    {
        return SDCARD_ERR;
    }

    // Check the response
    // in the function group 1, check the first function availability
    if ((cmd6_data[SFS_GET_BYTE_CNT(SFS_FUNC_GROUP1_BIT)] & 0x01) == 0)
        return -NIO_ENODEV;  //  if (bit 401 is '0') report the SD card does not support high speed mode and return;

    /* Set the high speed of bus */
    command.COMMAND = ESDHC_CREATE_CMD(6, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, ESDHC_COMMAND_DATACMD_FLAG | ESDHC_COMMAND_DATA_READ_FLAG);
    command.ARGUMENT = 0x80FFFFF1;
    command.BLOCKS = 1;
    command.BLOCKSIZE = 64;

    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
    {
        return SDCARD_ERR;
    }

    if (64 != read(sdcard_ptr->COM_DEVICE, cmd6_data, 64))
    {
        return SDCARD_ERR;
    }

    // Check the response
    // in the function group 1, check the first function availability
    if ((cmd6_data[SFS_GET_BYTE_CNT(SFS_FUNC_GROUP1_FUNC)] & 0x0F) != 0x01)
        return -NIO_ENODEV;  //  if (bit 401 is '0') report the SD card does not support high speed mode and return;


    param = baudrate;
    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SET_BAUDRATE, &param))
    {
        return SDCARD_ERR;
    }

    return SDCARD_OK;
}


/*!
 * \brief Initializes ESDHC communication, SD card itself and reads its parameters.
 *
 * \param[in, out] desc SDCARD_STRUCT_PTR, SDCard descriptor structure
 *
 * \return SDCARD_OK on success, SDCARD_ERR on error
 */
int _io_sdcard_esdhc_init(
    /* [IN/OUT] SD card descriptor */
    void *desc)
{
    uint32_t baudrate, param;
    ESDHC_COMMAND_STRUCT command;
    uint8_t csd[16];
    SDCARD_STRUCT_PTR sdcard_ptr = (SDCARD_STRUCT_PTR)desc;

    /* Check parameters */
    if ((NULL == sdcard_ptr) || (-1 == sdcard_ptr->COM_DEVICE))
    {
        return SDCARD_ERR;
    }

    sdcard_ptr->TIMEOUT = 0;
    sdcard_ptr->NUM_BLOCKS = 0;
    sdcard_ptr->ADDRESS = 0;
    sdcard_ptr->isSDHC = FALSE;
    sdcard_ptr->VERSION2 = FALSE;
    sdcard_ptr->ALIGNMENT = 1;
    _mem_zero(sdcard_ptr->CID, sizeof(sdcard_ptr->CID));

    /* Get buffer alignment requirements from the communication driver and check it */
    ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_GET_REQ_ALIGNMENT, &sdcard_ptr->ALIGNMENT);
    if ((sdcard_ptr->ALIGNMENT & (sdcard_ptr->ALIGNMENT - 1)) || (sdcard_ptr->ALIGNMENT > 4))
    {
        /* Alignment is either not power of 2 or greater than 4 */
        return SDCARD_ERR;
    }

    /* Initialize and detect card */
    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_INIT, NULL))
    {
        return SDCARD_ERR;
    }

    param = ESDHC_INIT_BAUDRATE;
    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SET_BAUDRATE, &param))
    {
        return SDCARD_ERR;
    }

    /* GET CARD TYPE */
    param = 0;
    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_GET_CARD, &param))
    {
        return SDCARD_ERR;
    }

    /* SDHC check */
    if ((ESDHC_CARD_SD == param) || (ESDHC_CARD_SDHC == param) || (ESDHC_CARD_SDCOMBO == param) || (ESDHC_CARD_SDHCCOMBO == param))
    {
        if ((ESDHC_CARD_SDHC == param) || (ESDHC_CARD_SDHCCOMBO == param))
        {
            sdcard_ptr->isSDHC = TRUE;
        }
    }
    else
    {
        return SDCARD_ERR;
    }

    /* Card identify */
    command.COMMAND = ESDHC_CREATE_CMD(2, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R2, ESDHC_COMMAND_NONE_FLAG);
    command.ARGUMENT = 0;
    command.BLOCKS = 0;
    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
    {
        return SDCARD_ERR;
    }

    _io_sdcard_esdhc_r2_format(command.RESPONSE, sdcard_ptr->CID);

    /* Get card address */
    command.COMMAND = ESDHC_CREATE_CMD(3, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, ESDHC_COMMAND_NONE_FLAG);
    command.ARGUMENT = 0;
    command.BLOCKS = 0;
    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
    {
        return SDCARD_ERR;
    }
    sdcard_ptr->ADDRESS = command.RESPONSE[0] & 0xFFFF0000;

    /* Get card parameters */
    command.COMMAND = ESDHC_CREATE_CMD(9, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R2, ESDHC_COMMAND_NONE_FLAG);
    command.ARGUMENT = sdcard_ptr->ADDRESS;
    command.BLOCKS = 0;
    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
    {
        return SDCARD_ERR;
    }

    _io_sdcard_esdhc_r2_format(command.RESPONSE, csd);
    sdcard_ptr->NUM_BLOCKS = _io_sdcard_csd_capacity(csd);
    param = _io_sdcard_csd_baudrate(csd);

    /* Get maximal baudrate for card setup */
    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_GET_BAUDRATE_MAX, &baudrate))
    {
        return SDCARD_ERR;
    }

    if (param > baudrate)
    {
        param = baudrate;
    }
    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SET_BAUDRATE, &param))
    {
        return SDCARD_ERR;
    }

    /* Select card */
    command.COMMAND = ESDHC_CREATE_CMD(7, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1b, ESDHC_COMMAND_NONE_FLAG);
    command.ARGUMENT = sdcard_ptr->ADDRESS;
    command.BLOCKS = 0;
    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
    {
        return SDCARD_ERR;
    }

    /* Set block size to 512 */
    command.COMMAND = ESDHC_CREATE_CMD(16, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, ESDHC_COMMAND_NONE_FLAG);
    command.ARGUMENT = IO_SDCARD_BLOCK_SIZE;
    command.BLOCKS = 0;
    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
    {
        return SDCARD_ERR;
    }

    if (ESDHC_BUS_WIDTH_4BIT == sdcard_ptr->INIT.SIGNALS)
    {
        /* Application specific command */
        command.COMMAND = ESDHC_CREATE_CMD(55, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, ESDHC_COMMAND_NONE_FLAG);
        command.ARGUMENT = sdcard_ptr->ADDRESS;
        command.BLOCKS = 0;
        if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
        {
            return SDCARD_ERR;
        }

        /* Set bus width == 4 */
        command.COMMAND = ESDHC_CREATE_CMD(6, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, 0 /*ESDHC_COMMAND_ACMD_FLAG*/);
        command.ARGUMENT = 2;
        command.BLOCKS = 0;
        if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
        {
            return SDCARD_ERR;
        }

        param = ESDHC_BUS_WIDTH_4BIT;
        if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SET_BUS_WIDTH, &param))
        {
            return SDCARD_ERR;
        }
    }

    if (baudrate > ESDHC_DEFAULT_BAUDRATE)
    {
        // Try to set high speed mode
        _set_sd_high_speed_mode(sdcard_ptr, baudrate);
    }

    return SDCARD_OK;
}


/*!
 * \brief Reads blocks from SD card starting with given index into given buffer.
 *
 * \param[in] desc SDCARD_STRUCT_PTR, SDCard descriptor structure
 * \param[out] buffer Output buffer for the data
 * \param[in] index Index of first block to read
 * \param[in] num Number of blocks to read
 *
 * \return Number of read bytes on success, SDCARD_ERR or -NIO_ETIMEDOUT on error
 */
int _io_sdcard_esdhc_read_block(
    /* [IN] SD card info */
    void *desc,

    /* [OUT] Buffer to fill with read 512*num bytes */
    unsigned char *buffer,

    /* [IN] Index of first block to read */
    uint32_t index,

    /* [IN] Number of blocks to read */
    uint32_t num)
{
    SDCARD_STRUCT_PTR sdcard_ptr = (SDCARD_STRUCT_PTR)desc;
    ESDHC_COMMAND_STRUCT command;
    int result;

    /* Check parameters */
    if ((NULL == sdcard_ptr) || (-1 == sdcard_ptr->COM_DEVICE) || (NULL == buffer))
    {
        return SDCARD_ERR;
    }

    /* SD card data address adjustment */
    if (!sdcard_ptr->isSDHC)
    {
        index <<= IO_SDCARD_BLOCK_SIZE_POWER;
    }


    /* Read block(s) command */
    if (num > 1)
    {
        command.COMMAND = ESDHC_CREATE_CMD(18, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, (ESDHC_COMMAND_DATACMD_FLAG | ESDHC_COMMAND_DATA_READ_FLAG));
    }
    else
    {
        command.COMMAND = ESDHC_CREATE_CMD(17, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, (ESDHC_COMMAND_DATACMD_FLAG | ESDHC_COMMAND_DATA_READ_FLAG));
    }

    command.ARGUMENT = index;
    command.BLOCKS = num;
    command.BLOCKSIZE = IO_SDCARD_BLOCK_SIZE;

    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
    {
        return SDCARD_ERR;
    }


    /* Read data */
    result = read(sdcard_ptr->COM_DEVICE, buffer, num << IO_SDCARD_BLOCK_SIZE_POWER);

    if (result == -NIO_ETIMEDOUT)
    {
        // restore the card communication
        _io_sdcard_esdhc_init(sdcard_ptr);
    }

    return result;
}


/*!
 * \brief Writes blocks starting with given index to SD card from given buffer
 *
 * \param[in] desc SDCARD_STRUCT_PTR, SDCard descriptor structure
 * \param[in] buffer Input buffer for the data
 * \param[in] index Index of first block to write
 * \param[in] num Number of blocks to write
 *
 * \return Number of written bytes on success, SDCARD_ERR or -NIO_ETIMEDOUT on error
 */
int _io_sdcard_esdhc_write_block(
    /* [IN] SD card descriptor */
    void *desc,

    /* [IN] Buffer with data to write */
    unsigned char *buffer,

    /* [IN] Index of first block to write */
    uint32_t index,

    /* [IN] Number of blocks to be written */
    uint32_t num)
{
    SDCARD_STRUCT_PTR sdcard_ptr = (SDCARD_STRUCT_PTR)desc;
    ESDHC_COMMAND_STRUCT command;
    uint8_t tmp[4];
    int count;

    /* Check parameters */
    if ((NULL == sdcard_ptr) || (-1 == sdcard_ptr->COM_DEVICE) || (NULL == buffer))
    {
        return SDCARD_ERR;
    }

    /* SD card data address adjustment */
    if (!sdcard_ptr->isSDHC)
    {
        index <<= IO_SDCARD_BLOCK_SIZE_POWER;
    }

    /* Write block(s) command */
    if (num > 1)
    {
        command.COMMAND = ESDHC_CREATE_CMD(25, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, (ESDHC_COMMAND_DATACMD_FLAG));
    }
    else
    {
        command.COMMAND = ESDHC_CREATE_CMD(24, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, (ESDHC_COMMAND_DATACMD_FLAG));
    }

    command.ARGUMENT = index;
    command.BLOCKS = num;
    command.BLOCKSIZE = IO_SDCARD_BLOCK_SIZE;
    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
    {
        return SDCARD_ERR;
    }

    count = write(sdcard_ptr->COM_DEVICE, buffer, IO_SDCARD_BLOCK_SIZE * num);

    if (count < 0)
    {
        if (count == -NIO_ETIMEDOUT)
        {
            // restore the card communication
            _io_sdcard_esdhc_init(sdcard_ptr);
        }

        return count;
    }

    /* Wait for card to finish the operation - flush */
    if (write(sdcard_ptr->COM_DEVICE, NULL, 0) != 0)
    {
        return SDCARD_ERR;
    }

    command.COMMAND = ESDHC_CREATE_CMD(13, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, (ESDHC_COMMAND_NONE_FLAG));
    command.ARGUMENT = sdcard_ptr->ADDRESS;
    command.BLOCKS = 0;

    if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
    {
        return SDCARD_ERR;
    }


    /* Card status error check */
    if (command.RESPONSE[0] & 0xFFD98008)
    {
        count = 0; /* necessary to get real number of written blocks */
    }
    else if (0x000000900 != (command.RESPONSE[0] & 0x00001F00))
    {
        // the Card is not in the Transfer state and data ready
        return SDCARD_ERR;
    }

    if ((count >> IO_SDCARD_BLOCK_SIZE_POWER) != num)
    {
        /* Application specific command */
        command.COMMAND = ESDHC_CREATE_CMD(55, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, ESDHC_COMMAND_NONE_FLAG);
        command.ARGUMENT = sdcard_ptr->ADDRESS;
        command.BLOCKS = 0;
        if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
        {
            return SDCARD_ERR;
        }

        /* use ACMD22 to get number of written sectors */
        command.COMMAND = ESDHC_CREATE_CMD(22, ESDHC_COMMAND_TYPE_NORMAL, ESDHC_COMMAND_RESPONSE_R1, (ESDHC_COMMAND_DATACMD_FLAG | ESDHC_COMMAND_DATA_READ_FLAG));
        command.ARGUMENT = 0;
        command.BLOCKS = 1;
        command.BLOCKSIZE = 4;
        if (ESDHC_OK != ioctl(sdcard_ptr->COM_DEVICE, IO_IOCTL_ESDHC_SEND_COMMAND, &command))
        {
            return SDCARD_ERR;
        }

        if (4 != read(sdcard_ptr->COM_DEVICE, tmp, 4))
        {
            return SDCARD_ERR;
        }

        /* Wait for card to finish the operation - flush */
        if (write(sdcard_ptr->COM_DEVICE, NULL, 0) != 0)
        {
            return SDCARD_ERR;
        }

        count = (tmp[0] << 24) | (tmp[1] << 16) | (tmp[2] << 8) | tmp[3];
        if ((count < 0) || (count > num))
            return SDCARD_ERR;
    }

    return count;
}

/* EOF */
