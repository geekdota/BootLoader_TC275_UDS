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
**   $FILENAME   : Mcal_TcLib.h $                                             **
**                                                                            **
**   $CC VERSION : \main\31 $                                                 **
**                                                                            **
**   $DATE       : 2015-11-19 $                                               **
**                                                                            **
**   AUTHOR      : DL-AUTOSAR-Engineering                                     **
**                                                                            **
**   VENDOR      : Infineon Technologies                                      **
**                                                                            **
**   DESCRIPTION : This header file exports Mcal Tricore library              **
                   type definitions and functions                             **
**                                                                            **
**   MAY BE CHANGED BY USER [yes/no]: no                                      **
**                                                                            **
*******************************************************************************/

#ifndef MCAL_TCLIB_H
#define MCAL_TCLIB_H

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "Std_Types.h"
#include  "Mcal_Compiler.h"

#ifdef OS_KERNEL_TYPE
#include "McalOsConfig.h"
#include MCAL_OS_HEADER
#endif

/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/
/*
  File version information
*/
#define MCAL_SW_MAJOR_VERSION  (1)
#define MCAL_SW_MINOR_VERSION  (0)
#define MCAL_SW_PATCH_VERSION  (5)

#define TC27x  (1U)
#define TC26x  (2U)
#define TC29x  (3U)

#define AS40    (4U)
#define AS321   (321U)
#define AS402   (402U)
#define AS403   (403U)

#ifndef STD_ON
#define STD_ON     (1U)
#endif

#ifndef STD_OFF
#define STD_OFF    (0U)
#endif

#ifndef DISABLE_DEM_REPORT
#define DISABLE_DEM_REPORT    (0U)
#endif

#ifndef ENABLE_DEM_REPORT
#define ENABLE_DEM_REPORT    (1U)
#endif

/*IFX_MISRA_RULE_19_07_STATUS=Fucntion like macro is used for efficient
 implementation to get mask values*/
#define Mcal_GetSetMask(Value,Mask,BitPos)\
                       (((uint32)(Value) & (uint32)(Mask))<<(BitPos))
/*IFX_MISRA_RULE_19_07_STATUS=Fucntion like macro is used for efficient
 implementation to get mask values*/
#define Mcal_GetClearMask(Value,Mask,BitPos)\
                      (((uint32)(~(uint32)(Value)) & (uint32)(Mask))<<(BitPos))

/* Macro for Unused parameters*/
/*IFX_MISRA_RULE_19_07_STATUS=UNUSED_PARAMETER is declared as a function like
 macro to avoid the function call at every unused parameters*/
#define UNUSED_PARAMETER(VariableName)          {if((VariableName) != 0U)\
                                                {/* Do Nothing */}}
/* defines for Semaphore/s */
#define MCAL_RESOURCE_BUSY (1U)
#define MCAL_RESOURCE_FREE (0U)

#define MCAL_NO_OF_CORES   (3U)

/* DSPR values for 27x chip*/
#define MCAL_DSPR0_SIZE      (0x0001C000U)
#define MCAL_DSPR1_SIZE      (0x0001E000U)
#define MCAL_DSPR2_SIZE      (0x0001E000U)

/*
The following type definitions are compiler specific.
These type definitions are used
for tasking intrinsic function
to be used in bit field structures.
This helps in avoding two MISRA complaints.
Rule 13 and Rule 111
*/

typedef signed int signed_int;

typedef unsigned int unsigned_int;


typedef enum
{
    MCAL_HALT,
    MCAL_RUN,
    MCAL_IDLE,
    MCAL_SLEEP,
    MCAL_STBY,
    MCAL_INVALID
} Mcal_CoreMode;



