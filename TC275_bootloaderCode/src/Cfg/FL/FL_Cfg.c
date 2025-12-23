/** Copyright (C) 2009-2017, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <FL_Cfg.c>
 *  @brief      <Flash Loader Configuration >
 *               describe the block information.
 *  
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <cywang>
 *  @date       <2016-10-15>
 */
/*============================================================================*/

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *     V1.0   20121227   Gary       Initial version
 *
 *     V1.1   20160801  cywang        update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "fl.h"

/*=======[E X T E R N A L   D A T A]==========================================*/
const FL_BlockDescriptorType FL_BlkInfo[FL_NUM_LOGICAL_BLOCKS] =
{
    /* start global address,length,    maximum attempt */
    /* @type:uint32 range:0x0000C000~0xFFFFFFFF note:NONE */
    /* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
    /* @type:uint8 range:0x00~0xFF note:NONE */
		{
				0xA0038000U, 0x000400U, (uint16)0xFFFFU
		},/* 1K, flash erase and write code, for FEE driver */
#if 0
		{
				0xA005E000U, 0x002000U, (uint16)0xFFFFU
		},/* 8K, interrupt vector table */
#endif
		{
				0xA0080000U, 0x180000U, (uint16)0xFFFFU
		},/* 1.5M, BSW code */
		{
				0xA0200000U, 0x100000U, (uint16)0xFFFFU
		},/* 1M, ASW code */
		{
				0xA0340000U, 0x0A0000U, (uint16)0xFFFFU
		},/* 640K, calibration data */
		{
				0xA03E0000, 0x0001000U, (uint16)0xFFFFU
		} /* 4K, UDS DID const data */
};

/*=======[E N D   O F   F I L E]==============================================*/
