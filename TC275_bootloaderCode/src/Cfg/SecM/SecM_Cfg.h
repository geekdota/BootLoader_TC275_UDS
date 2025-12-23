/*============================================================================*/
/** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 *
 *  @file       <SecM_Cfg.h>
 *  @brief      <Config File for Security Module >
 *              Config the SECM_ECU_KEY
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <10086>
 *  @date       <2012-11-09>
 */
/*============================================================================*/
#ifndef SECM_CFG_H
#define SECM_CFG_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121109    Gary       Initial version
 *
 *  V1.1    20130913    ccl        update
 *
 *  V1.2    20180509    CChen      updata
 */
/*============================================================================*/

/*=======[M A C R O S]========================================================*/

/*  CRC method */
#define CAL_CRC16 0x00u
#define CAL_CRC32 0x01u
#define CAL_METHOD CAL_CRC32

/** value k for security access */
/* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
#define SECM_ECU_KEY 0x8704162BU

#endif /* endof SECM_CFG_H */

/*=======[E N D   O F   F I L E]==============================================*/
