
/*HEADER**********************************************************************
*
* Copyright 2013 Freescale Semiconductor, Inc.
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the LA_OPT_FSL_OPEN_3RD_PARTY_IP License
* distributed with this Material.
* See the LA_OPT_FSL_OPEN_3RD_PARTY_IP License file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*
*
*
*END************************************************************************/
#include <stdio.h>
#include "buf_prv.h"

/*!
 * \brief This function write to the stream.
 *
 * \param[in]
 * \param[in]
 *
 * \return
 */

int _buf_write(const unsigned char *s, int count, FILE* stream)
{
    int result;
    result = _WRITE(stream->_FD, s, count);

    return result;
}
/*!
 * \brief This function read from a stream.
 *
 * \param[in]
 * \param[in]
 *
 * \return
 */
int _buf_read(unsigned char *s, int count, FILE* stream)
{
    int result = 0, rv;

    _STD_LOCK();
    /* If byte in buffer is valid for read */
    if (stream->_MODE & _MODE_UC)
    {
        stream->_MODE &= ~_MODE_UC;
        *s = *stream->_BUF;
        s++;
        result++;
        count--;
    }
    /* Check if cr flag is set. */
    if (count && ( stream->_MODE & _MODE_LASTCR))
    {
        /* Clear flag */
        stream->_MODE &= ~_MODE_LASTCR;
        /* read next byte if it is not \n (lf) */
        rv = _READ(stream->_FD, s, 1);
        if (0 > rv)
        {
            _STD_UNLOCK();
            return rv;
        }
        /* If read char is not LF than accept this character */
        if ('\n' != s[0])
        {
            s++;
            result++;
            count--;
        }
    }
    /* read count characters */
    if (count)
    {
        rv = _READ(stream->_FD, s, count);
        if (0 > rv)
        {
            _STD_UNLOCK();
            return rv;
        }
        result += rv;
    }

    _STD_UNLOCK();
    return result;
}

/*!
 * \brief This function write to the stream.
 *
 * \param[in]
 * \param[in]
 *
 * \return
 */
int _buf_flush(FILE* stream)
{
    /* flush can be implemented in ioctl or writte zero bytes from NULL buffer. we deal to implement write method because ioctl functionality is not available on all drivers */
    _WRITE(stream->_FD, NULL, 0);
    return 0;
}

/*!
 * \brief
 *
 * \param[in]
 * \param[in]
 *
 * \return
 */
int _buf_unget_byte(unsigned char c, FILE* stream)
{
    _STD_LOCK();
    /* If byte in buffer is valid */
    if (stream->_MODE & _MODE_UC)
    {
        _STD_UNLOCK();
        return EOF;
    }
    /* If buffer is empty */
    *stream->_BUF = c;
    // stream->_MODE |= _MODE_CWVAL;
    stream->_MODE |= _MODE_UC;
    _STD_UNLOCK();
    return 0;
}

/*!
 * \brief
 *
 * \param[in]
 * \param[in]
 *
 * \return
 */
int _buf_set_pos(FILE *stream, long int offset, int whence)
{

    if ((SEEK_CUR == whence) && (stream->_MODE & _MODE_UC))
    {
        offset--;
    }
    if (0 > _LSEEK(stream->_FD, offset, whence))
    {
        return -1;
    }

    return 0;
}
/*!
 * \brief
 *
 * \param[in]
 * \param[in]
 *
 * \return
 */
long _buf_get_pos(FILE *stream)
{
    long offset;

    offset = (long) _LSEEK(stream->_FD, 0, SEEK_CUR);
    if (-1 == offset)
    {
        return offset;
    }

    return (stream->_MODE & _MODE_UC)? (offset - 1): offset;
}
