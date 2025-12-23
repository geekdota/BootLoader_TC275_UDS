/*============================================================================*/
/*  Copyright (C) 2016, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is 10086 property. Duplication
 *  or disclosure without 10086 written authorization is prohibited.
 *
 *  @file       <Compiler_Cfg.h>
 *  @brief      <Briefly describe file(one line)>
 *
 *  <Compiler: HighTec    MCU:TC275>
 *
 *  @author     <chen xue hua>
 *  @date       <2013-02-27>
 */
/*============================================================================*/
#ifndef COMPILER_CFG_H
#define COMPILER_CFG_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/*  <VERSION>    <DATE>    <AUTHOR>    <REVISION LOG>
 *  V1.0.0       20130227  10086  Initial version
 *  V1.0.1       20140916  10086         add EcuM_MemoryAndPointerClasses
 */
/*============================================================================*/

/*=======[V E R S I O N  I N F O R M A T I O N]===============================*/
#define COMPILER_CFG_VENDOR_ID 62
#define COMPILER_CFG_MODULE_ID 1
#define COMPILER_CFG_AR_MAJOR_VERSION 4
#define COMPILER_CFG_AR_MINOR_VERSION 2
#define COMPILER_CFG_AR_PATCH_VERSION 2
#define COMPILER_CFG_SW_MAJOR_VERSION 1
#define COMPILER_CFG_SW_MINOR_VERSION 0
#define COMPILER_CFG_SW_PATCH_VERSION 0
#define COMPILER_CFG_VENDOR_API_INFIX 0

/* @req COMPILER055 @req COMPILER054 */
/*=======[M A C R O S]========================================================*/

/*=======[Os_MemoryAndPointerClasses]========================================*/
/* Configurable memory class for code. */
#define OS_CODE

/* Configurable memory class for ISR code. */
#define OS_CODE_FAST

/*
 * Configurable memory class for all global or static variables that are never
 * initialized.
 */
#define OS_VAR_NOINIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized only after power on reset.
 */
#define OS_VAR_POWER_ON_INIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized after every reset.
 */
#define OS_VAR

/*
 * Configurable memory class for all global or static variables that will
 * be accessed frequently.
 */
#define OS_VAR_NOINIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define OS_VAR_POWER_ON_INIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define OS_VAR_FAST

/* Configurable memory class for global or static constants. */
#define OS_CONST

/*
 * Configurable memory class for global or static constants that will be
 * accessed frequently.
 */
#define OS_CONST_FAST

/* Configurable memory class for global or static constants in post build. */
#define OS_CONST_PBCFG

/*
 * Configurable memory class for pointers to applicaiton data(expected to be
 * in RAM or ROM)passed via API.
 */
#define OS_APPL_DATA

/*
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
#define OS_APPL_CONST

/*
 * Configurable memory class for pointers to applicaiton functions(e.g. call
 * back function pointers).
 */
#define OS_APPL_CODE

/* @req COMPILER044 @req COMPILER040 */
/*=======[Can_MemoryAndPointerClasses]========================================*/
/* Configurable memory class for code. */
#define CAN_CODE

/* Configurable memory class for ISR code. */
#define CAN_CODE_FAST

/*
 * Configurable memory class for all global or static variables that are never
 * initialized.
 */
#define CAN_VAR_NOINIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized only after power on reset.
 */
#define CAN_VAR_POWER_ON_INIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized after every reset.
 */
#define CAN_VAR

/*
 * Configurable memory class for all global or static variables that will
 * be accessed frequently.
 */
#define CAN_VAR_NOINIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define CAN_VAR_POWER_ON_INIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define CAN_VAR_FAST

/* Configurable memory class for global or static constants. */
#define CAN_CONST

/*
 * Configurable memory class for global or static constants that will be
 * accessed frequently.
 */
#define CAN_CONST_FAST

/* Configurable memory class for global or static constants in post build. */
#define CAN_CONST_PBCFG

/*
 * Configurable memory class for pointers to applicaiton data(expected to be
 * in RAM or ROM)passed via API.
 */
#define CAN_APPL_DATA

