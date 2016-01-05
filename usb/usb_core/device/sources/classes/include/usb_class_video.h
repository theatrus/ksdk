/**HEADER********************************************************************
* 
* Copyright (c) 2015 Freescale Semiconductor;
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
* $FileName: usb_class_video.h$
* $Version : 
* $Date    : 
*
* Comments:
*
*****************************************************************************/

#ifndef _USB_CLASS_VIDEO_H
#define _USB_CLASS_VIDEO_H 1

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_class.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/

/* macros for queuing */
#define USB_VIDEO_ISO_REQ_VAL_INVALID             (0xFFFD)
#define USB_VIDEO_BULK_REQ_VAL_INVALID            (0xFFFE)
#define USB_REQ_VAL_INVALID                       (0xFFFF)

typedef uint32_t video_handle_t;


/* command */
/* GET CUR COMMAND */
#define GET_CUR_VC_POWER_MODE_CONTROL                  (0x8101)
#define GET_CUR_VC_ERROR_CODE_CONTROL                  (0x8102)

#define GET_CUR_PU_BACKLIGHT_COMPENSATION_CONTROL         (0x8121)
#define GET_CUR_PU_BRIGHTNESS_CONTROL                     (0x8122)
#define GET_CUR_PU_CONTRACT_CONTROL                       (0x8123)
#define GET_CUR_PU_GAIN_CONTROL                           (0x8124)
#define GET_CUR_PU_POWER_LINE_FREQUENCY_CONTROL           (0x8125)
#define GET_CUR_PU_HUE_CONTROL                            (0x8126)
#define GET_CUR_PU_SATURATION_CONTROL                     (0x8127)
#define GET_CUR_PU_SHARRNESS_CONTROL                      (0x8128)
#define GET_CUR_PU_GAMMA_CONTROL                          (0x8129)
#define GET_CUR_PU_WHITE_BALANCE_TEMPERATURE_CONTROL      (0x812A)
#define GET_CUR_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL (0x812B)
#define GET_CUR_PU_WHITE_BALANCE_COMPONENT_CONTROL        (0x812C)
#define GET_CUR_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL   (0x812D)
#define GET_CUR_PU_DIGITAL_MULTIPLIER_CONTROL             (0x812E)
#define GET_CUR_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL       (0x812F)
#define GET_CUR_PU_HUE_AUTO_CONTROL                       (0x8130)
#define GET_CUR_PU_ANALOG_VIDEO_STANDARD_CONTROL          (0x8131)
#define GET_CUR_PU_ANALOG_LOCK_STATUS_CONTROL             (0x8132)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_CUR_PU_CONTRAST_AUTO_CONTROL                  (0x8133)
#endif

#define GET_CUR_CT_SCANNING_MODE_CONTROL            (0x8141)
#define GET_CUR_CT_AE_MODE_CONTROL                  (0x8142)
#define GET_CUR_CT_AE_PRIORITY_CONTROL              (0x8143)
#define GET_CUR_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL   (0x8144)
#define GET_CUR_CT_EXPOSURE_TIME_RELATIVE_CONTROL   (0x8145)
#define GET_CUR_CT_FOCUS_ABSOLUTE_CONTROL           (0x8146)
#define GET_CUR_CT_FOCUS_RELATIVE_CONTROL           (0x8147)
#define GET_CUR_CT_FOCUS_AUTO_CONTROL               (0x8148)
#define GET_CUR_CT_IRIS_ABSOLUTE_CONTROL            (0x8149)
#define GET_CUR_CT_IRIS_RELATIVE_CONTROL            (0x814A)
#define GET_CUR_CT_ZOOM_ABSOLUTE_CONTROL            (0x814B)
#define GET_CUR_CT_ZOOM_RELATIVE_CONTROL            (0x814C)
#define GET_CUR_CT_PANTILT_ABSOLUTE_CONTROL         (0x814D)
#define GET_CUR_CT_PANTILT_RELATIVE_CONTROL         (0x814E)
#define GET_CUR_CT_ROLL_ABSOLUTE_CONTROL            (0x814F)
#define GET_CUR_CT_ROLL_RELATIVE_CONTROL            (0x8150)
#define GET_CUR_CT_PRIVACY_CONTROL                  (0x8151)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_CUR_CT_FOCUS_SIMPLE_CONTROL             (0x8152)
#define GET_CUR_CT_DIGITAL_WINDOW_CONTROL           (0x8153)
#define GET_CUR_CT_REGION_OF_INTEREST_CONTROL       (0x8154)
#endif

#define GET_CUR_VS_PROBE_CONTROL                         (0x8161)
#define GET_CUR_VS_COMMIT_CONTROL                        (0x8162)
#define GET_CUR_VS_STILL_PROBE_CONTROL                   (0x8163)
#define GET_CUR_VS_STILL_COMMIT_CONTROL                  (0x8164)
#define GET_CUR_VS_STILL_IMAGE_TRIGGER_CONTROL           (0x8165)
#define GET_CUR_VS_STREAM_ERROR_CODE_CONTROL             (0x8166)
#define GET_CUR_VS_GENERATE_KEY_FRAME_CONTROL            (0x8167)
#define GET_CUR_VS_UPDATE_FRAME_SEGMENT_CONTROL          (0x8168)
#define GET_CUR_VS_SYNCH_DELAY_CONTROL                   (0x8169)

#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_CUR_EU_SELECT_LAYER_CONTROL                  (0x8181)
#define GET_CUR_EU_PROFILE_TOOLSET_CONTROL               (0x8182)
#define GET_CUR_EU_VIDEO_RESOLUTION_CONTROL              (0x8183)
#define GET_CUR_EU_MIN_FRAME_INTERVAL_CONTROL            (0x8184)
#define GET_CUR_EU_SLICE_MODE_CONTROL                    (0x8185)
#define GET_CUR_EU_RATE_CONTROL_MODE_CONTROL             (0x8186)
#define GET_CUR_EU_AVERAGE_BITRATE_CONTROL               (0x8187)
#define GET_CUR_EU_CPB_SIZE_CONTROL                      (0x8188)
#define GET_CUR_EU_PEAK_BIT_RATE_CONTROL                 (0x8189)
#define GET_CUR_EU_QUANTIZATION_PARAMS_CONTROL           (0x818A)
#define GET_CUR_EU_SYNC_REF_FRAME_CONTROL                (0x818B)
#define GET_CUR_EU_LTR_BUFFER_CONTROL                    (0x818C)
#define GET_CUR_EU_LTR_PICTURE_CONTROL                   (0x818D)
#define GET_CUR_EU_LTR_VALIDATION_CONTROL                (0x818E)
#define GET_CUR_EU_LEVEL_IDC_LIMIT_CONTROL               (0x818F)
#define GET_CUR_EU_SEI_PAYLOADTYPE_CONTROL               (0x8190)
#define GET_CUR_EU_QP_RANGE_CONTROL                      (0x8191)
#define GET_CUR_EU_PRIORITY_CONTROL                      (0x8192)
#define GET_CUR_EU_START_OR_STOP_LAYER_CONTROL           (0x8193)
#define GET_CUR_EU_ERROR_RESILIENCY_CONTROL              (0x8194)
#endif

