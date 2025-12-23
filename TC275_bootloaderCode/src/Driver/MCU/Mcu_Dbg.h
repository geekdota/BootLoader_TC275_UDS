/******************************************************************************
**                                                                           **
** Copyright (C) Infineon Technologies (2013)                                **
**                                                                           **
** All rights reserved.                                                      **
**                                                                           **
** This document contains proprietary information belonging to Infineon      **
** Technologies. Passing on and copying of this document, and communication  **
** of its contents is not permitted without prior written authorization.     **
**                                                                           **
*******************************************************************************
**                                                                           **
**  $FILENAME   : Mcu_Dbg.h $                                                **
**                                                                           **
**  $CC VERSION : \main\9 $                                                  **
**                                                                           **
**  $DATE       : 2015-10-12 $                                               **
**                                                                           **
**  AUTHOR      : DL-AUTOSAR-Engineering                                     **
**                                                                           **
**  VENDOR      : Infineon Technologies                                      **
**                                                                           **
**  DESCRIPTION : This file contains MCU debug variable declarations.        **
**                                                                           **
**  MAY BE CHANGED BY USER [yes/no]: No                                      **
**                                                                           **
******************************************************************************/

#ifndef MCU_DBG_H
#define MCU_DBG_H

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
  /* Inclusion of module header file  */
#include "Mcu.h"
/******************************************************************************/
#define MCU_START_SEC_VAR_INIT_32BIT
#include "MemMap.h"

extern uint32 Mcu_ResetStatusVal;

#if (MCU_PB_FIXEDADDR == STD_OFF)
/* To store the Mcu driver configuration pointer */
extern const Mcu_ConfigType *Mcu_kConfigPtr;
#endif /* MCU_PB_FIXEDADDR == STD_OFF */

#define MCU_STOP_SEC_VAR_INIT_32BIT
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives
 is allowed only for MemMap.h*/
#include "MemMap.h"

/* Ideally, compilers donot throw warning when declaration is present without
a corresponding definition(untill and unless referenced) */

#define MCU_START_SEC_VAR_8BIT
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives
 is allowed only for MemMap.h*/
#include "MemMap.h"

/* To store the Mcu driver state */
extern uint8 Mcu_DriverState;

/* Clock Setting Id used to configure clocks */
extern uint8 Mcu_ClockSetting;

#define MCU_STOP_SEC_VAR_8BIT
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives
 is allowed only for MemMap.h*/
#include "MemMap.h"

#if (MCU_PB_FIXEDADDR == STD_ON)
#define MCU_START_SEC_CONST_32BIT
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives
 is allowed only for MemMap.h*/
#include "MemMap.h"

/* To store the Mcu driver configuration pointer */
extern const Mcu_ConfigType * const Mcu_kConfigPtr;

#define MCU_STOP_SEC_CONST_32BIT
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives
 is allowed only for MemMap.h*/
#include "MemMap.h"
#endif /* MCU_PB_FIXEDADDR == STD_ON */

#endif  
/* End of  MCU_DBG_H */