/*
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
#define CAN_APPL_CONST

/*
 * Configurable memory class for pointers to applicaiton functions(e.g. call
 * back function pointers).
 */
#define CAN_APPL_CODE

/*=======[CanIf_MemoryAndPointerClasses]========================================*/
#define CANIF_CODE

#define CANIF_CODE_FAST

#define CANIF_VAR_NOINIT

#define CANIF_VAR_POWER_ON_INIT

#define CANIF_VAR

#define CANIF_VAR_NOINIT_FAST

#define CANIF_VAR_POWER_ON_INIT_FAST

#define CANIF_VAR_FAST

#define CANIF_CONST

#define CANIF_CONST_FAST

#define CANIF_CONST_PBCFG

#define CANIF_APPL_DATA

#define CANIF_APPL_CONST

#define CANIF_APPL_CODE

/* @req COMPILER044 @req COMPILER040 */
/*=======[OSEKCOM_MemoryAndPointerClasses]====================================*/
/* Configurable memory class for code. */
#define COM_CODE

/* Configurable memory class for ISR code. */
#define COM_CODE_FAST

/*
 * Configurable memory class for all global or static variables that are never
 * initialized.
 */
#define COM_VAR_NOINIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized only after power on reset.
 */
#define COM_VAR_POWER_ON_INIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized after every reset.
 */
#define COM_VAR

/*
 * Configurable memory class for all global or static variables that will
 * be accessed frequently.
 */
#define COM_VAR_NOINIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define COM_VAR_POWER_ON_INIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define COM_VAR_FAST

/* Configurable memory class for global or static constants. */
#define COM_CONST

/*
 * Configurable memory class for global or static constants that will be
 * accessed frequently.
 */
#define COM_CONST_FAST

/* Configurable memory class for global or static constants in post build. */
#define COM_CONST_PBCFG

/*
 * Configurable memory class for pointers to applicaiton data(expected to be
 * in RAM or ROM)passed via API.
 */
#define COM_APPL_DATA

/*
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
#define COM_APPL_CONST

/*
 * Configurable memory class for pointers to applicaiton functions(e.g. call
 * back function pointers).
 */
#define COM_APPL_CODE
/*=======[CanSM_MemoryAndPointerClasses]========================================*/
#define CANSM_CODE

#define CANSM_CODE_FAST

#define CANSM_VAR_NOINIT

#define CANSM_VAR_POWER_ON_INIT

#define CANSM_VAR

#define CANSM_VAR_NOINIT_FAST

#define CANSM_VAR_POWER_ON_INIT_FAST

#define CANSM_VAR_FAST

#define CANSM_CONST

#define CANSM_CONST_FAST

#define CANSM_CONST_PBCFG

#define CANSM_APPL_DATA

#define CANSM_APPL_CONST

#define CANSM_APPL_CODE

/*=======[NmIf_MemoryAndPointerClasses]========================================*/

#define NM_CODE

#define NM_VAR_NOINIT

#define NM_VAR_POWER_ON_INIT

#define NM_VAR

#define NM_CONST

#define NM_APPL_DATA

#define NM_APPL_CONST

#define NM_APPL_CODE

/*=======[OsekNm_MemoryAndPointerClasses]========================================*/

#define OSEKNM_CODE

#define OSEKNM_VAR_NOINIT

#define OSEKNM_VAR_POWER_ON_INIT

#define OSEKNM_VAR

#define OSEKNM_CONST

#define OSEKNM_APPL_DATA

#define OSEKNM_APPL_CONST

#define OSEKNM_APPL_CODE

/*=======[ComM_MemoryAndPointerClasses]========================================*/
#define COMM_CODE

#define COMM_CODE_FAST

#define COMM_VAR_NOINIT

#define COMM_VAR_POWER_ON_INIT

#define COMM_VAR

#define COMM_VAR_NOINIT_FAST

#define COMM_VAR_POWER_ON_INIT_FAST

#define COMM_VAR_FAST

#define COMM_CONST

#define COMM_CONST_FAST

#define COMM_CONST_PBCFG

#define COMM_APPL_DATA

#define COMM_APPL_CONST

#define COMM_APPL_CODE