/* GET MIN COMMAND */

#define GET_MIN_PU_BACKLIGHT_COMPENSATION_CONTROL         (0x8221)
#define GET_MIN_PU_BRIGHTNESS_CONTROL                     (0x8222)
#define GET_MIN_PU_CONTRACT_CONTROL                       (0x8223)
#define GET_MIN_PU_GAIN_CONTROL                           (0x8224)
#define GET_MIN_PU_HUE_CONTROL                            (0x8226)
#define GET_MIN_PU_SATURATION_CONTROL                     (0x8227)
#define GET_MIN_PU_SHARRNESS_CONTROL                      (0x8228)
#define GET_MIN_PU_GAMMA_CONTROL                          (0x8229)
#define GET_MIN_PU_WHITE_BALANCE_TEMPERATURE_CONTROL      (0x822A)
#define GET_MIN_PU_WHITE_BALANCE_COMPONENT_CONTROL        (0x822C)
#define GET_MIN_PU_DIGITAL_MULTIPLIER_CONTROL             (0x822E)
#define GET_MIN_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL       (0x822F)

#define GET_MIN_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL   (0x8244)
#define GET_MIN_CT_FOCUS_ABSOLUTE_CONTROL           (0x8246)
#define GET_MIN_CT_FOCUS_RELATIVE_CONTROL           (0x8247)
#define GET_MIN_CT_IRIS_ABSOLUTE_CONTROL            (0x8249)
#define GET_MIN_CT_ZOOM_ABSOLUTE_CONTROL            (0x824B)
#define GET_MIN_CT_ZOOM_RELATIVE_CONTROL            (0x824C)
#define GET_MIN_CT_PANTILT_ABSOLUTE_CONTROL         (0x824D)
#define GET_MIN_CT_PANTILT_RELATIVE_CONTROL         (0x824E)
#define GET_MIN_CT_ROLL_ABSOLUTE_CONTROL            (0x824F)
#define GET_MIN_CT_ROLL_RELATIVE_CONTROL            (0x8250)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_MIN_CT_DIGITAL_WINDOW_CONTROL           (0x8251)
#define GET_MIN_CT_REGION_OF_INTEREST_CONTROL       (0x8252)
#endif

#define GET_MIN_VS_PROBE_CONTROL                         (0x8261) 
#define GET_MIN_VS_STILL_PROBE_CONTROL                   (0x8263) 
#define GET_MIN_VS_UPDATE_FRAME_SEGMENT_CONTROL          (0x8268)
#define GET_MIN_VS_SYNCH_DELAY_CONTROL                   (0x8269)

#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_MIN_EU_VIDEO_RESOLUTION_CONTROL       (0x8283)
#define GET_MIN_EU_MIN_FRAME_INTERVAL_CONTROL     (0x8284)
#define GET_MIN_EU_SLICE_MODE_CONTROL             (0x8285)
#define GET_MIN_EU_AVERAGE_BITRATE_CONTROL        (0x8287)
#define GET_MIN_EU_CPB_SIZE_CONTROL               (0x8288)
#define GET_MIN_EU_PEAK_BIT_RATE_CONTROL          (0x8289)
#define GET_MIN_EU_QUANTIZATION_PARAMS_CONTROL    (0x828A)
#define GET_MIN_EU_SYNC_REF_FRAME_CONTROL         (0x828B)
#define GET_MIN_EU_LEVEL_IDC_LIMIT_CONTROL        (0x828F)
#define GET_MIN_EU_SEI_PAYLOADTYPE_CONTROL        (0x8290)
#define GET_MIN_EU_QP_RANGE_CONTROL               (0x8291)
#endif

/* GET MAX COMMAND */

#define GET_MAX_PU_BACKLIGHT_COMPENSATION_CONTROL         (0x8321)
#define GET_MAX_PU_BRIGHTNESS_CONTROL                     (0x8322)
#define GET_MAX_PU_CONTRACT_CONTROL                       (0x8323)
#define GET_MAX_PU_GAIN_CONTROL                           (0x8324)
#define GET_MAX_PU_HUE_CONTROL                            (0x8326)
#define GET_MAX_PU_SATURATION_CONTROL                     (0x8327)
#define GET_MAX_PU_SHARRNESS_CONTROL                      (0x8328)
#define GET_MAX_PU_GAMMA_CONTROL                          (0x8329)
#define GET_MAX_PU_WHITE_BALANCE_TEMPERATURE_CONTROL      (0x832A)
#define GET_MAX_PU_WHITE_BALANCE_COMPONENT_CONTROL        (0x832C)
#define GET_MAX_PU_DIGITAL_MULTIPLIER_CONTROL             (0x832E)
#define GET_MAX_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL       (0x832F)

#define GET_MAX_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL   (0x8344)
#define GET_MAX_CT_FOCUS_ABSOLUTE_CONTROL           (0x8346)
#define GET_MAX_CT_FOCUS_RELATIVE_CONTROL           (0x8347)
#define GET_MAX_CT_IRIS_ABSOLUTE_CONTROL            (0x8349)
#define GET_MAX_CT_ZOOM_ABSOLUTE_CONTROL            (0x834B)
#define GET_MAX_CT_ZOOM_RELATIVE_CONTROL            (0x834C)
#define GET_MAX_CT_PANTILT_ABSOLUTE_CONTROL         (0x834D)
#define GET_MAX_CT_PANTILT_RELATIVE_CONTROL         (0x834E)
#define GET_MAX_CT_ROLL_ABSOLUTE_CONTROL            (0x834F)
#define GET_MAX_CT_ROLL_RELATIVE_CONTROL            (0x8350)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_MAX_CT_DIGITAL_WINDOW_CONTROL           (0x8351)
#define GET_MAX_CT_REGION_OF_INTEREST_CONTROL       (0x8352)
#endif

#define GET_MAX_VS_PROBE_CONTROL                         (0x8361) 
#define GET_MAX_VS_STILL_PROBE_CONTROL                   (0x8363)
#define GET_MAX_VS_UPDATE_FRAME_SEGMENT_CONTROL          (0x8368)
#define GET_MAX_VS_SYNCH_DELAY_CONTROL                   (0x8369)

