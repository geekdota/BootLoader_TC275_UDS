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
**   $FILENAME   : Mcal_TcLib.c $                                             **
**                                                                            **
**   $CC VERSION : \main\49 $                                                 **
**                                                                            **
**   $DATE       : 2015-10-16 $                                               **
**                                                                            **
**   AUTHOR      : DL-AUTOSAR-Engineering                                     **
**                                                                            **
**   VENDOR      : Infineon Technologies                                      **
**                                                                            **
**   DESCRIPTION : This file contains Mcal Tricore library routines           **
**                                                                            **
**   SPECIFICATION(S) :    NA                                                 **
**                                                                            **
**   MAY BE CHANGED BY USER [yes/no]: no                                      **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "Std_Types.h"
#include "IfxCpu_reg.h"
#include "IfxCpu_bf.h"
#include "IfxSrc_reg.h"
#include "IfxScu_reg.h"
/* Own header file */
#include "Mcal.h"
#include "Mcal_Options.h"


/*******************************************************************************
**                      File Inclusion Check                                  **
*******************************************************************************/
#ifndef MCAL_SW_MAJOR_VERSION
#error "MCAL_SW_MAJOR_VERSION is not defined. "
#endif

#ifndef MCAL_SW_MINOR_VERSION
  #error "MCAL_SW_MINOR_VERSION is not defined. "
#endif

#ifndef MCAL_SW_PATCH_VERSION
#error "MCAL_SW_PATCH_VERSION is not defined. "
#endif

#if ( MCAL_SW_MAJOR_VERSION != 1 )
  #error "MCAL_SW_MAJOR_VERSION does not match. "
#endif

#if ( MCAL_SW_MINOR_VERSION != 0 )
  #error "MCAL_SW_MINOR_VERSION does not match. "
#endif

/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/
#define MCAL_DBGST_HALT  (0U)
#define MCAL_DBGST_RUN   (1U)
#define MCAL_PMST_NORMAL (1U)
#define MCAL_PMST_IDLE   (3U)


/* Macros for Mcal_GetDsprReMapAddress */
#define MCAL_DSPR_GLOBAL_MASK         (0xF0000000U)
#define MCAL_DSPR_GLOBAL_MASK_N       (~MCAL_DSPR_GLOBAL_MASK)
#define MCAL_VALID_DSPR_MSB           (0xD0000000U)
/*Local mask of 0x0003FFFFU is chosen to work with all ranges 72 - 240kb */
#define MCAL_DSPR_LOCAL_MASK          (0x0003FFFFU)

#define MCAL_CORE0_DSPR_BASE_ADDR     (0x70000000U)
#define MCAL_CORE1_DSPR_BASE_ADDR     (0x60000000U)
#define MCAL_CORE2_DSPR_BASE_ADDR     (0x50000000U)

#define MCAL_SAFETY_ENDINT_TIMEOUT ((uint32)150000U)

/*******************************************************************************
**                      Private Variable Definitions                          **
*******************************************************************************/
#if (IFX_MCAL_USED == STD_ON)
#define MCAL_TCLIB_START_SEC_VAR_INIT_32BIT
#include "MemMap.h"
#else
#define IFX_MCAL_TCLIB_START_SEC_VAR_INIT_32BIT_ASIL_B
#include "Ifx_MemMap.h"
#endif

#define _GNU_C_TRICORE_ 1

#ifndef OS_KERNEL_TYPE

/* Variables are not used when OS_KERNEL_TYPE is defined */
/* used for nesting enable/disable management */
#if ( MCAL_NO_OF_CORES == 3U )
static uint32 Mcal_SavedIntState[MCAL_NO_OF_CORES] = {0U,0U,0U};
static uint32 Mcal_IntDisableCounter[MCAL_NO_OF_CORES] = {0U,0U,0U};
static uint32 Mcal_SavedIntStateRedn[MCAL_NO_OF_CORES] = {0xFFFFFFFFU,
                                                          0xFFFFFFFFU,
                                                          0xFFFFFFFFU};
static uint32 Mcal_IntDisableCounterRedn[MCAL_NO_OF_CORES] = {0xFFFFFFFFU,
                                                              0xFFFFFFFFU,
                                                              0xFFFFFFFFU};