/* @req COMPILER044 @req COMPILER040 */
/*=======[XCP_MemoryAndPointerClasses]====================================*/
/* Configurable memory class for code. */
#define XCP_CODE

/* Configurable memory class for ISR code. */
#define XCP_CODE_FAST

/*
 * Configurable memory class for all global or static variables that are never
 * initialized.
 */
#define XCP_VAR_NOINIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized only after power on reset.
 */
#define XCP_VAR_POWER_ON_INIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized after every reset.
 */
#define XCP_VAR

/*
 * Configurable memory class for all global or static variables that will
 * be accessed frequently.
 */
#define XCP_VAR_NOINIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define XCP_VAR_POWER_ON_INIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define XCP_VAR_FAST

/* Configurable memory class for global or static constants. */
#define XCP_CONST

/*
 * Configurable memory class for global or static constants that will be
 * accessed frequently.
 */
#define XCP_CONST_FAST

/* Configurable memory class for global or static constants in post build. */
#define XCP_CONST_PBCFG

/*
 * Configurable memory class for pointers to applicaiton data(expected to be
 * in RAM or ROM)passed via API.
 */
#define XCP_APPL_DATA

/*
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
#define XCP_APPL_CONST

/*
 * Configurable memory class for pointers to applicaiton functions(e.g. call
 * back function pointers).
 */
#define XCP_APPL_CODE

/*=======[EcuM_MemoryAndPointerClasses]========================================*/
#define ECUM_CODE

#define ECUM_CODE_FAST

#define ECUM_VAR_NOINIT

#define ECUM_VAR_POWER_ON_INIT

#define ECUM_VAR

#define ECUM_VAR_NOINIT_FAST

#define ECUM_VAR_POWER_ON_INIT_FAST

#define ECUM_VAR_FAST

#define ECUM_CONST

#define ECUM_CONST_FAST

#define ECUM_CONST_PBCFG

#define ECUM_APPL_DATA

#define ECUM_APPL_CONST

#define ECUM_APPL_CODE

/*=======[Flash_MemoryAndPointerClasses]========================================*/
#define FLS_CODE

#define FLS_CODE_FAST

#define FLS_VAR_NOINIT

#define FLS_VAR_POWER_ON_INIT

#define FLS_VAR

#define FLS_VAR_NOINIT_FAST

#define FLS_VAR_POWER_ON_INIT_FAST

#define FLS_VAR_FAST

#define FLS_CONST

#define FLS_CONST_FAST

#define FLS_CONST_PBCFG

#define FLS_APPL_DATA

#define FLS_APPL_CONST

#define FLS_APPL_CODE

/*=======[NvM_MemoryAndPointerClasses]========================================*/
/* Configurable memory class for code. */
#define NVM_CODE

/* Configurable memory class for ISR code. */
#define NVM_CODE_FAST

/*
 * Configurable memory class for all global or static variables that are never
 * initialized.
 */
#define NVM_VAR_NOINIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized only after power on reset.
 */
#define NVM_VAR_POWER_ON_INIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized after every reset.
 */
#define NVM_VAR

/*
 * Configurable memory class for all global or static variables that will
 * be accessed frequently.
 */
#define NVM_VAR_NOINIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define NVM_VAR_POWER_ON_INIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define NVM_VAR_FAST

/* Configurable memory class for global or static constants. */
#define NVM_CONST

/*
 * Configurable memory class for global or static constants that will be
 * accessed frequently.
 */
#define NVM_CONST_FAST

/* Configurable memory class for global or static constants in post build. */
#define NVM_CONST_PBCFG

/*
 * Configurable memory class for pointers to applicaiton data(expected to be
 * in RAM or ROM)passed via API.
 */
#define NVM_APPL_DATA

/*
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
#define NVM_APPL_CONST

/*
 * Configurable memory class for pointers to applicaiton functions(e.g. call
 * back function pointers).
 */
#define NVM_APPL_CODE

/*=======[MemIf_MemoryAndPointerClasses]========================================*/
/* Configurable memory class for code. */
#define MEMIF_CODE

/* Configurable memory class for ISR code. */
#define MEMIF_CODE_FAST

/*
 * Configurable memory class for all global or static variables that are never
 * initialized.
 */
