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
**   $FILENAME   : Mcal.h $                                                   **
**                                                                            **
**   $CC VERSION : \main\11 $                                                 **
**                                                                            **
**   $DATE       : 2014-08-03 $                                               **
**                                                                            **
**   VENDOR      : Infineon Technologies                                      **
**                                                                            **
**   DESCRIPTION : This header file exports all global macros,                **
**                 type definitions and functions needed by all MCAL drivers. **
**                                                                            **
**   SPECIFICATION(S) : NA                                                    **
**                                                                            **
**   MAY BE CHANGED BY USER [yes/no]: yes                                     **
**                                                                            **
*******************************************************************************/

#ifndef MCAL_H 
#define MCAL_H 

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/

/* Inclusion of Mcal Library files File */
#include "Mcal_TcLib.h"
#include "Mcal_WdgLib.h"

/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/
/* 
   This macro is editable by the user
   If this macro is STD_ON the Tim value will be stored  into a global variable 
   in Mcal Reset ENDINIT APIs
   Again it will be stored into the REL register in Mcal Set ENDINIT APIs
   
   If this macro is STD_OFF then the above function will be disabled.
   By default it will be STD_OFF  
       
*/


#define WDT_RESTORE_TIM          (STD_OFF)
#define MCAL_DIV_INCONSISTENCY   (1U)
#define MCAL_SPINLOCK_TIMEOUT    (2U)

extern void Mcal_SafeErrorHandler(uint32 ErrorType);
//extern void startup_Init(void);

#endif /* MCAL_H  */
