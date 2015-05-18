/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       MLIB_Round.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for MLIB_Round function
*
******************************************************************************/
#ifndef MLIB_ROUND_H
#define MLIB_ROUND_H
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
* @section MLIB_Round_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section MLIB_Round_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section MLIB_Round_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section MLIB_Round_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*
* @section MLIB_Round_h_REF_5
* Violates MISRA 2004 Required Rule 8.5, Object/function definition in header file.
* Allowing the inline functions significantly increase the speed of the library thus the lowest
* layer (MLIB) is implemented as inline. This approach removes the overhead caused by standard
* function calling.
*
* @section MLIB_Round_h_REF_6
* Violates MISRA 2004 Required Rule 14.7, Return statement before end of function.
* There is no required code which needs to be executed after the calculation in the branch, thus the
* performance of the function will be negatively influenced in case the single return point is used.
*/
#include "SWLIBS_Defines.h"
#ifndef AMMCLIB_TESTING_ENV
  #include "MLIB_Add.h"
  #include "MLIB_ShL.h"
  #include "MLIB_ShR.h"
#else
  /* Following includes serve for Freescale internal testing purposes only.
  *  These headers are not part of the release. */
  #include "CCOV_MLIB_Add.h"
  #include "CCOV_MLIB_ShL.h"
  #include "CCOV_MLIB_ShR.h"
#endif

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef  _MATLAB_BAM_CREATE
  /*
  * @violates @ref MLIB_Round_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_Round_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Round_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define MLIB_Round(...)     macro_dispatcher(MLIB_Round, __VA_ARGS__)(__VA_ARGS__)     /*!< This function rounds the first input value for number of digits defined by second parameter and saturate automatically. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref MLIB_Round_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_Round_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_Round_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00205 */
    #define MLIB_Round_Dsptchr_2(In1,In2)     MLIB_Round_Dsptchr_3(In1,In2,F32)     /*!< Function dispatcher for MLIB_Round_Dsptchr_2, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref MLIB_Round_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
    * macro.
    * @violates @ref MLIB_Round_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
    * @violates @ref MLIB_Round_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
    * parameter in definition of macro.
    */
    /** @remarks Implements DMLIB00205 */
    #define MLIB_Round_Dsptchr_2(In1,In2)     MLIB_Round_Dsptchr_3(In1,In2,F16)     /*!< Function dispatcher for MLIB_Round_Dsptchr_2, do not modify!!! */
  #endif

  /*
  * @violates @ref MLIB_Round_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_Round_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Round_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define MLIB_Round_Dsptchr_3(In1,In2,Impl)    MLIB_Round_Dsptchr_(In1,In2,Impl)   /*!< Function dispatcher for MLIB_Round_Dsptchr_3, do not modify!!! */

  /*
  * @violates @ref MLIB_Round_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition for
  * macro.
  * @violates @ref MLIB_Round_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
  * @violates @ref MLIB_Round_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  * @violates @ref MLIB_Round_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
  */
  /** @remarks Implements DMLIB00207 */
  #define MLIB_Round_Dsptchr_(In1,In2,Impl)     MLIB_Round_##Impl(In1,In2)          /*!< Function dispatcher for MLIB_Round_Dsptchr_, do not modify!!! */
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

@brief        The function rounds the first input value for number of digits
              defined by second parameter and saturate automatically.

@param[in]         f32In1     The first value to be rounded.

@param[in]         u16In2     The round digits amount.

@return       32-bit fractional value rounded to the nearest n-bit fractional
              value where "n" is defined by the second input value.
              The bits beyond the 16-bit boundary are discarded.

@details      This function rounds the first argument to nearest value defined by
              the number of bits defined by second argument and saturate if an
              overflow is detected.
              The function returns a saturated fractional value if the return
              value cannot fit into the return type.