#elif ( MCAL_NO_OF_CORES == 2U )
static uint32 Mcal_SavedIntState[MCAL_NO_OF_CORES] = {0U,0U};
static uint32 Mcal_IntDisableCounter[MCAL_NO_OF_CORES] = {0U,0U};
static uint32 Mcal_SavedIntStateRedn[MCAL_NO_OF_CORES] = {0xFFFFFFFFU,
                                                          0xFFFFFFFFU};
static uint32 Mcal_IntDisableCounterRedn[MCAL_NO_OF_CORES] = {0xFFFFFFFFU,
                                                              0xFFFFFFFFU};

#else /*( MCAL_NO_OF_CORES == 1U )*/
static uint32 Mcal_SavedIntState[MCAL_NO_OF_CORES] = {0U};
static uint32 Mcal_IntDisableCounter[MCAL_NO_OF_CORES] = {0U};
static uint32 Mcal_SavedIntStateRedn[MCAL_NO_OF_CORES] = {0xFFFFFFFFU};
static uint32 Mcal_IntDisableCounterRedn[MCAL_NO_OF_CORES] = {0xFFFFFFFFU};
#endif /*End for MCAL_NO_OF_CORES */


#endif /*End for OS_KERNEL_TYPE */

#if (IFX_MCAL_USED == STD_ON)
#define MCAL_TCLIB_STOP_SEC_VAR_INIT_32BIT
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives is
 allowed only for MemMap.h*/
#include "MemMap.h"
#else
#define IFX_MCAL_TCLIB_STOP_SEC_VAR_INIT_32BIT_ASIL_B
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives is
 allowed only for MemMap.h*/
#include "Ifx_MemMap.h"
#endif
/*
 * To be used for all global or static variables
 * that are never  initialized.
 */
#if (IFX_MCAL_USED == STD_ON)
#define MCAL_TCLIB_START_SEC_CONST_32BIT
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives is
 allowed only for MemMap.h*/
#include "MemMap.h"
#else
#define IFX_MCAL_TCLIB_START_SEC_CONST_32BIT_ASIL_B
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives is
 allowed only for MemMap.h*/
#include "Ifx_MemMap.h"
#endif

#if ( MCAL_NO_OF_CORES > 1U )
/* used only if no of cores is greater than 1 */
/*IFX_MISRA_RULE_08_07_STATUS=The variable Mcal_CpuAddressMap is made
 global to reduce the code complexity*/
static Ifx_CPU* const Mcal_CpuAddressMap[MCAL_NO_OF_CORES] =
{
  &MODULE_CPU0

  #if ( MCAL_NO_OF_CORES > 1U )
  ,&MODULE_CPU1

  #if ( MCAL_NO_OF_CORES == 3U )
  ,&MODULE_CPU2
  #endif /* ( MCAL_NO_OF_CORES == 3U )  */

  #endif /* ( MCAL_NO_OF_CORES > 1U ) */
};
#endif /*End for MCAL_NO_OF_CORES*/

#if (IFX_MCAL_USED == STD_ON)
#define MCAL_TCLIB_STOP_SEC_CONST_32BIT
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives is
 allowed only for MemMap.h*/
#include "MemMap.h"
#else
#define IFX_MCAL_TCLIB_STOP_SEC_CONST_32BIT_ASIL_B
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives is
 allowed only for MemMap.h*/
#include "Ifx_MemMap.h"
#endif


#if (IFX_MCAL_USED == STD_ON)
/*Memory Map of the Code*/
#define MCAL_TCLIB_START_SEC_CODE
/*
  Allows to map variables, constants and code of modules to individual
  memory sections.
*/
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives is
 allowed only for MemMap.h*/
#include "MemMap.h"
#else
#define IFX_MCAL_TCLIB_START_SEC_CODE_ASIL_B
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives is
 allowed only for MemMap.h*/
#include "Ifx_MemMap.h"
#endif

/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/
#if ( MCAL_NO_OF_CORES > 1U )
static Ifx_CPU* Mcal_GetCpuAddress(uint8 CpuId);
#endif /*End for MCAL_NO_OF_CORES */



