/**HEADER********************************************************************
*
* Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989 - 2008 ARC International;
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
* $FileName: ieee11073_phd_types.h$
* $Date    :
* $Version :
*
* Comments:
*
* @brief This file contains definitions of the data types and the structures
*        for the data that appears on the network (specific to IEEE_11073)
*****************************************************************************/
#ifndef PHD_TYPES
#define PHD_TYPES

#include "usb.h"

PACKED_STRUCT_BEGIN
struct _any
{
    uint16_t    length;
    uint8_t     value[1]; /* first element of the array */
}PACKED_STRUCT_END;
typedef struct _any any_t;

typedef uint16_t oid_type_t;

typedef uint16_t private_oid_t;

typedef uint16_t handle_t;

typedef uint16_t inst_number_t;

typedef uint16_t nom_partition_t;

#define NOM_PART_UNSPEC         0
#define NOM_PART_OBJ            1
#define NOM_PART_METRIC         2
#define NOM_PART_ALERT          3
#define NOM_PART_DIM            4
#define NOM_PART_VATTR          5
#define NOM_PART_PGRP           6
#define NOM_PART_SITES          7
#define NOM_PART_INFRASTRUCT    8
#define NOM_PART_FEF            9
#define NOM_PART_ECG_EXTN       10
#define NOM_PART_PHD_DM         128
#define NOM_PART_PHD_HF         129
#define NOM_PART_PHD_AI         130
#define NOM_PART_RET_CODE       255
#define NOM_PART_EXT_NOM        256
#define NOM_PART_PRIV           1024

PACKED_STRUCT_BEGIN
struct _type
{
    nom_partition_t                partition;
    oid_type_t                     code;
} PACKED_STRUCT_END;
typedef struct _type type_t;

PACKED_STRUCT_BEGIN
struct _ava_type
{
    oid_type_t                    attribute_id;
    any_t                         attribute_value;
} PACKED_STRUCT_END ;
typedef struct _ava_type ava_type_t;

