/*HEADER**********************************************************************
*
* Copyright 2008, 2014 Freescale Semiconductor, Inc.
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
*
*   Example using RTCS Library.
*
*
*END************************************************************************/
#include <mfs.h>
#include <rtcs.h>
#include <shell.h>
#include <ipcfg.h>
#include "config.h"
#if PLATFORM_SDK_ENABLED
#include <fsl_os_abstraction.h>
#include <fsl_gpio_driver.h>
#include <board.h>
#endif

#if MQX_USE_IO_OLD
#include <fio.h>
#else
#include <stdio.h>
#include <nio.h>
#include <fcntl.h>
 #include <errno.h>
#include <fs_supp.h>
  #if PLATFORM_SDK_ENABLED   
    #include <nio_mem.h>
    #include "app_rtos.h"
    #include "main.h"
  #else
    #include <nmem.h>
  #endif
#endif

#if DEMOCFG_ENABLE_PPP
#include <ppp.h>

#if ! RTCSCFG_ENABLE_VIRTUAL_ROUTES
#error This application requires RTCSCFG_ENABLE_VIRTUAL_ROUTES defined non-zero in rtcs_user_config.h. Please recompile libraries (BSP, RTCS) with this option.
#endif

#if ! RTCSCFG_ENABLE_GATEWAYS
#error This application requires RTCSCFG_ENABLE_GATEWAYS defined non-zero in rtcs_user_config.h. Please recompile libraries (BSP, RTCS) with this option.
#endif

#warning This application requires PPP device to be defined manually and being different from the default IO channel (BSP_DEFAULT_IO_CHANNEL). See PPP_DEVICE in config.h.

#warning This application requires PPP device QUEUE_SIZE  to be more than size of input LCP packet (set BSPCFG_UARTX_QUEUE_SIZE to 128 in mqx_sdk_config.h) If no, you can lose LCP packets.

#if PPP_SECRETS_NOT_SHARED
#error This application requires PPP_SECRETS_NOT_SHARED defined to zero in /src/rtcs/source/include/ppp.h. Please recompile RTCS with this option.
#endif

#endif /* DEMOCFG_ENABLE_PPP */


#if !PLATFORM_SDK_ENABLED
  #define MAIN_TASK 1

  #if ! BSPCFG_ENABLE_IO_SUBSYSTEM
  #error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in mqx_sdk_config.h. Please recompile BSP with this option.
  #endif


  #ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
  #error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in mqx_sdk_config.h and recompile BSP with this option.
  #endif
  
  extern const SHELL_COMMAND_STRUCT Shell_commands[];
  extern void main_task (uint32_t);
  
  const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
  {
     /* Task Index,   Function,   Stack,  Priority,   Name,   Attributes,          Param, Time Slice */
      { MAIN_TASK,    main_task,  2500,   9, "Main", MQX_AUTO_START_TASK, 0,     0 },
      { 0 }
  };
  
  typedef uint32_t task_param_t;
#endif


#define RAMDISK_FILESYSTEM_NAME "a:"

void Ram_disk_start(void);
void Ramdisk_format(char *name);

static const char *BYTE_SIZES[] = { "bytes", "kB", "MB", "GB", "TB" };

#if DEMOCFG_ENABLE_RAMDISK

#if (DEMOCFG_ENABLE_MRAM_RAMDISK) /* MRAM RAM disk enabled */

/* The MRam disk base address definition taken from linker command file*/
#if defined (BSP_EXTERNAL_MRAM_RAM_BASE) && defined (BSP_EXTERNAL_MRAM_RAM_SIZE)
    #define RAM_DISK_BASE   BSP_EXTERNAL_MRAM_RAM_BASE 
    #define RAM_DISK_SIZE   BSP_EXTERNAL_MRAM_RAM_SIZE
#else
   #error MRAM RAM disk needs space to be defined
#endif

