
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
*   Contains the strings functionality
*
*
*END************************************************************************/

#include <strings.h>
#include <ctype.h>
/*!
 * \brief Compares two strings with different case size.
 *
 * \param[in] s1 Pointer to string 1.
 * \param[in] s2 Pointer to string 2.
 *
 * \return 0 End of string 1.
 * \return Difference between the two strings.
 */
int strcasecmp(const char *s1, const char *s2)
{
   while (tolower((int)*s1) == tolower((int)*s2))
   {
      if (*s1++ == '\0')
         return (0);
      s2++;
   }
   return (tolower((int)*s1) - tolower((int)*s2));
}

/*!
 * \brief Compares first n characters of two strings with different case size.
 *
 * \param[in] s1 Pointer to string 1.
 * \param[in] s2 Pointer to string 2.
 * \param[in] n  Number of characters to compare.
 *
 * \return 0 End of string 1.
 * \return Difference between the two strings.
 */
int strncasecmp(const char *s1, const char *s2, size_t n)
{
   if (n != 0)
   {
      do
      {
         if (tolower((int)*s1) != tolower((int)*s2))
            return (tolower((int)*s1) - tolower((int)*s2));
         if (*s1++ == '\0')
            break;
         s2++;
      } while (--n != 0);
   }
   return (0);
}
