
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
*   Contains the function fread.
*
*
*END************************************************************************/

#include <stdio.h>
#include "buf_prv.h"

/*!
 * \brief This function read from the file.
 *
 * \param[in] ptr       Point to array where this function reads
 * \param[in] size      Size of element to be read
 * \param[in] nmemb     Number of elemens to be read
 * \param[in] stream    Stream read from.
 *
 * \return The number of elements successfully read, which may be less than nmemb if a read error or EOF is encountered.
 */
size_t fread(void *ptr, size_t size, size_t nmemb, FILE  *stream)
{
    size_t count = 0;
    int result;

    if ((NULL == ptr) || (NULL == stream) || (0 == size) || (0 == nmemb))
    {
        return 0; //error: bad input parameter
    }

    for (; 0 < nmemb; nmemb--)
    {
        result = _buf_read((unsigned char*)ptr, size, stream);
        ptr = (unsigned char*)ptr + size;
        if (result == 0)
        {
            stream->_MODE |= _MODE_EOF;
            break;
        }
        else if (0 > result)
        {
            stream->_MODE |= _MODE_ERR;
            break;
        }
        else if (result != size)
        {
            break;
        }
        count++;
    }

    return count;

}
