/*============================================================================*/
/** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 *
 *  @file       <Appl.h>
 *  @brief      <function declarations for Appl>
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <10086>
 *  @date       <2012-12-27>
 */
/*============================================================================*/

#ifndef APPL_H
#define APPL_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/* <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121227    Gary       Initial version
 *  V1.1    20180511    CChen      update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "std_types.h"
#include "fl.h"
#include "secm.h"

#define LED_DEBUG

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern uint8 FunctService255;

extern FL_ResultType Appl_EcuStartup(void);

extern FL_ResultType Appl_FlStartup(void);

#if (FL_SLEEP_TIMER > 0)
extern void Appl_EcuShutdown(void);
#endif

extern void Appl_EcuReset(void);

extern FL_ResultType Appl_CheckConsistency(void);

extern void Appl_UpdateTriggerCondition(void);

extern void Appl_Memcpy(uint8 *dest, const uint8 *source, uint32 length);

extern void Appl_Memset(uint8 *dest, const uint8 source, uint32 length);
#endif
/* end of APPL_H */

/*=======[E N D   O F   F I L E]==============================================*/
