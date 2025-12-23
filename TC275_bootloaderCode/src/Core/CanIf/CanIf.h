/*============================================================================*/
/** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 * @file 	CanIf.h
 * @brief
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 * @author 	10086
 * @date 	2013-5-22
 *
 */
/*============================================================================*/
#ifndef CANIF_H_
#define CANIF_H_

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *    V1.0    20130522   10086        Initial version
 *    V1.1    20180511   10086     update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "can.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /*=======[E X T E R N A L   D A T A]==========================================*/
    typedef enum
    {
        CANIF_CS_UNINIT = 0,

        CANIF_CS_STOPPED = 1,

        CANIF_CS_STARTED = 2,

        CANIF_CS_SLEEP = 3

    } CanIf_ControllerModeType;

    typedef void (*TxConf)(PduIdType TpPduId); /* 10086 do */

    typedef struct
    {
        /* symbolic handle for transmit CAN L_PDU */
        const PduIdType PduId;

        /* symbolic handle of TP layer */
        const PduIdType TpPduId;

        /* CAN Identifier of transmit CAN L-PDUs used by the CAN Driver for CAN L-PDU transmission */
        const Can_IdType CanId;

        /* symbolic handle of hardware */
        const uint8 ObjectId;

        /* name of target confirmation services to target upper layers */
        /*const void (*TxConf)(PduIdType TpPduId);*/ /* 10086 do */
        const TxConf TxConf_fun;                     /* 10086 do */

    } CanIf_TxChannelType;

    typedef void (*RxInd)(PduIdType TpPduId, const PduInfoType *const CanTp_objRxPtr); /* 10086 do */
    typedef struct
    {
        /* symbolic handle for receive CAN L-PDU */
        const PduIdType PduId;

        /* symbolic handle of TP layer */
        const PduIdType TpPduId;

        /* CAN Identifier of receive CAN L-PDUs used by the CAN Driver for CAN L-PDU reception */
        const Can_IdType CanId;

        /* symbolic handle of hardware */
        const uint8 ObjectId;

        /* name of target indication services to target upper layers */
        /* const void (*RxInd)(PduIdType TpPduId, const PduInfoType * const CanTp_objRxPtr);*/ /* 10086 do */
        const RxInd RxInd_fun;                                                                 /* 10086 do */

    } CanIf_RxChannelType;

    extern void CanIf_Init(void);

    extern Std_ReturnType CanIf_Transmit(PduIdType Id, const PduInfoType *PduInfoPtr);

    extern void CanIf_TxConfirmation(PduIdType CanTxPduId);

    extern void CanIf_RxIndication(uint8 Hrh, Can_IdType CanId, uint8 CanDlc, uint8 *CanSduPtr);

    extern void CanIf_ControllerBusOff(uint8 Controller);

    extern void CanIf_MainFunction(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CANIF_H_ */

/*=======[E N D   O F   F I L E]==============================================*/