IFX_INLINE unsigned int cmpswap_w (unsigned int volatile *address,
           unsigned int value, unsigned int condition)
{
  __extension__ unsigned long long reg64
    = value | (unsigned long long) condition << 32;

  __asm__ __volatile__ ("cmpswap.w [%[addr]]0, %A[reg]"
                        : [reg] "+d" (reg64)
                        : [addr] "a" (address)
                        : "memory");
    return reg64;
}

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/
/*******************************************************************************
** Traceability     : [cover parentID=SAS_NAS_MCALLIB_PR122_13 ]        [/cover]
** Syntax : uint32 Mcal_GetCoreId(void)                                       **
**                                                                            **
** Service ID:  None                                                          **
**                                                                            **
** Sync/Async:  Synchronous                                                   **
**                                                                            **
** Reentrancy:  Reentrant                                                     **
**                                                                            **
** Parameters (in) :  None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value    :  Core ID                                                 **
**                                                                            **
** Description : This function returns the core ID of the core on which       **
              it is executing.                                                **
*******************************************************************************/
uint8 Mcal_GetCoreId(void)
{
  uint8 RetVal;

  RetVal = (uint8)MFCR(CPU_CORE_ID);
  return RetVal;
}

/*******************************************************************************
** Traceability     : [cover parentID=SAS_NAS_MCALLIB_PR122_14 ]        [/cover]
** Syntax : uint32 Mcal_GetDsprReMapAddress(uint32 Address)                   **
**                                                                            **
** Service ID:  None                                                          **
**                                                                            **
** Sync/Async:  Synchronous                                                   **
**                                                                            **
** Reentrancy:  Reentrant                                                     **
**                                                                            **
** Parameters (in) :  None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value    :  Remapped DSPR address                                   **
**                                                                            **
** Description : This function maps the DSPR address to the global address    **
              which can be used by other cores or Dma                         **
*******************************************************************************/
uint32 Mcal_GetDsprReMapAddress(uint32 Address)
{
  uint32 RetAddress;
  uint8  CoreId;


  /*Initialise Return value with the input global address*/
  RetAddress = Address;

  if ((Address & MCAL_DSPR_GLOBAL_MASK) == MCAL_VALID_DSPR_MSB)
  {
    CoreId = Mcal_GetCoreId();
    #if defined MCALLIB_DEBUG1
    #if (MCALLIB_DEBUG1 == STD_ON)
    CoreId = 1U;
    #endif
    #elif defined MCALLIB_DEBUG2
    #if (MCALLIB_DEBUG2 == STD_ON)
    CoreId = 2U;
    #endif
    #endif

    switch(CoreId)
    {
      case 0:
       if( (Address & MCAL_DSPR_GLOBAL_MASK_N) < MCAL_DSPR0_SIZE)
       {
         RetAddress = ((Address & MCAL_DSPR_LOCAL_MASK) |
                        MCAL_CORE0_DSPR_BASE_ADDR);
       }
       break;

      case 1:
      #if ( MCAL_NO_OF_CORES > 1U )
        if( (Address & MCAL_DSPR_GLOBAL_MASK_N) < MCAL_DSPR1_SIZE)
        {
          RetAddress = ((Address & MCAL_DSPR_LOCAL_MASK) |
                         MCAL_CORE1_DSPR_BASE_ADDR);
          /*TC24x and lower - core 1 Not Available*/
        }
      #endif
        break;

      default:
      #if ( MCAL_NO_OF_CORES == 3U )
        if( (Address & MCAL_DSPR_GLOBAL_MASK_N) < MCAL_DSPR2_SIZE)
        {
         RetAddress = ((Address & MCAL_DSPR_LOCAL_MASK) |
                        MCAL_CORE2_DSPR_BASE_ADDR);
         /*TC26x and lower - core 2 Not Available*/
        }
      #endif
        break;
    }
  }
  return RetAddress;
}