/*******************************************************************************
** Macro Syntax : Mcal_EnableAllInterrupts()                                  **
**                                                                            **
** Parameters (in) :  None                                                    **
** Parameters (out):  None                                                    **
**                                                                            **
** Description    : Call to Enable All Interrupts                             **
**                                                                            **
*******************************************************************************/
#ifdef OS_KERNEL_TYPE
/* Use Os function is available */
/*IFX_MISRA_RULE_19_07_STATUS=Function like macro used to call the OS API*/
#define Mcal_EnableAllInterrupts()    EnableAllInterrupts()
#else
/* Or use Intrinsic function call otherwise */
/*IFX_MISRA_RULE_19_07_STATUS=Function like macro used to call the Tricore
intrinsic function*/
#define Mcal_EnableAllInterrupts()    (ENABLE())
#endif /* OS_KERNEL_TYPE */

/*******************************************************************************
** Macro Syntax : Mcal_DisableAllInterrupts()                                 **
**                                                                            **
** Parameters (in) :  None                                                    **
** Parameters (out):  None                                                    **
**                                                                            **
** Description    : Call to Disable All Interrupts                            **
**                                                                            **
*******************************************************************************/
#ifdef OS_KERNEL_TYPE
/* Use Os function is available */
/*IFX_MISRA_RULE_19_07_STATUS=Function like macro used to call the OS API*/
#define Mcal_DisableAllInterrupts()   DisableAllInterrupts()
#else
/* Or use Intrinsic function call otherwise */
/*IFX_MISRA_RULE_19_07_STATUS=Function like macro used to call the Tricore
intrinsic function*/
#define Mcal_DisableAllInterrupts()    (DISABLE())
#endif /* OS_KERNEL_TYPE */

/*******************************************************************************
** Macro Syntax : Mcal_SetAtomic(Address, Value, Offset, Bits)                **
**                                                                            **
** Parameters (in) :  Address: Address of the Variable to be Modified         **
**                    Value  : Value to be written                            **
**                    Offset : Bit Offset from LSB                            **
**                    Bits   : No of Bits Modified                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Description    : Mcal_SetAtomic is used to write atomic instructions in    **
**                  code.The function writes the number of bits of an integer **
**                  value at a certain address location in memory with a ...  **
**                  bitoffset. The number of bits must be a constant value... **
**                  Note that the Address must be Word Aligned                **
**                                                                            **
*******************************************************************************/
/*IFX_MISRA_RULE_19_07_STATUS=Function like macro used to call the Tricore
intrinsic function*/
/*IFX_MISRA_RULE_19_04_STATUS=IMASKLDMST cannot be expand to a braced
 initialiser*/
#define Mcal_SetAtomic(Address, Value, Offset, Bits) \
            IMASKLDMST((Address),(Value),(Offset),(Bits))

/*******************************************************************************
** Macro Syntax : Mcal_Extract(Address, Value, Offset, Bits)                  **
**                                                                            **
** Parameters (in) :  Address: Address of the Variable to be Modified         **
**                    Value  : Value to be written                            **
**                    Offset : Bit Offset from LSB                            **
**                    Bits   : No of Bits Modified                            **
** Parameters (out):  None                                                    **
** Return          : returns the extracted specifed value                     **
** Description    : EXTRACT is used to read value from a required position    **
**                  for the desired number of bits.                           **
**                  Reads the values in a single instruction when compared    **
**                  mask and loading the same                                 **
**                                                                            **
*******************************************************************************/
/*IFX_MISRA_RULE_19_07_STATUS=Function like macro used to call the Tricore
intrinsic function*/
#define Mcal_Extract(Value,Pos,Width) \
            (EXTRACT((Value),(Pos),(Width)))

/******************************************************************************/

/******************************************************************************/


/*******************************************************************************
**                      Global Type Definitions                               **
*******************************************************************************/


/*******************************************************************************
**                      Global Constant Declarations                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Variable Declarations                          **
*******************************************************************************/


/*******************************************************************************
**                      Global Function Declarations                          **
*******************************************************************************/

#if (IFX_MCAL_USED == STD_ON)
/*Memory Map of the Code*/
#define MCAL_TCLIB_START_SEC_CODE
/*
  Allows to map variables, constants and code of modules to individual
  memory sections.
*/
#include "MemMap.h"
#else
#define IFX_MCAL_TCLIB_START_SEC_CODE_ASIL_B
#include "Ifx_MemMap.h"
#endif

