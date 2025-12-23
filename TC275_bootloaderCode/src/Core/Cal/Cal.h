/*============================================================================*/
/** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <Cal.h>
 *  @brief      <function declarations for Cal>
 *  
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <Gary Chen>
 *  @date       <2012-12-27>
 */
/*============================================================================*/

#ifndef CAL_H
#define CAL_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *    V1.0    20121227   Gary       Initial version
 *    V1.1    20180511   CChen      update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "std_types.h"
#include "secm.h"

/*=======[E X T E R N A L   D A T A]==========================================*/

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern void Cal_CrcInit(SecM_CRCType *curCrc);

extern void Cal_CrcCal(SecM_CRCType *curCrc, const uint8 *buf, const uint32 size);

extern void Cal_CrcFinalize(SecM_CRCType *curCrc);

#endif/* end of CAL_H */

/*=======[E N D   O F   F I L E]==============================================*/
