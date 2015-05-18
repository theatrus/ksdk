/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       MLIB_Sub.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for MLIB_Sub function
*
******************************************************************************/
#ifndef MLIB_SUB_H
#define MLIB_SUB_H
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
* @section MLIB_Sub_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section MLIB_Sub_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section MLIB_Sub_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section MLIB_Sub_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*
* @section MLIB_Sub_h_REF_5
* Violates MISRA 2004 Required Rule 8.5, Object/function definition in header file.
* Allowing the inline functions significantly increase the speed of the library thus the lowest
* layer (MLIB) is implemented as inline. This approach removes the overhead caused by standard
* function calling.
*/
#include "SWLIBS_Defines.h"

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef  _MATLAB_BAM_CREATE
  /*
  * @violates @ref MLIB_Sub_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_Sub_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Sub_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  */
  #define MLIB_Sub(...)     macro_dispatcher(MLIB_Sub, __VA_ARGS__)(__VA_ARGS__)     /*!< This function subtracts the second parameter from the first one. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref MLIB_Sub_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_Sub_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_Sub_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00069 */
    #define MLIB_Sub_Dsptchr_2(In1,In2)     MLIB_Sub_Dsptchr_3(In1,In2,F32)     /*!< Function dispatcher for MLIB_Sub_Dsptchr_2, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref MLIB_Sub_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_Sub_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_Sub_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00069 */
    #define MLIB_Sub_Dsptchr_2(In1,In2)     MLIB_Sub_Dsptchr_3(In1,In2,F16)     /*!< Function dispatcher for MLIB_Sub_Dsptchr_2, do not modify!!! */
  #endif

  /*
  * @violates @ref MLIB_Sub_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_Sub_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Sub_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  */
  #define MLIB_Sub_Dsptchr_3(In1,In2,Impl)    MLIB_Sub_Dsptchr_(In1,In2,Impl)     /*!< Function dispatcher for MLIB_Sub_Dsptchr_3, do not modify!!! */

  /*
  * @violates @ref MLIB_Sub_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_Sub_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Sub_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  * @violates @ref MLIB_Sub_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
  */
  /** @remarks Implements DMLIB00071 */
  #define MLIB_Sub_Dsptchr_(In1,In2,Impl)     MLIB_Sub_##Impl(In1,In2)            /*!< Function dispatcher for MLIB_Sub_Dsptchr_, do not modify!!! */
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

@brief        This function subtracts the second parameter from the first one.

@param[in]    f32In1     Operand is a 32-bit number normalized between[-1,1).

@param[in]    f32In2     Operand is a 32-bit number normalized between[-1,1).

@return       The subtraction of the second argument from the first argument.

@details      Subtraction of two fractional 32-bit values. The second argument is subtracted from the first one. The
              input values as well as output value are considered as 32-bit fractional data type. The output saturation
              is not implemented in this function, thus in case the subtraction of input parameters is outside the
              (-1, 1) interval, the output value will overflow without any detection.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_Sub_F32
              \image rtf subEq1_f32.math "MLIB_Sub_Eq1"

@note         Overflow is not detected.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac32 f32In1;
Frac32 f32In2;
Frac32 f32Out;

void main(void)
{
  // first input = 0.5
  f32In1  = FRAC32(0.5);

  // second input = 0.25
  f32In2  = FRAC32(0.25);

  // output should be 0x20000000
  f32Out = MLIB_Sub_F32(f32In1,f32In2);

  // output should be 0x20000000
  f32Out = MLIB_Sub(f32In1,f32In2,F32);

  // ##############################################################
  // Available only if 32-bit fractional implementation selected
  // as default
  // ##############################################################

  // output should be 0x20000000
  f32Out = MLIB_Sub(f32In1,f32In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00070, DMLIB00063, DMLIB00066, DMLIB00068 */
static inline Frac32 MLIB_Sub_F32(register Frac32 f32In1,register Frac32 f32In2)
/*
* @violates @ref MLIB_Sub_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in header
* file.
*/
{
  /** @remarks Implements DMLIB00067 */
  return(f32In1 - f32In2);
}





/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        This function subtracts the second parameter from the first one.

@param[in]    f16In1     Operand is a 16-bit number normalized between [-1,1).

@param[in]    f16In2     Operand is a 16-bit number normalized between [-1,1).

@return       The subtraction of the second argument from the first argument.

@details      Subtraction of two fractional 16-bit values. The second argument is subtracted from the first one. The
              input values as well as output value are considered as 16-bit fractional data type. The output saturation
              is not implemented in this function, thus in case the subtraction of input parameters is outside the
              (-1, 1) interval, the output value will overflow without any detection.

              \par

              The output of the function is defined by the following simple equation:
              \anchor eq1_Sub_F16
              \image rtf subEq1_f16.math "MLIB_Sub_Eq1"

@note         Overflow is not detected.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac16 f16In1;
Frac16 f16In2;
Frac16 f16Out;

void main(void)
{
  // first input = 0.5
  f16In1  = FRAC16(0.5);

  // second input = 0.25
  f16In2  = FRAC16(0.25);

  // output should be 0x2000
  f16Out = MLIB_Sub_F16(f16In1,f16In2);

  // output should be 0x2000
  f16Out = MLIB_Sub(f16In1,f16In2,F16);

  // ##############################################################
  // Available only if 16-bit fractional implementation selected
  // as default
  // ##############################################################

  // output should be 0x20000000
  f16Out = MLIB_Sub(f16In1,f16In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00070, DMLIB00063, DMLIB00065, DMLIB00068 */
static inline Frac16 MLIB_Sub_F16(register Frac16 f16In1,register Frac16 f16In2)
/*
* @violates @ref MLIB_Sub_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in header
* file.
*/
{
  /** @remarks Implements DMLIB00067 */
  return(f16In1 - f16In2);
}





#ifdef __cplusplus
}
#endif

#endif /* MLIB_SUB_H */