/*******************************************************************************
** Syntax : uint8 Mcal_GetCoreId(void)                                        **
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
extern uint8 Mcal_GetCoreId(void);

/*******************************************************************************
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
extern uint32 Mcal_GetDsprReMapAddress(uint32 Address);

/*******************************************************************************
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
extern void Mcal_StartCore (uint8 CpuNo, uint32 Pcval);
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
extern void Mcal_SetCpuPC (uint8 CpuNo, uint32 ProgramCounter);

/*******************************************************************************
** Syntax : uint32 Mcal_LockResource(uint32 *ResourceStatusPtr)               **
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
**                    If a resource is free, the semaphore is taken and       **
**                    status MCAL_RESOURCE_FREE is returned.                  **
**                    If the resource was already taken,                      **
**                    a busy status (MCAL_RESOURCE_BUSY) is returned.         **
*******************************************************************************/
extern uint32 Mcal_LockResource(uint32 *ResourcePtr);
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
extern void  Mcal_UnlockResource(uint32* ResourcePtr);

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
extern Std_ReturnType Mcal_GetSpinLock(uint32* SpinLckPtr,uint32 Timeout);

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
extern void Mcal_ReleaseSpinLock(uint32* SpinLckPtr);

/*******************************************************************************
** Traceability     : [cover parentID=]        [/cover]                       **
**                                                                            **
** Syntax           : void Mcal_SuspendAllInterrupts(void)                    **
**                                                                            **
**                                                                            **
** Service ID       : none                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Re-entrant                                              **
**                                                                            **
** Parameters(in)   : None                                                    **
**                                                                            **
** Parameters (out) : None                                                    **
**                                                                            **
** Return value     : None                                                    **
**                                                                            **
**                                                                            **
** Description      : Suspend all interrupts                                  **
**                                                                            **
*******************************************************************************/
extern void Mcal_SuspendAllInterrupts(void);

/*******************************************************************************
** Traceability     : [cover parentID=]        [/cover]                       **
**                                                                            **
** Syntax           : void Mcal_ResumeAllInterrupts(void)                     **
**                                                                            **
**                                                                            **
** Service ID       : none                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Re-entrant                                              **
**                                                                            **
** Parameters(in)   : None                                                    **
**                                                                            **
** Parameters (out) : None                                                    **
**                                                                            **
** Return value     : None                                                    **
**                                                                            **
**                                                                            **
** Description      : Resume all interrupts                                   **
**                                                                            **
*******************************************************************************/
extern void Mcal_ResumeAllInterrupts(void);

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
** Caution          :                                                         **
*******************************************************************************/
extern IFX_NO_INLINE void Mcal_SetRegValSV (volatile uint32 *Address, \
  uint32 SetMask, uint32 ClearMask, uint8 InitProtectionType);

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
** Caution          :                                                         **
*******************************************************************************/
extern IFX_NO_INLINE void Mcal_UpdateRegSV (volatile uint32 *Address,\
      uint32 SetMask, uint32 ClearMask, uint8 InitProtectionType, uint16 Area);

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
** Caution          :                                                         **
*******************************************************************************/
extern IFX_NO_INLINE void Mcal_SVModeExecution (volatile uint32 *Address, \
                   uint32 SetMask, uint32 ClearMask, uint8 InitProtectionType);
#endif
/*******************************************************************************
**                      Global Inline Function Definitions                    **
*******************************************************************************/
#if (IFX_MCAL_USED == STD_ON)
/*Memory Map of the Code*/
#define MCAL_TCLIB_STOP_SEC_CODE
/*
  Allows to map variables, constants and code of modules to individual
  memory sections.
*/
#include "MemMap.h"
#else
#define IFX_MCAL_TCLIB_STOP_SEC_CODE_ASIL_B
#include "Ifx_MemMap.h"
#endif

#endif /* MCAL_TCLIB_H  */
