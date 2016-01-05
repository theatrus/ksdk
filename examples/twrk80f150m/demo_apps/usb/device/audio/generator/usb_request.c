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
#include "usb_request.h"

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/

uint8_t g_copy_protect = 0x01;
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Copy_Protect
 *
 * @brief  This function is called in response to Set Terminal Control Request
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Calls to Set Copy Protect Level
 *****************************************************************************/
uint8_t USB_Class_Set_Copy_Protect
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set copy protect data*/
        g_copy_protect = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Copy_Protect
 *
 * @brief  This function is called in response to Get Terminal Control Request
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Return Copy Protect Level
 *****************************************************************************/
uint8_t USB_Class_Get_Copy_Protect
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_copy_protect;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_mute = 0x01;
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Cur_Mute
 *
 * @brief  This function is called to Set Current Mute Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets current Mute state specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Cur_Mute
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set cur mute data*/
        g_cur_mute = **coding_data;
        USB_PRINTF("Mute: %d\n", g_cur_mute);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Mute
 *
 * @brief  This function is called to Get Current Mute Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Mute state to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Cur_Mute
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_cur_mute;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_volume[2] = { 0x00, 0x80 };
uint8_t g_min_volume[2] = { 0x00, 0x80 };
uint8_t g_max_volume[2] = { 0xFF, 0x7F };
uint8_t g_res_volume[2] = { 0x01, 0x00 };
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Cur_Volume
 *
 * @brief  This function is called to Set Current Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets current Volume value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Cur_Volume
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    uint16_t volume;
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set current volume data*/
        g_cur_volume[0] = **coding_data;
        g_cur_volume[1] = *(*coding_data + 1);
        volume = (g_cur_volume[1] << 8) | g_cur_volume[0];
        USB_PRINTF("Volume : %x\n", volume);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Min_Volume
 *
 * @brief  This function is called to Set Min Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Volume value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Min_Volume
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set min volume data*/
        g_min_volume[0] = **coding_data;
        g_min_volume[1] = *(*coding_data + 1);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Set_Max_Volume
 *
 * @brief  This function is called to Set Max Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Volume value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Max_Volume
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set max volume data*/
        g_max_volume[0] = **coding_data;
        g_max_volume[1] = *(*coding_data + 1);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Res_Volume
 *
 * @brief  This function is called to Set Resolution Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Volume value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Res_Volume
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)

    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set resolution volume data*/
        g_res_volume[0] = **coding_data;
        g_res_volume[1] = *(*coding_data + 1);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Cur_Volume
 *
 * @brief  This function is called to Get Current Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Volume value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Cur_Volume
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = g_cur_volume;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Min_Volume
 *
 * @brief  This function is called to Get Min Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Min Volume value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Min_Volume
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = g_min_volume;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Max_Volume
 *
 * @brief  This function is called to Get Max Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Max Volume value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Max_Volume
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = g_max_volume;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Res_Volume
 *
 * @brief  This function is called to Get Resolution Volume Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Res Volume value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Res_Volume
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = g_res_volume;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_bass = 0x00;
uint8_t g_min_bass = 0x80;
uint8_t g_max_bass = 0x7F;
uint8_t g_res_bass = 0x01;
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Cur_Bass
 *
 * @brief  This function is called to Set Current Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Bass value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Cur_Bass
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set current bass data*/
        g_cur_bass = **coding_data;
        USB_PRINTF("bass: %d\n", g_cur_bass);
        return USB_OK;
    }

    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Min_Bass
 *
 * @brief  This function is called to Set Min Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Bass value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Min_Bass
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set min bass data*/
        g_min_bass = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Max_Bass
 *
 * @brief  This function is called to Set Max Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Bass value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Max_Bass
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
)
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set max bass data*/
        g_max_bass = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Res_Bass
 *
 * @brief  This function is called to Set Resolution Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Bass value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Res_Bass
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set resolution bass data*/
        g_res_bass = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Cur_Bass
 *
 * @brief  This function is called to Get Current Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Bass value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Cur_Bass
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_cur_bass;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Min_Bass
 *
 * @brief  This function is called to Get Min Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Minimum Bass value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Min_Bass
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_min_bass;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Max_Bass
 *
 * @brief  This function is called to Get Max Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Maximum Bass value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Max_Bass
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_max_bass;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Res_Bass
 *
 * @brief  This function is called to Get Resolution Bass Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Resolution Bass value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Res_Bass
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_res_bass;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_mid = 0x00;
uint8_t g_min_mid = 0x80;
uint8_t g_max_mid = 0x7F;
uint8_t g_res_mid = 0x01;
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Cur_Mid
 *
 * @brief  This function is called to Set Current Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Mid value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Cur_Mid
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set current mid data*/
        g_cur_mid = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Min_Mid
 *
 * @brief  This function is called to Set Min Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Mid value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Min_Mid
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set min mid data*/
        g_min_mid = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Max_Mid
 *
 * @brief  This function is called to Set Max Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Mid value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Max_Mid
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set max mid data*/
        g_max_mid = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Res_Mid
 *
 * @brief  This function is called to Set Resolution Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Mid value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Res_Mid
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set resolution mid data*/
        g_res_mid = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Cur_Mid
 *
 * @brief  This function is called to Get Current Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Mid value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Cur_Mid
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get current data data*/
        *coding_data = &g_cur_mid;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Min_Mid
 *
 * @brief  This function is called to Get Min Mid Value *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Minimum Mid value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Min_Mid
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_min_mid;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Max_Mid
 *
 * @brief  This function is called to Get Max Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Maximum Mid value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Max_Mid
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_max_mid;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Res_Mid
 *
 * @brief  This function is called to Get Resolution Mid Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Resolution Mid value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Res_Mid
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_res_mid;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_treble = 0x01;
uint8_t g_min_treble = 0x80;
uint8_t g_max_treble = 0x7F;
uint8_t g_res_treble = 0x01;
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Cur_Treble
 *
 * @brief  This function is called to Set Current Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Treble value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Cur_Treble
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)

    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set current treble data*/
        g_cur_treble = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Min_Treble
 *
 * @brief  This function is called to Set Min Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Treble value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Min_Treble
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set min treble data*/
        g_min_treble = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Max_Treble
 *
 * @brief  This function is called to Set Max Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Treble value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Max_Treble
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set max treble data*/
        g_max_treble = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Res_Treble
 *
 * @brief  This function is called to Set Resolution Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Treble value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Res_Treble
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set resolution treble data*/
        g_res_treble = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Cur_Treble
 *
 * @brief  This function is called to Get Current Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Treble value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Cur_Treble
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_cur_treble;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Min_Treble
 *
 * @brief  This function is called to Get Min Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Minimum Treble value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Min_Treble
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_min_treble;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Max_Treble
 *
 * @brief  This function is called to Get Max Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Maximum Treble value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Max_Treble
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_max_treble;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Res_Treble
 *
 * @brief  This function is called to Get Resolution Treble Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Resolution Treble value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Res_Treble
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_res_treble;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_graphic_equalizer[4] = { 0x01, 0x01, 0x01, 0x01 };
uint8_t g_min_graphic_equalizer[4] = { 0x01, 0x01, 0x01, 0x01 };
uint8_t g_max_graphic_equalizer[4] = { 0x01, 0x01, 0x01, 0x01 };
uint8_t g_res_graphic_equalizer[4] = { 0x01, 0x01, 0x01, 0x01 };
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Cur_Graphic_Equalizer
 *
 * @brief  This function is called to Set Current Graphic Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Graphic Equalizer values specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Cur_Graphic_Equalizer
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set current graphic equalizer data*/
        g_cur_graphic_equalizer[0] = **coding_data;
        g_cur_graphic_equalizer[1] = *(*coding_data + 1);
        g_cur_graphic_equalizer[2] = *(*coding_data + 2);
        g_cur_graphic_equalizer[3] = *(*coding_data + 3);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Min_Graphic_Equalizer
 *
 * @brief  This function is called to Set Min Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Graphic Equalizer values specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Min_Graphic_Equalizer
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set min graphic equalizer data*/
        g_min_graphic_equalizer[0] = **coding_data;
        g_min_graphic_equalizer[1] = *(*coding_data + 1);
        g_min_graphic_equalizer[2] = *(*coding_data + 2);
        g_min_graphic_equalizer[3] = *(*coding_data + 3);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Max_Graphic_Equalizer
 *
 * @brief  This function is called to Set Max Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Graphic Equalizer values specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Max_Graphic_Equalizer
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set max graphic equalizer data*/
        g_max_graphic_equalizer[0] = **coding_data;
        g_max_graphic_equalizer[1] = *(*coding_data + 1);
        g_max_graphic_equalizer[2] = *(*coding_data + 2);
        g_max_graphic_equalizer[3] = *(*coding_data + 3);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Res_Graphic_Equalizer
 *
 * @brief  This function is called to Set Max Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Graphic Equalizer values specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Res_Graphic_Equalizer
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set resolution graphic equalizer data*/
        g_res_graphic_equalizer[0] = **coding_data;
        g_res_graphic_equalizer[1] = *(*coding_data + 1);
        g_res_graphic_equalizer[2] = *(*coding_data + 2);
        g_res_graphic_equalizer[3] = *(*coding_data + 3);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Cur_Graphic_Equalizer
 *
 * @brief  This function is called to Get Current Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Graphic Equalizer values to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Cur_Graphic_Equalizer
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = g_cur_graphic_equalizer;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Min_Graphic_Equalizer
 *
 * @brief  This function is called to Get Min Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Minimum Graphic Equalizer values to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Min_Graphic_Equalizer
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = g_min_graphic_equalizer;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Max_Graphic_Equalizer
 *
 * @brief  This function is called to Get Max Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Maximum Graphic Equalizer values to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Max_Graphic_Equalizer
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = g_max_graphic_equalizer;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Res_Graphic_Equalizer
 *
 * @brief  This function is called to Get Resolution Graphic Equalizer Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Resolution Graphic Equalizer values to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Res_Graphic_Equalizer
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = g_res_graphic_equalizer;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_automatic_gain = 0x01;
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Cur_Automatic_Gain
 *
 * @brief  This function is called to Set Current Automatic Gain Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Automatic Gain value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Cur_Automatic_Gain
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set current automatic data*/
        g_cur_automatic_gain = **coding_data;
        USB_PRINTF("AGC: %d\n", g_cur_automatic_gain);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Cur_Automatic_Gain
 *
 * @brief  This function is called to Get Current Automatic Gain Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Automatic Gain value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Cur_Automatic_Gain
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_cur_automatic_gain;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_delay[2] = { 0x00, 0x40 };
uint8_t g_min_delay[2] = { 0x00, 0x00 };
uint8_t g_max_delay[2] = { 0xFF, 0xFF };
uint8_t g_res_delay[2] = { 0x00, 0x01 };
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Cur_Delay
 *
 * @brief  This function is called to Set Current Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Delay value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Cur_Delay
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)

    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set current delay data*/
        g_cur_delay[0] = **coding_data;
        g_cur_delay[1] = *(*coding_data + 1);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Min_Delay
 *
 * @brief  This function is called to Set Min Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Delay value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Min_Delay
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)

    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set min delay data*/
        g_min_delay[0] = **coding_data;
        g_min_delay[1] = *(*coding_data + 1);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Max_Delay
 *
 * @brief  This function is called to Set Max Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Delay value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Max_Delay
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set max delay data*/
        g_max_delay[0] = **coding_data;
        g_max_delay[1] = *(*coding_data + 1);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Res_Delay
 *
 * @brief  This function is called to Set Resolution Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Delay value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Res_Delay
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set resolution delay data*/
        g_res_delay[0] = **coding_data;
        g_res_delay[1] = *(*coding_data + 1);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Cur_Delay
 *
 * @brief  This function is called to Get Current Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Delay value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Cur_Delay
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = g_cur_delay;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Min_Delay
 *
 * @brief  This function is called to Get Min Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Minimum Delay value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Min_Delay
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = g_min_delay;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Max_Delay
 *
 * @brief  This function is called to Get Max Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Maximum Delay value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Max_Delay
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = g_max_delay;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Res_Delay
 *
 * @brief  This function is called to Get Resolution Delay Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Resolution Delay value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Res_Delay
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = g_res_delay;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_bass_boost = 0x01;
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Cur_Bass_Boost
 *
 * @brief  This function is called to Set Current Bass Boost Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Set Current Bass Boost value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Cur_Bass_Boost
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set current bass boost data*/
        g_cur_bass_boost = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Cur_Bass_Boost
 *
 * @brief  This function is called to Get Current Bass Boost Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Bass Boost value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Cur_Bass_Boost
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_cur_bass_boost;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_loudness = 0x01;
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Cur_Loudness
 *
 * @brief  This function is called to Set Current Loudness Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Loudness value specified to the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Cur_Loudness
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set current loudness data*/
        g_cur_loudness = **coding_data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Cur_Loudness
 *
 * @brief  This function is called to Get Current Loudness Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Loudness value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Cur_Loudness
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    /* if interface valid */
    if (interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        *coding_data = &g_cur_loudness;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint8_t g_cur_sampling_frequency[3] = { 0x00, 0x00, 0x01 };
uint8_t g_min_sampling_frequency[3] = { 0x00, 0x00, 0x01 };
uint8_t g_max_sampling_frequency[3] = { 0x00, 0x00, 0x01 };
uint8_t g_res_sampling_frequency[3] = { 0x00, 0x00, 0x01 };
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Cur_Sampling_Frequency
 *
 * @brief  The function sets Current Sampling Frequency value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return :
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Sampling Frequency value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Cur_Sampling_Frequency
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    UNUSED_ARGUMENT (interface)

    /* set current sampling frequency data*/
    g_cur_sampling_frequency[0] = **coding_data;
    g_cur_sampling_frequency[1] = *(*coding_data + 1);
    g_cur_sampling_frequency[2] = *(*coding_data + 2);
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Min_Sampling_Frequency
 *
 * @brief  The function sets Current Sampling Frequency value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return :
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Sampling Frequency value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Min_Sampling_Frequency
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    UNUSED_ARGUMENT (interface)

    /* set min sampling frequency data*/
    g_min_sampling_frequency[0] = **coding_data;
    g_min_sampling_frequency[1] = *(*coding_data + 1);
    g_min_sampling_frequency[2] = *(*coding_data + 2);
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Max_Sampling_Frequency
 *
 * @brief  The function sets Current Sampling Frequency value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return :
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Sampling Frequency value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Max_Sampling_Frequency
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    UNUSED_ARGUMENT (interface)

    /* set max sampling frequency data*/
    g_max_sampling_frequency[0] = **coding_data;
    g_max_sampling_frequency[1] = *(*coding_data + 1);
    g_max_sampling_frequency[2] = *(*coding_data + 2);
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Res_Sampling_Frequency
 *
 * @brief  The function sets Current Sampling Frequency value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return :
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Sampling Frequency value specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Res_Sampling_Frequency
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
) 
{
    UNUSED_ARGUMENT (handle)
    UNUSED_ARGUMENT (interface)

    /* set res sampling frequency data*/
    g_res_sampling_frequency[0] = **coding_data;
    g_res_sampling_frequency[1] = *(*coding_data + 1);
    g_res_sampling_frequency[2] = *(*coding_data + 2);
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Cur_Sampling_Frequency
 *
 * @brief  This function is called to Get Current sampling frequency Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current sampling frequency value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Cur_Sampling_Frequency
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
    )
{
    UNUSED_ARGUMENT (handle)
    UNUSED_ARGUMENT(interface)

    * coding_data = g_cur_sampling_frequency;
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Min_Sampling_Frequency
 *
 * @brief  This function is called to Get Current sampling frequency Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Minimize sampling frequency value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Min_Sampling_Frequency
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
)
{
    UNUSED_ARGUMENT (handle)
    UNUSED_ARGUMENT(interface)

    * coding_data = g_min_sampling_frequency;
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Max_Sampling_Frequency
 *
 * @brief  This function is called to Get Current sampling frequency Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Maximize sampling frequency value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Max_Sampling_Frequency
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
)
{
    UNUSED_ARGUMENT (handle)
    UNUSED_ARGUMENT(interface)

    * coding_data = g_max_sampling_frequency;
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Res_Sampling_Frequency
 *
 * @brief  This function is called to Get Current sampling frequency Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Resolution sampling frequency value to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Res_Sampling_Frequency
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
)
{
    UNUSED_ARGUMENT (handle)
    UNUSED_ARGUMENT(interface)

    * coding_data = g_res_sampling_frequency;
    return USB_OK;
}

