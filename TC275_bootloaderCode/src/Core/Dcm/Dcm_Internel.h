/*============================================================================*/
/** Copyright (C) 2009-2018, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is 10086 property. Duplication 
 *  or disclosure without 10086 written authorization is prohibited.
 *  
 *  @file       <Dcm_Internel.h>
 *  @brief      <Macro and function declarations for Dcm Module>
 *  
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *  
 *  @author     <Gary Chen>
 *  @date       <2012-11-09>
 */
/*============================================================================*/
#ifndef DCM_INTERNEL_H
#define DCM_INTERNEL_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121109    Gary       Initial version
 *
 *  V1.1    20180511    CChen      update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "dcm.h"
#include "dcm_cfg.h"

/*=======[E X T E R N A L   D A T A]==========================================*/
/** use static memory for service */
extern uint8 Dcm_MainRxBuff[DCM_RX_BUF_SIZE];
extern uint8 Dcm_MainTxBuff[DCM_TX_BUF_SIZE];
/* flag of resetting by itself */
extern boolean dcmDummyDefault;

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
/* Interface used for DCM Init */
extern void Dcm_DspInit(void);

/* Interface used for DCM Dsp */
extern void Dcm_ServiceFinish(void);

extern void Dcm_SetSessionMode(const Dcm_SessionType sessMode);

extern boolean Dcm_CheckSessionSupp(const Dcm_SessionType sessionSupportMask);

extern boolean Dcm_CheckSessionSuppMask(const Dcm_SessionType sessionSupportMask);

extern boolean Dcm_CheckSessionSuppMask_2E(const Dcm_SessionType sessionSupportMask);

extern void Dcm_SetSecurityLevel(const Dcm_SecurityType secLev);

extern boolean Dcm_GetSecurityTimerExpired(void);

extern boolean Dcm_CheckSecuritySupp(const Dcm_SecurityType securitySupportMask);

extern void Dcm_StartSecurityTimer(uint32 timeOut);

extern void Dcm_StartResetTimer(uint16 timeOut);

extern void Dcm_SendRsp(void);

extern void Dcm_SendNcr(uint8 nrcCode);

#endif/* endof DCM_INTERNEL_H */

/*=======[E N D   O F   F I L E]==============================================*/

