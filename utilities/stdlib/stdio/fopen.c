
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
*   Contains the function fopen.
*
*
*END************************************************************************/

#include <stdio.h>
#include <stdlib.h>

/*!
 * \brief Function parse mods  for open a return it as a flag combination
 *
 * \param[in] mods Different modes for opening file.
 *
 * \return Parsed flag combination on success
 * \return -1 (Parse operation fails.)
 */
static int parse_mods(const char *mods)
{
    #define MODS_MAX_LENGTH 4
    int flags = 0;
    int i;
    /*
     * Check mods
     * r or rb O_RDONLY
     * w or wb O_WRONLY|O_CREAT|O_TRUNC
     * a or ab O_WRONLY|O_CREAT|O_APPEND
     * r+ or rb+ or r+b O_RDWR
     * w+ or wb+ or w+b O_RDWR|O_CREAT|O_TRUNC
     * a+ or ab+ or a+b O_RDWR|O_CREAT|O_APPEND
     */

    /* found string length */
    for (i = 0; i <= MODS_MAX_LENGTH; i++)
    {
        if (mods[i] == '\0')
        {
            break;
        }
    }
    /* Parse mods string */
    if (0 != i)
    {
        switch (mods[0])
        {
        case 'r':
            flags = _MODE_ORDONLY;
            break;
        case 'w':
            flags |= _MODE_OWRONLY | _MODE_OCREAT | _MODE_OTRUNC;
            break;
        case 'a':
            flags |= _MODE_OWRONLY | _MODE_OCREAT | _MODE_OAPPEND;
            break;
        default:
            return -1; /*  Bad mods */
        }
        if (1 < i)
        {
            i--;
            /* if 'r' 'w' or 'a' is followed by  '+' or 'b' */
            if ((1 == i) && (('b' == mods[i]) || ('+' == mods[i])))
            {
                if ('+' == mods[i])
                {
                    flags |= _MODE_ORDWR;
                }
            }
            /* if 'r' 'w' or 'a' is followed by  "b+" or "+b" */
            else if ((2 == i) && ((('+' == mods[1]) && ('b' == mods[2])) || (('b' == mods[1]) && ('+' == mods[2]))))
            {
                flags |= _MODE_ORDWR;
            }
            else
            {
                return -1;
            }
        }
    }
    else
    {
        return -1;
    }
    return flags;
}
/*!
 * \brief The returned value is a pointer to file.
 *
 * \param[in] name The name of the file to open.
 * \param[in] mods Different modes for opening file.
 *
 * \return A pointer to the object controlling the stream.
 * \return NULL (Open operation fails.)
 */
FILE *fopen(const char  *name, const char  *mods)
{

    int     flags = 0;
    int     i;
    FILE    *file;

    if (!mods)
    {
        return NULL;
    }

    flags = parse_mods(mods);
    if (-1 == flags)
    {
        return NULL;
    }

    _STD_LOCK();
    if (FOPEN_MAX > _files_cnt)
    {
        _files_cnt++;
    }
    else
    {
        _STD_UNLOCK();
        return NULL;
    }
    _STD_UNLOCK();

    file = (FILE*) malloc(sizeof(FILE));
    if (NULL == file)
    {
        return NULL;
    }

    file->_MODE = _MODE_FOC;
    file->_BUF  = &file->_CBUF;

    /* store flags to file structure */
    file->_MODE |= flags;

    /* Call low lvel open on file */
    file->_FD = _OPEN(name, flags, 0);
    if (0 > file->_FD)
    {
        free(file);
        _STD_LOCK();
        _files_cnt--;
        _STD_UNLOCK();
        return NULL;
    }

    _STD_LOCK();
    for(i = 0; i < FOPEN_MAX; i++)
    {
        if (NULL == _files[i])
        {
            _files[i] = file;
            break;
        }
    }
    _STD_UNLOCK();

    return file;
}
/*!
 * \brief The returned value is a pointer to file.
 *
 * \param[in] name The name of the file to open.
 * \param[in] mods Different modes for opening file.
 *
 * \return A pointer to the object controlling the stream.
 * \return NULL (Open operation fails.)
 */
FILE *fdopen(int  fd, const char  *mods)
{

    int     flags;
    int     i;
    FILE    *file;

    if ((!mods) || (0 > fd))
    {
        return NULL;
    }

    flags = parse_mods(mods);
    if (-1 == flags)
    {
        return NULL;
    }

    _STD_LOCK();
    if (FOPEN_MAX > _files_cnt)
    {
        _files_cnt++;
    }
    else
    {
        _STD_UNLOCK();
        return NULL;
    }
    _STD_UNLOCK();

    file = (FILE*) malloc(sizeof(FILE));
    if (NULL == file)
    {
        return NULL;
    }

    file->_MODE = _MODE_FOC;
    file->_BUF  = &file->_CBUF;

    /* store flags to file structure */
    file->_MODE |= flags;

    file->_FD = fd;

    _STD_LOCK();
    for(i = 0; i < FOPEN_MAX; i++)
    {
        if (NULL == _files[i])
        {
            _files[i] = file;
            break;
        }
    }
    _STD_UNLOCK();

    return file;
}
