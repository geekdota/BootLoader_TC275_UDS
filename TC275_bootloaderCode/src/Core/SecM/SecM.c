/*============================================================================*/
/** Copyright (C) 2009-2018, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is 10086 property. Duplication
 *  or disclosure without 10086 written authorization is prohibited.
 *
 *  @file       <SecM.c>
 *  @brief      <Security set>
 *
 *              <seed and key generate for UDS>
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <ccl>
 *  @date       <2013-09-13>
 */
/*============================================================================*/

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121109    Gary       Initial version
 *
 *  V1.1    20130517    liuyp      modify the generate key algorithm
 *
 *  V1.2    20130913    ccl        update
 *
 *  V1.3    20180511    CChen      update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "secm.h"
#include "cal.h"
#include "appl.h"
#include "stdlib.h"

/*=======[E X T E R N A L   D A T A]==========================================*/
/** CRC buffer */
uint8 SecM_CrcDataBuffer[SECM_CRC_BUFFER_LEN];

/*=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
STATIC SecM_StatusType SecM_ProcessCrc(SecM_CRCParamType *crcParam,
                                       uint32 Address,
                                       uint32 Length);

/*=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/
/******************************************************************************/
/**
 * @brief               <generate seed for UDS>
 *
 * <Needed by the UDS module,generate seed> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <seed (OUT)>
 * @param[in/out]       <NONE>
 * @return              <SecM_StatusType>
 */
/******************************************************************************/
SecM_StatusType SecM_GenerateSeed(SecM_SeedType *seed)
{
    uint32 secM_Seed;

    /* random seed */
    srand(FL_NvmInfo.blockInfo[0].blkProgAttempt);
    secM_Seed = rand();
    secM_Seed = ((secM_Seed << 16) + rand());

    /* seed is non zero */
    if (0UL == secM_Seed)
    {
        secM_Seed++;
    }

    *seed = secM_Seed;

    return SECM_OK;
}

/******************************************************************************/
/**
 * @brief               <compute key>
 *
 * <compute key before compute key> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <seed (IN), k (IN)>
 * @param[out]          <key (OUT)>
 * @param[in/out]       <NONE>
 * @return              <SecM_StatusType>
 */
/******************************************************************************/
SecM_StatusType SecM_ComputeKey(SecM_SeedType seed,
                                SecM_WordType mask,
                                SecM_KeyType *key)
{
    uint32 seed2;
    uint32 key1;
    uint32 key2;

    /* calculate seed2 */
    seed2 = ((seed << (uint8)16) & 0xFFFF0000U) + ((seed >> (uint8)16) & 0xFFFFU);

    /* calculate key1 */
    key1 = seed ^ mask;

    /* calculate key2 */
    key2 = seed2 ^ mask;

    /* calculate key */
    *key = (key1 + key2);

    return SECM_OK;
}

/******************************************************************************/
/**
 * @brief               <compare key>
 *
 * <Needed by the UDS module,compare seed> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <key (IN), seed (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <SecM_StatusType>
 */
