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
**  $FILENAME   : Mcu_Crc.c $                                                **
**                                                                            **
**  $CC VERSION : \main\22 $                                                 **
**                                                                            **
**  $DATE       : 2015-08-28 $                                               **
**                                                                            **
**  AUTHOR      : DL-AUTOSAR-Engineering                                      **
**                                                                            **
**  VENDOR      : Infineon Technologies                                       **
**                                                                            **
**  DESCRIPTION : This file contains basic initialization of CRC(FCE) module. **
**                                                                            **
**  MAY BE CHANGED BY USER [yes/no]: No                                       **
**                                                                            **
*******************************************************************************/
/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/

#include "Mcu.h"
/* Inclusion of controller sfr file */
#include "IfxFce_reg.h"
#include "Mcu_Local.h"

/*******************************************************************************
**                      Global Constant Definitions                           **
*******************************************************************************/
#define MCU_CRC32_INIT_CONFIG                (0x00000700U)
#define MCU_CRC_KERNEL_RESET_WAIT_COUNTS     (0x00000100U)
/*******************************************************************************
**                      Global Variable Definitions                           **
*******************************************************************************/
#ifdef IFX_MCU_DEBUG
extern volatile uint32 TestMcu_DebugMask;
#endif
#ifdef IFX_MCU_DEBUG02
extern volatile uint32 TestMcu_DebugMask02;
#endif
/*******************************************************************************
**                      Private Constant Definitions                          **
*******************************************************************************/

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/


/*******************************************************************************
**                      Private Variable Definitions                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
/*Memory Map of the MCU Code*/
#define MCU_START_SEC_CODE
#include "MemMap.h"

#if (MCU_CRC_HW_USED == STD_ON)

/*******************************************************************************
** Syntax : Std_ReturnType Mcu_lCrcInit (void)                                **
**                                                                            **
** Service ID:    None                                                        **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Non-reentrant                                               **
**                                                                            **
** Parameters (in):   None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value:      E_NOT_OK - Error in enabling CLC                        **
**                    E_OK - CLC is enabled                                   **
**                                                                            **
** Description :  This service shall initialize FCE clock and configure the   **
**                configured FCE Kernels.                                     **
**                                                                            **
*******************************************************************************/
Std_ReturnType Mcu_lCrcInit(void)
{
  Std_ReturnType RetVal;
  uint32 Readback;

  /* Enable the FCE clock */
  Mcal_ResetENDINIT();
  MODULE_FCE0.CLC.B.DISR = 0U;
  Mcal_SetENDINIT();

  Readback = (uint32)MODULE_FCE0.CLC.B.DISS;
  #ifdef IFX_MCU_DEBUG
  Readback |= TestMcu_DebugMask;
  #endif
  if(Readback == 0U)
  {
    #if (MCU_CRC8_HW_MODE == STD_ON)
    /* Configure CFG Register for respective configured kernels */
    Crc_lCfgInit();
    #endif

    #if ((MCU_CRC16_HW_MODE == STD_ON)||(MCU_CRC32_HW_MODE == STD_ON))
    Mcal_ResetENDINIT();

    #if (MCU_CRC16_HW_MODE == STD_ON)
    FCE0_IN2_CFG.U = 0U;
    #endif

    #if (MCU_CRC32_HW_MODE == STD_ON)
    FCE0_IN0_CFG.U = (unsigned_int)MCU_CRC32_INIT_CONFIG;
    #endif

    Mcal_SetENDINIT();
    #endif /*End for MCU_CRC16_HW_MODE OR MCU_CRC32_HW_MODE */
    RetVal = E_OK;
  }
  else
  {
    RetVal = E_NOT_OK;
  }

return(RetVal);
}/*End of Mcu_lCrcInit()*/

/*******************************************************************************
** Syntax : void Crc_lCfgInit (void)                                          **
**                                                                            **
** Service ID:    None                                                        **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Non-reentrant                                               **
**                                                                            **
** Parameters (in):   None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value:      None                                                    **
**                                                                            **
** Description :  This service shall initialize FCE Cfg Initialization        **
**                AUTOSAR specific initialization done here                   **
**                                                                            **
*******************************************************************************/
void Crc_lCfgInit(void)
{
#if (MCU_CRC8_HW_MODE == STD_ON)
  /* Initizlize CFG register */
  Mcal_ResetENDINIT();
  FCE0_IN3_CFG.U = 0U;
  FCE0_IN3_CFG.B.XSEL = 1U;
  Mcal_SetENDINIT();
#endif
}/*End of Crc_lCfgInit()*/

