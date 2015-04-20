
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
#include <stdarg.h>
#include "print_scan.h"

#define IO_MAXLINE 50 ///temp
/*!
 * \brief This function performs similarly to the scanf function of 'C'.
 *
 * \param[in] file_ptr The stream to scan from.
 * \param[in] fmt_ptr  The format string to use when scanning.
 *
 * \return Number of input items converted and assigned.
 * \return EOF
 */
int fscanf(FILE *stream, const char  *fmt_ptr,  ...)
{
   char    temp_buf[IO_MAXLINE];
   va_list ap;
   int result;

   if (NULL == stream)
   {
      return EOF;
   }

   va_start(ap, fmt_ptr);
   /* get a line of input from user */
   if (NULL == fgets(temp_buf, IO_MAXLINE, stream))
   {
      return EOF;
   }
   result = scan_prv(temp_buf, (char *)fmt_ptr, ap);
   va_end(ap);
   return result;

}

/*!
 * \brief This function performs similarly to the scanf function found in 'C'.
 *
 * \param[in] fmt_ptr The format string to scan with.
 *
 * \return The number of input items converted and assigned.
 * \return IO_EOF (Failure.)
 */
int scanf(const char  *fmt_ptr, ...)
{
   char    temp_buf[IO_MAXLINE];
   va_list ap;
   int result;

   va_start(ap, fmt_ptr);
   temp_buf[0] = '\0';

   if ( NULL == fgets(temp_buf, IO_MAXLINE, stdin))
   {
      return(EOF);
   }
   result = scan_prv(temp_buf, (char *)fmt_ptr, ap);
   va_end(ap);

   return result;
}

/*!
 * \brief  This function performs similarly to the 'C' sscanf function.
 *
 * \param[in] str_ptr    The string to scan from.
 * \param[in] format_ptr The format string to scan with.
 *
 * \return Number of input items converted and assigned.
 */
int sscanf(const char *str_ptr, const char *format_ptr, ...)
{
   va_list ap;
   int result;

   va_start(ap, format_ptr);
   result = scan_prv(str_ptr, (char *)format_ptr, ap);
   va_end(ap);
   return result;
}
