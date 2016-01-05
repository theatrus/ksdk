/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       MLIB_DivSat.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for MLIB_DivSat function
*
******************************************************************************/
#ifndef MLIB_DIVSAT_H
#define MLIB_DIVSAT_H
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
* @section MLIB_DivSat_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section MLIB_DivSat_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section MLIB_DivSat_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section MLIB_DivSat_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*
* @section MLIB_DivSat_h_REF_5
* Violates MISRA 2004 Required Rule 8.5, Object/function definition in header file.
* Allowing the inline functions significantly increase the speed of the library thus the lowest
* layer (MLIB) is implemented as inline. This approach removes the overhead caused by standard
* function calling.
*
* @section MLIB_DivSat_h_REF_6
* Violates MISRA 2004 Required Rule 12.7, Bitwise operator applied to signed underlying type.
* The fractional arithmetic requires the bit-wise operations on signed values.
*/
#include "SWLIBS_Defines.h"
#ifndef AMMCLIB_TESTING_ENV
  #include "MLIB_Div.h"
  #include "MLIB_AbsSat.h"
#else
  /* Following includes serve for Freescale internal testing purposes only.
  *  These headers are not part of the release. */
  #include "CCOV_MLIB_Div.h"
  #include "CCOV_MLIB_AbsSat.h"
#endif

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef  _MATLAB_BAM_CREATE
  /*
  * @violates @ref MLIB_DivSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_DivSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_DivSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define MLIB_DivSat(...)     macro_dispatcher(MLIB_DivSat, __VA_ARGS__)(__VA_ARGS__)     /*!< This function divides the first parameter by the second one as saturate. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref MLIB_DivSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_DivSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_DivSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00134 */
    #define MLIB_DivSat_Dsptchr_2(In1,In2)     MLIB_DivSat_Dsptchr_3(In1,In2,F32)     /*!< Function dispatcher for MLIB_DivSat_Dsptchr_2, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref MLIB_DivSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_DivSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_DivSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00134 */
    #define MLIB_DivSat_Dsptchr_2(In1,In2)     MLIB_DivSat_Dsptchr_3(In1,In2,F16)     /*!< Function dispatcher for MLIB_DivSat_Dsptchr_2, do not modify!!! */
  #endif

  /*
  * @violates @ref MLIB_DivSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_DivSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_DivSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define MLIB_DivSat_Dsptchr_3(In1,In2,Impl)    MLIB_DivSat_Dsptchr_(In1,In2,Impl)     /*!< Function dispatcher for MLIB_DivSat_Dsptchr_3, do not modify!!! */

  /*
  * @violates @ref MLIB_DivSat_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_DivSat_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_DivSat_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  * @violates @ref MLIB_DivSat_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
  */
  /** @remarks Implements DMLIB00136 */
  #define MLIB_DivSat_Dsptchr_(In1,In2,Impl)     MLIB_DivSat_##Impl(In1,In2)            /*!< Function dispatcher for MLIB_DivSat_Dsptchr_, do not modify!!! */
#endif

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

@brief        This function divides the first parameter by the second one as saturate.

@param[in]         f32In1     Numerator of division.

@param[in]         f32In2     Denominator of division.

@return       Division of two input values, saturated if necessary.

@details      This inline function returns the division of two input values. The first input value is numerator and the
              second input value is denominator. The input values as well as output value is considered as 32-bit
              fractional data type. The output saturation is implemented in this function, thus in case the sum of input
              values is outside the (-1, 1) interval, the output value is limited to the boundary value.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_DivSat_F32
              \image rtf divsatEq1_f32.math "MLIB_DivSat_Eq1"

@note         Due to effectivity reason this function is implemented as inline and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac32 f32In1, f32In2;
Frac32 f32Out;

