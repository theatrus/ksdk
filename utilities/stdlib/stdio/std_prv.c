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
*
*
*
*END************************************************************************/
#include <stdio.h>

#pragma weak _WRITE
int _WRITE(int fd, const void *buf, size_t nbytes)
{
    return 0;
}

#pragma weak _READ
int _READ(int fd, void *buf, size_t nbytes)
{
    return 0;
}

#pragma weak _CLOSE
int _CLOSE(int fd)
{
    return 0;
}

#pragma weak _LSEEK
long _LSEEK(int fd, long offset, int whence)
{
    return 0;
}

#pragma weak _STD_LOCK
void _STD_LOCK()
{
    return;
}

#pragma weak _STD_UNLOCK
void _STD_UNLOCK()
{
    return;
}

#pragma weak _OPEN
int _OPEN(const char *name, int flags, int mode)
{
    return 0;   
}
