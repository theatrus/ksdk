/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
* Copyright 1989-2008 ARC International
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
Contains benchmark read/write functions.
*
*
*
*
*END************************************************************************/

#include <mqx.h>
#include <mfs.h>
#include <stdio.h>
#include <shell.h>
#include <fs_supp.h>
#include <unistd.h>
#include <fcntl.h>

#include "mfs_benchmark.h"

/*FUNCTION********************************************************************
*
* Function Name    : Sh_Benchmark
* Returned Value   : return SHELL_EXIT_SUCCESS or SHELL_EXIT_ERROR
* Comments         : Main benchmark application
*
*END**************************************************************************/
int32_t Sh_Benchmark(int32_t argc, char *argv[])
{ /* Body */
    bool print_usage, shorthelp = FALSE;
    int fs_ptr, file_ptr;
    int write_mode = O_RDWR | O_CREAT;
    int read_mode = O_RDONLY;
    char file_name[NAME_SIZE];
    uint32_t n, wr_time, rd_time;
    int32_t return_code = SHELL_EXIT_SUCCESS;
    int32_t block_size = 1;
    int32_t block_count = 0x100000;


    print_usage = Shell_check_help_request(argc, argv, &shorthelp);

    if (!print_usage){
        if (argc != 2 && argc != 4) {
            printf("Error, invalid number of parameters\n");
            return_code = SHELL_EXIT_ERROR;
            print_usage = TRUE;
        }
        else {
            fs_ptr = _io_get_fs_by_name(argv[1]);
            if (fs_ptr == NULL) {
                printf("Error, file system %s not found\n", argv[1]);
                return_code = SHELL_EXIT_ERROR;
            }
            else {
                printf("\nRunning MFS speed benchmark...\n\n");
                printf("--------------------------------------------\n");
                printf("# blocks  |  Size  |  WR time  |  RD time  |\n");
                printf("--------------------------------------------\n");

                snprintf(file_name, NAME_SIZE, "%s/test.txt", argv[1]);

                /* Automatic benchmark with 1MB file */
                if (argc == 2)
                {
                  for (n = 0; n < 15; n ++) {
                      ioctl(fs_ptr, IO_IOCTL_DELETE_FILE, file_name);
                      file_ptr = open(file_name, write_mode);
                      if (file_ptr == NULL) {
                          printf("Error, could not create test file for writing\n");
                          return_code = SHELL_EXIT_ERROR;
                          break;
                      }
                      else {
                          /* Returns write time in ms */
                          wr_time = mfs_speed_write(file_ptr, block_size, block_count);
                          close(file_ptr);
                      }
                     /* Reopen file in read only mode */
                     file_ptr = open(file_name, read_mode);

                     if (file_ptr == NULL) {
                         printf("Error, could not open test file for reading\n");
                         return_code = SHELL_EXIT_ERROR;
                         break;
                     }
                     else {
                         /* Returns read time in ms */
                         rd_time = mfs_speed_read(file_ptr, block_size, block_count);
                         close(file_ptr);
                     }
                     ioctl(fs_ptr, IO_IOCTL_DELETE_FILE, file_name);

                     /* Print results */
                     printf(" %8d %6d B   %6d ms   %6d ms\n", block_count, block_size, wr_time, rd_time);

                     block_size <<= 1;
                     block_count >>= 1;
                  }
                }

                /* Benchmark with user size */
                else {

                    /* Get parameters */
                    if (!Shell_parse_int_32(argv[2], &block_count) ||
                        !Shell_parse_int_32(argv[3], &block_size))
                    {
                        printf("Error, invalid format\n");
                        return_code = SHELL_EXIT_ERROR;
                        print_usage = TRUE;
                    }
                    else {
                        ioctl(fs_ptr, IO_IOCTL_DELETE_FILE, file_name);
                        file_ptr = open(file_name, write_mode);
                        if (file_ptr == NULL){
                            printf("Error, could not create test file for writing\n");
                            return_code = SHELL_EXIT_ERROR;
                        }
                        else {
                            /* Returns write time in ms */
                            wr_time = mfs_speed_write(file_ptr, block_size, block_count);
                            close(file_ptr);
                        }
                        /* Reopen file in read only mode */
                        file_ptr = open(file_name, read_mode);
                        if (file_ptr == NULL) {
                            printf("Error, could not create test file for writing\n");
                            return_code = SHELL_EXIT_ERROR;
                        }
                        else {
                            /* Returns read time in ms */
                            rd_time = mfs_speed_read(file_ptr, block_size, block_count);
                            close(file_ptr);
                        }
                        ioctl(fs_ptr, IO_IOCTL_DELETE_FILE, file_name);

                        /* Print results */
                        printf(" %8d %6d B   %6d ms   %6d ms\n", block_count, block_size, wr_time, rd_time);                                    
                    }
                }
            }
        }
    }

    if (print_usage) {
        if (shorthelp) {
            printf("%s <drive:> [<count>] [<size>]\n", argv[0]);
        }
        else {
            printf("Usage: %s <drive:> [<count>] [<size>]\n", argv[0]);
            printf("   <drive:>  = specifies the drive name (followed by a colon)\n");
            printf("   [<count>] = number of blocks to write\n");
            printf("   [<size>]  = size of one block\n");
        }
    }

    return return_code;
} /* Endbody */


/*FUNCTION********************************************************************
*
* Function Name    : mfs_speed_write
* Returned Value   : return time in ms or -1 if error occurred
* Comments         : Function for testing writing speed
*
*END**************************************************************************/
int32_t mfs_speed_write(int file_ptr,
                        uint32_t block_size,
                        uint32_t block_count)
{ /* Body */
    bool overflow;
    MQX_TICK_STRUCT start_tick, end_tick;
    void   *data_ptr = NULL;
    _mqx_int wr = 0;
    _mqx_int return_value = -1;

    data_ptr = _mem_alloc(block_size);

    if (data_ptr != NULL){
        _time_get_ticks(&start_tick);
        while (block_count--) {
            wr = write(file_ptr, data_ptr, block_size);
        }
        _time_get_ticks(&end_tick);
        _mem_free(data_ptr);

        if (wr == block_size){
            return_value = _time_diff_milliseconds(&end_tick, &start_tick, &overflow);
        }
    }

    return return_value;
} /* Endbody */


/*FUNCTION********************************************************************
*
* Function Name    : mfs_speed_read
* Returned Value   : return time in ms or -1 if error occurred
* Comments         : Function for testing reading speed
*
*END**************************************************************************/
int32_t mfs_speed_read(int file_ptr,
                       uint32_t block_size,
                       uint32_t block_count)
{ /* Body */
    bool overflow;
    MQX_TICK_STRUCT start_tick, end_tick;
    void *data_ptr = NULL;
    _mqx_int rd = 0;
    _mqx_int return_value = -1;

    data_ptr = _mem_alloc(block_size);

    if (data_ptr != NULL){
        _time_get_ticks(&start_tick);
        while (block_count--) {
            rd = read(file_ptr, data_ptr, block_size);
        }
        _time_get_ticks(&end_tick);
        _mem_free(data_ptr);

        if (rd == block_size){
            return_value = _time_diff_milliseconds(&end_tick, &start_tick, &overflow);
        }
    }

    return return_value;
} /* Endbody */

/* EOF */