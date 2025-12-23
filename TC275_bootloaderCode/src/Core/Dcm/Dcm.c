/*============================================================================*/
/** Copyright (C) 2009-2017, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is 10086 property. Duplication
 *  or disclosure without 10086 written authorization is prohibited.
 *
 *  @file       <Dcm.c>
 *  @brief      <UDS Service - ISO14229>
 *
 *  <This Diagnostic Communication Manager file contained UDS services
 *   which used for bootloader project>
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <10086>
 *  @date       <2013-09-13>
 */
/*============================================================================*/

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *    V1.0    20121109   10086       Initial version
 *
 *    V1.1    20130913   10086        update
 *
 *    V1.2    20160801  10086      update
 *
 *    V1.3    20180511  10086       update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "dcm.h"
#include "dcm_types.h"
#include "dcm_internel.h"
#include "appl.h"
#include "cantp.h"

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/** Dcm communicate status */
typedef struct
{
    /* DCM main rx buffer */
    Dcm_BuffType rxBuff;

    /* DCM main tx buffer */
    Dcm_BuffType txBuff;

    /* DCM current service */
    const Dcm_ServiceTableType *curServicePtr;

    /* if there is a new service request is not processed */
    boolean reqNew;

    /* if there is response is finished */
    boolean respFinished;

    uint16 pendingCount;

    /* DCM P3C timer */
    uint16 p3cTimer;

    /* DCM P2e timer */
    uint16 p2eTimer;

} Dcm_ComType;

/** Session runtime status */
typedef struct
{
    /* DCM current session type */
    Dcm_SessionType curSession;

    /* ECU reset Timer */
    uint16 resetTimer;

    /* if security access is unlocked */
    Dcm_SecurityType securityLevel;

    /* DCM security access timer */
    uint32 securityTimer;

} Dcm_RunTimeType;

/*=======[E X T E R N A L   D A T A]==========================================*/
/** use static memory for service */
uint8 Dcm_MainRxBuff[DCM_RX_BUF_SIZE];
uint8 Dcm_MainTxBuff[DCM_TX_BUF_SIZE];
/* flag of resetting by itself */
boolean dcmDummyDefault = FALSE;

/*=======[I N T E R N A L   D A T A]==========================================*/
/** dcm communication status about service */
STATIC Dcm_ComType dcmComStatus;
/** dcm runtime status about session and security access */
STATIC Dcm_RunTimeType dcmRunTime;

/*=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
STATIC void Dcm_ServiceStart(void);
STATIC void Dcm_ServiceHandle(void);
STATIC void Dcm_ServiceProcess(void);
STATIC void Dcm_ClearRxBuff(const PduLengthType clearLength);
STATIC void Dcm_StopP3cTimer(void);
STATIC void Dcm_P3cTimerCheck(void);
STATIC void Dcm_StartP2eTimer(const uint16 timeout);
STATIC void Dcm_StopP2eTimer(void);
STATIC void Dcm_P2eTimerCheck(void);
STATIC void Dcm_StartTransmit(void);
STATIC void Dcm_SecurityTimerCheck(void);
STATIC void Dcm_StopResetTimer(void);
STATIC void Dcm_ResetTimerCheck(void);
STATIC void Dcm_TimerProc(void);

/*=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/
/******************************************************************************/
/**
 * @brief               <DCM module initialize>
 *
 * <DCM module initialize> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void Dcm_Init(void)
{
    /* set session to default session */
    Dcm_SetSessionMode(DCM_SESSION_DEFAULT);

    /* reset to security access level */
    Dcm_SetSecurityLevel(DCM_SECURITY_LOCKED);

    /* start security access timer */
    Dcm_StartSecurityTimer((uint32)DCM_SECURITY_TIME);

    /* stop ECU reset timer */
    Dcm_StopResetTimer();

    /* reset service process */
    Dcm_ServiceFinish();

    /* initialize DSP */
    Dcm_DspInit();

    return;
}