#endif /* MRAM RAM disk enabled */
/*FUNCTION*------------------------------------------------
* 
* Function Name: Ram_disk_start
* Comments     :
*    
*
*END*-----------------------------------------------------*/
void Ram_disk_start(void)
{
#if MQX_USE_IO_OLD
    MQX_FILE_PTR    dev_handle1;
    MQX_FILE_PTR    a_fd_ptr;
#else
    int    dev_handle1;
    int    a_fd_ptr;
    uint32_t        isFormatted = 0;
#if PLATFORM_SDK_ENABLED==1
    NIO_MEM_INIT_DATA_STRUCT nmem_init_struct = {0};
#else
    NMEM_INIT_DATA_STRUCT nmem_init_struct = {0};
#endif
#endif
    int32_t         error_code;
    _mqx_uint       mqx_status = MQX_OK;
    uint32_t        size;
    unsigned char   *base;

    a_fd_ptr = 0;
#if DEMOCFG_ENABLE_MRAM_RAMDISK /* MRAM RAM disk enabled */
    size = RAM_DISK_SIZE;
    base = RAM_DISK_BASE;
#else 
    /* Determine RAMDISK size  - start on 128 KiB and go down to 16 KiB */
    #if 1 /* Constant size 8KB.*/
        size = 8*1024;
    #else    /* Determine RAMDISK size  - start on 128 KiB and go down to 16 KiB */
        for (size = 128*1024; size > 16*1024; size /= 2)
        {
            base = (unsigned char *) _mem_alloc(size);
            if (base != NULL)
            {
                _mem_free(base);
                base = NULL;
                break;
            }
        }
        _task_set_error(MQX_OK);
        /* If memory block is big half its size, otherwise leave it on 8 KiB */
        if (size > 8*1024)
        {
            size /= 2;
        }
    #endif /* Constant size 8 KB */
    base = (unsigned char *) _mem_alloc(size);
    if (base == NULL)
    {
        printf("\nError - unable to allocate space for RAM disk - NULL pointer");
        _task_block();
    }
#endif /* MRAM RAM disk enabled */
    
    /* Install device */
#if MQX_USE_IO_OLD
    mqx_status = _io_mem_install("mfs_ramdisk:", (unsigned char *)base, (_file_size)size);
#else
    nmem_init_struct.BASE = (uint32_t)base;
    nmem_init_struct.SIZE = (uint32_t)size;
    if(NULL == _nio_dev_install("mfs_ramdisk:", &nio_mem_dev_fn, &nmem_init_struct, &error_code))
    {
      mqx_status = (_mqx_uint)error_code;
    }
#endif
    if ( mqx_status != MQX_OK ) 
    {
        printf("\nError installing memory device (0x%x)", mqx_status);
        _task_block();
    }

    /* Open the device which MFS will be installed on */
#if MQX_USE_IO_OLD    
    dev_handle1 = fopen("mfs_ramdisk:", 0);
    if ( dev_handle1 == NULL )
#else
    dev_handle1 = open("mfs_ramdisk:", O_RDWR);
    if ( dev_handle1 == -1 )
#endif     
    {
        printf("\nUnable to open Ramdisk device");
        _task_block();
    }

    /* Install MFS  */
#if MQX_USE_IO_OLD
    mqx_status = _io_mfs_install(dev_handle1, RAMDISK_FILESYSTEM_NAME, (_file_size)0);
#else
    mqx_status = _io_mfs_install(dev_handle1, RAMDISK_FILESYSTEM_NAME, (_file_size)0);
#endif
    if (mqx_status != MFS_NO_ERROR) 
    {
        printf("\nError initializing %s", RAMDISK_FILESYSTEM_NAME);
        _task_block();
    } 
    else 
    {
        printf("\nInitialized Ram Disk to %s\\\n", RAMDISK_FILESYSTEM_NAME);
    }

    /* Open the filesystem and detect, if format is required */
#if MQX_USE_IO_OLD
    a_fd_ptr = fopen(RAMDISK_FILESYSTEM_NAME, NULL);
    error_code = ferror(a_fd_ptr);
    if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK))
    {
        printf("\nError while opening %s\\ (%s)", MFS_Error_text((uint32_t)(uint32_t)error_code), RAMDISK_FILESYSTEM_NAME);
        _task_block();
    }
