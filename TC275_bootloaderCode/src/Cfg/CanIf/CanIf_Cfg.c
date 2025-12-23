/*============================================================================*/
/** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 * @file 	CanIf_Cfg.c
 * @brief
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 * @author 	10086
 * @date 	2013-5-22
 *
 */
/*============================================================================*/

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20130522    10086       Initial version
 *  V1.1    20180522    10086        Modify version
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "canif_cfg.h"
#include "canif.h"
#include "cantp.h"

/*=======[E X T E R N A L   D A T A]==========================================*/
const CanIf_TxChannelType CanIfTxCh[CANIF_TX_CHANNEL_NUM] =
    {
        {(uint16)0x00U, (uint16)0x01U, (uint32)0x7ECU, (uint8)0x03U, &CanTp_TxConfirmation}};

const CanIf_RxChannelType CanIfRxCh[CANIF_RX_CHANNEL_NUM] =
    {
        {(uint16)0x00U, (uint16)0x11U, (uint32)0x7E4U, (uint8)0x00U, &CanTp_RxIndication},
        {(uint16)0x01U, (uint16)0x12U, (uint32)0x7DFU, (uint8)0x01U, &CanTp_RxIndication},
        {(uint16)0x01U, (uint16)0x12U, (uint32)0x214U, (uint8)0x02U, &CanTp_RxIndication}};

/*=======[E N D   O F   F I L E]==============================================*/
