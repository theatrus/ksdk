
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
*   Contains the function scan.c
*
*
*END************************************************************************/

#include <stdio.h>
#include "buf_prv.h"

/*!
 * \brief  Get current file position information.
 *
 * \param[in] stream Pointer to a FILE object that identifies the stream.
 * \param[out] pos Pointer to a fpos_t object
 *
 * \return 0 Upon successful completion
 * \return non-zero otherwise
 */
int fgetpos(FILE *stream, fpos_t *pos)
{
    long offset;

    if ((NULL == stream) || (NULL == pos))
    {
        return EOF;
    }

    offset = (long) _buf_get_pos(stream);
    if (-1 == offset)
    {
        return EOF;
    }

    pos->_OFF = (unsigned long) offset;
    return 0;
}

/*!
 * \brief  This function shall set the file-position indicator for the stream pointed to by stream. If a
 * read or write error occurs, the error indicator for the stream shall be set and fseek () fails.
 *
 * \param[in] stream Pointer to a FILE object that identifies the stream.
 * \param[in] offset Offset for  position, measured.
 * \param[in] whence Position specified by.
 *
 * \return 0  If they succeed.
 * \return -1 otherwise
 */
int fseek(FILE *stream, long int offset, int whence)
{

    if (NULL == stream)
    {
        return -1;
    }
    /* Flush data to stream */
    if (EOF == fflush(stream))
    {
        /* Error indicator should be set by fflush function */
        return -1;
    }
    if (0 > _buf_set_pos(stream, offset, whence))
    {
         /* set error indicator */
        stream->_MODE |= _MODE_ERR;
        return -1;
    }
    else
    {
        /* Clear end-of-file indicator, CR indicator, Unget char indikator*/
        stream->_MODE &= ~(_MODE_EOF | _MODE_LASTCR | _MODE_UC);
        return 0;
    }
}

/*!
 * \brief This function shall set current file position
 *
 * \param[in] stream Pointer to a FILE object that identifies the stream.
 * \param[in] pos pointer to a fpos_t object.
 *
 * \return 0 If succeeds
 * \return non-zero otherwise
 */
int fsetpos(FILE *stream, const fpos_t *pos)
{

    if ((NULL == stream) || (NULL == pos))
    {
        return EOF;
    }

    /* Flush data to stream */
    if (EOF == fflush(stream))
    {
        /* Error indicator should be set by fflush function */
        return EOF;
    }

    if (0 >  _buf_set_pos(stream, pos->_OFF, SEEK_SET))
    {
        /* set error indicator */
        stream->_MODE |= _MODE_ERR;
        return EOF;
    }
    else
    {
        /* Clear end-of-file indicator, CR indicator, Unget char indikator*/
        stream->_MODE &= ~(_MODE_EOF | _MODE_LASTCR | _MODE_UC);
        return 0;
    }
}

/*!
 * \brief  This function shall obtain the current value of the file-position indicator
 *
 * \param[in] stream Pointer to a FILE object that identifies the stream.
 *
 * \return current value of the file-position indicator for the stream measured in bytes from the beginning of the file.
 * \return -1 otherwise
 */
long int ftell(FILE *stream)
{
    if (NULL == stream)
    {
        return -1;
    }
    return (long int) _buf_get_pos(stream);
}

/*!
 * \brief  This function shall reset the file position indicator in a stream.
 *
 * \param[in] stream Pointer to a FILE object that identifies the stream.
 *
 * \return none
 */
void rewind(FILE *stream)
{
    fseek(stream, 0L, SEEK_SET);
    stream->_MODE &= ~_MODE_ERR;
}
