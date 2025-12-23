/*============================================================================*/
/*** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  
 *  @file       <CanTp.h>
 *  @brief      <The CanTp used type defined and extern function declarations>
 *  
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *  
 *  @author     <Tommy Yu>
 *  @date       <2012-12-28>
 */
/*============================================================================*/

#ifndef CAN_TP_H
#define CAN_TP_H

/*=========[R E V I S I O N   H I S T O R Y]==================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121228     Tommy Yu     Initial version
 *  V1.1    20180511     CChen        update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "comstack_types.h"
#include "can.h"
#include "cantp_cfg.h"

/*=======[M A C R O S]========================================================*/
typedef enum
{
    CANTP_EXTENDED,

    CANTP_STANDARD

} CanTp_Address_FormatType;

typedef struct
{
    /*       rxPduId               rxDcmId         */
    /* Can -------------> CanTp -------------> DCM */
    /*       txFcPduId                             */
    /*     <------------                           */
    const uint16 rxDcmId;

    const PduIdType rxPduId;

    const CanTp_Address_FormatType addressingFormat;

    const uint8 nSa;

    const PduIdType txFcPduId;

} CanTp_RxSduType;

typedef struct
{
    /*         txPduId              txDcmId        */
    /* Can <------------- CanTp <------------- DCM */
    /*        rxFcPduId                            */
    /*      ------------>                          */
    const PduIdType txDcmId;

    const PduIdType txPduId;

    const PduIdType rxFcPduId;

} CanTp_TxSduType;

/*=======[E X T E R N A L   D A T A]==========================================*/
extern const CanTp_RxSduType CanTp_RxSdu[CANTP_RX_CHANNEL_NUM];
extern const CanTp_TxSduType CanTp_TxSdu[CANTP_TX_CHANNEL_NUM];

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern void CanTp_Transmit(const PduIdType CanTp_txSduId,
                           const PduInfoType * const CanTp_txInfoPtr);
extern void CanTp_RxIndication(const PduIdType CanTp_pduId,
                               const PduInfoType * const CanTp_objRxPtr);
extern void CanTp_TxConfirmation(const PduIdType CanTp_pduId);
extern void CanTp_MainFunction(void);
extern void CanTp_Init(void);

#endif

/*=========[E N D   O F   F I L E]==============================================*/
