
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
*   Contain declaration of _file array
*
*
*END************************************************************************/
#include <stdio.h>
#include "print_scan.h"
///todo:
///komentare!!!!!!!!!!!!!!!!!!!
///chars to unsigned chars

extern int dofputc(int ch, void *stream);
/*!
 * \brief Similar to the fprintf function of ANSI 'C'.
 *
 * \param[in] stream The stream to print upon.
 * \param[in] fmt_s  The format string to use for printing.
 *
 * \return Number of output characters.
 */
int fprintf(FILE *stream, const char *fmt_s, ...)
{
   va_list  ap;
   int  result;

   va_start(ap, fmt_s);
   result = 0;
   if ( stream )
   {
      result = _doprint((void *)stream, dofputc, -1, (char *)fmt_s, ap );
   }
   va_end(ap);

   return result;

}

/*!
 * \brief Performs similarly to the ANSI 'C' printf function.
 *
 * \param[in] fmt_s The format string to use when printing.
 *
 * \return Number of characters.
 * \return IO_EOF (End Of File found.)
 */
int printf(const char  *fmt_s, ...)
{
   va_list  ap;
   int  result;

   va_start(ap, fmt_s);
   result = _doprint((void *)stdout, dofputc, -1, (char *)fmt_s, ap);
   va_end(ap);

   return result;

}
/*!
 * \brief Performs similarly to the sprintf function found in 'C'.
 *
 * The returned number of characters does not include the terminating '\0'
 *
 * \param[in] str_ptr The string to print into.
 * \param[in] fmt_s The format specifier.
 *
 * \return Number of characters
 * \return IO_EOF (End Of File found.)
 */
int sprintf(char *str_ptr, const char *fmt_s, ...)
{
   int result;
   va_list ap;

   va_start(ap, fmt_s);
   result = _doprint((void *)&str_ptr, _sputc, -1, (char *)fmt_s, ap);
   *str_ptr = '\0';
   va_end(ap);
   return result;

}

/*!
 * \brief This function performs similarly to the sprintf function found in 'C'.
 *
 * The returned number of characters does not include the terminating '\0'.
 *
 * \param[in] str_ptr   The string to print into.
 * \param[in] max_count The maximal size of string.
 * \param[in] fmt_s   The format specifier.
 *
 * \return Number of characters
 * \return IO_EOF (End Of File found.)
 */
int snprintf(char *str_ptr, size_t max_count, const char *fmt_s, ...)
{
    int result;

    va_list ap;
    va_start(ap, fmt_s);
    result = _doprint((void *)&str_ptr, _sputc, max_count, (char *)fmt_s, ap);
    va_end(ap);
    if (0 != max_count)
    {
        *str_ptr = '\0';
    }

    return result;

}

/*!
 * \brief This function is equivalent to the corresponding printf function, except
 * that the variable argument list is replaced by one argument, which has been
 * initialized by the va_start macro.
 *
 * \param[in] fmt_s The format string.
 * \param[in] arg     The arguments.
 *
 * \return Number of characters
 * \return IO_EOF (End Of File found.)
 */
int vprintf(const char *fmt_s, va_list arg)
{
   int result;

   result = _doprint((void *)stdout, dofputc, -1, (char *)fmt_s, arg);

   return result;

}

/*!
 * \brief This function is equivalent to the corresponding printf function, except
 * that the variable argument list is replaced by one argument, which has been
 * initialized by the va_start macro.
 *
 * \param[in] stream The stream to print upon.
 * \param[in] fmt_s  The format string to use for printing.
 * \param[in] arg      The argument list to print.
 *
 * \return Number of characters
 * \return IO_EOF (End Of File found.)
 */
int vfprintf(FILE *stream, const char *fmt_s, va_list arg)
{
   int result;

   result = 0;
   if ( stream ) {
      result = _doprint((void *)stream, dofputc, -1, (char *)fmt_s, arg);
   }
   return result;

}

/*!
 * \brief This function is quivalent to the corresponding printf function, except
 * that the variable argument list is replaced by one argument, which has been
 * initialized by the va_start macro.
 *
 * \param[in] str_ptr The string to print into.
 * \param[in] fmt_s The format string.
 * \param[in] arg     The arguments.
 *
 * \return Number of characters
 * \return IO_EOF (End Of File found.)
 */
int vsprintf( char *str_ptr, const char *fmt_s, va_list arg)
{
   int result;

   result = _doprint((void *)&str_ptr, _sputc, -1, (char *)fmt_s, arg);
   *str_ptr = '\0';
   return result;

}

/*!
 * \brief This function is equivalent to the corresponding printf function, except
 * that the variable argument list is replaced by one argument, which has been
 * initialized by the va_start macro.
 *
 * \param[in] str_ptr The string to print into.
 * \param[in] max_count The maximal size of string.
 * \param[in] fmt_s   The format specifier.
 * \param[in] arg       The arguments.
 *
 * \return Number of characters
 * \return IO_EOF (End Of File found.)
 */
int vsnprintf(char *str_ptr, size_t max_count, const char *fmt_s, va_list arg)
{
    int result;

    result = _doprint((void *)&str_ptr, _sputc, max_count, (char *)fmt_s, arg);

    if (0 != max_count)
    {
        *str_ptr = '\0';
    }

    return result;

}
