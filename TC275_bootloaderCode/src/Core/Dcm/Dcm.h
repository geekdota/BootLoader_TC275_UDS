/*============================================================================*/
/** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <Dcm.h>
 *  @brief      <Macro and function declarations for Dcm Module>
 *  
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *  
 *  @author     <Gary Chen>
 *  @date       <2012-11-09>
 */
/*============================================================================*/
#ifndef DCM_H
#define DCM_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *    V1.0    20121109  Gary       Initial version
 *
 *    V1.1    20160801 cywang      update
 *
 *    V1.2    20180511 CChen       update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "comstack_types.h"

/*=======[M A C R O S]========================================================*/

/** Negative response code */
typedef uint8 Dcm_NrcType;
#define DCM_E_GENERAL_REJECT                                0x10u
#define DCM_E_SERVICE_NOT_SUPPORTED                         0x11u
#define DCM_E_SUBFUNC_NOT_SUPPORTED                         0x12u
#define DCM_E_INCORRECT_MESSAGE_LENGTH                      0x13u
#define DCM_E_BUSY_REPEAT_REQUEST                           0x21u
#define DCM_E_CONDITIONS_NOT_CORRECT                        0x22u
#define DCM_E_REQUEST_SEQUENCE_ERROR                        0x24u
#define DCM_E_REQUEST_OUT_OF_RANGE                          0x31u
#define DCM_E_SECURITY_ACCESS_DENIED                        0x33u
#define DCM_E_INVALID_KEY                                   0x35u
#define DCM_E_EXCEED_NUMBER_OF_ATTEMPTS                     0x36u
#define DCM_E_REQUIRED_TIME_DELAY_NOT_EXPIRED               0x37u
#define DCM_E_UPLOAD_DOWNLOAD_NOT_ACCEPTED                  0x70u
#define DCM_E_TRANSFER_DATA_SUSPENDED                       0x71u
#define DCM_E_GENERAL_PROGRAMMING_FAILURE                   0x72u
#define DCM_E_WRONG_BLOCK_SEQUENCE_COUNTER                  0x73u
#define DCM_E_PENDING                                       0x78u
#define DCM_E_SUBFUNCTION_NOT_SUPPORT_IN_ACTIVE_SESSION     0x7Eu
#define DCM_E_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION       0x7Fu
#define DCM_E_ENGINE_IS_RUNNING                             0x83u
#define DCM_E_ENGINE_IS_NOT_RUNNING                         0x84u
#define DCM_E_ENGINE_RUN_TIME_TOO_LOW                       0x85u
#define DCM_E_VOLTAGE_TOO_LOW                               0x92u
#define DCM_E_VOLTAGE_TOO_HIGH                              0x93u

/** SID negative response */
#define DCM_RSP_SID_NRC 0x7Fu

/** if response needed */
#define DCM_RSP_REQUIRED        0x00u
#define DCM_RSP_NOT_REQUIRED    0x80u
#define DCM_RSP_CLEAR_REQUIRED  0x7Fu

/** address mode support mask type
* physical addressing  :0x01u
* functional addressing: 0x02
*/
typedef uint8 Dcm_AddressModeType;
#define DCM_PHYSICAL_ADDRESSING 0x01u
#define DCM_FUNCTIONAL_ADDRESSING 0x02u

/**session convert table reserved*/
#define DCM_SESSION_RESEVED 0x00u

/** service 10 diagnosticSessionType */
typedef uint8 Dcm_SessionType;
#define DCM_SESSION_DEFAULT     0x01u
#define DCM_SESSION_PROGRAMMING 0x02u
#define DCM_SESSION_EXTEND      0x03u

/** service 11 resetType */
typedef uint8 Dcm_ResetType;
#define DCM_HARD_RESET  0x01u

/** service 31 routine control type */
typedef uint8 Dcm_RoutineControlType;
#define DCM_START_ROUTINE   0x01u
#define DCM_UDS31_MIN_LENGTH 0x04u

/** service 28 communication control */
typedef uint8 Dcm_ComControlType;
#define DCM_ENABLE_RXANDTX      0x00u
#define DCM_ENABLE_RXANDDISTX   0x01u
#define DCM_DISABLE_RXANDTX     0x03u

typedef uint8 Dcm_CommunicationType;
#define DCM_NORMAL_COM_MESSAGES      0x01u
#define DCM_NMANDNORMAL_COM_MESSAGES 0x03u

/** service 3E test present type */
typedef uint8 Dcm_zeroSubFuncType;
#define DCM_ZERO_SUB_FUNC       0x00u

/** service 85 DTC setting type */
typedef uint8 Dcm_DTCSettingType;
#define DCM_DTC_SETTING_ON  0x01u
#define DCM_DTC_SETTING_OFF 0x02u

/** service 27 security access lock type */
typedef uint8 Dcm_SecurityType;
#define DCM_SECURITY_LOCKED     0x01u
#define DCM_SECURITY_LEV1       0x02u

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/** Dcm Buffer status */
typedef enum
{
    DCM_BUFF_FREE,

    DCM_BUFF_FOR_TP,

    DCM_BUFF_FOR_SERVICE

} Dcm_BuffStatusType;

/** struct of DCM Buffer type */
typedef struct
{
    /* status of this buffer */
    Dcm_BuffStatusType buffStatus;

    /* PduId of this buffer */
    PduIdType pduId;

    /* Pdu Data of this buffer */
    PduInfoType pduInfo;

} Dcm_BuffType;

/*=======[E X T E R N A L   D A T A]==========================================*/

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern void Dcm_Init(void);

extern void Dcm_ForcePending(void);

extern void Dcm_ProgramInit(Dcm_SessionType targetSession);

extern void Dcm_MainFunction(void);

extern BufReq_ReturnType Dcm_ProvideRxBuffer(PduIdType pduId,
                                             PduLengthType pduLen,
                                             PduInfoType **sduInfo);

extern void Dcm_RxIndication(PduIdType pduId, NotifResultType result);

extern BufReq_ReturnType Dcm_ProvideTxBuffer(PduIdType pduId, PduInfoType **sduInfo);

extern void Dcm_TxConfirmation(PduIdType pduId, NotifResultType result);

extern Dcm_SessionType Dcm_GetSessionMode(void);

extern Dcm_SecurityType Dcm_GetSecurityLevel(void);

extern void Dcm_StartP3cTimer(void);

extern void PosResponse255(void);
#endif/* endof DCM_H */

/*=======[E N D   O F   F I L E]==============================================*/
