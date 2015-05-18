/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   This file contains the initialization and menu source code for
*   the USB mass storage MFS test example program using USB mass storage
*   link driver.
*
*
*END************************************************************************/
#include <stdio.h>
#include <nio.h>
#include <ioctl.h>
#include <fs_supp.h>
#include <unistd.h>

#include <mfs.h>
#include <part_mgr.h>
#include <mfs_usb.h>

#include "usb_file.h"


static char *dev_name[4] = {"USB1:", "USB2:", "USB3:", "USB4:"};
static char *pm_name[4] = {"PM1:", "PM2:", "PM3:", "PM4:"};


/* Mounts the filesystem instance given by open fd of filesystem and its assigned name. */
static int mfs_mount(int block_device, char *fs_name);
/* Unmounts the filesystem instance given by open fd of filesystem and its assigned name. */
int mfs_unmount(int fs_fd_ptr, char *fs_name, bool force);
/* Fills in the pointer to strings to register new created filesystem device. */
static int msd_assign_drive_letter(int block_device, char *fs_name, uint32_t *mountp);
/* Retrieves mask indicating valid partitions found on the device. */
static int get_valid_partitions(int pm_fd_ptr, uint32_t *valid_partitions);

static int mfs_mount(int block_device, char *fs_name)
{
    int fs_fd_ptr;
    int error_code;

    /* Install MFS */
    error_code = _io_mfs_install(block_device, fs_name, 0);
    if (error_code != MFS_NO_ERROR)
    {
        printf("Error initializing MFS: %s\n", MFS_Error_text((uint32_t)error_code));
        return -1;
    }

    /* Open filesytem */
    fs_fd_ptr = open(fs_name, 0);

    /* Handle error states */
    if (0 > fs_fd_ptr)
    {
        printf("Error opening filesystem: %d\n", errno);
    }
    else
    {
        /* This function is to register filesystem in the OS. Used by shell. */
        _io_register_file_system(fs_fd_ptr, fs_name);
    }

    return fs_fd_ptr;
}


int mfs_unmount(int fs_fd_ptr, char *fs_name, bool force)
{
    int error_code = MQX_OK;

    if (fs_name == NULL) {
        fs_name = "";
    }

    if (fs_fd_ptr) {
        error_code = close(fs_fd_ptr);
        /* This function is to unregister filesystem from the OS. Used by shell. */
        _io_unregister_file_system(fs_fd_ptr);
        if (error_code < 0)
        {
            printf("Error while closing filesystem %s\n", fs_name);
        }
    }
    if (*fs_name) {

      /* Uninstall MFS */
      if (force)
          error_code = _nio_dev_uninstall_force(fs_name, NULL);
      else
          error_code = _nio_dev_uninstall(fs_name, NULL);

      if (error_code < 0)
        {
            printf("Error while uninstalling filesystem %s\n", fs_name);
        }
    }

    return error_code;
}


static int msd_assign_drive_letter(int block_device, char *fs_name, uint32_t *mountp)
{
    int fs_fd_ptr;
    uint32_t mask;
    char drive_letter;

    /* Find free mountpint (drive letter) */
    mask = 1;
    for (drive_letter = 'a'; drive_letter <= 'z'; drive_letter++) {
        if ((*mountp & mask) == 0)
            break;
        mask <<= 1;
    }

    if (drive_letter > 'z') {
        printf("No drive letter available\n");
        return -1;
    }

    fs_name[0] = drive_letter;
    fs_name[1] = ':';
    fs_name[2] = '\0';

    fs_fd_ptr = mfs_mount(block_device, fs_name);
    if (0 < fs_fd_ptr) {
        *mountp |= mask;
    }
    else {
        *fs_name = '\0';
    }

    return fs_fd_ptr;
}

static int get_valid_partitions(
        int pm_fd_ptr,
        uint32_t *valid_partitions
    )
{
    int error_code;
    uint32_t part;
    uint32_t nullpart = 0;

    if (NULL == valid_partitions) {
        return MQX_INVALID_PARAMETER;
    }

    *valid_partitions = 0;

    error_code = ioctl(pm_fd_ptr, IO_IOCTL_SEL_PART, &nullpart);
    if (error_code < 0) {
        return error_code;
    }

    error_code = ioctl(pm_fd_ptr, IO_IOCTL_VAL_PART, &nullpart);
    if (error_code < 0) {
        return error_code;
    }

    /* Select partition */
    for (part = 1; part <= 4; part++) {
        error_code = ioctl(pm_fd_ptr, IO_IOCTL_SEL_PART, &part);
        if (error_code >= 0) {
            *valid_partitions |= (1 << part);
        }

        error_code = ioctl(pm_fd_ptr, IO_IOCTL_SEL_PART, &nullpart);
        if (error_code < 0) {
            break;
        }
    }

    return error_code;
}

