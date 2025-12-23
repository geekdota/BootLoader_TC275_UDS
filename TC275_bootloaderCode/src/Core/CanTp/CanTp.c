/*============================================================================*/
/*** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 *
 *  @file       <CanTp.c>
 *  @brief      <TP (ISO 15765) source component>
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <10086>
 *  @date       <2012-12-28>
 */
/*============================================================================*/

/*=========[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *    V1.0    20121228 10086     Initial version
 *    V1.1    20180511 10086       update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "canif.h"
#include "cantp.h"
#include "dcm.h"

/*=======[M A C R O S]========================================================*/
#define CANTP_FC_STATUS_CTS 0
#define CANTP_FC_STATUS_WT 1
#define CANTP_FC_STATUS_OVFLW 2

/** PCI mask */
#define CANTP_TPCI_MASK 0xF0U

/** Single Frame */
#define CANTP_TPCI_SF 0x00U

/** First Frame */
#define CANTP_TPCI_FF 0x10U

/** Consecutive Frame */
#define CANTP_TPCI_CF 0x20U

/** Flow Control */
#define CANTP_TPCI_FC 0x30U

/** Single frame data length mask */
#define CANTP_TPCI_DL 0x0FU

/** Flow control status mask */
#define CANTP_TPCI_FS_MASK 0x0FU

/** max data length of SF with standard address */
#define CANTP_STD_ADDR_MAX_SF_LEGNTH 7

/** max data length of SF with extended address */
#define CANTP_EXT_ADDR_MAX_SF_LEGNTH 6

#define CANTP_STD_ADDR_MAX_FF_LENGTH 6

/** max data length of CF with standard address */
#define CANTP_STD_ADDR_MAX_CF_LENGTH 7

/** data offset of standard address */
#define CANTP_STD_ADDR_SF_DATA_OFFSET 1

/** data offset of extended address */
#define CANTP_EXT_ADDR_SF_DATA_OFFSET 2

/** data offset of standard address */
#define CANTP_STD_ADDR_FF_DATA_OFFSET 2

#define CANTP_STD_ADDR_CF_DATA_OFFSET 1
#define CANTP_STD_ADDR_FC_BS_OFFSET 1
#define CANTP_STD_ADDR_FC_STMIN_OFFSET 2

#define CANTP_SEGMENT_NUMBER_MASK 0x0fU
#define CANTP_DRV_PDU_MAX_LENGTH 8

#define CANTP_TOTAL_CHANNEL_NUM (CANTP_TX_CHANNEL_NUM + CANTP_RX_CHANNEL_NUM)

#define CANTP_INDEX_NOT_FIND 0xFFU

/**=======[T Y P E   D E F I N I T I O N S]====================================*/
typedef enum
{
    /* Not specified by ISO15765 - used as error return type when decoding frame. */
    CANTP_INVALID_FRAME,

    CANTP_SINGLE_FRAME,

    CANTP_FIRST_FRAME,

    CANTP_CONSECUTIVE_FRAME,

    /* FC with CTS:Continus to send */
    CANTP_FLOW_CONTROL_CTS_FRAME,

    CANTP_FLOW_CONTROL_WT

} CanTp_Iso15765FrameType;

typedef enum
{
    CANTP_STATE_IDLE = 0,

    /* wait rx data buffer */
    /* wait buffer for single frame */
    CANTP_WAITING_RX_SF_BUFFER = 1,

    /* wait buffer for SF or CF frame,send control frame */
    CANTP_WAITING_RX_CF_BUFFER = 2,

    /* wait FC confirmation  */
    /* FC with CTS status confirmation */
    CANTP_WAITING_FOR_FC_CTS_TX_CONFIRMATION = 3,

    /* FC with overflow status confirmation */
    CANTP_WAITING_FOR_FC_OVFLW_TX_CONFIRMATION = 4,

    /* wait for cf frame */
    CANTP_WAITING_FOR_CF_RX = 5,

    /* wait buffer for SF or FF, maybe is not useful */
    CANTP_WAITING_TX_BUFFER = 6,

    /* single process */
    /* Single frame only */
    CANTP_WAITING_FOR_SF_TX_CONFIRMATION = 7,

    /* FF, CF process */
    CANTP_WAITING_FOR_FF_TX_CONFIRMATION = 8,
    CANTP_WAITING_FOR_FC_RX = 9,

    /* transmit CF */
    CANTP_WAITING_FOR_CF_TX = 10,
    CANTP_WAITING_FOR_LAST_CF_TX_CONFIRMATION = 11,
    CANTP_WAITING_FOR_CF_TX_CONFIRMATION = 12,
    CANTP_WAITING_FOR_CF_BLOCK_TX_CONFIRMATION = 13,
    WAITING_TX_FC_BUFFER = 14
} CanTp_StateType;

typedef struct
{
    uint8 data[8];

    uint8 byteCount;

} CanTp_PduType;

typedef struct
{
    /* Counter keeping track total frames handled. */
    uint8 framesHandledCount;

    /* In case we are transmitters the remote node can configure this value (only valid for TX). */
    uint8 stmin;

    /* Blocksize (only valid for TX).if FC with BS=0, the runtime BS will be 0xFFFF */
    uint16 bs;

    /* time counter */
    uint8 timeoutCounterValue;

    /* Transfer state machine. */
    CanTp_StateType tpState;

    /* The PDUR make an instance of this. */
    PduInfoType *pdurBuffer;

    /* Number of bytes in PDUR buffer, which has been written to dsm buffer */
    uint16 pdurBufferCount;

    /* Total length of the PDU. */
    uint16 transferTotal;

    /* Counter ongoing transfer. */
    uint16 transferCount;

    /* Temp storage of SDU data. */
    CanTp_PduType canFrameBuffer;

    /* Temp storage of Received block size*/
    uint16 CanTp_ReceivedCFBlockSize;

} CanTp_RunTime_DataType;

#define CanTp_ConvertMsToMainCycles(x) ((uint8)((x) / CANTP_MAIN_TICK))

/**=======[I N T E R N A L   D A T A]==========================================*/
STATIC CanTp_RunTime_DataType CanTp_RuntimeData[CANTP_TOTAL_CHANNEL_NUM];