/******************************************************************************/
/**
 * @brief               <DCM module program initialize>
 *
 * <when program boot request is equal to FL_EXT_PROG_REQUEST_RECEIVED,
 *  this API will be called in Appl_FlStartup function, session is initialized
 *  to programming session,and simulate an 10 03 session control service is
 *  received> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void Dcm_ProgramInit(Dcm_SessionType targetSession)
{
    if ((uint8)DCM_SESSION_DEFAULT == targetSession)
    {
        /* from boot, then set session to prog. session */
        Dcm_SetSessionMode(DCM_SESSION_PROGRAMMING);
    }
    else
    {
        /* from app, then set session to extended session */
        Dcm_SetSessionMode(DCM_SESSION_EXTEND);
    }

    /* simulate receive service 10 */
    dcmComStatus.rxBuff.buffStatus = DCM_BUFF_FOR_TP;
    dcmComStatus.rxBuff.pduId = (uint16)DCM_RX_PHY_PDU_ID;

    /* set received data */
    dcmComStatus.rxBuff.pduInfo.SduDataPtr[0] = (uint8)0x10u;
    dcmComStatus.rxBuff.pduInfo.SduDataPtr[1] = targetSession;
    dcmComStatus.rxBuff.pduInfo.SduLength = (uint8)0x02u;

    Dcm_StartSecurityTimer(0x00u);

    if ((uint8)DCM_SESSION_DEFAULT == targetSession)
    {
        dcmDummyDefault = TRUE;
    }
    else
    {
        /* empty */
    }

    /* start process service 0x10 */
    Dcm_ServiceStart();

    return;
}

/******************************************************************************/
/**
 * @brief               <DCM main task function>
 *
 * <DCM main task function> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void Dcm_MainFunction(void)
{
    /* process P2C, P3C, security timer */
    Dcm_TimerProc();

    /* process service */
    Dcm_ServiceProcess();

    return;
}

/******************************************************************************/
/**
 * @brief               <DCM provide rx buffer for CanTp>
 *
 * <DCM provide rx buffer for CanTp,mean an request is receiving> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <pduId (IN), pduLen (IN)>
 * @param[out]          <sduInfo (OUT)>
 * @param[in/out]       <NONE>
 * @return              <CanTp_BufReq_RetType>
 */
