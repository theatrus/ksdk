/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       MLIB_Div.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for MLIB_Div function
*
******************************************************************************/
#ifndef MLIB_DIV_H
#define MLIB_DIV_H
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
* @section MLIB_Div_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section MLIB_Div_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section MLIB_Div_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section MLIB_Div_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*
* @section MLIB_Div_h_REF_5
* Violates MISRA 2004 Required Rule 8.5, Object/function definition in header file.
* Allowing the inline functions significantly increase the speed of the library thus the lowest
* layer (MLIB) is implemented as inline. This approach removes the overhead caused by standard
* function calling.
*
* @section MLIB_Div_h_REF_6
* Violates MISRA 2004 Required Rule 12.7, Bitwise operator applied to signed underlying type.
* The fractional arithmetic requires the bit-wise operations on signed values.
*
* @section MLIB_Div_h_REF_7
* Violates MISRA 2004 Required Rule 13.3, floating point test for equality or inequality.
* Float comparison is necessary to handle boundary value cases.
*/
#include "SWLIBS_Defines.h"
#ifndef AMMCLIB_TESTING_ENV
  #include "MLIB_ShLSat.h"
  #include "MLIB_ShR.h"
  #include "MLIB_Abs.h"
#else
  /* Following includes serve for Freescale internal testing purposes only.
  *  These headers are not part of the release. */
  #include "CCOV_MLIB_ShLSat.h"
  #include "CCOV_MLIB_ShR.h"
  #include "CCOV_MLIB_Abs.h"
#endif

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef  _MATLAB_BAM_CREATE
  /*
  * @violates @ref MLIB_Div_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_Div_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Div_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  */
  #define MLIB_Div(...)     macro_dispatcher(MLIB_Div, __VA_ARGS__)(__VA_ARGS__)     /*!< This function divides the first parameter by the second one. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref MLIB_Div_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_Div_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_Div_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00119 */
    #define MLIB_Div_Dsptchr_2(In1,In2)     MLIB_Div_Dsptchr_3(In1,In2,F32)       /*!< Function dispatcher for MLIB_Div_Dsptchr_2, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref MLIB_Div_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_Div_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_Div_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00119 */
    #define MLIB_Div_Dsptchr_2(In1,In2)     MLIB_Div_Dsptchr_3(In1,In2,F16)       /*!< Function dispatcher for MLIB_Div_Dsptchr_2, do not modify!!! */
  #endif

  /*
  * @violates @ref MLIB_Div_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_Div_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Div_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  */
  #define MLIB_Div_Dsptchr_3(In1,In2,Impl)    MLIB_Div_Dsptchr_(In1,In2,Impl)     /*!< Function dispatcher for MLIB_Div_Dsptchr_3, do not modify!!! */

  /*
  * @violates @ref MLIB_Div_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_Div_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Div_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  * @violates @ref MLIB_Div_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
  */
  /** @remarks Implements DMLIB00121 */
  #define MLIB_Div_Dsptchr_(In1,In2,Impl)     MLIB_Div_##Impl(In1,In2)            /*!< Function dispatcher for MLIB_Div_Dsptchr_, do not modify!!! */
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

@brief        This function divides the first parameter by the second one.

@param[in]         f32In1     Numerator of division.

@param[in]         f32In2     Denominator of division.

@return       Division of two input values.

@details      This inline function returns the division of two input values. The first input value is numerator and the
              second input value is denominator. The input values as well as output value is considered as 32-bit
              fractional data type. The output saturation is not implemented in this function, thus in case the numerator
              is greater or equal to denominator, the output value is undefined and will overflow without any detection.
              As the division by zero can be handled differently on each platform and potentially can cause the core
              exception, the division by zero is handled separately.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_Div_F32
              \image rtf divEq1_f32.math "MLIB_Div_Eq1"

@note         Due to effectivity reason this function is implemented as inline and thus is not ANSI-C compliant.

