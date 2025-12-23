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
**  $FILENAME   : Mcu_Local.h $                                              **
**                                                                           **
**  $CC VERSION : \main\31 $                                                 **
**                                                                           **
**  $DATE       : 2015-11-12 $                                               **
**                                                                           **
**  AUTHOR      : DL-AUTOSAR-Engineering                                     **
**                                                                           **
**  VENDOR      : Infineon Technologies                                      **
**                                                                           **
**  DESCRIPTION : This file contains local functionality of MCU driver.      **
**                                                                           **
**  MAY BE CHANGED BY USER [yes/no]: No                                      **
**                                                                           **
******************************************************************************/
/*  TRACEABILITY : [cover parentID=DS_NAS_MCU_PR180]
                   [/cover]                                                   */
#ifndef MCU_LOCAL_H
#define MCU_LOCAL_H

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/

/*******************************************************************************
**                      Global Macro Definitions                             **
*******************************************************************************/
/* Macro to check the call for Mcu_Init.*/
#define MCU_DRIVER_INITIALIZED               (1U)
#define MCU_DRIVER_UNINITIALIZED             (0U)

/*SCU_CCUCON0.CLKSEL = PLL as source (29th & 28th bit = 0 & 1 respectively)*/
#define MCU_CLKSEL_IS_PLL_AS_SOURCE          ((unsigned_int)0x1U << 28)

/*SCU_CCUCON1.INSEL = ext osc = Fosc0 (29th & 28th bit = 0 & 1 respectively)*/
#define MCU_INSEL_IS_FOSC0_AS_SOURCE         ((unsigned_int)0x1U << 28)

/* Used to reset the VCOPWD bit in PLLCON0 SFR */
#define MCU_VCOPWD_IS_SET_TO_NORMAL          (~((unsigned_int)1U<<1))

/* Used to reset the PLLPWD bit in PLLCON0 SFR */
#define MCU_PLLPWD_IS_SET_TO_NORMAL          ((unsigned_int)1U<<16)

/* Delay for PLL stability*/
#define MCU_PLL_STABLE_DELAY                 (500U)

/* For Mcu_SetMode() local functions */
#define MCU_INDIVIDUAL_CORE_IDLE_MODE        ((uint32)0x00U)
#define MCU_UNANIMOUS_SEL_SYSTEM_MODE        ((uint32)0x07U)

/* Reset value of PMSWCR1 .STBYEVEN is set, so that STBYEV can be written */
#define MCU_PMSWCR1_DEINIT_VALUE             ((uint32)0x09000000U)

/* Reset value of RSTCON register */
#define MCU_RSTCON_DEINIT_VALUE              ((uint32)0x00000282U)

/* Reset value of ARSTDIS register */
#define MCU_ARSTDIS_DEINIT_VALUE             ((uint32)0x0U)

/*******************************************************************************
**                      Global Variable Declarations                          **
*******************************************************************************/
#define MCU_START_SEC_VAR_INIT_32BIT
#include "MemMap.h"

extern uint32 Mcu_ResetStatusVal;

#if (MCU_PB_FIXEDADDR == STD_OFF)
/* To store the Mcu driver configuration pointer */
extern const Mcu_ConfigType *Mcu_kConfigPtr;
#endif /* MCU_PB_FIXEDADDR == STD_OFF */

#define MCU_STOP_SEC_VAR_INIT_32BIT
#include "MemMap.h"

#define MCU_START_SEC_VAR_8BIT
#include "MemMap.h"

#if (MCU_DEV_ERROR_DETECT == STD_ON)
/*IFX_MISRA_RULE_08_08_STATUS=Mcu_DriverState is declared extern only here
 and is defined in Mcu.c. This violation is reported
  incorrectly by PC-lint tool */
extern uint8 Mcu_DriverState;
#endif /* End MCU_DEV_ERROR_DETECT == STD_ON */

/*IFX_MISRA_RULE_08_08_STATUS=Mcu_ClockSetting is declared extern only here and
 is defined in Mcu.c. This violation is reported
  incorrectly by PC-lint tool */
extern uint8 Mcu_ClockSetting;

#define MCU_STOP_SEC_VAR_8BIT
#include "MemMap.h"

#if (MCU_PB_FIXEDADDR == STD_ON)
#define MCU_START_SEC_CONST_32BIT
#include "MemMap.h"

/* To store the Mcu driver configuration pointer */
extern const Mcu_ConfigType * const Mcu_kConfigPtr;

#define MCU_STOP_SEC_CONST_32BIT
#include "MemMap.h"
#endif /* MCU_PB_FIXEDADDR == STD_ON */


