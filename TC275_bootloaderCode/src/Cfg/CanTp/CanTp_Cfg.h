/*============================================================================*/
/*** Copyright (C) 2009-2018, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is 10086 property. Duplication
 *  or disclosure without 10086 written authorization is prohibited.
 *
 *
 *  @file       <CanTp_Cfg.h>
 *  @brief      <The CanTp Configration file>
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <10086>
 *  @date       <2012-12-28>
 */
/*============================================================================*/

#ifndef CANTP_CFG_H
#define CANTP_CFG_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121228    10086     Initial version
 *  V1.1    20180522    10086        Modify version
 */
/*============================================================================*/

/* to get the STM0_CMP0_PERIOD_TIME */
#include "STM_Cfg.h"

/*=======[M A C R O S]========================================================*/
#define ISO15765_BLOCK_SIZE ((uint8)8u) // add by 10086

/** Minimum time the sender shall wait between transmissions of two N-PDU */
/* @type:uint8 range:0~255 note:uint-ms */
#define CANTP_ST_MIN 1

/** N_As timeout for transmission of any CAN frame */
/* @type:uint8 range:0~255 note:uint-ms */
#define CANTP_N_AS 70

/** Timeout for transmission of a CAN frame (ms) */
/* @type:uint8 range:0~255 note:uint-ms */
#define CANTP_N_AR 70

/** N_Bs timeout */
/* @type:uint8 range:0~255 note:uint-ms */
#define CANTP_N_BS 150

/** Time out for consecutive frames (ms) */
/* @type:uint8 range:0~255 note:uint-ms */
#define CANTP_N_CR 150

/* @type:define range:STM0_CMP0_PERIOD_TIME note:auto generate */
#define CANTP_MAIN_TICK STM0_CMP0_PERIOD_TIME

/** padding value */
/* @type:uint8 range:0x00~0xFF note:NONE */
#define CANTP_PADDING_VALUE 0xAAU

/* @type:NONE range:NONE note:auto generate */
#define CANTP_PADDING_ON

/* @type:uint8 range:1~255 note:reference to num of CanTp_TxSdu */
#define CANTP_TX_CHANNEL_NUM 1

/* @type:uint8 range:1~255 note:reference to num of CanTp_RxSdu */
#define CANTP_RX_CHANNEL_NUM 2

#endif

/*=======[E N D   O F   F I L E]==============================================*/

