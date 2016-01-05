/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       MLIB_Convert.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for MLIB_Convert function
*
******************************************************************************/
#ifndef MLIB_CONVERT_H
#define MLIB_CONVERT_H
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
* @section MLIB_Convert_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section MLIB_Convert_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section MLIB_Convert_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section MLIB_Convert_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*
* @section MLIB_Convert_h_REF_5
* Violates MISRA 2004 Required Rule 8.5, Object/function definition in header file.
* Allowing the inline functions significantly increase the speed of the library thus the lowest
* layer (MLIB) is implemented as inline. This approach removes the overhead caused by standard
* function calling.
*
* @section MLIB_Convert_h_REF_6
* Violates MISRA 2004 Required Rule 12.7, Bitwise operator applied to signed underlying type.
* The fractional arithmetic requires the bit-wise operations on signed values.
*/
#include "SWLIBS_Defines.h"
#ifndef AMMCLIB_TESTING_ENV
  #include "MLIB_ShL.h"
  #include "MLIB_Mul.h"
  #include "MLIB_ShR.h"
  #include "MLIB_Div.h"
  #include "MLIB_MulSat.h"
  #include "MLIB_DivSat.h"
  #include "MLIB_Neg.h"
  #include "MLIB_NegSat.h"
  #include "MLIB_ConvertPU.h"
#else
  /* Following includes serve for Freescale internal testing purposes only.
  *  These headers are not part of the release. */
  #include "CCOV_MLIB_ShL.h"
  #include "CCOV_MLIB_Mul.h"
  #include "CCOV_MLIB_ShR.h"
  #include "CCOV_MLIB_Div.h"
  #include "CCOV_MLIB_MulSat.h"
  #include "CCOV_MLIB_DivSat.h"
  #include "CCOV_MLIB_Neg.h"
  #include "CCOV_MLIB_NegSat.h"
  #include "CCOV_MLIB_ConvertPU.h"
#endif

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef  _MATLAB_BAM_CREATE
  /*
  * @violates @ref MLIB_Convert_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_Convert_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Convert_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define MLIB_Convert(...)     macro_dispatcher(MLIB_Convert, __VA_ARGS__)(__VA_ARGS__)     /*!< This function converts the input value to different representation. */

  /*
  * @violates @ref MLIB_Convert_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_Convert_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Convert_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define MLIB_Convert_Dsptchr_3(In1,In2,Impl)    MLIB_Convert_Dsptchr_(In1,In2,Impl)     /*!< Function dispatcher for MLIB_Convert_Dsptchr_3, do not modify!!! */

  /*
  * @violates @ref MLIB_Convert_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_Convert_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Convert_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  * @violates @ref MLIB_Convert_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used in
  * macro.
  */
  /** @remarks Implements DMLIB00262*/
  #define MLIB_Convert_Dsptchr_(In1,In2,Impl)     MLIB_Convert_##Impl(In1,In2)            /*!< Function dispatcher for MLIB_Convert_Dsptchr_, do not modify!!! */
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

@brief        This function converts the input value to different representation with scale.

@param[in]         f16In1     Input value in 16-bit fractional format to be converted.

@param[in]         f16In2     Scale factor in 16-bit fractional format.

@return       Converted input value in 32-bit fractional format.

@details      This inline function returns converted input value. The input value is considered as 16-bit fractional
              data type and output value is considered as 32-bit fractional data type thus both values represent the
              values in unity model. The second value represents the scale factor and is considered as 16-bit
              fractional data type. The sign of the second value represents the scale mechanism. In case the second value
              is positive the first input value is multiplied with the second one and converted to the output format.
              In case the second value is negative, the first input value is divided by absolute value of second input
              value and converted to the output format.
              The output saturation is not implemented in this function, thus in case the input
              value is outside the (-1, 1) interval, the output value will overflow without any detection.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_Convert_F32F16
              \image rtf convertEq1_f32f16.math "MLIB_Convert_Eq1"

@note         Due to effectivity reason this function is implemented as inline and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac16 f16In1, f16In2;
Frac32 f32Out;