#if (MCU_DEINIT_API == STD_ON)
/*******************************************************************************
** Syntax : void Mcu_lCrcDeInit (void)                                        **
**                                                                            **
** Service ID:    None                                                        **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Non-reentrant                                               **
**                                                                            **
** Parameters (in):   None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value:      None                                                    **
**                                                                            **
** Description :  This service shall de-initialize FCE and
                  remove the clock connection                                 **
**                                                                            **
*******************************************************************************/
void Mcu_lCrcDeInit(void)
{
  volatile uint32 ReadBack;
  uint32 WaitCount,RstStatus;

  Mcal_ResetENDINIT();
  FCE_KRST0.B.RST = 1U;
  ReadBack = (uint32)FCE_KRST0.U;
  FCE_KRST1.B.RST = 1U;
  ReadBack = (uint32)FCE_KRST1.U;
  Mcal_SetENDINIT();

  WaitCount = (uint32)MCU_CRC_KERNEL_RESET_WAIT_COUNTS;
  do
  {
    WaitCount--;
    RstStatus = (uint32)FCE_KRST0.B.RSTSTAT;
    #ifdef IFX_MCU_DEBUG02
    RstStatus = (uint32)(FCE_KRST0.B.RSTSTAT & TestMcu_DebugMask02);
    #endif
  }while((RstStatus == 0U)&&(WaitCount > 0U));

  Mcal_ResetENDINIT();
  /* Clear the kernel reset status bit */
  FCE_KRSTCLR.B.CLR = 1U;
  ReadBack = (uint32)FCE_KRSTCLR.U;

  /* Disable FCE clock */
  MODULE_FCE0.CLC.B.DISR = 1U;
  ReadBack = (uint32)MODULE_FCE0.CLC.U;

  Mcal_SetENDINIT();

  /* Read back kernel reset clear register */
  UNUSED_PARAMETER(ReadBack)
}/*End of Mcu_lCrcDeInit()*/
#endif /*End of MCU_DEINIT_API*/

#if (MCU_SAFETY_ENABLE == STD_ON)
/*******************************************************************************
** Syntax : Std_ReturnType Mcu_lCrcInitCheck (void)                           **
**                                                                            **
** Service ID:    None                                                        **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Non-reentrant                                               **
**                                                                            **
** Parameters (in):   None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value    :    E_OK - if initialization comparison is success        **
**                      E_NOT_OK - if initialization comparison fails         **
**                                                                            **
** Description :  This service shall verify the DMA specific                  **
**                initialization done by MCU.                                 **
**                                                                            **
*******************************************************************************/
Std_ReturnType Mcu_lCrcInitCheck(void)
{
  Std_ReturnType ErrorFlag = E_OK;
  uint32 Readback;
  #if (MCU_CRC32_HW_MODE == STD_ON) || (MCU_CRC16_HW_MODE == STD_ON)
  uint32         RegVal;
  #endif
  Readback = (uint32)MODULE_FCE0.CLC.B.DISS;
  if(Readback == 0U)
  {
    ErrorFlag = Crc_lCfgInitCheck();

    #if (MCU_CRC16_HW_MODE == STD_ON)
    RegVal = MODULE_FCE0.IN2.CFG.U;
    if(RegVal != 0U)
    {
      ErrorFlag = E_NOT_OK;
    }
    #endif /*End for MCU_CRC16_HW_MODE*/
    #if (MCU_CRC32_HW_MODE == STD_ON)
    RegVal = MODULE_FCE0.IN0.CFG.U;
    /* Verify XSEL, REFIN, REFOUT bits */
    if(RegVal != ((uint32)7U << 8U))
    {
      ErrorFlag = E_NOT_OK;
    }
    #endif/*End for MCU_CRC32_HW_MODE*/
  }
  else
  {
    ErrorFlag = E_NOT_OK;
  }
  return ErrorFlag;
}/*End of Mcu_lCrcInitCheck()*/
#endif /*End Of MCU_SAFETY_ENABLE*/

#if (MCU_SAFETY_ENABLE == STD_ON)
/*******************************************************************************
** Syntax : Std_ReturnType Crc_lCfgInitCheck (void)                           **
**                                                                            **
** Service ID:    None                                                        **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Non-reentrant                                               **
**                                                                            **
** Parameters (in):   None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value    :    E_OK - if initialization comparison is success        **
**                      E_NOT_OK - if initialization comparison fails         **
**                                                                            **
** Description :  This service shall verify FCE Cfg Initialization            **
**                                                                            **
*******************************************************************************/
Std_ReturnType Crc_lCfgInitCheck(void)
{
  Std_ReturnType ErrorFlag = E_OK;
  #if (MCU_CRC8_HW_MODE == STD_ON)
  uint32         TempFlag;
  #endif

  #if (MCU_CRC8_HW_MODE == STD_ON)
  TempFlag = MODULE_FCE0.IN3.CFG.U;
  if (TempFlag != ((uint32)1U << 10U)) /* XSEL bit */
  {
    ErrorFlag = E_NOT_OK;
  }
  #endif

  return ErrorFlag;
}/*End of Crc_lCfgInitCheck()*/
#endif /*End Of MCU_SAFETY_ENABLE */

#endif /*End of MCU_CRC_HW_USED */

#define MCU_STOP_SEC_CODE
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives
 is allowed only for MemMap.h*/
#include "MemMap.h"

