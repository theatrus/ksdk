/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
***************************************************************************//*!
*
* @file     SWLIBS_Typedefs.h
*
* @version  1.0.1.0
*
* @date     Oct-9-2013
*
* @brief    Basic types definition file.
*
*******************************************************************************
*
******************************************************************************/

/**
* @page misra_violations MISRA-C:2004 violations
*
* @section SWLIBS_Typedefs_h_REF_1
* Violates MISRA 2004 Required Rule 20.2, Re-use of C90 identifier.
* NULL symbol is redefined only in case, that null pointer is not defined by compiler.
*
* @section SWLIBS_Typedefs_h_REF_2
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*/

#ifndef SWLIBS_TYPEDEFS_H
#define SWLIBS_TYPEDEFS_H

#include "SWLIBS_Config.h"
#include "SWLIBS_MacroDisp.h"

/******************************************************************************
* Implementation configuration checking            (scope: module-global)
******************************************************************************/
#ifndef _MATLAB_BAM_CREATE
  #if((SWLIBS_DEFAULT_IMPLEMENTATION != SWLIBS_DEFAULT_IMPLEMENTATION_F32)&& \
      (SWLIBS_DEFAULT_IMPLEMENTATION != SWLIBS_DEFAULT_IMPLEMENTATION_F16)&& \
      (SWLIBS_DEFAULT_IMPLEMENTATION != SWLIBS_DEFAULT_IMPLEMENTATION_FLT))
    #error "Define default implementation in SWLIBS_Config.h file."
  #endif

  #if((SWLIBS_SUPPORT_F32 == SWLIBS_STD_OFF)&&(SWLIBS_SUPPORT_F16 == SWLIBS_STD_OFF)&&(SWLIBS_SUPPORT_FLT == SWLIBS_STD_OFF))
    #error "Define at least one supported implementation in SWLIBS_Config.h file."
  #endif

  #if((SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)&&(SWLIBS_SUPPORT_F32 == SWLIBS_STD_OFF))
    #error "Set the SWLIBS_SUPPORT_F32 to SWLIBS_STD_ON in SWLIBS_Config.h file to support FRAC32 default implementation."
  #endif

  #if((SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)&&(SWLIBS_SUPPORT_F16 == SWLIBS_STD_OFF))
    #error "Set the SWLIBS_SUPPORT_F16 to SWLIBS_STD_ON in SWLIBS_Config.h file to support FRAC16 default implementation."
  #endif

  #if((SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_FLT)&&(SWLIBS_SUPPORT_FLT == SWLIBS_STD_OFF))
    #error "Set the SWLIBS_SUPPORT_FLT to SWLIBS_STD_ON in SWLIBS_Config.h file to support FLOAT default implementation."
  #endif
#endif

/******************************************************************************
* Defines and macros            (scope: module-local)
******************************************************************************/
#ifndef NULL
  #ifdef __cplusplus
    #define NULL ((void*)0)
  #else
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_1 MISRA 2004 Required Rule 20.2, Re-use of C90
    * identifier.
    */
    #define NULL 0
  #endif
#endif /* NULL */


/******************************************************************************
* Defines and macros            (scope: module-global)
******************************************************************************/


/******************************************************************************
* Typedefs and structures       (scope: module-local)
******************************************************************************/
typedef unsigned char       bool;          /*!< basic boolean type */

#ifndef FALSE
#define FALSE ((bool)0)                    /*!< Boolean type FALSE constant */
#endif

#ifndef TRUE
#define TRUE ((bool)1)                     /*!< Boolean type TRUE constant */
#endif

typedef unsigned char       UWord8;            /*!< unsigned 8-bit integer type */
typedef signed char         Word8;            /*!< signed 8-bit integer type */
typedef unsigned short      UWord16;           /*!< unsigned 16-bit integer type */
typedef signed short        Word16;           /*!< signed 16-bit integer type */
typedef unsigned long       UWord32;           /*!< unsigned 32-bit integer type */
typedef signed long         Word32;           /*!< signed 32-bit integer type */
typedef signed long long    Word64;           /*!< signed 64-bit integer type */
typedef Word16                Frac16;        /*!< 16-bit signed fractional Q1.15 type */
typedef Word32                Frac32;        /*!< 32-bit Q1.31 type */
typedef float               Float;         /*!< single precision float type */