/**=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
#ifdef CANTP_PADDING_CHECK
STATIC boolean CanTp_FramePaddingCheck(const PduInfoType *const CanTp_objRxPtr,
                                       const uint8 CanTp_checkDataOffset);
#endif

#ifdef CANTP_PADDING_ON
STATIC void CanTp_FrameAddPadding(PduInfoType *const CanTp_pduInfo);
#else
#define CanTp_FrameAddPadding(x)
#endif

STATIC void CanTp_ReceiveSingleFrame(CanTp_RunTime_DataType *const CanTp_rxRuntime,
                                     const CanTp_RxSduType *const CanTp_rxConfig,
                                     const PduInfoType *const CanTp_objRxPtr,
                                     const uint8 CanTp_length);

STATIC void CanTp_ReceiveFirstFrame(CanTp_RunTime_DataType *const CanTp_rxRuntime,
                                    const CanTp_RxSduType *const CanTp_rxConfig,
                                    const PduInfoType *const CanTp_objRxPtr,
                                    const PduLengthType pduLength);

STATIC void CanTp_ReceiveConsecutiveFrame(CanTp_RunTime_DataType *const CanTp_rxRuntime,
                                          const CanTp_RxSduType *const CanTp_rxConfig,
                                          const PduInfoType *const CanTp_objRxPtr);

STATIC void CanTp_ReceiveFlowControlFrame(CanTp_RunTime_DataType *const CanTp_txRuntime,
                                          const CanTp_TxSduType *const CanTp_txConfig,
                                          const PduInfoType *const CanTp_objRxPtr,
                                          const CanTp_Iso15765FrameType frameType);

STATIC void CanTp_Init15765RuntimeData(CanTp_RunTime_DataType *const CanTp_runtime);

STATIC BufReq_ReturnType CanTp_CopySegmentToRxBuffer(const CanTp_RxSduType *const CanTp_rxConfig,
                                                     CanTp_RunTime_DataType *const CanTp_rxRuntime);

STATIC uint8 CanTp_FindTpRxIndicationIndexFromRxChannel(const PduIdType CanTp_pduId,
                                                        const PduInfoType *const CanTp_objRxPtr,
                                                        uint8 *const CanTp_tpci,
                                                        uint8 *const CanTp_tpciOffset);

STATIC uint8 CanTp_FindTpRxIndexFromTxChannel(const PduIdType CanTp_pduId);

STATIC uint8 CanTp_FindTpListIndexFromRxPdu(const PduIdType CanTp_pduId,
                                            const PduInfoType *const CanTp_objRxPtr,
                                            CanTp_Iso15765FrameType *const CanTp_format,
                                            PduLengthType *const pduLength);

STATIC uint8 CanTp_FindTpTxConfirmIndexFromTxChannel(PduIdType const CanTp_txPduId);

STATIC uint8 CanTp_FindTpTxConfirmIndexFromRxChannel(PduIdType const CanTp_txPduId);

STATIC uint8 CanTp_FindTpTxListFromSduId(const PduIdType CanTp_SduId);

STATIC CanTp_Iso15765FrameType CanTp_calcTxFrameType(const CanTp_RunTime_DataType *const CanTp_txRuntime);

STATIC void CanTp_SendFlowControlFrame(const CanTp_RxSduType *const CanTp_rxConfig,
                                       CanTp_RunTime_DataType *const CanTp_rxRuntime,
                                       BufReq_ReturnType const CanTp_flowStatus);

STATIC boolean CanTp_CopyDataToMsgFromTxBuf(const CanTp_TxSduType *const CanTp_txConfig,
                                            CanTp_RunTime_DataType *const CanTp_txRuntime,
                                            PduInfoType *const CanTp_pduInfo,
                                            const uint8 t_u1_actualPayload);

STATIC void CanTp_SendSingleFrame(const CanTp_TxSduType *const CanTp_txConfig,
                                  CanTp_RunTime_DataType *const CanTp_txRuntime,
                                  PduInfoType *const CanTp_pduInfo);

STATIC void CanTp_SendFirstFrame(const CanTp_TxSduType *const CanTp_txConfig,
                                 CanTp_RunTime_DataType *const CanTp_txRuntime,
                                 PduInfoType *const CanTp_pduInfo);

STATIC void CanTp_SendConsecutiveFrame(const CanTp_TxSduType *const CanTp_txConfig,
                                       CanTp_RunTime_DataType *const CanTp_txRuntime,
                                       PduInfoType *const CanTp_pduInfo);

STATIC void CanTp_SendTxFrame(const CanTp_TxSduType *const CanTp_txConfig,
                              CanTp_RunTime_DataType *const CanTp_txRuntime,
                              const CanTp_Iso15765FrameType CanTp_framType);

STATIC void CanTp_WaitSForLastCFBufProcess(const CanTp_RxSduType *const CanTp_rxConfig,
                                           CanTp_RunTime_DataType *const CanTp_runtimeItem);

STATIC void CanTp_WaitCFBufProcess(const CanTp_RxSduType *const CanTp_rxConfig,
                                   CanTp_RunTime_DataType *const CanTp_runtimeItem);

STATIC void CanTp_WaitFCTxBufProcess(const CanTp_RxSduType *CanTp_rxConfig,
                                     CanTp_RunTime_DataType *CanTp_runtimeItem);

/**=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/

/********************************************************************************/
/****
 * @brief			:CanTp_Init
 *
 * Service ID		: <CanTp_Init>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
void CanTp_Init(void)
{
    uint8 loop;

    for (loop = (uint8)0; loop < (uint8)CANTP_TOTAL_CHANNEL_NUM; loop++)
    {
        /* initial runtime item */
        CanTp_Init15765RuntimeData(&CanTp_RuntimeData[loop]);
    }

    return;
}

/********************************************************************************/
/****
 * @brief			:CanTp_RxIndication
 *
 * Service ID		: <CanTp_RxIndication>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
void CanTp_RxIndication(const PduIdType CanTp_pduId,
                        const PduInfoType *const CanTp_objRxPtr)
{
    /* SF, FF, CF, FC */
    CanTp_Iso15765FrameType frameType;
    PduLengthType pduLength;

    /* find */
    uint8 const CanTp_listIndex = CanTp_FindTpListIndexFromRxPdu(CanTp_pduId,
                                                                 CanTp_objRxPtr,
                                                                 &frameType,
                                                                 &pduLength);

    if (CanTp_listIndex != (uint8)CANTP_INDEX_NOT_FIND)
    {
        switch (frameType)
        {
        case CANTP_INVALID_FRAME:
            break;

        case CANTP_SINGLE_FRAME:
            /* deal with single frame */
            CanTp_ReceiveSingleFrame(&CanTp_RuntimeData[CanTp_listIndex],
                                     &CanTp_RxSdu[CanTp_listIndex],
                                     CanTp_objRxPtr, (uint8)pduLength);
            break;

        case CANTP_FIRST_FRAME:
            /* physics address FF */
            CanTp_ReceiveFirstFrame(&CanTp_RuntimeData[CanTp_listIndex],
                                    &CanTp_RxSdu[CanTp_listIndex],
                                    CanTp_objRxPtr, pduLength);
            break;

        case CANTP_CONSECUTIVE_FRAME:
            /** deal with consecutive frame */
            CanTp_ReceiveConsecutiveFrame(&CanTp_RuntimeData[CanTp_listIndex],
                                          &CanTp_RxSdu[CanTp_listIndex],
                                          CanTp_objRxPtr);
            break;

        case CANTP_FLOW_CONTROL_CTS_FRAME:
            /* RX, FC; tx channel */
            /* deal with a flow control frame */
            CanTp_ReceiveFlowControlFrame(&CanTp_RuntimeData[CanTp_listIndex + (uint8)CANTP_RX_CHANNEL_NUM],
                                          &CanTp_TxSdu[CanTp_listIndex],
                                          CanTp_objRxPtr, frameType);
            break;

        case CANTP_FLOW_CONTROL_WT:
            /* wait flow control frame */
            CanTp_RuntimeData[CanTp_listIndex + (uint8)CANTP_RX_CHANNEL_NUM].tpState = CANTP_WAITING_FOR_FC_RX;
            CanTp_RuntimeData[CanTp_listIndex + (uint8)CANTP_RX_CHANNEL_NUM].timeoutCounterValue = CanTp_ConvertMsToMainCycles((uint8)CANTP_N_BS);
            break;

        default:
            break;
        }
    }
    else
    {
        /* empty */
    }

    return;
}

/********************************************************************************/
/****
 * @brief			:CanTp_TxConfirmation
 *
 * Service ID		: <CanTp_TxConfirmation>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
void CanTp_TxConfirmation(const PduIdType CanTp_pduId)
{
    /* find tp index from tx channel */
    uint8 CanTp_listIndex = CanTp_FindTpTxConfirmIndexFromTxChannel(CanTp_pduId);
    CanTp_RunTime_DataType *CanTp_runtimeItem;
    CanTp_RunTime_DataType *CanTp_rxRuntime;

    if (CanTp_listIndex != (uint8)CANTP_INDEX_NOT_FIND)
    {
        /* tx:SF, CF or FF confirmation, tx channel */
        CanTp_runtimeItem = &CanTp_RuntimeData[CanTp_listIndex + (uint8)CANTP_RX_CHANNEL_NUM];

        switch (CanTp_runtimeItem->tpState)
        {
        case CANTP_WAITING_FOR_SF_TX_CONFIRMATION:
        case CANTP_WAITING_FOR_LAST_CF_TX_CONFIRMATION:
            /* transmit ok */
            Dcm_TxConfirmation(CanTp_TxSdu[CanTp_listIndex].txDcmId, NTFRSLT_OK);
            CanTp_Init15765RuntimeData(CanTp_runtimeItem);
            break;

        case CANTP_WAITING_FOR_FF_TX_CONFIRMATION:
        case CANTP_WAITING_FOR_CF_BLOCK_TX_CONFIRMATION:
            /* wait flow control frame */
            CanTp_runtimeItem->tpState = CANTP_WAITING_FOR_FC_RX;
            CanTp_runtimeItem->timeoutCounterValue = CanTp_ConvertMsToMainCycles(CANTP_N_BS);
            break;

        case CANTP_WAITING_FOR_CF_TX_CONFIRMATION:
            CanTp_runtimeItem->tpState = CANTP_WAITING_FOR_CF_TX;
            break;

        default:
            /* wrong state */
            Dcm_TxConfirmation(CanTp_TxSdu[CanTp_listIndex].txDcmId,
                               NTFRSLT_E_NOT_OK);
            CanTp_Init15765RuntimeData(CanTp_runtimeItem);
            break;
        }
    }
    else
    {
        /* TX: FC, rx channel */
        /* find tp index from rx channel, the FC confirmation */
        CanTp_listIndex = CanTp_FindTpTxConfirmIndexFromRxChannel(CanTp_pduId);
        if (CanTp_listIndex != (uint8)CANTP_INDEX_NOT_FIND)
        {
            /* FC tx confirmation */
            CanTp_rxRuntime = &CanTp_RuntimeData[CanTp_listIndex];

            if (CANTP_WAITING_FOR_FC_CTS_TX_CONFIRMATION == CanTp_rxRuntime->tpState)
            {
                /* FC confirmation */
                CanTp_rxRuntime->tpState = CANTP_WAITING_FOR_CF_RX;
                CanTp_rxRuntime->timeoutCounterValue = CanTp_ConvertMsToMainCycles(CANTP_N_CR);
            }
            else
            {
                /* wrong state */
                Dcm_RxIndication(CanTp_RxSdu[CanTp_listIndex].rxDcmId, NTFRSLT_E_NOT_OK);
                CanTp_Init15765RuntimeData(CanTp_rxRuntime);
            }
        }
        else
        {
            /* empty */
        }
    }

    return;
}

