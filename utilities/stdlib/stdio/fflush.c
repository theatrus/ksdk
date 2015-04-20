
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
*   Contains the function fflush.
*
*
*END************************************************************************/

#include <stdio.h>
#include "buf_prv.h"
/*!
 * \brief This function causes any unwritten data for that stream to be delivered to the host environment to be written to the file.
 *
 * \param[in] stream points to output stream. If  stream is NULL pointer, this function perform fflush for all opened streams.
 *
 * \return  EOF if write error occurs
 *          0   Otherwise
 */
int fflush(FILE  *stream)
{
    int i;
    if (NULL != stream)
    {
        if (0 > _buf_flush(stream))
        {
            return EOF;
        }
    }
    else
    {
        /* If inpunt stream point to NULL perform fflush for all opened streams. */
        for (i = 0; i < FOPEN_MAX; i++)
        {
            if (NULL != _files[i])
            {
                /* recurse call on all opened streams */
                if (EOF == fflush(_files[i]))
                {
                    return EOF;
                }
            }
        }
    }
    return 0;
}