typedef unsigned char UInt8;          /*!< unsigned 8-bit integer type */
typedef signed char Int8;             /*!< signed 8-bit integer type */
typedef unsigned short UInt16;        /*!< unsigned 16-bit integer type */
typedef signed short Int16;           /*!< signed 16-bit integer type */
typedef unsigned int UInt32;          /*!< unsigned 32-bit integer type */
typedef signed int Int32;             /*!< signed 32-bit integer type */
typedef unsigned long long UInt64;    /*!< unsigned 64-bit integer type */
typedef signed long long Int64;       /*!< signed 64-bit integer type */

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef _MATLAB_BAM_CREATE
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition
    * for macro.
    */
    #define MCLIB_2_COOR_SYST_D_Q_T    MCLIB_2_COOR_SYST_D_Q_T_F32                        /*!< Definition of MCLIB_2_COOR_SYST_D_Q_T as alias for MCLIB_2_COOR_SYST_D_Q_T_F32 array in case the 32-bit fractional implementation is selected. */
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition
    * for macro.
    */
    #define MCLIB_ANGLE_T    MCLIB_ANGLE_T_F32                        /*!< Definition of MCLIB_ANGLE_T as alias for MCLIB_ANGLE_T_F32 array in case the 32-bit fractional implementation is selected. */
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition
    * for macro.
    */
    #define MCLIB_2_COOR_SYST_ALPHA_BETA_T    MCLIB_2_COOR_SYST_ALPHA_BETA_T_F32                        /*!< Definition of MCLIB_2_COOR_SYST_ALPHA_BETA_T as alias for MCLIB_2_COOR_SYST_ALPHA_BETA_T_F32 array in case the 32-bit fractional implementation is selected. */
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition
    * for macro.
    */
    #define MCLIB_3_COOR_SYST_T    MCLIB_3_COOR_SYST_T_F32                        /*!< Definition of MCLIB_3_COOR_SYST_T as alias for MCLIB_3_COOR_SYST_T_F32 array in case the 32-bit fractional implementation is selected. */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition
    * for macro.
    */
    #define MCLIB_2_COOR_SYST_D_Q_T    MCLIB_2_COOR_SYST_D_Q_T_F16                        /*!< Definition of MCLIB_2_COOR_SYST_D_Q_T as alias for MCLIB_2_COOR_SYST_D_Q_T_F16 array in case the 16-bit fractional implementation is selected. */
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition
    * for macro.
    */
    #define MCLIB_ANGLE_T    MCLIB_ANGLE_T_F16                        /*!< Definition of MCLIB_ANGLE_T as alias for MCLIB_ANGLE_T_F16 array in case the 16-bit fractional implementation is selected. */
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition
    * for macro.
    */
    #define MCLIB_2_COOR_SYST_ALPHA_BETA_T    MCLIB_2_COOR_SYST_ALPHA_BETA_T_F16                        /*!< Definition of MCLIB_2_COOR_SYST_ALPHA_BETA_T as alias for MCLIB_2_COOR_SYST_ALPHA_BETA_T_F16 array in case the 16-bit fractional implementation is selected. */
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition
    * for macro.
    */
    #define MCLIB_3_COOR_SYST_T    MCLIB_3_COOR_SYST_T_F16                        /*!< Definition of MCLIB_3_COOR_SYST_T as alias for MCLIB_3_COOR_SYST_T_F16 array in case the 16-bit fractional implementation is selected. */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_FLT)
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition
    * for macro.
    */
    #define MCLIB_2_COOR_SYST_D_Q_T    MCLIB_2_COOR_SYST_D_Q_T_FLT                        /*!< Definition of MCLIB_2_COOR_SYST_D_Q_T as alias for MCLIB_2_COOR_SYST_D_Q_T_FLT array in case the single precision floating point implementation is selected. */
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition
    * for macro.
    */
    #define MCLIB_ANGLE_T    MCLIB_ANGLE_T_FLT                        /*!< Definition of MCLIB_ANGLE_T as alias for MCLIB_ANGLE_T_FLT array in case the single precision floating point implementation is selected. */
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition
    * for macro.
    */
    #define MCLIB_2_COOR_SYST_ALPHA_BETA_T    MCLIB_2_COOR_SYST_ALPHA_BETA_T_FLT                        /*!< Definition of MCLIB_2_COOR_SYST_ALPHA_BETA_T as alias for MCLIB_2_COOR_SYST_ALPHA_BETA_T_FLT array in case the single precision floating point implementation is selected. */
    /*
    * @violates @ref SWLIBS_Typedefs_h_REF_2 MISRA 2004 Required Rule 19.4, Disallowed definition
    * for macro.
    */
    #define MCLIB_3_COOR_SYST_T    MCLIB_3_COOR_SYST_T_FLT                        /*!< Definition of MCLIB_3_COOR_SYST_T as alias for MCLIB_3_COOR_SYST_T_FLT array in case the single precision floating point implementation is selected. */
  #endif