#if ( MCAL_NO_OF_CORES > 1U )
/*******************************************************************************
** Syntax : Ifx_CPU* Mcal_GetCpuAddress(uint8 CpuId)                          **
**                                                                            **
** Service ID:  None                                                          **
**                                                                            **
** Sync/Async:  Synchronous                                                   **
**                                                                            **
** Reentrancy:  Reentrant                                                     **
**                                                                            **
** Parameters (in) :  None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value    :  CPU base address                                        **
**                                                                            **
** Description : This function get the CPU base address of the provided CPU ID**
*******************************************************************************/
static Ifx_CPU* Mcal_GetCpuAddress(uint8 CpuId)
{
  Ifx_CPU* module;

  /* Get the base address of the CPU from the structure */
  module = Mcal_CpuAddressMap[CpuId];

  return(module);
}
#endif /*End for MCAL_NO_OF_CORES */
#if ( MCAL_NO_OF_CORES > 1U )
/*******************************************************************************
** Syntax : void Mcal_SetCpuPC (uint8 CpuNo, uint32 ProgramCounter)           **
**                                                                            **
** Service ID:  None                                                          **
**                                                                            **
** Sync/Async:  Synchronous                                                   **
**                                                                            **
** Reentrancy:  Reentrant                                                     **
**                                                                            **
** Parameters (in) :  CpuNo - CPU index                                       **
**                    ProgramCounter - Address to be updated in PC register   **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value    :  None                                                    **
**                                                                            **
** Description : This function updates the PC register of the corresponding   **
**               CPU with the provided address                                **
*******************************************************************************/
void Mcal_SetCpuPC (uint8 CpuNo, uint32 ProgramCounter)
{
  Ifx_CPU* CpuBase;

  if( CpuNo < MCAL_NO_OF_CORES )
  {
    /* Get the base address of the CPU */
    CpuBase = Mcal_GetCpuAddress(CpuNo);
    /* Set the PC */
    CpuBase->PC.U = ProgramCounter;
  }
}
#endif /*End for MCAL_NO_OF_CORES */
#if ( MCAL_NO_OF_CORES > 1U )
/*******************************************************************************
** Traceability     : [cover parentID=]        [/cover]                       **
** Syntax : void Mcal_StartCore (uint8 CpuNo, uint32 Pcval)                   **
**                                                                            **
** Service ID:  None                                                          **
**                                                                            **
** Sync/Async:  Synchronous                                                   **
**                                                                            **
** Reentrancy:  Reentrant                                                     **
**                                                                            **
** Parameters (in) :  CpuNo - CPU index                                       **
**                    Pcval - Address to be updated in PC register            **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value    :  None                                                    **
**                                                                            **
** Description : This function updates the PC register of the corresponding   **
**               CPU with the provided address and puts the CPU to run mode   **
*******************************************************************************/
void Mcal_StartCore (uint8 CpuNo, uint32 Pcval)
{
  Ifx_CPU* CpuBase;
  volatile Ifx_CPU_DBGSR DbgsrValue;

  if( CpuNo < MCAL_NO_OF_CORES )
  {
    CpuBase = Mcal_GetCpuAddress(CpuNo);

    /* Set the PC for the Core*/
    Mcal_SetCpuPC (CpuNo, Pcval);

    /* Set the CPU to run mode */
    DbgsrValue.U = CpuBase->DBGSR.U;
    DbgsrValue.B.HALT = 0x2U;
    CpuBase->DBGSR.U = DbgsrValue.U;
  }
}
#endif /*End for MCAL_NO_OF_CORES */
/*******************************************************************************
** Syntax : uint32 Mcal_LockResource(uint32 *ResourcePtr)                     **
**                                                                            **
** Service ID:  None                                                          **
**                                                                            **
** Sync/Async:  Synchronous                                                   **
**                                                                            **
** Reentrancy:  Reentrant                                                     **
**                                                                            **
** Parameters (in) :  ResourcePtr - Pointer to variable representing the      **
**                    resource                                                **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value     : MCAL_RESOURCE_BUSY-If Resource was already locked       **
**                    MCAL_RESOURCE_FREE-If Resource is free for use          **
**                                                                            **
** Description      : This function is a implementation of a binary semaphore **
**                    using the "CmpAndswap" instruction of tricore.          **
**                    Allowed values are 0 and 1 for this binary semaphore.   **
**                    If a resource is free, the semaphore is taken and       **
**                    status MCAL_RESOURCE_FREE is returned.                  **
**                    If the resource was already taken,                      **
**                    a busy status (MCAL_RESOURCE_BUSY) is returned.         **
*******************************************************************************/
uint32 Mcal_LockResource(uint32 *ResourcePtr)
{
  /*
  Sets semaphore at address res_status automatically to MCAL_RESOURCE_BUSY = 1
  and returns the previous semaphore state.
  */
  uint32 RetVal;

  RetVal = (uint32)MCAL_RESOURCE_BUSY;

  if( ResourcePtr != NULL_PTR )
  {
    RetVal = (uint32)Mcal_CmpAndSwap((unsigned_int*)(void*)ResourcePtr,RetVal,MCAL_RESOURCE_FREE);
  }

  return (RetVal);
}

