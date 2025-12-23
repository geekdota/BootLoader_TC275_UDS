/*============================================================================*/
/*  Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <ComStack_Types.h>
 *  @brief      <Briefly describe file(one line)>
 *  
 *  <Compiler: All Compilers    MCU:All MCUs>
 *  
 *  @author     <chen xue hua>
 *  @date       <2013-02-27>
 */
/*============================================================================*/
/* @req COMTYPE003 @req COMTYPE004 @req COMTYPE015 @req COMTYPE016 */
#ifndef COMSTACK_TYPES_H
#define COMSTACK_TYPES_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/*  <VERSION>    <DATE>    <AUTHOR>    <REVISION LOG>
 *  V1.0.0       20130227  chenxuehua  Initial version
 *  V1.0.0       20140628  chenxuehua  Add Can Tp Type define
 */
/*============================================================================*/

/*=======[V E R S I O N  I N F O R M A T I O N]===============================*/
/* @req COMTYPE002 */
#define COMSTACK_TYPES_VENDOR_ID  62
#define COMSTACK_TYPES_MODULE_ID  0
#define COMSTACK_TYPES_AR_MAJOR_VERSION  2
#define COMSTACK_TYPES_AR_MINOR_VERSION  2
#define COMSTACK_TYPES_AR_PATCH_VERSION  2
#define COMSTACK_TYPES_SW_MAJOR_VERSION  1
#define COMSTACK_TYPES_SW_MINOR_VERSION  0
#define COMSTACK_TYPES_SW_PATCH_VERSION  0
#define COMSTACK_TYPES_VENDOR_API_INFIX  0

/*=======[I N C L U D E S]====================================================*/
/* @req COMTYPE001 */
#include "Std_Types.h"

/*=======[M A C R O S]========================================================*/
/* @req COMTYPE018 @req COMTYPE019 */
/* General return codes for NotifResultType */
#define NTFRSLT_OK                     0x00
#define NTFRSLT_E_NOT_OK               0x01
#define NTFRSLT_E_TIMEOUT_A            0x02
#define NTFRSLT_E_TIMEOUT_BS           0x03
#define NTFRSLT_E_TIMEOUT_CR           0x04
#define NTFRSLT_E_WRONG_SN             0x05
#define NTFRSLT_E_INVALID_FS           0x06
#define NTFRSLT_E_UNEXP_PDU            0x07
#define NTFRSLT_E_WFT_OVRN             0x08
#define NTFRSLT_E_NO_BUFFER            0x09
#define NTFRSLT_E_CANCELATION_OK       0x0A
#define NTFRSLT_E_CANCELATION_NOT_OK   0x0B

/* COMTYPE021 General return codes for BusTrcvErrorType */
#define BUSTRCV_NO_ERROR     0x00
#define BUSTRCV_OK           0x00U
#define BUSTRCV_E_ERROR      0x01U

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/* @req COMTYPE005 @req COMTYPE006 @req COMTYPE007 @req COMTYPE014 */
/* 
 * This type is used with the entire AUTOSAR Com Stack except for bus drivers.
 */
typedef uint16 PduIdType; 

/* @req COMTYPE008 @req COMTYPE010 @req COMTYPE017 */
/* 
 * This type is used with the entire AUTOSAR Com Stack except for bus drivers.
 */
typedef uint16 PduLengthType;

/* @req COMTYPE011 */
/* 
 * Variables of this type shall be used to store the basic information about 
 * a PDU of any type,namely a pointer variable pointing to it's SDU(payload), 
 * and the corresponding length of the SDU in bytes.
 */
typedef struct
{
    uint8 *SduDataPtr;

    PduLengthType SduLength;
}PduInfoType;

/* @req COMTYPE012 */
/* 
 * Variables of this type shall be used to store result of a buffer request. 
 */
typedef enum
{
    BUFREQ_OK,
    BUFREQ_E_NOT_OK,
    BUFREQ_E_BUSY,
    BUFREQ_E_OVEL
}BufReq_ReturnType;

/* @req COMTYPE013 */
/* 
 * Variables of this type shall be used to store result of a notification
 * (confirmation or indication). Currently this type is only used for 
 * communication between DCM and TP to enable the notification that an error 
 * has occurred and a dedicated buffer can be unlocked.
 */
typedef uint8 NotifResultType;

/* @req COMTYPE020 */
/* 
 * Variables of this type shall be used to return the bus status evaluated 
 * by a transceiver.
 */
typedef uint8 BusTrcvErrorType;

/* @req COMTYPE026 */
/* 
 * Variables of the type NetworkHandleType shall be used to store the 
 * identifier of a communication channel. 
 */
typedef uint8 NetworkHandleType;

/* @req SWS_COMTYPE_00027 */
/* Variables of this type shall be used to store the state of TP buffer.
 */
typedef enum
{
	TP_DATACONF,
	TP_DATARETRY,
	TP_CONFPENDING
}TpDataStateType;

/* @req SWS_COMTYPE_00037 */
/* Variables of this type shall be used to store the information about
 * Tp buffer handling.
 */
typedef struct
{
	TpDataStateType TpDataState;
	PduLengthType TxTpDataCnt;
}RetryInfoType;




#endif /* end of COMSTACK_TYPES_H */

/*=======[E N D   O F   F I L E]==============================================*/