/********************************************************************************/
/****
 * @brief			:CanTp_Transmit
 *
 * Service ID		: <CanTp_Transmit>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
void CanTp_Transmit(const PduIdType CanTp_txSduId,
                    const PduInfoType *const CanTp_txInfoPtr)
{
    /* point to the message transmitted */
    CanTp_RunTime_DataType *CanTp_txRuntime;

    /* find tp list index from SduId which should be send */
    const uint8 CanTp_listIndex = CanTp_FindTpTxListFromSduId(CanTp_txSduId);

    if (CanTp_listIndex != (uint8)CANTP_INDEX_NOT_FIND)
    {
        CanTp_txRuntime = &CanTp_RuntimeData[CanTp_listIndex + (uint8)CANTP_RX_CHANNEL_NUM];

        if (CANTP_STATE_IDLE == CanTp_txRuntime->tpState)
        {
            /*
             * the TP state is IDLE state, and then change the trans state to
             * prepare sent
             */
            CanTp_txRuntime->transferTotal = CanTp_txInfoPtr->SduLength;
            /* change state to wait tx buffer */
            CanTp_txRuntime->tpState = CANTP_WAITING_TX_BUFFER;
        }
        else
        {
            /* empty */
        }
    }
    else
    {
        /* empty */
    }

    return;
}

/********************************************************************************/
/****
 * @brief			:CanTp_MainFunc
 *
 * Service ID		: <CanTp_MainFunc>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
void CanTp_MainFunction(void)
{
    uint8 mainLoop;
    CanTp_Iso15765FrameType frameType;
    CanTp_RunTime_DataType *CanTp_runtimeItem = NULL_PTR;

    for (mainLoop = (uint8)0; mainLoop < (uint8)CANTP_TOTAL_CHANNEL_NUM; mainLoop++)
    {
        /* process every channel */
        CanTp_runtimeItem = &CanTp_RuntimeData[mainLoop];

        if (CANTP_STATE_IDLE != CanTp_runtimeItem->tpState)
        {

            if ((CANTP_WAITING_RX_CF_BUFFER == CanTp_runtimeItem->tpState) || (CANTP_WAITING_RX_SF_BUFFER == CanTp_runtimeItem->tpState) || (CANTP_WAITING_TX_BUFFER == CanTp_runtimeItem->tpState) || (WAITING_TX_FC_BUFFER == CanTp_runtimeItem->tpState))
            {

                /* for calc frame type, SF or FF */
                switch (CanTp_runtimeItem->tpState)
                {

                case CANTP_WAITING_RX_SF_BUFFER:
                    if (mainLoop < (uint8)CANTP_RX_CHANNEL_NUM)
                    {
                        /* wait single buffer of last CF buffer */
                        CanTp_WaitSForLastCFBufProcess(&CanTp_RxSdu[mainLoop], CanTp_runtimeItem);
                    }
                    else
                    {
                        /* empty */
                    }
                    break;

                case CANTP_WAITING_RX_CF_BUFFER:
                    if (mainLoop < (uint8)CANTP_RX_CHANNEL_NUM)
                    {
                        /* wait CF buffer */
                        CanTp_WaitCFBufProcess(&CanTp_RxSdu[mainLoop], CanTp_runtimeItem);
                    }
                    else
                    {
                        /* empty */
                    }
                    break;

                case CANTP_WAITING_TX_BUFFER:
                    /* calc the frame type which should be sent */
                    frameType = CanTp_calcTxFrameType(CanTp_runtimeItem);
                    /* send fram SF or FF */
                    CanTp_SendTxFrame(&CanTp_TxSdu[mainLoop - (uint8)CANTP_RX_CHANNEL_NUM],
                                      CanTp_runtimeItem, frameType);
                    break;

                case WAITING_TX_FC_BUFFER:
                    if (mainLoop < CANTP_RX_CHANNEL_NUM)
                    {
                        /* wait CF buffer */
                        CanTp_WaitFCTxBufProcess(&CanTp_RxSdu[mainLoop], CanTp_runtimeItem);
                    }
                    break;

                default:
                    break;
                }
            }
            else
            {
                /* wait tx confirmation or rx indication  */
                /* Decrement timeout counter, if not already zero. */
                if (CanTp_runtimeItem->timeoutCounterValue > (uint8)0)
                {
                    CanTp_runtimeItem->timeoutCounterValue--;
                }
                else
                {
                    /* empty */
                }

                if ((uint8)0 == CanTp_runtimeItem->timeoutCounterValue)
                {
                    /* time out */
                    switch (CanTp_runtimeItem->tpState)
                    {
                    /* rx channel */
                    case CANTP_WAITING_FOR_FC_CTS_TX_CONFIRMATION:
                    case CANTP_WAITING_FOR_FC_OVFLW_TX_CONFIRMATION:
                    case CANTP_WAITING_FOR_CF_RX:
                        /* receive data time */
                        if (mainLoop < (uint8)CANTP_RX_CHANNEL_NUM)
                        {
                            Dcm_RxIndication(CanTp_RxSdu[mainLoop].rxDcmId, NTFRSLT_E_NOT_OK);
                        }
                        else
                        {
                            /* empty */
                        }
                        CanTp_Init15765RuntimeData(CanTp_runtimeItem);
                        break;

                    /* tx channel */
                    /* signal frame tx confirmation */
                    case CANTP_WAITING_FOR_SF_TX_CONFIRMATION:
                    /* first frame tx confirmation */
                    case CANTP_WAITING_FOR_FF_TX_CONFIRMATION:
                    /* last CF of this message */
                    case CANTP_WAITING_FOR_LAST_CF_TX_CONFIRMATION:
                    /* wait for CF tx confirmation */
                    case CANTP_WAITING_FOR_CF_TX_CONFIRMATION:
                    /* CF tx confirmation ,if confirmation then wait FC fram */
                    case CANTP_WAITING_FOR_CF_BLOCK_TX_CONFIRMATION:
                    /* wait for flow control fram */
                    case CANTP_WAITING_FOR_FC_RX:
                        /* transmit data timeout */
                        Dcm_TxConfirmation(
                            CanTp_TxSdu[mainLoop - (uint8)CANTP_RX_CHANNEL_NUM].txDcmId,
                            NTFRSLT_E_NOT_OK);
                        CanTp_Init15765RuntimeData(CanTp_runtimeItem);
                        break;

                    case CANTP_WAITING_FOR_CF_TX:
                        /* send CF */
                        CanTp_SendTxFrame(&CanTp_TxSdu[mainLoop - (uint8)CANTP_RX_CHANNEL_NUM],
                                          CanTp_runtimeItem, CANTP_CONSECUTIVE_FRAME);
                        break;

                    default:
                        break;
                    }
                }
                else
                {
                    /* empty */
                }
            }

        } /* end of CANTP_STATE_IDLE */
        else
        {
            /* empty */
        }
    }
    return;
}

