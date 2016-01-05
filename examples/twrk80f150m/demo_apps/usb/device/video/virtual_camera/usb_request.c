/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013- 2014 Freescale Semiconductor;
* All Rights Reserved
*
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
************************************************************************** *
 * @file usb_descriptor.c
 *
 * @author
 *
 * @version
 *
 * @date 
 *
 * @brief The file contains USB descriptors for Audio Application
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device_stack_interface.h"
#include "usb_descriptor.h"

#include "usb_descriptor.h"
#include "virtual_camera.h"   /* Virtual camera Application Header File */
#include "usb_request.h"

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/


uint8_t g_cur_brightness[2] = {0x00,0x00};
/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Brightness
*
* @brief The function sets current brightness value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
*Set current brightness value specified by the Host
*****************************************************************************/
uint8_t USB_Desc_Set_Cur_Brightness(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    
    

    g_cur_brightness[0]=**data;
    g_cur_brightness[1]=*(*data+1);

    return USB_OK;
}
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Brightness
*
* @brief The function gets the current Brightness value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return current brightness value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Cur_Brightness(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size = 2;
    *data = g_cur_brightness;

    return USB_OK;
}
uint8_t g_min_brightness[2] = {0xf0,0xff};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Min_Brightness
*
* @brief The function gets the minimum Brightness value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return minimum brightness value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Min_Brightness(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size = 2;
    *data = g_min_brightness;

    return USB_OK;
}
uint8_t g_max_brightness[2] = {0x10,0x00};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Max_Brightness
*
* @brief The function gets the maximum Brightness value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return maximum brightness value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Max_Brightness(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size = 2;
    *data = g_max_brightness;

    return USB_OK;
}

uint8_t g_res_brightness[2] = {0x01,0x00};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Res_Brightness
*
* @brief The function gets the resolution Brightness value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return resolution brightness value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Res_Brightness(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size = 2;
    *data = g_res_brightness;

    return USB_OK;
}

uint8_t g_info_brightness[1] = {0x03};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Info_Brightness
*
* @brief The function gets the information Brightness value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return information brightness value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Info_Brightness(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size = 1;
    *data = g_info_brightness;

    return USB_OK;
}

uint8_t g_def_brightness[2] = {0x01,0x00};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Def_Brightness
*
* @brief The function gets the default Brightness value
*
* @param controller_ID : Controller ID
* @param data             : Pointer to Data
* @param size             : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return default brightness value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Def_Brightness(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size = 2;
    *data = g_def_brightness;

    return USB_OK;
}

uint8_t g_cur_power_mode[1] = {0x00};
/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Power_Mode
*
* @brief The function sets current power mode value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
*Set current power mode value specified by the Host
*****************************************************************************/
uint8_t USB_Desc_Set_Cur_Power_Mode(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    
    

    g_cur_power_mode[0]=**data;

    return USB_OK;
}
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Power_Mode
*
* @brief The function gets the current power mode value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return current power mode value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Cur_Power_Mode(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size = 1;
    *data = g_cur_power_mode;
    return USB_OK;
}

uint8_t g_info_power_mode[1] = {0x03};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Info_Power_Mode
*
* @brief The function gets the information power mode value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return information brightness value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Info_Power_Mode(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size = 1;
    *data = g_info_power_mode;
    return USB_OK;
}

//uint8_t g_cur_error_code[1] = {0x00};
uint8_t g_cur_error_code[1] = {0x07};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Error_Code
*
* @brief The function gets the current error code value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return current error code value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Cur_Error_Code(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size = 1;
    *data = g_cur_error_code;
    return USB_OK;
}

