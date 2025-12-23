/*******************************************************************************
**                                                                            **
** Copyright (C) Infineon Technologies (2013)                                 **
**                                                                            **
** All rights reserved.                                                       **
**                                                                            **
** This document contains proprietary information belonging to Infineon       **
** Technologies. Passing on and copying of this document, and communication   **
** of its contents is not permitted without prior written authorization.      **
**                                                                            **
********************************************************************************
**                                                                            **
**  $FILENAME   : Mcu_Ver.c $                                                **
**                                                                            **
**  $CC VERSION : \main\16 $                                                 **
**                                                                            **
**  $DATE       : 2015-10-05 $                                               **
**                                                                            **
**  AUTHOR      : DL-AUTOSAR-Engineering                                      **
**                                                                            **
**  VENDOR      : Infineon Technologies                                       **
**                                                                            **
**  DESCRIPTION : This file contains AUTOSAR version specific functionality   **
**                of MCU driver.                                              **
**                                                                            **
**  MAY BE CHANGED BY USER [yes/no]: No                                       **
**                                                                            **
*******************************************************************************/
/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
  /* Inclusion of controller sfr file */
#include "IfxScu_reg.h"
#include "Mcu.h"
#include "Mcu_Local.h"
/*******************************************************************************
**                      Imported Compiler Switch Check                        **
*******************************************************************************/

/*
  Requirement key:

*/

/*
      VERSION CHECK FOR MCU MODULE INCLUSION
*/

#ifndef MCU_AR_RELEASE_MAJOR_VERSION
  #error "MCU_AR_RELEASE_MAJOR_VERSION is not defined. "
#endif

#ifndef MCU_AR_RELEASE_MINOR_VERSION
  #error "MCU_AR_RELEASE_MINOR_VERSION is not defined. "
#endif

#ifndef MCU_AR_RELEASE_REVISION_VERSION
  #error "MCU_AR_RELEASE_REVISION_VERSION is not defined. "
#endif
/*
  Requirement key
  MCU121, MCU037 : Version Identification
*/
#if ( MCU_AR_RELEASE_MAJOR_VERSION != 4U )
  #error "MCU_AR_RELEASE_MAJOR_VERSION does not match. "
#endif
#if ( MCU_AR_RELEASE_MINOR_VERSION != 0U )
  #error "MCU_AR_RELEASE_MINOR_VERSION does not match. "
#endif

#ifndef MCU_SW_MAJOR_VERSION
  #error "MCU_SW_MAJOR_VERSION is not defined. "
#endif

#ifndef MCU_SW_MINOR_VERSION
  #error "MCU_SW_MINOR_VERSION is not defined. "
#endif

#ifndef MCU_SW_PATCH_VERSION
  #error "MCU_SW_PATCH_VERSION is not defined. "
#endif

#if ( MCU_SW_MAJOR_VERSION != 4U )
  #error "MCU_SW_MAJOR_VERSION does not match. "
#endif
#if ( MCU_SW_MINOR_VERSION != 4U )
  #error "MCU_SW_MINOR_VERSION does not match. "
#endif

/*
      VERSION CHECK FOR DET MODULE INCLUSION
*/
#if ( MCU_DEV_ERROR_DETECT == STD_ON)

#ifndef DET_AR_RELEASE_MAJOR_VERSION
  #error "DET_AR_RELEASE_MAJOR_VERSION is not defined. "
#endif

#ifndef DET_AR_RELEASE_MINOR_VERSION
  #error "DET_AR_RELEASE_MINOR_VERSION is not defined. "
#endif

/*
  Requirement key
  MCU121, MCU037 : Version Identification
*/
/* [cover parentID=DS_AS40X_MCU110] [/cover] */

#if (IFX_DET_VERSION_CHECK == STD_ON)

#if ( DET_AR_RELEASE_MAJOR_VERSION != 4U )
  #error "DET_AR_RELEASE_MAJOR_VERSION does not match. "
#endif
#if ( DET_AR_RELEASE_MINOR_VERSION != 0U )
  #error "DET_AR_RELEASE_MINOR_VERSION does not match. "
#endif

#endif /* (IFX_DET_VERSION_CHECK == STD_ON) */
#endif  /* End Of MCU_DEV_ERROR_DETECT */

#if ((MCU_E_CLOCK_FAILURE_DEM_REPORT == MCU_ENABLE_DEM_REPORT)||\
     (MCU_E_ERAY_TIMEOUT_DEM_REPORT == MCU_ENABLE_DEM_REPORT)||\
     (MCU_E_OSC_FAILURE_DEM_REPORT == MCU_ENABLE_DEM_REPORT))
