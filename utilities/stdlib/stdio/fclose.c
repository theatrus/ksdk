
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
*   Contains the function fclose.
*
*
*END************************************************************************/

#include <stdio.h>
#include <stdlib.h>

/*!
 * \brief This function close the file.
 *
 * \param[in] Pointer to FILE structure
 * \param[in] mods Different modes for opening file.
 *
 * \return
 */
int fclose(FILE  *stream)
{
    int i, error_flag;

    if (NULL == stream)
    {
        return 0; //error: bad input parameter
    }

    /* Flush buffer */
    error_flag = fflush(stream);

    /* Close io primitives ???*/
    error_flag |= _CLOSE(stream->_FD);///carry return value?
    /* Remove stream from global _file array */
    for (i = 0; i < FOPEN_MAX; i++)
    {
        if (stream == _files[i])
        {
            _STD_LOCK();
            _files[i] = NULL;
            _files_cnt --;
            _STD_UNLOCK();
            break;
        }
    }

    /* If stream should be freed on close than free the stream  */
    if (stream->_MODE & _MODE_FOC)
    {
        free(stream);
    }

    return error_flag? EOF : 0;
}