/******************************************************************************/
SecM_StatusType SecM_CompareKey(SecM_KeyType key, SecM_SeedType seed)
{
    SecM_StatusType ret = SECM_OK;
    SecM_KeyType getkey;

    ret = SecM_ComputeKey(seed, SECM_ECU_KEY, &getkey);

    if (getkey != key)
    {
        // ret = SECM_NOT_OK; /*10086 mask SeedKey veritfy function*/
    }
    else
    {
        /*empty */
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <compute CRC>
 *
 * <process CRC compute,include init,compute and finish> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <crcParam (IN/OUT)>
 * @return              <SecM_StatusType>
 */
/******************************************************************************/
SecM_StatusType SecM_ComputeCRC(SecM_CRCParamType *crcParam)
{
    SecM_StatusType ret = SECM_OK;

    switch (crcParam->crcState)
    {
    case SECM_CRC_INIT:
        /* CRC value initialize */
        Cal_CrcInit(&crcParam->currentCRC);
        break;

    case SECM_CRC_COMPUTE:
        /* CRC value compute */
        Cal_CrcCal(&crcParam->currentCRC,
                   crcParam->crcSourceBuffer,
                   (uint32)crcParam->crcByteCount);
        break;

    case SECM_CRC_FINALIZE:
        /* CRC value finish */
        Cal_CrcFinalize(&crcParam->currentCRC);
        break;

    default:
        ret = SECM_NOT_OK;
        break;
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <verificate CRC value>
 *
 * <verificate if transfered CRC is equal to computed CRC> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <verifyParam (IN/OUT)>
 * @return              <SecM_StatusType>
 */
/******************************************************************************/
SecM_StatusType SecM_Verification(SecM_VerifyParamType *verifyParam)
{
    SecM_StatusType ret = SECM_OK;
    SecM_CRCParamType crcParam;
    uint8 segmentIndex = (uint8)0;
    SecM_CRCType transferedCrc;

    if (NULL_PTR == verifyParam->segmentList)
    {
        /* if segment list is NULL, verification is failed */
        ret = SECM_NOT_OK;
    }
    else
    {
        /* initialize CRC */
        crcParam.currentCRC = 0UL;
        crcParam.crcState = (uint8)SECM_CRC_INIT;
        ret = SecM_ComputeCRC(&crcParam);
    }

    for (segmentIndex = (uint8)0;
         (segmentIndex < verifyParam->segmentList->nrOfSegments) && (SECM_OK == ret);
         segmentIndex++)
    {
        /* compute each segment CRC */
        ret = SecM_ProcessCrc(&crcParam,
                              verifyParam->segmentList->segmentInfo[segmentIndex].address,
                              verifyParam->segmentList->segmentInfo[segmentIndex].length);
    }

    if (SECM_OK == ret)
    {
        /* finish compute CRC */
        crcParam.crcState = (uint8)SECM_CRC_FINALIZE;
        ret = SecM_ComputeCRC(&crcParam);
    }
    else
    {
        /* empty */
    }

    if (SECM_OK == ret)
    {
        /* get CRC transfered from client */
#if (CAL_CRC32 == CAL_METHOD)
        transferedCrc = (((SecM_CRCType)verifyParam->verificationData[0]) << (uint8)24);
        transferedCrc += (((SecM_CRCType)verifyParam->verificationData[1]) << (uint8)16);
        transferedCrc += (((SecM_CRCType)verifyParam->verificationData[2]) << (uint8)8);
        transferedCrc += ((SecM_CRCType)verifyParam->verificationData[3]);
#else
        /* CRC16 */
        transferedCrc = ((SecM_CRCType)verifyParam->verificationData[0] << (uint8)8);
        transferedCrc += (SecM_CRCType)verifyParam->verificationData[1];
#endif

        /* compare CRC */
        if (transferedCrc != crcParam.currentCRC)
        {
            // ret = SECM_NOT_OK; /* 10086 do */
        }
        else
        {
            /* empty */
        }

        /* save CRC */
        verifyParam->crcTotle = crcParam.currentCRC;
    }
    else
    {
        /* empty */
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <process CRC compute>
 *
 * <CRC compute> .
 * @param[in]           <Address (IN), Length (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <crcParam (IN/OUT)>
 * @return              <SecM_StatusType>
 */
/******************************************************************************/
STATIC SecM_StatusType SecM_ProcessCrc(SecM_CRCParamType *crcParam,
                                       uint32 Address, uint32 Length)
{
    SecM_StatusType ret = SECM_OK;
    uint32 readLength = 0UL;

    /* set CRC compute step */
    crcParam->crcState = (uint8)SECM_CRC_COMPUTE;

    while ((Length > 0UL) && (SECM_OK == ret))
    {
        /* read maximum length is SECM_CRC_BUFFER_LEN */
        if (Length > (uint32)SECM_CRC_BUFFER_LEN)
        {
            readLength = SECM_CRC_BUFFER_LEN;
        }
        else
        {
            readLength = Length;
        }

        /* get source data from memory */
        crcParam->crcByteCount = (uint16)FL_ReadMemory(Address,
                                                       readLength,
                                                       SecM_CrcDataBuffer);
        Length -= readLength;
        Address += readLength;
        crcParam->crcSourceBuffer = SecM_CrcDataBuffer;

        /* update watch dog */
        Appl_UpdateTriggerCondition();

        /* compute CRC */
        ret = SecM_ComputeCRC(crcParam);
    }

    return ret;
}

/*=======[E N D   O F   F I L E]==============================================*/
