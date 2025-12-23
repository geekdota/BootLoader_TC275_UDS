/******************************************************************************
**                                                                           **
** Copyright (C) Infineon Technologies (2014)                                **
**                                                                           **
** All rights reserved.                                                      **
**                                                                           **
** This document contains proprietary information belonging to Infineon      **
** Technologies. Passing on and copying of this document, and communication  **
** of its contents is not permitted without prior written authorization.     **
**                                                                           **
*******************************************************************************
**                                                                           **
**  $FILENAME   : Mcal_Options.h $                                           **
**                                                                           **
**  $CC VERSION : \main\8 $                                                  **
**                                                                           **
**  $DATE       : 2015-11-05 $                                               **
**                                                                           **
**  AUTHOR      : DL-AUTOSAR-Engineering                                     **
**                                                                           **
**  VENDOR      : Infineon Technologies                                      **
**                                                                           **
**  DESCRIPTION  : This header file configures pre-compile switches          **
**                                                                           **
**  MAY BE CHANGED BY USER [yes/no]: yes                                     **
**                                                                           **
******************************************************************************/

#ifndef MCAL_OPTIONS_H
#define MCAL_OPTIONS_H

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "Std_Types.h"

/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/
/* 
   Configuration:
   IFX_MCAL_USED :
   Pre-Compile switch to enable stand alone Mcal
   IFX_SAFETLIB_USED:
   Pre-Compile switch to enable Standalone safeTlib
*/

#define IFX_MCAL_USED          (STD_ON)
#define IFX_SAFETLIB_USED      (STD_OFF)
/* Pre-Compile switch to enable/disable DEM module version check */
#define IFX_DEM_VERSION_CHECK  (STD_ON)
/* Pre-Compile switch to enable/disable DET module version check */
#define IFX_DET_VERSION_CHECK  (STD_ON)


/* TIN(Trap 6) for Endinit protected register and Safety EndInit Protected 
registers*/
/* If IFX_MCAL_RUN_MODE_DEFINE is 1 or 2, these TIN no shall be configured by 
used and are reseved for MCAL. Allowed range is 0-255 
*/
#define MCAL_TIN_NO_ENDINIT (0U)
#define MCAL_TIN_CPU_ENDINIT (1U)
#define MCAL_TIN_SAFETY_ENDINIT (2U)

/* Macro to define the type of the implementaion
Following values for macro IFX_MCAL_RUN_MODE_DEFINE is permitted
0U MCAL runs in Supervisor mode 
1U MCAL runs in User-1 mode without OS
2U MCAL runs in User-1 mode with OS
*/
/* This macro can also be defined via command line during compilation */
#ifndef IFX_MCAL_RUN_MODE_DEFINE 
#define IFX_MCAL_RUN_MODE_DEFINE (0U)
#endif /* End For IFX_MCAL_RUN_MODE_DEFINE */
/*******************************************************************************
**                      Global Type Definitions                               **
*******************************************************************************/


/*******************************************************************************
**                      Global Constant Declarations                          **
*******************************************************************************/



#endif /* MCAL_OPTIONS_H */