/*******************************************************************************
** Syntax : void  Mcal_UnlockResource(uint32* ResourcePtr)                    **
**                                                                            **
** Service ID:  None                                                          **
**                                                                            **
** Sync/Async:  Synchronous                                                   **
**                                                                            **
** Reentrancy:  Reentrant                                                     **
**                                                                            **
** Parameters (in) :  ResourcePtr - Pointer to variable representing the      **
**                    resource                                                **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value    :  None                                                    **
**                                                                            **
** Description      : This function Frees the Resource already locked .       **
*******************************************************************************/
void  Mcal_UnlockResource(uint32* ResourcePtr)
{
  if( ResourcePtr != NULL_PTR )
  {
    /*
    Sets semaphore at address res_status atomically to MCAL_RESOURCE_FREE = 0.
    */
    *ResourcePtr = (uint32)MCAL_RESOURCE_FREE;
  }
}

/*******************************************************************************
** Traceability     : [cover parentID=]        [/cover]                       **
**                                                                            **
** Syntax           : Std_ReturnType Mcal_GetSpinLock                         **
**                    (                                                       **
**                      uint32* SpinLckPtr,uint32 Timeout                     **
**                    )                                                       **
**                                                                            **
** Service ID       : None                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters(in)   : SpinLckPtr: Spinlock to be acquired                     **
**                    Timeout: Wait duration for acquisition of SpinLock      **
** Parameters (out) : None                                                    **
**                                                                            **
** Return value     : None                                                    **
**                                                                            **
** Description      :                                                         **
**                                                                            **
*******************************************************************************/
Std_ReturnType Mcal_GetSpinLock(uint32* SpinLckPtr,uint32 Timeout)
{
  Std_ReturnType RetVal;
  uint32 SpinLockVal;

  RetVal = E_NOT_OK;

  if( SpinLckPtr != NULL_PTR )
  {
    do
    {

      SpinLockVal = 1UL;
      SpinLockVal = (uint32)
          Mcal_CmpAndSwap((unsigned_int*)(void*)SpinLckPtr,SpinLockVal,0U);

      /* Check if the SpinLock WAS set before the attempt to acquire spinlock */
      if(SpinLockVal == 0U)
      {
        RetVal = E_OK;
      }
      else
      {
        Timeout-- ;

      }
    } while((RetVal == E_NOT_OK) && (Timeout > 0U));
  }

  return(RetVal);
}

/*******************************************************************************
** Traceability     : [cover parentID=]        [/cover]                       **
**                                                                            **
** Syntax           : void Mcal_ReleaseSpinLock                               **
**                    (                                                       **
**                      uint32* SpinLckPtr                                    **
**                    )                                                       **
**                                                                            **
** Service ID       : None                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters(in)   : SpinLckPtr: Spinlock to be released                     **
**                                                                            **
** Parameters (out) : None                                                    **
**                                                                            **
** Return value     : None                                                    **
**                                                                            **
** Description      :                                                         **
**                                                                            **
*******************************************************************************/
void Mcal_ReleaseSpinLock(uint32* SpinLckPtr)
{
  if( SpinLckPtr != NULL_PTR )
  {
    /* Reset the SpinLock */
    *SpinLckPtr = 0U;
  }
}

