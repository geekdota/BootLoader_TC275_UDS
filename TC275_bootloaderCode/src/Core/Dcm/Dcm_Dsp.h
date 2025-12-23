/*============================================================================*/
/** Copyright (C) 2009-2018, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is 10086 property. Duplication 
 *  or disclosure without 10086 written authorization is prohibited.
 *  
 *  @file       <Dcm_Dsp.h>
 *  @brief      <Macro and function declarations for Dcm Module>
 *  
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *  
 *  @author     <Gary Chen>
 *  @date       <2012-11-09>
 */
/*============================================================================*/
#ifndef DCM_DSP_H
#define DCM_DSP_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *     V1.0   20121109   Gary       Initial version
 *
 *     V1.1   20160801  cywang      update
 *
 *     V1.2   20180511  CChen       update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "dcm.h"
#include "dcm_cfg.h"
#include "fl.h"

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern void Dcm_RecvMsg10(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

#if (STD_ON == DCM_SERVICE_11_ENABLE)
extern void Dcm_RecvMsg11(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);
#endif

#if ((DCM_READDID_NUM > 0) && (STD_ON == DCM_SERVICE_22_ENABLE))
extern void Dcm_RecvMsg22(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);
#endif

extern void Dcm_RecvMsg2E(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

extern void Dcm_RecvMsg27(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

extern void Dcm_RecvMsg28(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

extern void Dcm_RecvMsg34(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

extern void Dcm_RecvMsg36(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

extern void Dcm_RecvMsg37(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

extern void Dcm_RecvMsg31(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

extern void Dcm_CheckProgPreCondition(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

extern void Dcm_CheckProgDependencies(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

extern void Dcm_CheckMemory(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

extern void Dcm_EraseMemory(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

extern void Dcm_RecvMsg3E(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

extern void Dcm_RecvMsg85(const Dcm_BuffType * rxBuff, Dcm_BuffType * txBuff);

extern void Dcm_Pending2E(const FL_ResultType errorCode,
                          const Dcm_BuffType * rxBuff,
                          Dcm_BuffType * txBuff);

extern void Dcm_Pending36(const FL_ResultType errorCode,
                          const Dcm_BuffType * rxBuff,
                          Dcm_BuffType * txBuff);

extern void Dcm_Pending31(const FL_ResultType errorCode,
                          const Dcm_BuffType * rxBuff,
                          Dcm_BuffType * txBuff);

#endif/* end of DCM_DSP_H */

/*=======[E N D   O F   F I L E]==============================================*/

