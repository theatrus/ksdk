/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file       GMCLIB_ElimDcBusRip.h
*
* @version    1.0.1.0
*
* @date       Oct-9-2013
*
* @brief      Header file for GMCLIB_ElimDcBusRip function
*
******************************************************************************/
#ifndef GMCLIB_ELIMDCBUSRIP_H
#define GMCLIB_ELIMDCBUSRIP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
* @page misra_violations MISRA-C:2004 violations
*
* @section GMCLIB_ElimDcBusRip_h_REF_1
* Violates MISRA 2004 Required Rule 19.4, Disallowed definition for macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of disallowed macro
* definition.
*
* @section GMCLIB_ElimDcBusRip_h_REF_2
* Violates MISRA 2004 Advisory Rule 19.7, Function-like macro defined.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires the function-like macro
* definition.
*
* @section GMCLIB_ElimDcBusRip_h_REF_3
* Violates MISRA 2004 Required Rule 19.10, Unparenthesized macro parameter in definition of macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of unparenthesized
* macro parameters.
*
* @section GMCLIB_ElimDcBusRip_h_REF_4
* Violates MISRA 2004 Advisory Rule 19.13, #/##' operator used in macro.
* To allow the user utilize the benefits of using all three supported implementation of each
* function in user application, the macro dispatcher inevitably requires use of '#/##' operators.
*/
#include "SWLIBS_Typedefs.h"
#include "SWLIBS_Defines.h"
#include "SWLIBS_MacroDisp.h"
#include "mlib.h"

/****************************************************************************
* Defines and macros            (scope: module-local)
****************************************************************************/
#ifndef  _MATLAB_BAM_CREATE
  /*
  * @violates @ref GMCLIB_ElimDcBusRip_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition
  * for macro.
  * @violates @ref GMCLIB_ElimDcBusRip_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro
  * defined.
  * @violates @ref GMCLIB_ElimDcBusRip_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define GMCLIB_ElimDcBusRip(...)     macro_dispatcher(GMCLIB_ElimDcBusRip, __VA_ARGS__)(__VA_ARGS__)     /*!< This function implements the DC Bus voltage ripple elimination. */

  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F32)
    /*
    * @violates @ref GMCLIB_ElimDcBusRip_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    */
    #define GMCLIB_ELIM_DC_BUS_RIP_T                              GMCLIB_ELIM_DC_BUS_RIP_T_F32                              /*!< Definition of GMCLIB_ELIM_DC_BUS_RIP_T as alias for GMCLIB_ELIM_DC_BUS_RIP_T_F32 datatype in case the 32-bit fractional implementation is selected. */
    /*
    * @violates @ref GMCLIB_ElimDcBusRip_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    */
    #define GMCLIB_ELIMDCBUSRIP_DEFAULT                        GMCLIB_ELIMDCBUSRIP_DEFAULT_F32                        /*!< Definition of GMCLIB_ELIMDCBUSRIP_DEFAULT as alias for GMCLIB_ELIMDCBUSRIP_DEFAULT_F32 default value in case the 32-bit fractional implementation is selected. */
    /*
    * @violates @ref GMCLIB_ElimDcBusRip_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    * @violates @ref GMCLIB_ElimDcBusRip_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro
    * defined.
    * @violates @ref GMCLIB_ElimDcBusRip_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized
    * macro parameter in definition of macro.
    */
    /** @remarks Implements DGMCLIB00044 */
    #define GMCLIB_ElimDcBusRip_Dsptchr_3(pOut,pIn,pParam)     GMCLIB_ElimDcBusRip_Dsptchr_4(pOut,pIn,pParam,F32)     /*!< Function dispatcher for GMCLIB_ElimDcBusRip_Dsptchr_3, do not modify!!! */
  #endif
  #if (SWLIBS_DEFAULT_IMPLEMENTATION == SWLIBS_DEFAULT_IMPLEMENTATION_F16)
    /*
    * @violates @ref GMCLIB_ElimDcBusRip_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    */
    #define GMCLIB_ELIM_DC_BUS_RIP_T                              GMCLIB_ELIM_DC_BUS_RIP_T_F16                              /*!< Definition of GMCLIB_ELIM_DC_BUS_RIP_T as alias for GMCLIB_ELIM_DC_BUS_RIP_T_F16 datatype in case the 16-bit fractional implementation is selected. */
    /*
    * @violates @ref GMCLIB_ElimDcBusRip_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    */
    #define GMCLIB_ELIMDCBUSRIP_DEFAULT                        GMCLIB_ELIMDCBUSRIP_DEFAULT_F16                        /*!< Definition of GMCLIB_ELIMDCBUSRIP_DEFAULT as alias for GMCLIB_ELIMDCBUSRIP_DEFAULT_F16 default value in case the 16-bit fractional implementation is selected. */
    /*
    * @violates @ref GMCLIB_ElimDcBusRip_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed
    * definition for macro.
    * @violates @ref GMCLIB_ElimDcBusRip_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro
    * defined.
    * @violates @ref GMCLIB_ElimDcBusRip_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized
    * macro parameter in definition of macro.
    */
    /** @remarks Implements DGMCLIB00044 */
    #define GMCLIB_ElimDcBusRip_Dsptchr_3(pOut,pIn,pParam)     GMCLIB_ElimDcBusRip_Dsptchr_4(pOut,pIn,pParam,F16)     /*!< Function dispatcher for GMCLIB_ElimDcBusRip_Dsptchr_3, do not modify!!! */
  #endif

  /*
  * @violates @ref GMCLIB_ElimDcBusRip_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition
  * for macro.
  * @violates @ref GMCLIB_ElimDcBusRip_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro
  * defined.
  * @violates @ref GMCLIB_ElimDcBusRip_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  */
  #define GMCLIB_ElimDcBusRip_Dsptchr_4(pOut,pIn,pParam,Impl)    GMCLIB_ElimDcBusRip_Dsptchr_(pOut,pIn,pParam,Impl)   /*!< Function dispatcher for GMCLIB_ElimDcBusRip_Dsptchr_4, do not modify!!! */

  /*
  * @violates @ref GMCLIB_ElimDcBusRip_h_REF_1 MISRA 2004 Required Rule 19.4, Disallowed definition
  * for macro.
  * @violates @ref GMCLIB_ElimDcBusRip_h_REF_2 MISRA 2004 Advisory Rule 19.7, Function-like macro
  * defined.
  * @violates @ref GMCLIB_ElimDcBusRip_h_REF_3 MISRA 2004 Required Rule 19.10, Unparenthesized macro
  * parameter in definition of macro.
  * @violates @ref GMCLIB_ElimDcBusRip_h_REF_4 MISRA 2004 Advisory Rule 19.13, #/##' operator used
  * in macro.
  */
  /** @remarks Implements DGMCLIB00050 */
  #define GMCLIB_ElimDcBusRip_Dsptchr_(pOut,pIn,pParam,Impl)     GMCLIB_ElimDcBusRip_##Impl(pOut,pIn,pParam)          /*!< Function dispatcher for GMCLIB_ElimDcBusRip_Dsptchr_, do not modify!!! */
