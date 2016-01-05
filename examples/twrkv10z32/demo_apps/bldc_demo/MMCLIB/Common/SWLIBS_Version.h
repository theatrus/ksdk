/******************************************************************************
*
* (c) Copyright 2013, Freescale
*
******************************************************************************/
/*!
*
* @file     SWLIBS_Version.h
*
* @version  1.0.1.0
*
* @date     Oct-9-2013
*
* @brief    MCLIB version header file.
*
******************************************************************************/
#ifndef SWLIBS_VERSION_H_
#define SWLIBS_VERSION_H_

#include "SWLIBS_Config.h"
#include "SWLIBS_Typedefs.h"

/******************************************************************************
* Defines and macros            (scope: module-local)
******************************************************************************/
#define SWLIBS_MCID_SIZE ((UWord8)4U)
#define SWLIBS_MCVERSION_SIZE ((UWord8)3U)
#define SWLIBS_MCIMPLEMENTATION_SIZE ((UWord8)8U)

#define SWLIBS_ID {(UWord8)0x90U,(UWord8)0x71U,(UWord8)0x77U,(UWord8)0x68U} /*!< Library identification string. */

/******************************************************************************
* Typedefs and structures       (scope: module-local)
******************************************************************************/
/***************************************************************************/
/*!
@struct SWLIBS_VERSION_T "\SWLIBS_Version.h"

@brief  Motor Control Library Set identification structure
*//*************************************************************************/
typedef struct
{
  UWord8 mcId[SWLIBS_MCID_SIZE];                   /*!< MCLIB identification code */
  UWord8 mcVersion[SWLIBS_MCVERSION_SIZE];         /*!< MCLIB version code */
  UWord8 mcImpl[SWLIBS_MCIMPLEMENTATION_SIZE];     /*!< MCLIB supported implementation code */
  UWord8 mcDefaultImpl;                           /*!< MCLIB default implementation code */
}SWLIBS_VERSION_T;

const SWLIBS_VERSION_T* SWLIBS_GetVersion(void);

#endif /* SWLIBS_VERSION_H_ */
