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
* $FileName: usb_host_audio.h$
* $Version :
* $Date    :
*
* Comments:
*
*   This file defines a template structure for Audio Class Driver.
*
*END************************************************************************/
#ifndef __usb_host_audio_h__
#define __usb_host_audio_h__


#define SET_REQUEST_ITF                  (0x21)
#define SET_REQUEST_EP                   (0x22)
#define GET_REQUEST_ITF                  (0xA1)
#define GET_REQUEST_EP                   (0xA2)

/* Audio class codes */
#define REQUEST_CODE_UNDEFINED           (0x00)
#define SET_CUR                          (0x01)
#define GET_CUR                          (0x81)
#define SET_MIN                          (0x02)
#define GET_MIN                          (0x82)
#define SET_MAX                          (0x03)
#define GET_MAX                          (0x83)
#define SET_RES                          (0x04)
#define GET_RES                          (0x84)
#define SET_MEM                          (0x05)
#define GET_MEM                          (0x85)
#define GET_STAT                         (0xFF)

/* FU Command codes*/
#define USB_AUDIO_GET_CUR_MUTE           (0x00)
#define USB_AUDIO_SET_CUR_MUTE           (0x01)
#define USB_AUDIO_GET_CUR_VOLUME         (0x02)
#define USB_AUDIO_SET_CUR_VOLUME         (0x03)
#define USB_AUDIO_GET_MIN_VOLUME         (0x04)
#define USB_AUDIO_SET_MIN_VOLUME         (0x05)
#define USB_AUDIO_GET_MAX_VOLUME         (0x06)
#define USB_AUDIO_SET_MAX_VOLUME         (0x07)
#define USB_AUDIO_GET_RES_VOLUME         (0x08)
#define USB_AUDIO_SET_RES_VOLUME         (0x09)
#define USB_AUDIO_GET_CUR_BASS           (0x0A)
#define USB_AUDIO_SET_CUR_BASS           (0x0B)
#define USB_AUDIO_GET_MIN_BASS           (0x0C)
#define USB_AUDIO_SET_MIN_BASS           (0x0D)
#define USB_AUDIO_GET_MAX_BASS           (0x0E)
#define USB_AUDIO_SET_MAX_BASS           (0x0F)
#define USB_AUDIO_GET_RES_BASS           (0x10)
#define USB_AUDIO_SET_RES_BASS           (0x11)
#define USB_AUDIO_GET_CUR_MID            (0x12)
#define USB_AUDIO_SET_CUR_MID            (0x13)
#define USB_AUDIO_GET_MIN_MID            (0x14)
#define USB_AUDIO_SET_MIN_MID            (0x15)
#define USB_AUDIO_GET_MAX_MID            (0x16)
#define USB_AUDIO_SET_MAX_MID            (0x17)
#define USB_AUDIO_GET_RES_MID            (0x18)
#define USB_AUDIO_SET_RES_MID            (0x19)
#define USB_AUDIO_GET_CUR_TREBLE         (0x1A)
#define USB_AUDIO_SET_CUR_TREBLE         (0x1B)
#define USB_AUDIO_GET_MIN_TREBLE         (0x1C)
#define USB_AUDIO_SET_MIN_TREBLE         (0x1D)
#define USB_AUDIO_GET_MAX_TREBLE         (0x1E)
#define USB_AUDIO_SET_MAX_TREBLE         (0x1F)
#define USB_AUDIO_GET_RES_TREBLE         (0x20)
#define USB_AUDIO_SET_RES_TREBLE         (0x21)
#define USB_AUDIO_GET_CUR_AGC            (0x22)
#define USB_AUDIO_SET_CUR_AGC            (0x23)
#define USB_AUDIO_GET_CUR_DELAY          (0x24)
#define USB_AUDIO_SET_CUR_DELAY          (0x25)
#define USB_AUDIO_GET_MIN_DELAY          (0x26)
#define USB_AUDIO_SET_MIN_DELAY          (0x27)
#define USB_AUDIO_GET_MAX_DELAY          (0x28)
#define USB_AUDIO_SET_MAX_DELAY          (0x29)
#define USB_AUDIO_GET_RES_DELAY          (0x2A)
#define USB_AUDIO_SET_RES_DELAY          (0x2B)
#define USB_AUDIO_GET_CUR_BASS_BOOST     (0x2C)
#define USB_AUDIO_SET_CUR_BASS_BOOST     (0x2D)

/* Terminal Control Selectors */
#define TE_CONTROL_UNDEFINED             (0x00)
#define COPY_PROTECT_CONTROL             (0x01)

