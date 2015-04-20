
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
*   This is a private header for stdio.h header.
*
*
*END************************************************************************/
#ifndef __buf_prv_h__
#define __buf_prv_h__

#include <stdio.h>

int _buf_write(const unsigned char *c, const int count, FILE* stream);
int _buf_read(unsigned char *c, const int count, FILE* stream);
int _buf_flush(FILE* stream);
int _buf_unget_byte(unsigned char c, FILE* stream);
int _buf_set_pos(FILE *stream, long int offset, int whence);
long _buf_get_pos(FILE *stream);

#endif