/******************************************************************************/
BufReq_ReturnType Dcm_ProvideRxBuffer(PduIdType pduId,
                                      PduLengthType pduLen,
                                      PduInfoType **sduInfo)
{
    BufReq_ReturnType ret = BUFREQ_OK;

    /* Rx buffer is free state and pduid is correct */
    if ((DCM_BUFF_FREE == dcmComStatus.rxBuff.buffStatus) && (((uint16)DCM_RX_PHY_PDU_ID == pduId) || ((uint16)DCM_RX_FUNC_PDU_ID == pduId)))
    {
        /* Main Buffer should be free */
        if (pduLen <= (uint16)DCM_RX_BUF_SIZE)
        {
            /* allocate MainRxBuff */
            dcmComStatus.rxBuff.buffStatus = DCM_BUFF_FOR_TP;
            dcmComStatus.rxBuff.pduId = pduId;
            dcmComStatus.rxBuff.pduInfo.SduLength = pduLen;
            *sduInfo = &dcmComStatus.rxBuff.pduInfo;
        }
        else
        {
            /* main buffer provided fail */
            ret = BUFREQ_E_OVEL;
        }
    }
    else
    {
        /* No buffer available */
        ret = BUFREQ_E_NOT_OK;
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <DCM rx indication from CanTp>
 *
 * <DCM rx indication from CanTp,mean an request is received> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <pduId (IN), result (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void Dcm_RxIndication(PduIdType pduId, NotifResultType result)
{
    /* check if Rx buffer is provide for CanTp */
    if (DCM_BUFF_FOR_TP == dcmComStatus.rxBuff.buffStatus)
    {
        /* Indication for MainBuff */
        if (((uint8)NTFRSLT_OK == result) && (pduId == dcmComStatus.rxBuff.pduId))
        {
            /* receive Ok */
            Dcm_ServiceStart();
        }
        else
        {
            /* receive failed */
            Dcm_ServiceFinish();
        }
    }
    else
    {
        /* empty */
    }

    return;
}

/******************************************************************************/
/**
 * @brief               <DCM provide tx buffer for CanTp>
 *
 * <DCM provide rx buffer for CanTp,mean a response is sending> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <pduId (IN)>
 * @param[out]          <sduInfo (OUT)>
 * @param[in/out]       <NONE>
 * @return              <CanTp_BufReq_RetType>
 */
/******************************************************************************/
BufReq_ReturnType Dcm_ProvideTxBuffer(PduIdType pduId, PduInfoType **sduInfo)
{
    BufReq_ReturnType ret = BUFREQ_OK;

    /* check if Tx Buffer is provide for service process */
    if ((DCM_BUFF_FOR_SERVICE == dcmComStatus.txBuff.buffStatus) && (pduId == dcmComStatus.txBuff.pduId))
    {
        /* send MainTXBuff */
        *sduInfo = &dcmComStatus.txBuff.pduInfo;
        dcmComStatus.txBuff.buffStatus = DCM_BUFF_FOR_TP;
    }
    else
    {
        Dcm_ServiceFinish();
        ret = BUFREQ_E_NOT_OK;
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <DCM tx confirmation from CanTp>
 *
 * <DCM rx indication from CanTp,mean a response is sended> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <pduId (IN), result (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void Dcm_TxConfirmation(PduIdType pduId, NotifResultType result)
{
    uint8 *pduData;

    /* service response finished */
    if (((uint8)NTFRSLT_OK == result) && (pduId == dcmComStatus.txBuff.pduId) && (FALSE == dcmComStatus.respFinished))
    {
        /*
         ** this case is only appear when send pending message and service is
         ** not finished
         */
        dcmComStatus.txBuff.buffStatus = DCM_BUFF_FREE;
    }
    else
    {
        pduData = &(dcmComStatus.txBuff.pduInfo.SduDataPtr[0]);

        if (((uint8)0x51U == pduData[0]) && ((uint8)0x01U == pduData[1]))
        {
            /* reset response sent, reset the MCU */
            Appl_EcuReset();
        }
        else
        {
            /* empty */
        }

        Dcm_ServiceFinish();
    }

    return;
}

/******************************************************************************/
/**
 * @brief               <set service process finish>
 *
 * <set service process finish> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void Dcm_ServiceFinish(void)
{
    /* set current service */
    dcmComStatus.curServicePtr = NULL_PTR;

    /* initialize tx and rx buffer */
    Dcm_ClearRxBuff((uint16)DCM_RX_BUF_SIZE);
    dcmComStatus.rxBuff.pduInfo.SduDataPtr = Dcm_MainRxBuff;
    dcmComStatus.txBuff.pduInfo.SduDataPtr = Dcm_MainTxBuff;

    /* initialize tx and rx buffer status */
    dcmComStatus.rxBuff.buffStatus = DCM_BUFF_FREE;
    dcmComStatus.txBuff.buffStatus = DCM_BUFF_FREE;

    /* initialize no new service request */
    dcmComStatus.reqNew = FALSE;
    dcmComStatus.respFinished = TRUE;

    /* clear pending count num */
    dcmComStatus.pendingCount = (uint16)0x00u;

    /* stop P2E timer */
    Dcm_StopP2eTimer();

    /* process P3C timer in extended and programming session */
    if ((uint8)DCM_SESSION_DEFAULT == dcmRunTime.curSession)
    {
        Dcm_StopP3cTimer();
    }
    else
    {
        Dcm_StartP3cTimer();
    }

    return;
}

/******************************************************************************/
/**
 * @brief               <clear rx buffer>
 *
 * <clear rx buffer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
STATIC void Dcm_ClearRxBuff(const PduLengthType clearLength)
{
    PduLengthType dataId = (uint16)0;

    for (; dataId < clearLength; dataId++)
    {
        Dcm_MainRxBuff[dataId] = (uint8)0xFFU;
    }

    return;
}

/******************************************************************************/
/**
 * @brief               <set session mode>
 *
 * <set session mode> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <sessMode (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void Dcm_SetSessionMode(const Dcm_SessionType sessMode)
{
    /* set current session */
    dcmRunTime.curSession = sessMode;

    return;
}

/******************************************************************************/
/**
 * @brief               <get session mode>
 *
 * <get session mode> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
Dcm_SessionType Dcm_GetSessionMode(void)
{
    /* get current session */
    return dcmRunTime.curSession;
}

/******************************************************************************/
/**
 * @brief               <check if session mode is support>
 *
 * <check if session mode is support> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <sessionTable (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <boolean>
 */
/******************************************************************************/
boolean Dcm_CheckSessionSupp(const Dcm_SessionType sessionSupportMask)
{
    boolean ret = TRUE;

    if ((uint8)0 == (uint8)(dcmRunTime.curSession & sessionSupportMask))
    {
        ret = FALSE;
    }
    else
    {
        /* empty */
    }

    return ret;
}

boolean Dcm_CheckSessionSuppMask(const Dcm_SessionType sessionSupportMask)
{
    boolean ret = TRUE;

    if ((uint8)3 != (uint8)(dcmRunTime.curSession & sessionSupportMask))
    {
        Dcm_SendNcr(DCM_E_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION);
        ret = FALSE;
    }
    else
    {
        /* empty */
    }

    return ret;
}

boolean Dcm_CheckSessionSuppMask_2E(const Dcm_SessionType sessionSupportMask)
{
    boolean ret = TRUE;

    if ((uint8)2 != (uint8)(dcmRunTime.curSession & sessionSupportMask))
    {
        Dcm_SendNcr(DCM_E_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION);
        ret = FALSE;
    }
    else
    {
        /* empty */
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <set security level>
 *
 * <set security level> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <secLev (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void Dcm_SetSecurityLevel(const Dcm_SecurityType secLev)
{
    /* set current security level */
    dcmRunTime.securityLevel = secLev;

    return;
}

/******************************************************************************/
/**
 * @brief               <get security level>
 *
 * <get security level> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <Dcm_SecurityType>
 */
/******************************************************************************/
Dcm_SecurityType Dcm_GetSecurityLevel(void)
{
    /* get current security level */
    return dcmRunTime.securityLevel;
}

/******************************************************************************/
/**
 * @brief               <check if security timer is expired>
 *
 * <check if security timer is expired> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <boolean>
 */
/******************************************************************************/
boolean Dcm_GetSecurityTimerExpired(void)
{
    boolean ret = TRUE;
#if 0
    /* check if security timer expired */
    if (dcmRunTime.securityTimer > 0UL)
    {
        ret = FALSE;
    }
    else
    {
        /* empty */
    }
#endif // 180904
    return ret;
}

/******************************************************************************/
/**
 * @brief               <check if security level if supportted>
 *
 * <check if security level if supportted> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <secLevTable (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <boolean>
 */
/******************************************************************************/
boolean Dcm_CheckSecuritySupp(const Dcm_SecurityType securitySupportMask)
{
    boolean ret = TRUE;

    if ((uint8)0 == (uint8)(dcmRunTime.securityLevel & securitySupportMask))
    {
        ret = FALSE;
    }
    else
    {
        /* empty */
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <start security timer>
 *
 * <start security timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <timeOut (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void Dcm_StartSecurityTimer(uint32 timeOut)
{
    /* set to config value */
    dcmRunTime.securityTimer = (timeOut / (uint32)DCM_MAIN_TICK);

    return;
}

/******************************************************************************/
/**
 * @brief               <start reset timer>
 *
 * <start reset timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <timeOut (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void Dcm_StartResetTimer(uint16 timeOut)
{
    /* set reset timer value */
    dcmRunTime.resetTimer = (uint16)(timeOut / (uint16)DCM_MAIN_TICK);

    return;
}

/******************************************************************************/
/**
 * @brief               <send response>
 *
 * <send response> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void Dcm_SendRsp(void)
{
    /* service process is finished */
    dcmComStatus.respFinished = TRUE;

    /* transmit message */
    Dcm_StartTransmit();

    Dcm_ClearRxBuff(dcmComStatus.rxBuff.pduInfo.SduLength);

    return;
}

/******************************************************************************/
/**
 * @brief               <transmit negative response>
 *
 * <transmit negative response> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <nrcCode (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void Dcm_SendNcr(uint8 nrcCode)
{
    uint8 *const pduData = &(dcmComStatus.txBuff.pduInfo.SduDataPtr[0]);

    /* check if send pending */
    if (nrcCode != (uint8)DCM_E_PENDING)
    {
        /* if send NRC, service process is finished */
        dcmComStatus.respFinished = TRUE;
    }
    else
    {
        /* if send pending, increase count */
        dcmComStatus.pendingCount++;
    }

    pduData[0] = (uint8)DCM_RSP_SID_NRC;
    pduData[1] = dcmComStatus.rxBuff.pduInfo.SduDataPtr[0];
    pduData[2] = nrcCode;
    dcmComStatus.txBuff.pduInfo.SduLength = (uint8)0x03u;

    Dcm_StartTransmit();

    if (TRUE == dcmComStatus.respFinished)
    {
        Dcm_ClearRxBuff(dcmComStatus.rxBuff.pduInfo.SduLength);
    }
    else
    {
        /* empty */
    }

    return;
}

/******************************************************************************/
/**
 * @brief               <start service process when received message>
 *
 * <start service process when received message> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
STATIC void Dcm_ServiceStart(void)
{
    /* Rx buffer in service status */
    dcmComStatus.rxBuff.buffStatus = DCM_BUFF_FOR_SERVICE;

    /* start P2E timer */
    Dcm_StartP2eTimer((uint16)DCM_P2MAX_TIME);

    /* stop P3C timer */
    Dcm_StopP3cTimer();

    /* a new service request is received */
    dcmComStatus.reqNew = TRUE;

    dcmComStatus.respFinished = FALSE;

    return;
}

/******************************************************************************/
/**
 * @brief               <handle requested service>
 *
 * <handle requested service> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
uint8 TestFlag = FALSE;
STATIC void Dcm_ServiceHandle(void)
{
    boolean SIDFind = FALSE;
    uint8 tableIndex = (uint8)DCM_SERVICE_NUM;
    boolean sesSupp;

    /* record current service table */
    dcmComStatus.curServicePtr = Dcm_ServiceTable;

    /* find service table */
    while ((tableIndex > (uint8)0) && (FALSE == SIDFind))
    {
        if ((uint16)DCM_RX_FUNC_PDU_ID == dcmComStatus.rxBuff.pduId)
        {
            TestFlag = TRUE;
        }

        tableIndex--;
        if (dcmComStatus.curServicePtr->SID == dcmComStatus.rxBuff.pduInfo.SduDataPtr[0])
        {
            if (((uint16)DCM_RX_PHY_PDU_ID == dcmComStatus.rxBuff.pduId) && ((dcmComStatus.curServicePtr->addressSupportMask & (uint8)DCM_PHYSICAL_ADDRESSING) != (uint8)0))
            {
                SIDFind = TRUE;
            }
            else
            {
                /* empty */
            }
            if (((uint16)DCM_RX_FUNC_PDU_ID == dcmComStatus.rxBuff.pduId) && ((dcmComStatus.curServicePtr->addressSupportMask & (uint8)DCM_FUNCTIONAL_ADDRESSING) != (uint8)0))
            {
                SIDFind = TRUE;
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
        if (FALSE == SIDFind)
        {
            dcmComStatus.curServicePtr++;
        }
        else
        {
            /* empty */
        }
    }

    /* check if service table is found */
    if (TRUE == SIDFind)
    {
        /* check if service is supported in current session */
        sesSupp = Dcm_CheckSessionSupp(dcmComStatus.curServicePtr->sessionSupportMask);
        if (TRUE == sesSupp)
        {

            /* execute service process */
            dcmComStatus.curServicePtr->serviceFct(&dcmComStatus.rxBuff,
                                                   &dcmComStatus.txBuff);
        }
        else
        {
            /* service is not supported in active session */
            Dcm_SendNcr(DCM_E_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION);
        }
    }
    else
    {
        /* has not find service table */
        if ((uint16)DCM_RX_FUNC_PDU_ID == dcmComStatus.rxBuff.pduId)
        {
            /* if received PDU ID is function address, reset service process */
            Dcm_ServiceFinish();
        }

        else
        {
            /* if received PDU ID is physical address, service is not supported */
            Dcm_SendNcr(DCM_E_SERVICE_NOT_SUPPORTED);
        }
    }

    return;
}

/******************************************************************************/
/**
 * @brief               <process new requested service>
 *
 * <process new requested service> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
STATIC void Dcm_ServiceProcess(void)
{
    FL_ResultType errorCode;
    boolean serviceFinished;

    /* check if tx buffer is free */
    if (DCM_BUFF_FREE == dcmComStatus.txBuff.buffStatus)
    {
        /* check if there if a new request service */
        if (TRUE == dcmComStatus.reqNew)
        {
            dcmComStatus.reqNew = FALSE;

            Dcm_ServiceHandle();
        }
        else
        {
            /* check service process for pending */
            if (FALSE == dcmComStatus.respFinished)
            {
                serviceFinished = FL_ServiceFinished(&errorCode);

                /* check if service busy is finished, then can send response. */
                if ((TRUE == serviceFinished) && (dcmComStatus.curServicePtr->pendingFct != NULL_PTR))
                {
                    dcmComStatus.curServicePtr->pendingFct(errorCode,
                                                           &dcmComStatus.rxBuff,
                                                           &dcmComStatus.txBuff);
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
        }
    }
    else
    {
        /* empty */
    }

    return;
}

/******************************************************************************/
/**
 * @brief               <start P3C timer by config value>
 *
 * <start P3C timer by config value> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void Dcm_StartP3cTimer(void)
{
    dcmComStatus.p3cTimer = (uint16)(DCM_P3MAX_TIME / DCM_MAIN_TICK);

    return;
}

/******************************************************************************/
/**
 * @brief               <stop P3C timer>
 *
 * <stop P3C timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
STATIC void Dcm_StopP3cTimer(void)
{
    dcmComStatus.p3cTimer = (uint16)0;

    return;
}

/******************************************************************************/
/**
 * @brief               <process P3C timer>
 *
 * <process P3C timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
STATIC void Dcm_P3cTimerCheck(void)
{
    if (dcmComStatus.p3cTimer > (uint16)0)
    {
        dcmComStatus.p3cTimer--;

        if ((uint16)0 == dcmComStatus.p3cTimer)
        {
            /* if P3C timeout, set ECU reset to default session */
            Appl_EcuReset();
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

/******************************************************************************/
/**
 * @brief               <start P2E timer by given value>
 *
 * <start P2E timer by given value> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <timeout (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
STATIC void Dcm_StartP2eTimer(const uint16 timeout)
{
    dcmComStatus.p2eTimer = (uint16)(timeout / (uint16)DCM_MAIN_TICK);

    return;
}

/******************************************************************************/
/**
 * @brief               <stop P2E timer>
 *
 * <stop P2E timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
STATIC void Dcm_StopP2eTimer(void)
{
    dcmComStatus.p2eTimer = (uint16)0;

    return;
}

/******************************************************************************/
/**
 * @brief               <process P2E timer>
 *
 * <process P2E timer,and send pending> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
STATIC void Dcm_P2eTimerCheck(void)
{
    if (dcmComStatus.p2eTimer > (uint16)0)
    {
        dcmComStatus.p2eTimer--;
        if ((uint16)0 == dcmComStatus.p2eTimer)
        {
            /* start P2E timer to 2000ms timeout */
            Dcm_StartP2eTimer((uint16)DCM_P2SMAX_TIME);

            if (DCM_BUFF_FREE == dcmComStatus.txBuff.buffStatus)
            {

                if ((1 == FunctService255) || (2 == FunctService255))
                {
                    if (dcmComStatus.pendingCount < 5)
                    {
                        Dcm_SendNcr(DCM_E_PENDING);
                    }
                    else
                    {
                        Dcm_SendNcr(DCM_E_GENERAL_REJECT);
                    }
                }
                else
                {
                    if (dcmComStatus.pendingCount < (uint16)DCM_PENDING_ATTEMPT_NUM)
                    {
                        if (1 != Fl_GetActiveJob())
                        {
                            Dcm_SendNcr(DCM_E_PENDING);
                        }
                    }
                    else
                    {
                        Dcm_SendNcr(DCM_E_GENERAL_REJECT);
                    }
                }
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
    }
    else
    {
        /* empty */
    }

    return;
}

/******************************************************************************/
/**
 * @brief               <transmit response>
 *
 * <transmit response, but not include pending> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
STATIC void Dcm_StartTransmit(void)
{
    /* set Dcm status */
    dcmComStatus.txBuff.buffStatus = DCM_BUFF_FOR_SERVICE;

    /* set PduId */
    dcmComStatus.txBuff.pduId = (uint16)DCM_TX_PDU_ID;

    /* call function of CanTp layer */
    CanTp_Transmit(dcmComStatus.txBuff.pduId, &(dcmComStatus.txBuff.pduInfo));

    return;
}

/******************************************************************************/
/**
 * @brief               <process security timer>
 *
 * <process security timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
STATIC void Dcm_SecurityTimerCheck(void)
{
    if (dcmRunTime.securityTimer > 0UL)
    {
        dcmRunTime.securityTimer--;
    }
    else
    {
        /* empty */
    }

    return;
}

/******************************************************************************/
/**
 * @brief               <stop reset timer>
 *
 * <stop reset timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
STATIC void Dcm_StopResetTimer(void)
{
    /* clear reset timer value */
    dcmRunTime.resetTimer = (uint16)0x00u;

    return;
}

/******************************************************************************/
/**
 * @brief               <process reset timer>
 *
 * <process reset timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
STATIC void Dcm_ResetTimerCheck(void)
{
    if (dcmRunTime.resetTimer > (uint16)0)
    {
        dcmRunTime.resetTimer--;

        /* check if reset timer is timeout */
        if ((uint16)0 == dcmRunTime.resetTimer)
        {
            /* ECU reset */
            Appl_EcuReset();
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

/******************************************************************************/
/**
 * @brief               <process session timer P2E,P3C,reset ,security timer>
 *
 * <process session timer P2E,P3C,reset,security timer> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
STATIC void Dcm_TimerProc(void)
{
    /* process security timer */
    Dcm_SecurityTimerCheck();

    /* process P3C timer */
    Dcm_P3cTimerCheck();

    /* process P2E timer */
    Dcm_P2eTimerCheck();

    /* process ECU reset timer */
    Dcm_ResetTimerCheck();

    return;
}

void Dcm_ForcePending(void)
{
    Dcm_StartP2eTimer(10);
}

void PosResponse255(void)
{
    uint8 *pduData = (uint8 *)&dcmComStatus.txBuff.pduInfo.SduDataPtr[0];

    pduData[0] = 0x68u;
    pduData[1] = 0x03u;

    dcmComStatus.txBuff.pduInfo.SduLength = 0x02u;

    Dcm_SendRsp();

    return;
}
/*=======[E N D   O F   F I L E]==============================================*/