/* Endpoint Control Selectors */
#define EP_CONTROL_UNDEFINED             (0x00)
#define SAMPLING_FREQ_CONTROL            (0x01)
#define PITCH_CONTROL                    (0x02)

#define SAMPLING_FREQ_MASK               (0x01)
#define PITCH_MASK                       (0x02)

/* Endpoint Command Codes*/
#define USB_AUDIO_GET_CUR_PITCH          (0x00)
#define USB_AUDIO_SET_CUR_PITCH          (0x01)
#define USB_AUDIO_GET_CUR_SAMPLING_FREQ  (0x02)
#define USB_AUDIO_SET_CUR_SAMPLING_FREQ  (0x03)
#define USB_AUDIO_GET_MIN_SAMPLING_FREQ  (0x04)
#define USB_AUDIO_SET_MIN_SAMPLING_FREQ  (0x05)
#define USB_AUDIO_GET_MAX_SAMPLING_FREQ  (0x06)
#define USB_AUDIO_SET_MAX_SAMPLING_FREQ  (0x07)
#define USB_AUDIO_GET_RES_SAMPLING_FREQ  (0x08)
#define USB_AUDIO_SET_RES_SAMPLING_FREQ  (0x09)

#define NUMBER_OF_FEATURE_COMMANDS       (0x2E)
#define NUMBER_OF_ENDPOINT_COMMANDS      (0x0A)

/* audio command struct */
typedef struct {
    uint8_t   control_mask;
    uint8_t   request_type;
    uint8_t   request_code;
    uint8_t   request_value;
    uint8_t   length;
} usb_audio_command_t;


/* Receive interrupt state */
typedef struct {
    uint8_t       status;
    uint8_t		  originator;
} usb_audio_control_status_t;


/* Audio Control Subclass */
/* Header descriptor */
#define USB_DESC_SUBTYPE_AUDIO_CS_HEADER           0x01
typedef struct {
    uint8_t   bfunctionlength;
    uint8_t   bdescriptortype;
    uint8_t   bdescriptorsubtype;
    uint8_t   bcdcdc[2];
    uint8_t	  wtotallength[2];
    uint8_t   bincollection;
} usb_audio_ctrl_desc_header_t;

/* Input Terminal descriptor */
#define USB_DESC_SUBTYPE_AUDIO_CS_IT               0x02
typedef struct {
    uint8_t   bfunctionlength;
    uint8_t   bdescriptortype;
    uint8_t   bdescriptorsubtype;
    uint8_t   bterminalid;
    uint8_t   wterminaltype[2];
    uint8_t   bassocterminal;
    uint8_t   bnrchannels;
    uint8_t   wchannelconfig[2];
    uint8_t   ichannelnames;
    uint8_t   iterminal;
} usb_audio_ctrl_desc_it_t;

/* Output Terminal descriptor */
#define USB_DESC_SUBTYPE_AUDIO_CS_OT               0x03
typedef struct {
    uint8_t   bfunctionlength;
    uint8_t   bdescriptortype;
    uint8_t   bdescriptorsubtype;
    uint8_t   bterminalid;
    uint8_t   wterminaltype[2];
    uint8_t   bassocterminal;
    uint8_t   bsourceid;
    uint8_t   iterminal;
} usb_audio_ctrl_desc_ot_t;

/* Feature Unit descriptor */
#define USB_DESC_SUBTYPE_AUDIO_CS_FU               0x06
typedef struct {
    uint8_t   blength;
    uint8_t   bdescriptortype;
    uint8_t   bdescriptorsubtype;
    uint8_t   bunitid;
    uint8_t   bsourceid;
    uint8_t   bcontrolsize;
#define USB_AUDIO_CTRL_FU_MUTE              0x01
#define USB_AUDIO_CTRL_FU_VOLUME            0x02
#define USB_AUDIO_CTRL_FU_BASS              0x03
#define USB_AUDIO_CTRL_FU_MID               0x04
#define USB_AUDIO_CTRL_FU_TREBLE            0x05
#define USB_AUDIO_CTRL_FU_GRAPHIC_EQ        0x06
#define USB_AUDIO_CTRL_FU_AGC               0x07
#define USB_AUDIO_CTRL_FU_DELAY             0x08
#define USB_AUDIO_CTRL_FU_BASS_BOOST        0x09


#define FU_MUTE_MASK                        0x01
#define FU_VOLUME_MASK                      0x02
#define FU_BASS_MASK                        0x04
#define FU_MID_MASK                         0x08
#define FU_TREBLE_MASK                      0x10
#define FU_GRAPHIC_EQ_MASK                  0x20
#define FU_AGC_MASK                         0x40
#define FU_DELAY_MASK                       0x80
#define FU_BASS_BOOST_MASK                  0x01
    uint8_t 	 bmacontrols[];
    //uint8_t 	 ifeature;
} usb_audio_ctrl_desc_fu_t;

