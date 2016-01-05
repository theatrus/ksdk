/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       MLIB_Norm.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for MLIB_Norm function
*
******************************************************************************/
#ifndef MLIB_NORM_H
#define MLIB_NORM_H
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
* @section MLIB_Norm_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section MLIB_Norm_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section MLIB_Norm_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section MLIB_Norm_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*
* @section MLIB_Norm_h_REF_5
* Violates MISRA 2004 Required Rule 8.5, Object/function definition in header file.
* Allowing the inline functions significantly increase the speed of the library thus the lowest
* layer (MLIB) is implemented as inline. This approach removes the overhead caused by standard
* function calling.
*
* @section MLIB_Norm_h_REF_6
* Violates MISRA 2004 Required Rule 14.7, Return statement before end of function.
* There is no required code which needs to be executed after the calculation in the branch, thus the
* performance of the function will be negatively influenced in case the single return point is used.
*/
#include "SWLIBS_Defines.h"
#ifndef AMMCLIB_TESTING_ENV
  #include "MLIB_ShR.h"
  #include "MLIB_AbsSat.h"
#else
  /* Following includes serve for Freescale internal testing purposes only.
  *  These headers are not part of the release. */
  #include "CCOV_MLIB_ShR.h"
  #include "CCOV_MLIB_AbsSat.h"
#endif

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef  _MATLAB_BAM_CREATE
  /*
  * @violates @ref MLIB_Norm_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_Norm_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Norm_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  */
  #define MLIB_Norm(...)     macro_dispatcher(MLIB_Norm, __VA_ARGS__)(__VA_ARGS__)     /*!< This function returns the number of left shifts needed to normalize the input parameter. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref MLIB_Norm_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_Norm_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_Norm_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00195 */
    #define MLIB_Norm_Dsptchr_1(In)     MLIB_Norm_Dsptchr_2(In,F32)     /*!< Function dispatcher for MLIB_Norm_Dsptchr_1, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref MLIB_Norm_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_Norm_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_Norm_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00195 */
    #define MLIB_Norm_Dsptchr_1(In)     MLIB_Norm_Dsptchr_2(In,F16)     /*!< Function dispatcher for MLIB_Norm_Dsptchr_1, do not modify!!! */
  #endif

  /*
  * @violates @ref MLIB_Norm_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_Norm_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Norm_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter
  * in definition of macro.
  */
  #define MLIB_Norm_Dsptchr_2(In,Impl)    MLIB_Norm_Dsptchr_(In,Impl)   /*!< Function dispatcher for MLIB_Norm_Dsptchr_2, do not modify!!! */

  /*
  * @violates @ref MLIB_Norm_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
  * @violates @ref MLIB_Norm_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Norm_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
  */
  /** @remarks Implements DMLIB00197 */
  #define MLIB_Norm_Dsptchr_(In,Impl)     MLIB_Norm_##Impl(In)          /*!< Function dispatcher for MLIB_Norm_Dsptchr_, do not modify!!! */
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

@brief        This function returns the number of left shifts needed to normalize
              the input parameter.

@param[in]         f32In     The first value to be normalized.

@return       The number of left shift needed to normalize the argument.
              For the input "0" returns "0".

@details      Depending on the sign of the input value the function counts and
              returns the number of the left shift needed to get an equality
              between input value and the maximum fractional values "1" or "-1".

@note         Due to effectivity reason this function is implemented as inline
              and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac32 f32In;
UWord16 u16Out;

void main(void)
{
    // first input = 0.00005
    f32In = FRAC32(0.00005);

    // output should be 14
    u16Out = MLIB_Norm_F32(f32In);

    // output should be 14
    u16Out = MLIB_Norm(f32In,F32);

    // ##############################################################
    // Available only if 32-bit fractional implementation selected
    // as default
    // ##############################################################

    // output should be 14
    u16Out = MLIB_Norm(f32In);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00196, DMLIB00189, DMLIB00192, DMLIB00194 */
static inline UWord16 MLIB_Norm_F32(register Frac32 f32In)
/*
* @violates @ref MLIB_Norm_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /*
  * @violates @ref MLIB_Norm_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register UWord16 u16ShiftCount = (UWord16)0;
  /*
  * @violates @ref MLIB_Norm_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Frac32 f32InAbs;

  if(f32In == (Frac32)0){
    /*
    * @violates @ref MLIB_Norm_h_REF_6 MISRA 2004 Required Rule 14.7, Return statement before end of
    * function.
    */
    return ((UWord16) 0);
  }
  f32InAbs = MLIB_AbsSat_F32(f32In);
  do{
    f32InAbs = MLIB_ShR_F32(f32InAbs,(UWord16)1);
    u16ShiftCount += (UWord16)1;
  } while(f32InAbs > (Frac32)0);
  /** @remarks Implements DMLIB00193 */
  return ((UWord16)31-u16ShiftCount);
}





/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        This function returns the number of left shifts needed to normalize
              the input parameter.

@param[in]         f16In     The first value to be normalized.

@return       The number of left shift needed to normalize the argument.
              For the input "0" returns "0".

@details      Depending on the sign of the input value the function counts and
              returns the number of the left shift needed to get an equality
              between input value and the maximum fractional values "1" or "-1".

@note         Due to effectivity reason this function is implemented as inline
              and thus is not ANSI-C compliant.

@par Re-entrancy
              The function is re-entrant.

@par Code Example
\code
#include "mlib.h"

Frac16 f16In;
UWord16 u16Out;

void main(void)
{
    // first input = 0.00005
    f16In = FRAC16(0.00005);

    // output should be 14
    u16Out = MLIB_Norm_F16(f16In);

    // output should be 14
    u16Out = MLIB_Norm(f16In,F16);

    // ##############################################################
    // Available only if 16-bit fractional implementation selected
    // as default
    // ##############################################################

    // output should be 14
    u16Out = MLIB_Norm(f16In);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00196, DMLIB00189, DMLIB00191, DMLIB00194 */
static inline UWord16 MLIB_Norm_F16(register Frac16 f16In)
/*
* @violates @ref MLIB_Norm_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  /*
  * @violates @ref MLIB_Norm_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register UWord16 u16ShiftCount = (UWord16)0;
  /*
  * @violates @ref MLIB_Norm_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
  * header file.
  */
  register Frac16 f16InAbs;

  if(f16In == (Frac16)0){
    /*
    * @violates @ref MLIB_Norm_h_REF_6 MISRA 2004 Required Rule 14.7, Return statement before end of
    * function.
    */
    return ((UWord16) 0);
  }
  f16InAbs = MLIB_AbsSat_F16(f16In);
  do{
    f16InAbs = MLIB_ShR_F16(f16InAbs,(UWord16)1);
    u16ShiftCount += (UWord16)1;
  } while(f16InAbs > (Frac16)0);
  /** @remarks Implements DMLIB00193 */
  return ((UWord16)15-u16ShiftCount);
}

#ifdef __cplusplus
}
#endif

#endif /* MLIB_NORM_H */
