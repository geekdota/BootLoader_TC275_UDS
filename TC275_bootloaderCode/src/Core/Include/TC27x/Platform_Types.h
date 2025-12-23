/*============================================================================*/
/*  Copyright (C) 2016, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <MemMap.h>
 *  @brief      <Briefly describe file(one line)>
 *  
 *  <Compiler: HighTec    MCU:TC275>
 *  
 *  @author     <chen xue hua>
 *  @date       <2013-02-27>
 */
/*============================================================================*/
/* @req PLATFORM001 @req PLATFORM031 @req PLATFORM003 @req PLATFORM002 */
#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/*  <VERSION>    <DATE>    <AUTHOR>    <REVISION LOG>
 *  V1.0.0       20130227  chenxuehua  Initial version
 *  v1.0.1       20150204  Tommy       Add uint64
 */
/*============================================================================*/

/*=======[V E R S I O N  I N F O R M A T I O N]===============================*/
#define PLATFORM_TYPES_VENDOR_ID  			62
#define PLATFORM_TYPES_MODULE_ID  			1
#define PLATFORM_TYPES_AR_MAJOR_VERSION  	4
#define PLATFORM_TYPES_AR_MINOR_VERSION  	2
#define PLATFORM_TYPES_AR_PATCH_VERSION  	2
#define PLATFORM_TYPES_SW_MAJOR_VERSION  	1
#define PLATFORM_TYPES_SW_MINOR_VERSION  	0
#define PLATFORM_TYPES_SW_PATCH_VERSION  	0
#define PLATFORM_TYPES_VENDOR_API_INFIX  	0

/* @req PLATFORM010 */

/*=======[M A C R O S]========================================================*/
/* @req PLATFORM057 */

/* CPU Type */
#define CPU_TYPE_8     0
#define CPU_TYPE_16    1
#define CPU_TYPE_32    2
/* @req PLATFORM044 @req PLATFORM045 */
#define CPU_TYPE	CPU_TYPE_32

/* Endianess */
/* @req PLATFORM038 */
#define MSB_FIRST    0
#define LSB_FIRST    1
/* @req PLATFORM043 @req PLATFORM048 @req PLATFORM049 */
#define CPU_BIT_ORDER	MSB_FIRST

/* @req PLATFORM039 */
#define HIGH_BYTE_FIRST    0
#define LOW_BYTE_FIRST     1
/* @req PLATFORM046 @req PLATFORM050 @req PLATFORM051 */
#define CPU_BYTE_ORDER	LOW_BYTE_FIRST

/* @req PLATFORM034 @req PLATFORM026 @req PLATFORM060 */
/* @req PLATFORM054 @req PLATFORM056 @req PLATFORM055 */
#ifndef TRUE
  #define TRUE      1
#endif

#ifndef FALSE
  #define FALSE     0
#endif

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/*
 * AUTOSAR integer data types
 */
/* @req PLATFORM016 */
typedef signed char         sint8;          /*        -128 .. +127           */
/* @req PLATFORM013 */
typedef unsigned char       uint8;          /*           0 .. 255            */
/* @req PLATFORM017 */
typedef signed short        sint16;         /*      -32768 .. +32767         */
/* @req PLATFORM014 */
typedef unsigned short      uint16;         /*           0 .. 65535          */
/* @req PLATFORM018 */
typedef signed long         sint32;         /* -2147483648 .. +2147483647    */
/* @req PLATFORM015 */
typedef unsigned long       uint32;         /*           0 .. 4294967295     */

typedef unsigned long long  uint64;          /* defined by tommy for flash driver */

/* @req PLATFORM041 */
typedef float               float32;
/* @req PLATFORM042 */
typedef double              float64;
/* @req PLATFORM027 */
typedef unsigned char       boolean;        /* for use with TRUE/FALSE       */

/* only used with a local scope inside a module */
/* @req PLATFORM005 @req PLATFORM032 @req PLATFORM033 */
/* @req PLATFORM020 */
typedef unsigned long       uint8_least;    /* At least 8 bit                */
/* @req PLATFORM021 */
typedef unsigned long       uint16_least;   /* At least 16 bit               */
/* @req PLATFORM022 */
typedef unsigned long       uint32_least;   /* At least 32 bit               */
/* @req PLATFORM023 */
typedef signed long         sint8_least;    /* At least 7 bit + 1 bit sign   */
/* @req PLATFORM024 */
typedef signed long         sint16_least;   /* At least 15 bit + 1 bit sign  */
/* @req PLATFORM025 */
typedef signed long         sint32_least;   /* At least 31 bit + 1 bit sign  */


#endif /* end of PLATFORM_TYPES_H */

/*=======[E N D   O F   F I L E]==============================================*/
