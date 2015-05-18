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
Provide MFS file system on a SDCARD
*
*   
*
*
*END************************************************************************/

#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <nio.h>
#include <nio_mem.h>
#include <stdio.h>
#include <mfs.h>
#include <part_mgr.h>
#include <shell.h>
#include <fcntl.h>
#include <unistd.h>
#include <fs_supp.h>

#include <esdhc.h>
#include <sdcard.h>
#include <sdcard_esdhc.h>
#include <sdcard_settings.h>


#if ! SHELLCFG_USES_MFS
//#error This application requires SHELLCFG_USES_MFS defined non-zero in mqx_sdk_config.h. Please recompile libraries with this option.
#endif

void Shell_Task(uint32_t);
void sdcard_task(void);

const SHELL_COMMAND_STRUCT Shell_commands[] = {   
   { "cd",        Shell_cd },      
   { "copy",      Shell_copy },
   { "create",    Shell_create },
   { "del",       Shell_del },       
   { "disect",    Shell_disect},      
   { "dir",       Shell_dir },      
   { "df",        Shell_df },      
   { "exit",      Shell_exit }, 
   { "format",    Shell_format },
   { "help",      Shell_help }, 
   { "mkdir",     Shell_mkdir },     
   { "pwd",       Shell_pwd },       
   { "read",      Shell_read },      
   { "ren",       Shell_rename },    
   { "rmdir",     Shell_rmdir },
   { "sh",        Shell_sh },
   { "type",      Shell_type },
   { "write",     Shell_write },
   { "?",         Shell_command_list },
   { NULL,        NULL } 
};

#ifdef BOARD_SDHC_INSTANCE
  #define CDET_PIN sdhcCdPin
#else
  #define CDET_PIN sdcardCdPin
#endif

/*ISR*-----------------------------------------------------------------
*
* Function Name  : card_detect_isr
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void card_detect_isr(void *lwsem_ptr)
{
   GPIO_DRV_ClearPinIntFlag(CDET_PIN->pinName);

  /* Post card_detect semaphore */
   _lwsem_post(lwsem_ptr);
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_Task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void Shell_Task(uint32_t temp)
{
   (void)temp; /* suppress 'unused variable' warning */

   /* Run the shell on the serial port */
   printf("Demo start\n\r");
   for(;;)  {
      Shell(Shell_commands, NULL);
      printf("Shell exited, restarting...\n");
   }
}
 