#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_MAX_EU_VIDEO_RESOLUTION_CONTROL       (0x8383)
#define GET_MAX_EU_MIN_FRAME_INTERVAL_CONTROL     (0x8384)
#define GET_MAX_EU_SLICE_MODE_CONTROL             (0x8385)
#define GET_MAX_EU_AVERAGE_BITRATE_CONTROL        (0x8387)
#define GET_MAX_EU_CPB_SIZE_CONTROL               (0x8388)
#define GET_MAX_EU_PEAK_BIT_RATE_CONTROL          (0x8389)
#define GET_MAX_EU_QUANTIZATION_PARAMS_CONTROL    (0x838A)
#define GET_MAX_EU_SYNC_REF_FRAME_CONTROL         (0x838B)
#define GET_MAX_EU_LTR_BUFFER_CONTROL             (0x838C)
#define GET_MAX_EU_LEVEL_IDC_LIMIT_CONTROL        (0x838F)
#define GET_MAX_EU_SEI_PAYLOADTYPE_CONTROL        (0x8390)
#define GET_MAX_EU_QP_RANGE_CONTROL               (0x8391)
#endif
/* GET RES COMMAND */

#define GET_RES_PU_BACKLIGHT_COMPENSATION_CONTROL         (0x8421)
#define GET_RES_PU_BRIGHTNESS_CONTROL                     (0x8422)
#define GET_RES_PU_CONTRACT_CONTROL                       (0x8423)
#define GET_RES_PU_GAIN_CONTROL                           (0x8424)
#define GET_RES_PU_HUE_CONTROL                            (0x8426)
#define GET_RES_PU_SATURATION_CONTROL                     (0x8427)
#define GET_RES_PU_SHARRNESS_CONTROL                      (0x8428)
#define GET_RES_PU_GAMMA_CONTROL                          (0x8429)
#define GET_RES_PU_WHITE_BALANCE_TEMPERATURE_CONTROL      (0x842A)
#define GET_RES_PU_WHITE_BALANCE_COMPONENT_CONTROL        (0x842C)
#define GET_RES_PU_DIGITAL_MULTIPLIER_CONTROL             (0x842E)
#define GET_RES_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL       (0x842F)

#define GET_RES_CT_AE_MODE_CONTROL                  (0x8442)
#define GET_RES_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL   (0x8444)
#define GET_RES_CT_FOCUS_ABSOLUTE_CONTROL           (0x8446)
#define GET_RES_CT_FOCUS_RELATIVE_CONTROL           (0x8447)
#define GET_RES_CT_IRIS_ABSOLUTE_CONTROL            (0x8449)
#define GET_RES_CT_ZOOM_ABSOLUTE_CONTROL            (0x844B)
#define GET_RES_CT_ZOOM_RELATIVE_CONTROL            (0x844C)
#define GET_RES_CT_PANTILT_ABSOLUTE_CONTROL         (0x844D)
#define GET_RES_CT_PANTILT_RELATIVE_CONTROL         (0x844E)
#define GET_RES_CT_ROLL_ABSOLUTE_CONTROL            (0x844F)
#define GET_RES_CT_ROLL_RELATIVE_CONTROL            (0x8450)

#define GET_RES_VS_PROBE_CONTROL                         (0x8461)
#define GET_RES_VS_STILL_PROBE_CONTROL                   (0x8463)
#define GET_RES_VS_UPDATE_FRAME_SEGMENT_CONTROL          (0x8468)
#define GET_RES_VS_SYNCH_DELAY_CONTROL                   (0x8469)

#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_RES_EU_AVERAGE_BITRATE_CONTROL        (0x8487)
#define GET_RES_EU_CPB_SIZE_CONTROL               (0x8488)
#define GET_RES_EU_PEAK_BIT_RATE_CONTROL          (0x8489)
#define GET_RES_EU_QUANTIZATION_PARAMS_CONTROL    (0x848A)
#define GET_RES_EU_ERROR_RESILIENCY_CONTROL       (0x8494)
#endif

/* GET LEN COMMAND */

#define GET_LEN_VS_PROBE_CONTROL                         (0x8561)
#define GET_LEN_VS_COMMIT_CONTROL                        (0x8562)
#define GET_LEN_VS_STILL_PROBE_CONTROL                   (0x8563)
#define GET_LEN_VS_STILL_COMMIT_CONTROL                  (0x8564)

#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_LEN_EU_SELECT_LAYER_CONTROL           (0x8581)
#define GET_LEN_EU_PROFILE_TOOLSET_CONTROL        (0x8582)
#define GET_LEN_EU_VIDEO_RESOLUTION_CONTROL       (0x8583)
#define GET_LEN_EU_MIN_FRAME_INTERVAL_CONTROL     (0x8584)
#define GET_LEN_EU_SLICE_MODE_CONTROL             (0x8585)
#define GET_LEN_EU_RATE_CONTROL_MODE_CONTROL      (0x8586)
#define GET_LEN_EU_AVERAGE_BITRATE_CONTROL        (0x8587)
#define GET_LEN_EU_CPB_SIZE_CONTROL               (0x8588)
#define GET_LEN_EU_PEAK_BIT_RATE_CONTROL          (0x8589)
#define GET_LEN_EU_QUANTIZATION_PARAMS_CONTROL    (0x858A)
#define GET_LEN_EU_SYNC_REF_FRAME_CONTROL         (0x858B)
#define GET_LEN_EU_LTR_BUFFER_CONTROL             (0x858C)
#define GET_LEN_EU_LTR_PICTURE_CONTROL            (0x858D)
#define GET_LEN_EU_LTR_VALIDATION_CONTROL         (0x858E)
#define GET_LEN_EU_QP_RANGE_CONTROL               (0x8591)
#define GET_LEN_EU_PRIORITY_CONTROL               (0x8592)
#define GET_LEN_EU_START_OR_STOP_LAYER_CONTROL    (0x8593)
#endif

/* GET INFO COMMAND */
#define GET_INFO_VC_POWER_MODE_CONTROL                (0x8601)
#define GET_INFO_VC_ERROR_CODE_CONTROL                (0x8602)

#define GET_INFO_PU_BACKLIGHT_COMPENSATION_CONTROL         (0x8621)
#define GET_INFO_PU_BRIGHTNESS_CONTROL                     (0x8622)
#define GET_INFO_PU_CONTRACT_CONTROL                       (0x8623)
#define GET_INFO_PU_GAIN_CONTROL                           (0x8624)
#define GET_INFO_PU_POWER_LINE_FREQUENCY_CONTROL           (0x8625)
#define GET_INFO_PU_HUE_CONTROL                            (0x8626)
#define GET_INFO_PU_SATURATION_CONTROL                     (0x8627)
#define GET_INFO_PU_SHARRNESS_CONTROL                      (0x8628)
#define GET_INFO_PU_GAMMA_CONTROL                          (0x8629)
#define GET_INFO_PU_WHITE_BALANCE_TEMPERATURE_CONTROL      (0x862A)
#define GET_INFO_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL (0x862B)
#define GET_INFO_PU_WHITE_BALANCE_COMPONENT_CONTROL        (0x862C)
#define GET_INFO_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL   (0x862D)
#define GET_INFO_PU_DIGITAL_MULTIPLIER_CONTROL             (0x862E)
#define GET_INFO_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL       (0x862F)
#define GET_INFO_PU_HUE_AUTO_CONTROL                       (0x8630)
#define GET_INFO_PU_ANALOG_VIDEO_STANDARD_CONTROL          (0x8631)
#define GET_INFO_PU_ANALOG_LOCK_STATUS_CONTROL             (0x8632)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_INFO_PU_CONTRAST_AUTO_CONTROL                  (0x8633)
#endif