/*******************************************************************************
** Traceability     : [cover parentID=]        [/cover]                       **
**                                                                            **
** Syntax           : void Mcal_SuspendAllInterrupts(void)                    **
**                                                                            **
** Service ID       : None                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters(in)   : None                                                    **
**                                                                            **
** Parameters (out) : None                                                    **
**                                                                            **
** Return value     : None                                                    **
**                                                                            **
** Description      : Mcal_SuspendAllInterrupts, disables all interrupts      **
**                    Nesting disable/enable supported                        **
**                    This API shall not be called prior to C-init            **
*******************************************************************************/
void Mcal_SuspendAllInterrupts(void)
{
  #ifndef OS_KERNEL_TYPE
  uint32 CoreID;
  uint32 ICRState;
  uint32 ICRStateRedn;
  #endif

  #ifdef OS_KERNEL_TYPE
    SuspendAllInterrupts();
  #else
  CoreID = (uint32)MFCR(CPU_CORE_ID);

  if(Mcal_IntDisableCounter[CoreID] == 0U)
  {
    ICRState = (uint32)MFCR(CPU_ICR);

    ICRStateRedn = (uint32)(~(uint32)ICRState);

    /* disable interrupts */
    Mcal_DisableAllInterrupts();

    /* At this point Interrupts are already disabled */
    Mcal_SavedIntState[CoreID] = ICRState;
    Mcal_SavedIntStateRedn[CoreID] = ICRStateRedn;
  }

  Mcal_IntDisableCounter[CoreID]++;
  Mcal_IntDisableCounterRedn[CoreID]--;

  /* The check is done at the end as interrupts are disabled
     either by this call or previously*/
  /* Inconsistency is reported.It is NOT expected to return from the callback*/
  /* Incase, there is a return, proceed with normal functioning */
  if(Mcal_IntDisableCounter[CoreID] != (~Mcal_IntDisableCounterRedn[CoreID]))
  {
     Mcal_SafeErrorHandler(MCAL_DIV_INCONSISTENCY);
  }
  #endif
}

/*******************************************************************************
** Traceability     : [cover parentID=]        [/cover]                       **
**                                                                            **
** Syntax           : void Mcal_ResumeAllInterrupts( void )                   **
**                                                                            **
** Service ID       : None                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters(in)   : None                                                    **
**                                                                            **
** Parameters (out) : None                                                    **
**                                                                            **
** Return value     : None                                                    **
**                                                                            **
** Description      : Mcal_ResumeAllInterrupts, re-enables interrupts         **
**                    Nesting disable/enable supported                        **
**                    This API shall not be called prior to C-init            **
*******************************************************************************/
void Mcal_ResumeAllInterrupts(void)
{
  #ifndef OS_KERNEL_TYPE
  uint32 CoreID;
  uint32 IntDisableCntr;
  uint32 IntDisableCntrRedn;
  uint32 SavedIntState;
  uint32 SavedIntStateRedn;
  #endif

  #ifdef OS_KERNEL_TYPE
    ResumeAllInterrupts();
  #else
  CoreID = (uint32)MFCR(CPU_CORE_ID);

  /* Take local copies of the required global variables */
  IntDisableCntr = Mcal_IntDisableCounter[CoreID];
  IntDisableCntrRedn = Mcal_IntDisableCounterRedn[CoreID];
  SavedIntState = Mcal_SavedIntState[CoreID];
  SavedIntStateRedn = Mcal_SavedIntStateRedn[CoreID];

  /* Check for consistency */
  /* Inconsistency is reported.It is NOT expected to return from the callback */
  /* Incase, there is a return, proceed with normal functioning */
  if((IntDisableCntr != (~IntDisableCntrRedn)) ||
     (SavedIntState != (~SavedIntStateRedn)))
  {
     Mcal_SafeErrorHandler(MCAL_DIV_INCONSISTENCY);
  }

  if(IntDisableCntr > 0U)
  {
    IntDisableCntr-- ;
    IntDisableCntrRedn++;
  }

  Mcal_IntDisableCounter[CoreID] = IntDisableCntr;
  Mcal_IntDisableCounterRedn[CoreID] = IntDisableCntrRedn;


  if(IntDisableCntr == 0U)
  {
    if ((SavedIntState &
        ((uint32)IFX_CPU_ICR_IE_LEN << IFX_CPU_ICR_IE_OFF)) != 0U)
    {
      /* interrupts were enabled, enable again */
      Mcal_EnableAllInterrupts();
    }
  }
  #endif
}


