/*============================================================================*/
/*  Copyright (C) 2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 *  @file       <Std_Types.h>
 *  @brief      <Briefly describe file(one line)>
 *
 *  <Compiler: All Compilers    MCU:All MCUs>
 *
 *  @author     <10086>
 *  @date       <2013-02-27>
 */
/*============================================================================*/
/* @req STD003 @req STD004 @req STD014 */
#ifndef STD_TYPES_H
#define STD_TYPES_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/*  <VERSION>    <DATE>    <AUTHOR>    <REVISION LOG>
 *  V1.0.0       20130227  10086  Initial version
 */
/*============================================================================*/

/*=======[V E R S I O N  I N F O R M A T I O N]===============================*/
#define STD_TYPES_VENDOR_ID 62
#define STD_TYPES_MODULE_ID 1
#define STD_TYPES_AR_MAJOR_VERSION 4
#define STD_TYPES_AR_MINOR_VERSION 2
#define STD_TYPES_AR_PATCH_VERSION 2
#define STD_TYPES_SW_MAJOR_VERSION 1
#define STD_TYPES_SW_MINOR_VERSION 0
#define STD_TYPES_SW_PATCH_VERSION 0
#define STD_TYPES_VENDOR_API_INFIX 0

/*=======[I N C L U D E S]====================================================*/
/* @req STD016 @req STD001 */
#include "Platform_Types.h"
#include "Compiler.h"

/*=======[M A C R O S]========================================================*/
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL_PTR
#define NULL_PTR ((void *)0)
#endif

/* @req STD011 @req STD006 */
#ifndef STATUSTYPEDEFINED
#define STATUSTYPEDEFINED
#define E_OK 0x00
typedef unsigned char StatusType; /* OSEK compliance */
#endif
#define E_NOT_OK 0x01

/* @req STD007 */
#define STD_LOW 0x00  /* Physical state 0V         */
#define STD_HIGH 0x01 /* Physical state 5V or 3.3V */

/* @req STD013 */
#define STD_IDLE 0x00	/* Logical state active */
#define STD_ACTIVE 0x01 /* Logical state idle   */

/* @req STD010 */
#define STD_OFF 0x00
#define STD_ON 0x01

#define STATIC static

#define ONE 1
#define TWO 2
#define FOUR 4
#define EIGHT 8

#define VARIANT_PRE_COMPILE 0x00 /* module config use variant pre compile */
#define VARIANT_LINK_TIME 0x01	 /* module config use variant link time */
#define VARIANT_POST_BUILD 0x02	 /* module config use variant post build */

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/*
 * This type can be used as standard API return type which is shared between the
 * RTE and the BSW modules.
 * 0x02-0x3F used for user specific errors, 0x40-0xFF used for RTE
 */
typedef uint8 Std_ReturnType; /* @req STD005 */

/* @req STD015 */
/*
 * This type shall be used to request the version of a BSW module using the
 * <Module name>_GetVersionInfo() function.
 */
typedef struct
{
	uint16 vendorID;
	uint16 moduleID;
	uint8 instanceID;
	uint8 sw_major_version;
	uint8 sw_minor_version;
	uint8 sw_patch_version;
} Std_VersionInfoType;

#endif /* end of STD_TYPES_H */

/*=======[E N D   O F   F I L E]==============================================*/
