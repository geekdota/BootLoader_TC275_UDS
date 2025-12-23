/*============================================================================*/
/** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 * @file 	CanIf.c
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
 *    V1.0    20130522   10086        Initial version
 *    V1.1    20180511   10086      update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "canif.h"
#include "canif_cfg.h"

/*=======[E X T E R N A L   D A T A]==========================================*/
extern const CanIf_TxChannelType CanIfTxCh[CANIF_TX_CHANNEL_NUM];
extern const CanIf_RxChannelType CanIfRxCh[CANIF_RX_CHANNEL_NUM];

/*=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/
/******************************************************************************/
/*
 * Brief               Initialize CAN IF.
 * ServiceId           None
 * Sync/Async          Synchronous
 * Reentrancy          Non Reentrant
 * Param-Name[in]      None
 * Param-Name[out]     None
 * Param-Name[in/out]  None
 * Return              None
 * PreCondition        None
 * CallByAPI           CanIf_Init
 */
/******************************************************************************/
void CanIf_Init(void)
{
    Can_Init(NULL_PTR);
    Can_InitController((uint8)0, &Can_ControllerConfigData[0]);
    Can_SetControllerMode((uint8)0, CAN_T_START);
}

/******************************************************************************/
/*
 * Brief               CAN If Transmit
 * ServiceId           None
 * Sync/Async          Synchronous
 * Reentrancy          Non Reentrant
 * Param-Name[in]      Id, *PduInfoPtr
 * Param-Name[out]     None
 * Param-Name[in/out]  None
 * Return              Std_ReturnType
 * PreCondition        None
 * CallByAPI           CanIf_Transmit
 */
/******************************************************************************/
Std_ReturnType CanIf_Transmit(PduIdType Id, const PduInfoType *PduInfoPtr)
{
    const CanIf_TxChannelType *pCh = CanIfTxCh; /* point to canif transmit configuration */
    Can_ReturnType canRes = CAN_NOT_OK;         /* result of can write */
    uint8 iloop;                                /* loop counter */
    Can_PduType canPdu;                         /* store transmit message */
    Std_ReturnType res = (uint8)E_OK;           /* return result */

    for (iloop = (uint8)0; iloop < (uint8)CANIF_TX_CHANNEL_NUM; iloop++)
    {
        if (pCh->TpPduId == Id)
        {
            canPdu.id = pCh->CanId;
            canPdu.length = (uint8)PduInfoPtr->SduLength;
            canPdu.sdu = PduInfoPtr->SduDataPtr;
            canPdu.swPduHandle = pCh->PduId;
            canRes = Can_Write(pCh->ObjectId, &canPdu);
            break;
        }
        else
        {
            /* empty */
        }
        pCh++;
    }

    if (canRes != CAN_OK)
    {
        res = (uint8)E_NOT_OK;
    }
    else
    {
        /* empty */
    }

    return res;
}

/******************************************************************************/
/*
 * Brief               CAN If Tx Confirmation
 * ServiceId           None
 * Sync/Async          Synchronous
 * Reentrancy          Non Reentrant
 * Param-Name[in]      CanTxPduId
 * Param-Name[out]     None
 * Param-Name[in/out]  None
 * Return              None
 * PreCondition        None
 * CallByAPI           CanIf_TxConfirmation
 */
/******************************************************************************/
void CanIf_TxConfirmation(PduIdType CanTxPduId)
{
    const CanIf_TxChannelType *pCh = CanIfTxCh; /* point to canif configuration */
    uint8 iloop;                                /* loop counter */

    for (iloop = (uint8)0; iloop < (uint8)CANIF_TX_CHANNEL_NUM; iloop++)
    {
        /* if PduId matches, call confirmation */
        if (pCh->PduId == CanTxPduId)
        {
            pCh->TxConf_fun(pCh->TpPduId);
            break;
        }
        else
        {
            /* empty */
        }
        pCh++;
    }
}

/******************************************************************************/
/*
 * Brief               CAN If Rx Indication
 * ServiceId           None
 * Sync/Async          Synchronous
 * Reentrancy          Non Reentrant
 * Param-Name[in]      Hrh, CanId, CanDlc, *CanSduPtr
 * Param-Name[out]     None
 * Param-Name[in/out]  None
 * Return              None
 * PreCondition        None
 * CallByAPI           CanIf_RxIndication
 */
/******************************************************************************/
void CanIf_RxIndication(uint8 Hrh, Can_IdType CanId, uint8 CanDlc, uint8 *CanSduPtr)
{
    const CanIf_RxChannelType *pCh = CanIfRxCh; /* point to can if rx configuration */
    uint8 iloop;                                /* loop counter */
    PduInfoType PduInfo;                        /* store received message */

    for (iloop = (uint8)0; iloop < (uint8)CANIF_RX_CHANNEL_NUM; iloop++)
    {
        if ((pCh->ObjectId == Hrh) && (pCh->CanId == CanId))
        {
            PduInfo.SduLength = CanDlc;
            PduInfo.SduDataPtr = CanSduPtr;
            /* call CanTp rx indication */
            pCh->RxInd_fun(pCh->TpPduId, &PduInfo);
            break;
        }
        else
        {
            /* empty */
        }
        pCh++;
    }
}

/******************************************************************************/
/*
 * Brief               CAN If Controller Bus Off
 * ServiceId           None
 * Sync/Async          Synchronous
 * Reentrancy          Non Reentrant
 * Param-Name[in]      Controller
 * Param-Name[out]     None
 * Param-Name[in/out]  None
 * Return              None
 * PreCondition        None
 * CallByAPI           CanIf_ControllerBusOff
 */
/******************************************************************************/
void CanIf_ControllerBusOff(uint8 Controller)
{
    /* empty */
    if (Controller != 0) /* 10086 do */
    {
        /* do nothing */
    }
    return;
}

/******************************************************************************/
/*
 * Brief               CAN If Main Function
 * ServiceId           None
 * Sync/Async          Synchronous
 * Reentrancy          Non Reentrant
 * Param-Name[in]      None
 * Param-Name[out]     None
 * Param-Name[in/out]  None
 * Return              None
 * PreCondition        None
 * CallByAPI           CanIf_MainFunction
 */
/******************************************************************************/
void CanIf_MainFunction(void)
{
#if (STD_ON == CAN_TX_POLLING)
    Can_MainFunction_Write();
#endif

#if (STD_ON == CAN_RX_POLLING)
    Can_MainFunction_Read();
#endif

#if (STD_ON == CAN_WAKEUP_POLLING)
    Can_MainFunction_Wakeup();
#endif

#if (STD_ON == CAN_BUSOFF_POLLING)
    Can_MainFunction_BusOff();
#endif
}

/*=======[E N D   O F   F I L E]==============================================*/
