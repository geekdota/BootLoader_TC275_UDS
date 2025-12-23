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
**  $FILENAME   : McalOsConfig.h $                                           **
**                                                                           **
**  $CC VERSION : \main\1 $                                                  **
**                                                                           **
**  $DATE       : 2013-08-12 $                                               **
**                                                                           **
**  AUTHOR      : DL-AUTOSAR-Engineering                                     **
**                                                                           **
**  VENDOR      : Infineon Technologies                                      **
**                                                                           **
**  DESCRIPTION  : This header file configures on required OS                **
**                                                                           **
**  MAY BE CHANGED BY USER [yes/no]: yes                                     **
**                                                                           **
******************************************************************************/

#ifndef MCAL_OS_CONFIG_H
#define MCAL_OS_CONFIG_H

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/


/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/
/* 
   Configuration:
   MCAL_OS_HEADER :
   Supply the required header file here
   default is Os.h, user can change to "osek.h" if required
*/
#define MCAL_OS_HEADER      "Os.h"
/*******************************************************************************
**                      Global Type Definitions                               **
*******************************************************************************/


/*******************************************************************************
**                      Global Constant Declarations                          **
*******************************************************************************/



#endif /* MCAL_OS_CONFIG_H */