void main(void)
{
  // input value 1 = 0.25
  f32In1  = FRAC32(0.25);
  // input value 2 = 0.5
  f32In2  = FRAC32(0.5);

  // output should be FRAC32(0.5) = 0x40000000
  f32Out = MLIB_DivSat_F32(f32In1, f32In2);

  // output should be FRAC32(0.5) = 0x40000000
  f32Out = MLIB_DivSat(f32In1, f32In2, F32);

  // ##############################################################
  // Available only if 32-bit fractional implementation selected
  // as default
  // ##############################################################

  // output should be FRAC32(0.5) = 0x40000000
  f32Out = MLIB_DivSat(f32In1, f32In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00135, DMLIB00123, DMLIB00131, DMLIB00133, DMLIB00138 */
static inline Frac32 MLIB_DivSat_F32(register Frac32 f32In1,register Frac32 f32In2)
/*
* @violates @ref MLIB_DivSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /*
  * @violates @ref MLIB_DivSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Frac32 f32Limit;
  /*
  * @violates @ref MLIB_DivSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Frac32 f32Out;

  if (f32In2 == (Frac32)0){
    f32Out = (f32In1 > (Frac32)0) ? INT32TOF32(INT32_MAX) : INT32TOF32(INT32_MIN);
  }else{
    /*
    * @violates @ref MLIB_DivSat_h_REF_6 MISRA 2004 Required Rule 12.7, Bitwise operator applied to
    * signed underlying type (^).
    * @violates @ref MLIB_DivSat_h_REF_6 MISRA 2004 Required Rule 12.7, Bitwise operator applied to
    * signed underlying type (&).
    */
    f32Limit = (((INT32_MIN & f32In1)^(INT32_MIN & f32In2)) == INT32_MIN)? INT32TOF32(INT32_MIN) : INT32TOF32(INT32_MAX);
    f32Out = (MLIB_AbsSat_F32(f32In1) >= MLIB_AbsSat_F32(f32In2)) ? f32Limit : MLIB_Div_F32(f32In1, f32In2);
  }
  /** @remarks Implements DMLIB00124, DMLIB00127, DMLIB00132 */
  return(f32Out);
}





/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        This function divides the first parameter by the second one as saturate.

@param[in]         f16In1     Numerator of division.

@param[in]         f16In2     Denominator of division.

@return       Division of two input values, saturated if necessary.

@details      This inline function returns the division of two input values. The first input value is numerator and the
              second input value is denominator. The input values as well as output value is considered as 16-bit
              fractional data type. The output saturation is implemented in this function, thus in case the sum of input
              values is outside the (-1, 1) interval, the output value is limited to the boundary value.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_DivSat_F16
              \image rtf divsatEq1_f16.math "MLIB_DivSat_Eq1"

@note         Due to effectivity reason this function is implemented as inline and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac16 f16In1, f16In2;
Frac16 f16Out;

void main(void)
{
  // input value 1 = 0.25
  f16In1  = FRAC16(0.25);
  // input value 2 = 0.5
  f16In2  = FRAC16(0.5);

  // output should be FRAC16(0.5) = 0x4000
  f16Out = MLIB_DivSat_F16(f16In1, f16In2);

  // output should be FRAC16(0.5) = 0x4000
  f16Out = MLIB_DivSat(f16In1, f16In2, F16);

  // ##############################################################
  // Available only if 16-bit fractional implementation selected
  // as default
  // ##############################################################

  // output should be FRAC16(0.5) = 0x4000
  f16Out = MLIB_DivSat(f16In1, f16In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00135, DMLIB00123, DMLIB00130, DMLIB00133, DMLIB00139 */
static inline Frac16 MLIB_DivSat_F16(register Frac16 f16In1,register Frac16 f16In2)
/*
* @violates @ref MLIB_DivSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /*
  * @violates @ref MLIB_DivSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Frac16 f16Limit;
  /*
  * @violates @ref MLIB_DivSat_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Frac16 f16Out;


  if (f16In2 == (Frac16)0){
    f16Out = (f16In1 > (Frac16)0) ? INT16TOF16(INT16_MAX) : INT16TOF16(INT16_MIN);
  }else{
    /*
    * @violates @ref MLIB_DivSat_h_REF_6 MISRA 2004 Required Rule 12.7, Bitwise operator applied to
    * signed underlying type (^).
    * @violates @ref MLIB_DivSat_h_REF_6 MISRA 2004 Required Rule 12.7, Bitwise operator applied to
    * signed underlying type (~).
    */
    f16Limit = (((INT16_MIN & f16In1)^(INT16_MIN & f16In2)) == INT16_MIN) ? INT16TOF16(INT16_MIN) : INT16TOF16(INT16_MAX);
    f16Out = (MLIB_AbsSat_F16(f16In1) >= MLIB_AbsSat_F16(f16In2)) ? f16Limit : MLIB_Div_F16(f16In1, f16In2);
  }
  /** @remarks Implements DMLIB00125, DMLIB00128, DMLIB00132 */
  return(f16Out);
}

#ifdef __cplusplus
}
#endif

#endif /* MLIB_DIVSAT_H */