/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/
#define MCU_START_SEC_CODE
#include "MemMap.h"

#if (MCU_ENABLE_CLOCK_MONITORING == STD_ON)
void Mcu_lInitClockMonitoring(void);
#endif

extern Std_ReturnType Mcu_lPlatformInit(void);

extern Mcu_ResetType Mcu_lGetPlatformRstReason(Mcu_RawResetType RstReasonRaw);

#if (MCU_DMA_USED == STD_ON)
extern Std_ReturnType Mcu_lDmaInit (void);

#if (MCU_DEINIT_API == STD_ON)
extern void Mcu_lDmaDeInit (void);
#endif/* MCU_DEINIT_API == STD_ON */
#endif/* MCU_DMA_USED == STD_ON */


#if (MCU_CRC_HW_USED == STD_ON)

extern Std_ReturnType Mcu_lCrcInit (void);
extern Std_ReturnType Mcu_lCrcPlatformInit (void);
extern void Crc_lCfgInit (void);

#if (MCU_DEINIT_API == STD_ON)
extern void Mcu_lCrcDeInit (void);
#endif/*MCU_DEINIT_API == STD_ON*/
#endif/*MCU_CRC_HW_USED == STD_ON*/


/* Functions to enter low power modes */
extern void Mcu_lSetIdleMode(uint8 CoreId);

extern void Mcu_lSetSleepMode(uint8 CoreId);

extern void Mcu_lSetStandbyMode(uint8 CoreId);


#if (MCU_SAFETY_ENABLE == STD_ON)

extern Std_ReturnType Mcu_lPlatformInitCheck(void);

#if (MCU_DMA_USED == STD_ON)
extern Std_ReturnType Mcu_lDmaInitCheck(void);
#endif

#if (MCU_CRC_HW_USED == STD_ON)
extern Std_ReturnType Mcu_lCrcInitCheck(void);
#endif

#if (MCU_CRC_HW_USED == STD_ON)
extern Std_ReturnType Mcu_lCrcPlatformInitCheck(void);
#endif

#if (MCU_CRC_HW_USED == STD_ON)
extern Std_ReturnType Crc_lCfgInitCheck (void);
#endif

#endif /* (MCU_SAFETY_ENABLE == STD_ON) */

/*******************************************************************************
**                      Global Inline Function Definitions                    **
*******************************************************************************/

#if (MCU_DEV_ERROR_DETECT == STD_ON)
/******************************************************************************
** Traceability : [cover parentID=DS_AS_MCU101, DS_AS_MCU125 ]               **
**                                                                           **
** Syntax :           _IFXEXTERN_ IFX_INLINE uint32 Mcu_lInitCallVerify       **
**                                  (                                        **
**                                    uint8 ServiceId)                       **
**                                  )                                        **
** [/cover]                                                                  **
** Service ID:        none                                                   **
**                                                                           **
** Sync/Async:        Synchronous                                            **
**                                                                           **
** Reentrancy:        reentrant                                              **
**                                                                           **
** Parameters (in):   ServiceId - API Service Id number                      **
**                                                                           **
** Parameters (out):   None                                                  **
**                                                                           **
** Return value:      Error - Informs whether error has found or not         **
**                                                                           **
** Description :      Local function :Service to verify whether Init has been**
**                                    called or report DET error             **
**                                                                           **
*****************************************************************************/
_IFXEXTERN_ IFX_INLINE uint32 Mcu_lInitCallVerify(uint8 ServiceId)
/*IFX_MISRA_RULE_08_05_STATUS=Allowed for inline functions defined
 in header files*/
{

  /*IFX_MISRA_RULE_08_05_STATUS=Allowed for inline functions defined
   in header files for ErrorFlag*/
  uint32 ErrorFlag;
  ErrorFlag = MCU_ERR_NOT_FOUND;
  /* Verify whether Init had been called */

  if (Mcu_DriverState !=  MCU_DRIVER_INITIALIZED)
  {
    Det_ReportError(
                     (uint16)MCU_MODULE_ID,
                     MCU_MODULE_INSTANCE,
                     ServiceId,
                     MCU_E_UNINIT
                   );/* End of report to  DET */
    ErrorFlag = MCU_ERR_FOUND;
  }
  return (ErrorFlag);
}/* End of Mcu_lInitCallVerify() function */
#endif /*End Of MCU_DEV_ERROR_DETECT*/

#define MCU_STOP_SEC_CODE
#include "MemMap.h"

#endif /* End of MCU_LOCAL_H */