#define GET_INFO_CT_SCANNING_MODE_CONTROL            (0x8641)
#define GET_INFO_CT_AE_MODE_CONTROL                  (0x8642)
#define GET_INFO_CT_AE_PRIORITY_CONTROL              (0x8643)
#define GET_INFO_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL   (0x8644)
#define GET_INFO_CT_EXPOSURE_TIME_RELATIVE_CONTROL   (0x8645)
#define GET_INFO_CT_FOCUS_ABSOLUTE_CONTROL           (0x8646)
#define GET_INFO_CT_FOCUS_RELATIVE_CONTROL           (0x8647)
#define GET_INFO_CT_FOCUS_AUTO_CONTROL               (0x8648)
#define GET_INFO_CT_IRIS_ABSOLUTE_CONTROL            (0x8649)
#define GET_INFO_CT_IRIS_RELATIVE_CONTROL            (0x864A)
#define GET_INFO_CT_ZOOM_ABSOLUTE_CONTROL            (0x864B)
#define GET_INFO_CT_ZOOM_RELATIVE_CONTROL            (0x864C)
#define GET_INFO_CT_PANTILT_ABSOLUTE_CONTROL         (0x864D)
#define GET_INFO_CT_PANTILT_RELATIVE_CONTROL         (0x864E)
#define GET_INFO_CT_ROLL_ABSOLUTE_CONTROL            (0x864F)
#define GET_INFO_CT_ROLL_RELATIVE_CONTROL            (0x8650)
#define GET_INFO_CT_PRIVACY_CONTROL                  (0x8651)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_INFO_CT_FOCUS_SIMPLE_CONTROL             (0x8652)
#endif

#define GET_INFO_VS_PROBE_CONTROL                         (0x8661)
#define GET_INFO_VS_COMMIT_CONTROL                        (0x8662)
#define GET_INFO_VS_STILL_PROBE_CONTROL                   (0x8663)
#define GET_INFO_VS_STILL_COMMIT_CONTROL                  (0x8664)
#define GET_INFO_VS_STILL_IMAGE_TRIGGER_CONTROL           (0x8665)
#define GET_INFO_VS_STREAM_ERROR_CODE_CONTROL             (0x8666)
#define GET_INFO_VS_GENERATE_KEY_FRAME_CONTROL            (0x8667)
#define GET_INFO_VS_UPDATE_FRAME_SEGMENT_CONTROL          (0x8668)
#define GET_INFO_VS_SYNCH_DELAY_CONTROL                   (0x8669)

#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_INFO_EU_SELECT_LAYER_CONTROL           (0x8681)
#define GET_INFO_EU_PROFILE_TOOLSET_CONTROL        (0x8682)
#define GET_INFO_EU_VIDEO_RESOLUTION_CONTROL       (0x8683)
#define GET_INFO_EU_MIN_FRAME_INTERVAL_CONTROL     (0x8684)
#define GET_INFO_EU_SLICE_MODE_CONTROL             (0x8685)
#define GET_INFO_EU_RATE_CONTROL_MODE_CONTROL      (0x8686)
#define GET_INFO_EU_AVERAGE_BITRATE_CONTROL        (0x8687)
#define GET_INFO_EU_CPB_SIZE_CONTROL               (0x8688)
#define GET_INFO_EU_PEAK_BIT_RATE_CONTROL          (0x8689)
#define GET_INFO_EU_QUANTIZATION_PARAMS_CONTROL    (0x868A)
#define GET_INFO_EU_SYNC_REF_FRAME_CONTROL         (0x868B)
#define GET_INFO_EU_LTR_BUFFER_CONTROL             (0x868C)
#define GET_INFO_EU_LTR_PICTURE_CONTROL            (0x868D)
#define GET_INFO_EU_LTR_VALIDATION_CONTROL         (0x868E)
#define GET_INFO_EU_SEI_PAYLOADTYPE_CONTROL        (0x8690)
#define GET_INFO_EU_QP_RANGE_CONTROL               (0x8691)
#define GET_INFO_EU_PRIORITY_CONTROL               (0x8692)
#define GET_INFO_EU_START_OR_STOP_LAYER_CONTROL    (0x8693)
#endif

/* GET DEF COMMAND */

#define GET_DEF_PU_BACKLIGHT_COMPENSATION_CONTROL         (0x8721)
#define GET_DEF_PU_BRIGHTNESS_CONTROL                     (0x8722)
#define GET_DEF_PU_CONTRACT_CONTROL                       (0x8723)
#define GET_DEF_PU_GAIN_CONTROL                           (0x8724)
#define GET_DEF_PU_POWER_LINE_FREQUENCY_CONTROL           (0x8725)
#define GET_DEF_PU_HUE_CONTROL                            (0x8726)
#define GET_DEF_PU_SATURATION_CONTROL                     (0x8727)
#define GET_DEF_PU_SHARRNESS_CONTROL                      (0x8728)
#define GET_DEF_PU_GAMMA_CONTROL                          (0x8729)
#define GET_DEF_PU_WHITE_BALANCE_TEMPERATURE_CONTROL      (0x872A)
#define GET_DEF_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL (0x872B)
#define GET_DEF_PU_WHITE_BALANCE_COMPONENT_CONTROL        (0x872C)
#define GET_DEF_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL   (0x872D)
#define GET_DEF_PU_DIGITAL_MULTIPLIER_CONTROL             (0x872E)
#define GET_DEF_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL       (0x872F)
#define GET_DEF_PU_HUE_AUTO_CONTROL                       (0x8730)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_DEF_PU_CONTRAST_AUTO_CONTROL                  (0x8731)
#endif

#define GET_DEF_CT_AE_MODE_CONTROL                  (0x8742)
#define GET_DEF_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL   (0x8744)
#define GET_DEF_CT_FOCUS_ABSOLUTE_CONTROL           (0x8746)
#define GET_DEF_CT_FOCUS_RELATIVE_CONTROL           (0x8747)
#define GET_DEF_CT_FOCUS_AUTO_CONTROL               (0x8748)
#define GET_DEF_CT_IRIS_ABSOLUTE_CONTROL            (0x8749)
#define GET_DEF_CT_ZOOM_ABSOLUTE_CONTROL            (0x874B)
#define GET_DEF_CT_ZOOM_RELATIVE_CONTROL            (0x874C)
#define GET_DEF_CT_PANTILT_ABSOLUTE_CONTROL         (0x874D)
#define GET_DEF_CT_PANTILT_RELATIVE_CONTROL         (0x874E)
#define GET_DEF_CT_ROLL_ABSOLUTE_CONTROL            (0x874F)
#define GET_DEF_CT_ROLL_RELATIVE_CONTROL            (0x8750)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_DEF_CT_FOCUS_SIMPLE_CONTROL             (0x8751)
#define GET_DEF_CT_DIGITAL_WINDOW_CONTROL           (0x8752)
#define GET_DEF_CT_REGION_OF_INTEREST_CONTROL       (0x8753)
#endif