PACKED_STRUCT_BEGIN
struct _attribute_list
{
    uint16_t                    count;
    uint16_t                    length;
    ava_type_t                  value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _attribute_list attribute_list_t;

PACKED_STRUCT_BEGIN
struct _attribute_id_list
{
    uint16_t                    count;
    uint16_t                    length;
    oid_type_t                  value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _attribute_id_list attribute_id_list_t;

typedef uint32_t float_type_t;
typedef uint16_t sfloat_type_t;
typedef uint32_t relative_time_t;
PACKED_STRUCT_BEGIN
struct _high_res_relative_time
{
    uint8_t                     value[8];
} PACKED_STRUCT_END ;
typedef struct _high_res_relative_time high_res_relative_time_t;

PACKED_STRUCT_BEGIN
struct _absolute_time_adjust
{
    uint8_t                     value[6];
} PACKED_STRUCT_END ;
typedef struct _absolute_time_adjust absolute_time_adjust_t;

PACKED_STRUCT_BEGIN
struct _absolute_time
{
    uint8_t                     century;
    uint8_t                     year;
    uint8_t                     month;
    uint8_t                     day;
    uint8_t                     hour;
    uint8_t                     minute;
    uint8_t                     second;
    uint8_t                     sec_fractions;
} PACKED_STRUCT_END ;
typedef struct _absolute_time absolute_time_t;

typedef uint16_t operational_state_t;
#define OS_DISABLED             0
#define OS_ENABLED              1
#define OS_NOT_AVAILABLE        2

PACKED_STRUCT_BEGIN
struct _octet_string
{
    uint16_t                    length;
    uint8_t                     value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _octet_string octet_string_t;

PACKED_STRUCT_BEGIN
struct _system_model
{
    octet_string_t                manufacturer;
    octet_string_t                model_number;
} PACKED_STRUCT_END ;
typedef struct _system_model system_model_t;

PACKED_STRUCT_BEGIN
struct _prod_spec_entry
{
    uint16_t                    spec_type;
#define UNSPECIFIED             0
#define SERIAL_NUMBER           1
#define PART_NUMBER             2
#define HW_REVISION             3
#define SW_REVISION             4
#define FW_REVISION             5
#define PROTOCOL_REVISION       6
#define PROD_SPEC_GMDN          7
    private_oid_t                 component_id;
    octet_string_t                prod_spec;
} PACKED_STRUCT_END ;
typedef struct _prod_spec_entry prod_spec_entry_t;

PACKED_STRUCT_BEGIN
struct _production_spec
{
    uint16_t                    count;
    uint16_t                    length;
    prod_spec_entry_t value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _production_spec production_spec_t;

typedef uint16_t power_status_t;

#define ON_MAINS                0x8000
#define ON_BATTERY              0x4000
#define CHARGING_FULL           0x0080
#define CHARGING_TRICKLE        0x0040
#define CHARGING_OFF            0x0020

PACKED_STRUCT_BEGIN
struct _bat_measure
{
    float_type_t                  value;
    oid_type_t                    unit;
} PACKED_STRUCT_END ;
typedef struct _bat_measure bat_measure_t;

typedef uint16_t measurement_status;

#define MS_INVALID              0x8000
#define MS_QUESTIONABLE         0x4000
#define MS_NOT_AVAILABLE        0x2000
#define MS_CALIBRATION_ONGOING  0x1000
#define MS_TEST_DATA            0x0800
#define MS_DEMO_DATA            0x0400
#define MS_VALIDATED_DATA       0x0080
#define MS_EARLY_INDICATION     0x0040
#define MS_MSMT_ONGOING         0x0020

PACKED_STRUCT_BEGIN
struct _nu_obs_value
{
    oid_type_t                   metric_id;
    measurement_status           state;
    oid_type_t                   unit_code;
    float_type_t                 value;
} PACKED_STRUCT_END ;
typedef struct _nu_obs_value nu_obs_value_t;

PACKED_STRUCT_BEGIN
struct _nu_obs_value_cmp
{
    uint16_t                    count;
    uint16_t                    length;
    nu_obs_value_t              value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _nu_obs_value_cmp nu_obs_value_cmp_t;

PACKED_STRUCT_BEGIN
struct _sample_type
{
    uint8_t                     sample_size;
    uint8_t                     significant_bits;
} PACKED_STRUCT_END ;
typedef struct _sample_type sample_type_t;

#define SAMPLE_TYPE_SIGNIFICANT_BITS_SIGNED_SAMPLES 255

typedef uint16_t sa_flags_t;

#define SMOOTH_CURVE            0x8000
#define DELAYED_CURVE           0x4000
#define STATIC_SCALE            0x2000
#define SA_EXT_VAL_RANGE        0x1000

PACKED_STRUCT_BEGIN
struct _sa_spec
{
    uint16_t                    array_size;
    sample_type_t               sample_type;
    sa_flags_t                  flags;
} PACKED_STRUCT_END ;
typedef struct _sa_spec sa_spec_t;

PACKED_STRUCT_BEGIN
struct _scale_range_spec8
{
    float_type_t                lower_absolute_value;
    float_type_t                upper_absolute_value;
    uint8_t                     lower_scaled_value;
    uint8_t                     upper_scaled_value;
} PACKED_STRUCT_END ;
typedef struct _scale_range_spec8 scale_range_spec8_t;

PACKED_STRUCT_BEGIN
struct _scale_range_spec16
{
    float_type_t                lower_absolute_value;
    float_type_t                upper_absolute_value;
    uint16_t                    lower_scaled_value;
    uint16_t                    upper_scaled_value;
} PACKED_STRUCT_END ;
typedef struct _scale_range_spec16 scale_range_spec16_t;

PACKED_STRUCT_BEGIN
struct _scale_range_spec32
{
    float_type_t                lower_absolute_value;
    float_type_t                upper_absolute_value;
    uint32_t                    lower_scaled_value;
    uint32_t                    upper_scaled_value;
} PACKED_STRUCT_END ;
typedef struct _scale_range_spec32 scale_range_spec32_t;


union _scale_range_spec32_union
{
    oid_type_t                  enum_obj_id;
    octet_string_t              enum_text_string;
    uint32_t                    enum_bit_str; /* BITS-32 */
} ;
typedef union _scale_range_spec32_union scale_range_spec32_union_t;

PACKED_STRUCT_BEGIN
struct _enum_val
{
    uint16_t                    choice;
    uint16_t                    length;
#define OBJ_ID_CHOSEN           0x0001
#define TEXT_STRING_CHOSEN      0x0002
#define BIT_STR_CHOSEN          0x0010
    scale_range_spec32_union_t      u;
} PACKED_STRUCT_END ;
typedef struct _enum_val enum_val_t;

PACKED_STRUCT_BEGIN
struct _enum_obs_value
{
    oid_type_t                   metric_id;
    measurement_status           state;
    enum_val_t                   value;
} PACKED_STRUCT_END ;
typedef struct _enum_obs_value enum_obs_value_t;

PACKED_STRUCT_BEGIN
struct _attr_val_map_entry
{
    oid_type_t                  attribute_id;
    uint16_t                    attribute_len;
} PACKED_STRUCT_END ;
typedef struct _attr_val_map_entry attr_val_map_entry_t;

PACKED_STRUCT_BEGIN
struct _attr_val_map
{
    uint16_t                    count;
    uint16_t                    length;
    attr_val_map_entry_t        value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _attr_val_map attr_val_map_t;

PACKED_STRUCT_BEGIN
struct _handle_attr_val_map_entry
{
    handle_t                      obj_handle;
    attr_val_map_t                attr_val_map;
} PACKED_STRUCT_END ;
typedef struct _handle_attr_val_map_entry handle_attr_val_map_entry_t;

typedef uint16_t confirm_mode;

#define UNCONFIRMED             0x0000
#define CONFIRMED               0x0001

PACKED_STRUCT_BEGIN
struct _handle_attr_val_map
{
    uint16_t                    count;
    uint16_t                    length;
    handle_attr_val_map_entry_t       value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _handle_attr_val_map handle_attr_val_map_t;

typedef uint16_t sto_sample_alg_t;

#define ST_ALG_NOS              0x0000
#define ST_ALG_MOVING_AVERAGE   0x0001
#define ST_ALG_RECURSIVE_       0x0002
#define ST_ALG_MIN_PICK         0x0003
#define ST_ALG_MAX_PICK         0x0004
#define ST_ALG_MEDIAN           0x0005
#define ST_ALG_TRENDED          0x0200
#define ST_ALG_NO_DOWNSAMPLING  0x0400

PACKED_STRUCT_BEGIN
struct _set_time_invoke
{
    absolute_time_t               date_time;
    float_type_t                  accuracy;
} PACKED_STRUCT_END ;
typedef struct _set_time_invoke set_time_invoke_t;

PACKED_STRUCT_BEGIN
struct _segm_id_list
{
    uint16_t                    count;
    uint16_t                    length;
    inst_number_t               value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _segm_id_list segm_id_list_t;

PACKED_STRUCT_BEGIN
struct _abs_time_range
{
    absolute_time_t                from_time;
    absolute_time_t                to_time;
} PACKED_STRUCT_END ;
typedef struct _abs_time_range abs_time_range_t;

PACKED_STRUCT_BEGIN
struct _segment_info
{
    inst_number_t                  seg_inst_no;
    attribute_list_t               seg_info;
} PACKED_STRUCT_END ;
typedef struct _segment_info segment_info_t;

PACKED_STRUCT_BEGIN
struct _segment_info_list
{
    uint16_t                    count;
    uint16_t                    length;
    segment_info_t                 value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _segment_info_list segment_info_list_t;


union _segm_selection_union
{
    uint16_t                    all_segments;
    segm_id_list_t              segm_id_list;
    abs_time_range_t            abs_time_range;
} ;
typedef union _segm_selection_union segm_selection_union_t;

PACKED_STRUCT_BEGIN
struct _segm_selection
{
    uint16_t                                choice;
    uint16_t                                length;
#define ALL_SEGMENTS_CHOSEN                 0x0001
#define SEGM_ID_LIST_CHOSEN                 0x0002
#define ABS_TIME_RANGE_CHOSEN               0x0003
    segm_selection_union_t                     u;
} PACKED_STRUCT_END ;
typedef struct _segm_selection segm_selection_t;

typedef uint16_t pm_store_capab_t;

#define PMSC_VAR_NO_OF_SEGM                 0x8000
#define PMSC_EPI_SEG_ENTRIES                0x0800
#define PMSC_PERI_SEG_ENTRIES               0x0400
#define PMSC_ABS_TIME_SELECT                0x0200
#define PMSC_CLEAR_SEGM_BY_LIST_SUP         0x0100
#define PMSC_CLEAR_SEGM_BY_TIME_SUP         0x0080
#define PMSC_CLEAR_SEGM_REMOVE              0x0040
#define PMSC_MULTI_PERSON                   0x0008

typedef uint16_t segm_entry_header_t;

#define SEG_ELEM_HDR_ABSOLUTE_TIME          0x8000
#define SEG_ELEM_HDR_RELATIVE_TIME          0x4000
#define SEG_ELEM_HDR_HIRES_RELATIVE_TIME    0x2000

PACKED_STRUCT_BEGIN
struct _segm_entry_elem
{
    oid_type_t                                class_id;
    type_t                                    metric_type;
    handle_t                                  handle;
    attr_val_map_t                            attr_val_map;
} PACKED_STRUCT_END ;
typedef struct _segm_entry_elem segm_entry_elem_t;

PACKED_STRUCT_BEGIN
struct _segm_entry_elem_list
{
    uint16_t                                count;
    uint16_t                                length;
    segm_entry_elem_t                       value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _segm_entry_elem_list segm_entry_elem_list_t;

PACKED_STRUCT_BEGIN
struct _pm_segment_entry_map
{
    segm_entry_header_t                         segm_entry_header;
    segm_entry_elem_list_t                      segm_entry_elem_list;
} PACKED_STRUCT_END ;
typedef struct _pm_segment_entry_map pm_segment_entry_map_t;

PACKED_STRUCT_BEGIN
struct _segm_elem_static_attr_entry
{
    oid_type_t                                class_id;
    type_t                                    metric_type;
    attribute_list_t                          attribute_list;
} PACKED_STRUCT_END ;
typedef struct _segm_elem_static_attr_entry segm_elem_static_attr_entry_t;

PACKED_STRUCT_BEGIN
struct _pm_segm_elem_static_attrList
{
    uint16_t                                count;
    uint16_t                                length;
    segm_elem_static_attr_entry_t           value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _pm_segm_elem_static_attrList pm_segm_elem_static_attrList_t;

PACKED_STRUCT_BEGIN
struct _trig_segm_sata_xfer_req
{
    inst_number_t                              seg_inst_no;
} PACKED_STRUCT_END ;
typedef struct _trig_segm_sata_xfer_req trig_segm_sata_xfer_req_t;

typedef uint16_t trig_segm_xfer_rsp_t;

#define TSXR_SUCCESSFUL                     0
#define TSXR_FAIL_NO_SUCH_SEGMENT           1
#define TSXR_FAIL_SEGM_TRY_LATER            2
#define TSXR_FAIL_SEGM_EMPTY                3
#define TSXR_FAIL_OTHER                     512

PACKED_STRUCT_BEGIN
struct _trig_segm_data_xfer_rsp
{
    inst_number_t                              seg_inst_no;
    trig_segm_xfer_rsp_t                       trig_segm_xfer_rsp;
} PACKED_STRUCT_END ;
typedef struct _trig_segm_data_xfer_rsp trig_segm_data_xfer_rsp_t;

typedef uint16_t segm_evt_status;

#define SEVTSTA_FIRST_ENTRY                 0x8000
#define SEVTSTA_LAST_ENTRY                  0x4000
#define SEVTSTA_AGENT_ABORT                 0x0800
#define SEVTSTA_MANAGER_CONFIRM             0x0080
#define SEVTSTA_MANAGER_ABORT               0x0008

PACKED_STRUCT_BEGIN
struct _segm_data_event_descr
{
    inst_number_t                           segm_instance;
    uint32_t                                segm_evt_entry_index;
    uint32_t                                segm_evt_entry_count;
    segm_evt_status                         segm_evt;
} PACKED_STRUCT_END ;
typedef struct _segm_data_event_descr segm_data_event_descr_t;

PACKED_STRUCT_BEGIN
struct _segment_data_event
{
    segm_data_event_descr_t                   segm_data_event_descr;
    octet_string_t                            segm_data_event_entries;
} PACKED_STRUCT_END ;
typedef struct _segment_data_event segment_data_event_t;

PACKED_STRUCT_BEGIN
struct _segment_data_result
{
    segm_data_event_descr_t                   segm_data_event_descr;
} PACKED_STRUCT_END ;
typedef struct _segment_data_result segment_data_result_t;

typedef uint16_t segm_stat_type_t;

#define SEGM_STAT_TYPE_MINIMUM              1
#define SEGM_STAT_TYPE_MAXIMUM              2
#define SEGM_STAT_TYPE_AVERAGE              3

PACKED_STRUCT_BEGIN
struct _segment_statistic_entry
{
    segm_stat_type_t                          segm_stat_type;
    octet_string_t                            segm_stat_entry;
} PACKED_STRUCT_END ;
typedef struct _segment_statistic_entry segment_statistic_entry_t;

PACKED_STRUCT_BEGIN
struct _segment_statistics
{
    uint16_t                                count;
    uint16_t                                length;
    segment_statistic_entry_t               value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _segment_statistics segment_statistics_t;

PACKED_STRUCT_BEGIN
struct _observation_scan
{
    handle_t                                  obj_handle;
    attribute_list_t                          attributes;
} PACKED_STRUCT_END ;
typedef struct _observation_scan observation_scan_t;

typedef oid_type_t time_protocol_id_t;

typedef uint32_t association_version_t;
#define ASSOC_VERSION1                      0x80000000

typedef uint32_t protocol_version_t;
#define PROTOCOL_VERSION1                   0x80000000

typedef uint16_t encoding_rules_t;
#define MDER                                0x8000
#define XER                                 0x4000
#define PER                                 0x2000

PACKED_STRUCT_BEGIN
struct _uuid_ident
{
    uint8_t value[16];
} PACKED_STRUCT_END ;
typedef struct _uuid_ident uuid_ident_t;

typedef uint16_t data_proto_id_t;
#define DATA_PROTO_ID_20601                 20601
#define DATA_PROTO_ID_EXTERNAL              65535

PACKED_STRUCT_BEGIN
struct _data_proto
{
    data_proto_id_t                           data_proto_id;
    any_t                                     data_proto_info;
} PACKED_STRUCT_END ;
typedef struct _data_proto data_proto_t;

PACKED_STRUCT_BEGIN
struct _data_proto_list
{
    uint16_t                                count;
    uint16_t                                length;
    data_proto_t                            value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _data_proto_list data_proto_list_t;

PACKED_STRUCT_BEGIN
struct _aarq_apdu
{
    association_version_t                      assoc_version;
    data_proto_list_t                          data_proto_list;
} PACKED_STRUCT_END ;
typedef struct _aarq_apdu aarq_apdu_t;

typedef uint16_t associate_result_t;
#define ACCEPTED                            0
#define REJECTED_PERMANENT                  1
#define REJECTED_TRANSIENT                  2
#define ACCEPTED_UNKNOWN_CONFIG             3
#define REJECTED_NO_COMMON_PROTOCOL         4
#define REJECTED_NO_COMMON_PARAMETER        5
#define REJECTED_UNKNOWN                    6
#define REJECTED_UNAUTHORIZED               7
#define REJECTED_UNSUPPORTED_ASSOC_VERSION  8

PACKED_STRUCT_BEGIN
struct _aare_apdu
{
    associate_result_t                        result;
    data_proto_t                              selected_data_proto;
} PACKED_STRUCT_END ;
typedef struct _aare_apdu aare_apdu_t;

typedef uint16_t release_request_reason_t;
#define RELEASE_REQUEST_REASON_NORMAL       0

PACKED_STRUCT_BEGIN
struct _rlrq_apdu
{
    release_request_reason_t                  reason;
} PACKED_STRUCT_END ;
typedef struct _rlrq_apdu rlrq_apdu_t;

typedef uint16_t release_response_reason_t;
#define RELEASE_RESPONSE_REASON_NORMAL      0

PACKED_STRUCT_BEGIN
struct _rlre_apdu
{
    release_response_reason_t                 reason;
} PACKED_STRUCT_END ;
typedef struct _rlre_apdu rlre_apdu_t;

typedef uint16_t abort_reason_t;
#define ABORT_REASON_UNDEFINED              0
#define ABORT_REASON_BUFFER_OVERFLOW        1
#define ABORT_REASON_RESPONSE_TIMEOUT       2
#define ABORT_REASON_CONFIGURATION_TIMEOUT  3

PACKED_STRUCT_BEGIN
struct _abrt_apdu
{
    abort_reason_t                            reason;
}  PACKED_STRUCT_END ;
typedef struct _abrt_apdu abrt_apdu_t;

typedef octet_string_t prst_apdu_t;
typedef uint16_t invoke_id_type_t;

PACKED_STRUCT_BEGIN
struct _event_report_argument_simple
{
    handle_t                                  obj_handle;
    relative_time_t                           event_time;
    oid_type_t                                event_type;
    any_t                                     event_info;
} PACKED_STRUCT_END  ;
typedef struct _event_report_argument_simple event_report_argument_simple_t;

PACKED_STRUCT_BEGIN
struct _get_argument_simple
{
    handle_t                                  obj_handle;
    attribute_id_list_t                       attribute_id_list;
} PACKED_STRUCT_END ;
typedef struct _get_argument_simple get_argument_simple_t;

typedef uint16_t modify_operator_t;
#define REPLACE                             0
#define ADD_VALUES                          1
#define REMOVE_VALUES                       2
#define SET_TO_DEFAULT                      3

PACKED_STRUCT_BEGIN
struct _attribute_mod_entry
{
    modify_operator_t                         modify_operator;
    ava_type_t                                attribute;
} PACKED_STRUCT_END ;
typedef struct _attribute_mod_entry attribute_mod_entry_t;

PACKED_STRUCT_BEGIN
struct _modification_list
{
    uint16_t                                count;
    uint16_t                                length;
    attribute_mod_entry_t                   value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _modification_list modification_list_t;

PACKED_STRUCT_BEGIN
struct _set_argument_simple
{
    handle_t                                  obj_handle;
    modification_list_t                       modification_list;
} PACKED_STRUCT_END ;
typedef struct _set_argument_simple set_argument_simple_t;

PACKED_STRUCT_BEGIN
struct _action_argument_simple
{
    handle_t                                  obj_handle;
    oid_type_t                                action_type;
    any_t                                     action_info_args;
} PACKED_STRUCT_END ;
typedef struct _action_argument_simple action_argument_simple_t;

PACKED_STRUCT_BEGIN
struct _event_report_result_simple
{
    handle_t                                  obj_handle;
    relative_time_t                           currentTime;
    oid_type_t                                event_type;
    any_t                                     event_reply_info;
} PACKED_STRUCT_END;
typedef struct _event_report_result_simple event_report_result_simple_t;

PACKED_STRUCT_BEGIN
struct _get_result_simple
{
    handle_t                                  obj_handle;
    attribute_list_t                           attribute_list;
} PACKED_STRUCT_END ;
typedef struct _get_result_simple get_result_simple_t;

PACKED_STRUCT_BEGIN
struct _type_ver
{
    oid_type_t                              type;
    uint16_t                                version;
} PACKED_STRUCT_END ;
typedef struct _type_ver type_ver_t;

PACKED_STRUCT_BEGIN
struct _type_ver_list
{
    uint16_t                                count;
    uint16_t                                length;
    type_ver_t                              value[1]; /* first element of the array */
} PACKED_STRUCT_END ;
typedef struct _type_ver_list type_ver_list_t;

PACKED_STRUCT_BEGIN
struct _set_result_simple
{
    handle_t                                  obj_handle;
    attribute_list_t                          attribute_list;
} PACKED_STRUCT_END ;
typedef struct _set_result_simple set_result_simple_t;

PACKED_STRUCT_BEGIN
struct _action_result_simple
{
    handle_t                                  obj_handle;
    oid_type_t                                action_type;
    any_t                                     action_info_args;
} PACKED_STRUCT_END ;
typedef struct _action_result_simple action_result_simple_t;

typedef uint16_t error_t;
#define NO_SUCH_OBJECT_INSTANCE             1
#define ACCESS_DENIED                       2
#define NO_SUCH_ACTION                      9
#define INVALID_OBJECT_INSTANCE             17
#define PROTOCOL_VIOLATION                  23
#define NOT_ALLOWED_BY_OBJECT               24
#define ACTION_TIMED_OUT                    25
#define ACTION_ABORTED                      26

PACKED_STRUCT_BEGIN
struct _error_result
{
    error_t                                   error_value;
    any_t                                     parameter;
} PACKED_STRUCT_END ;
typedef struct _error_result error_result_t;

typedef uint16_t rorj_problem_t;
#define UNRECOGNIZED_APDU                   0
#define BADLY_STRUCTURED_APDU               2
#define UNRECOGNIZED_OPERATION              101
#define RESOURCE_LIMITATION                 103
#define UNEXPECTED_ERROR                    303

PACKED_STRUCT_BEGIN
struct _reject_result
{
    rorj_problem_t                             problem;
} PACKED_STRUCT_END ;
typedef struct _reject_result reject_result_t;

PACKED_UNION_BEGIN
union _data_apdu_union
{
    event_report_argument_simple_t              roiv_cmipEventReport;
    event_report_argument_simple_t              roiv_cmipConfirmedEventReport;
    get_argument_simple_t                       roiv_cmipGet;
    set_argument_simple_t                       roiv_cmipSet;
    set_argument_simple_t                       roiv_cmipConfirmedSet;
    action_argument_simple_t                    roiv_cmipAction;
    action_argument_simple_t                    roiv_cmipConfirmedAction;
    event_report_result_simple_t                rors_cmipConfirmedEventReport;
    get_result_simple_t                         rors_cmipGet;
    set_result_simple_t                         rors_cmipConfirmedSet;
    action_result_simple_t                      rors_cmipConfirmedAction;
    error_result_t                             roer;
    reject_result_t                            rorj;
} PACKED_UNION_END ;
typedef union _data_apdu_union data_apdu_union_t;

PACKED_STRUCT_BEGIN
struct _data_apdu_struct
{
    uint16_t                                choice;
    uint16_t                                length;
#define ROIV_CMIP_EVENT_REPORT_CHOSEN           0x0100
#define ROIV_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN 0x0101
#define ROIV_CMIP_GET_CHOSEN                    0x0103
#define ROIV_CMIP_SET_CHOSEN                    0x0104
#define ROIV_CMIP_CONFIRMED_SET_CHOSEN          0x0105
#define ROIV_CMIP_ACTION_CHOSEN                 0x0106
#define ROIV_CMIP_CONFIRMED_ACTION_CHOSEN       0x0107
#define RORS_CMIP_CONFIRMED_EVENT_REPORT_CHOSEN 0x0201
#define RORS_CMIP_GET_CHOSEN                    0x0203
#define RORS_CMIP_CONFIRMED_SET_CHOSEN          0x0205
#define RORS_CMIP_CONFIRMED_ACTION_CHOSEN       0x0207
#define ROER_CHOSEN                             0x0300
#define RORJ_CHOSEN                             0x0400
    data_apdu_union_t                         u;
} PACKED_STRUCT_END ;
typedef struct _data_apdu_struct data_apdu_struct_t;

PACKED_STRUCT_BEGIN
struct _data_apdu
{
    invoke_id_type_t                          invoke_id;
    data_apdu_struct_t                        choice;
} PACKED_STRUCT_END ;
typedef struct _data_apdu data_apdu_t;

PACKED_UNION_BEGIN
union _apdu_union
{
    aarq_apdu_t                               aarq;
    aare_apdu_t                               aare;
    rlrq_apdu_t                               rlrq;
    rlre_apdu_t                               rlre;
    abrt_apdu_t                               abrt;
    prst_apdu_t                               prst;
} PACKED_UNION_END ;
typedef union _apdu_union apdu_union_t;

PACKED_STRUCT_BEGIN
struct _apdu
{
    uint16_t                                choice;
    uint16_t                                length;
#define AARQ_CHOSEN                         0xE200
#define AARE_CHOSEN                         0xE300
#define RLRQ_CHOSEN                         0xE400
#define RLRE_CHOSEN                         0xE500
#define ABRT_CHOSEN                         0xE600
#define PRST_CHOSEN                         0xE700
    apdu_union_t                              u;
} PACKED_STRUCT_END ;
typedef struct _apdu apdu_t;

typedef uint32_t nomenclature_version_t;
#define NOM_VERSION1 0x80000000

typedef uint32_t functional_units_t;
#define FUN_UNITS_UNIDIRECTIONAL            0x80000000
#define FUN_UNITS_HAVETESTCAP               0x40000000
#define FUN_UNITS_CREATETESTASSOC           0x20000000

typedef uint32_t system_type_t;
#define SYS_TYPE_MANAGER                    0x80000000
#define SYS_TYPE_AGENT                      0x00800000

typedef uint16_t config_id_t;
#define MANAGER_CONFIG_RESPONSE             0x0000
#define STANDARD_CONFIG_START               0x0001
#define STANDARD_CONFIG_END                 0x3FFF
#define EXTENDED_CONFIG_START               0x05DC
#define EXTENDED_CONFIG_END                 0x7FFF
#define RESERVED_START                      0x8000
#define RESERVED_END                        0xFFFF

typedef uint16_t data_req_mode_flags_t;
#define DATA_REQ_SUPP_STOP                  0x8000
#define DATA_REQ_SUPP_SCOPE_ALL             0x0800
#define DATA_REQ_SUPP_SCOPE_CLASS           0x0400
#define DATA_REQ_SUPP_SCOPE_HANDLE          0x0200
#define DATA_REQ_SUPP_MODE_SINGLE_RSP       0x0080
#define DATA_REQ_SUPP_MODE_TIME_PERIOD      0x0040
#define DATA_REQ_SUPP_MODE_TIME_NO_LIMIT    0x0020
#define DATA_REQ_SUPP_PERSON_ID             0x0010
#define DATA_REQ_SUPP_INIT_AGENT            0x0001

struct _data_req_mode_capab
{
    data_req_mode_flags_t                   data_req_mode_flags;
    uint8_t                                 data_req_init_agent_count;
    uint8_t                                 data_req_init_manager_count;
};
typedef struct _data_req_mode_capab data_req_mode_capab_t;

struct _phd_association_information
{
    protocol_version_t                      protocolVersion;
    encoding_rules_t                        encodingRules;
    nomenclature_version_t                  nomenclatureVersion;
    functional_units_t                      functionalUnits;
    system_type_t                           systemType;
    octet_string_t                          system_id;
    uint16_t                                dev_config_id;
    data_req_mode_capab_t                   data_req_mode_capab;
    attribute_list_t                        optionList;
};
typedef struct _phd_association_information phd_association_information_t;

struct _manuf_spec_association_information
{
    uuid_ident_t                              data_proto_id_ext;
    any_t                                     data_proto_info_ext;
};
typedef struct _manuf_spec_association_information manuf_spec_association_information_t;

typedef uint16_t mds_time_cap_state_t;
#define MDS_TIME_CAPAB_REAL_TIME_CLOCK              0x8000
#define MDS_TIME_CAPAB_SET_CLOCK                    0x4000
#define MDS_TIME_CAPAB_RELATIVE_TIME                0x2000
#define MDS_TIME_CAPAB_HIGH_RES_RELATIVE_TIME       0x1000
#define MDS_TIME_CAPAB_SYNC_ABS_TIME                0x0800
#define MDS_TIME_CAPAB_SYNC_REL_TIME                0x0400
#define MDS_TIME_CAPAB_SYNC_HI_RES_RELATIVE_TIME    0x0200
#define MDS_TIME_STATE_ABS_TIME_SYNCED              0x0080
#define MDS_TIME_STATE_REL_TIME_SYNCED              0x0040
#define MDS_TIME_STATE_HI_RES_RELATIVE_TIME_SYNCED  0x0020
#define MDS_TIME_MGR_SET_TIME                       0x0010

struct _mds_time_info
{
    mds_time_cap_state_t                            mds_time_cap_state;
    time_protocol_id_t                              time_sync_protocol;
    relative_time_t                                 time_sync_accuracy;
    uint16_t                                        time_resolution_abs_time;
    uint16_t                                        time_resolution_rel_time;
    uint32_t                                        time_resolution_high_res_time;
};
typedef struct _mds_time_info mds_time_info_t;

typedef octet_string_t enum_printable_string_t;

typedef uint16_t person_id_t;
#define UNKNOWN_PERSON_ID                           0xFFFF

typedef uint16_t metric_spec_small_t;
#define MSS_AVAIL_INTERMITTENT                      0x8000
#define MSS_AVAIL_STORED_DATA                       0x4000
#define MSS_UPD_APERIODIC                           0x2000
#define MSS_MSMT_APERIODIC                          0x1000
#define MSS_MSMT_PHYS_EV_ID                         0x0800
#define MSS_MSMT_BTB_METRIC                         0x0400
#define MSS_ACC_MANAGER_INITIATED                   0x0080
#define MSS_ACC_AGENT_INITIATED                     0x0040
#define MSS_CAT_MANUAL                              0x0008
#define MSS_CAT_SETTING                             0x0004
#define MSS_CAT_CALCULATION                         0x0002

struct _metric_structure_small
{
    uint8_t                                     ms_struct;
#define MS_STRUCT_SIMPLE                            0
#define MS_STRUCT_COMPOUND                          1
#define MS_STRUCT_RESERVED                          2
#define MS_STRUCT_COMPOUND_FIX                      3
    uint8_t                                     ms_comp_no;
};
typedef struct _metric_structure_small metric_structure_small_t;

struct _metric_id_list
{
    uint16_t                                    count;
    uint16_t                                    length;
    oid_type_t                                  value[1]; /* first element of the array */
};
typedef struct _metric_id_list metric_id_list_t;

struct _supplemental_type_list
{
    uint16_t                                    count;
    uint16_t                                    length;
    type_t                                      value[1]; /* first element of the array */
};
typedef struct _supplemental_type_list supplemental_type_list_t;


struct _observation_scan_list
{
    uint16_t                                    count;
    uint16_t                                    length;
    observation_scan_t                          value[1]; /* first element of the array */
};
typedef struct _observation_scan_list observation_scan_list_t;

struct _scan_report_per_var
{
    uint16_t                                    person_id;
    observation_scan_list_t                     obs_scan_var;
};
typedef struct _scan_report_per_var scan_report_per_var_t;

struct _scan_report_per_var_list
{
    uint16_t                                    count;
    uint16_t                                    length;
    scan_report_per_var_t                       value[1]; /* first element of the array */
};
typedef struct _scan_report_per_var_list scan_report_per_var_list_t;

typedef uint16_t data_req_id_t;
#define DATA_REQ_ID_MANAGER_INITIATED_MIN       0x0000
#define DATA_REQ_ID_MANAGER_INITIATED_MAX       0xEFFF
#define DATA_REQ_ID_AGENT_INITIATED             0xF000

struct _scan_report_info_mp_var
{
    data_req_id_t                               data_req_id;
    uint16_t                                    scan_report_no;
    scan_report_per_var_t                       scan_per_var;
};
typedef struct _scan_report_info_mp_var scan_report_info_mp_var_t;

struct _observation_scan_fixed
{
    handle_t                                      obj_handle;
    octet_string_t                                obs_val_data;
};
typedef struct _observation_scan_fixed observation_scan_fixed_t;

struct _observation_scan_fixed_list
{
    uint16_t                                    count;
    uint16_t                                    length;
    observation_scan_fixed_t                    value[1]; /* first element of the array */
};
typedef struct _observation_scan_fixed_list observation_scan_fixed_list_t;

struct _scan_report_per_fixed
{
    uint16_t                                    person_id;
    observation_scan_fixed_list_t               obs_scan_fix;
};
typedef struct _scan_report_per_fixed scan_report_per_fixed_t;

struct _scan_report_per_fixed_list
{
    uint16_t                                    count;
    uint16_t                                    length;
    scan_report_per_fixed_t                     value[1]; /* first element of the array */
};
typedef struct _scan_report_per_fixed_list scan_report_per_fixed_list_t;

struct _scan_report_info_mp_fixed
{
    data_req_id_t                               data_req_id;
    uint16_t                                    scan_report_no;
    scan_report_per_fixed_list_t                scan_per_fixed;
};
typedef struct _scan_report_info_mp_fixed scan_report_info_mp_fixed_t;

struct _scan_report_info_var
{
    data_req_id_t                               data_req_id;
    uint16_t                                    scan_report_no;
    observation_scan_list_t                     obs_scan_var;
};
typedef struct _scan_report_info_var scan_report_info_var_t;

struct _scan_report_info_fixed
{
    data_req_id_t                               data_req_id;
    uint16_t                                    scan_report_no;
    observation_scan_fixed_list_t               obs_scan_fixed;
};
typedef struct _scan_report_info_fixed scan_report_info_fixed_t;

typedef octet_string_t observation_scan_grouped_t;

struct _scan_report_info_grouped_list
{
    uint16_t                                    count;
    uint16_t                                    length;
    observation_scan_grouped_t                  value[1]; /* first element of the array */
};
typedef struct _scan_report_info_grouped_list scan_report_info_grouped_list_t;

struct _scan_report_info_grouped
{
    uint16_t                                    data_req_id;
    uint16_t                                    scan_report_no;
    scan_report_info_grouped_list_t             obs_scan_grouped;
};
typedef struct _scan_report_info_grouped scan_report_info_grouped_t;

struct _scan_report_per_grouped
{
    person_id_t                                  person_id;
    observation_scan_grouped_t                   obs_scan_grouped;
};
typedef struct _scan_report_per_grouped scan_report_per_grouped_t;

struct _scan_report_per_grouped_list
{
    uint16_t                                    count;
    uint16_t                                    length;
    scan_report_per_grouped_t                   value[1]; /* first element of the array */
};
typedef struct _scan_report_per_grouped_list scan_report_per_grouped_list_t;

struct _scan_report_info_mp_grouped
{
    uint16_t                                    data_req_id;
    uint16_t                                    scan_report_no;
    scan_report_per_grouped_list_t              scan_per_grouped;
};
typedef struct _scan_report_info_mp_grouped scan_report_info_mp_grouped_t;

struct _config_object
{
    oid_type_t                                    obj_class;
    handle_t                                      obj_handle;
    attribute_list_t                              attributes;
};
typedef struct _config_object config_object_t;

struct _config_object_list
{
    uint16_t                                    count;
    uint16_t                                    length;
    config_object_t                             value[1]; /* first element of the array */
};
typedef struct _config_object_list config_object_list_t;

struct _config_report
{
    config_id_t                                    config_report_id;
    config_object_list_t                           config_obj_list;
};
typedef struct _config_report config_report_t;

typedef uint16_t config_result_t;
#define ACCEPTED_CONFIG                         0x0000
#define UNSUPPORTED_CONFIG                      0x0001
#define STANDARD_CONFIG_UNKNOWN                 0x0002

struct _config_report_rsp
{
    config_id_t                                    config_report_id;
    config_result_t                                config_result;
};
typedef struct _config_report_rsp config_report_rsp_t;

typedef uint16_t data_req_mode_t;
#define DATA_REQ_START_STOP                     0x8000
#define DATA_REQ_CONTINUATION                   0x4000
#define DATA_REQ_SCOPE_ALL                      0x0800
#define DATA_REQ_SCOPE_TYPE                     0x0400
#define DATA_REQ_SCOPE_HANDLE                   0x0200
#define DATA_REQ_MODE_SINGLE_RSP                0x0080
#define DATA_REQ_MODE_TIME_PERIOD               0x0040
#define DATA_REQ_MODE_TIME_NO_LIMIT             0x0020
#define DATA_REQ_MODE_DATA_REQ_PERSON_ID        0x0008

struct _handle_list
{
    uint16_t                                    count;
    uint16_t                                    length;
    handle_t                                    value[1]; /* first element of the array */
};
typedef struct _handle_list handle_list_t;

struct _data_request
{
    data_req_id_t                               data_req_id;
    data_req_mode_t                             data_req_mode;
    relative_time_t                             data_req_time;
    uint16_t                                    data_req_person_id;
    oid_type_t                                  data_req_class;
    handle_list_t                               data_req_obj_handle_list;
};
typedef struct _data_request data_request_t;

typedef uint16_t data_req_result_t;
#define DATA_REQ_RESULT_NO_ERROR                        0
#define DATA_REQ_RESULT_UNSPECIFIC_ERROR                1
#define DATA_REQ_RESULT_NO_STOP_SUPPORT                 2
#define DATA_REQ_RESULT_NO_SCOPE_ALL_SUPPORT            3
#define DATA_REQ_RESULT_NO_SCOPE_CLASS_SUPPORT          4
#define DATA_REQ_RESULT_NO_SCOPE_HANDLE_SUPPORT         5
#define DATA_REQ_RESULT_NO_MODE_SINGLE_RSP_SUPPORT      6
#define DATA_REQ_RESULT_NO_MODE_TIME_PERIOD_SUPPORT     7
#define DATA_REQ_RESULT_NO_MODE_TIME_NO_LIMIT_SUPPORT   8
#define DATA_REQ_RESULT_NO_PERSON_ID_SUPPORT            9
#define DATA_REQ_RESULT_UNKNOWN_PERSON_ID               11
#define DATA_REQ_RESULT_UNKNOWN_CLASS                   12
#define DATA_REQ_RESULT_UNKNOWN_HANDLE                  13
#define DATA_REQ_RESULT_UNSUPP_SCOPE                    14
#define DATA_REQ_RESULT_UNSUPP_MODE                     15
#define DATA_REQ_RESULT_INIT_MANAGER_OVERFLOW           16
#define DATA_REQ_RESULT_CONTINUATION_NOT_SUPPORTED      17
#define DATA_REQ_RESULT_INVALID_REQ_ID                  18

struct _data_response
{
    relative_time_t       rel_time_stamp;
    data_req_result_t     data_req_result;
    oid_type_t            event_type;
    any_t                 event_info;
};
typedef struct _data_response data_response_t;

typedef float_type_t simple_nu_obs_value_t;

struct _simple_nu_obs_value_cmp
{
    uint16_t            count;
    uint16_t            length;
    simple_nu_obs_value_t    value[1]; /* first element of the array */
};
typedef struct _simple_nu_obs_value_cmp simple_nu_obs_value_cmp_t;

typedef sfloat_type_t basic_nu_obs_value_t;

struct _basic_nu_obs_value_cmp
{
    uint16_t                 count;
    uint16_t                 length;
    basic_nu_obs_value_t     value[1]; /* first element of the array */
};
typedef struct _basic_nu_obs_value_cmp basic_nu_obs_value_cmp_t;

#endif /* PHD_TYPES */
