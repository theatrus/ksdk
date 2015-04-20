
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
*   Contains the function put.c
*
*
*END************************************************************************/

#include <stdio.h>
#include "buf_prv.h"
/*!
 * \brief This function shall write the byte specified by ch (converted to an unsigned char)to the
 * output stream pointed to by stream
 *
 * \param[in] ch        byte to be written
 * \param[in] stream    output stream
 *
 * \return  character written on success
 *          EOF otherwise
 */
int fputc(int ch, FILE* stream)
{
    const unsigned char c = (unsigned char) ch;
    int result;

    if (NULL == stream)
    {
        return EOF;
    }

    result = _buf_write(&c, 1, stream);

    if (0 > result)
    {
        /* Mark error */
        stream->_MODE |= _MODE_ERR;
        return EOF;
    }
    else
    {
        return ch;
    }
}

int dofputc(int ch, void *stream)
{
    FILE *str = (FILE *)stream;

    return fputc(ch, str);
}
/*!
 * \brief This function shall write the byte specified by ch (converted to an unsigned char)to the
 * output stream pointed to by stream
 *
 * \param[in] ch        byte to be written
 * \param[in] stream    output stream
 *
 * \return  character written on success
 *          EOF otherwise
 */
int putc(int ch, FILE* stream)
{
    return fputc(ch, stream);
}
/*!
 * \brief This function shall be equivalent to putc(c,stdout).
 *
 * \param[in] ch        byte to be written
 *
 * \return  character written on success
 *          EOF otherwise
 */
int putchar(int ch)
{
    return fputc(ch, stdout);
}
/*!
 * \brief This function shall write the null-terminated string pointed to by s to the stream pointed
 * to by stream. The terminating null byte shall not be written.
 *
 * \param[in] s         null-terminated string
 * \param[in] stream    output stream
 *
 * \return  non-negative-number on successful completion
 *          EOF otherwise
 *
 */
int fputs(const char *s, FILE* stream)
{
    int cnt = 0;
    int result;

    if ((NULL == stream) || (NULL == s))
    {
        return EOF;
    }

    /* get string length */
    while (0 != s[cnt])
    {
        cnt++;
    }
    /* write string to buffer. Terminating null character is not writen*/
    result  = _buf_write((const unsigned char *)s, cnt, stream);

    if (0 > result)
    {
        /* Mark error */
        stream->_MODE |= _MODE_ERR;
        return EOF;
    }
    return result;
}
/*!
 * \brief function shall write the string pointed to by s,followed by a <newline>, to the
*   standard output stream stdout. The terminating null byte shall not be written.
 *
 * \param[in] s string
 *
 * \return  non-negative-number on successful completion
 *          EOF otherwise
 */
int puts(const char *s)
{
    int result;

    if (NULL == s)
    {
        return EOF;
    }
    /* write string to stream */
    result = fputs(s, stdout);
    if (0 > result)
    {
        /* Mark error */
        stdout->_MODE |= _MODE_ERR;
        return EOF;
    }
    /* write <newline> to stream */
    result = putchar('\n');
    if (0 > result)
    {
        /* Mark error */
        stdout->_MODE |= _MODE_ERR;
        return EOF;
    }

    return result;
}