#define GET_DEF_VS_PROBE_CONTROL                         (0x8761) 
#define GET_DEF_VS_STILL_PROBE_CONTROL                   (0x8763) 
#define GET_DEF_VS_UPDATE_FRAME_SEGMENT_CONTROL          (0x8768)
#define GET_DEF_VS_SYNCH_DELAY_CONTROL                   (0x8769)

#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_DEF_EU_PROFILE_TOOLSET_CONTROL        (0x8782)
#define GET_DEF_EU_VIDEO_RESOLUTION_CONTROL       (0x8783)
#define GET_DEF_EU_MIN_FRAME_INTERVAL_CONTROL     (0x8784)
#define GET_DEF_EU_SLICE_MODE_CONTROL             (0x8785)
#define GET_DEF_EU_RATE_CONTROL_MODE_CONTROL      (0x8786)
#define GET_DEF_EU_AVERAGE_BITRATE_CONTROL        (0x8787)
#define GET_DEF_EU_CPB_SIZE_CONTROL               (0x8788)
#define GET_DEF_EU_PEAK_BIT_RATE_CONTROL          (0x8789)
#define GET_DEF_EU_QUANTIZATION_PARAMS_CONTROL    (0x878A)
#define GET_DEF_EU_LTR_BUFFER_CONTROL             (0x878C)
#define GET_DEF_EU_LTR_PICTURE_CONTROL            (0x878D)
#define GET_DEF_EU_LTR_VALIDATION_CONTROL         (0x878E)
#define GET_DEF_EU_LEVEL_IDC_LIMIT_CONTROL        (0x878F)
#define GET_DEF_EU_SEI_PAYLOADTYPE_CONTROL        (0x8790)
#define GET_DEF_EU_QP_RANGE_CONTROL               (0x8791)
#define GET_DEF_EU_ERROR_RESILIENCY_CONTROL       (0x8794)
#endif

/* SET CUR COMMAND */
#define SET_CUR_VC_POWER_MODE_CONTROL                 (0x0101)

#define SET_CUR_PU_BACKLIGHT_COMPENSATION_CONTROL         (0x0121)
#define SET_CUR_PU_BRIGHTNESS_CONTROL                     (0x0122)
#define SET_CUR_PU_CONTRACT_CONTROL                       (0x0123)
#define SET_CUR_PU_GAIN_CONTROL                           (0x0124)
#define SET_CUR_PU_POWER_LINE_FREQUENCY_CONTROL           (0x0125)
#define SET_CUR_PU_HUE_CONTROL                            (0x0126)
#define SET_CUR_PU_SATURATION_CONTROL                     (0x0127)
#define SET_CUR_PU_SHARRNESS_CONTROL                      (0x0128)
#define SET_CUR_PU_GAMMA_CONTROL                          (0x0129)
#define SET_CUR_PU_WHITE_BALANCE_TEMPERATURE_CONTROL      (0x012A)
#define SET_CUR_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL (0x012B)
#define SET_CUR_PU_WHITE_BALANCE_COMPONENT_CONTROL        (0x012C)
#define SET_CUR_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL   (0x012D)
#define SET_CUR_PU_DIGITAL_MULTIPLIER_CONTROL             (0x012E)
#define SET_CUR_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL       (0x012F)
#define SET_CUR_PU_HUE_AUTO_CONTROL                       (0x0130)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define SET_CUR_PU_CONTRAST_AUTO_CONTROL                  (0x0131)
#endif

#define SET_CUR_CT_SCANNING_MODE_CONTROL            (0x0141)
#define SET_CUR_CT_AE_MODE_CONTROL                  (0x0142)
#define SET_CUR_CT_AE_PRIORITY_CONTROL              (0x0143)
#define SET_CUR_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL   (0x0144)
#define SET_CUR_CT_EXPOSURE_TIME_RELATIVE_CONTROL   (0x0145)
#define SET_CUR_CT_FOCUS_ABSOLUTE_CONTROL           (0x0146)
#define SET_CUR_CT_FOCUS_RELATIVE_CONTROL           (0x0147)
#define SET_CUR_CT_FOCUS_AUTO_CONTROL               (0x0148)
#define SET_CUR_CT_IRIS_ABSOLUTE_CONTROL            (0x0149)
#define SET_CUR_CT_IRIS_RELATIVE_CONTROL            (0x014A)
#define SET_CUR_CT_ZOOM_ABSOLUTE_CONTROL            (0x014B)
#define SET_CUR_CT_ZOOM_RELATIVE_CONTROL            (0x014C)
#define SET_CUR_CT_PANTILT_ABSOLUTE_CONTROL         (0x014D)
#define SET_CUR_CT_PANTILT_RELATIVE_CONTROL         (0x014E)
#define SET_CUR_CT_ROLL_ABSOLUTE_CONTROL            (0x014F)
#define SET_CUR_CT_ROLL_RELATIVE_CONTROL            (0x0150)
#define SET_CUR_CT_PRIVACY_CONTROL                  (0x0151)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define SET_CUR_CT_FOCUS_SIMPLE_CONTROL             (0x0152)
#define SET_CUR_CT_DIGITAL_WINDOW_CONTROL           (0x0153)
#define SET_CUR_CT_REGION_OF_INTEREST_CONTROL       (0x0154)
#endif

#define SET_CUR_VS_PROBE_CONTROL                         (0x0161)
#define SET_CUR_VS_COMMIT_CONTROL                        (0x0162)
#define SET_CUR_VS_STILL_PROBE_CONTROL                   (0x0163)
#define SET_CUR_VS_STILL_COMMIT_CONTROL                  (0x0164)
#define SET_CUR_VS_STILL_IMAGE_TRIGGER_CONTROL           (0x0165)
#define SET_CUR_VS_STREAM_ERROR_CODE_CONTROL             (0x0166)
#define SET_CUR_VS_GENERATE_KEY_FRAME_CONTROL            (0x0167)
#define SET_CUR_VS_UPDATE_FRAME_SEGMENT_CONTROL          (0x0168)
#define SET_CUR_VS_SYNCH_DELAY_CONTROL                   (0x0169)