/********************************************************************************/
/****
 * @brief			:CanTp_FramePaddingCheck
 *
 * Service ID		: <CanTp_FramePaddingCheck>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
#ifdef CANTP_PADDING_CHECK
STATIC boolean CanTp_FramePaddingCheck(const PDU_INFO *const CanTp_objRxPtr,
                                       const uint8 CanTp_checkDataOffset)
{
    uint8 loop;
    boolean checkOk = TRUE;

    if (CANTP_DRV_PDU_MAX_LENGTH == CanTp_objRxPtr->SduLength)
    {
        for (loop = CanTp_checkDataOffset;
             (loop < CANTP_DRV_PDU_MAX_LENGTH) && (TRUE == checkOk);
             loop++)
        {
            if (CANTP_PADDING_VALUE != CanTp_objRxPtr->SduDataPtr[loop])
            {
                /* check error */
                checkOk = FALSE;
            }
        }
    }
    else
    {
        checkOk = FALSE;
    }
    return checkOk;
}
#endif

/********************************************************************************/
/****
 * @brief			:if less than 8, add padding to message
 *
 * Service ID		: <CanTp_FrameAddPadding>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
#ifdef CANTP_PADDING_ON
STATIC void CanTp_FrameAddPadding(PduInfoType *const CanTp_pduInfo)
{
    for (; CanTp_pduInfo->SduLength < (uint16)CANTP_DRV_PDU_MAX_LENGTH; CanTp_pduInfo->SduLength++)
    {
        /* add padding */
        CanTp_pduInfo->SduDataPtr[CanTp_pduInfo->SduLength] = (uint8)CANTP_PADDING_VALUE;
    }
}
#endif

/********************************************************************************/
/****
 * @brief			:CanTp_ReceiveSingleFrame
 *
 * Service ID		: <CanTp_ReceiveSingleFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC void CanTp_ReceiveSingleFrame(CanTp_RunTime_DataType *const CanTp_rxRuntime,
                                     const CanTp_RxSduType *const CanTp_rxConfig,
                                     const PduInfoType *const CanTp_objRxPtr,
                                     const uint8 CanTp_length)
{
    uint8 loop;
    uint8 dataOffset;
#ifdef CANTP_PADDING_CHECK
    boolean paddingCheckOk = FALSE;
#endif

    if (CANTP_STANDARD == CanTp_rxConfig->addressingFormat)
    {
        /* standard address frame */
        dataOffset = (uint8)CANTP_STD_ADDR_SF_DATA_OFFSET;
    }
    else
    {
        /* extended address frame */
        dataOffset = (uint8)CANTP_EXT_ADDR_SF_DATA_OFFSET;
    }

#ifdef CANTP_PADDING_CHECK
    /* padding check */
    paddingCheckOk = CanTp_FramePaddingCheck(CanTp_objRxPtr, (CanTp_length + dataOffset));
    if (TRUE == paddingCheckOk)
#endif
    {
        if (CanTp_rxRuntime->tpState != CANTP_STATE_IDLE)
        {
            /* not idle state, reset state */
            Dcm_RxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_NOT_OK);
            CanTp_Init15765RuntimeData(CanTp_rxRuntime);
        }
        else
        {
            /* empty */
        }

        for (loop = (uint8)0; loop < CanTp_length; loop++)
        {
            /* copy data to local buffer */
            CanTp_rxRuntime->canFrameBuffer.data[loop] =
                CanTp_objRxPtr->SduDataPtr[dataOffset + loop];
        }
        CanTp_rxRuntime->canFrameBuffer.byteCount = CanTp_length;
        CanTp_rxRuntime->transferTotal = (uint16)CanTp_length;
        /* change state */
        CanTp_rxRuntime->tpState = CANTP_WAITING_RX_SF_BUFFER;
    }
#ifdef CANTP_PADDING_CHECK
    else
    {
        /* empty */
    }
#endif

    return;
}

/********************************************************************************/
/****
 * @brief			:CanTp_ReceiveFirstFrame
 *
 * Service ID		: <CanTp_ReceiveFirstFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC void CanTp_ReceiveFirstFrame(CanTp_RunTime_DataType *const CanTp_rxRuntime,
                                    const CanTp_RxSduType *const CanTp_rxConfig,
                                    const PduInfoType *const CanTp_objRxPtr,
                                    const PduLengthType pduLength)
{
    uint8 loop;

    if ((uint16)CANTP_DRV_PDU_MAX_LENGTH == CanTp_objRxPtr->SduLength)
    {
        /* first frame length must be 8 */
        if (CanTp_rxRuntime->tpState != CANTP_STATE_IDLE)
        {
            /* check if is a correct state */
            Dcm_RxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_NOT_OK);
            CanTp_Init15765RuntimeData(CanTp_rxRuntime);
        }
        else
        {
            /* empty */
        }

        for (loop = (uint8)0; loop < (uint8)CANTP_STD_ADDR_MAX_FF_LENGTH; loop++)
        {
            /* copy data to local buffer */
            CanTp_rxRuntime->canFrameBuffer.data[loop] =
                CanTp_objRxPtr->SduDataPtr[(uint8)CANTP_STD_ADDR_FF_DATA_OFFSET + loop];
        }

        CanTp_rxRuntime->canFrameBuffer.byteCount = (uint8)CANTP_STD_ADDR_MAX_FF_LENGTH;
        CanTp_rxRuntime->transferTotal = pduLength;

        CanTp_rxRuntime->tpState = CANTP_WAITING_RX_CF_BUFFER;
        CanTp_rxRuntime->framesHandledCount++;
    }
    else
    {
        /* empty */
    }

    return;
}

/********************************************************************************/
/****
 * @brief			:CanTp_ReceiveConsecutiveFrame
 *
 * Service ID		: <CanTp_ReceiveConsecutiveFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC void CanTp_ReceiveConsecutiveFrame(CanTp_RunTime_DataType *const CanTp_rxRuntime,
                                          const CanTp_RxSduType *const CanTp_rxConfig,
                                          const PduInfoType *const CanTp_objRxPtr)
{
    uint8 loop;
    boolean lastCF = FALSE;
    uint8 dataCount;
    uint16 const bytesLeftToCopy = CanTp_rxRuntime->transferTotal - CanTp_rxRuntime->transferCount;
    uint8 const CanTp_sn = CanTp_objRxPtr->SduDataPtr[(uint8)0] & (uint8)CANTP_SEGMENT_NUMBER_MASK;
#ifdef CANTP_PADDING_CHECK
    boolean paddingCheckOk = FALSE;
#endif

    if (CanTp_rxRuntime->tpState != CANTP_WAITING_FOR_CF_RX)
    {
        /* error state rx cf frame */
        Dcm_RxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_NOT_OK);
        CanTp_Init15765RuntimeData(CanTp_rxRuntime);
    }
    else
    {
        if (CanTp_sn != CanTp_rxRuntime->framesHandledCount)
        {
            /* "unit too fast? Increase STmin (cofig) to slow it down!\n");*/
            Dcm_RxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_WRONG_SN);
            CanTp_Init15765RuntimeData(CanTp_rxRuntime);
        }
        else
        {
            CanTp_rxRuntime->framesHandledCount++;
            /* increase for sn */
            CanTp_rxRuntime->framesHandledCount &= (uint8)CANTP_SEGMENT_NUMBER_MASK;

            if (bytesLeftToCopy <= (uint16)CANTP_STD_ADDR_MAX_CF_LENGTH)
            {
                /* last cf of this message */
                dataCount = (uint8)bytesLeftToCopy; /** 1-7 */
                lastCF = TRUE;
            }
            else
            {
                /* depends on addressing CanTp_format used. */
                dataCount = (uint8)CANTP_STD_ADDR_MAX_CF_LENGTH;
            }

#ifdef CANTP_PADDING_CHECK
            /* padding check */
            paddingCheckOk = CanTp_FramePaddingCheck(CanTp_objRxPtr,
                                                     (dataCount + CANTP_STD_ADDR_SF_DATA_OFFSET));
            if (TRUE == paddingCheckOk)
#endif
            {
                /* copy data */
                for (loop = (uint8)0; loop < dataCount; loop++)
                {
                    /* copy to buffer */
                    CanTp_rxRuntime->pdurBuffer->SduDataPtr[CanTp_rxRuntime->pdurBufferCount] =
                        CanTp_objRxPtr->SduDataPtr[loop + (uint8)CANTP_STD_ADDR_CF_DATA_OFFSET];
                    CanTp_rxRuntime->pdurBufferCount++;
                }

                /* calc transfer counter */
                CanTp_rxRuntime->transferCount += (uint16)dataCount;

                CanTp_rxRuntime->CanTp_ReceivedCFBlockSize++;
                if (TRUE == lastCF)
                {
                    /* last cf, complete */
                    Dcm_RxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_OK);
                    CanTp_Init15765RuntimeData(CanTp_rxRuntime);
                }
                else
                {
                    /* reset time counter */
                    CanTp_rxRuntime->timeoutCounterValue = CanTp_ConvertMsToMainCycles(CANTP_N_CR);
                }
            }