/* Audio Control function descriptor */
typedef union {
    usb_audio_ctrl_desc_header_t         header;
    usb_audio_ctrl_desc_it_t             it;
    usb_audio_ctrl_desc_ot_t             ot;
    usb_audio_ctrl_desc_fu_t             fu;
} usb_audio_ctrl_func_desc_t;

/* Audio control subclass structure */
typedef struct {
	/* Each audio subclass must start with a USB_AUDIO_GENERAL_CLASS struct */
	//  USB_AUDIO_GENERAL_CLASS                   AUDIO_G;
	usb_host_handle                 host_handle; 
	usb_device_instance_handle      dev_handle;
	usb_interface_descriptor_handle intf_handle;
	usb_audio_ctrl_desc_header_t*           header_desc;
	usb_audio_ctrl_desc_it_t*               it_desc;
	usb_audio_ctrl_desc_ot_t*               ot_desc;
	usb_audio_ctrl_desc_fu_t*               fu_desc; 
	/* Only 1 command can be issued at one time */
	uint32_t                                         in_setup;
	tr_callback                                ctrl_callback;
	void*                                       ctrl_param;
	usb_pipe_handle                          interrupt_pipe;
	usb_audio_control_status_t                  interrupt_buffer;

	tr_callback                               interrupt_callback;
	void*                                     interrupt_callback_param;
	/* here we store callback and parameter from higher level */
	tr_callback                               user_callback;
	void*                                     user_param;
	uint8_t                                    ifnum;
} audio_control_struct_t;

/* Audio Stream Subclass */
/* Class-specific Audio stream interface descriptor */
#define USB_DESC_SUBTYPE_AS_CS_GENERAL 		0x01
typedef struct {
    uint8_t   blength;
    uint8_t   bdescriptortype;
    uint8_t   bdescriptorsubtype;
    uint8_t   bterminallink;
    uint8_t	  bdelay;
    uint8_t	  wformattag[2];
} usb_audio_stream_desc_spepific_as_if_t;

/* Format type descriptor */
#define USB_DESC_SUBTYPE_AS_CS_FORMAT_TYPE       0x02
typedef struct {
    uint8_t   blength;
    uint8_t   bdescriptortype;
    uint8_t   bdescriptorsubtype;
    uint8_t   bformattype;
    uint8_t   bnrchannels;
    uint8_t   bsubframesize;
    uint8_t   bbitresolution;
    uint8_t   bsamfreqtype;
    uint8_t   tsamfreq[][3];
} usb_audio_stream_desc_format_type_t;
typedef enum
{
    Sam_Continuous = 0,
    Sam_discrete,
    NONE
} Sam_Freq_Type;

/* Class-specific Isochronous Audio Data Endpoint Descriptor */
#define USB_DESC_CLASS_ENDPOINT_GENERAL          0x01
typedef struct {
    uint8_t   blength;
    uint8_t   bdescriptortype;
    uint8_t   bdescriptorsubtype;
    uint8_t   bmattributes;
    uint8_t   blockdlayunits;
    uint8_t   wlockdelay[2];
} usb_audio_stream_desc_specific_iso_endp_t;


typedef struct {
	uint8_t blength;
	uint8_t bdescriptortype;
	uint8_t bendpointaddress;
	uint8_t bmattributes;
	uint8_t wmaxpacketsize[2];
	uint8_t binterval;
	uint8_t brefresh;
} usb_audio_stream_desc_synch_endp_t;

/* Audio stream interface descriptor */
typedef union {
    usb_audio_stream_desc_spepific_as_if_t         as_general;
    usb_audio_stream_desc_format_type_t            frm_type;
    usb_audio_stream_desc_specific_iso_endp_t      iso_endp_specific;
} usb_audio_stream_func_desc_t;

typedef struct {
    /* Each Audio subclass must start with a USB_AUDIO_GENERAL_CLASS struct */
    usb_host_handle                            host_handle; 
    usb_device_instance_handle                 dev_handle;
    usb_interface_descriptor_handle            intf_handle;
    usb_audio_stream_desc_spepific_as_if_t*    as_itf_desc;
    usb_audio_stream_desc_format_type_t*       frm_type_desc;
    usb_audio_stream_desc_specific_iso_endp_t* iso_endp_spec_desc;
    usb_pipe_handle                             iso_in_pipe;
    usb_pipe_handle                             iso_out_pipe;
    tr_callback                                 recv_callback;
    void*                                       recv_param;
    tr_callback                                 send_callback;
    void*                                       send_param;

#define USB_DATA_DETACH                  0x01
#define USB_DATA_READ_COMPLETE     0x02
#define USB_DATA_READ_PIPE_FREE    0x04
#define USB_DATA_SEND_COMPLETE     0x08
#define USB_DATA_SEND_PIPE_FREE    0x10
    os_event_handle                            stream_event;
    uint8_t                                  iso_ep_num;
} audio_stream_struct_t;