#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define SET_CUR_EU_SELECT_LAYER_CONTROL           (0x0181)
#define SET_CUR_EU_PROFILE_TOOLSET_CONTROL        (0x0182)
#define SET_CUR_EU_VIDEO_RESOLUTION_CONTROL       (0x0183)
#define SET_CUR_EU_MIN_FRAME_INTERVAL_CONTROL     (0x0184)
#define SET_CUR_EU_SLICE_MODE_CONTROL             (0x0185)
#define SET_CUR_EU_RATE_CONTROL_MODE_CONTROL      (0x0186)
#define SET_CUR_EU_AVERAGE_BITRATE_CONTROL        (0x0187)
#define SET_CUR_EU_CPB_SIZE_CONTROL               (0x0188)
#define SET_CUR_EU_PEAK_BIT_RATE_CONTROL          (0x0189)
#define SET_CUR_EU_QUANTIZATION_PARAMS_CONTROL    (0x018A)
#define SET_CUR_EU_SYNC_REF_FRAME_CONTROL         (0x018B)
#define SET_CUR_EU_LTR_BUFFER_CONTROL             (0x018C)
#define SET_CUR_EU_LTR_PICTURE_CONTROL            (0x018D)
#define SET_CUR_EU_LTR_VALIDATION_CONTROL         (0x018E)
#define SET_CUR_EU_LEVEL_IDC_LIMIT_CONTROL        (0x018F)
#define SET_CUR_EU_SEI_PAYLOADTYPE_CONTROL        (0x0190)
#define SET_CUR_EU_QP_RANGE_CONTROL               (0x0191)
#define SET_CUR_EU_PRIORITY_CONTROL               (0x0192)
#define SET_CUR_EU_START_OR_STOP_LAYER_CONTROL    (0x0193)
#define SET_CUR_EU_ERROR_RESILIENCY_CONTROL       (0x0194)
#endif

/* Video Tierminal Types Code */

/* USB Terminal Type Codes */
#define TT_VENDOR_SPECIFIC                           (0x0100)
#define TT_STREAMING                                  (0x0101)

/* Input Terminal Type Codes */
#define ITT_VENDOR_SPECIFIC                           (0x0200)
#define ITT_CAMERA                                     (0x0201)
#define ITT_MEDIA_TRANSPORT_INPUT                       (0x0202)

/* Output Terminal Type Codes */
#define OTT_VENDOR_SPECIFIC                           (0x0300)
#define OTT_DISPLAY                                   (0x0301)
#define OTT_MEDIA_TRANSPORT_ONTPUT                    (0x0302)

/* External Terminal Type Codes */
#define EXTERNAL_VENDOR_SPECIFIC                        (0x0400)
#define COMPOSITE_CONNECTOR                            (0x0401)
#define SVIDEO_CONNECTOR                               (0x0402)
#define COMPONENT_CONNECTOR                            (0x0403)

/* Video Interface Class Code */
#define CC_VIDEO                          (0x0E)

/* Video Interface Subclass Codes */
#define SC_UNDEFINED                      (0x00)
#define SC_VIDEOCONTROL                   (0x01)
#define SC_VIDEOSTREAMING                 (0x02)
#define SC_VIDEO_INTERFACE_COLLECTION     (0x03)

/* Video Interface Protocol Codes */
#define PC_PROTOCOL_UNDEFINED             (0x00)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define PC_PROTOCOL_15                     (0x01)
#endif

/* Video Class-Specific Descriptor Types */
#define CS_UNDEFINED                      (0x20)
#define CS_DEVICE                         (0x21)
#define CS_CONFIGURATION                  (0x22)
#define CS_STRING                         (0x23)
#define CS_INTERFACE                      (0x24)
#define CS_ENDPOINT                       (0x25)

/* Video Class-Specific VC Interface Descriptor Subtypes */
#define VC_DESCRIPTOR_UNDEFINED           (0x00)
#define VC_HEADER                         (0x01)
#define VC_INPUT_TERMINAL                 (0x02)
#define VC_OUTPUT_TERMINAL                (0x03)
#define VC_SECLECTOR_UNIT                 (0x04)
#define VC_PROCESSING_UNIT                (0x05)
#define VC_EXTENSION_UNIT                 (0x06)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define VC_ENCODING_UNIT                  (0x07)
#endif

/* Video Class-specific VS Interface Desriptor Subtypes */
#define VS_UNDEFINED                      (0x00)
#define VS_INPUT_HEADER                   (0x01)
#define VS_OUTPUT_HEADER                  (0x02)
#define VS_STILL_IMAGE_FRAME              (0x03)
#define VS_FORMAT_UNCOMPRESSED            (0x04)
#define VS_FRAME_UNCOMPRESSED             (0x05)
#define VS_FORMAT_MJPEG                   (0x06)
#define VS_FRAME_MJPEG                    (0x07)
#define VS_FORMAT_MPEG2TS                 (0x0A)
#define VS_FORMAT_DV                      (0x0C)
#define VS_COLORFORMAT                    (0x0D)
#define VS_FORMAT_FRAME_BASED             (0x10)
#define VS_FRAME_FRAME_BASED              (0x11)
#define VS_FORMAT_STREAM_BASED            (0x12)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define VS_FORMAT_H264                    (0x13)
#define VS_FRAME_H264                     (0x14)
#define VS_FORMAT_H264_SIMULCAST          (0x15)
#define VS_FORMAT_VP8                     (0x16)
#define VS_FRAME_VP8                      (0x17)
#define VS_FORMAT_VP8_SIMULCAST           (0x18)
#endif

/* Video Class-Specific Endpoint Descriptor Subtypes */
#define EP_UNDEFINED                      (0x00)
#define EP_GENERAL                        (0x01)
#define EP_ENDPOINT                       (0x02)
#define EP_INTERRUPT                      (0x03)

/* Video Class-Specific Request Codes */
#define RC_UNDEFINED                      (0x00)
#define SET_CUR                           (0x01)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define SET_CUR_ALL                       (0x11)
#endif
#define GET_CUR                           (0x81)
#define GET_MIN                           (0x82)
#define GET_MAX                           (0x83)
#define GET_RES                           (0x84)
#define GET_LEN                           (0x85)
#define GET_INFO                          (0x86)
#define GET_DEF                           (0x87)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define GET_CUR_ALL                       (0x91)
#define GET_MIN_ALL                       (0x92)
#define GET_MAX_ALL                       (0x93)
#define GET_RES_ALL                       (0x94)
#define GET_DEF_ALL                       (0x97)
#endif


/* VideoControl Interface Control Selector Codes */
#define VC_CONTROL_UNDEFINED              (0x00)
#define VC_VIDEO_POWER_MODE_CONTROL       (0x01)
#define VC_REQUEST_ERROR_CODE_CONTROL     (0x02)

/* Terminal Control Selector Codes */
#define TE_CONTROL_UNDEFINED              (0x00)

