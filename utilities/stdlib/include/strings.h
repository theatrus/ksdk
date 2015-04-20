
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
*   This is a standard strings header implemented by MQX
*
*
*END************************************************************************/
#ifndef __strings_h__
#define __strings_h__

#include <stdio.h>
/*--------------------------------------------------------------------------*/

/*
 *                      FUNCTION PROTOTYPES
 */
//int ffs(int);
int strcasecmp(const char *, const char *);
//int strcasecmp_l(const char *, const char *, locale_t);
int strncasecmp(const char *, const char *, size_t);
//int strncasecmp_l(const char *, const char *, size_t, locale_t);

#endif