#endif /* _MATLAB_BAM_CREATE */

/****************************************************************************
* Typedefs and structures       (scope: module-local)
****************************************************************************/




/****************************************************************************
* Implementation variant: 32-bit fractional
****************************************************************************/
#if (SWLIBS_SUPPORT_F32 == SWLIBS_STD_ON)
  #define GMCLIB_ELIMDCBUSRIP_DEFAULT_F32 {0,0}     /*!< Default value for GMCLIB_ELIM_DC_BUS_RIP_T_F32. */

  /************************************************************************/
  /*!
  @struct GMCLIB_ELIM_DC_BUS_RIP_T_F32 "\GMCLIB_ElimDcBusRip.h"

  @brief  Structure containing the PWM modulation index and the measured value of the DC bus voltage.
  *//*************************************************************************/
  /** @remarks Implements DGMCLIB00048, DGMCLIB00052, DGMCLIB00047 */
  typedef struct{
    Frac32 f32ModIndex; /*!< Inverse Modulation Index. */
    Frac32 f32ArgDcBusMsr; /*!< Measured DC bus voltage. */
  }GMCLIB_ELIM_DC_BUS_RIP_T_F32;

/****************************************************************************
* Exported function prototypes
****************************************************************************/
  extern void GMCLIB_ElimDcBusRip_F32(MCLIB_2_COOR_SYST_ALPHA_BETA_T_F32 * const pOut,const MCLIB_2_COOR_SYST_ALPHA_BETA_T_F32 * const pIn,const GMCLIB_ELIM_DC_BUS_RIP_T_F32 * const pParam);
#endif /* SWLIBS_SUPPORT_F32 == SWLIBS_STD_ON */




/****************************************************************************
* Implementation variant: 16-bit fractional
****************************************************************************/
#if (SWLIBS_SUPPORT_F16 == SWLIBS_STD_ON)
  #define GMCLIB_ELIMDCBUSRIP_DEFAULT_F16 {0,0}     /*!< Default value for GMCLIB_ELIM_DC_BUS_RIP_T_F16. */

  /************************************************************************/
  /*!
  @struct GMCLIB_ELIM_DC_BUS_RIP_T_F16 "\GMCLIB_ElimDcBusRip.h"

  @brief  Structure containing the PWM modulation index and the measured value of the DC bus voltage.
  *//*************************************************************************/
  /** @remarks Implements DGMCLIB00048, DGMCLIB00052, DGMCLIB00046 */
  typedef struct{
    Frac16 f16ModIndex; /*!< Inverse Modulation Index. */
    Frac16 f16ArgDcBusMsr; /*!< Measured DC bus voltage. */
  }GMCLIB_ELIM_DC_BUS_RIP_T_F16;

/****************************************************************************
* Exported function prototypes
****************************************************************************/
  extern void GMCLIB_ElimDcBusRip_F16(MCLIB_2_COOR_SYST_ALPHA_BETA_T_F16 * const pOut,const MCLIB_2_COOR_SYST_ALPHA_BETA_T_F16 * const pIn,const GMCLIB_ELIM_DC_BUS_RIP_T_F16 * const pParam);
#endif /* SWLIBS_SUPPORT_F16 == SWLIBS_STD_ON */




#ifdef __cplusplus
}
#endif

#endif /* GMCLIB_ELIMDCBUSRIP_H */