@note         The round amount cannot exceed in magnitude the bit-width of the
              rounded value, that means must be within the range 0...31. Otherwise
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
    // first input = 0.5
    f32In1 = FRAC32(0.5);
    // second input = 29
    u16In2 = 29;

    // output should be 0x60000000 ~ FRAC32(0.75)
    f32Out = MLIB_Round_F32(f32In1,u16In2);

    // output should be 0x60000000 ~ FRAC32(0.75)
    f32Out = MLIB_Round(f32In1,u16In2,F32);

    // ##############################################################
    // Available only if 32-bit fractional implementation selected
    // as default
    // ##############################################################

    // output should be 0x60000000 ~ FRAC32(0.75)
    f32Out = MLIB_Round(f32In1,u16In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00206, DMLIB00199, DMLIB00204 */
static inline Frac32 MLIB_Round_F32(register Frac32 f32In1,register UWord16 u16In2)
/*
* @violates @ref MLIB_Round_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  if(f32In1 >= MLIB_ShL_F32(MLIB_ShR_F32(INT32_MAX,u16In2),u16In2)){
    /*
    * @violates @ref MLIB_Round_h_REF_6 MISRA 2004 Required Rule 14.7, Return statement before end
    * of function.
    */
    return(INT32_MAX);
  }else{
    /*
    * @violates @ref MLIB_Round_h_REF_6 MISRA 2004 Required Rule 14.7, Return statement before end
    * of function.
    */
    /** @remarks Implements DMLIB00202, DMLIB00203 */
    return(MLIB_ShL_F32(MLIB_Add_F32(MLIB_ShR_F32(f32In1,u16In2),(Frac32)1),u16In2));
  }
}





/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
/***************************************************************************/
/*!
@ingroup    MLIB_GROUP

@brief        The function rounds the first input value for number of digits
              defined by second parameter and saturate automatically.

@param[in]         f16In1     The first value to be rounded.

@param[in]         u16In2     The round digits amount.

@return       16-bit fractional value rounded to the nearest n-bit fractional
              value where "n" is defined by the second input value.
              The bits beyond the 16-bit boundary are discarded.

@details      This function rounds the first argument to nearest value defined by
              the number of bits defined by second argument and saturate if an
              overflow is detected.
              The function returns a saturated fractional value if the return
              value cannot fit into the return type.

@note         The round amount cannot exceed in magnitude the bit-width of the
              rounded value, that means must be within the range 0...15. Otherwise
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
    // first input = 0.5
    f16In1 = FRAC16(0.5);
    // second input = 13
    u16In2 = 13;

    // output should be 0x6000 ~ FRAC16(0.75)
    f16Out = MLIB_Round_F16(f16In1,u16In2);

    // output should be 0x6000 ~ FRAC16(0.75)
    f16Out = MLIB_Round(f16In1,u16In2,F16);

    // ##############################################################
    // Available only if 16-bit fractional implementation selected
    // as default
    // ##############################################################

    // output should be 0x6000 ~ FRAC16(0.75)
    f16Out = MLIB_Round(f16In1,u16In2);
}
\endcode
****************************************************************************/
/** @remarks Implements DMLIB00206, DMLIB00199, DMLIB00204 */
static inline Frac16 MLIB_Round_F16(register Frac16 f16In1,register UWord16 u16In2)
/*
* @violates @ref MLIB_Round_h_REF_5 MISRA 2004 Required Rule 8.5, Object/function definition in
* header file.
*/
{
  if(f16In1 >= MLIB_ShL_F16(MLIB_ShR_F16(INT16_MAX,u16In2),u16In2)){
    /*
    * @violates @ref MLIB_Round_h_REF_6 MISRA 2004 Required Rule 14.7, Return statement before end
    * of function.
    */
    return(INT16_MAX);
  }else{
    /*
    * @violates @ref MLIB_Round_h_REF_6 MISRA 2004 Required Rule 14.7, Return statement before end
    * of function.
    */
    /** @remarks Implements DMLIB00201, DMLIB00203 */
    return(MLIB_ShL_F16(MLIB_Add_F16(MLIB_ShR_F16(f16In1,u16In2),(Frac16)1),u16In2));
  }
}

#ifdef __cplusplus
}
#endif

#endif /* MLIB_ROUND_H */
