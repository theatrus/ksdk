/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: msd_fat_demo.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file implements MAS FAT demo application.
*
*END************************************************************************/
#include "usb.h"
#include "ff.h"
#include "msd_diskio.h"
#include "usb_host_msd_ufi.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
#include "fsl_clock_manager.h"
#endif

/* User-defined macros */
#define DIR_OPERATION 1
#define FILE_OPERATION 1

/* Local variables */
static FATFS fatfs;            /* File system object */
static char buffer[257];

/* Function prototypes */
static FRESULT put_rc (FRESULT rc);
uint32_t out_stream ( const uint8_t *p,uint32_t btf);
static void Display_File_Info(FILINFO*Finfo);
static FRESULT List_Directory(const TCHAR * path);
int fat_demo(void);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : put_rc
* Returned Value : None
* Comments       : This function is to display returnCode of FAT's APIs 
*
*END*--------------------------------------------------------------------*/
static FRESULT put_rc 
  (
    /* [IN] return code value*/
    FRESULT rc
  )
{
    const TCHAR *str =
        (TCHAR *)"OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
        "INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
        "INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
        "LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0";
    FRESULT i;

    for (i = FR_OK; i != rc && *str; i++) 
    {
        while (*str++) ;
    }
    USB_PRINTF("  returnCode=%u (FR_%s)\n\r", (uint32_t)rc, str);
    
    if (FR_OK != rc)
    {
        USB_PRINTF("\n\r*------------------------------     DEMO FAILED       ------------------------ *"); 
    }
    
    return rc;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : out_stream
* Returned Value : Returns number of bytes sent or stream status
* Comments       : Sample code of data transfer function to be called back from f_forward
*
*END*--------------------------------------------------------------------*/
 uint32_t out_stream 
   (
      /* [IN] Pointer to the data block to be sent */
      const uint8_t *p, 
      /* [IN] >0: Transfer call (Number of bytes to be sent). 0: Sense call */ 
      uint32_t btf        
   )
{                            
    
    uint32_t cnt = 0;
    if (btf == 0) 
    {  /*Sense call */   
       cnt = 1;
    }
    else 
    {              /* Transfer call */
       do 
       {    /* Repeat while there is any data to be sent */
           USB_PRINTF("%c",*p++);
           cnt++;
       } while (cnt < btf);
    }

    return cnt;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : Display_File_Info
* Returned Value : None
* Comments       : This function is to display file information
*
*END*--------------------------------------------------------------------*/
static void Display_File_Info(FILINFO*Finfo) 
{
    USB_PRINTF("    %c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %s\n\r", 
            (Finfo->fattrib & AM_DIR) ? 'D' : '-',
            (Finfo->fattrib & AM_RDO) ? 'R' : '-',
            (Finfo->fattrib & AM_HID) ? 'H' : '-',
            (Finfo->fattrib & AM_SYS) ? 'S' : '-',
            (Finfo->fattrib & AM_ARC) ? 'A' : '-',
            (Finfo->fdate.Bits.year) + YEAR_ORIGIN, (Finfo->fdate.Bits.month), Finfo->fdate.Bits.day,
            (Finfo->ftime.Bits.hour), (Finfo->ftime.Bits.minute),
            Finfo->fsize,
#if _USE_LFN
            (Finfo->lfname[0])?&Finfo->lfname[0]:&Finfo->fname[0]
#else
            &Finfo->fname[0]
#endif
            );
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : List_Directory
* Returned Value : FR_OK: successful, != FR_OK: failed
* Comments       : List and display all the file and directory in the specified directory
*
*END*--------------------------------------------------------------------*/
static FRESULT List_Directory(const TCHAR *path)
{
  
    FRESULT returnCode = FR_OK; /* return code */
    FILINFO Finfo;      /* File object */
    DIR dir;                  /* Directory object */
    uint16_t dir_no = 0, file_no = 0;
    uint32_t size = 0;

#if _USE_LFN
    /* Allocate memory for Finfo->lfname */
    Finfo.lfname = ff_memalloc(_MAX_LFN);
    Finfo.lfsize = _MAX_LFN;
#endif
    
    USB_PRINTF(" Directory listing...\n\r");
    /* Open root directory */
    returnCode = f_opendir(&dir,path);
    if (returnCode)
    {
#if _USE_LFN
        if(NULL != Finfo.lfname)
        {
            /* Free memory for Finfo->lfname */
            ff_memfree(Finfo.lfname);
        }
#endif
        return(returnCode);
    }
  
    for(;;) 
    {
        returnCode = f_readdir(&dir, &Finfo);
        if ((returnCode != FR_OK) || !Finfo.fname[0])
        {
            break;
        }
        if (Finfo.fattrib & AM_DIR) 
        {
            dir_no++;
        } 
        else 
        {
            file_no++; size += Finfo.fsize;
        }

        Display_File_Info(&Finfo);   
    }
    
    USB_PRINTF("\n\n    %-4u File(s),%10lu bytes total\n    %-4u Dir(s)\n\r", file_no, size, dir_no);
#if _USE_LFN
    if(NULL != Finfo.lfname)
    {
        /* Free memory for Finfo->lfname */
        ff_memfree(Finfo.lfname);
    }
#endif
    return returnCode;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : fat_demo
* Returned Value : 
* Comments       : Test Fat file system
*
*END*--------------------------------------------------------------------*/
int fat_demo(void)
{
    FRESULT returnCode;        /* Result code */
    FATFS *fs;
    FIL fil;                /* File object */
    FILINFO Finfo;            /* File information object */
    char *p_str;
    char *dev_info;
    uint32_t fre_clust, size;
    static const char *str ="Line 4: Write data to file uses f_putc function\0";
    char *str_temp = (char *)str;
    const uint8_t ft[] = {0,12,16,32};


    OS_Time_delay(1000);

#if _USE_LFN
    /* Allocate memory for Finfo->lfname */
    Finfo.lfname = ff_memalloc(_MAX_LFN);
    Finfo.lfsize = _MAX_LFN;
#endif

    USB_PRINTF("\n\r******************************************************************************");
    USB_PRINTF("\n\r*                              FatFS DEMO                                    *");
    if(_USE_LFN)
    {
        USB_PRINTF("\n\r*          Configuration:  LNF Enabled, Code page  =%u                       *", _CODE_PAGE);
    }
    else 
    {
        USB_PRINTF("\n\r*          Configuration:  LNF Disabled, Code page =%u                       *", _CODE_PAGE);
    }
    USB_PRINTF("\n\r******************************************************************************");
    
    USB_PRINTF("\n\r******************************************************************************");
    USB_PRINTF("\n\r*                             DRIVER OPERATION                               *");
    USB_PRINTF("\n\r******************************************************************************");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/ 
    /* DM1: Initialize logical driver <f_mount>                                             */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    USB_PRINTF("\n\r1. Demo function: f_mount\n\n\r");
    USB_PRINTF("  Initializing logical drive 0...\n\r");
    returnCode = f_mount(0, &fatfs);
    if(returnCode)
    {
        goto Error1;
    }
    USB_PRINTF("  Initialization complete \n\r");
    USB_PRINTF("-----------------------------------------------------------------------------\n\r");
    
    /* Send some SCSI requests first */
    disk_initialize(0);
#if !HIGH_SPEED_DEVICE
    OS_Time_delay(1000);
#endif

    /* Get the vendor information and display it */
    USB_PRINTF("\n************************************************************************\n");
    disk_ioctl(0, GET_VENDOR_INFO, &dev_info);
    USB_PRINTF("Vendor Information:     %-1.8s Mass Storage Device\n",dev_info);
    disk_ioctl(0, GET_PRODUCT_ID,  &dev_info);
    USB_PRINTF("Product Identification: %-1.16s\n",dev_info);
    disk_ioctl(0, GET_PRODUCT_REV, &dev_info);
    USB_PRINTF("Product Revision Level: %-1.4s\n",dev_info);
    USB_PRINTF("************************************************************************\n");
    
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM2.  Show logical drive status <f_getfree, f_opendir, f_readdir>                    */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    USB_PRINTF("\n\r2. Demo functions:f_getfree, f_opendir, f_readdir\n\n\r");
    USB_PRINTF("getting drive 0 attributes............... \n\r");
    USB_PRINTF("Logical drive 0 attributes:\n\r");
    /* Get free clusters */
    returnCode = f_getfree(_T("0:"),&fre_clust,&fs);
    if (returnCode)
    {
        goto Error1;
    }
    /* Display drive status */
    USB_PRINTF(" FAT type = FAT%u\n\r Bytes/Cluster = %lu\n Number of FATs = %u\n\r" \
          " Root DIR entries = %u\n\r Sectors/FAT = %lu\n\r Number of clusters = %lu\n\r" \
          " FAT start (lba) = %lu\n DIR start (lba,clustor) = %lu\n Data start (lba) = %lu\n\n\r...\n",\
          ft[fs->fs_type & 3], fs->csize * 512UL, fs->n_fats,\
          fs->n_rootdir, fs->fsize, fs->n_fatent - 2, \
          fs->fatbase, fs->dirbase, fs->database);
    
    USB_PRINTF("%lu KB total disk space.\n\r%lu KB available.\n\r",(((fs->n_fatent - 2)*(fs->csize ))/2), ((fre_clust * (fs->csize ))/2));
    USB_PRINTF("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM3.  Make file system <f_mkfs>                                                     */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if _USE_MKFS
    /* Format drive */
    USB_PRINTF("\n\r3. Demo functions:f_mkfs\n\n\r");
    USB_PRINTF("Formatting drive ...............\n\r");
    returnCode = f_mkfs(0,0,4096);
    if (returnCode) 
    {
        goto Error1;
    }
    else
    {
        USB_PRINTF("Format complete\n\r");
    }
    USB_PRINTF("getting drive 0 attributes............... \n\r");
    USB_PRINTF("Logical drive 0 attributes:\n\r");
    /* Display drive status */
    returnCode = f_getfree("0:",&fre_clust,&fs);
    if (returnCode)
    {
        goto Error1;
    }
    USB_PRINTF(" FAT type = FAT%u\n\r Bytes/Cluster = %lu\n\r Number of FATs = %u\n\r" \
          " Root DIR entries = %u\n\r Sectors/FAT = %lu\n\r Number of clusters = %lu\n\r" \
          " FAT start (lba) = %lu\n\r DIR start (lba,clustor) = %lu\n\r Data start (lba) = %lu\n\n...\n\r",\
          ft[fs->fs_type & 3], fs->csize * 512UL, fs->n_fats,\
          fs->n_rootdir, fs->fsize, fs->n_fatent - 2, \
          fs->fatbase, fs->dirbase, fs->database);
    
    USB_PRINTF("%lu KB total disk space.\n\r%lu KB available.\n\r",(((fs->n_fatent - 2)*(fs->csize ))/2), ((fre_clust * (fs->csize ))/2));
#endif
    
    
#if (DIR_OPERATION)  
    USB_PRINTF("\n\r******************************************************************************");
    USB_PRINTF("\n\r*                             DRECTORY OPERATION                             *");
    USB_PRINTF("\n\r******************************************************************************");
    
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM1. Directory listing <f_opendir, f_readdir>                                        */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    
    USB_PRINTF("\n\r1. Demo functions:f_opendir, f_readdir\n\n");
    returnCode = List_Directory(_T(""));
    if (returnCode)
    {
        goto Error1;
    }
    USB_PRINTF("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM2. Create directory <f_mkdir>                                                      */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* Create Dir_1 as a sub directory of root */
    USB_PRINTF("\n\r2. Demo functions:f_mkdir\n\n\r");
    USB_PRINTF("2.0. Create <Dir_1> \n\r");
    returnCode = f_mkdir(_T("Dir_1"));
    if((returnCode != FR_OK)&&(returnCode != FR_EXIST))
    {
        goto Error1;
    }
    
    /* Create Dir_2 as a sub directory of root */ 
    USB_PRINTF("2.1. Create <Dir_2> \n\r");
    returnCode = f_mkdir(_T("Dir_2"));
    if((returnCode != FR_OK)&&(returnCode != FR_EXIST))
    {
        goto Error1;
    }
    
    /* Create Sub1 as a sub directory of Dir_1 */ 
    USB_PRINTF("2.2. Create <Sub1> as a sub directory of <Dir_1> \n\r");
    returnCode = f_mkdir(_T("Dir_1/sub1"));
    if((returnCode != FR_OK)&&(returnCode != FR_EXIST))
    {
        goto Error1;
    }
    /* List dir */
    USB_PRINTF("2.3. Directory list\n\r");
    returnCode = List_Directory(_T(""));
    if (returnCode)
    {
        goto Error1;
    }
    
    USB_PRINTF("2.4. \"Dir_1\" Directory list\n\r");
    returnCode = List_Directory(_T("Dir_1"));
    if (returnCode)
    {
        goto Error1;
    }
    USB_PRINTF("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM3. Get and Change current directory <f_getcwd, f_chdir>                            */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    USB_PRINTF("\n\r3. Demo functions:f_getcwd, f_chdir\n\n\r");
    /* Get the current directory */
    USB_PRINTF("3.0. Get the current directory \n\r");
    returnCode = f_getcwd((TCHAR *)buffer,256);
    if (returnCode)
    {
        goto Error1;
    }
    else 
    {
        USB_PRINTF("    CWD: %s\n\r",buffer);
    }
    
    /* Change directory to Dir_1 */
    USB_PRINTF("3.1. Change current directory to <Dir_1>\n\r");
    returnCode = f_chdir(_T("/Dir_1"));
    if (returnCode)
    {
        goto Error1;
    }
    
    /* List dir */
    USB_PRINTF("3.2. Directory listing \n\r");
    returnCode = List_Directory(_T("."));
    if (returnCode)
    {
        goto Error1;
    }
    
    /* Get the current directory */
    USB_PRINTF("3.3. Get the current directory \n\r");
    returnCode = f_getcwd((TCHAR *)buffer,256);
    if (returnCode)
    {
        goto Error1;
    }
    else 
    {
        USB_PRINTF("    CWD: %s\n\r",buffer);
    }
    USB_PRINTF("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM4. Get directory status, Change attribute and time <f_stat, f_chmod, f_utime>      */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    USB_PRINTF("\n\r4. Demo functions:f_stat(File status), f_chmod, f_utime\n\n\r");
    /* Get directory status */ 
    USB_PRINTF("4.1. Get directory information of <Dir_1>\n\r");
    returnCode = f_stat(_T("../Dir_1"),&Finfo);
    if(returnCode) 
    {
        goto Error1;
    }
    else
    {
        Display_File_Info(&Finfo);
    }
    
    /* Change timestamp */
    USB_PRINTF("4.2  Change the timestamp of Dir_1 to 12.25.2010: 23h 30' 20\n\r");
    Finfo.fdate.Bits.year = 2010 - YEAR_ORIGIN;
    Finfo.fdate.Bits.month = 12;
    Finfo.fdate.Bits.day = 25;
    
    
    Finfo.ftime.Bits.hour = 23;
    Finfo.ftime.Bits.minute = 30;
    Finfo.ftime.Bits.second = 20; 
    returnCode = f_utime(_T("../Dir_1"),&Finfo);
    if (returnCode) 
    {
        goto Error1;
    }
    
    /* Chang directory attribute */
    USB_PRINTF("4.3. Set Read Only Attribute to Dir_1\n\r");
    returnCode = f_chmod(_T("../Dir_1"),AM_RDO,AM_RDO);
    if(returnCode)
    {
        goto Error1;
    } 
    
    /* Get directory status */
    USB_PRINTF("4.4. Get directory information (Dir_1)\n\r");
    returnCode = f_stat(_T("../Dir_1"),&Finfo);
    if(returnCode) 
    {
        goto Error1;
    }
    else
    {
        Display_File_Info(&Finfo);
    }
    USB_PRINTF("-----------------------------------------------------------------------------\n");
    
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM5. Rename, Move directory < f_rename>                                              */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    USB_PRINTF("\n\r5. Demo functions:f_rename\n\n\r");
    USB_PRINTF("Rename <sub1> to <sub1_rm> and move it to <Dir_2> \n\r");
    returnCode = f_rename(_T("./sub1"),_T("../Dir_2/sub1_rm"));
    if(returnCode && returnCode != FR_EXIST)
    {
        goto Error1;
    }
    else 
    {
    
        List_Directory(_T("../Dir_2"));
    }
    
    /* Back to home directory */
    /*
    USB_PRINTF("\n\r6. Back to home directory\n\r");
    returnCode = f_chdir(_T(".."));
    if (returnCode)
    {
        goto Error1;
    }
    
    USB_PRINTF("\n\r7. List home files\n\n");
    returnCode = List_Directory(_T(""));
    if (returnCode)
    {
        goto Error1;
    }
    */

    USB_PRINTF("-----------------------------------------------------------------------------\n");
    
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM6. Delete directory < f_unlink>                                                    */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    USB_PRINTF("\n\r6. Demo functions:f_unlink\n\n\r");
    USB_PRINTF(" Delete Dir_1/sub1_rm\n\r");
    returnCode = f_unlink(_T("../Dir_2/sub1_rm"));
    if(returnCode) 
    {
        goto Error1;
    }
    else 
    {
        List_Directory(_T("../Dir_2"));
    }
#endif /* End of #if(DIR_OPERATION) */ 
    
#if (FILE_OPERATION) 
    USB_PRINTF("\n\r******************************************************************************");
    USB_PRINTF("\n\r*                             FILE OPERATION                                 *");
    USB_PRINTF("\n\r******************************************************************************");
    
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM1. Write to  file < f_open,f_write, f_printf, f_putc, f_puts, fclose>              */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    USB_PRINTF("\n\r1. Demo functions:f_open,f_write, f_printf, f_putc, f_puts, fclose\n\n\r");
    
    /* Create a new file to write */
    USB_PRINTF("1.0. Create new file <New_F_1> (f_open)\n\r");
    returnCode = f_stat(_T("New_F_1.dat"), &Finfo);
    if (returnCode == FR_OK)
    {
        USB_PRINTF("    File exist\n");
        if (Finfo.fattrib & AM_RDO)
        {
            USB_PRINTF("    Clear readonly attribute\n");
            returnCode = f_chmod(_T("New_F_1.dat"), 0, AM_RDO);
            if (returnCode)
                goto Error1;
        }
    }
    returnCode = f_open(&fil,_T("New_F_1.dat"),FA_WRITE|FA_CREATE_ALWAYS);
    if (returnCode) 
    {
        goto Error1;
    }
    USB_PRINTF("    File size = %4d\n\r",fil.fsize);
    
    /* Write data to file uses f_write function */
    USB_PRINTF("1.1. Write data to <New_F_1>(f_write)\n\r");
    returnCode = f_write(&fil,"Line 1: Write data to  file uses f_write function \n\r",52,&size);
    if (returnCode) 
    {
        goto Error1;
    }
    /* Flush cached data */
    USB_PRINTF("1.2. Flush cached data\n\r");
    returnCode = f_sync(&fil);
    if (returnCode) 
    {
        goto Error1;
    }
    USB_PRINTF("    File size = %4d\n",fil.fsize);
    
    /* Write data to file uses f_printf function */
    USB_PRINTF("1.3. Write data to <New_F_1> (f_printf)\n\r");
    
    if (EOF == f_printf(&fil,_T("Line %d: %s"),2,_T("Write data to file uses f_printf function\n\r"))) 
    {
        goto Error1;
    }
    
    /* Flush cached data */
    USB_PRINTF("1.4. Flush cached data\n\r");
    returnCode = f_sync(&fil);
    if (returnCode) 
    {
        goto Error1;
    }
    USB_PRINTF("    File size = %4d\n\r",fil.fsize);
    
    /* Write data to file uses f_puts function */
    USB_PRINTF("1.5. Write data to <New_F_1> (f_puts)\n\r");
    if (EOF == f_puts(_T("Line 3: Write data to file uses f_puts function\n\r"), &fil)) 
    {
        goto Error1;
    }
    /* Flush cached data */
    USB_PRINTF("1.6. Flush cached data\n\r");
    
    returnCode = f_sync(&fil);
    if (returnCode) 
    {
        goto Error1;
    }
    USB_PRINTF("    File size = %4d\n\r",fil.fsize);
    
    /* Write data to file uses f_putc function */
    USB_PRINTF("1.7. Write data to <New_F_1> uses f_putc function\n\r");
    while(*str_temp)
    {
        if (EOF == f_putc(*str_temp++,&fil))
        {
            goto Error1;
        }
    }
    /* Flush cached data */
    USB_PRINTF("1.8. Flush cached data\n\r");
    returnCode = f_sync(&fil);
    if (returnCode) 
    {
        goto Error1;
    }
    USB_PRINTF("    File size = %4d\n\r",fil.fsize);
    
    /* Close file */
    USB_PRINTF("1.9. Close file <New_F_1>\n\r");
    returnCode = f_close(&fil);
    if (returnCode) 
    {
        goto Error1;
    }
    
    USB_PRINTF("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM2. Read from a file < f_open,f_read, f_gets, fclose>                       */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    USB_PRINTF("\n\r2. Demo functions:f_open,f_read, f_seek, f_gets, f_close\n\n\r");
    
    /* Open New_F_1 to read */
    USB_PRINTF("2.0. Open <New_F_1> to read (f_open)\n\r");
    returnCode = f_open(&fil,_T("New_F_1.dat"),FA_READ);
    if(returnCode)
    {
        goto Error1;
    }
    
    /* Use f_gets to get a string from file */
    USB_PRINTF("2.1. Get a string from file (f_gets)\n\r");
    p_str = buffer;
    p_str = (char *)f_gets((TCHAR*)p_str,256,&fil);
    USB_PRINTF("    %s",buffer);
    
    /* Use f_read to get the rest of file content */
    USB_PRINTF("\n\r2.2. Get the rest of file content (f_read)\n\r");
    fil.fptr++;
    while(fil.fptr < fil.fsize) 
    {
        returnCode = f_read(&fil, buffer, sizeof(buffer)-1,&size);
        if(returnCode)
        {
            goto Error1;
        }
        else
        {
            buffer[size] = '\0';
            USB_PRINTF("    %s",buffer);
        }
    }
    
    /* Close file */
    USB_PRINTF("\n\r2.3. Close file (f_close)\n\r");
    returnCode = f_close(&fil);
    if(returnCode)
    {
        goto Error1;
    }
    USB_PRINTF("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM3.  Get file infor, Change attribute and timestamp of file <f_stat, f_utime, f_chmod>*/
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    USB_PRINTF("\n\r3. Demo functions:f_stat, f_utime, f_chmod\n\n\r");
    
    USB_PRINTF("3.1. Get  information of <New_F_1> file (f_stat)\n\r");
    returnCode = f_stat(_T("New_F_1.dat"),&Finfo);
    if(returnCode) 
    {
        goto Error1;
    }
    else
    {
        Display_File_Info(&Finfo);
    }
    
    /* Change timestamp */
    USB_PRINTF("3.2  Change the timestamp of Dir_1 to 12.25.2010: 23h 30' 20 (f_utime)\n\r");
    Finfo.fdate.Bits.year = 2010 - YEAR_ORIGIN;
    Finfo.fdate.Bits.month = 12;
    Finfo.fdate.Bits.day = 25;
    
    
    Finfo.ftime.Bits.hour = 23;
    Finfo.ftime.Bits.minute = 30;
    Finfo.ftime.Bits.second = 10; 
    
    returnCode = f_utime(_T("New_F_1.dat"),&Finfo);
    if (returnCode) 
    {
        goto Error1;
    }
    
    /* Chang directory attribute */
    USB_PRINTF("3.3. Set Read Only Attribute to <New_F_1> (f_chmod) \n\r");
    returnCode = f_chmod(_T("New_F_1.dat"),AM_RDO,AM_RDO);
    if(returnCode)
    {
        goto Error1;
    } 
    
    /* Get directory status */
    USB_PRINTF("3.4. Get directory information of <New_F_1> (f_stat)\n\r");
    returnCode = f_stat(_T("New_F_1.dat"),&Finfo);
    if(returnCode) 
    {
        goto Error1;
    }
    else
    {
        Display_File_Info(&Finfo);
    }
    
    /* Chang directory attribute */
    USB_PRINTF("3.5. Clear Read Only Attribute of <New_F_1> (f_chmod)\n\r");
    returnCode = f_chmod(_T("New_F_1.dat"),0,AM_RDO);
    if(returnCode)
    {
        goto Error1;
    } 
    
    /* Get directory status */
    USB_PRINTF("3.6. Get directory information of <New_F_1>\n\r");
    returnCode = f_stat(_T("New_F_1.dat"),&Finfo);
    if(returnCode) 
    {
        goto Error1;
    }
    else
    {
        Display_File_Info(&Finfo);
    }
    
    USB_PRINTF("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM4.  Change file name <f_rename>                                                    */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    USB_PRINTF("\n\r4. Demo functions:f_ulink\n\n\r");
    USB_PRINTF(" Rename <New_F_1.dat> to  <File_Rm.txt> \n\r");
    returnCode = f_rename(_T("New_F_1.dat"),_T("File_Rm.txt"));
    if(returnCode && returnCode != FR_EXIST)
    {
        goto Error1;
    }
    else 
    {
    
        List_Directory(_T("."));
    }
    
    USB_PRINTF("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* TC5.  Truncate file <f_truncate,f_lseek>                                             */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    USB_PRINTF("\n\r5. Demo functions:f_truncate\n\n\r");
    USB_PRINTF(" Truncate file <File_Rm.txt>\n\r");
    /* Open File_Rm.txt to read */
    USB_PRINTF("5.0. Open <File_Rm.txt> to write\n\r");
    returnCode = f_open(&fil,_T("File_Rm.txt"),FA_WRITE);
    if(returnCode)
    {
        goto Error1;
    }
    
    /* Seek file pointer */
    USB_PRINTF("5.1. Seek file pointer\n\r    Current file pointer: %4d\n\r",fil.fptr);
    returnCode = f_lseek(&fil, 102);
    USB_PRINTF("    File pointer after seeking: %4d\n\r",fil.fptr);
    
    /* Truncate file */
    USB_PRINTF("5.2. Truncate file\n\r");
    returnCode = f_truncate(&fil); 
    if(returnCode)
    {
        goto Error1;
    }
    USB_PRINTF("    File size = %4d\n\r",fil.fsize);
    
    /* Close file */
    USB_PRINTF("5.3. Close file\n\r");
    returnCode = f_close(&fil);
    if(returnCode)
    {
        goto Error1;
    }
    
    USB_PRINTF("-----------------------------------------------------------------------------\n\r");
#if _FS_TINY
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM6.  Forward file <f_forward>                                                       */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    USB_PRINTF("\n\r6. Demo functions:f_forward\n\n\r"); 
    /* Open File_Rm.txt to read */
    USB_PRINTF("6.0. Open <File_Rm.txt> to read\n\r");
    returnCode = f_open(&fil,_T("File_Rm.txt"),FA_READ);
    if(returnCode)
    {
        goto Error1;
    }
    
    /* Forward file to terminal */ 
    USB_PRINTF("6.1. Forward file to terminal\n\r");
    returnCode = f_forward(&fil, out_stream, 102,&size);   
    if(returnCode)
    {
        goto Error1;
    }
    
    /* Close file */
    USB_PRINTF("\n\r6.2. Close file\n\r");
    returnCode = f_close(&fil);
    if(returnCode)
    {
        goto Error1;
    }
#endif
    
    
    USB_PRINTF("-----------------------------------------------------------------------------\n\r");
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* DM7.  Delete a file <f_ulink>                                                    */
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    USB_PRINTF("\n\r7. Demo functions:f_ulink\n\n");
    USB_PRINTF(" Delete <File_Rm.txt>\n\r");
    /* returnCode = f_unlink(_T("File_Rm.txt")); */
    if(returnCode) 
    {
        goto Error1;
    }
    else 
    {
        List_Directory(_T("."));
    }

#endif /* End of #if (FILE_OPERATION) */

#if _USE_LFN
    if(NULL != Finfo.lfname)
    {
        /* Free memory for Finfo->lfname */
        ff_memfree(Finfo.lfname);
    }
#endif
    USB_PRINTF("\n\r*------------------------------   DEMO COMPLETED    ------------------------ *");
    USB_PRINTF("\n\r******************************************************************************\n\r");
    return FR_OK;       
Error1:
#if _USE_LFN
    if(NULL != Finfo.lfname)
    {
        /* Free memory for Finfo->lfname */
        ff_memfree(Finfo.lfname);
    }
#endif
    return(put_rc(returnCode));
}

#if THROUGHPUT_TEST_ENABLE

#ifndef  DEMCR
#define  DEMCR       *((volatile uint32_t *)0xE000EDFC)
#endif
#ifndef  DWT_CR
#define  DWT_CR      *((volatile uint32_t *)0xE0001000)                   /* Data Watchpoint and Trace (DWT) Control Register     */                
#endif
#ifndef  DWT_CYCCNT
#define  DWT_CYCCNT  *((volatile uint32_t *)0xE0001004)                   /* Data Watchpoint and Trace (DWT) Cycle Count Register */
#endif
#define  DEMCR_TRCENA                     (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)


#define EACH_TRANSFOR_SECTORS           (128)
#define FATFS_FORMATE                   (0)

uint32_t raw_size_arr[] = {100, 100}; /* 100M, unit M */
uint8_t test_buffer[EACH_TRANSFOR_SECTORS * 512];
char file_name[30];
uint32_t g_cpu_core_clk_khz = 120000;
FIL fil;


void test_fun(void* volPtr);
int fat_throughput_test(void)
{
    FRESULT returnCode;
    uint8_t TEST_TIME = 0;
    uint8_t vol = 0;
    uint8_t volatile time = 0;
    uint64_t time_sum;
    uint32_t time_count;
    uint32_t rw_time;
    uint32_t size = 0;
    uint64_t temp;
    uint32_t raw_size = 0;
    
    /* get core clock */
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
    extern const TCpuClockConfiguration PE_CpuClockConfigurations[];
    g_cpu_core_clk_khz = PE_CpuClockConfigurations[Cpu_GetClockConfiguration()].cpu_core_clk_hz / 1000;
#elif (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_SDK)
    CLOCK_SYS_GetFreq(kCoreClock, &g_cpu_core_clk_khz);
    g_cpu_core_clk_khz /= 1000;
#endif
    
    DEMCR |= DEMCR_TRCENA; /* enable DWT module counter */
    TEST_TIME = sizeof(raw_size_arr) / sizeof(uint32_t);
    
    /* init write buffer */
    for (uint32_t i = 0; i < sizeof(test_buffer); ++i)
        test_buffer[i] = i;
    
    returnCode = f_mount(vol, &fatfs);
    if (returnCode)
    {
        return(put_rc(returnCode));
    }
    else
        USB_PRINTF("vol %d mount success\r\n", vol);
    
#if FATFS_FORMATE
    USB_PRINTF("******************************************************************************\r\n");
    USB_PRINTF("*                          vol %d FATFS MKFS                                   *\r\n", vol);
    USB_PRINTF("******************************************************************************\r\n");
    returnCode = f_mkfs(vol, 0, 4096);
    if (returnCode)
    {
        put_rc(returnCode);
        goto done;
    }
    USB_PRINTF("mkfs done\r\n");
#endif
    
    USB_PRINTF("******************************************************************************\r\n");
    USB_PRINTF("*             vol %d file.dat for write and read speed                        *\r\n", vol);
    USB_PRINTF("******************************************************************************\r\n");
    sprintf(file_name, "%c:/file.dat", vol + '0');
    
    for (time = 1; time <= TEST_TIME; ++time)
    {
        raw_size = raw_size_arr[time - 1];
        returnCode = f_unlink(file_name);
        if (returnCode != FR_OK && returnCode != FR_NO_FILE)
        {
            put_rc(returnCode);
            goto done;
        }
        returnCode = f_open(&fil, file_name, FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
        if (returnCode)
        {
            put_rc(returnCode);
            goto done;
        }
        
        /* write speed test */
        USB_PRINTF("***************************%d write %dM test**********************************\r\n", time, raw_size);
        time_sum = 0;
        rw_time = raw_size * 2048 / EACH_TRANSFOR_SECTORS;
        while (rw_time--)
        {
            DWT_CYCCNT = (uint32_t)0u;
            DWT_CR |= DWT_CR_CYCCNTENA;
            returnCode = f_write(&fil, test_buffer, EACH_TRANSFOR_SECTORS << 9, &size);
            
            if (returnCode)
            {
                f_close(&fil);
                {
                    put_rc(returnCode);
                    goto done;
                }
            }
            time_count = DWT_CYCCNT;
            DWT_CR &= ~((uint32_t)DWT_CR_CYCCNTENA);
            time_sum += (uint64_t)time_count;
        }
        time_sum = time_sum / (g_cpu_core_clk_khz);
        temp = (uint64_t)((uint64_t)512*(uint64_t)(raw_size * 2048));
        temp = temp * 1000 / (uint64_t)time_sum;
        USB_PRINTF("write test results: time = %dms  speed = %dK/s\r\n", (uint32_t)time_sum, (uint32_t)(temp / 1024));

        /* read speed test */
        USB_PRINTF("***************************%d read %dM test***********************************\r\n", time, raw_size);
        returnCode = f_lseek(&fil, 0);
        if (returnCode)
        {
            put_rc(returnCode);
            goto done;
        }
        time_sum = 0;
        while (fil.fptr < fil.fsize)
        {
            DWT_CYCCNT = (uint32_t)0u;
            DWT_CR |= DWT_CR_CYCCNTENA;
            returnCode = f_read(&fil, test_buffer, EACH_TRANSFOR_SECTORS << 9, &size);
            if(returnCode)
            {
                f_close(&fil);
                {
                    put_rc(returnCode);
                    goto done;
                }
            }
            time_count = DWT_CYCCNT;
            DWT_CR &= ~((uint32_t)DWT_CR_CYCCNTENA);
            time_sum += (uint64_t)time_count;
        }
        time_sum = time_sum / (g_cpu_core_clk_khz);
        temp = (uint64_t)((uint64_t)512*(uint64_t)(raw_size * 2048));
        temp = temp * 1000 / (uint64_t)time_sum;
        USB_PRINTF("read test results: time = %dms  speed = %dK/s\r\n", (uint32_t)time_sum, (uint32_t)(temp / 1024));
        
        returnCode = f_close(&fil);
        if (returnCode)
        {
            put_rc(returnCode);
            goto done;
        }
    }

done:
    USB_PRINTF("******************************************************************************\r\n");
    USB_PRINTF("*                          vol %d TEST DONE                                   *\r\n", vol);
    USB_PRINTF("******************************************************************************\r\n");
    
    return 0;
}
#endif

