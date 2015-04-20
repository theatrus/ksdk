
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

static FILE _stdin =
{
    _MODE_ORDONLY,
    0,
    &_stdin._CBUF,
};
static FILE _stdout =
{
    _MODE_OWRONLY,
    1,
    &_stdout._CBUF,
};
static FILE _stderr =
{
    _MODE_OWRONLY,
    2,
    &_stderr._CBUF,
};

FILE *_files[FOPEN_MAX] = {&_stdin, &_stdout, &_stderr};
unsigned int _files_cnt   = 3;
