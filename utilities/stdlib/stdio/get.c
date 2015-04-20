
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
*   Contains the get functionality
*
*
*END************************************************************************/

#include <stdio.h>
#include "buf_prv.h"

/*!
 * \brief This function obtains the next character(if present) as an unsigned char converted to an int, from the input stream pointed to by stream.
 *
 * \param[in] stream    input stream
 *
 * \return  next character from input stream
 *          EOF - if stream is at end-of-file or read error occurs.
 */
int fgetc(FILE* stream)
{
    unsigned char c;
    int result;

    if (NULL == stream)
    {
        return EOF;
    }

    if (stream->_MODE & _MODE_EOF) /* if end of file indicator is set */
    {
        return EOF;
    }
    else
    {
        result = _buf_read(&c, 1, stream);
        if (0 > result)
        {
            stream->_MODE |= _MODE_ERR;
            return EOF;
        }
        else if (1 > result)
        {
            stream->_MODE |= _MODE_EOF;
            return EOF;
        }
        else
        {
            return (int)c;
        }
    }
}

/*!
 * \brief function shall be equivalent to fgetc ().
 *
 * \param[in] stream    input stream
 *
 * \return  next character from input stream
 *          EOF - if stream is at end-of-file or read error occurs.
 */
int getc(FILE* stream)
{
    return fgetc(stream);
}

/*!
 * \brief The getchar () function shall be equivalent to getc(stdin)
 *
 * \return  next character from input stream
 *          EOF - if stream is at end-of-file or read error occurs.
 */
int getchar()
{
    return fgetc(stdin);
}

/*!
 * \brief This function shall read bytes from stream into the array pointed to by s,until n−1 bytes
 * are read, or a <newline> is read and transferred to s,oranend-of-file condition is encountered.
 * The string is then terminated with a null byte.
 *
 * \param[in]   s   Array where characters are read.
 * \param[in]   n   Number of characters to be read.
 * \param[in]   stream Input stream.
 *
 * \return  s       - If success.
 *          NULL    - If read error occurs
 *                  - If end-of-file is encountered and no characters have been read into the array
 */
char *fgets(char *s, int n, FILE* stream)
{
    int i = 0;
    int result;

    if ((NULL == stream) || (NULL == s) || (1 > n))
    {
        return NULL;
    }

    /* if end-of-file flag is set */
    if (stream->_MODE & _MODE_EOF)
    {
        return NULL;
    }

    for (i = 0; i < n - 1; i++)
    {
        result = _buf_read((unsigned char *)&s[i], 1, stream);
        /* if read error occurs */
        if (0 > result)
        {
            /* Mark error flag */
            stream->_MODE |= _MODE_ERR;
            return NULL;
        }
        /* If end-of-file is obtained */
        if (0 == result)
        {
            stream->_MODE |= _MODE_EOF;
            return NULL;
        }
        /* If newline character is obtained */
        if ('\r' == s[i])
        {
            /* indicate CR flag for case that next read is LF */
            stdin->_MODE |= _MODE_LASTCR;
            break;
        }
        /* If newline character is obtained */
        if ('\n' == s[i])
        {
            break;
        }
    }
    s[i + 1] = '\0';
    return s;
}

/*!
 * \brief  function shall read bytes from the standard input stream, stdin, into the array pointed
 * to by s, until a <newline> is read or an end-of-file condition is encountered.
 *
 * \param[in]   s   Array where characters are read.
 *
 * \return  s       - If success.
 *          NULL    - If read error occurs
 *                  - If end-of-file is encountered and no characters have been read into the array
 */
char *gets(char *s)
{
    int i = 0;
    int result;
    if (NULL == s)
    {
        return NULL;
    }

    /* if end-of-file flag is set */
    if (stdin->_MODE & _MODE_EOF)
    {
        return NULL;
    }

    /* Read bytes until newline character is obtained */
    do
    {
        result = _buf_read((unsigned char *)&s[i], 1, stdin);
        /* if read error occurs */
        if (0 > result)
        {
            /* Mark error flag */
            stdin->_MODE |= _MODE_ERR;
            return NULL;
        }
        /* If end-of-file is obtained */
        if (0 == result)
        {
            stdin->_MODE |= _MODE_EOF;
            return NULL;
        }
        if (s[i] == '\r')
        {
            /* indicate CR flag for case that next read is LF */
            stdin->_MODE |= _MODE_LASTCR;
            break;
        }
    }
    while (s[i++] != '\n');

    s[i - 1] = '\0'; /* Rewrite new-line with zero character */
    return s;
}

/*!
 * \brief This function shall push the byte specified by c (converted to an unsigned char)back
 * onto the input stream pointed to by stream
 *
 * \param[in] stream    input stream
 * \param[in] c         unget character
 *
 * \return byte pushed back after conversion
 *          EOF otherwise
 */
int ungetc(int c, FILE *stream)
{
    int result;

    if ((NULL == stream) || (EOF == c))
    {
        return EOF;
    }

    result = _buf_unget_byte((unsigned char) c, stream);
    if (0 > result)
    {
        return EOF;
    }
    else
    {
        /* Clear end of file indicator */
        stream->_MODE &= ~_MODE_EOF;
        return c;
    }
}