#endif /* _MATLAB_BAM_CREATE */


#if (SWLIBS_SUPPORT_F32 == SWLIBS_STD_ON)
/************************************************************************//*!
@struct MCLIB_3_COOR_SYST_T_F32 "\SWLIBS_Typedefs.h"

@brief  Three phase coordinate system (32-bit fractional).
*//*************************************************************************/
typedef struct
{
    Frac32 f32A; /*!< A component, type signed 32-bit fractional */
    Frac32 f32B; /*!< B component, type signed 32-bit fractional */
    Frac32 f32C; /*!< C component, type signed 32-bit fractional */
} MCLIB_3_COOR_SYST_T_F32;

/************************************************************************//*!
@struct MCLIB_2_COOR_SYST_ALPHA_BETA_T_F32 "\SWLIBS_Typedefs.h"

@brief  Two coordinate system alpha-beta (32-bit fractional).
*//*************************************************************************/
typedef struct
{
    Frac32 f32Alpha; /*!< Alpha component, type signed 32-bit fractional */
    Frac32 f32Beta;  /*!< Beta component, type signed 32-bit fractional */
} MCLIB_2_COOR_SYST_ALPHA_BETA_T_F32;

/************************************************************************//*!
@struct MCLIB_2_COOR_SYST_D_Q_T_F32 "\SWLIBS_Typedefs.h"

@brief  Two coordinate quadrature system (32-bit fractional).
*//*************************************************************************/
typedef struct
{
    Frac32 f32D;     /*!< Direct axis component, type signed 32-bit fractional */
    Frac32 f32Q;     /*!< Quadrature axis component, type signed 32-bit fractional */
} MCLIB_2_COOR_SYST_D_Q_T_F32;

/************************************************************************//*!
@struct MCLIB_ANGLE_T_F32 "\SWLIBS_Typedefs.h"

@brief  Sine and cosine values (32-bit fractional).
*//*************************************************************************/
typedef struct
{
    Frac32 f32Sin;   /*!< Sine component, type signed 32-bit fractional */
    Frac32 f32Cos;   /*!< Cosine component, type signed 32-bit fractional */
} MCLIB_ANGLE_T_F32;
#endif


#if (SWLIBS_SUPPORT_F16 == SWLIBS_STD_ON)
/************************************************************************//*!
@struct MCLIB_3_COOR_SYST_T_F16 "\SWLIBS_Typedefs.h"

@brief  Three phase coordinate system (16-bit fractional).
*//*************************************************************************/
typedef struct
{
    Frac16 f16A; /*!< A component, type signed 16-bit fractional */
    Frac16 f16B; /*!< B component, type signed 16-bit fractional */
    Frac16 f16C; /*!< C component, type signed 16-bit fractional */
} MCLIB_3_COOR_SYST_T_F16;

/************************************************************************//*!
@struct MCLIB_2_COOR_SYST_ALPHA_BETA_T_F16 "\SWLIBS_Typedefs.h"

@brief  Two coordinate system alpha-beta (16-bit fractional).
*//*************************************************************************/
typedef struct
{
    Frac16 f16Alpha; /*!< Alpha component, type signed 16-bit fractional */
    Frac16 f16Beta;  /*!< Beta component, type signed 16-bit fractional */
} MCLIB_2_COOR_SYST_ALPHA_BETA_T_F16;

/************************************************************************//*!
@struct MCLIB_2_COOR_SYST_D_Q_T_F16 "\SWLIBS_Typedefs.h"

@brief  Two coordinate quadrature system (16-bit fractional).
*//*************************************************************************/
typedef struct
{
    Frac16 f16D;     /*!< Direct axis component, type signed 16-bit fractional */
    Frac16 f16Q;     /*!< Quadrature axis component, type signed 16-bit fractional */
} MCLIB_2_COOR_SYST_D_Q_T_F16;

/************************************************************************//*!
@struct MCLIB_ANGLE_T_F16 "\SWLIBS_Typedefs.h"

@brief  Sine and cosine values (16-bit fractional).
*//*************************************************************************/
typedef struct
{
    Frac16 f16Sin;   /*!< Sine component, type signed 16-bit fractional */
    Frac16 f16Cos;   /*!< Cosine component, type signed 16-bit fractional */
} MCLIB_ANGLE_T_F16;
#endif



#endif /* SWLIBS_TYPEDEFS_H */