#define MEMIF_VAR_NOINIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized only after power on reset.
 */
#define MEMIF_VAR_POWER_ON_INIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized after every reset.
 */
#define MEMIF_VAR

/*
 * Configurable memory class for all global or static variables that will
 * be accessed frequently.
 */
#define MEMIF_VAR_NOINIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define MEMIF_VAR_POWER_ON_INIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define MEMIF_VAR_FAST

/* Configurable memory class for global or static constants. */
#define MEMIF_CONST

/*
 * Configurable memory class for global or static constants that will be
 * accessed frequently.
 */
#define MEMIF_CONST_FAST

/* Configurable memory class for global or static constants in post build. */
#define MEMIF_CONST_PBCFG

/*
 * Configurable memory class for pointers to applicaiton data(expected to be
 * in RAM or ROM)passed via API.
 */
#define MEMIF_APPL_DATA

/*
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
#define MEMIF_APPL_CONST

/*
 * Configurable memory class for pointers to applicaiton functions(e.g. call
 * back function pointers).
 */
#define MEMIF_APPL_CODE

/*=======[Fee_MemoryAndPointerClasses]========================================*/
/* Configurable memory class for code. */
#define FEE_CODE

/* Configurable memory class for ISR code. */
#define FEE_CODE_FAST

/*
 * Configurable memory class for all global or static variables that are never
 * initialized.
 */
#define FEE_VAR_NOINIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized only after power on reset.
 */
#define FEE_VAR_POWER_ON_INIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized after every reset.
 */
#define FEE_VAR

/*
 * Configurable memory class for all global or static variables that will
 * be accessed frequently.
 */
#define FEE_VAR_NOINIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define FEE_VAR_POWER_ON_INIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define FEE_VAR_FAST

/* Configurable memory class for global or static constants. */
#define FEE_CONST

/*
 * Configurable memory class for global or static constants that will be
 * accessed frequently.
 */
#define FEE_CONST_FAST

/* Configurable memory class for global or static constants in post build. */
#define FEE_CONST_PBCFG

/*
 * Configurable memory class for pointers to applicaiton data(expected to be
 * in RAM or ROM)passed via API.
 */
#define FEE_APPL_DATA

/*
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
#define FEE_APPL_CONST

/*
 * Configurable memory class for pointers to applicaiton functions(e.g. call
 * back function pointers).
 */
#define FEE_APPL_CODE

/*=======[Crc_MemoryAndPointerClasses]========================================*/
/* Configurable memory class for code. */
#define CRC_CODE

/* Configurable memory class for ISR code. */
#define CRC_CODE_FAST

/*
 * Configurable memory class for all global or static variables that are never
 * initialized.
 */
#define CRC_VAR_NOINIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized only after power on reset.
 */
#define CRC_VAR_POWER_ON_INIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized after every reset.
 */
#define CRC_VAR

/*
 * Configurable memory class for all global or static variables that will
 * be accessed frequently.
 */
#define CRC_VAR_NOINIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define CRC_VAR_POWER_ON_INIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define CRC_VAR_FAST

/* Configurable memory class for global or static constants. */
#define CRC_CONST

/*
 * Configurable memory class for global or static constants that will be
 * accessed frequently.
 */
#define CRC_CONST_FAST

/* Configurable memory class for global or static constants in post build. */
#define CRC_CONST_PBCFG

/*
 * Configurable memory class for pointers to applicaiton data(expected to be
 * in RAM or ROM)passed via API.
 */
#define CRC_APPL_DATA

/*
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
#define CRC_APPL_CONST

/*
 * Configurable memory class for pointers to applicaiton functions(e.g. call
 * back function pointers).
 */
#define CRC_APPL_CODE

/*=======[Dem_MemoryAndPointerClasses]========================================*/
/* Configurable memory class for code. */
#define DEM_CODE

/* Configurable memory class for ISR code. */
#define DEM_CODE_FAST

/*
 * Configurable memory class for all global or static variables that are never
 * initialized.
 */
#define DEM_VAR_NOINIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized only after power on reset.
 */
#define DEM_VAR_POWER_ON_INIT

