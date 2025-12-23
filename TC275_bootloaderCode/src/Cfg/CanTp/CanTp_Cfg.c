/*============================================================================*/
/*** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 *
 *  @file       <CanTp.c>
 *  @brief      <canTp configuration file>
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <10086>
 *  @date       <2012-12-28>
 */
/*============================================================================*/

/*========[R E V I S I O N   H I S T O R Y]===================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121228    10086    Initial version
 *  V1.1    20180522    10086        Modify version
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "cantp.h"
#include "CanTp_Cfg.h"
/*=======[E X T E R N A L   D A T A]==========================================*/
const CanTp_RxSduType CanTp_RxSdu[CANTP_RX_CHANNEL_NUM] =
    {
        {/* rxDcmId */
         /* @type:uint16 range:0x0000~0xFFFF note:auto generate */
         (uint16)0xCCU,

         /* rxPduId */
         /* @type:uint16 range:0x0000~0xFFFF note:reference to Can_RxPdu->pduId */
         (uint16)0x11U,

         /* addressingFormat */
         /* @type:ENUM range:NONE note:NONE */
         CANTP_STANDARD,

         /* nSa */
         /* @type:uint8 range:0x00~0xFF note:NONE */
         (uint8)0x00U,

         /* txFcPduId */
         /* @type:uint16 range:0x0000~0xFFFF note:reference to Can_TxPdu->pduId */
         (uint16)0x01U},
        {/* rxDcmId */
         /* @type:uint16 range:0x0000~0xFFFF note:auto generate */
         (uint16)0xFFU,

         /* rxPduId */
         /* @type:uint16 range:0x0000~0xFFFF note:reference to Can_RxPdu->pduId */
         (uint16)0x12U,

         /* addressingFormat */
         /* @type:ENUM range:NONE note:NONE */
         CANTP_STANDARD,

         /* nSa */
         /* @type:uint8 range:0x00~0xFF note:NONE */
         (uint8)0x00U,

         /* txFcPduId */
         /* @type:uint16 range:0x0000~0xFFFF note:reference to Can_TxPdu->pduId */
         (uint16)0x01U}};

const CanTp_TxSduType CanTp_TxSdu[CANTP_TX_CHANNEL_NUM] =
    {
        {/* txDcmId */
         /* @type:uint16 range:0x0000~0xFFFF note:auto generate */
         (uint16)0xAAU,

         /* txPduId */
         /* @type:uint16 range:0x0000~0xFFFF note:reference to Can_TxPdu->pduId */
         (uint16)0x01U,

         /* rxFcPduId */
         /* @type:uint16 range:0x0000~0xFFFF note:reference to Can_RxPdu->pduId */
         (uint16)0x11U}};

/*=======[E N D   O F   F I L E]==============================================*/
