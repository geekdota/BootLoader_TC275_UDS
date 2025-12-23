
/*******************************************************************************
**                                                                            **
** Copyright (C) Infineon Technologies (2015)                                 **
**                                                                            **
** All rights reserved.                                                       **
**                                                                            **
** This document contains proprietary information belonging to Infineon       **
** Technologies. Passing on and copying of this document, and communication   **
** of its contents is not permitted without prior written authorization.      **
**                                                                            **
********************************************************************************
**                                                                            **
**   $FILENAME   : Mcal.c $                                                   **
**                                                                            **
**   $CC VERSION : \main\71 $                                                 **
**                                                                            **
**   $DATE       : 2015-11-02 $                                               **
**                                                                            **
**   AUTHOR      : DL-AUTOSAR-Engineering                                     **
**                                                                            **
**   VENDOR      : Infineon Technologies                                      **
**                                                                            **
**   DESCRIPTION : This file contains the startup code, endinit protection    **
**                 functions                                                  **
**                                                                            **
**   SPECIFICATION(S) :  NA                                                   **
**                                                                            **
**   MAY BE CHANGED BY USER [yes/no]: yes                                     **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/

/* Inclusion of Std_Types.h */
#include "Std_Types.h"
#include "IfxCpu_reg.h"
#include "IfxScu_reg.h"
#include "IfxFlash_reg.h"
/* Inclusion of Mcal.h */
#include "Mcal.h"
#include "Mcal_Options.h"
#if (IFX_MCAL_USED == STD_ON)

#endif

void Mcal_SafeErrorHandler(uint32 ErrorType)
{
  volatile uint32 TimeOut;
  
  TimeOut = 0U;
  /* User can add the code here */
  UNUSED_PARAMETER(ErrorType)
  /* While loop added for UTP AI00252128 */
  while(TimeOut < 0xFFFFFFFFU)
  {
    TimeOut++;
  }
  /* Control should not reach here. WDG timeout happens before this. */


}