/* Selector Unit Control Selector Codes */
#define SU_CONTROL_UNDEFINED               (0x00)
#define SU_INPUT_SELECT_CONTROL            (0x01)

/* Camera Terminal Control Selector Codes */
#define CT_CONTROL_UNDEFINED                (0x00)
#define CT_SCANNING_MODE_CONTROL            (0x01)
#define CT_AE_MODE_CONTROL                  (0x02)
#define CT_AE_PRIORITY_CONTROL              (0x03)
#define CT_EXPOSURE_TIME_ABSOLUTE_CONTROL   (0x04)
#define CT_EXPOSURE_TIME_RELATIVE_CONTROL   (0x05)
#define CT_FOCUS_ABSOLUTE_CONTROL           (0x06)
#define CT_FOCUS_RELATIVE_CONTROL           (0x07)
#define CT_FOCUS_AUTO_CONTROL               (0x08)
#define CT_IRIS_ABSOLUTE_CONTROL            (0x09)
#define CT_IRIS_RELATIVE_CONTROL            (0x0A)
#define CT_ZOOM_ABSOLUTE_CONTROL            (0x0B)
#define CT_ZOOM_RELATIVE_CONTROL            (0x0C)
#define CT_PANTILT_ABSOLUTE_CONTROL         (0x0D)
#define CT_PANTILT_RELATIVE_CONTROL         (0x0E)
#define CT_ROLL_ABSOLUTE_CONTROL            (0x0F)
#define CT_ROLL_RELATIVE_CONTROL            (0x10)
#define CT_PRIVACY_CONTROL                  (0x11)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define CT_FOCUS_SIMPLE_CONTROL             (0x12)
#define CT_DIGITAL_WINDOW_CONTROL           (0x13)
#define CT_REGION_OF_INTEREST_CONTROL       (0x14)
#endif

/* Processing Unit Control Selector Codes */
#define PU_CONTROL_UNDEFINED                      (0x00)
#define PU_BACKLIGHT_COMPENSATION_CONTROL         (0x01)
#define PU_BRIGHTNESS_CONTROL                     (0x02)
#define PU_CONTRACT_CONTROL                       (0x03)
#define PU_GAIN_CONTROL                           (0x04)
#define PU_POWER_LINE_FREQUENCY_CONTROL           (0x05)
#define PU_HUE_CONTROL                            (0x06)
#define PU_SATURATION_CONTROL                     (0x07)
#define PU_SHARRNESS_CONTROL                      (0x08)
#define PU_GAMMA_CONTROL                          (0x09)
#define PU_WHITE_BALANCE_TEMPERATURE_CONTROL      (0x0A)
#define PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL (0x0B)
#define PU_WHITE_BALANCE_COMPONENT_CONTROL        (0x0C)
#define PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL   (0x0D)
#define PU_DIGITAL_MULTIPLIER_CONTROL             (0x0E)
#define PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL       (0x0F)
#define PU_HUE_AUTO_CONTROL                       (0x10)
#define PU_ANALOG_VIDEO_STANDARD_CONTROL          (0x11)
#define PU_ANALOG_LOCK_STATUS_CONTROL             (0x12)
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
#define PU_CONTRAST_AUTO_CONTROL                  (0x13)
#endif

#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
/* Encoding Unit Control Selectors Codes */
#define EU_CONTROL_UNDEFINED              (0x00)
#define EU_SELECT_LAYER_CONTROL           (0x01)
#define EU_PROFILE_TOOLSET_CONTROL        (0x02)
#define EU_VIDEO_RESOLUTION_CONTROL       (0x03)
#define EU_MIN_FRAME_INTERVAL_CONTROL     (0x04)
#define EU_SLICE_MODE_CONTROL             (0x05)
#define EU_RATE_CONTROL_MODE_CONTROL      (0x06)
#define EU_AVERAGE_BITRATE_CONTROL        (0x07)
#define EU_CPB_SIZE_CONTROL               (0x08)
#define EU_PEAK_BIT_RATE_CONTROL          (0x09)
#define EU_QUANTIZATION_PARAMS_CONTROL    (0x0A)
#define EU_SYNC_REF_FRAME_CONTROL         (0x0B)
#define EU_LTR_BUFFER_CONTROL             (0x0C)
#define EU_LTR_PICTURE_CONTROL            (0x0D)
#define EU_LTR_VALIDATION_CONTROL         (0x0E)
#define EU_LEVEL_IDC_LIMIT_CONTROL        (0x0F)
#define EU_SEI_PAYLOADTYPE_CONTROL        (0x10)
#define EU_QP_RANGE_CONTROL               (0x11)
#define EU_PRIORITY_CONTROL               (0x12)
#define EU_START_OR_STOP_LAYER_CONTROL    (0x13)
#define EU_ERROR_RESILIENCY_CONTROL       (0x14)
#endif

/* Extension Unit Control Selectors Codes */
#define XU_CONTROL_UNDEFINED                       (0x00)

/* VideoStreming Unit Control Selector Codes */
#define VS_CONTROL_UNDEFINED                     (0x00)
#define VS_PROBE_CONTROL                         (0x01)
#define VS_COMMIT_CONTROL                        (0x02)
#define VS_STILL_PROBE_CONTROL                   (0x03)
#define VS_STILL_COMMIT_CONTROL                  (0x04)
#define VS_STILL_IMAGE_TRIGGER_CONTROL           (0x05)
#define VS_STREAM_ERROR_CODE_CONTROL             (0x06)
#define VS_GENERATE_KEY_FRAME_CONTROL            (0x07)
#define VS_UPDATE_FRAME_SEGMENT_CONTROL          (0x08)
#define VS_SYNCH_DELAY_CONTROL                   (0x09)

PACKED_STRUCT_BEGIN
struct _video_mjpeg_payload_header_struct
{
    uint8_t          bHeaderLength;
    union
    {
        uint8_t      bmHeaderInfo;
        struct
        {
          uint8_t frame_id:1;
          uint8_t end_of_frame:1;
          uint8_t presentation_time:1;
          uint8_t source_clock:1;
          uint8_t reserved:1;
          uint8_t still_image:1;
          uint8_t error:1;
          uint8_t end_of_header:1;
        }bitMap;
        struct
        {
          uint8_t FID:1;
          uint8_t EOI:1;
          uint8_t PTS:1;
          uint8_t SCR:1;
          uint8_t RES:1;
          uint8_t STI:1;
          uint8_t ERR:1;
          uint8_t EOH:1;
        }bitField;
    }HeaderInfo;
    uint32_t         dwPresentationTime;
    uint8_t          bSourceClockReference[6];
} PACKED_STRUCT_END;
typedef struct _video_mjpeg_payload_header_struct video_mjpeg_payload_header_struct_t;

