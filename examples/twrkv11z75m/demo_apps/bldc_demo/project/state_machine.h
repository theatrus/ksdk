/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2013 Freescale Semiconductor, Inc.
* ALL RIGHTS RESERVED.
*
***************************************************************************//*!
*
* @file      state_machine.h
*
* @author    r63172
* 
* @version   1.0.1.0
* 
* @date      Sep-16-2013
* 
* @brief     State machine
*
*******************************************************************************
*
* State machine.
*
******************************************************************************/

#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

#ifndef true
#define true  ((tBool)1)
#endif

#ifndef false
#define false ((tBool)0)
#endif


//! @brief Application state identification enum
typedef enum {
    FAULT       = 0,
    INIT        = 1,
    STOP        = 2,
    RUN         = 3
} SM_APP_STATE_T;

typedef unsigned short SM_APP_CTRL;
typedef unsigned long SM_APP_FAULT;

//! @brief pointer to function
typedef void (*PFCN_VOID_VOID)(void);

//! @brief User state machine functions structure
typedef struct
{
    PFCN_VOID_VOID Fault;
    PFCN_VOID_VOID Init;
    PFCN_VOID_VOID Stop;
    PFCN_VOID_VOID Run;
} SM_APP_STATE_FCN_T;

//! @brief User state-transition functions structure
typedef struct
{
    PFCN_VOID_VOID FaultInit;
    PFCN_VOID_VOID InitFault;
    PFCN_VOID_VOID InitStop;
    PFCN_VOID_VOID StopFault;
    PFCN_VOID_VOID StopInit;
    PFCN_VOID_VOID StopRun;
    PFCN_VOID_VOID RunFault;
    PFCN_VOID_VOID RunStop;
} SM_APP_TRANS_FCN_T;

//! @brief State machine control structure
typedef struct
{
    SM_APP_STATE_FCN_T const*   psState;    //!< State functions
    SM_APP_TRANS_FCN_T const*   psTrans;    //!< Transition functions
    SM_APP_CTRL                 uiCtrl;     //!< Control flags
    SM_APP_STATE_T              eState;     //!< State
} SM_APP_CTRL_T;

//! @brief Pointer to function with a pointer to state machine control structure
typedef void (*PFCN_VOID_PSM)(SM_APP_CTRL_T *sAppCtrl); 


//! @brief State machine control command flags
#define SM_CTRL_NONE            0x0
#define SM_CTRL_FAULT           0x1
#define SM_CTRL_FAULT_CLEAR     0x2
#define SM_CTRL_INIT_DONE       0x4
#define SM_CTRL_STOP            0x8
#define SM_CTRL_START           0x10
#define SM_CTRL_STOP_ACK        0x20
#define SM_CTRL_RUN_ACK         0x40

//! @brief State machine function table (in pmem)
extern const PFCN_VOID_PSM gSM_STATE_TABLE[4];

//! @brief State machine function
#if defined __CC_ARM
extern __inline void SM_StateMachine(SM_APP_CTRL_T *sAppCtrl);
#else
extern  void SM_StateMachine(SM_APP_CTRL_T *sAppCtrl);
#endif

#endif /*_STATE_MACHINE_H_ */

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