/*******************************************************************************
** Traceability     : [cover parentID=]        [/cover]                       **
**                                                                            **
** Syntax           :  void Mcal_UpdateRegSV (volatile uint32 *Address,       **
**   uint32 SetMask, uint32 ClearMask, uint8 InitProtectionType, uint16 area )**
**                                                                            **
** Service ID       : None                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in) :  Address - Address of the SFR to be modified             **
**                    SetMask - Set mask to be ored with the value            **
**                    ClearMask - Clear mask to be anded with the value       **
**                    InitProtectionType - Type of the protection required    **
**                    for SFR update.                                         **
**                        MCAL_TIN_NO_ENDINIT (0U)                            **
**                        MCAL_TIN_CPU_ENDINIT  (1U)                          **
**                        MCAL_TIN_SAFETY_ENDINIT  (2U)                       **
**                    Area - Module ID of the calling module                  **
** Parameters(out)   : Value referenced by Address will be modified           **
**                                                                            **
** Return value     : None                                                    **
**                                                                            **
** Description      : Mcal_UpdateRegSV calls Apis to put the system in        **
**  supervisory mode. System can be put in supervisory mode either by Mcal API**
** or by an OS API which is decided by IFX_MCAL_RUN_MODE_DEFINE               **
*******************************************************************************/
IFX_NO_INLINE void Mcal_UpdateRegSV (volatile uint32 *Address,\
      uint32 SetMask, uint32 ClearMask, uint8 InitProtectionType, uint16 Area)
{
  #if (IFX_MCAL_RUN_MODE_DEFINE == (2U))
    Os_UpdateRegSV (Address , SetMask , ClearMask , InitProtectionType, Area);
  #else
    Mcal_SetRegValSV (Address , SetMask , ClearMask , InitProtectionType);
    UNUSED_PARAMETER(Area)
  #endif
}

/*******************************************************************************
** Traceability     : [cover parentID=]        [/cover]                       **
**                                                                            **
** Syntax           : void Mcal_SetRegValSV (volatile uint32 *Address,        **
**   uint32 SetMask, uint32 ClearMask, uint8 InitProtectionType)              **
**                                                                            **
** Service ID       : None                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in) :  Address - Address of the SFR to be modified             **
**                    SetMask - Set mask to be ored with the value            **
**                    ClearMask - Clear mask to be anded with the value       **
**                    InitProtectionType - Type of the protection required for**
**                      SFR update.                                           **
**                      MCAL_TIN_NO_ENDINIT (0U)                              **
**                      MCAL_TIN_CPU_ENDINIT  (1U)                            **
**                      MCAL_TIN_SAFETY_ENDINIT  (2U)                         **
** Parameters(out)   : Value referenced by Address will be modified           **
**                                                                            **
** Return value     : None                                                    **
**                                                                            **
** Description      : Mcal_SetRegValSV, updates the value pointed by Address  **
** based on SetMask and ClearMask if IFX_MCAL_RUN_MODE_DEFINE is  0U          **
** (Expression : ((Value) & (~ClearMask) | SetMask))                          **
** Otherwise it makes a systemcall with InitProtectionType as TIN.            **
*******************************************************************************/
IFX_NO_INLINE void Mcal_SetRegValSV (volatile uint32 *Address, \
  uint32 SetMask, uint32 ClearMask, uint8 InitProtectionType)
{
  #if  (IFX_MCAL_RUN_MODE_DEFINE == (0U))
  /*MCAL is running in Supervisor Mode, Syscall is not required */
    if(MCAL_TIN_NO_ENDINIT == InitProtectionType)
    {
      *Address =(uint32)( ((*Address) & (uint32)(~ClearMask)) | (SetMask));
    }
    else if(MCAL_TIN_CPU_ENDINIT == InitProtectionType)
    {
      Mcal_ResetENDINIT();

      *Address =(uint32)( ((*Address) & (uint32)(~ClearMask)) | (SetMask));

      Mcal_SetENDINIT();
    }
    else if(MCAL_TIN_SAFETY_ENDINIT == InitProtectionType)
    {
      Mcal_ResetSafetyENDINIT_Timed(MCAL_SAFETY_ENDINT_TIMEOUT);

      *Address =(uint32)( ((*Address) & (uint32)(~ClearMask)) | (SetMask));

      Mcal_SetSafetyENDINIT_Timed();
    }
    else
    {
      /*Do Nothing*/
    }
  #else
  /*MCAL is running in User-1 Mode, Syscall is required */
    if(MCAL_TIN_NO_ENDINIT == InitProtectionType)
    {
      /*IFX_MISRA_RULE_14_02_STATUS=Syscall will generate Trap Class 6, 
       hence not a null statement*/
      SYSCALL(MCAL_TIN_NO_ENDINIT);
    }
    else if(MCAL_TIN_CPU_ENDINIT == InitProtectionType)
    {
      /*IFX_MISRA_RULE_14_02_STATUS=Syscall will generate Trap Class 6, 
       hence not a null statement*/
      SYSCALL(MCAL_TIN_CPU_ENDINIT);
    }
    else if(MCAL_TIN_SAFETY_ENDINIT == InitProtectionType)
    {
      /*IFX_MISRA_RULE_14_02_STATUS=Syscall will generate Trap Class 6, 
       hence not a null statement*/
      SYSCALL(MCAL_TIN_SAFETY_ENDINIT);
    }
    else
    {
      /*Do Nothing*/
    }
    UNUSED_PARAMETER(*Address)
    UNUSED_PARAMETER(SetMask)
    UNUSED_PARAMETER(ClearMask)
  #endif   /* IFX_MCAL_RUN_MODE_DEFINE */
/*IFX_MISRA_RULE_16_07_STATUS=Parameter Address cannot be passes as a constant,
since it will be updated in SYSCALL */
}