/*
 * Configurable memory class for all global or static variables that are
 * initialized after every reset.
 */
#define DEM_VAR

/*
 * Configurable memory class for all global or static variables that will
 * be accessed frequently.
 */
#define DEM_VAR_NOINIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define DEM_VAR_POWER_ON_INIT_FAST

/*
 * Configurable memory class for all global or static variables that have at
 * be accessed frequently.
 */
#define DEM_VAR_FAST

/* Configurable memory class for global or static constants. */
#define DEM_CONST

/*
 * Configurable memory class for global or static constants that will be
 * accessed frequently.
 */
#define DEM_CONST_FAST

/* Configurable memory class for global or static constants in post build. */
#define DEM_CONST_PBCFG

/*
 * Configurable memory class for pointers to applicaiton data(expected to be
 * in RAM or ROM)passed via API.
 */
#define DEM_APPL_DATA

/*
 * Configurable memory class for pointers to applicaiton constants(expected to
 * be certainly in ROM,for instance point of Init-function)passed via API.
 */
#define DEM_APPL_CONST

/*
 * Configurable memory class for pointers to applicaiton functions(e.g. call
 * back function pointers).
 */
#define DEM_APPL_CODE

#define CANTP_CODE

#define CANTP_CODE_FAST

#define CANTP_VAR_NOINIT

#define CANTP_VAR_POWER_ON_INIT

#define CANTP_VAR

#define CANTP_VAR_NOINIT_FAST

#define CANTP_VAR_POWER_ON_INIT_FAST

#define CANTP_VAR_FAST

#define CANTP_CONST

#define CANTP_CONST_FAST

#define CANTP_CONST_PBCFG

#define CANTP_APPL_DATA

#define CANTP_APPL_CONST

#define CANTP_APPL_CODE

/*=======[DCM_MemoryAndPointerClasses]========================================*/
#define DCM_CODE

#define DCM_CODE_FAST

#define DCM_VAR_NOINIT

#define DCM_VAR_POWER_ON_INIT

#define DCM_VAR

#define DCM_VAR_NOINIT_FAST

#define DCM_VAR_POWER_ON_INIT_FAST

#define DCM_VAR_FAST

#define DCM_CONST

#define DCM_CONST_FAST

#define DCM_CONST_PBCFG

#define DCM_APPL_DATA

#define DCM_APPL_CONST

#define DCM_APPL_CODE

#define MCU_CODE

/* ================================  zheda's compiler_Cfg.h ============================================ */

/*
Tasking Keyword for __bisr_
*/

#define BISR __bisr_
/*
Tasking Keyword for Trap
*/

#define TRAP __trap
/*
volatile keyword
*/

#define _VOLATILE_ volatile

/*

Inline keyword

*/

#define _INLINE_ inline

/*
extern keyword
*/

#define _EXTERN_ extern

/*
debug instruction
*/
#define DEBUG() __debug()

/*
Tasking Intrinsic: move contents of the addressed core SFR into a data register
*/

#define MFCR(Reg) (__mfcr((Reg)))

#if 0
/*
Tasking Intrinsic:  Move contents of a data register (Data)
to the addressed core SFR (Reg)
*/

#define MTCR(Reg, Data) (__mtcr((Reg), (Data)))



/*
Tasking Intrinsic: Insert ISYNC Instruction
*/

#define ISYNC() (__isync())

#define DSYNC() (__dsync())


/*
Tasking Intrinsic: Disable Global Interrupt Flag
*/

#define DISABLE() (__disable())



/*
Tasking Intrinsic: Enable Global Interrupt Flag
*/

#define ENABLE() (__enable())
#endif

#define ALIGN(exp) __align(exp)
#define SHARE __share
#define FAR __far
/*******************************************************************************
** Macro Syntax : IMASKLDMST(Address, Value, Offset, Bits)                    **
**                                                                            **
** Parameters (in) :  Address: Address of the Variable to be Modified         **
**                    Value  : Value to be written                            **
**                    Offset : Bit Offset from LSB                            **
**                    Bits   : No of Bits Modified                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Description    : IMASKLDMST is used to write atomic instructions in        **
**                  code.The function writes the number of bits of an integer **
**                  value at a certain address location in memory with a ...  **
**                  bitoffset. The number of bits must be a constant value... **
**                  Note that the Address must be Word Aligned.A direct type  **
**                  cast to "int"is needed to avoid Misra Violation           **
*******************************************************************************/