#ifdef CANTP_PADDING_CHECK
            else
            {
                /* padding check not ok */
                Dcm_RxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_NOT_OK);
                CanTp_Init15765RuntimeData(CanTp_rxRuntime);
            }
#endif
        }
    }

    return;
}

/********************************************************************************/
/****
 * @brief			:CanTp_ReceiveConsecutiveFrame
 *
 * Service ID		: <CanTp_ReceiveConsecutiveFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC void CanTp_ReceiveFlowControlFrame(CanTp_RunTime_DataType *const CanTp_txRuntime,
                                          const CanTp_TxSduType *const CanTp_txConfig,
                                          const PduInfoType *const CanTp_objRxPtr,
                                          const CanTp_Iso15765FrameType frameType)
{
#ifdef CANTP_PADDING_CHECK
    boolean paddingCheckOk = FALSE;
#endif

    if (CANTP_WAITING_FOR_FC_RX == CanTp_txRuntime->tpState)
    {
        /* state ok */
        if (CANTP_FLOW_CONTROL_CTS_FRAME == frameType)
        {
#ifdef CANTP_PADDING_CHECK
            /* padding check */
            paddingCheckOk = CanTp_FramePaddingCheck(CanTp_objRxPtr,
                                                     ((uint8)CANTP_STD_ADDR_FC_STMIN_OFFSET + 1));
            if (TRUE == paddingCheckOk)
#endif
            {
                CanTp_txRuntime->bs =
                    (uint16)CanTp_objRxPtr->SduDataPtr[(uint8)CANTP_STD_ADDR_FC_BS_OFFSET];
                if ((uint16)0 == CanTp_txRuntime->bs)
                {
                    CanTp_txRuntime->bs = (uint16)0xFFFFu;
                }
                else
                {
                    /* empty */
                }

                if (((CanTp_objRxPtr->SduDataPtr[(uint8)CANTP_STD_ADDR_FC_STMIN_OFFSET] > (uint8)0x7FU) && (CanTp_objRxPtr->SduDataPtr[(uint8)CANTP_STD_ADDR_FC_STMIN_OFFSET] < (uint8)0xF0U)) || (CanTp_objRxPtr->SduDataPtr[(uint8)CANTP_STD_ADDR_FC_STMIN_OFFSET] > (uint8)0xF9U))
                {
                    /* if STmin is reserved value */
                    Dcm_TxConfirmation(CanTp_txConfig->txDcmId, NTFRSLT_E_NOT_OK);
                    CanTp_Init15765RuntimeData(CanTp_txRuntime);
                }
                else
                {
                    /* get STmin value */
                    if (CanTp_objRxPtr->SduDataPtr[(uint8)CANTP_STD_ADDR_FC_STMIN_OFFSET] > (uint8)0x7FU)
                    {
                        CanTp_txRuntime->stmin = (uint8)0;
                    }
                    else
                    {
                        CanTp_txRuntime->stmin =
                            CanTp_ConvertMsToMainCycles((uint8)CanTp_objRxPtr->SduDataPtr[(uint8)CANTP_STD_ADDR_FC_STMIN_OFFSET]);
                    }
                    CanTp_txRuntime->tpState = CANTP_WAITING_FOR_CF_TX;
                    /* clear time, send data immediately */
                    CanTp_txRuntime->timeoutCounterValue = (uint8)0;
                }
            }
#ifdef CANTP_PADDING_CHECK
            else
            {
                /* padding check not ok */
                Dcm_TxConfirmation(CanTp_txConfig->txDcmId, NTFRSLT_E_NOT_OK);
                CanTp_Init15765RuntimeData(CanTp_txRuntime);
            }
#endif
        }
        else
        {
            Dcm_TxConfirmation(CanTp_txConfig->txDcmId, NTFRSLT_E_NOT_OK);
            CanTp_Init15765RuntimeData(CanTp_txRuntime);
        }
    }
    else
    {
        /* empty */
    }

    return;
}

/********************************************************************************/
/****
 * @brief			:CanTp_Init15765RuntimeData
 *
 * Service ID		: <CanTp_Init15765RuntimeData>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC void CanTp_Init15765RuntimeData(CanTp_RunTime_DataType *const CanTp_runtime)
{
    CanTp_runtime->framesHandledCount = (uint8)0;
    CanTp_runtime->stmin = (uint8)0;
    CanTp_runtime->bs = (uint16)0;
    CanTp_runtime->timeoutCounterValue = (uint8)0;
    CanTp_runtime->tpState = CANTP_STATE_IDLE;
    CanTp_runtime->pdurBuffer = NULL_PTR;
    CanTp_runtime->pdurBufferCount = (uint16)0;
    CanTp_runtime->transferTotal = (uint16)0;
    CanTp_runtime->transferCount = (uint16)0;
    CanTp_runtime->canFrameBuffer.byteCount = (uint8)0;
    CanTp_runtime->CanTp_ReceivedCFBlockSize = 0u;
    return;
}

/********************************************************************************/
/***
 * @brief			:CanTp_CopySegmentToRxBuffer
 *
 * Service ID		: <CanTp_CopySegmentToRxBuffer>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC BufReq_ReturnType CanTp_CopySegmentToRxBuffer(const CanTp_RxSduType *const CanTp_rxConfig,
                                                     CanTp_RunTime_DataType *const CanTp_rxRuntime)
{
    BufReq_ReturnType Can_ret = BUFREQ_E_NOT_OK;
    uint8 loop;

    if (NULL_PTR == CanTp_rxRuntime->pdurBuffer)
    {
        /* buffer should be enough for message */
        Can_ret = Dcm_ProvideRxBuffer(CanTp_rxConfig->rxDcmId,
                                      CanTp_rxRuntime->transferTotal,
                                      &CanTp_rxRuntime->pdurBuffer);
    }
    else
    {
        /* empty */
    }

    if ((CanTp_rxRuntime->pdurBuffer != NULL_PTR) && (CanTp_rxRuntime->pdurBuffer->SduLength > CanTp_rxRuntime->pdurBufferCount))
    {
        Can_ret = BUFREQ_OK;

        for (loop = (uint8)0;
             loop < CanTp_rxRuntime->canFrameBuffer.byteCount;
             loop++)
        {
            /* copy local buffer value to provided buffer from DCM */
            CanTp_rxRuntime->pdurBuffer->SduDataPtr[CanTp_rxRuntime->pdurBufferCount] = CanTp_rxRuntime->canFrameBuffer.data[loop];
            CanTp_rxRuntime->transferCount++;
            CanTp_rxRuntime->pdurBufferCount++;
        }
    }
    else
    {
        /* empty */
    }

    return Can_ret;
}

/********************************************************************************/
/***
 * @brief			:CanTp_FindTpRxIndicationIndexFromRxChannel
 *
 * Service ID		: <CanTp_FindTpRxIndicationIndexFromRxChannel>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC uint8 CanTp_FindTpRxIndicationIndexFromRxChannel(const PduIdType CanTp_pduId,
                                                        const PduInfoType *const CanTp_objRxPtr,
                                                        uint8 *const CanTp_tpci,
                                                        uint8 *const CanTp_tpciOffset)
{
    uint8 loop;
    uint8 index = (uint8)CANTP_INDEX_NOT_FIND;
    const CanTp_RxSduType *CanTp_rxConfig;

    for (loop = (uint8)0;
         (loop < (uint8)CANTP_RX_CHANNEL_NUM) && ((uint8)CANTP_INDEX_NOT_FIND == index); loop++)
    {
        CanTp_rxConfig = &CanTp_RxSdu[loop];

        /* check is this channel */
        if ((CanTp_rxConfig->rxPduId == CanTp_pduId) && (((CANTP_STANDARD == CanTp_rxConfig->addressingFormat) && ((CanTp_objRxPtr->SduDataPtr[0] & (uint8)CANTP_TPCI_MASK) < (uint8)CANTP_TPCI_FC)) || ((CANTP_EXTENDED == CanTp_rxConfig->addressingFormat) && (CanTp_rxConfig->nSa == CanTp_objRxPtr->SduDataPtr[0u]) && ((CanTp_objRxPtr->SduDataPtr[1] & (uint8)CANTP_TPCI_MASK) < (uint8)CANTP_TPCI_FC))))
        {
            if (CANTP_STANDARD == CanTp_rxConfig->addressingFormat)
            {
                *CanTp_tpci = CanTp_objRxPtr->SduDataPtr[0];
                *CanTp_tpciOffset = (uint8)0;
            }
            else
            {
                *CanTp_tpci = CanTp_objRxPtr->SduDataPtr[1];
                *CanTp_tpciOffset = (uint8)1;
            }
            index = loop;
        }
        else
        {
            /* empty */
        }
    }

    return index;
}

