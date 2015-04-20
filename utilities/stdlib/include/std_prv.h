
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
#ifndef __std_prv_h__
#define __std_prv_h__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define STD_FP_IO 1
//Wraper for standard io

#define _MODE_ORDONLY   (0x00000000)    /* open for reading only */
#define _MODE_OWRONLY   (0x00000001)    /* open for writing only */
#define _MODE_ORDWR     (0x00000002)    /* open for reading and writing */
#define _MODE_OACCMODE  (0x00000003)    /* mask for above modes */
#define _MODE_OAPPEND   (0x00000008)    /* set append mode */
#define _MODE_OTRUNC    (0x00000400)    /* truncate to zero length */
#define _MODE_OCREAT    (0x00000200)    /* create if nonexistant */
                                          
/* Set if file should be freed on close */
#define _MODE_FOC       (0x00010000)
#define _MODE_EOF       (0x00020000)
#define _MODE_ERR       (0x00040000)

/* If value in one byte buffer is valid for write */
#define _MODE_LASTCR    (0x00080000)
/* If value in one byte buffer is valid for read */
#define _MODE_UC        (0x00100000)

/* Add function declaration here which is defined as weak */
int _WRITE(int fd, const void *buf, size_t nbytes);
int _READ(int fd, void *buf, size_t nbytes);
int _CLOSE(int fd);
long _LSEEK(int fd, long offset, int whence);
void _STD_LOCK();
void _STD_UNLOCK();
int _OPEN(const char *name, int flags, int mode);

#endif
