
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
*   Contains the function error handling.
*
*
*END************************************************************************/

#include <stdio.h>

/*!
 * \brief This  function shall clear the end-of-file and error indicators for the stream to which
 * stream points.
 *
 * \param[in] stream Pointed stream.
 *
 * \return  void
 */
void clearerr(FILE* stream)
{
    stream->_MODE &= ~(_MODE_ERR | _MODE_EOF);
}
/*!
 * \brief This function function shall test the end-of-file indicator for the stream pointed to by stream.
 *
 * \param[in] stream Pointed stream.
 *
 * \return   non-zero   If and only if the end-of-file indicator is set for stream.
 *           0          Otherwise.
 */
int feof(FILE* stream)
{
    // if (NULL == stream)
    // {
        // return 0
    // }
    // else
    // {
        // return (stream->_MODE & _MODE_EOF);
    // }
    return (NULL == stream)? 0: (stream->_MODE & _MODE_EOF);
}
/*!
 * \brief This  function shall test the error indicator for the stream pointed to by stream.
 *
 * \param[in] stream Pointed stream.
 *
 * \return   non-zero   If and only if the error indicator is set for stream.
 *           0          Otherwise.
 */
int ferror(FILE* stream)
{
    // if (NULL == stream)
    // {
        // return 0
    // }
    // else
    // {
        // return (stream->_MODE & _MODE_ERR);
    // }
    return (NULL == stream)? 0: (stream->_MODE & _MODE_ERR);
}