/********************************************************************************/
/***
 * @brief			:CanTp_FindTpRxIndicationIndexFromTxChannel
 *
 * Service ID		: <CanTp_FindTpRxIndicationIndexFromTxChannel>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC uint8 CanTp_FindTpRxIndexFromTxChannel(const PduIdType CanTp_pduId)
{
    uint8 index = (uint8)CANTP_INDEX_NOT_FIND;
    uint8 loop;

    for (loop = (uint8)0;
         (loop < (uint8)CANTP_TX_CHANNEL_NUM) && ((uint8)CANTP_INDEX_NOT_FIND == index);
         loop++)
    {
        /* check it is the receiver channel pduid */
        if (CanTp_pduId == CanTp_TxSdu[loop].rxFcPduId)
        {
            index = loop;
        }
        else
        {
            /* empty */
        }
    }

    return index;
}

/********************************************************************************/
/***
 * @brief			:CanTp_FindTpListIndexFromRxPdu
 *
 * Service ID		: <CanTp_FindTpListIndexFromRxPdu>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC uint8 CanTp_FindTpListIndexFromRxPdu(const PduIdType CanTp_pduId,
                                            const PduInfoType *const CanTp_objRxPtr,
                                            CanTp_Iso15765FrameType *const CanTp_format,
                                            PduLengthType *const pduLength)
{
    uint8 index;
    uint8 CanTp_tpci = (uint8)0;
    uint8 CanTp_tpciOffset = (uint8)0;
    const CanTp_RxSduType *CanTp_rxConfig;

    /* find index from received message */
    index = CanTp_FindTpRxIndicationIndexFromRxChannel(CanTp_pduId, CanTp_objRxPtr,
                                                       &CanTp_tpci, &CanTp_tpciOffset);

    if ((uint8)CANTP_INDEX_NOT_FIND == index)
    {
        /* find index from received message */
        index = CanTp_FindTpRxIndexFromTxChannel(CanTp_pduId);
        CanTp_tpci = CanTp_objRxPtr->SduDataPtr[(uint8)0];
    }
    else
    {
        /* empty */
    }

    if (index != (uint8)CANTP_INDEX_NOT_FIND)
    {
        CanTp_rxConfig = &CanTp_RxSdu[index];

        switch (CanTp_tpci & (uint8)CANTP_TPCI_MASK)
        {
        case CANTP_TPCI_SF:
            *CanTp_format = CANTP_SINGLE_FRAME;
            *pduLength = (PduLengthType)(CanTp_objRxPtr->SduDataPtr[CanTp_tpciOffset] & (uint8)CANTP_TPCI_DL);
            if (((*pduLength > (uint16)CANTP_STD_ADDR_MAX_SF_LEGNTH) && (CANTP_STANDARD == CanTp_rxConfig->addressingFormat)) || ((*pduLength > (uint16)CANTP_EXT_ADDR_MAX_SF_LEGNTH) && (CANTP_EXTENDED == CanTp_rxConfig->addressingFormat)) || ((uint16)0 == *pduLength))
            {
                /* error frame, if single frame length is 0 of the length is less than 6 or 7 */
                *CanTp_format = CANTP_INVALID_FRAME;
            }
            else
            {
                /* empty */
            }
            break;

        case CANTP_TPCI_FF:
            *CanTp_format = CANTP_FIRST_FRAME;
            *pduLength = ((PduLengthType)(CanTp_objRxPtr->SduDataPtr[(uint8)0] & (uint16)0xfU) << (uint8)8) + (PduLengthType)CanTp_objRxPtr->SduDataPtr[(uint8)1];
            if (*pduLength < ((uint16)CANTP_STD_ADDR_MAX_SF_LEGNTH + (uint16)1))
            {
                /* error frame, for this tp we only support standard frame */
                *CanTp_format = CANTP_INVALID_FRAME;
            }
            else
            {
                /* empty */
            }
            break;

        case CANTP_TPCI_CF:
            *CanTp_format = CANTP_CONSECUTIVE_FRAME;
            break;

        case CANTP_TPCI_FC:
            /* we only receive FC with CTS */
            if ((uint8)CANTP_FC_STATUS_CTS == (uint8)(CanTp_tpci & (uint8)CANTP_TPCI_FS_MASK))
            {
                *CanTp_format = CANTP_FLOW_CONTROL_CTS_FRAME;
            }
            else if ((uint8)CANTP_FC_STATUS_WT == (uint8)(CanTp_tpci & (uint8)CANTP_TPCI_FS_MASK))
            {
                *CanTp_format = CANTP_FLOW_CONTROL_WT;
            }
            else if ((uint8)CANTP_FC_STATUS_OVFLW == (uint8)(CanTp_tpci & (uint8)CANTP_TPCI_FS_MASK))
            {
                /* if WAIT or OVFLW not ok */
                *CanTp_format = CANTP_INVALID_FRAME;
                Dcm_RxIndication(CanTp_RxSdu[index].rxDcmId, NTFRSLT_E_NOT_OK);
                CanTp_Init15765RuntimeData(&CanTp_RuntimeData[index + (uint8)CANTP_RX_CHANNEL_NUM]);
            }
            else
            {
                *CanTp_format = CANTP_INVALID_FRAME;
            }
            break;

        default:
            *CanTp_format = CANTP_INVALID_FRAME;
            break;
        }
    }
    else
    {
        /* empty */
    }

    return index;
}

/********************************************************************************/
/***
 * @brief			:CanTp_FindTpTxConfirmIndexFromTxChannel
 *
 * Service ID		: <CanTp_FindTpTxConfirmIndexFromTxChannel>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC uint8 CanTp_FindTpTxConfirmIndexFromTxChannel(PduIdType const CanTp_txPduId)
{
    uint8 loop;
    uint8 index = (uint8)CANTP_INDEX_NOT_FIND;
    CanTp_RunTime_DataType *CanTp_txRuntime;

    for (loop = (uint8)0;
         (loop < (uint8)CANTP_TX_CHANNEL_NUM) && ((uint8)CANTP_INDEX_NOT_FIND == index);
         loop++)
    {
        /* search tp index */
        CanTp_txRuntime = &CanTp_RuntimeData[loop + (uint8)CANTP_RX_CHANNEL_NUM];

        if ((CanTp_TxSdu[loop].txPduId == CanTp_txPduId) && ((CANTP_WAITING_FOR_SF_TX_CONFIRMATION == CanTp_txRuntime->tpState) || (CANTP_WAITING_FOR_FF_TX_CONFIRMATION == CanTp_txRuntime->tpState) || (CANTP_WAITING_FOR_CF_TX_CONFIRMATION == CanTp_txRuntime->tpState) || (CANTP_WAITING_FOR_LAST_CF_TX_CONFIRMATION == CanTp_txRuntime->tpState) || (CANTP_WAITING_FOR_CF_BLOCK_TX_CONFIRMATION == CanTp_txRuntime->tpState)))
        {
            index = loop;
        }
        else
        {
            /* empty */
        }
    }

    return index;
}

/********************************************************************************/
/***
 * @brief			:CanTp_FindTpTxConfirmIndexFromRxChannel
 *
 * Service ID		: <CanTp_FindTpTxConfirmIndexFromRxChannel>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC uint8 CanTp_FindTpTxConfirmIndexFromRxChannel(PduIdType const CanTp_txPduId)
{
    uint8 rx_Index = (uint8)CANTP_INDEX_NOT_FIND;
    uint8 tx_Index = (uint8)CANTP_INDEX_NOT_FIND;

    uint8 loop;

    for (loop = (uint8)0;
         (loop < (uint8)CANTP_TX_CHANNEL_NUM) && ((uint8)CANTP_INDEX_NOT_FIND == tx_Index);
         loop++)
    {
        if (CanTp_txPduId == CanTp_TxSdu[loop].txPduId)
        {
            tx_Index = loop;
        }
        else
        {
            /* empty */
        }
    }

    for (loop = (uint8)0;
         (loop < (uint8)CANTP_RX_CHANNEL_NUM) && ((uint8)CANTP_INDEX_NOT_FIND == rx_Index);
         loop++)
    {
        if ((CanTp_txPduId == CanTp_RxSdu[loop].txFcPduId) &&
            (CanTp_RxSdu[loop].rxPduId == CanTp_TxSdu[tx_Index].rxFcPduId))
        {
            rx_Index = loop;
        }
        else
        {
            /* empty */
        }
    }

    return rx_Index;
}