#else
    a_fd_ptr = open(RAMDISK_FILESYSTEM_NAME, O_RDWR);
    _io_register_file_system(a_fd_ptr, (char *)RAMDISK_FILESYSTEM_NAME);
    /* We check if the device is formatted with the ioctl command.*/
    error_code = ioctl(a_fd_ptr, IO_IOCTL_CHECK_FORMATTED, &isFormatted);
    if(0 > error_code) 
    {
      printf("\nError while accessing %s\\ (%s)", MFS_Error_text((uint32_t)error_code), RAMDISK_FILESYSTEM_NAME);
     _task_block();
    }
    else
    {
      if(!isFormatted)
      {
        error_code = MFS_NOT_A_DOS_DISK;
      }
    }
#endif
    if (error_code == MFS_NOT_A_DOS_DISK) 
    {
        Ramdisk_format(RAMDISK_FILESYSTEM_NAME);
    }
} 

void Ramdisk_format(char *name)
{
    uint32_t        error_code;
    
#if MQX_USE_IO_OLD
    MQX_FILE_PTR    fs_ptr = NULL;
#else
    int fs_ptr;
#endif

    fs_ptr = _io_get_fs_by_name(name);
#if MQX_USE_IO_OLD
    if (fs_ptr == NULL)
#else
    if (fs_ptr == 0)  
#endif
    {
        printf("Error, file system %s not found\n", name);
    }
    else
    {
        error_code = ioctl(fs_ptr, IO_IOCTL_DEFAULT_FORMAT, NULL);     
        if (error_code)
        {
           printf("Error while formatting: 0x%x\n", error_code);
        }
        else
        {
            uint64_t bytes;
            uint64_t space_big;
            uint32_t small_number;
            
            ioctl(fs_ptr, IO_IOCTL_FREE_SPACE, &bytes);
            space_big = bytes;
            for (small_number = 0; space_big > 1024; space_big = space_big/1024) small_number++;
            printf("\nFree disk space: %lu %s or %llu bytes.\n", (uint32_t)space_big, BYTE_SIZES[small_number], bytes);
        } 
    }
}
#endif /* DEMOCFG_ENABLE_RAMDISK */

/*TASK*-----------------------------------------------------------------
*
* Task Name      : Main_task
* Returned Value : void
* Comments       :
*
*END*************************************************************************/

void main_task ( task_param_t init_data )
{
    uint32_t     error;

   /* runtime RTCS configuration for devices with small RAM, for others the default BSP setting is used */
   _RTCSPCB_init = 4;
   _RTCSPCB_grow = 2;
   _RTCSPCB_max = 20;
   _RTCS_msgpool_init = 4;
   _RTCS_msgpool_grow = 2;
   _RTCS_msgpool_max  = 20;
   _RTCS_socket_part_init = 4;
   _RTCS_socket_part_grow = 2;
   _RTCS_socket_part_max  = 20;

    _RTCSTASK_stacksize = 3000;
    error = RTCS_create();
    if (error != RTCS_OK) 
    {
        printf("\nRTCS failed to initialize, error = %X", error);
        _task_block();
    }
    /* Enable IP forwarding */
    _IP_forward = TRUE;
        
#if DEMOCFG_ENABLE_RAMDISK  
    Ram_disk_start();
#endif /* DEMOCFG_ENABLE_RAMDISK */

    /* Run the shell */
    Shell(Shell_commands, NULL);
    for(;;) 
    {
        printf("Shell exited, restarting...\n");
        Shell(Shell_commands, NULL);
    }
}
/* EOF */