/* Audio Command */
typedef struct {
   	class_handle            class_control_handle;
	class_handle            class_stream_handle;
    tr_callback             callback_fn;
    void*                  callback_param;
} audio_command_t;

#ifdef __cplusplus
extern "C" {
#endif

extern usb_status usb_class_audio_control_init(usb_device_instance_handle, usb_interface_descriptor_handle, class_handle*);
extern usb_status usb_class_audio_stream_init(usb_device_instance_handle, usb_interface_descriptor_handle, class_handle*);

extern usb_status usb_class_audio_control_deinit(class_handle);
extern usb_status usb_class_audio_stream_deinit(class_handle);

extern usb_status usb_class_audio_control_pre_deinit(class_handle);
extern usb_status usb_class_audio_stream_pre_deinit(class_handle);

extern usb_status usb_class_audio_control_recv_data(audio_command_t*  audio_ptr,  uint8_t *  buffer,uint32_t buf_size);

extern usb_status usb_class_audio_control_get_descriptors(usb_device_instance_handle, usb_interface_descriptor_handle, usb_audio_ctrl_desc_header_t* *, usb_audio_ctrl_desc_it_t* *, usb_audio_ctrl_desc_ot_t**, usb_audio_ctrl_desc_fu_t**);
extern usb_status usb_class_audio_stream_get_descriptors(usb_device_instance_handle, usb_interface_descriptor_handle, usb_audio_stream_desc_spepific_as_if_t* *,       usb_audio_stream_desc_format_type_t* *, usb_audio_stream_desc_specific_iso_endp_t* *);
extern usb_status usb_class_audio_stream_set_descriptors(class_handle, usb_audio_stream_desc_spepific_as_if_t*, usb_audio_stream_desc_format_type_t*, usb_audio_stream_desc_specific_iso_endp_t*);
extern usb_status usb_class_audio_control_set_descriptors(class_handle, usb_audio_ctrl_desc_header_t*, usb_audio_ctrl_desc_it_t*, usb_audio_ctrl_desc_ot_t*, usb_audio_ctrl_desc_fu_t*);
extern usb_status usb_class_audio_stream_get_sample_type(usb_device_instance_handle dev_handle,usb_interface_descriptor_handle intf_handle,uint8_t *type);

extern usb_status usb_class_audio_recv_data(audio_command_t*,  uint8_t *, uint32_t);
extern usb_status usb_class_audio_send_data(audio_command_t*,  uint8_t *, uint32_t);

extern usb_status usb_class_audio_get_copy_protect(audio_command_t*,void* );
extern usb_status usb_class_audio_set_copy_protect(audio_command_t*,void* );

extern usb_status usb_class_audio_feature_command(audio_command_t*,uint8_t,void* ,uint32_t);

extern usb_status usb_class_audio_get_cur_graphic_eq(audio_command_t*,uint16_t,uint8_t,void* );
extern usb_status usb_class_audio_set_cur_graphic_eq(audio_command_t*,uint16_t,uint8_t,void* );

extern usb_status usb_class_audio_get_min_graphic_eq(audio_command_t*,uint16_t,uint8_t,void* );
extern usb_status usb_class_audio_set_min_graphic_eq(audio_command_t*,uint16_t,uint8_t,void* );

extern usb_status usb_class_audio_get_max_graphic_eq(audio_command_t*,uint16_t,uint8_t,void* );
extern usb_status usb_class_audio_set_max_graphic_eq(audio_command_t*,uint16_t,uint8_t,void* );

extern usb_status usb_class_audio_get_res_graphic_eq(audio_command_t*,uint16_t,uint8_t,void* );
extern usb_status usb_class_audio_set_res_graphic_eq(audio_command_t*,uint16_t,uint8_t,void* );

extern usb_status usb_class_audio_endpoint_command(audio_command_t*,void* ,uint32_t);

extern usb_status usb_class_audio_get_mem_endpoint(audio_command_t*,uint16_t,uint16_t,void* );
extern usb_status usb_class_audio_set_mem_endpoint(audio_command_t*,uint16_t,uint16_t,void* );

#ifdef __cplusplus
}
#endif

#endif
