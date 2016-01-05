/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 Freescale Semiconductor;
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
 *
 * @file usb_descriptor.h
 *
 * @author
 *
 * @version
 *
 * @date 
 *
 * @brief The file is a header file for USB Descriptors required for Mouse
 *        Application
 *****************************************************************************/

#ifndef _USB_REQUEST_H
#define _USB_REQUEST_H

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_class_audio.h"
#include "usb_device_stack_interface.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/


/******************************************************************************
 * Types
 *****************************************************************************/

/******************************************************************************
 * Global Functions
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

uint8_t USB_Class_Set_Copy_Protect(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Copy_Protect(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Cur_Mute(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Cur_Volume(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Min_Volume(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Max_Volume(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Res_Volume(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Cur_Mute(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Max_Volume(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Min_Volume(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Cur_Volume(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Res_Volume(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Min_Bass(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Min_Bass(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Max_Bass(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Max_Bass(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Cur_Bass(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Cur_Bass(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Res_Bass(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Res_Bass(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Cur_Mid(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Min_Mid(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Max_Mid(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Res_Mid(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Cur_Mid(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Min_Mid(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Max_Mid(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Res_Mid(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Cur_Treble(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Res_Treble(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Min_Treble(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Max_Treble(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Cur_Treble(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Res_Treble(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Min_Treble(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Max_Treble(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Res_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Cur_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Min_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Max_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Res_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Cur_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Min_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Max_Graphic_Equalizer(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Cur_Automatic_Gain(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Cur_Automatic_Gain(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Cur_Delay(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Cur_Delay(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Min_Delay(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Max_Delay(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Res_Delay(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Cur_Delay(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Min_Delay(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Max_Delay(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Res_Delay(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Cur_Bass_Boost(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Cur_Bass_Boost(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Cur_Loudness(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Cur_Loudness(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Cur_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Min_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Max_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Res_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Cur_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Min_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Max_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Res_Sampling_Frequency(uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Set_Cur_Pitch (uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_Cur_Pitch (uint32_t handle,uint8_t interface,uint8_t * *coding_data);
uint8_t USB_Class_Get_feature(uint32_t handle,int32_t cmd, uint8_t in_data,uint8_t * * in_buf);
uint8_t USB_Class_Set_feature(uint32_t handle,int32_t cmd, uint8_t in_data,uint8_t * * in_buf);

#ifdef __cplusplus
}
#endif

#endif