int usb_msd_install
    (
        struct msd_dev_data_struct *msd_dev_data,
        uint32_t    *mountp
    )
{
    uint32_t error_code;
    uint32_t part = 0;
    uint32_t valid_parts;
    uint32_t dev_id = -1;
    uint32_t pm_id = -1;

    /* Install USB device */
    do {
        dev_id++;
        if (0 < _io_usb_mfs_install(dev_name[dev_id], msd_dev_data->ccs))
        {
            printf("Error while installing USB device (0x%X)\n", errno);
            return -1;
        }
    } while ((NIO_EEXIST == errno) && (dev_id < (ELEMENTS_OF(dev_name) - 1)));

    _time_delay(500);

    /* Open the USB mass storage device */
    msd_dev_data->DEV_NAME = dev_name[dev_id];
    msd_dev_data->DEV_FD_PTR = open(msd_dev_data->DEV_NAME, 0);

    if (0 > msd_dev_data->DEV_FD_PTR)
    {
        printf("Unable to open USB device\n");
        return _task_get_error();
    }

    /* Get the vendor information and display it */
/*    printf("\n************************************************************************\n");
    ioctl(msd_dev_data->DEV_FD_PTR, IO_IOCTL_GET_VENDOR_INFO, &dev_info);
    printf("Vendor Information:     %-1.8s Mass Storage Device\n",dev_info);
    ioctl(msd_dev_data->DEV_FD_PTR, IO_IOCTL_GET_PRODUCT_ID, &dev_info);
    printf("Product Identification: %-1.16s\n", dev_info);
    ioctl(msd_dev_data->DEV_FD_PTR, IO_IOCTL_GET_PRODUCT_REV, &dev_info);
    printf("Product Revision Level: %-1.4s\n", dev_info);
    printf("************************************************************************\n");

    ioctl(msd_dev_data->DEV_FD_PTR, IO_IOCTL_SET_BLOCK_MODE, NULL);
*/

    /* Try to install the partition manager */
    do {
        pm_id++;
        error_code = _io_part_mgr_install(msd_dev_data->DEV_FD_PTR, pm_name[pm_id], 0);
    } while (MQX_OK != error_code && NIO_EEXIST == errno && pm_id < (ELEMENTS_OF(pm_name) - 1));

    if (error_code != MFS_NO_ERROR)
    {
        printf("Error while installing partition manager: %s\n", MFS_Error_text((uint32_t)error_code));
        return error_code;
    }

    /* Open partition manager */
    msd_dev_data->PM_NAME = pm_name[pm_id];
    msd_dev_data->PM_FD_PTR[0] = open(msd_dev_data->PM_NAME, 0);

    if (0 > msd_dev_data->PM_FD_PTR[0])
    {
        printf("Error while opening partition manager: %d\n", errno);
        return error_code;
    }

    /* Validate MBR */
    error_code = get_valid_partitions(msd_dev_data->PM_FD_PTR[0], &valid_parts);

    if ((error_code != MQX_OK) || (valid_parts == 0))
    {
        /* Install MFS on whole storage device */
        printf("Installing MFS over USB device...\n");

        /* Close partition manager */
        close(msd_dev_data->PM_FD_PTR[0]);
        msd_dev_data->PM_FD_PTR[0] = -1;

        /* Uninstall partition manager */
        error_code = _nio_dev_uninstall(msd_dev_data->PM_NAME, NULL);
        if (error_code != MFS_NO_ERROR)
        {
            printf("Error while uninstalling partition manager.\n");
        }
        msd_dev_data->PM_NAME = NULL;

        /* Assign drive letter to whole device */
        msd_dev_data->FS_FD_PTR[0] = msd_assign_drive_letter(msd_dev_data->DEV_FD_PTR, msd_dev_data->FS_NAME[0], mountp);
        if (0 < msd_dev_data->FS_FD_PTR[0]) {
            printf("Storage device installed as %s\n", msd_dev_data->FS_NAME[0]);
        }
    }

    else {
        /* Install MFS over partitions */
        printf("Installing MFS over partitions...\n");

        for (part = 1; part <= 4 ;part++) {

            /* Open partition manager */
            msd_dev_data->PM_FD_PTR[part] = open(msd_dev_data->PM_NAME, 0);

            if (0 > msd_dev_data->PM_FD_PTR[part])
            {
                printf("Error while opening partition manager: %d\n", errno);
                return errno; /* It probably does not make a sense to try other partitions at this point */
            }

            /* Select partition */
            error_code = ioctl(msd_dev_data->PM_FD_PTR[part], IO_IOCTL_SEL_PART, &part);

            if (error_code != MFS_NO_ERROR)
            {
                /* This particular partition cannot be selected but this is not a fatal error */
                close(msd_dev_data->PM_FD_PTR[part]);
                msd_dev_data->PM_FD_PTR[part] = -1;
                continue;
            }

            /* Validate partition */
            error_code = ioctl(msd_dev_data->PM_FD_PTR[part], IO_IOCTL_VAL_PART, NULL);
            if (error_code != MFS_NO_ERROR)
            {
                printf("Error while validating partition: %s\n", MFS_Error_text((uint32_t)error_code));
                printf("Not installing MFS.\n");
                continue;
            }

            /* Assign drive letter for the partition */
            msd_dev_data->FS_FD_PTR[part] = msd_assign_drive_letter(msd_dev_data->PM_FD_PTR[part], msd_dev_data->FS_NAME[part], mountp);
            if (0 < msd_dev_data->FS_FD_PTR[part]) {
                printf("Partition %d installed as %s\n", part, msd_dev_data->FS_NAME[part]);
            }
        }
    }

    return MQX_OK;
}


