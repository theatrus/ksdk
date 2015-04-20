/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
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
*   This file contains the functions that are used to be used on clusters.
*
*
*END************************************************************************/

#include "mfs.h"
#include "mfs_prv.h"


#if !MFSCFG_READ_ONLY

void MFS_Increment_free_clusters(MFS_DRIVE_STRUCT_PTR drive_ptr)
{
    if ( drive_ptr->FREE_COUNT != FSI_UNKNOWN )
    {
        drive_ptr->FREE_COUNT++;
    }
}

void MFS_Decrement_free_clusters(MFS_DRIVE_STRUCT_PTR drive_ptr)
{
    if ( drive_ptr->FREE_COUNT != FSI_UNKNOWN )
    {
        drive_ptr->FREE_COUNT--;
    }
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Find_unused_cluster_from
* Returned Value   :  cluster_number, 0xffff means that none is available
* Comments  :
*      Get an unused cluster number. Assumes that the FAT has been read
*      Assumes drive SEM is locked.
*END*---------------------------------------------------------------------*/

uint32_t MFS_Find_unused_cluster_from
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,      /*[IN] the drive on which to operate */
    uint32_t                 cluster_number  /*[IN] first cluster number to search */
    )
{
    uint32_t      max_clusters,cluster;
    _mfs_error   error_code;
    uint32_t      cluster_status;

    max_clusters = drive_ptr->LAST_CLUSTER;
    for ( cluster = cluster_number; cluster <= max_clusters; cluster++ )
    {
        error_code = MFS_get_cluster_from_fat(drive_ptr,cluster,&cluster_status);
        if ( error_code != MFS_NO_ERROR )
        {
            return(CLUSTER_INVALID);
        }
        else if ( cluster_status == CLUSTER_UNUSED )
        {
            drive_ptr->NEXT_FREE_CLUSTER = cluster;
            return(cluster);
        }
    }

    for ( cluster = CLUSTER_MIN_GOOD; cluster < cluster_number; cluster++ )
    {
        error_code = MFS_get_cluster_from_fat(drive_ptr,cluster, &cluster_status);
        if ( error_code != MFS_NO_ERROR )
        {
            return(CLUSTER_INVALID);
        }
        else if ( cluster_status == CLUSTER_UNUSED )
        {
            drive_ptr->NEXT_FREE_CLUSTER = cluster;
            return(cluster);
        }
    }

    return(CLUSTER_INVALID);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Release_chain
* Returned Value   :  MFS error code
* Comments  :
*      Follow a chain of clusters and mark the entries as unused.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Release_chain
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,      /*[IN] the drive on which to operate */
    uint32_t                 cluster_number  /*[IN] first cluster number to release */
    )
{
    uint32_t    next_cluster;
    _mfs_error error_code;

    if ( (cluster_number >= CLUSTER_MIN_GOOD) && (cluster_number <= drive_ptr->LAST_CLUSTER) )
    {
        do
        {
            error_code = MFS_get_cluster_from_fat(drive_ptr,cluster_number,&next_cluster);
            if ( error_code != MFS_NO_ERROR )
            {
                break;
            }
            else if ( ((next_cluster > drive_ptr->LAST_CLUSTER) || (next_cluster < CLUSTER_MIN_GOOD)) && (next_cluster != CLUSTER_EOF) )
            {
                error_code = MFS_LOST_CHAIN;
                break;
            }

            error_code = MFS_Put_fat(drive_ptr,cluster_number, CLUSTER_UNUSED);
            if ( error_code )
            {
                break;
            }

            /* Invalidation of data sectors of released chain here may certainly save some write operations but it is mostly a corner case */
            error_code = MFS_sector_cache_invalidate(drive_ptr, CLUSTER_TO_SECTOR(drive_ptr, cluster_number), drive_ptr->SECTORS_PER_CLUSTER);
            if ( error_code )
            {
                break;
            }

            cluster_number = next_cluster;
        } while ( cluster_number != CLUSTER_EOF );
    }
    else
    {
        error_code = MFS_NO_ERROR;
    }

    return(error_code);
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Extend_chain
* Returned Value   :  MFS error code
* Comments  :
*      Find a free cluster, follow a chain and add it to the chain.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Extend_chain
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,      /*[IN] the drive on which to operate */
    uint32_t                 cluster_number, /*[IN] number of a cluster within the chain */
    uint32_t                 num_clusters,   /*[IN] number of clusters to append to chain */
    uint32_t             *added_cluster   /* [IN] pointer to the place where we should put the # of the first  new cluster. */
    )
{
    uint32_t    next_cluster;
    uint32_t    extended;
    _mfs_error error_code;

    if ( cluster_number < CLUSTER_MIN_GOOD || cluster_number > drive_ptr->LAST_CLUSTER )
    {
        return MFS_INVALID_CLUSTER_NUMBER;
    }

    if ( num_clusters <= 0 )
    {
        return MFS_NO_ERROR;
    }

    /* Find the end of the chain */
    next_cluster = cluster_number;
    do
    {
        cluster_number = next_cluster;
        error_code = MFS_get_cluster_from_fat(drive_ptr, cluster_number, &next_cluster);
        if ( error_code != MFS_NO_ERROR )
        {
            return error_code;
        }
        else if ( (next_cluster > drive_ptr->LAST_CLUSTER || next_cluster < CLUSTER_MIN_GOOD) && next_cluster != CLUSTER_EOF )
        {
            return MFS_LOST_CHAIN;
        }
    } while ( next_cluster != CLUSTER_EOF );

    extended = 0;

    while ( num_clusters )
    {
        /* Find a free cluster */
        next_cluster = MFS_Find_unused_cluster_from(drive_ptr, cluster_number+1);

        /* Check to see if the disk is not full */
        if ( next_cluster == CLUSTER_INVALID )
        {
            break; /* Condition handled after the loop */
        }

        /* Link the free cluster to the chain, at the end */
        error_code = MFS_Put_fat(drive_ptr, cluster_number, next_cluster);
        if ( error_code != MFS_NO_ERROR )
        {
            return error_code;
        }

        if ( extended==0 && added_cluster!=NULL )
        {
            *added_cluster = next_cluster;
        }

        extended++;
        cluster_number = next_cluster;
        num_clusters--;
    }

    /* If the chain was extended, write EOF to it's last link */
    if ( extended )
    {
        error_code = MFS_Put_fat(drive_ptr, cluster_number, CLUSTER_EOF);
    }

    if ( num_clusters && !error_code)
    {
        error_code = MFS_DISK_FULL;
    }

    return error_code;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Clear_cluster
* Returned Value   :  MFS error code
* Comments  :
*     It resets all the bytes in a cluster to zero.
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Clear_cluster
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,  /*[IN] the drive on which to operate */
    uint32_t                cluster     /*[IN] the # of the cluster to clear*/
    )
{
    _mfs_error    error_code;
    uint32_t      sector, sector_count;
    void         *sector_buffer;

    error_code = MFS_NO_ERROR;

    sector = CLUSTER_TO_SECTOR(drive_ptr, cluster);
    sector_count=drive_ptr->SECTORS_PER_CLUSTER;

    while (sector_count--)
    {
        /* Map sector and mark if for overwrite */
        error_code = MFS_sector_map(drive_ptr, sector, &sector_buffer, MFS_MAP_MODE_OVERWRITE, 0);
        if (error_code != MFS_NO_ERROR) {
            break;
        }

        /* The sector buffer is cleared automatically when mapped in overwrite mode, just unmap it */
        error_code = MFS_sector_unmap(drive_ptr, sector, 1);
        if (error_code != MFS_NO_ERROR) {
            break;
        }

        sector++;
    }

    return error_code;
}

