/*============================================================================*/
/** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <Dcm_Cfg.h>
 *  @brief      <Dcm Configuration file>
 *              include Dcm used Timer and Macros defined.
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 * 
 *  @author     <Gary Chen>
 *  @date       <2012-11-09>
 */
/*============================================================================*/

#ifndef DCM_CFG_H
#define DCM_CFG_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *    V1.0   20121109    Gary       Initial version
 *
 *    V1.1   20130913    ccl        update
 *
 *    V1.2   20160801  cywang       update
 *
 *    V1.3   20180522    CChen     update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "STM_Cfg.h"

/*=======[M A C R O S]========================================================*/
#define DCM_SERVICE_11_ENABLE   	STD_ON

#define DCM_SERVICE_22_ENABLE   	STD_ON

#define DCM_SUPPRESS_POS_SUPPORT	STD_ON

#define DCM_SID_NRC_SUPPORT			STD_ON
/** main receive buffer size */
/* @type:uint16 range:1~4095 note:NONE */
#define DCM_RX_BUF_SIZE     0x400

/** main send buffer size */
/* @type:uint16 range:1~4095 note:NONE */
#define DCM_TX_BUF_SIZE     80

/** DCM pduId use for CanTp */
/* @type:define range:NONE note:reference to CanTp_TxSdu->txDcmId */
#define DCM_TX_PDU_ID       0xAAu

/* @type:define range:NONE note:reference to CanTp_RxSdu->rxDcmId */
#define DCM_RX_PHY_PDU_ID   0xCCu

/* @type:define range:NONE note:reference to CanTp_RxSdu->rxDcmId */
#define DCM_RX_FUNC_PDU_ID  0xFFu

/** DCM protocol time */
/* @type:define range:1~65535 note:unit ms */
#define DCM_P2MAX_TIME  50 /* ms */

/* @type:define range:1~65535 note:unit ms */
#define DCM_P2SMAX_TIME 2000 /* ms */

/* @type:define range:1~65535 note:unit ms */
#define DCM_P3MAX_TIME  5000 /* ms */

/** DCM main task tick time */
/* @type:define range:NONE note:auto generate */
#define DCM_MAIN_TICK   STM0_CMP0_PERIOD_TIME

/** pending send attempt num */
/* @type:define range:1~65535 note:NONE */
#define DCM_PENDING_ATTEMPT_NUM     15

/** DCM security access time */
/* security access timeout on boot */
/* @type:define range:0~65535 note:unit ms */
#define DCM_SECURITY_TIME_ON_BOOT   0UL

/* security access timeout when failed */
/* @type:define range:0~65535 note:unit ms */
#define DCM_SECURITY_TIME           10000

/* security access attempt max num */
/* @type:define range:0~65535 note:NONE */
#define DCM_SECURITY_ATTEMPT_NUM    3

/** session supported num */
/* @type:define range:NONE note:auto generate */
#define DCM_SESSION_SUPPORT_NUM     3

/** security supported num */
/* @type:define range:NONE note:auto generate */
#define DCM_SECURITY_SUPPORT_NUM    3

/** service table num */
/* @type:define range:1~255 note:reference to Dcm_ServiceTable */
#if ((STD_OFF == DCM_SERVICE_11_ENABLE) && (STD_OFF == DCM_SERVICE_22_ENABLE))
#define DCM_SERVICE_NUM         10
#elif (((DCM_SERVICE_11_ENABLE != STD_OFF) && (STD_OFF == DCM_SERVICE_22_ENABLE)) \
    || ((STD_OFF == DCM_SERVICE_11_ENABLE) && (DCM_SERVICE_22_ENABLE != STD_OFF)))
#define DCM_SERVICE_NUM         11
#else
#define DCM_SERVICE_NUM         12
#endif

/** session mode num */
/* @type:define range:1~3 note:reference to Dcm_SessionRow */
#define DCM_SESSION_NUM         3

/** reset mode num */
/* @type:define range:1~255 note:reference to Dcm_ResetRow */
#define DCM_RESET_NUM           1

/** read data ID num */
/* @type:define range:0~255 note:reference to Dcm_ReadDidRow */
#define DCM_READDID_NUM         2

/** security level num,not include locked level */
/* @type:define range:1~2 note:reference to Dcm_SecurityRow */
#define DCM_SECURITY_NUM        1

/** communication control num */
/* @type:define range:0~255 note:reference to Dcm_ComControlRow */
#define DCM_COM_CONTROL_NUM     3

/** write data ID num */
/* @type:define range:1~255 note:reference to Dcm_WriteDidRow */
#define DCM_WRITEDID_NUM        1

/** routine control ID num */
/** write data ID num */
/* @type:define range:1~255 note:reference to Dcm_RoutineControlRow */
#define DCM_ROUTINE_CONTROL_NUM  4

/** test present num */
/* @type:define range:0~255 note:reference to Dcm_testPresentRow */
#define DCM_TESTPRESENT_NUM      1

/** DTC setting mode num */
/* @type:define range:0~255 note:reference to Dcm_DTCSettingRow */
#define DCM_DTC_SET_NUM          2

#endif/* end of DCM_CFG_H */

/*=======[E N D   O F   F I L E]==============================================*/
