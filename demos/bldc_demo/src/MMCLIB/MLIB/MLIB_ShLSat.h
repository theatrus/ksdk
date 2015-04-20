/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       MLIB_ShLSat.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for MLIB_ShLSat function
*
******************************************************************************/
#ifndef MLIB_SHLSAT_H
#define MLIB_SHLSAT_H
/*!
@if MLIB_GROUP
    @addtogroup MLIB_GROUP
@else
    @defgroup MLIB_GROUP   MLIB
@endif
*/

#ifdef __cplusplus
extern "C" {
#endif

/**
* @page misra_violations MISRA-C:2004 violations
*
* @section MLIB_ShLSat_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section MLIB_ShLSat_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section MLIB_ShLSat_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section MLIB_ShLSat_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*
* @section MLIB_ShLSat_h_REF_5
* Violates MISRA 2004 Required Rule 8.5, Object/function definition in header file.
* Allowing the inline functions significantly increase the speed of the library thus the lowest
* layer (MLIB) is implemented as inline. This approach removes the overhead caused by standard
* function calling.
*/
#include "SWLIBS_Defines.h"
#ifndef AMMCLIB_TESTING_ENV
  #include "MLIB_ShL.h"
  #include "MLIB_ShR.h"
#else
  /* Following includes serve for Freescale internal testing purposes only.
  *  These headers are not part of the release. */
  #include "CCOV_MLIB_ShL.h"
  #include "CCOV_MLIB_ShR.h"
#endif

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef _MATLAB_BAM_CREATE
  /*
  * @violates @ref MLIB_ShLSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_ShLSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_ShLSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define MLIB_ShLSat(...)     macro_dispatcher(MLIB_ShLSat, __VA_ARGS__)(__VA_ARGS__)     /*!< This function shifts the first parameter to left by number defined by second parameter and saturate if necessary. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref MLIB_ShLSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_ShLSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_ShLSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00174 */
    #define MLIB_ShLSat_Dsptchr_2(In1,In2)     MLIB_ShLSat_Dsptchr_3(In1,In2,F32)     /*!< Function dispatcher for MLIB_ShLSat_Dsptchr_2, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref MLIB_ShLSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_ShLSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_ShLSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00174 */
    #define MLIB_ShLSat_Dsptchr_2(In1,In2)     MLIB_ShLSat_Dsptchr_3(In1,In2,F16)     /*!< Function dispatcher for MLIB_ShLSat_Dsptchr_2, do not modify!!! */
  #endif

  /*
  * @violates @ref MLIB_ShLSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_ShLSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_ShLSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define MLIB_ShLSat_Dsptchr_3(In1,In2,Impl)    MLIB_ShLSat_Dsptchr_(In1,In2,Impl)   /*!< Function dispatcher for MLIB_ShLSat_Dsptchr_3, do not modify!!! */

  /*
  * @violates @ref MLIB_ShLSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_ShLSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_ShLSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  * @violates @ref MLIB_ShLSat_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
  */
  /** @remarks Implements DMLIB00176 */
  #define MLIB_ShLSat_Dsptchr_(In1,In2,Impl)     MLIB_ShLSat_##Impl(In1,In2)          /*!< Function dispatcher for MLIB_ShLSat_Dsptchr_, do not modify!!! */
#endif /* _MATLAB_BAM_CREATE */

/****************************************************************************
* Typedefs and structures       (scope: module-local)
****************************************************************************/

/****************************************************************************
* Exported function prototypes
****************************************************************************/

/****************************************************************************
* Inline functions
****************************************************************************/





/****************************************************************************
* Implementation variant: 32-bit fractional
****************************************************************************/
/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        This function shifts the first parameter to left by number defined
              by second parameter and saturate if necessary.

@param[in]         f32In1     First value to be left shift.

@param[in]         u16In2     The shift amount value.

@return       32-bit fractional value shifted to left by the shift amount.
              The bits beyond the 32-bit boundary are discarded.

@details      This function shifts the first argument to left by number defined
              by second argument and saturate if an overflow is detected.
              The function returns a saturated fractional value if the return
              value cannot fit into the return type.

@note         The shift amount cannot exceed in magnitude the bit-width of the
              shift value, that means must be within the range 0...31. Otherwise
              the result of the function is undefined.
              Due to effectivity reason this function is implemented as inline
              and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac32 f32In1;
Frac32 f32Out;
UWord16 u16In2;

void main(void)
{
    // first input = 0.25
    f32In1 = FRAC32(0.25);
    // second input = 1
    u16In2 = 1;

    // output should be 0x40000000 ~ FRAC32(0.5)
    f32Out = MLIB_ShLSat_F32(f32In1, u16In2);

    // output should be 0x40000000 ~ FRAC32(0.5)
    f32Out = MLIB_ShLSat(f32In1, u16In2, F32);

    // ##############################################################
    // Available only if 32-bit fractional implementation selected
    // as default
    // ##############################################################

    // output should be 0x40000000 ~ FRAC32(0.5)
    f32Out = MLIB_ShLSat(f32In1, u16In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00175, DMLIB00169, DMLIB00173, DMLIB00178 */
static inline Frac32 MLIB_ShLSat_F32(register Frac32 f32In1,register UWord16 u16In2)
/*
* @violates @ref MLIB_ShLSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /*
  * @violates @ref MLIB_ShLSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Frac32 f32Out;
  /*
  * @violates @ref MLIB_ShLSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Frac32 f32Temp;

  f32Out = MLIB_ShL_F32(f32In1, u16In2);
  f32Temp = MLIB_ShR_F32(INT32TOF32(INT32_MAX),u16In2);
  if(f32In1>f32Temp){
    f32Out = INT32TOF32(INT32_MAX);
  }else{
    f32Temp = MLIB_ShR_F32(INT32TOF32(INT32_MIN),u16In2);
    if(f32In1 <= f32Temp){
      f32Out = INT32TOF32(INT32_MIN);
    }
  }
  /** @remarks Implements DMLIB00171, DMLIB00172 */
  return(f32Out);
}





/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        This function shifts the first parameter to left by number defined
              by second parameter and saturate if necessary.

@param[in]         f16In1     First value to be left shift.

@param[in]         u16In2     The shift amount value.

@return       16-bit fractional value shifted to left by the shift amount.
              The bits beyond the 16-bit boundary are discarded.

@details      This function shifts the first argument to left by number defined
              by second argument and saturate if an overflow is detected.
              The function returns a saturated fractional value if the return
              value cannot fit into the return type.

@note         The shift amount cannot exceed in magnitude the bit-width of the
              shift value, that means must be within the range 0...15. Otherwise
              the result of the function is undefined.
              Due to effectivity reason this function is implemented as inline
              and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac16 f16In1;
Frac16 f16Out;
UWord16 u16In2;

void main(void)
{
    // first input = 0.25
    f16In1 = FRAC16(0.25);
    // second input = 1
    u16In2 = 1;

    // output should be 0x4000 ~ FRAC16(0.5)
    f16Out = MLIB_ShLSat_F16(f16In1, u16In2);

    // output should be 0x4000 ~ FRAC16(0.5)
    f16Out = MLIB_ShLSat(f16In1, u16In2, F16);

    // ##############################################################
    // Available only if 16-bit fractional implementation selected
    // as default
    // ##############################################################

    // output should be 0x4000 ~ FRAC16(0.5)
    f16Out = MLIB_ShLSat(f16In1, u16In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00175, DMLIB00169, DMLIB00173, DMLIB00179 */
static inline Frac16 MLIB_ShLSat_F16(register Frac16 f16In1,register UWord16 u16In2)
/*
* @violates @ref MLIB_ShLSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /*
  * @violates @ref MLIB_ShLSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Frac16 f16Out;
  /*
  * @violates @ref MLIB_ShLSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Frac16 f16Temp;

  f16Out = MLIB_ShL_F16(f16In1, u16In2);
  f16Temp = MLIB_ShR_F16(INT16TOF16(INT16_MAX),u16In2);
  if(f16In1>f16Temp){
    f16Out = INT16TOF16(INT16_MAX);
  }else{
    f16Temp = MLIB_ShR_F16(INT16TOF16(INT16_MIN),u16In2);
    if(f16In1 <= f16Temp){
      f16Out = INT16TOF16(INT16_MIN);
    }
  }
  /** @remarks Implements DMLIB00170, DMLIB00172 */
  return(f16Out);
}

#ifdef __cplusplus
}
#endif

#endif /* MLIB_SHLSAT_H */