int usb_msd_uninstall
    (
        void        *usb_handle,
        uint32_t    *mountp,
        bool        force
    )
{
    MSD_DEV_DATA_PTR msd_dev_data = (MSD_DEV_DATA_PTR) usb_handle;

    uint32_t part;
    uint32_t error_code;
    int drive_letter_index;


    if (msd_dev_data == NULL) {
        return MQX_INVALID_PARAMETER;
    }

    for (part=0; part<=4 ; part++) {
        /* Check for invalid parameters is done internally in mfs_unmount */
        mfs_unmount(msd_dev_data->FS_FD_PTR[part], msd_dev_data->FS_NAME[part], force);

        /* Mark drive letter as free */
        if (msd_dev_data->FS_NAME[part][0]) {
            drive_letter_index = msd_dev_data->FS_NAME[part][0] - 'a';
            *mountp &= ~(1 << drive_letter_index);
        }

        /* Close partition manager, if any */
        if ((0 < msd_dev_data->PM_FD_PTR[part]) && (MQX_OK != close(msd_dev_data->PM_FD_PTR[part])))
        {
            printf("Error while closing partition manager.\n");
        }
    }

    /* All partition manager instances are closed for sure at this point, uninstall partition manager (if any) */
    if (msd_dev_data->PM_NAME && *(msd_dev_data->PM_NAME)) {
        if (force)
            error_code = _nio_dev_uninstall_force(msd_dev_data->PM_NAME, NULL);
        else
            error_code = _nio_dev_uninstall(msd_dev_data->PM_NAME, NULL);
    }

    if (error_code < 0) {
        printf("Error while uninstalling partition manager.\n");
    }

    /* Close USB device */
    if ((0 < msd_dev_data->DEV_FD_PTR) && (MQX_OK != close(msd_dev_data->DEV_FD_PTR)))
    {
        printf("Unable to close USB device.\n");
    }

    /* Uninstall USB device */
    if (msd_dev_data->DEV_NAME && *(msd_dev_data->DEV_NAME)) {
        if (force)
            error_code = _nio_dev_uninstall_force(msd_dev_data->DEV_NAME, NULL);
        else
            error_code = _nio_dev_uninstall(msd_dev_data->DEV_NAME, NULL);
    }

    if (error_code < 0) {
        printf("Error while uninstalling USB device.\n");
    }

    printf("USB device removed.\n");

    return MQX_OK;
}


/* EOF */