/*TASK*-----------------------------------------------------
* 
* Task Name    : sdcard_task
* Comments     : Open device and install MFS on device
*    
*
*END*-----------------------------------------------------*/
void sdcard_task(void)
{ /* Body */
   int                          dev_fd = -1, a_fd = -1;
   bool                         inserted = FALSE, last = FALSE;
   int32_t                      error_code;
   uint32_t                     isFormatted = 0;
   int                          esdhc_handle = -1;
   char                         partman_name[] = "pm:";
   char                         partition_name[] = "pm:1";
   int                          partition_handle;
   SDCARD_INIT_STRUCT           sdcard_init_data;
   LWSEM_STRUCT                 lwsem;

   /* Create and post card detection semaphore */
   if( _lwsem_create(&lwsem, 0) != MQX_OK ){
       printf("\nlwsem create failed!");
       _task_block();
   }
   if( _lwsem_post(&lwsem) != MQX_OK ){
       printf("\nlwsem post failed!");
       _task_block();
   }

#ifdef BOARD_SDHC_GPIO_INSTANCE
   configure_gpio_pins(GPIO_EXTRACT_PORT(BOARD_SDHC_GPIO_INSTANCE));
#endif

   /* initialize SDHC pins */
   configure_sdhc_pins(BOARD_SDHC_INSTANCE);

   /* install SDHC low-level driver */
   _nio_dev_install("esdhc:", &nio_esdhc_dev_fn, (void*)&_bsp_esdhc_init, NULL);

   /* get an instance of the SDHC driver */
   esdhc_handle = open("esdhc:", 0);

   if(esdhc_handle < 0)
   {
      printf("\nCould not open esdhc!");
      _task_block();
   }

   /* prepare init data structure */
   sdcard_init_data.com_dev = esdhc_handle;
   sdcard_init_data.const_data = (SDCARD_CONST_INIT_STRUCT_PTR)&sdcard_esdhc_init_data;

   /* install device */
   if (_nio_dev_install("sdcard:", &nio_sdcard_dev_fn, (void*)&sdcard_init_data, NULL) == NULL)
   {
   /* Number of sectors is returned by ioctl IO_IOCTL_GET_NUM_SECTORS function */
   /* If another disc structure is desired, use MFS_FORMAT_DATA structure to   */
   /* define it and call standart format function instead default_format       */   
      printf("\nError installing memory device");
      _task_block();
   } /* Endif */

   /* install isr for card detection handling and initialize gpio pin */
   _int_install_isr(g_portIrqId[GPIO_EXTRACT_PORT(CDET_PIN->pinName)], card_detect_isr, &lwsem);   
   GPIO_DRV_InputPinInit(CDET_PIN);

   for(;;){

     /* Wait for card insertion or removal */
     if( _lwsem_wait(&lwsem) != MQX_OK ){
       printf("\nlwsem_wait failed!");
       _task_block();
     }

     /* Check if card is present */
     if(CDET_PIN->config.pullSelect == kPortPullDown)
         inserted = GPIO_DRV_ReadPinInput(CDET_PIN->pinName);
     else
         inserted = !GPIO_DRV_ReadPinInput(CDET_PIN->pinName);

     if(last != inserted){
       last = inserted;

       /* Card detection switch debounce delay */
       _time_delay(100);

       if(inserted)
       {
         /* Open the device which MFS will be installed on */
         dev_fd = open("sdcard:", O_RDWR);
         if ( dev_fd <= 0 ) {
              printf("\nUnable to open SDCARD device");
              _task_block();
         } /* Endif */

         /* Install partition manager over SD card driver */
         error_code = _io_part_mgr_install(dev_fd, partman_name, 0);
         if (error_code != MFS_NO_ERROR)
         {
             printf("Error installing partition manager: %s\n", MFS_Error_text((uint32_t)error_code));
             _task_block();
         }

         /* Open partition */
         partition_handle = open(partition_name, O_RDWR);
         if (partition_handle >= 0)
         {
             printf("Installing MFS over partition...\n");
        
             /* Validate partition */
             error_code = ioctl(partition_handle, IO_IOCTL_VAL_PART, NULL);
             if (error_code != MFS_NO_ERROR)
             {
                 printf("Error validating partition: %s\n", MFS_Error_text((uint32_t)error_code));
                 printf("Not installing MFS.\n");
                 _task_block();
             }

             /* Install MFS over partition */
             error_code = _io_mfs_install(partition_handle, "a:", 0);
             if (error_code != MFS_NO_ERROR)
             {
                 printf("Error initializing MFS over partition: %s\n", MFS_Error_text((uint32_t)error_code));
             }

         } else
         {
           printf("Installing MFS over SD card driver...\n");
            
           /* Install MFS over SD card driver */
           error_code = _io_mfs_install(dev_fd, "a:", (_file_size)0);
           if (error_code != MFS_NO_ERROR)
           {
               printf("Error initializing MFS: %s\n", MFS_Error_text((uint32_t)error_code));
           }
         }

         /* Open the filesystem and format detect, if format is required */
         a_fd = open("a:", O_RDWR);
         if (0 >= a_fd) {
             printf("\nError while opening a:\\ (%s)", MFS_Error_text((uint32_t)errno));
             _task_block();
         }

         _io_register_file_system(a_fd, "a:");

         /* We check if the device is formatted with the ioctl command. */
         error_code = ioctl(a_fd, IO_IOCTL_CHECK_FORMATTED, &isFormatted);
         if (0 > error_code) {
             printf("\nError while accessing a:\\ (%s)", MFS_Error_text((uint32_t)error_code));
             _task_block();
         }
         else{
           if(!isFormatted){
               printf("\nNOT A DOS DISK! You must format to continue.");
           }
         }
       }
       else
       {
         /* Close the filesystem */
         if ((a_fd >= 0) && (MQX_OK != close(a_fd)))
         {
             printf("Error closing filesystem.\n");
         }
         a_fd = -1;

         /* Force uninstall filesystem */
         error_code = _nio_dev_uninstall_force("a:", NULL);
         if (error_code < 0)
         {
             printf("Error uninstalling filesystem.\n");
         }

         /* Close partition */
         if ((partition_handle >= 0) && (MQX_OK != close(partition_handle)))
         {
             printf("Error closing partition.\n");
         }
         partition_handle = -1;

         /* Uninstall partition manager */
         error_code = _nio_dev_uninstall_force(partman_name, NULL);
         if (error_code < 0)
         {
             printf("Error uninstalling partition manager.\n");
         }

         /* Close the SD card device */
         if ((dev_fd >= 0) && (MQX_OK != close(dev_fd)))
         {
             printf("Error closing SD card device.\n");
         }

         dev_fd = -1;

         printf ("SD card uninstalled.\n");
       }
     }
   }
} /* Endbody */ 

/* EOF */
