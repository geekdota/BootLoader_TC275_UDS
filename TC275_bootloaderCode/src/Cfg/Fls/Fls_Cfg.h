/*============================================================================*/
/** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 *
 *  @file       <Flash_Cfg.h>
 *  @brief      <Flash Driver Configuration file>
 *               describe the falsh driver version and clock set.
 *
 *  @author     <10086>
 *  @date       <2012-09-09>
 *
 *
 *  @author     <10086>
 *  @date       <2018-05-09>
 */
/*============================================================================*/
#ifndef FLASH_CFG_H
#define FLASH_CFG_H

/*=======[M A C R O S]========================================================*/
#define FLS_USED STD_OFF

/** Motorola Star12 */
#define TFLASH_DRIVER_VERSION_MCUTYPE 0x12u

/** some mask number */
#define TFLASH_DRIVER_VERSION_MASKTYPE 0xabu

/** interface version number */
#define TFLASH_DRIVER_VERSION_INTERFACE 0x01u

/** major version number / interface */
#define FLASH_DRIVER_VERSION_MAJOR 0x01u

/** minor version number / internal */
#define FLASH_DRIVER_VERSION_MINOR 0x01u

/** bugfix / patchlevel */
#define FLASH_DRIVER_VERSION_PATCH 0x00u

/** config if flash driver is compiled */
#define TFLASH_COMPILED STD_OFF

/* Eight 16k, one 128k nine 256k */
#define FLASH_ONE_SECTOR (16 * 1024)

#define FLASH_ONE_PHRASE 8u
#endif /* endof FLASH_CFG_H */

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20120909    Gary       Initial version
 *  V1.1    20180509    CC         update
 */
/*=======[E N D   O F   F I L E]==============================================*/