#endif


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Bad_clusters
* Returned Value   :  error_code
* Comments  :  Get the number of bad clusters in the file system.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Bad_clusters
    (
    MFS_DRIVE_STRUCT_PTR     drive_ptr,
    uint32_t                *bad_clusters_ptr
    )
{
    uint32_t                 last_cluster;
    uint32_t                 k;
    uint32_t                 bad_slots;
    _mfs_error               error_code;
    uint32_t                 cluster_status;

    if (bad_clusters_ptr == NULL)
    {
        return MFS_INVALID_POINTER;
    }

    bad_slots = 0;

    error_code = MFS_lock_and_enter(drive_ptr, 0);
    if ( error_code != MFS_NO_ERROR )
    {
        return error_code;
    }

    last_cluster = drive_ptr->LAST_CLUSTER;

    for ( k = CLUSTER_MIN_GOOD; k <= last_cluster; k++ )
    {
        error_code = MFS_get_cluster_from_fat(drive_ptr, k, &cluster_status);
        if ( error_code != MFS_NO_ERROR )
        {
            break;
        }
        else if ( cluster_status == CLUSTER_BAD )
        {
            bad_slots++;
        }
    }

    *bad_clusters_ptr = bad_slots;

    MFS_leave_and_unlock(drive_ptr, 0);

    return error_code;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Last_cluster
* Returned Value   :  error_code
* Comments  :  Get the number of the last cluster in the file system.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Last_cluster
    (
    MFS_DRIVE_STRUCT_PTR     drive_ptr,
    uint32_t                *last_cluster_ptr
    )
{
    _mfs_error               error_code;

    if (last_cluster_ptr == NULL)
    {
        return MFS_INVALID_POINTER;
    }

    error_code = MFS_lock_and_enter(drive_ptr, 0);
    if ( error_code != MFS_NO_ERROR )
    {
        return error_code;
    }

    *last_cluster_ptr = drive_ptr->LAST_CLUSTER;

    MFS_leave_and_unlock(drive_ptr, 0);

    return error_code;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Get_disk_free_space
* Returned Value   :  error code
* Comments  :  Get number of free clusters/bytes on the disk.
*
*END*---------------------------------------------------------------------*/

_mfs_error  MFS_Get_disk_free_space
    (
    MFS_DRIVE_STRUCT_PTR     drive_ptr,
    uint32_t                *clusters_free_ptr,
    uint64_t                *bytes_free_ptr
    )
{
    uint32_t                 clusters_free;
    uint32_t                 error_code;

    error_code = MFS_lock_and_enter(drive_ptr, 0);
    if (error_code != MFS_NO_ERROR)
    {
        return error_code;
    }

    error_code = MFS_Get_disk_free_space_internal(drive_ptr, &clusters_free);

    if (clusters_free_ptr)
    {
        *clusters_free_ptr = clusters_free;
    }

    if (bytes_free_ptr)
    {
        *bytes_free_ptr = ((uint64_t)clusters_free) << drive_ptr->CLUSTER_POWER_BYTES;
    }

    MFS_leave_and_unlock(drive_ptr, 0);

    return error_code;
}


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :  MFS_Get_disk_free_space_internal
* Returned Value   :  error code
* Comments  :  Get number of free clusters on the disk.
*
*END*---------------------------------------------------------------------*/

_mfs_error MFS_Get_disk_free_space_internal
    (
    MFS_DRIVE_STRUCT_PTR    drive_ptr,
    uint32_t               *free_clusters_ptr
    )
{
    uint32_t  last_cluster;
    uint32_t  k;
    uint32_t  free_slots;
    _mfs_error error_code = MFS_NO_ERROR;
    uint32_t  cluster_status;

    last_cluster = drive_ptr->LAST_CLUSTER;

    if ( drive_ptr->FREE_COUNT == FSI_UNKNOWN )
    {
        free_slots = 0;

        for ( k = CLUSTER_MIN_GOOD; k <= last_cluster; k++ )
        {
            error_code = MFS_get_cluster_from_fat(drive_ptr, k, &cluster_status);
            if ( error_code != MFS_NO_ERROR )
            {
                break;
            }
            else if ( cluster_status == CLUSTER_UNUSED )
            {
                free_slots++;
            }
        }

        if ( error_code == MFS_NO_ERROR )
        {
            drive_ptr->FREE_COUNT = free_slots;
        }
    }
    else
    {
        free_slots = drive_ptr->FREE_COUNT;
    }

    if (free_clusters_ptr != NULL)
    {
        *free_clusters_ptr = free_slots;
    }

    return error_code;
}