/********************************************************************************/
/***
 * @brief			:CanTp_FindTpTxListFromSduId
 *
 * Service ID		: <CanTp_FindTpTxListFromSduId>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC uint8 CanTp_FindTpTxListFromSduId(const PduIdType CanTp_SduId)
{
    uint8 index = (uint8)CANTP_INDEX_NOT_FIND;
    uint8 loop;

    for (loop = (uint8)0;
         (loop < (uint8)CANTP_TX_CHANNEL_NUM) && ((uint8)CANTP_INDEX_NOT_FIND == index);
         loop++)
    {
        if (CanTp_SduId == CanTp_TxSdu[loop].txDcmId)
        {
            index = loop;
        }
        else
        {
            /* empty */
        }
    }

    return index;
}

/********************************************************************************/
/***
 * @brief			:CanTp_calcTxFrameType
 *
 * Service ID		: <CanTp_calcTxFrameType>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC CanTp_Iso15765FrameType CanTp_calcTxFrameType(const CanTp_RunTime_DataType *const CanTp_txRuntime)
{
    CanTp_Iso15765FrameType Can_ret = CANTP_INVALID_FRAME;

    /* CANTP_STANDARD */
    if (CanTp_txRuntime->transferTotal > (uint16)CANTP_STD_ADDR_MAX_SF_LEGNTH)
    {
        /* first frame */
        Can_ret = CANTP_FIRST_FRAME;
    }
    else
    {
        /* single frame */
        Can_ret = CANTP_SINGLE_FRAME;
    }

    return Can_ret;
}

/********************************************************************************/
/***
 * @brief			:CanTp_SendFlowControlFrame
 *
 * Service ID		: <CanTp_SendFlowControlFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC void CanTp_SendFlowControlFrame(const CanTp_RxSduType *const CanTp_rxConfig,
                                       CanTp_RunTime_DataType *const CanTp_rxRuntime,
                                       BufReq_ReturnType const CanTp_flowStatus)
{
    PduInfoType CanTp_pduInfo;
    uint8 sduData[8];

    CanTp_pduInfo.SduDataPtr = sduData;
    CanTp_pduInfo.SduLength = (uint16)0;

    if (BUFREQ_OK == CanTp_flowStatus)
    {
        /* add PCI, and STmin */
        sduData[CanTp_pduInfo.SduLength] = (uint8)CANTP_TPCI_FC | (uint8)CANTP_FC_STATUS_CTS;
        CanTp_pduInfo.SduLength++;
        sduData[CanTp_pduInfo.SduLength] = (uint8)0u;
        CanTp_pduInfo.SduLength++;
        sduData[CanTp_pduInfo.SduLength] = (uint8)CANTP_ST_MIN;
        CanTp_pduInfo.SduLength++;
    }
    else
    {
        sduData[CanTp_pduInfo.SduLength] = (uint8)CANTP_TPCI_FC | (uint8)CANTP_FC_STATUS_OVFLW;
        CanTp_pduInfo.SduLength++;
    }

    /* add padding */
    CanTp_FrameAddPadding(&CanTp_pduInfo);

    /* maybe should for a tolerant value */
    CanTp_rxRuntime->timeoutCounterValue = CanTp_ConvertMsToMainCycles(CANTP_N_AR);

    /* send frame */
    (void)CanIf_Transmit(CanTp_rxConfig->txFcPduId, &CanTp_pduInfo);

    return;
}

/********************************************************************************/
/***
 * @brief			:CanTp_CopyDataToMsgFromTxBuf
 *
 * Service ID		: <CanTp_CopyDataToMsgFromTxBuf>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC boolean CanTp_CopyDataToMsgFromTxBuf(const CanTp_TxSduType *const CanTp_txConfig,
                                            CanTp_RunTime_DataType *const CanTp_txRuntime,
                                            PduInfoType *const CanTp_pduInfo,
                                            const uint8 t_u1_actualPayload)
{
    boolean framPrepare = FALSE;
    uint8 loop;

    if (NULL_PTR == CanTp_txRuntime->pdurBuffer)
    {
        /* first get buffer */
        (void)Dcm_ProvideTxBuffer(CanTp_txConfig->txDcmId, &CanTp_txRuntime->pdurBuffer);
    }
    else
    {
        /* empty */
    }

    if (CanTp_txRuntime != NULL_PTR)
    {
        /* buffer ok, copy data */
        for (loop = (uint8)0; loop < t_u1_actualPayload; loop++)
        {
            /* copy local buffer to provided buffer by DCM */
            CanTp_pduInfo->SduDataPtr[CanTp_pduInfo->SduLength] = CanTp_txRuntime->pdurBuffer->SduDataPtr[CanTp_txRuntime->pdurBufferCount];
            CanTp_txRuntime->transferCount++;
            CanTp_pduInfo->SduLength++;
            CanTp_txRuntime->pdurBufferCount++;
        }

        framPrepare = TRUE;
    }
    else
    {
        /* empty */
    }

    CanTp_FrameAddPadding(CanTp_pduInfo);

    return framPrepare;
}

/********************************************************************************/
/***
 * @brief			:CanTp_SendSingleFrame
 *
 * Service ID		: <CanTp_SendSingleFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC void CanTp_SendSingleFrame(const CanTp_TxSduType *const CanTp_txConfig,
                                  CanTp_RunTime_DataType *const CanTp_txRuntime,
                                  PduInfoType *const CanTp_pduInfo)
{
    boolean framPrepare;

    CanTp_pduInfo->SduDataPtr[CanTp_pduInfo->SduLength] =
        (uint8)CANTP_TPCI_SF + (uint8)CanTp_txRuntime->transferTotal;
    CanTp_pduInfo->SduLength++;

    framPrepare = CanTp_CopyDataToMsgFromTxBuf(CanTp_txConfig,
                                               CanTp_txRuntime,
                                               CanTp_pduInfo,
                                               (uint8)CanTp_txRuntime->transferTotal);

    if (TRUE == framPrepare)
    {
        CanTp_txRuntime->tpState = CANTP_WAITING_FOR_SF_TX_CONFIRMATION;
        /* load time of N_As */
        CanTp_txRuntime->timeoutCounterValue = CanTp_ConvertMsToMainCycles(CANTP_N_AS);
        (void)CanIf_Transmit(CanTp_txConfig->txPduId, CanTp_pduInfo);
    }
    else
    {
        Dcm_TxConfirmation(CanTp_txConfig->txDcmId, NTFRSLT_E_NO_BUFFER);
        CanTp_Init15765RuntimeData(CanTp_txRuntime);
    }
}

/********************************************************************************/
/***
 * @brief			:CanTp_SendFirstFrame
 *
 * Service ID		: <CanTp_SendFirstFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC void CanTp_SendFirstFrame(const CanTp_TxSduType *const CanTp_txConfig,
                                 CanTp_RunTime_DataType *const CanTp_txRuntime,
                                 PduInfoType *const CanTp_pduInfo)
{
    const uint8 sfActualPayload = (uint8)CANTP_STD_ADDR_MAX_FF_LENGTH;
    boolean framPrepare;

    CanTp_pduInfo->SduDataPtr[CanTp_pduInfo->SduLength] = (uint8)CANTP_TPCI_FF | (uint8)((CanTp_txRuntime->transferTotal & (uint16)0x0f00U) >> (uint8)8);
    CanTp_pduInfo->SduLength++;
    CanTp_pduInfo->SduDataPtr[CanTp_pduInfo->SduLength] =
        (uint8)(CanTp_txRuntime->transferTotal & (uint16)0xffu);
    CanTp_pduInfo->SduLength++;

    framPrepare = CanTp_CopyDataToMsgFromTxBuf(CanTp_txConfig, CanTp_txRuntime,
                                               CanTp_pduInfo, sfActualPayload);
    if (TRUE == framPrepare)
    {
        /* + 1 is because the consecutive frame numbering begins with 1 and not */
        CanTp_txRuntime->framesHandledCount = (uint8)1;
        CanTp_txRuntime->tpState = CANTP_WAITING_FOR_FF_TX_CONFIRMATION;
        CanTp_txRuntime->timeoutCounterValue = CanTp_ConvertMsToMainCycles(CANTP_N_AS);
        (void)CanIf_Transmit(CanTp_txConfig->txPduId, CanTp_pduInfo);
    }
    else
    {
        /* frame error */
        Dcm_TxConfirmation(CanTp_txConfig->txDcmId, NTFRSLT_E_NO_BUFFER);
        CanTp_Init15765RuntimeData(CanTp_txRuntime);
    }
}