uint8_t g_info_error_code[1] = {0x01};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Info_Error_Code
*
* @brief The function gets the information error code value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return information error code value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Info_Error_Code(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size = 1;
    *data = g_info_error_code;
    return USB_OK;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Video_Probe
*
* @brief The function sets current probe value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
*Set current probe value specified by the Host
*****************************************************************************/
uint8_t USB_Desc_Set_Cur_Video_Probe(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    video_probe_and_commit_controls_struct_t* probe_handle;
    
    probe_handle = (video_probe_and_commit_controls_struct_t*)*data;

    if ((10000000/probe_handle->dwFrameInterval) <= 30)
    {
        virtual_camera.probe_struct.dwFrameInterval = probe_handle->dwFrameInterval;
    }
    
    if ((probe_handle->dwMaxPayloadTransferSize) && (probe_handle->dwMaxPayloadTransferSize <= VIDEO_ISO_ENDPOINT_PACKET_SIZE))
    {
        virtual_camera.probe_struct.dwMaxPayloadTransferSize = probe_handle->dwMaxPayloadTransferSize;
    }

    virtual_camera.probe_struct.bFormatIndex = probe_handle->bFormatIndex;
    virtual_camera.probe_struct.bFrameIndex = probe_handle->bFrameIndex;
    
    return USB_OK;
}
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Video_Probe
*
* @brief The function gets the current probe value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return current probe value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Cur_Video_Probe(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size=virtual_camera.probe_length;
    *data=(uint8_t*)&virtual_camera.probe_struct;

    return USB_OK;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Len_Video_Probe
*
* @brief The function gets the length probe value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return lengh probe value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Len_Video_Probe(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    *size=sizeof(virtual_camera.probe_length);
    *data=(uint8_t*)&virtual_camera.probe_length;

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Info_Video_Probe
 *
 * @brief The function gets the information probe value
 *
 * @param controller_ID : Controller ID
 * @param data		    : Pointer to Data
 * @param size			: Pointer to Size of Data
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 * Return information probe value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Info_Video_Probe(
    uint32_t handle,               /* [IN] class handle */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size=sizeof(virtual_camera.probe_info);
    *data=(uint8_t*)&virtual_camera.probe_info;

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Video_Commit
 *
 * @brief The function sets current commit value
 *
 * @param controller_ID : Controller ID
 * @param data		    : Pointer to data
 * @param size			: Pointer to Size of Data
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 *Set current commit value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Video_Commit(
    uint32_t handle,               /* [IN] class handle */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    video_probe_and_commit_controls_struct_t* commit_handle;
    
    commit_handle = (video_probe_and_commit_controls_struct_t*)*data;

    if ((10000000/commit_handle->dwFrameInterval) <= 30)
    {
        virtual_camera.commit_struct.dwFrameInterval = commit_handle->dwFrameInterval;
    }
    
    if ((commit_handle->dwMaxPayloadTransferSize) && (commit_handle->dwMaxPayloadTransferSize <= VIDEO_ISO_ENDPOINT_PACKET_SIZE))
    {
        virtual_camera.commit_struct.dwMaxPayloadTransferSize = commit_handle->dwMaxPayloadTransferSize;
    }
    
    virtual_camera.commit_struct.bFormatIndex = commit_handle->bFormatIndex;
    virtual_camera.commit_struct.bFrameIndex = commit_handle->bFrameIndex;
    
    virtual_camera.image_position[virtual_camera.commit_struct.bFormatIndex-1] = 0;
    virtual_camera.still_image_transmit = 0;
    virtual_camera.transmit_type = 0;
    
    return USB_OK;
}
/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Video_Commit
 *
 * @brief The function gets the current commit value
 *
 * @param controller_ID : Controller ID
 * @param data		    : Pointer to Data
 * @param size			: Pointer to Size of Data
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 * Return current commit value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Video_Commit(
    uint32_t handle,               /* [IN] class handle */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
  
    *size=virtual_camera.commit_length;
    *data=(uint8_t*)&virtual_camera.commit_struct;

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Len_Video_Commit
 *
 * @brief The function gets the length Commit value
 *
 * @param controller_ID : Controller ID
 * @param data		    : Pointer to Data
 * @param size			: Pointer to Size of Data
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 * Return length commit value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Len_Video_Commit(
    uint32_t handle,               /* [IN] class handle */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size=sizeof(virtual_camera.commit_length);
    *data=(uint8_t*)&virtual_camera.commit_length;;

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Info_Video_Commit
 *
 * @brief The function gets the information commit value
 *
 * @param controller_ID : Controller ID
 * @param data		    : Pointer to Data
 * @param size			: Pointer to Size of Data
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 * Return information commit value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Info_Video_Commit(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    
    *size=sizeof(virtual_camera.commit_info);
    *data=(uint8_t*)&virtual_camera.commit_info;

    return USB_OK;
}


/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Video_Still_Probe
*
* @brief The function sets current probe value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
*Set current probe value specified by the Host
*****************************************************************************/
uint8_t USB_Desc_Set_Cur_Video_Still_Probe(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    video_still_probe_and_commit_controls_struct_t* probe_handle;
    
    probe_handle = (video_still_probe_and_commit_controls_struct_t*)*data;
    
    if ((probe_handle->dwMaxPayloadTransferSize) && (probe_handle->dwMaxPayloadTransferSize <= VIDEO_ISO_ENDPOINT_PACKET_SIZE))
    {
        virtual_camera.still_probe_struct.dwMaxPayloadTransferSize = probe_handle->dwMaxPayloadTransferSize;
    }

    virtual_camera.still_probe_struct.bFormatIndex = probe_handle->bFormatIndex;
    virtual_camera.still_probe_struct.bFrameIndex = probe_handle->bFrameIndex;
    
    return USB_OK;
}
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Video_Still_Probe
*
* @brief The function gets the current probe value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return current probe value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Cur_Video_Still_Probe(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size=virtual_camera.still_probe_length;
    *data=(uint8_t*)&virtual_camera.still_probe_struct;

    return USB_OK;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Len_Video_Still_Probe
*
* @brief The function gets the length probe value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return lengh probe value to the Host
*****************************************************************************/
uint8_t USB_Desc_Get_Len_Video_Still_Probe(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    *size=sizeof(virtual_camera.still_probe_length);
    *data=(uint8_t*)&virtual_camera.still_probe_length;

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Info_Video_Still_Probe
 *
 * @brief The function gets the information probe value
 *
 * @param controller_ID : Controller ID
 * @param data		    : Pointer to Data
 * @param size			: Pointer to Size of Data
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 * Return information probe value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Info_Video_Still_Probe(
    uint32_t handle,               /* [IN] class handle */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size=sizeof(virtual_camera.still_probe_info);
    *data=(uint8_t*)&virtual_camera.still_probe_info;

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Video_Still_Commit
 *
 * @brief The function sets current commit value
 *
 * @param controller_ID : Controller ID
 * @param data		    : Pointer to data
 * @param size			: Pointer to Size of Data
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 *Set current commit value specified by the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Video_Still_Commit(
    uint32_t handle,               /* [IN] class handle */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    video_still_probe_and_commit_controls_struct_t* commit_handle;
    
    commit_handle = (video_still_probe_and_commit_controls_struct_t*)*data;
    
    if ((commit_handle->dwMaxPayloadTransferSize) && (commit_handle->dwMaxPayloadTransferSize <= VIDEO_ISO_ENDPOINT_PACKET_SIZE))
    {
        virtual_camera.still_commit_struct.dwMaxPayloadTransferSize = commit_handle->dwMaxPayloadTransferSize;
    }
    
    virtual_camera.still_commit_struct.bFormatIndex = commit_handle->bFormatIndex;
    virtual_camera.still_commit_struct.bFrameIndex = commit_handle->bFrameIndex;
    
    return USB_OK;
}
/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Video_Still_Commit
 *
 * @brief The function gets the current commit value
 *
 * @param controller_ID : Controller ID
 * @param data		    : Pointer to Data
 * @param size			: Pointer to Size of Data
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 * Return current commit value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Video_Still_Commit(
    uint32_t handle,               /* [IN] class handle */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
  
    *size=virtual_camera.still_commit_length;
    *data=(uint8_t*)&virtual_camera.still_commit_struct;

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Len_Video_Still_Commit
 *
 * @brief The function gets the length Commit value
 *
 * @param controller_ID : Controller ID
 * @param data		    : Pointer to Data
 * @param size			: Pointer to Size of Data
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 * Return length commit value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Len_Video_Still_Commit(
    uint32_t handle,               /* [IN] class handle */
    uint8_t* *data,                   /* [OUT] Pointer to Data */
    uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    

    *size=sizeof(virtual_camera.still_commit_length);
    *data=(uint8_t*)&virtual_camera.still_commit_length;;

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Info_Video_Still_Commit
 *
 * @brief The function gets the information commit value
 *
 * @param controller_ID : Controller ID
 * @param data		    : Pointer to Data
 * @param size			: Pointer to Size of Data
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 * Return information commit value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Info_Video_Still_Commit(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    
    *size=sizeof(virtual_camera.still_commit_info);
    *data=(uint8_t*)&virtual_camera.still_commit_info;

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Video_Still_Image_Trigger_Control
 *
 * @brief The function gets the still image trigger control
 *
 * @param controller_ID : Controller ID
 * @param data		    : Pointer to Data
 * @param size			: Pointer to Size of Data
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 * Return information commit value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Get_Cur_Video_Still_Image_Trigger_Control(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    
    *size=sizeof(virtual_camera.transmit_type);
    *data=(uint8_t*)&virtual_camera.transmit_type;

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Video_Still_Image_Trigger_Control
 *
 * @brief The function sets the still image trigger control
 *
 * @param controller_ID : Controller ID
 * @param data		    : Pointer to Data
 * @param size			: Pointer to Size of Data
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 * Return information commit value to the Host
 *****************************************************************************/
uint8_t USB_Desc_Set_Cur_Video_Still_Image_Trigger_Control(
uint32_t handle,               /* [IN] class handle */
uint8_t* *data,                   /* [OUT] Pointer to Data */
uint32_t *size               /* [OUT] Pointer to Size of Data */
)
{
    uint8_t        control = (uint8_t)(*data)[0];
    if (control < 4)
    {
        virtual_camera.transmit_type = control;
    }

    if (virtual_camera.transmit_type == 3)
    {
        virtual_camera.still_image_transmit = 0;
    }

    return USB_OK;
}


/**************************************************************************//*!
 *
 * @name  USB_Class_Get_feature
 *
 * @brief  .
 *
 * @param handle:
 *
 * @param cmd:
 * @param in_data:
 * @param out_buf
 * @return status:
 *                        USB_OK : When Successfull
 *                        Others : When Error
 *
 *****************************************************************************/
uint8_t USB_Class_Get_feature
(
   uint32_t handle,
   int32_t cmd,
   uint8_t * * out_buf,
   uint32_t * size
)
{
    uint8_t error = USBERR_INVALID_REQ_TYPE;

    switch (cmd)
    {
    /* GET CUR COMMAND */
    
    case GET_CUR_VC_POWER_MODE_CONTROL:
        error = USB_Desc_Get_Cur_Power_Mode(handle, out_buf, size);
        break;
    case GET_CUR_VC_ERROR_CODE_CONTROL:
        error = USB_Desc_Get_Cur_Error_Code(handle, out_buf, size);
        break;

    case GET_CUR_PU_BACKLIGHT_COMPENSATION_CONTROL:
        break;
    case GET_CUR_PU_BRIGHTNESS_CONTROL:
        error = USB_Desc_Get_Cur_Brightness(handle, out_buf, size);
        break;
    case GET_CUR_PU_CONTRACT_CONTROL:
        break;
    case GET_CUR_PU_GAIN_CONTROL:
        break;
    case GET_CUR_PU_POWER_LINE_FREQUENCY_CONTROL:
        break;
    case GET_CUR_PU_HUE_CONTROL:
        break;
    case GET_CUR_PU_SATURATION_CONTROL:
        break;
    case GET_CUR_PU_SHARRNESS_CONTROL:
        break;
    case GET_CUR_PU_GAMMA_CONTROL:
        break;
    case GET_CUR_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        break;
    case GET_CUR_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
        break;
    case GET_CUR_PU_WHITE_BALANCE_COMPONENT_CONTROL:
        break;
    case GET_CUR_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
        break;
    case GET_CUR_PU_DIGITAL_MULTIPLIER_CONTROL:
        break;
    case GET_CUR_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        break;
    case GET_CUR_PU_HUE_AUTO_CONTROL:
        break;
    case GET_CUR_PU_ANALOG_VIDEO_STANDARD_CONTROL:
        break;
    case GET_CUR_PU_ANALOG_LOCK_STATUS_CONTROL:
        break;
#if USBCFG_VIDEO_CLASS_1_5
    case GET_CUR_PU_CONTRAST_AUTO_CONTROL:
        break;
#endif

    case GET_CUR_CT_SCANNING_MODE_CONTROL:
        break;
    case GET_CUR_CT_AE_MODE_CONTROL:
        break;
    case GET_CUR_CT_AE_PRIORITY_CONTROL:
        break;
    case GET_CUR_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        break;
    case GET_CUR_CT_EXPOSURE_TIME_RELATIVE_CONTROL:
        break;
    case GET_CUR_CT_FOCUS_ABSOLUTE_CONTROL:
        break;
    case GET_CUR_CT_FOCUS_RELATIVE_CONTROL:
        break;
    case GET_CUR_CT_FOCUS_AUTO_CONTROL:
        break;
    case GET_CUR_CT_IRIS_ABSOLUTE_CONTROL:
        break;
    case GET_CUR_CT_IRIS_RELATIVE_CONTROL:
        break;
    case GET_CUR_CT_ZOOM_ABSOLUTE_CONTROL:
        break;
    case GET_CUR_CT_ZOOM_RELATIVE_CONTROL:
        break;
    case GET_CUR_CT_PANTILT_ABSOLUTE_CONTROL:
        break;
    case GET_CUR_CT_PANTILT_RELATIVE_CONTROL:
        break;
    case GET_CUR_CT_ROLL_ABSOLUTE_CONTROL:
        break;
    case GET_CUR_CT_ROLL_RELATIVE_CONTROL:
        break;
    case GET_CUR_CT_PRIVACY_CONTROL:
        break;
#if USBCFG_VIDEO_CLASS_1_5
    case GET_CUR_CT_FOCUS_SIMPLE_CONTROL:
        break;
    case GET_CUR_CT_DIGITAL_WINDOW_CONTROL:
        break;
    case GET_CUR_CT_REGION_OF_INTEREST_CONTROL:
        break;
#endif

    case GET_CUR_VS_PROBE_CONTROL:
        error = USB_Desc_Get_Cur_Video_Probe(handle, out_buf, size);
        break;
    case GET_CUR_VS_COMMIT_CONTROL:
        error = USB_Desc_Get_Cur_Video_Commit(handle, out_buf, size);
        break;
    case GET_CUR_VS_STILL_PROBE_CONTROL:
        error = USB_Desc_Get_Cur_Video_Still_Probe(handle, out_buf, size);
        break;
    case GET_CUR_VS_STILL_COMMIT_CONTROL:
        error = USB_Desc_Get_Cur_Video_Still_Commit(handle, out_buf, size);
        break;
    case GET_CUR_VS_STILL_IMAGE_TRIGGER_CONTROL:
        error = USB_Desc_Get_Cur_Video_Still_Image_Trigger_Control(handle, out_buf, size);
        break;
    case GET_CUR_VS_STREAM_ERROR_CODE_CONTROL:
        break;
    case GET_CUR_VS_GENERATE_KEY_FRAME_CONTROL:
        break;
    case GET_CUR_VS_UPDATE_FRAME_SEGMENT_CONTROL:
        break;
    case GET_CUR_VS_SYNCH_DELAY_CONTROL:
        break;

#if USBCFG_VIDEO_CLASS_1_5
    case GET_CUR_EU_SELECT_LAYER_CONTROL:
        break;
    case GET_CUR_EU_PROFILE_TOOLSET_CONTROL:
        break;
    case GET_CUR_EU_VIDEO_RESOLUTION_CONTROL:
        break;
    case GET_CUR_EU_MIN_FRAME_INTERVAL_CONTROL:
        break;
    case GET_CUR_EU_SLICE_MODE_CONTROL:
        break;
    case GET_CUR_EU_RATE_CONTROL_MODE_CONTROL:
        break;
    case GET_CUR_EU_AVERAGE_BITRATE_CONTROL:
        break;
    case GET_CUR_EU_CPB_SIZE_CONTROL:
        break;
    case GET_CUR_EU_PEAK_BIT_RATE_CONTROL:
        break;
    case GET_CUR_EU_QUANTIZATION_PARAMS_CONTROL:
        break;
    case GET_CUR_EU_SYNC_REF_FRAME_CONTROL:
        break;
    case GET_CUR_EU_LTR_BUFFER_CONTROL:
        break;
    case GET_CUR_EU_LTR_PICTURE_CONTROL:
        break;
    case GET_CUR_EU_LTR_VALIDATION_CONTROL:
        break;
    case GET_CUR_EU_LEVEL_IDC_LIMIT_CONTROL:
        break;
    case GET_CUR_EU_SEI_PAYLOADTYPE_CONTROL:
        break;
    case GET_CUR_EU_QP_RANGE_CONTROL:
        break;
    case GET_CUR_EU_PRIORITY_CONTROL:
        break;
    case GET_CUR_EU_START_OR_STOP_LAYER_CONTROL:
        break;
    case GET_CUR_EU_ERROR_RESILIENCY_CONTROL:
        break;
#endif

    /* GET MIN COMMAND */
          
    case GET_MIN_PU_BACKLIGHT_COMPENSATION_CONTROL:
        break;
    case GET_MIN_PU_BRIGHTNESS_CONTROL:
        error = USB_Desc_Get_Min_Brightness(handle, out_buf, size);
        break;
    case GET_MIN_PU_CONTRACT_CONTROL:
        break;
    case GET_MIN_PU_GAIN_CONTROL:
        break;
    case GET_MIN_PU_HUE_CONTROL:
        break;
    case GET_MIN_PU_SATURATION_CONTROL:
        break;
    case GET_MIN_PU_SHARRNESS_CONTROL:
        break;
    case GET_MIN_PU_GAMMA_CONTROL:
        break;
    case GET_MIN_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        break;
    case GET_MIN_PU_WHITE_BALANCE_COMPONENT_CONTROL:
        break;
    case GET_MIN_PU_DIGITAL_MULTIPLIER_CONTROL:
        break;
    case GET_MIN_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        break;

    case GET_MIN_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        break;
    case GET_MIN_CT_FOCUS_ABSOLUTE_CONTROL:
        break;
    case GET_MIN_CT_FOCUS_RELATIVE_CONTROL:
        break;
    case GET_MIN_CT_IRIS_ABSOLUTE_CONTROL:
        break;
    case GET_MIN_CT_ZOOM_ABSOLUTE_CONTROL:
        break;
    case GET_MIN_CT_ZOOM_RELATIVE_CONTROL:
        break;
    case GET_MIN_CT_PANTILT_ABSOLUTE_CONTROL:
        break;
    case GET_MIN_CT_PANTILT_RELATIVE_CONTROL:
        break;
    case GET_MIN_CT_ROLL_ABSOLUTE_CONTROL:
        break;
    case GET_MIN_CT_ROLL_RELATIVE_CONTROL:
        break;
#if USBCFG_VIDEO_CLASS_1_5
    case GET_MIN_CT_DIGITAL_WINDOW_CONTROL:
        break;
    case GET_MIN_CT_REGION_OF_INTEREST_CONTROL:
        break;
#endif

    case GET_MIN_VS_PROBE_CONTROL:
        break;
    case GET_MIN_VS_STILL_PROBE_CONTROL:
        break;
    case GET_MIN_VS_UPDATE_FRAME_SEGMENT_CONTROL:
        break;
    case GET_MIN_VS_SYNCH_DELAY_CONTROL:
        break;

#if USBCFG_VIDEO_CLASS_1_5
    case GET_MIN_EU_VIDEO_RESOLUTION_CONTROL:
        break;
    case GET_MIN_EU_MIN_FRAME_INTERVAL_CONTROL:
        break;
    case GET_MIN_EU_SLICE_MODE_CONTROL:
        break;
    case GET_MIN_EU_AVERAGE_BITRATE_CONTROL:
        break;
    case GET_MIN_EU_CPB_SIZE_CONTROL:
        break;
    case GET_MIN_EU_PEAK_BIT_RATE_CONTROL:
        break;
    case GET_MIN_EU_QUANTIZATION_PARAMS_CONTROL:
        break;
    case GET_MIN_EU_SYNC_REF_FRAME_CONTROL:
        break;
    case GET_MIN_EU_LEVEL_IDC_LIMIT_CONTROL:
        break;
    case GET_MIN_EU_SEI_PAYLOADTYPE_CONTROL:
        break;
    case GET_MIN_EU_QP_RANGE_CONTROL:
        break;
#endif

    /* GET MAX COMMAND */
          
    case GET_MAX_PU_BACKLIGHT_COMPENSATION_CONTROL:
        break;
    case GET_MAX_PU_BRIGHTNESS_CONTROL:
        error = USB_Desc_Get_Max_Brightness(handle, out_buf, size);
        break;
    case GET_MAX_PU_CONTRACT_CONTROL:
        break;
    case GET_MAX_PU_GAIN_CONTROL:
        break;
    case GET_MAX_PU_HUE_CONTROL:
        break;
    case GET_MAX_PU_SATURATION_CONTROL:
        break;
    case GET_MAX_PU_SHARRNESS_CONTROL:
        break;
    case GET_MAX_PU_GAMMA_CONTROL:
        break;
    case GET_MAX_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        break;
    case GET_MAX_PU_WHITE_BALANCE_COMPONENT_CONTROL:
        break;
    case GET_MAX_PU_DIGITAL_MULTIPLIER_CONTROL:
        break;
    case GET_MAX_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        break;

    case GET_MAX_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        break;
    case GET_MAX_CT_FOCUS_ABSOLUTE_CONTROL:
        break;
    case GET_MAX_CT_FOCUS_RELATIVE_CONTROL:
        break;
    case GET_MAX_CT_IRIS_ABSOLUTE_CONTROL:
        break;
    case GET_MAX_CT_ZOOM_ABSOLUTE_CONTROL:
        break;
    case GET_MAX_CT_ZOOM_RELATIVE_CONTROL:
        break;
    case GET_MAX_CT_PANTILT_ABSOLUTE_CONTROL:
        break;
    case GET_MAX_CT_PANTILT_RELATIVE_CONTROL:
        break;
    case GET_MAX_CT_ROLL_ABSOLUTE_CONTROL:
        break;
    case GET_MAX_CT_ROLL_RELATIVE_CONTROL:
        break;
#if USBCFG_VIDEO_CLASS_1_5
    case GET_MAX_CT_DIGITAL_WINDOW_CONTROL:
        break;
    case GET_MAX_CT_REGION_OF_INTEREST_CONTROL:
        break;
#endif

    case GET_MAX_VS_PROBE_CONTROL:
        break;
    case GET_MAX_VS_STILL_PROBE_CONTROL:
        break;
    case GET_MAX_VS_UPDATE_FRAME_SEGMENT_CONTROL:
        break;
    case GET_MAX_VS_SYNCH_DELAY_CONTROL:
        break;

#if USBCFG_VIDEO_CLASS_1_5
    case GET_MAX_EU_VIDEO_RESOLUTION_CONTROL:
        break;
    case GET_MAX_EU_MIN_FRAME_INTERVAL_CONTROL:
        break;
    case GET_MAX_EU_SLICE_MODE_CONTROL:
        break;
    case GET_MAX_EU_AVERAGE_BITRATE_CONTROL:
        break;
    case GET_MAX_EU_CPB_SIZE_CONTROL:
        break;
    case GET_MAX_EU_PEAK_BIT_RATE_CONTROL:
        break;
    case GET_MAX_EU_QUANTIZATION_PARAMS_CONTROL:
        break;
    case GET_MAX_EU_SYNC_REF_FRAME_CONTROL:
        break;
    case GET_MAX_EU_LTR_BUFFER_CONTROL:
        break;
    case GET_MAX_EU_LEVEL_IDC_LIMIT_CONTROL:
        break;
    case GET_MAX_EU_SEI_PAYLOADTYPE_CONTROL:
        break;
    case GET_MAX_EU_QP_RANGE_CONTROL:
        break;
#endif

    /* GET RES COMMAND */
          
    case GET_RES_PU_BACKLIGHT_COMPENSATION_CONTROL:
        break;
    case GET_RES_PU_BRIGHTNESS_CONTROL:
        error = USB_Desc_Get_Res_Brightness(handle, out_buf, size);
        break;
    case GET_RES_PU_CONTRACT_CONTROL:
        break;
    case GET_RES_PU_GAIN_CONTROL:
        break;
    case GET_RES_PU_HUE_CONTROL:
        break;
    case GET_RES_PU_SATURATION_CONTROL:
        break;
    case GET_RES_PU_SHARRNESS_CONTROL:
        break;
    case GET_RES_PU_GAMMA_CONTROL:
        break;
    case GET_RES_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        break;
    case GET_RES_PU_WHITE_BALANCE_COMPONENT_CONTROL:
        break;
    case GET_RES_PU_DIGITAL_MULTIPLIER_CONTROL:
        break;
    case GET_RES_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        break;

    case GET_RES_CT_AE_MODE_CONTROL:
        break;
    case GET_RES_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        break;
    case GET_RES_CT_FOCUS_ABSOLUTE_CONTROL:
        break;
    case GET_RES_CT_FOCUS_RELATIVE_CONTROL:
        break;
    case GET_RES_CT_IRIS_ABSOLUTE_CONTROL:
        break;
    case GET_RES_CT_ZOOM_ABSOLUTE_CONTROL:
        break;
    case GET_RES_CT_ZOOM_RELATIVE_CONTROL:
        break;
    case GET_RES_CT_PANTILT_ABSOLUTE_CONTROL:
        break;
    case GET_RES_CT_PANTILT_RELATIVE_CONTROL:
        break;
    case GET_RES_CT_ROLL_ABSOLUTE_CONTROL:
        break;
    case GET_RES_CT_ROLL_RELATIVE_CONTROL:
        break;

    case GET_RES_VS_PROBE_CONTROL:
        break;
    case GET_RES_VS_STILL_PROBE_CONTROL:
        break;
    case GET_RES_VS_UPDATE_FRAME_SEGMENT_CONTROL:
        break;
    case GET_RES_VS_SYNCH_DELAY_CONTROL:
        break;

#if USBCFG_VIDEO_CLASS_1_5
    case GET_RES_EU_AVERAGE_BITRATE_CONTROL:
        break;
    case GET_RES_EU_CPB_SIZE_CONTROL:
        break;
    case GET_RES_EU_PEAK_BIT_RATE_CONTROL:
        break;
    case GET_RES_EU_QUANTIZATION_PARAMS_CONTROL:
        break;
    case GET_RES_EU_ERROR_RESILIENCY_CONTROL:
        break;
#endif

    /* GET LEN COMMAND */

    case GET_LEN_VS_PROBE_CONTROL:
        error = USB_Desc_Get_Len_Video_Probe(handle, out_buf, size);
        break;
    case GET_LEN_VS_COMMIT_CONTROL:
        error = USB_Desc_Get_Len_Video_Commit(handle, out_buf, size);
        break;
    case GET_LEN_VS_STILL_PROBE_CONTROL:
        error = USB_Desc_Get_Len_Video_Still_Probe(handle, out_buf, size);
        break;
    case GET_LEN_VS_STILL_COMMIT_CONTROL:
        error = USB_Desc_Get_Len_Video_Still_Commit(handle, out_buf, size);
        break;

#if USBCFG_VIDEO_CLASS_1_5
    case GET_LEN_EU_SELECT_LAYER_CONTROL:
        break;
    case GET_LEN_EU_PROFILE_TOOLSET_CONTROL:
        break;
    case GET_LEN_EU_VIDEO_RESOLUTION_CONTROL:
        break;
    case GET_LEN_EU_MIN_FRAME_INTERVAL_CONTROL:
        break;
    case GET_LEN_EU_SLICE_MODE_CONTROL:
        break;
    case GET_LEN_EU_RATE_CONTROL_MODE_CONTROL:
        break;
    case GET_LEN_EU_AVERAGE_BITRATE_CONTROL:
        break;
    case GET_LEN_EU_CPB_SIZE_CONTROL:
        break;
    case GET_LEN_EU_PEAK_BIT_RATE_CONTROL:
        break;
    case GET_LEN_EU_QUANTIZATION_PARAMS_CONTROL:
        break;
    case GET_LEN_EU_SYNC_REF_FRAME_CONTROL:
        break;
    case GET_LEN_EU_LTR_BUFFER_CONTROL:
        break;
    case GET_LEN_EU_LTR_PICTURE_CONTROL:
        break;
    case GET_LEN_EU_LTR_VALIDATION_CONTROL:
        break;
    case GET_LEN_EU_QP_RANGE_CONTROL:
        break;
    case GET_LEN_EU_PRIORITY_CONTROL:
        break;
    case GET_LEN_EU_START_OR_STOP_LAYER_CONTROL:
        break;
#endif

    /* GET INFO COMMAND */
    case GET_INFO_VC_POWER_MODE_CONTROL:
        error = USB_Desc_Get_Info_Power_Mode(handle, out_buf, size);
        break;
    case GET_INFO_VC_ERROR_CODE_CONTROL:
        error = USB_Desc_Get_Info_Error_Code(handle, out_buf, size);
        break;
          
    case GET_INFO_PU_BACKLIGHT_COMPENSATION_CONTROL:
        break;
    case GET_INFO_PU_BRIGHTNESS_CONTROL:
        error = USB_Desc_Get_Info_Brightness(handle, out_buf, size);
        break;
    case GET_INFO_PU_CONTRACT_CONTROL:
        break;
    case GET_INFO_PU_GAIN_CONTROL:
        break;
    case GET_INFO_PU_POWER_LINE_FREQUENCY_CONTROL:
        break;
    case GET_INFO_PU_HUE_CONTROL:
        break;
    case GET_INFO_PU_SATURATION_CONTROL:
        break;
    case GET_INFO_PU_SHARRNESS_CONTROL:
        break;
    case GET_INFO_PU_GAMMA_CONTROL:
        break;
    case GET_INFO_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        break;
    case GET_INFO_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
        break;
    case GET_INFO_PU_WHITE_BALANCE_COMPONENT_CONTROL:
        break;
    case GET_INFO_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
        break;
    case GET_INFO_PU_DIGITAL_MULTIPLIER_CONTROL:
        break;
    case GET_INFO_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        break;
    case GET_INFO_PU_HUE_AUTO_CONTROL:
        break;
    case GET_INFO_PU_ANALOG_VIDEO_STANDARD_CONTROL:
        break;
    case GET_INFO_PU_ANALOG_LOCK_STATUS_CONTROL:
        break;
#if USBCFG_VIDEO_CLASS_1_5
    case GET_INFO_PU_CONTRAST_AUTO_CONTROL:
        break;
#endif

    case GET_INFO_CT_SCANNING_MODE_CONTROL:
        break;
    case GET_INFO_CT_AE_MODE_CONTROL:
        break;
    case GET_INFO_CT_AE_PRIORITY_CONTROL:
        break;
    case GET_INFO_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        break;
    case GET_INFO_CT_EXPOSURE_TIME_RELATIVE_CONTROL:
        break;
    case GET_INFO_CT_FOCUS_ABSOLUTE_CONTROL:
        break;
    case GET_INFO_CT_FOCUS_RELATIVE_CONTROL:
        break;
    case GET_INFO_CT_FOCUS_AUTO_CONTROL:
        break;
    case GET_INFO_CT_IRIS_ABSOLUTE_CONTROL:
        break;
    case GET_INFO_CT_IRIS_RELATIVE_CONTROL:
        break;
    case GET_INFO_CT_ZOOM_ABSOLUTE_CONTROL:
        break;
    case GET_INFO_CT_ZOOM_RELATIVE_CONTROL:
        break;
    case GET_INFO_CT_PANTILT_ABSOLUTE_CONTROL:
        break;
    case GET_INFO_CT_PANTILT_RELATIVE_CONTROL:
        break;
    case GET_INFO_CT_ROLL_ABSOLUTE_CONTROL:
        break;
    case GET_INFO_CT_ROLL_RELATIVE_CONTROL:
        break;
    case GET_INFO_CT_PRIVACY_CONTROL:
        break;
#if USBCFG_VIDEO_CLASS_1_5
    case GET_INFO_CT_FOCUS_SIMPLE_CONTROL:
        break;
#endif

    case GET_INFO_VS_PROBE_CONTROL:
        error = USB_Desc_Get_Info_Video_Probe(handle, out_buf, size);
        break;
    case GET_INFO_VS_COMMIT_CONTROL:
        error = USB_Desc_Get_Info_Video_Commit(handle, out_buf, size);
        break;
    case GET_INFO_VS_STILL_PROBE_CONTROL:
        error = USB_Desc_Get_Info_Video_Still_Probe(handle, out_buf, size);
        break;
    case GET_INFO_VS_STILL_COMMIT_CONTROL:
        error = USB_Desc_Get_Info_Video_Still_Commit(handle, out_buf, size);
        break;
    case GET_INFO_VS_STILL_IMAGE_TRIGGER_CONTROL:
        break;
    case GET_INFO_VS_STREAM_ERROR_CODE_CONTROL:
        break;
    case GET_INFO_VS_GENERATE_KEY_FRAME_CONTROL:
        break;
    case GET_INFO_VS_UPDATE_FRAME_SEGMENT_CONTROL:
        break;
    case GET_INFO_VS_SYNCH_DELAY_CONTROL:
        break;

#if USBCFG_VIDEO_CLASS_1_5
    case GET_INFO_EU_SELECT_LAYER_CONTROL:
        break;
    case GET_INFO_EU_PROFILE_TOOLSET_CONTROL:
        break;
    case GET_INFO_EU_VIDEO_RESOLUTION_CONTROL:
        break;
    case GET_INFO_EU_MIN_FRAME_INTERVAL_CONTROL:
        break;
    case GET_INFO_EU_SLICE_MODE_CONTROL:
        break;
    case GET_INFO_EU_RATE_CONTROL_MODE_CONTROL:
        break;
    case GET_INFO_EU_AVERAGE_BITRATE_CONTROL:
        break;
    case GET_INFO_EU_CPB_SIZE_CONTROL:
        break;
    case GET_INFO_EU_PEAK_BIT_RATE_CONTROL:
        break;
    case GET_INFO_EU_QUANTIZATION_PARAMS_CONTROL:
        break;
    case GET_INFO_EU_SYNC_REF_FRAME_CONTROL:
        break;
    case GET_INFO_EU_LTR_BUFFER_CONTROL:
        break;
    case GET_INFO_EU_LTR_PICTURE_CONTROL:
        break;
    case GET_INFO_EU_LTR_VALIDATION_CONTROL:
        break;
    case GET_INFO_EU_SEI_PAYLOADTYPE_CONTROL:
        break;
    case GET_INFO_EU_QP_RANGE_CONTROL:
        break;
    case GET_INFO_EU_PRIORITY_CONTROL:
        break;
    case GET_INFO_EU_START_OR_STOP_LAYER_CONTROL:
        break;
#endif

    /* GET DEF COMMAND */
          
    case GET_DEF_PU_BACKLIGHT_COMPENSATION_CONTROL:
        break;
    case GET_DEF_PU_BRIGHTNESS_CONTROL:
        error = USB_Desc_Get_Def_Brightness(handle, out_buf, size);
        break;
    case GET_DEF_PU_CONTRACT_CONTROL:
        break;
    case GET_DEF_PU_GAIN_CONTROL:
        break;
    case GET_DEF_PU_POWER_LINE_FREQUENCY_CONTROL:
        break;
    case GET_DEF_PU_HUE_CONTROL:
        break;
    case GET_DEF_PU_SATURATION_CONTROL:
        break;
    case GET_DEF_PU_SHARRNESS_CONTROL:
        break;
    case GET_DEF_PU_GAMMA_CONTROL:
        break;
    case GET_DEF_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        break;
    case GET_DEF_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
        break;
    case GET_DEF_PU_WHITE_BALANCE_COMPONENT_CONTROL:
        break;
    case GET_DEF_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
        break;
    case GET_DEF_PU_DIGITAL_MULTIPLIER_CONTROL:
        break;
    case GET_DEF_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        break;
    case GET_DEF_PU_HUE_AUTO_CONTROL:
        break;
#if USBCFG_VIDEO_CLASS_1_5
    case GET_DEF_PU_CONTRAST_AUTO_CONTROL:
        break;
#endif

    case GET_DEF_CT_AE_MODE_CONTROL:
        break;
    case GET_DEF_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        break;
    case GET_DEF_CT_FOCUS_ABSOLUTE_CONTROL:
        break;
    case GET_DEF_CT_FOCUS_RELATIVE_CONTROL:
        break;
    case GET_DEF_CT_FOCUS_AUTO_CONTROL:
        break;
    case GET_DEF_CT_IRIS_ABSOLUTE_CONTROL:
        break;
    case GET_DEF_CT_ZOOM_ABSOLUTE_CONTROL:
        break;
    case GET_DEF_CT_ZOOM_RELATIVE_CONTROL:
        break;
    case GET_DEF_CT_PANTILT_ABSOLUTE_CONTROL:
        break;
    case GET_DEF_CT_PANTILT_RELATIVE_CONTROL:
        break;
    case GET_DEF_CT_ROLL_ABSOLUTE_CONTROL:
        break;
    case GET_DEF_CT_ROLL_RELATIVE_CONTROL:
        break;
#if USBCFG_VIDEO_CLASS_1_5
    case GET_DEF_CT_FOCUS_SIMPLE_CONTROL:
        break;
    case GET_DEF_CT_DIGITAL_WINDOW_CONTROL:
        break;
    case GET_DEF_CT_REGION_OF_INTEREST_CONTROL:
        break;
#endif

    case GET_DEF_VS_PROBE_CONTROL:
        break;
    case GET_DEF_VS_STILL_PROBE_CONTROL:
        break;
    case GET_DEF_VS_UPDATE_FRAME_SEGMENT_CONTROL:
        break;
    case GET_DEF_VS_SYNCH_DELAY_CONTROL:
        break;

#if USBCFG_VIDEO_CLASS_1_5
    case GET_DEF_EU_PROFILE_TOOLSET_CONTROL:
        break;
    case GET_DEF_EU_VIDEO_RESOLUTION_CONTROL:
        break;
    case GET_DEF_EU_MIN_FRAME_INTERVAL_CONTROL:
        break;
    case GET_DEF_EU_SLICE_MODE_CONTROL:
        break;
    case GET_DEF_EU_RATE_CONTROL_MODE_CONTROL:
        break;
    case GET_DEF_EU_AVERAGE_BITRATE_CONTROL:
        break;
    case GET_DEF_EU_CPB_SIZE_CONTROL:
        break;
    case GET_DEF_EU_PEAK_BIT_RATE_CONTROL:
        break;
    case GET_DEF_EU_QUANTIZATION_PARAMS_CONTROL:
        break;
    case GET_DEF_EU_LTR_BUFFER_CONTROL:
        break;
    case GET_DEF_EU_LTR_PICTURE_CONTROL:
        break;
    case GET_DEF_EU_LTR_VALIDATION_CONTROL:
        break;
    case GET_DEF_EU_LEVEL_IDC_LIMIT_CONTROL:
        break;
    case GET_DEF_EU_SEI_PAYLOADTYPE_CONTROL:
        break;
    case GET_DEF_EU_QP_RANGE_CONTROL:
        break;
    case GET_DEF_EU_ERROR_RESILIENCY_CONTROL:
        break;
#endif

    default:
        break;
    }
    return error;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_feature
 *
 * @brief  .
 *
 * @param handle:           
 *                                
 * @param cmd:              
 * @param in_data:          
 * @param in_buf           
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
uint8_t USB_Class_Set_feature
(
   uint32_t handle,
   int32_t cmd,
   uint8_t * * in_buf,
   uint32_t * size
)
{
    uint8_t error = USBERR_INVALID_REQ_TYPE;

    switch (cmd)
    {
    /* Set CUR */
    case SET_CUR_VC_POWER_MODE_CONTROL:
        error = USB_Desc_Set_Cur_Power_Mode(handle, in_buf, size);
        break;
    /* Set CUR PU */
    case SET_CUR_PU_BACKLIGHT_COMPENSATION_CONTROL:
        break;
    case SET_CUR_PU_BRIGHTNESS_CONTROL:
        error = USB_Desc_Set_Cur_Brightness(handle, in_buf, size);
        break;
    case SET_CUR_PU_CONTRACT_CONTROL:
        break;
    case SET_CUR_PU_GAIN_CONTROL:
        break;
    case SET_CUR_PU_POWER_LINE_FREQUENCY_CONTROL:
        break;
    case SET_CUR_PU_HUE_CONTROL:
        break;
    case SET_CUR_PU_SATURATION_CONTROL:
        break;
    case SET_CUR_PU_SHARRNESS_CONTROL:
        break;
    case SET_CUR_PU_GAMMA_CONTROL:
        break;
    case SET_CUR_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
        break;
    case SET_CUR_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
        break;
    case SET_CUR_PU_WHITE_BALANCE_COMPONENT_CONTROL:
        break;
    case SET_CUR_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL:
        break;
    case SET_CUR_PU_DIGITAL_MULTIPLIER_CONTROL:
        break;
    case SET_CUR_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL:
        break;
    case SET_CUR_PU_HUE_AUTO_CONTROL:
        break;
#if USBCFG_VIDEO_CLASS_1_5
    case SET_CUR_PU_CONTRAST_AUTO_CONTROL:
        break;
#endif

    /* Set CUR Camera Terminal */
    case SET_CUR_CT_SCANNING_MODE_CONTROL:
        break;
    case SET_CUR_CT_AE_MODE_CONTROL:
        break;
    case SET_CUR_CT_AE_PRIORITY_CONTROL:
        break;
    case SET_CUR_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
        break;
    case SET_CUR_CT_EXPOSURE_TIME_RELATIVE_CONTROL:
        break;
    case SET_CUR_CT_FOCUS_ABSOLUTE_CONTROL:
        break;
    case SET_CUR_CT_FOCUS_RELATIVE_CONTROL:
        break;
    case SET_CUR_CT_FOCUS_AUTO_CONTROL:
        break;
    case SET_CUR_CT_IRIS_ABSOLUTE_CONTROL:
        break;
    case SET_CUR_CT_IRIS_RELATIVE_CONTROL:
        break;
    case SET_CUR_CT_ZOOM_ABSOLUTE_CONTROL:
        break;
    case SET_CUR_CT_ZOOM_RELATIVE_CONTROL:
        break;
    case SET_CUR_CT_PANTILT_ABSOLUTE_CONTROL:
        break;
    case SET_CUR_CT_PANTILT_RELATIVE_CONTROL:
        break;
    case SET_CUR_CT_ROLL_ABSOLUTE_CONTROL:
        break;
    case SET_CUR_CT_ROLL_RELATIVE_CONTROL:
        break;
    case SET_CUR_CT_PRIVACY_CONTROL:
        break;
#if USBCFG_VIDEO_CLASS_1_5
    case SET_CUR_CT_FOCUS_SIMPLE_CONTROL:
        break;
    case SET_CUR_CT_DIGITAL_WINDOW_CONTROL:
        break;
    case SET_CUR_CT_REGION_OF_INTEREST_CONTROL:
        break;
#endif

    /* Set CUR VS */
    case SET_CUR_VS_PROBE_CONTROL:
        error = USB_Desc_Set_Cur_Video_Probe(handle, in_buf, size);
        break;
    case SET_CUR_VS_COMMIT_CONTROL:
        error = USB_Desc_Set_Cur_Video_Commit(handle, in_buf, size);
        break;
    case SET_CUR_VS_STILL_PROBE_CONTROL:
        error = USB_Desc_Set_Cur_Video_Still_Probe(handle, in_buf, size);
        break;
    case SET_CUR_VS_STILL_COMMIT_CONTROL:
        error = USB_Desc_Set_Cur_Video_Still_Commit(handle, in_buf, size);
        break;
    case SET_CUR_VS_STILL_IMAGE_TRIGGER_CONTROL:
        error = USB_Desc_Set_Cur_Video_Still_Image_Trigger_Control(handle, in_buf, size);
        break;
    case SET_CUR_VS_STREAM_ERROR_CODE_CONTROL:
        break;
    case SET_CUR_VS_GENERATE_KEY_FRAME_CONTROL:
        break;
    case SET_CUR_VS_UPDATE_FRAME_SEGMENT_CONTROL:
        break;
    case SET_CUR_VS_SYNCH_DELAY_CONTROL:
        break;

#if USBCFG_VIDEO_CLASS_1_5
    /* Set CUR EU */
    case SET_CUR_EU_SELECT_LAYER_CONTROL:
        break;
    case SET_CUR_EU_PROFILE_TOOLSET_CONTROL:
        break;
    case SET_CUR_EU_VIDEO_RESOLUTION_CONTROL:
        break;
    case SET_CUR_EU_MIN_FRAME_INTERVAL_CONTROL:
        break;
    case SET_CUR_EU_SLICE_MODE_CONTROL:
        break;
    case SET_CUR_EU_RATE_CONTROL_MODE_CONTROL:
        break;
    case SET_CUR_EU_AVERAGE_BITRATE_CONTROL:
        break;
    case SET_CUR_EU_CPB_SIZE_CONTROL:
        break;
    case SET_CUR_EU_PEAK_BIT_RATE_CONTROL:
        break;
    case SET_CUR_EU_QUANTIZATION_PARAMS_CONTROL:
        break;
    case SET_CUR_EU_SYNC_REF_FRAME_CONTROL:
        break;
    case SET_CUR_EU_LTR_BUFFER_CONTROL:
        break;
    case SET_CUR_EU_LTR_PICTURE_CONTROL:
        break;
    case SET_CUR_EU_LTR_VALIDATION_CONTROL:
        break;
    case SET_CUR_EU_LEVEL_IDC_LIMIT_CONTROL:
        break;
    case SET_CUR_EU_SEI_PAYLOADTYPE_CONTROL:
        break;
    case SET_CUR_EU_QP_RANGE_CONTROL:
        break;
    case SET_CUR_EU_PRIORITY_CONTROL:
        break;
    case SET_CUR_EU_START_OR_STOP_LAYER_CONTROL:
        break;
    case SET_CUR_EU_ERROR_RESILIENCY_CONTROL:
        break;
#endif

    default:
        break;
   }
   return error;
}
/* EOF */