@warning      Due to effectivity reason the division is held in 16-bit precision.

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
  f32Out = MLIB_Div_F32(f32In1, f32In2);

  // output should be FRAC32(0.5) = 0x40000000
  f32Out = MLIB_Div(f32In1, f32In2, F32);

  // ##############################################################
  // Available only if 32-bit fractional implementation selected
  // as default
  // ##############################################################

  // output should be FRAC32(0.5) = 0x40000000
  f32Out = MLIB_Div(f32In1, f32In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00120, DMLIB00107, DMLIB00116, DMLIB00118, DMLIB00108, DMLIB00111 */
static inline Frac32 MLIB_Div_F32(register Frac32 f32In1, register Frac32 f32In2)
/*
* @violates @ref MLIB_Div_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in header
* file.
*/
{
  /*
  * @violates @ref MLIB_Div_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Frac32 f32Out;
  /*
  * @violates @ref MLIB_Div_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  * @violates @ref MLIB_Div_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Frac32 f32Temp1,f32Temp2;
  /*
  * @violates @ref MLIB_Div_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  UWord16 i=(UWord16)0;

  if (f32In2 == (Frac32)0){
    f32Out = (f32In1 > (Frac32)0) ? INT32TOF32(INT32_MAX) : INT32TOF32(INT32_MIN);
  }else{
    /* Get the absolute value of input numbers */
    f32Temp1 = MLIB_Abs_F32(f32In1);
    f32Temp2 = MLIB_Abs_F32(f32In2);
    /* Normalize the input values to get the max. precision */
    /* Owerflow of shifting handeld by checking the f32In2 for zero in first condition */
    /*
    * @violates @ref MLIB_Div_h_REF_6 MISRA 2004 Required Rule 12.7, Bitwise operator applied to
    * signed underlying type (&).
    */
    while(((f32Temp1 & 0x40000000)==0) && ((f32Temp2 & 0x40000000)==0)){
      f32Temp1 = MLIB_ShL_F32(f32Temp1,(UWord16)1);
      f32Temp2 = MLIB_ShL_F32(f32Temp2,(UWord16)1);
      i++;
    }
    /* Shift left the input values to get the best precision */
    f32In1 = MLIB_ShL_F32(f32In1,i);
    f32In2 = MLIB_ShL_F32(f32In2,i);

    /*
    * @violates @ref MLIB_Div_h_REF_6 MISRA 2004 Required Rule 12.7, Bitwise operator applied to
    * signed underlying type (>>).
    */
    f32Out = INT32TOF32(F32TOINT32(f32In1)/F32TOINT32(f32In2>>(UWord16)16));
    f32Out = MLIB_ShLSat_F32(f32Out,(UWord16)15);
  }
  /** @remarks Implements DMLIB00117 */
  return(f32Out);
}





/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        This function divides the first parameter by the second one.

@param[in]         f16In1     Numerator of division.

@param[in]         f16In2     Denominator of division.

@return       Division of two input values.

@details      This inline function returns the division of two input values. The first input value is numerator and the
              second input value is denominator. The input values as well as output value is considered as 16-bit
              fractional data type. The output saturation is not implemented in this function, thus in case the numerator
              is greater or equal to denominator, the output value is undefined and will overflow without any detection.
              As the division by zero can be handled differently on each platform and potentially can cause the core
              exception, the division by zero is handled separately.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_Div_F16
              \image rtf divEq1_f16.math "MLIB_Div_Eq1"

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
  f16Out = MLIB_Div_F16(f16In1, f16In2);

  // output should be FRAC16(0.5) = 0x4000
  f16Out = MLIB_Div(f16In1, f16In2, F16);

  // ##############################################################
  // Available only if 16-bit fractional implementation selected
  // as default
  // ##############################################################

  // output should be FRAC16(0.5) = 0x4000
  f16Out = MLIB_Div(f16In1, f16In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00120, DMLIB00107, DMLIB00115, DMLIB00118, DMLIB00109, DMLIB00112 */
static inline Frac16 MLIB_Div_F16(register Frac16 f16In1, register Frac16 f16In2)
/*
* @violates @ref MLIB_Div_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in header
* file.
*/
{
  /*
  * @violates @ref MLIB_Div_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Frac16 f16Out;

  if (f16In2 == (Frac16)0){
    f16Out = (f16In1 > (Frac16)0) ? INT16TOF16(INT16_MAX) : INT16TOF16(INT16_MIN);
  }else{
    /*
    * @violates @ref MLIB_Div_h_REF_6 MISRA 2004 Required Rule 12.7, Bitwise operator applied to
    * signed underlying type (<<).
    */
    f16Out = INT16TOF16(INT32TOINT16((F16TOINT32(f16In1)<<(UWord16)15)/F16TOINT32(f16In2)));
  }
  /** @remarks Implements DMLIB00117 */
  return(f16Out);
}





#ifdef __cplusplus
}
#endif

#endif /* MLIB_DIV_H */