uint8_t g_cur_pitch = 0x00;
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Cur_Pitch
 *
 * @brief  This function is called to Set Current Pitch Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets current Pitch state specified by the Host
 *****************************************************************************/
uint8_t USB_Class_Set_Cur_Pitch
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
)
{
    UNUSED_ARGUMENT (handle)
    UNUSED_ARGUMENT (interface)

    /* set cur pitch data*/
    g_cur_pitch = **coding_data;
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Cur_Pitch
 *
 * @brief  This function is called to Get Current Pitch Value
 *
 * @param handle          : handle
 * @param interface       : Interface number
 * @param coding_data     : Pointer to Data
 *
 * @return status:
 *      USB_OK                  : When Success
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Pitch state to the Host
 *****************************************************************************/
uint8_t USB_Class_Get_Cur_Pitch
(
    uint32_t handle,
    uint8_t interface,
    uint8_t * *coding_data
)
{
    UNUSED_ARGUMENT (handle)
    UNUSED_ARGUMENT(interface)

    * coding_data = &g_cur_pitch;
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
 *                        USB_OK : When Success
 *                        Others : When Error
 *
 *****************************************************************************/
uint8_t USB_Class_Get_feature
(
    uint32_t handle,
    int32_t cmd,
    uint8_t in_data,
    uint8_t * * out_buf
)
{
    uint8_t error = USBERR_INVALID_REQ_TYPE;

    switch(cmd)
    {
    case GET_CUR_COPY_PROTECT_CONTROL:
        error = USB_Class_Get_Copy_Protect(handle, in_data, out_buf);
        break;
        /* Get current value*/
    case GET_CUR_MUTE_CONTROL:
        error = USB_Class_Get_Cur_Mute(handle, in_data, out_buf);
        break;
    case GET_CUR_VOLUME_CONTROL:
        error = USB_Class_Get_Cur_Volume(handle, in_data, out_buf);
        break;
    case GET_CUR_BASS_CONTROL:
        error = USB_Class_Get_Cur_Bass(handle, in_data, out_buf);
        break;
    case GET_CUR_MID_CONTROL:
        error = USB_Class_Get_Cur_Mid(handle, in_data, out_buf);
        break;
    case GET_CUR_TREBLE_CONTROL:
        error = USB_Class_Get_Cur_Treble(handle, in_data, out_buf);
        break;
    case GET_CUR_GRAPHIC_EQUALIZER_CONTROL:
        error = USB_Class_Get_Cur_Graphic_Equalizer(handle, in_data, out_buf);
        break;
    case GET_CUR_AUTOMATIC_GAIN_CONTROL:
        error = USB_Class_Get_Cur_Automatic_Gain(handle, in_data, out_buf);
        break;
    case GET_CUR_DELAY_CONTROL:
        error = USB_Class_Get_Cur_Delay(handle, in_data, out_buf);
        break;
    case GET_CUR_BASS_BOOST_CONTROL:
        error = USB_Class_Get_Cur_Bass_Boost(handle, in_data, out_buf);
        break;
    case GET_CUR_LOUDNESS_CONTROL:
        error = USB_Class_Get_Cur_Loudness(handle, in_data, out_buf);
        break;
        /* GET_CUR endpoint */
    case GET_CUR_SAMPLING_FREQ_CONTROL:
        error = USB_Class_Get_Cur_Sampling_Frequency(handle, in_data, out_buf);
        break;
    case GET_CUR_PITCH_CONTROL:
        error = USB_Class_Get_Cur_Pitch(handle, in_data, out_buf);
        break;

        /* Get min value*/
    case GET_MIN_VOLUME_CONTROL:
        error = USB_Class_Get_Min_Volume(handle, in_data, out_buf);
        break;
    case GET_MIN_BASS_CONTROL:
        error = USB_Class_Get_Min_Bass(handle, in_data, out_buf);
        break;
    case GET_MIN_MID_CONTROL:
        error = USB_Class_Get_Min_Mid(handle, in_data, out_buf);
        break;
    case GET_MIN_TREBLE_CONTROL:
        error = USB_Class_Get_Min_Treble(handle, in_data, out_buf);
        break;
    case GET_MIN_GRAPHIC_EQUALIZER_CONTROL:
        error = USB_Class_Get_Min_Graphic_Equalizer(handle, in_data, out_buf);
        break;
    case GET_MIN_DELAY_CONTROL:
        error = USB_Class_Get_Min_Delay(handle, in_data, out_buf);
        break;
    case GET_MIN_SAMPLING_FREQ_CONTROL:
        error = USB_Class_Get_Min_Sampling_Frequency(handle, in_data, out_buf);
        break;

        /* Get max value*/
    case GET_MAX_VOLUME_CONTROL:
        error = USB_Class_Get_Max_Volume(handle, in_data, out_buf);
        break;
    case GET_MAX_BASS_CONTROL:
        error = USB_Class_Get_Max_Bass(handle, in_data, out_buf);
        break;
    case GET_MAX_MID_CONTROL:
        error = USB_Class_Get_Max_Mid(handle, in_data, out_buf);
        break;
    case GET_MAX_TREBLE_CONTROL:
        error = USB_Class_Get_Max_Treble(handle, in_data, out_buf);
        break;
    case GET_MAX_GRAPHIC_EQUALIZER_CONTROL:
        error = USB_Class_Get_Max_Graphic_Equalizer(handle, in_data, out_buf);
        break;
    case GET_MAX_DELAY_CONTROL:
        error = USB_Class_Get_Max_Delay(handle, in_data, out_buf);
        break;
    case GET_MAX_SAMPLING_FREQ_CONTROL:
        error = USB_Class_Get_Max_Sampling_Frequency(handle, in_data, out_buf);
        break;

        /* Get res value*/
    case GET_RES_VOLUME_CONTROL:
        error = USB_Class_Get_Res_Volume(handle, in_data, out_buf);
        break;
    case GET_RES_BASS_CONTROL:
        error = USB_Class_Get_Res_Bass(handle, in_data, out_buf);
        break;
    case GET_RES_MID_CONTROL:
        error = USB_Class_Get_Res_Mid(handle, in_data, out_buf);
        break;
    case GET_RES_TREBLE_CONTROL:
        error = USB_Class_Get_Res_Treble(handle, in_data, out_buf);
        break;
    case GET_RES_GRAPHIC_EQUALIZER_CONTROL:
        error = USB_Class_Get_Res_Graphic_Equalizer(handle, in_data, out_buf);
        break;
    case GET_RES_DELAY_CONTROL:
        error = USB_Class_Get_Res_Delay(handle, in_data, out_buf);
        break;
    case GET_RES_SAMPLING_FREQ_CONTROL:
        error = USB_Class_Get_Res_Sampling_Frequency(handle, in_data, out_buf);
        break;
    default:
        error = USBERR_INVALID_REQ_TYPE;
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
 *                        USB_OK : When Success
 *                        Others : When Error
 *
 *****************************************************************************/
uint8_t USB_Class_Set_feature
(
    uint32_t handle,
    int32_t cmd,
    uint8_t in_data,
    uint8_t * * in_buf
)
{
    uint8_t error = USBERR_INVALID_REQ_TYPE;

    switch(cmd)
    {
    case SET_CUR_COPY_PROTECT_CONTROL:
        error = USB_Class_Set_Copy_Protect(handle, in_data, in_buf);
        break;
        /* Get current value*/
    case SET_CUR_MUTE_CONTROL:
        error = USB_Class_Set_Cur_Mute(handle, in_data, in_buf);
        break;
    case SET_CUR_VOLUME_CONTROL:
        error = USB_Class_Set_Cur_Volume(handle, in_data, in_buf);
        break;
    case SET_CUR_BASS_CONTROL:
        error = USB_Class_Set_Cur_Bass(handle, in_data, in_buf);
        break;
    case SET_CUR_MID_CONTROL:
        error = USB_Class_Set_Cur_Mid(handle, in_data, in_buf);
        break;
    case SET_CUR_TREBLE_CONTROL:
        error = USB_Class_Set_Cur_Treble(handle, in_data, in_buf);
        break;
    case SET_CUR_GRAPHIC_EQUALIZER_CONTROL:
        error = USB_Class_Set_Cur_Graphic_Equalizer(handle, in_data, in_buf);
        break;
    case SET_CUR_AUTOMATIC_GAIN_CONTROL:
        error = USB_Class_Set_Cur_Automatic_Gain(handle, in_data, in_buf);
        break;
    case SET_CUR_DELAY_CONTROL:
        error = USB_Class_Set_Cur_Delay(handle, in_data, in_buf);
        break;
    case SET_CUR_BASS_BOOST_CONTROL:
        error = USB_Class_Set_Cur_Bass_Boost(handle, in_data, in_buf);
        break;
    case SET_CUR_LOUDNESS_CONTROL:
        error = USB_Class_Set_Cur_Loudness(handle, in_data, in_buf);
        break;
        /* SET_CUR for endpoint */
    case SET_CUR_SAMPLING_FREQ_CONTROL:
        error = USB_Class_Set_Cur_Sampling_Frequency(handle, in_data, in_buf);
        break;
    case SET_CUR_PITCH_CONTROL:
        error = USB_Class_Set_Cur_Pitch(handle, in_data, in_buf);
        break;

        /* SET min value*/
    case SET_MIN_VOLUME_CONTROL:
        error = USB_Class_Set_Min_Volume(handle, in_data, in_buf);
        break;
    case SET_MIN_BASS_CONTROL:
        error = USB_Class_Set_Min_Bass(handle, in_data, in_buf);
        break;
    case SET_MIN_MID_CONTROL:
        error = USB_Class_Set_Min_Mid(handle, in_data, in_buf);
        break;
    case SET_MIN_TREBLE_CONTROL:
        error = USB_Class_Set_Min_Treble(handle, in_data, in_buf);
        break;
    case SET_MIN_GRAPHIC_EQUALIZER_CONTROL:
        error = USB_Class_Set_Min_Graphic_Equalizer(handle, in_data, in_buf);
        break;
    case SET_MIN_DELAY_CONTROL:
        error = USB_Class_Set_Min_Delay(handle, in_data, in_buf);
        break;
    case SET_MIN_SAMPLING_FREQ_CONTROL:
        error = USB_Class_Set_Min_Sampling_Frequency(handle, in_data, in_buf);
        break;

        /* SET max value*/
    case SET_MAX_VOLUME_CONTROL:
        error = USB_Class_Set_Max_Volume(handle, in_data, in_buf);
        break;
    case SET_MAX_BASS_CONTROL:
        error = USB_Class_Set_Max_Bass(handle, in_data, in_buf);
        break;
    case SET_MAX_MID_CONTROL:
        error = USB_Class_Set_Max_Mid(handle, in_data, in_buf);
        break;
    case SET_MAX_TREBLE_CONTROL:
        error = USB_Class_Set_Max_Treble(handle, in_data, in_buf);
        break;
    case SET_MAX_GRAPHIC_EQUALIZER_CONTROL:
        error = USB_Class_Set_Max_Graphic_Equalizer(handle, in_data, in_buf);
        break;
    case SET_MAX_DELAY_CONTROL:
        error = USB_Class_Set_Max_Delay(handle, in_data, in_buf);
        break;
    case SET_MAX_SAMPLING_FREQ_CONTROL:
        error = USB_Class_Set_Max_Sampling_Frequency(handle, in_data, in_buf);
        break;

        /* SET res value*/
    case SET_RES_VOLUME_CONTROL:
        error = USB_Class_Set_Res_Volume(handle, in_data, in_buf);
        break;
    case SET_RES_BASS_CONTROL:
        error = USB_Class_Set_Res_Bass(handle, in_data, in_buf);
        break;
    case SET_RES_MID_CONTROL:
        error = USB_Class_Set_Res_Mid(handle, in_data, in_buf);
        break;
    case SET_RES_TREBLE_CONTROL:
        error = USB_Class_Set_Res_Treble(handle, in_data, in_buf);
        break;
    case SET_RES_GRAPHIC_EQUALIZER_CONTROL:
        error = USB_Class_Set_Res_Graphic_Equalizer(handle, in_data, in_buf);
        break;
    case SET_RES_DELAY_CONTROL:
        error = USB_Class_Set_Res_Delay(handle, in_data, in_buf);
        break;
    case SET_RES_SAMPLING_FREQ_CONTROL:
        error = USB_Class_Set_Res_Sampling_Frequency(handle, in_data, in_buf);
        break;
    default:
        error = USBERR_INVALID_REQ_TYPE;
        break;
    }
    return error;
}
/* EOF */