/********************************************************************************/
/***
 * @brief			:CanTp_SendConsecutiveFrame
 *
 * Service ID		: <CanTp_SendConsecutiveFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC void CanTp_SendConsecutiveFrame(const CanTp_TxSduType *const CanTp_txConfig,
                                       CanTp_RunTime_DataType *const CanTp_txRuntime,
                                       PduInfoType *const CanTp_pduInfo)
{
    uint8 cfActualPayload;
    boolean lastFrame = FALSE;
    const uint16 txDataRemain = CanTp_txRuntime->transferTotal - CanTp_txRuntime->transferCount;

    CanTp_pduInfo->SduDataPtr[CanTp_pduInfo->SduLength] = (uint8)CANTP_TPCI_CF + CanTp_txRuntime->framesHandledCount;
    CanTp_pduInfo->SduLength++;

    /* Calculate number of valid bytes that reside in this CF */
    if (txDataRemain <= (uint16)(CANTP_STD_ADDR_MAX_CF_LENGTH))
    {
        /* Last frame. */
        cfActualPayload = (uint8)(txDataRemain);
        lastFrame = TRUE;
    }
    else
    {
        cfActualPayload = (uint8)(CANTP_STD_ADDR_MAX_CF_LENGTH);
    }

    /* copy data to can message */
    (void)CanTp_CopyDataToMsgFromTxBuf(CanTp_txConfig, CanTp_txRuntime,
                                       CanTp_pduInfo, cfActualPayload);

    /* change tp state */
    if (TRUE == lastFrame)
    {
        /* this is the last frame of this message */
        CanTp_txRuntime->tpState = CANTP_WAITING_FOR_LAST_CF_TX_CONFIRMATION;
        CanTp_txRuntime->timeoutCounterValue = CanTp_ConvertMsToMainCycles(CANTP_N_AS);
    }
    else
    {
        if (CanTp_txRuntime->bs > (uint16)0)
        {
            CanTp_txRuntime->bs--;
        }
        else
        {
            /* empty */
        }

        if (CanTp_txRuntime->bs > (uint16)0)
        {
            /* for receiver side the block size is zero */
            CanTp_txRuntime->tpState = CANTP_WAITING_FOR_CF_TX_CONFIRMATION;
            CanTp_txRuntime->timeoutCounterValue = CanTp_txRuntime->stmin;
        }
        else
        {
            /* bs=0 */
            CanTp_txRuntime->tpState = CANTP_WAITING_FOR_CF_BLOCK_TX_CONFIRMATION;
            CanTp_txRuntime->timeoutCounterValue =
                CanTp_ConvertMsToMainCycles(CANTP_N_AS);
        }
    }

    CanTp_txRuntime->framesHandledCount = (CanTp_txRuntime->framesHandledCount + (uint8)1) & (uint8)CANTP_SEGMENT_NUMBER_MASK;

    /* send frame */
    (void)CanIf_Transmit(CanTp_txConfig->txPduId, CanTp_pduInfo);
}

/********************************************************************************/
/***
 * @brief			:CanTp_SendTxFrame
 *
 * Service ID		: <CanTp_SendTxFrame>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC void CanTp_SendTxFrame(const CanTp_TxSduType *const CanTp_txConfig,
                              CanTp_RunTime_DataType *const CanTp_txRuntime,
                              const CanTp_Iso15765FrameType CanTp_framType)
{
    uint8 sduData[CANTP_DRV_PDU_MAX_LENGTH];
    PduInfoType CanTp_pduInfo;

    CanTp_pduInfo.SduDataPtr = sduData;
    CanTp_pduInfo.SduLength = (uint16)0;

    switch (CanTp_framType)
    {
    case CANTP_SINGLE_FRAME:
        /* send single frame */
        CanTp_SendSingleFrame(CanTp_txConfig, CanTp_txRuntime, &CanTp_pduInfo);
        break;

    case CANTP_FIRST_FRAME:
        /* send first frame */
        CanTp_SendFirstFrame(CanTp_txConfig, CanTp_txRuntime, &CanTp_pduInfo);
        break;

    case CANTP_CONSECUTIVE_FRAME:
        /* send consecutive frame */
        CanTp_SendConsecutiveFrame(CanTp_txConfig, CanTp_txRuntime, &CanTp_pduInfo);
        break;

    default:
        Dcm_TxConfirmation(CanTp_txConfig->txDcmId, NTFRSLT_E_NOT_OK);
        CanTp_Init15765RuntimeData(CanTp_txRuntime);
        break;
    }

    return;
}

/********************************************************************************/
/***
 * @brief			:CanTp_WaitSForLastCFBufProcess
 *
 * Service ID		: <CanTp_WaitSForLastCFBufProcess>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC void CanTp_WaitSForLastCFBufProcess(const CanTp_RxSduType *const CanTp_rxConfig,
                                           CanTp_RunTime_DataType *const CanTp_runtimeItem)
{
    const BufReq_ReturnType Can_ret = CanTp_CopySegmentToRxBuffer(CanTp_rxConfig, CanTp_runtimeItem);

    if (BUFREQ_OK == Can_ret)
    {
        /* buffer ok */
        Dcm_RxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_OK);
        CanTp_Init15765RuntimeData(CanTp_runtimeItem); /* ok */
    }
    else
    {
        /* buffer not ok */
        Dcm_RxIndication(CanTp_rxConfig->rxDcmId, NTFRSLT_E_NO_BUFFER);
        CanTp_Init15765RuntimeData(CanTp_runtimeItem);
    }
}

/********************************************************************************/
/***
 * @brief			:CanTp_WaitCFBufProcess
 *
 * Service ID		: <CanTp_WaitCFBufProcess>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC void CanTp_WaitCFBufProcess(const CanTp_RxSduType *const CanTp_rxConfig,
                                   CanTp_RunTime_DataType *const CanTp_runtimeItem)
{
    const BufReq_ReturnType Can_ret = CanTp_CopySegmentToRxBuffer(CanTp_rxConfig,
                                                                  CanTp_runtimeItem);

    if (BUFREQ_OK == Can_ret)
    {
        /* sent CTS */
        CanTp_runtimeItem->tpState = CANTP_WAITING_FOR_FC_CTS_TX_CONFIRMATION;
        CanTp_SendFlowControlFrame(CanTp_rxConfig, CanTp_runtimeItem, Can_ret);
    }
    else
    {
        /* send overflow status control */
        CanTp_runtimeItem->tpState = CANTP_WAITING_FOR_FC_OVFLW_TX_CONFIRMATION;
        CanTp_SendFlowControlFrame(CanTp_rxConfig, CanTp_runtimeItem, BUFREQ_E_OVEL);
    }
}

/********************************************************************************/
/***
 * @brief			:CanTp_WaitCFBufProcess
 *
 * Service ID		: <CanTp_WaitCFBufProcess>
 * Sync/Async		: <Synchronous>
 * Reentrancy		: <Reentrant>
 * @param[in]		: <NONE>
 * @param[out]	    : <NONE>
 * @param[in/out]	: <NONE>
 * @return		    : <NONE>
 */
/********************************************************************************/
STATIC void CanTp_WaitFCTxBufProcess(const CanTp_RxSduType *CanTp_rxConfig,
                                     CanTp_RunTime_DataType *CanTp_runtimeItem)
{
    /* sent FC Frame */
    CanTp_runtimeItem->tpState = CANTP_WAITING_FOR_FC_CTS_TX_CONFIRMATION;
    CanTp_SendFlowControlFrame(CanTp_rxConfig, CanTp_runtimeItem, BUFREQ_OK);
}
/***=======[E N D   O F   F I L E]==============================================*/