#if (IFX_MCAL_RUN_MODE_DEFINE != (0U))
/*******************************************************************************
** Traceability     : [cover parentID=]        [/cover]                       **
**                                                                            **
** Syntax           : void Mcal_SVModeExecution (volatile uint32 *Address,    **
   uint32 SetMask, uint32 ClearMask, uint8 InitProtectionType)                **
**                                                                            **
** Service ID       : None                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in) :  Address - Address of the SFR to be modified             **
**                    SetMask - Set mask to be ored with the value            **
**                    ClearMask - Clear mask to be anded with the value       **
**                    InitProtectionType - Type of the protection required    **
**                    for SFR update.                                         **
**                      MCAL_TIN_NO_ENDINIT (0U)                              **
**                      MCAL_TIN_CPU_ENDINIT  (1U)                            **
**                      MCAL_TIN_SAFETY_ENDINIT  (2U)                         **
** Parameters(out)   : Value referenced by Address will be modified           **
**                                                                            **
** Return value     : None                                                    **
**                                                                            **
** Description      : Mcal_SVModeExecution, updates the value pointed by      **
** Address based on SetMask and ClearMask. This function will be called from  **
** SYSCALL . This function is not available if MCAL run in Supervisor Mode    **
*******************************************************************************/
IFX_NO_INLINE void Mcal_SVModeExecution (volatile uint32 *Address, \
                   uint32 SetMask, uint32 ClearMask, uint8 InitProtectionType)
{

  if (MCAL_TIN_CPU_ENDINIT == InitProtectionType)
  {
    Mcal_ResetENDINIT();

    *Address =(uint32)( ((*Address) & (uint32)(~ClearMask)) | (SetMask));

    Mcal_SetENDINIT();
  }
  else if (MCAL_TIN_SAFETY_ENDINIT == InitProtectionType)
  {
    Mcal_ResetSafetyENDINIT_Timed(MCAL_SAFETY_ENDINT_TIMEOUT);

    *Address =(uint32)( ((*Address) & (uint32)(~ClearMask)) | (SetMask));

    Mcal_SetSafetyENDINIT_Timed();
  }
  else if(MCAL_TIN_NO_ENDINIT == InitProtectionType)
  {
    *Address =(uint32)(((*Address) & (uint32)(~ClearMask)) | (SetMask));
  }
  else
  {
    /*Do Nothing*/
  }
}

#endif
/* (IFX_MCAL_RUN_MODE_DEFINE != (0U)) */
#if (IFX_MCAL_USED == STD_ON)
/*Memory Map of the Code*/
#define MCAL_TCLIB_STOP_SEC_CODE
/*
  Allows to map variables, constants and code of modules to individual
  memory sections.
*/
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives is
 allowed only for MemMap.h*/
#include "MemMap.h"
#else
#define IFX_MCAL_TCLIB_STOP_SEC_CODE_ASIL_B
/*IFX_MISRA_RULE_19_01_STATUS=File inclusion after pre-processor directives is
 allowed only for MemMap.h*/
#include "Ifx_MemMap.h"
#endif
