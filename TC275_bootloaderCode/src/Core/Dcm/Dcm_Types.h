/*============================================================================*/
/** Copyright (C) 2009-2017, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <Dcm_Types.h>
 *  @brief      <Macro and function decalrations for Dcm Module>
 *  
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *  
 *  @author     <Gary Chen>
 *  @date       <2012-11-09>
 */
/*============================================================================*/
#ifndef DCM_TYPES_H
#define DCM_TYPES_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *    V1.0    20121109   Gary       Initial version
 *
 *    V1.1    20160801  cywang      update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "dcm.h"
#include "fl.h"
#include "secm.h"
#include "dcm_cfg.h"

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/** service function type */
typedef void (*Dcm_ServiceFct)(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

/** service pending function type */
typedef void (*Dcm_PendingFct)(const FL_ResultType errorCode,
                               const Dcm_BuffType * rxBuff,
                               Dcm_BuffType * txBuff);

/** service table parameter configuration */
typedef struct
{
    /* service Id */
    const uint8 SID;

    /* if function address is supported */
    const Dcm_AddressModeType addressSupportMask;

    /* supported session mode */
    const Dcm_SessionType sessionSupportMask;

    /* service function */
    const Dcm_ServiceFct serviceFct;

    /* service pending function */
    const Dcm_PendingFct pendingFct;

} Dcm_ServiceTableType;

/** 0x10 service parameter configuration */
typedef struct
{
    /* session mode type */
    const Dcm_SessionType sessionType;

    /* supported session mode */
    const Dcm_SessionType sessionSupportMask;

    /* supported security level */
    const Dcm_SecurityType securitySupportMask;

} Dcm_SessionRowType;

/** 0x11 service parameter configuration */
typedef struct
{
    /* reset mode type */
    const Dcm_ResetType resetType;

    /* supported session mode */
    const Dcm_SessionType sessionSupportMask;

    /* supported security level */
    const Dcm_SecurityType securitySupportMask;

} Dcm_ResetRowType;

#if (DCM_READDID_NUM > 0)
/** read data function type */
typedef uint16 (*Dcm_ReadDataFct)(uint8 *readData);

/** 0x22 service parameter configuration */
typedef struct
{
    /* read data Id */
    const uint16 DID;

    /* supported security level */
    const Dcm_SecurityType securitySupportMask;

    /* read data function */
    const Dcm_ReadDataFct readDataFct;

} Dcm_ReadDidRowType;
#endif

/** generate seed function type */
typedef SecM_StatusType (*Dcm_GenerateSeedFct)(SecM_SeedType *seed);

/** compare key function type */
typedef SecM_StatusType (*Dcm_CompareKeyFct)(SecM_KeyType key, SecM_SeedType seed);

/** 0x27 service parameter configuration */
typedef struct
{
    /* request seed sub function Id */
    const uint8 reqSeedId;

    /* send key sub function Id */
    const uint8 sendKeyId;

    /* security access level */
    const Dcm_SecurityType secAccessLevel;

    /* supported session mode */
    const Dcm_SessionType sessionSupportMask;

    /* supported security level */
    const Dcm_SecurityType securitySupportMask;

    /* generate seed function */
    const Dcm_GenerateSeedFct generateSeed;

    /* compare key function */
    const Dcm_CompareKeyFct compareKey;

} Dcm_SecurityRowType;

#if (DCM_COM_CONTROL_NUM > 0)
/** 0x28 service parameter configuration */
typedef struct
{
    /* communication control type */
    const Dcm_ComControlType controlType;

    /* supported session mode */
    const Dcm_SessionType sessionSupportMask;

    /* supported security level */
    const Dcm_SecurityType securitySupportMask;

} Dcm_ComControlRowType;
#endif

/** write data function type */
typedef FL_ResultType (*Dcm_WriteDataFct)(const uint8 *data, const uint16 length);

/* 0x2E service parameter configuration */
typedef struct
{
    /* write data Id */
    const uint16 DID;

    /* write length */
    const uint16 dataLength;

    /* supported security level */
    const Dcm_SecurityType securitySupportMask;

    /* write data function */
    const Dcm_WriteDataFct writeDataFct;

} Dcm_WriteDidRowType;

/** routine control function type */
typedef void (*Dcm_RoutineControlFct)(const Dcm_BuffType * rxBuff,
                                      Dcm_BuffType * txBuff);

/** 0x31 service parameter configuration */
typedef struct
{
    /* routine Id */
    const uint16 routineId;

    /* routine option length */
    const uint8 optionLength;

    /* if function address supported */
    const boolean funcAddrSupp;

    /* supported session mode */
    const Dcm_SessionType sessionSupportMask;

    /* supported security level */
    const Dcm_SecurityType securitySupportMask;

    /* routine control function */
    const Dcm_RoutineControlFct routineControl;

} Dcm_RoutineControlRowType;

/** 0x34 0x36 0x37 service parameter configuration */
typedef struct
{
    /* supported security level */
    const Dcm_SecurityType securitySupportMask;

} Dcm_DownloadRowType;

/** 0x3E service parameter configuration */
typedef struct
{
    /* supported zeroSubFunc */
    const Dcm_zeroSubFuncType zeroSubFunc;

} Dcm_testPresentRowType;

#if (DCM_DTC_SET_NUM > 0)
/** 0x85 service parameter configuration */
typedef struct
{
    /* DTC setting type */
    const Dcm_DTCSettingType DTCSet;

    /* supported session mode */
    const Dcm_SessionType sessionSupportMask;

    /* supported security level */
    const Dcm_SecurityType securitySupportMask;

} Dcm_DTCSettingRowType;
#endif

/*=======[E X T E R N A L   D A T A]==========================================*/
extern const Dcm_ServiceTableType Dcm_ServiceTable[DCM_SERVICE_NUM];

extern const Dcm_SessionRowType Dcm_SessionRow[DCM_SESSION_NUM];

#if (DCM_RESET_NUM > 0)
extern const Dcm_ResetRowType Dcm_ResetRow[DCM_RESET_NUM];
#endif

#if (DCM_READDID_NUM > 0)
extern const Dcm_ReadDidRowType Dcm_ReadDidRow[DCM_READDID_NUM];
#endif

extern const Dcm_SecurityRowType Dcm_SecurityRow[DCM_SECURITY_NUM];

#if (DCM_COM_CONTROL_NUM > 0)
extern const Dcm_ComControlRowType Dcm_ComControlRow[DCM_COM_CONTROL_NUM];
#endif

extern const Dcm_WriteDidRowType Dcm_WriteDidRow[DCM_WRITEDID_NUM];

extern const Dcm_RoutineControlRowType Dcm_RoutineControlRow[DCM_ROUTINE_CONTROL_NUM];

extern const Dcm_DownloadRowType Dcm_DownloadRow;

extern const Dcm_testPresentRowType Dcm_testPresentRow[DCM_TESTPRESENT_NUM];

#if (DCM_DTC_SET_NUM > 0)
extern const Dcm_DTCSettingRowType Dcm_DTCSettingRow[DCM_DTC_SET_NUM];
#endif

#endif/* endof DCM_TYPES_H */

/*=======[E N D   O F   F I L E]==============================================*/