/*------------------------------------------------------------------------------
                            DEM File inclusion
------------------------------------------------------------------------------*/

#ifndef DEM_AR_RELEASE_MAJOR_VERSION
  #error "DEM_AR_RELEASE_MAJOR_VERSION is not defined. "
#endif
#ifndef DEM_AR_RELEASE_MINOR_VERSION
  #error "DEM_AR_RELEASE_MINOR_VERSION is not defined. "
#endif
#if (IFX_DEM_VERSION_CHECK == STD_ON)
#if ( DEM_AR_RELEASE_MAJOR_VERSION != 4U )
  #error "DEM_AR_RELEASE_MAJOR_VERSION does not match. "
#endif
#if ( DEM_AR_RELEASE_MINOR_VERSION != 0U )
  #error "DEM_AR_RELEASE_MINOR_VERSION does not match. "
#endif
#endif /*  (IFX_DEM_VERSION_CHECK == STD_ON)  */
#endif /*((MCU_E_CLOCK_FAILURE_DEM_REPORT == MCU_ENABLE_DEM_REPORT)||\
           (MCU_E_ERAY_TIMEOUT_DEM_REPORT == MCU_ENABLE_DEM_REPORT)||\
           (MCU_E_OSC_FAILURE_DEM_REPORT == MCU_ENABLE_DEM_REPORT))*/
/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/
#if (MCU_GET_RAM_STATE_API == STD_ON)
/* Mask for CPU0 bit of GET RAM STATE */
#define MCU_GERTAMSTATE_CPU_MASK  (0x01U)
#define MCU_RAMSTATE_SHIFT_VALUE  (12U)
#define MCU_ALLCORE_RAMSTATE_MASK (7U)
#endif
/*******************************************************************************
**                      Private Type Definitions                              **
*******************************************************************************/

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/



/*Memory Map of the MCU Code*/
#define MCU_START_SEC_CODE
#include "MemMap.h"

#if (MCU_GET_RAM_STATE_API == STD_ON)
/*******************************************************************************
** Traceability : [cover parentID=DS_AS40X_MCU207,DS_AS40X_MCU209]            **
**                                                                            **
** Syntax : Mcu_RamStateType Mcu_GetRamState                                  **
**          (                                                                 **
**             void                                                           **
**          )                                                                 **
** [/cover]                                                                   **
**                                                                            **
** Service ID:    4                                                           **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Reentrant                                                   **
**                                                                            **
** Parameters (in):   None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value:      Mcu_RamStateType                                        **
**                    Returns the status of RAM content                       **
**                                                                            **
** Description :  This service provides the actual status of the              **
**                microcontroller RAM.                                        **
**                                                                            **
*******************************************************************************/
Mcu_RamStateType Mcu_GetRamState(void)
{
  uint32 RamState;
  Mcu_RamStateType RetValue;
  uint8 CoreId;
  /* To remove the MISRA error this is used*/
  #if ( MCU_DEV_ERROR_DETECT == STD_ON )
  uint32 ErrorFlag;
  /*MCU101 : Parameter check for the API if DET is STD_ON*/
  /* MCU012, MCU016, MCU013, MCU015  : DET Parameter checking*/
  /* MCU125 :Check whether Mcu_init is  called before this API.*/
  ErrorFlag = Mcu_lInitCallVerify(MCU_SID_GETRAMSTATE);
  if (ErrorFlag == MCU_ERR_FOUND)
  {
    /*Return Undefined in case of Error */
    RetValue = MCU_RAMSTATE_INVALID;
  }
  else
  #endif /* MCU_DEV_ERROR_DETECT == STD_ON */
  {
     CoreId = Mcal_GetCoreId();
     RamState = ((uint32)(SCU_RSTCON2.U)>>MCU_RAMSTATE_SHIFT_VALUE) &\
                                                    MCU_ALLCORE_RAMSTATE_MASK;
     RetValue = (Mcu_RamStateType)((RamState>>CoreId) &\
                                   MCU_GERTAMSTATE_CPU_MASK);
  }
  return (RetValue);
}/*End of Mcu_GetRamState()*/

#endif /*End of Mcu_GetRamState */

#define MCU_STOP_SEC_CODE
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives
 is allowed only for MemMap.h*/
#include "MemMap.h"