PACKED_STRUCT_BEGIN
struct _video_probe_and_commit_controls_struct
{
    union
    {
        uint16_t     Hint;
        struct
        {
          uint8_t dwFrameInterval:1;
          uint8_t wKeyFrameRate:1;
          uint8_t wPFrameRate:1;
          uint8_t wCompQuality:1;
          uint8_t wCompWindowSize:1;
        }bitMap;
    }bmHint;
    uint8_t          bFormatIndex;
    uint8_t          bFrameIndex;
    uint32_t         dwFrameInterval;
    uint16_t         wKeyFrameRate;
    uint16_t         wPFrameRate;
    uint16_t         wCompQuality;
    uint16_t         wCompWindowSize;
    uint16_t         wDelay;
    uint32_t         dwMaxVideoFrameSize;
    uint32_t         dwMaxPayloadTransferSize;
    uint32_t         dwClockFrequency;
    uint8_t          bmFramingInfo;
    uint8_t          bPreferedVersion;
    uint8_t          bMinVersion;
    uint8_t          bMaxVersion;
#if defined(USBCFG_VIDEO_CLASS_1_5) && USBCFG_VIDEO_CLASS_1_5
    uint8_t          bUsage;
    uint8_t          bBitDepthLuma;
    uint8_t          bmSettings;
    uint8_t          bMaxNumberOfRefFramesPlus1;
    uint16_t         bmRateControlModes;
    uint64_t         bmLayoutPerStream;
#endif
} PACKED_STRUCT_END;
typedef struct _video_probe_and_commit_controls_struct video_probe_and_commit_controls_struct_t;

PACKED_STRUCT_BEGIN
struct _video_still_probe_and_commit_controls_struct
{
    uint8_t          bFormatIndex;
    uint8_t          bFrameIndex;
    uint8_t          bCompressionIndex;
    uint32_t         dwMaxVideoFrameSize;
    uint32_t         dwMaxPayloadTransferSize;
} PACKED_STRUCT_END;
typedef struct _video_still_probe_and_commit_controls_struct video_still_probe_and_commit_controls_struct_t;


/* structure to hold a request in the endpoint queue */
typedef struct _video_ut_struct
{
    uint8_t         unit_id;     /* uint id         */
    uint8_t         type;        /* type of uint        */
    uint16_t        unit_type;   /* type of terminal        */
}video_ut_struct_t;

typedef  struct _video_units_struct 
{
    uint8_t            count;       /* Number of terminal or Ferture Unit End point */  
    video_ut_struct_t* put;         /* Array of terminal or Feature Unit */
}video_units_struct_t; 


/* MSD Configuration structure to be passed by APP*/
typedef struct _video_config_struct
{    
    usb_application_callback_struct_t          video_application_callback;/*!< application callback function to handle the Device status related event*/
    usb_vendor_req_callback_struct_t           vendor_req_callback;       /*!< application callback function to handle the vendor request related event, reserved for future use*/
    usb_class_specific_callback_struct_t       class_specific_callback;   /*!< application callback function to handle all the class related event*/
    usb_desc_request_notify_struct_t*          desc_callback_ptr;         /*!< descriptor related callback function data structure*/
    usb_board_init_callback_struct_t           board_init_callback;       /*!< application callback function to handle board init*/
}video_config_struct_t;

extern void USB_Class_Periodic_Task(void);
#define USB_Video_Periodic_Task USB_Class_Periodic_Task 
/******************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Class_Video_Init
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param msd_config_ptr    : Configuration paramemter strucutre pointer
 *                            passed by APP.
 * @return status       
 *         MSD Handle           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the Video Class layer and layers it is dependednt on 
 ******************************************************************************/
extern usb_status USB_Class_Video_Init
(
    uint8_t controller_id,
    video_config_struct_t* video_config_ptr,
    video_handle_t *  video_handle
); 


/**************************************************************************//*!
 *
 * @name  USB_Class_Video_Deinit
 *
 * @brief The funtion initializes the Device and Controller layer 
 *
 * @param cdc_handle
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 ******************************************************************************
 *
 *This function initializes the Video Class layer and layers it is dependednt on 
 *
 *****************************************************************************/
extern usb_status USB_Class_Video_Deinit
(
    video_handle_t handle
);

/**************************************************************************//*!
 *
 * @name  USB_Class_Video_Send_Data
 *
 * @brief This fucntion is used by Application to send data through Video class
 *
 * @param controller_ID     : Controller ID
 * @param ep_num            : Endpoint number
 * @param app_buff          : Buffer to send
 * @param size              : Length of the transfer
 *
 * @return status:
 *                        USB_OK        : When Successfull
 *                        Others        : When Error
 *
 ******************************************************************************
 * This fucntion is used by Application to send data through Video class
 *****************************************************************************/
uint8_t USB_Class_Video_Send_Data (
        video_handle_t handle,       /* [IN] class handle */
        uint8_t ep_num,              /* [IN] Endpoint Number */
        uint8_t* app_buff,        /* [IN] Buffer to Send */
        uint32_t size        /* [IN] Length of the Transfer */
);

/**************************************************************************//*!
 *
 * @name  USB_Class_Video_Recv_Data
 *
 * @brief This fucntion is used by Application to send data through Video class
 *
 * @param handle            : Class handle
 * @param ep_num            : Endpoint number
 * @param app_buff          : Buffer to send
 * @param size              : Length of the transfer
 *
 * @return status:
 *                        USB_OK        : When Successfull
 *                        Others        : When Error
 *
 ******************************************************************************
 * This fucntion is used by Application to send data through Video class
 *****************************************************************************/
uint8_t USB_Class_Video_Recv_Data (
        video_handle_t handle,       /* [IN] class handle */
        uint8_t ep_num,              /* [IN] Endpoint Number */
        uint8_t* app_buff,        /* [IN] Buffer to Send */
        uint32_t size        /* [IN] Length of the Transfer */
);

#if USBCFG_DEV_ADVANCED_CANCEL_ENABLE
/**************************************************************************//*!
 *
 * @name  USB_Class_Video_Cancel
 *
 * @brief 
 *
 * @param handle          :   handle returned by USB_Class_Video_Init
 * @param ep_num          :   endpoint num 
 * @param direction        :   direction of the endpoint 
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *****************************************************************************/
usb_status USB_Class_Video_Cancel
(
    video_handle_t handle,/*[IN]*/
    uint8_t ep_num,/*[IN]*/
    uint8_t direction
);
#endif

/**************************************************************************//*!
 *
 * @name  USB_Class_Video_Get_Speed
 *
 * @brief This functions get speed from Host.
 *
 * @param handle          :   handle returned by USB_Class_Video_Init
 * @param speed           :   speed
 *
 * @return status       
 *         USB_OK         : When Successfull 
 *         Others         : Errors
 *****************************************************************************/
usb_status USB_Class_Video_Get_Speed
(
    video_handle_t        handle,
    uint16_t *            speed/* [OUT] the requested error */
);

#endif