void main(void)
{
    // input value = 0.25 = 0x2000
    f16In1  = FRAC16(0.25);

    // scale value = 0.5 = 0x4000
    f16In2 =  FRAC16(0.5);

    // output should be FRAC32(0.125) = 0x10000000
    f32Out = MLIB_Convert_F32F16(f16In1, f16In2);

    // output should be FRAC32(0.125) = 0x10000000
    f32Out = MLIB_Convert(f16In1, f16In2, F32F16);

    // scale value = -0.5 = 0xC000
    f16In2 =  FRAC16(-0.5);

    // output should be FRAC32(0.5) = 0x40000000
    f32Out = MLIB_Convert_F32F16(f16In1, f16In2);

    // output should be FRAC32(0.5) = 0x40000000
    f32Out = MLIB_Convert(f16In1, f16In2, F32F16);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00261, DMLIB00254, DMLIB00257, DMLIB00259 */
static inline Frac32 MLIB_Convert_F32F16(register Frac16 f16In1,register Frac16 f16In2)
/*
* @violates @ref MLIB_Convert_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /** @remarks Implements DMLIB00258 */
  return((f16In2>=(Frac16)0) ? MLIB_Mul_F32F16F16(f16In1, f16In2) : MLIB_DivSat_F32(MLIB_ShL_F32((Frac32)f16In1,(UWord16)16), MLIB_ShL_F32((Frac32)MLIB_NegSat_F16(f16In2),(UWord16)16)));
}






/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        This function converts the input value to different representation with scale.

@param[in]         f32In1     Input value in 32-bit fractional format to be converted.

@param[in]         f32In2     Scale factor in 32-bit fractional format.

@return       Converted input value in 16-bit fractional format.

@details      This inline function returns converted input value. The input value is considered as 32-bit fractional
              data type and output value is considered as 16-bit fractional data type thus both values represent the
              values in unity model. The second value represents the scale factor and is considered as 32-bit
              fractional data type. The sign of the second value represents the scale mechanism. In case the second value
              is positive the first input value is multiplied with the second one and converted to the output format.
              In case the second value is negative, the first input value is divided by absolute value of second input
              value and converted to the output format.
              The output saturation is not implemented in this function, thus in case the input
              value is outside the (-1, 1) interval, the output value will overflow without any detection.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_Convert_F16F32
              \image rtf convertEq1_f16f32.math "MLIB_Convert_Eq1"

@note         Due to effectivity reason this function is implemented as inline and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac32 f32In1, f32In2;
Frac16 f16Out;

void main(void)
{
    // input value = 0.25 = 0x20000000
    f32In1  = FRAC32(0.25);

    // scale value = 0.5 = 0x40000000
    f32In2 =  FRAC32(0.5);

    // output should be FRAC16(0.125) = 0x1000
    f16Out = MLIB_Convert_F16F32(f32In1, f32In2);

    // output should be FRAC16(0.125) = 0x1000
    f16Out = MLIB_Convert(f32In1, f32In2, F16F32);

    // scale value = -0.5 = 0xC0000000
    f32In2 =  FRAC32(-0.5);

    // output should be FRAC16(0.5) = 0x4000
    f16Out = MLIB_Convert_F16F32(f32In1, f32In2);

    // output should be FRAC16(0.5) = 0x4000
    f16Out = MLIB_Convert(f32In1, f32In2, F16F32);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00261, DMLIB00254, DMLIB00256, DMLIB00259 */
static inline Frac16 MLIB_Convert_F16F32(register Frac32 f32In1,register Frac32 f32In2)
/*
* @violates @ref MLIB_Convert_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /** @remarks Implements DMLIB00258 */
  return((f32In2>=(Frac32)0) ? (Frac16)MLIB_ShR_F32(MLIB_Mul_F32(f32In1, f32In2),(UWord16)16) : (Frac16)MLIB_ShR_F32(MLIB_DivSat_F32(f32In1, MLIB_NegSat_F32(f32In2)),(UWord16)16));
}






#ifdef __cplusplus
}
#endif

#endif /* MLIB_CONVERT_H */