/*
Use of Atomic Write Intrinsic from Tasking. A direct type cast to "int"
is needed to avoid Misra Violation
*/
#if 0
#define IMASKLDMST(address, val, offset, bits) \
  (__imaskldmst((sint32 *)(address),           \
                (sint32)(val),                 \
                (sint32)(offset),              \
                (sint32)(bits)))

#endif
/*******************************************************************************
** Macro Syntax : EXT_IMASKLDMST(Address, Value, Offset, Bits)                **
**                                                                            **
** Parameters (in) :  Address: Address of the Variable to be Modified         **
**                    Value  : Value to be written                            **
**                    Offset : Bit Offset from LSB                            **
**                    Bits   : No of Bits Modified                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Description    : If the variable is placed out side 16K boundary then      **
**                  Normal IMASKLDMST will result in compiler error,          **
**                  The user should                                           **
**                  use the following EXTENDED IMASKLDMST macro.              **
*******************************************************************************/

/*
Example:
A variable TESTVAR should be set to value 10 from  bit number 3 to 6.
Case 1:

  uint32 TESTVAR;  assume the variable is located at 0xd0001000
  IMASKLDMST(&TESTVAR,10,3,4);

  This will produce assembly instructions like
    imask	e0,#10,#3,#4
  ldmst	TESTVAR,e0

Case 2:

  Now if the variable is placed at outside 16K boundary
  uint32 TESTVAR __at(0xd0004000);

  Using IMASKLDMST will result in compile time error.
  So the user should use EXT_IMASKLDMST. The resulting
  assembly will be

  mov16	d15,#10
  movh.a	a15,#@his(TESTVAR)
  mov16	d0,#3
  lea	a15,[a15]@los(TESTVAR)
  imask e2,d15,d0,#4
  ldmst [a15]0,e2

*/

#define EXT_IMASKLDMST(address, val, offset, bits)                                                                                                \
  do                                                                                                                                              \
  {                                                                                                                                               \
    register uint32 *EXT_IMASKLDMSTaddr = address;                                                                                                \
    register uint32 EXT_IMASKLDMSTval = val;                                                                                                      \
    register uint32 EXT_IMASKLDMSToffset = offset;                                                                                                \
    __asm("  imask e2,%0,%1,#" #bits " \n ldmst [%2]0,e2" : : "d"(EXT_IMASKLDMSTval), "d"(EXT_IMASKLDMSToffset), "a"(EXT_IMASKLDMSTaddr) : "e2"); \
  } while (0)

/*******************************************************************************
** Macro Syntax : EXTRACT(value,pos,width)                                    **
**                                                                            **
** Parameters (in) :                                                          **
**                    Value  : Value to be extracted                          **
**                    pos : Bit Offset from LSB                               **
**                    Width   : No of Bits to be read                         **
** Parameters (out):  None                                                    **
** return value    : integer (extracted value)                                **
** Description    : EXTRACT is used to read value from a required position    **
**                  for the desired number of bits.                           **
**                  Reads the values in a single instruction when compared    **
**                  mask and loading the same                                 **
*******************************************************************************/
#if 0
#define EXTRACT(value, pos, width) (__extru((sint32)(value), \
                                            (sint32)(pos),   \
                                            (sint32)(width)))

/*******************************************************************************
** Macro Syntax : Mcal_CountLeadingZeros(Variable)                            **
**                                                                            **
** Parameters (in) :  Variable: variable whose leading zero needs to          **
**                              counted.                                      **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Description     : Macro to Count Leading Zeros in a  variable. The tasking **
**                   intrinsic __clz is used for this purpose.                **
**                                                                            **
*******************************************************************************/

#define Mcal_CountLeadingZeros(Variable) (__clz((sint32)(Variable)))

#endif
#endif /* end of COMPILER_CFG_H */

/*=======[E N D   O F   F I L E]==============================================*/

