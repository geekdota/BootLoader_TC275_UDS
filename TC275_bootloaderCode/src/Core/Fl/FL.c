/*============================================================================*/
/** Copyright (C) 2009-2018, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is 10086 property. Duplication
 *  or disclosure without 10086 written authorization is prohibited.
 *
 *  @file       <FL.c>
 *  @brief      <His Flash Loader >
 *
 *  < The Code process checksum,erase and program for bootloader project>
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <10086>
 *  @date       <2016-10-25>
 */
/*============================================================================*/

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *     V1.0   20121227   10086       Initial version
 *
 *     V1.1   20160801   10086        update
 *
 *     V1.2   20180511   10086         update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "fl.h"
#include "fls.h"
#include "secm.h"
#include "appl.h"
#include "dcm_internel.h"
#include "cantp.h"
#include "canif.h"

/*=======[M A C R O S]========================================================*/
/* macro of ECU Hardware Number length */
#define HW_VER_LENGTH 16
/* macro of Boot Software Identification length */
#define SW_REFNUM_LENGTH 10
/* the length of programming counter */
#define PROG_COUNTER_LENGTH 1
/* the length of programming attempt counter */
#define PROG_ATTEMPT_LENGTH 1

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/** flashloader job status */
typedef enum
{
    FL_JOB_IDLE,

    FL_JOB_ERASING,

    FL_JOB_PROGRAMMING,

    FL_JOB_CHECKING

} FL_ActiveJobType;

/** flashloader download step */
typedef enum
{
    FL_REQUEST_STEP,

    FL_TRANSFER_STEP,

    FL_EXIT_TRANSFER_STEP,

    FL_CHECKSUM_STEP

} FL_DownloadStepType;

/** flashloader status information */
typedef struct
{
    /* flag if finger print has written to NVM */
    boolean fingerPrintWrittenFlag;
    /* repair shop code buffer */
    /* flag if finger print has written */
    boolean fingerPrintWritten;

    /* flag if finger print buffer */
    uint8 fingerPrint[FL_FINGER_PRINT_LENGTH];

    /* flag if flash driver has downloaded */
    boolean flDrvDownloaded;

    /* error code for flash active job */
    FL_ResultType errorCode;

    /* flag if current block is erased */
    boolean blockErased;

    /* current process block index */
    uint8 blockIndex;

    /* current process start address */
    uint32 startAddr;

    /* current process length */
    uint32 downLength;

    /* current process buffer point, point to buffer supplied from DCM */
    const uint8 *dataBuff;

    /* segment list of current process block */
    FL_SegmentListType segmentList;

    /* flashloader download step */
    FL_DownloadStepType downloadStep;

    /* current job status */
    FL_ActiveJobType activeJob;

} FL_DownloadStateType;

/* handle the two segments in one page */
typedef struct
{
    /* current process start address */
    uint32 remainAddr;

    /* current process length */
    uint32 remainLength;

} FL_RemainDataType;

/*=======[E X T E R N A L   D A T A]==========================================*/
/** NVM information which include bootloader information */
FL_NvmInfoType FL_NvmInfo;

#define BL_NvmInf 1
#if BL_NvmInf
#pragma section ".BL_NvmInf" aw
const uint8 BL_Information[FL_NUM_LOGICAL_BLOCKS * 20 + 4] = {0x00};
#pragma section
#endif

/*=======[I N T E R N A L   D A T A]==========================================*/
/* save the data which not aligned */
STATIC FL_RemainDataType FL_RemainDataStruct =
    {
        0UL, 0UL};
/** flashloader status information */
STATIC FL_DownloadStateType FldownloadStatus;
/** flashloader program buffer */
STATIC uint8 FlProgramData[FL_PROGRAM_SIZE];
/** flashloader program length */
STATIC uint32 FlProgramLength = 0UL;
/** flash driver API input parameter */
STATIC tFlashParam flashParamInfo =
    {
        (uint8)FLASH_DRIVER_VERSION_PATCH,
        (uint8)FLASH_DRIVER_VERSION_MINOR,
        (uint8)FLASH_DRIVER_VERSION_MAJOR,
        (uint8)0x00u,
        (uint8)kFlashOk,
        (uint16)0x0000U,
        0UL,
        0UL,
        NULL_PTR,
        &Appl_UpdateTriggerCondition};

/*=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
STATIC FL_ResultType FL_Erasing(void);
STATIC FL_ResultType FL_CheckDownloadSegment(void);
STATIC FL_ResultType FL_DownloadRemainData(void);
STATIC FL_ResultType FL_HandleRemainData(void);
STATIC FL_ResultType FL_ProgrammingData(void);
STATIC FL_ResultType FL_Programming(void);
#if (STD_ON == FL_USE_GAP_FILL)
STATIC FL_ResultType FL_FillGap(void);
#endif
STATIC FL_ResultType FL_CheckSuming(void);
STATIC FL_ResultType FL_UpdateNvm(void);

/*=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/

/******************************************************************************/
/**
 * @brief               <flashloader module initialize>
 *
 * <initialize download status> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void FL_InitState(void)
{
#if BL_NvmInf
    uint8 i = 0;
    if ((0 == i) && (0 == BL_Information[0]))
    {
        i = 1;
    }
#endif

    /* finger print is not written to NVM*/
    FldownloadStatus.fingerPrintWrittenFlag = FALSE;
    /* finger print is not written */
    FldownloadStatus.fingerPrintWritten = FALSE;

    /* flash driver is not downloaded */
    FldownloadStatus.flDrvDownloaded = FALSE;

    /* current block is not erased */
    FldownloadStatus.blockErased = FALSE;

    /* download step is download request 0x34 */
    FldownloadStatus.downloadStep = FL_REQUEST_STEP;

    /* current active job is idle */
    FldownloadStatus.activeJob = FL_JOB_IDLE;

    FL_RemainDataStruct.remainLength = 0UL;

    return;
}

/******************************************************************************/
/**
 * @brief               <read data by identifier for 0x22>
 *
 * <read data by identifier for 0x22> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <readData (OUT)>
 * @param[in/out]       <NONE>
 * @return              <uint16>
 */
/******************************************************************************/
uint16 FL_ReadProgCounter(uint8 *readData)
{
    /* read programming counter from flash */
    *readData = FlashReadByte(0xA000C002U);

    return (uint16)PROG_COUNTER_LENGTH;
}

/******************************************************************************/
/**
 * @brief               <read data by identifier for 0x22>
 *
 * <read data by identifier for 0x22> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <readData (OUT)>
 * @param[in/out]       <NONE>
 * @return              <uint16>
 */
/******************************************************************************/
uint16 FL_ReadProgAttemptCounter(uint8 *readData)
{
    /* read programming attempt counter from flash */
    *readData = FlashReadByte(0xA000C001U);

    return (uint16)PROG_ATTEMPT_LENGTH;
}

/******************************************************************************/
/**
 * @brief               <read data by identifier for 0x22>
 *
 * <read data by identifier for 0x22> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <readData (OUT)>
 * @param[in/out]       <NONE>
 * @return              <uint16>
 */
/******************************************************************************/
uint16 FL_ReadBootSWReferenceNumber(uint8 *readData)
{
    uint8 dataIndex;

    /* Boot Software Identification, ASCII code */
    const uint8 SWRefNum[SW_REFNUM_LENGTH] =
        {
            (uint8)0x41u, (uint8)0x41u, (uint8)0x41u, (uint8)0x20u, (uint8)0x18u,
            (uint8)0x05u, (uint8)0x16u, (uint8)0x30u, (uint8)0x30u, (uint8)0x30u};

    for (dataIndex = (uint8)0; dataIndex < (uint8)SW_REFNUM_LENGTH; dataIndex++)
    {
        *readData = SWRefNum[dataIndex];
        readData++;
    }

    return (uint16)SW_REFNUM_LENGTH;
}
/******************************************************************************/
/**
 * @brief               <read data by identifier for 0x22>
 *
 * <read data by identifier for 0x22> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <readData (OUT)>
 * @param[in/out]       <NONE>
 * @return              <uint16>
 */
/******************************************************************************/
uint16 FL_ReadFingerPrint(uint8 *readData)
{
    uint8 length;

    if (TRUE == FldownloadStatus.fingerPrintWrittenFlag)
    {
        /* get fingerprint from NVM*/
        for (length = 0; length < FL_FINGER_PRINT_LENGTH; length++)
        {
            FL_NvmInfo.blockInfo[0].fingerPrint[length] = *(uint8 *)(FL_NVM_INFO_ADDRESS + FL_FINGER_PRINT_OFFSET + length);

            *readData = (uint8)FL_NvmInfo.blockInfo[0].fingerPrint[length];
            readData++;
        }
    }
    else
    {
        /* get fingerprint from RAM*/
        for (length = 0; length < FL_FINGER_PRINT_LENGTH; length++)
        {
            *readData = FldownloadStatus.fingerPrint[length];
            readData++;
        }
    }

    return FL_FINGER_PRINT_LENGTH;
}

/******************************************************************************/
/**
 * @brief               <read data by identifier for 0x22>
 *
 * <read data by identifier for 0x22> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <readData (OUT)>
 * @param[in/out]       <NONE>
 * @return              <uint16>
 */
/******************************************************************************/
uint16 FL_ReadSysECUHWVer(uint8 *readData)
{
    uint8 dataIndex;

    /* System Supplier ECU Hardware Version Number, ASCII code */
    const uint8 HWVer[HW_VER_LENGTH] =
        {
            (uint8)0x01u, (uint8)0x02u, (uint8)0x03u, (uint8)0x04u, (uint8)0x05u,
            (uint8)0x06u, (uint8)0x07u, (uint8)0x08u, (uint8)0x09u, (uint8)0x0Au,
            (uint8)0x0Bu, (uint8)0x0Cu, (uint8)0x0Du, (uint8)0x0Eu, (uint8)0x0Fu,
            (uint8)0x10u};

    for (dataIndex = (uint8)0; dataIndex < (uint8)HW_VER_LENGTH; dataIndex++)
    {
        *readData = HWVer[dataIndex];
        readData++;
    }

    return (uint16)HW_VER_LENGTH;
}

/******************************************************************************/
/**
 * @brief               <read memory for checksum>
 *
 * <Needed by the security module to read only those memory areas that are in
 *  the scope of the flashloader (e.g. not erase/write protected, not RAM, ...)> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <address (IN), length (IN)>
 * @param[out]          <data (OUT)>
 * @param[in/out]       <NONE>
 * @return              <uint32>
 */
/******************************************************************************/
uint32 FL_ReadMemory(uint32 address, uint32 length, uint8 *data)
{
    uint32 readLength = 0UL;
    uint8 curBlockIndex = (uint8)0;

    /* read data from flash block */
    for (curBlockIndex = (uint8)0; curBlockIndex < FL_NUM_LOGICAL_BLOCKS; curBlockIndex++)
    {
        /* check if address is in range of logical blocks */
        if ((address >= FL_BlkInfo[curBlockIndex].address) && (address < (FL_BlkInfo[curBlockIndex].address + FL_BlkInfo[curBlockIndex].length)))
        {
            FlashReadMemory(data, address, length);
            readLength = length;
        }
        else
        {
            /* empty */
        }
    }

    /* read data from RAM of flash driver */
    if ((address >= FL_DEV_BASE_ADDRESS) && (address < (FL_DEV_BASE_ADDRESS + FL_DEV_SIZE)))
    {
        while ((length > 0UL) && (address < (FL_DEV_BASE_ADDRESS + FL_DEV_SIZE)))
        {
            /* read data from local address in RAM, direct read and write */
            *data = *(uint8 *)address;
            data++;
            address++;
            length--;
            readLength++;
        }
    }
    else
    {
        /* empty */
    }

    return readLength;
}

/******************************************************************************/
/**
 * @brief               <0x31 check program pre-condition>
 *
 * <0x31 check program pre-condition .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <conditions (OUT)>
 * @param[in/out]       <NONE>
 * @return              <uint8>
 */
/******************************************************************************/
uint8 FL_CheckProgPreCondition(uint8 *conditions)
{
    /* defined by user */
    *conditions = (uint8)0x00u;

    return (uint8)0x00u;
}

/******************************************************************************/
/**
 * @brief               <0x2E service write repair shop code>
 *
 * <save RepairShopCode to internal buffer.>
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <data (IN), length (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
FL_ResultType FL_WriteRepairShopCode(const uint8 *data, const uint16 length)
{
    FL_ResultType ret = (uint8)FL_OK;

    /* check if FL step is correct */
    if (FldownloadStatus.downloadStep != FL_REQUEST_STEP)
    {
        ret = (uint8)FL_ERR_SEQUENCE;
    }
    else
    {
        /* check if repair shop code length is correct */
        if ((uint16)FL_REPAIR_SHOP_CODE_LENGTH == length)
        {
            /* save the repair shop code info */
            FldownloadStatus.errorCode = (uint8)FL_OK;

            /* set the download state of the repair shop code */
            ret = (uint8)FL_OK;
        }
        else
        {
            ret = (uint8)FL_FAILED;
        }
    }

    if (data != 0) /* 10086 do */
    {
        /* do nothing */
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <0x2E service write finger print>
 *
 * <save finger print to internal buffer.>
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <data (IN), length (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
FL_ResultType FL_WriteFingerPrint(const uint8 *data, const uint16 length)
{
    FL_ResultType ret = (uint8)FL_OK;

    /* check if FL step is correct */
    if (FldownloadStatus.downloadStep != FL_REQUEST_STEP)
    {
        ret = (uint8)FL_ERR_SEQUENCE;
    }
    else
    {
        /* check if finger print length is correct */
        if ((uint16)FL_FINGER_PRINT_LENGTH == length)
        {
            /* save the finger print info */
            Appl_Memcpy(FldownloadStatus.fingerPrint, data, FL_FINGER_PRINT_LENGTH);
            FldownloadStatus.errorCode = (uint8)FL_OK;

            /* set the download state of the finger print */
            FldownloadStatus.fingerPrintWritten = TRUE;

            ret = (uint8)FL_OK;
        }
        else
        {
            ret = (uint8)FL_FAILED;
        }
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <0x31 service routine checksum>
 *
 * <checksum routine for flash driver or logical blocks,only for current
 *  download address> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <checkSum (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
FL_ResultType FL_CheckSumRoutine(const uint8 *checkSum)
{
    FL_ResultType ret = (uint8)FL_OK;

    /* record checksum buffer address */
    FldownloadStatus.dataBuff = checkSum;

    /* check if download step is checksum step */
    if (FL_CHECKSUM_STEP == FldownloadStatus.downloadStep)
    {
        /* set the download active state to checksum */
        FldownloadStatus.activeJob = FL_JOB_CHECKING;
        FldownloadStatus.errorCode = (uint8)FL_OK;
        if (TRUE == FldownloadStatus.flDrvDownloaded)
        {
            ret = FL_DownloadRemainData();
        }
        else
        {
            /* empty */
        }
    }
    else
    {
        ret = (uint8)FL_ERR_SEQUENCE;

        /* initialize the flash download state */
        FL_InitState();
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <0x31 service routine erase>
 *
 * <erase routine for  logical block> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <blockIndex (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
FL_ResultType FL_EraseRoutine(const uint8 index)
{
    FL_ResultType ret = (uint8)FL_OK;

    /* check if finger print is written */
    if ((FALSE == FldownloadStatus.fingerPrintWritten))
    {
        ret = (uint8)FL_NO_FINGERPRINT;
    }
    else
    {
        /* check if flash driver has been downloaded */
        if (FALSE == FldownloadStatus.flDrvDownloaded)
        {
            ret = (uint8)FL_NO_FLASHDRIVER;
        }
        else
        {
            /* check download step sequence */
            if (FldownloadStatus.downloadStep != FL_REQUEST_STEP)
            {
                ret = (uint8)FL_ERR_SEQUENCE;
            }
            else
            {
                /* empty */
            }
        }
    }

    /* check the conditions of erase */
    if ((uint8)FL_OK == ret)
    {
        /* check the erase block index and the program attempt */
        if (index < FL_NUM_LOGICAL_BLOCKS)
        {
            if ((FL_NvmInfo.blockInfo[index].blkProgAttempt < FL_BlkInfo[index].maxProgAttempt) || ((uint16)0x00u == FL_BlkInfo[index].maxProgAttempt))
            {
                /* set current block is invalid */
                FL_NvmInfo.blockInfo[index].blkValid = FALSE;

                /* increment program attempt counter of current block */
                FL_NvmInfo.blockInfo[index].blkProgAttempt++;

                /* store repair shop code of current block */

                /* store finger print of current block */
                Appl_Memcpy(FL_NvmInfo.blockInfo[index].fingerPrint,
                            FldownloadStatus.fingerPrint, FL_FINGER_PRINT_LENGTH);

                /* change and initialize status of the programmed block */
                FldownloadStatus.segmentList.nrOfSegments = (uint8)0x00u;
                FldownloadStatus.blockIndex = index;
                FldownloadStatus.blockErased = FALSE;
                FldownloadStatus.errorCode = (uint8)FL_OK;

                /* set the download active state to erase */
                FldownloadStatus.activeJob = FL_JOB_ERASING;
            }
            else
            {
                ret = (uint8)FL_FAILED;
            }
        }
        else
        {
            ret = (uint8)FL_INVALID_DATA;
        }
    }
    else
    {
        /* empty */
    }

    if (ret != (uint8)FL_OK)
    {
        /* initialize the flash download state */
        FL_InitState();
    }
    else
    {
        /* empty */
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <0x34 service download request>
 *
 * <download request for current logical block> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <startAdd (IN), length (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
FL_ResultType FL_DownloadRequestValid(const uint32 startAdd, const uint32 length)
{
    FL_ResultType ret = (uint8)FL_OK;

    /* save the parameter used for active job use */
    FldownloadStatus.startAddr = startAdd;
    FldownloadStatus.downLength = length;

    /* check if finger print is written */
    if ((FALSE == FldownloadStatus.fingerPrintWritten))
    {
        ret = (uint8)FL_NO_FINGERPRINT;
    }
    else
    {
        /* check download step sequence */
        if ((FldownloadStatus.downloadStep != FL_REQUEST_STEP) && (FldownloadStatus.downloadStep != FL_CHECKSUM_STEP))
        {
            ret = (uint8)FL_ERR_SEQUENCE;
        }
        else
        {
            if (0UL == FldownloadStatus.downLength)
            {
                ret = (uint8)FL_ERR_ADDR_LENGTH;
            }
            else
            {
                /* empty */
            }
        }
    }

    /* check the condition for download request */
    if ((uint8)FL_OK == ret)
    {
        /* check if flash driver is downloaded */
        if (FALSE == FldownloadStatus.flDrvDownloaded)
        {
            /* check if download address is in flash driver RAM range */
            if ((FL_DEV_BASE_ADDRESS == FldownloadStatus.startAddr) && (FldownloadStatus.downLength <= FL_DEV_SIZE))
            {
                /* set the download step */
                FldownloadStatus.downloadStep = FL_TRANSFER_STEP;
                FldownloadStatus.segmentList.nrOfSegments = (uint8)0x01u;
                FldownloadStatus.segmentList.segmentInfo[0].address = FldownloadStatus.startAddr;
                FldownloadStatus.segmentList.segmentInfo[0].length = FldownloadStatus.downLength;
            }
            else
            {
                ret = (uint8)FL_NO_FLASHDRIVER;
            }
        }
        else
        {
            /* check if download address is in correct range */
            ret = FL_CheckDownloadSegment();

            if ((uint8)FL_OK == ret)
            {
                ret = FL_HandleRemainData();
                /* set the download step */
                FldownloadStatus.downloadStep = FL_TRANSFER_STEP;
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

    if (ret != (uint8)FL_OK)
    {
        /* initialize the flash download state */
        FL_InitState();
    }
    else
    {
        /* empty */
    }

    FldownloadStatus.activeJob = FL_JOB_IDLE;

    return ret;
}

/******************************************************************************/
/**
 * @brief               <0x36 service download data>
 *
 * <download data for current logical block> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <destAddr (IN), sourceBuff (IN), length (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
FL_ResultType FL_FlashProgramRegion(const uint32 destAddr,
                                    const uint8 *sourceBuff,
                                    const uint32 length)
{
    FL_ResultType ret = (uint8)FL_OK;

    /* change local address to global address */
    const uint32 globalAddr = destAddr;

    /* check the conditions of the program */
    if (FldownloadStatus.downloadStep != FL_TRANSFER_STEP)
    {
        ret = (uint8)FL_ERR_SEQUENCE;
    }
    else
    {
        if ((FldownloadStatus.startAddr != globalAddr) || (FldownloadStatus.downLength < length))
        {
            ret = (uint8)FL_ERR_ADDR_LENGTH;
        }
        else
        {
            /* empty */
        }
    }

    if ((uint8)FL_OK == ret)
    {
        /* check if flash driver is downloaded */
        if (FALSE == FldownloadStatus.flDrvDownloaded)
        {
            /* copy the data to the request address */
            Appl_Memcpy((uint8 *)globalAddr, sourceBuff, length);

            /* index start address and length */
            FldownloadStatus.startAddr += length;
            FldownloadStatus.downLength -= length;

            /* check if flash driver download is finished */
            if (0UL == FldownloadStatus.downLength)
            {
                /* set the download step*/
                FldownloadStatus.downloadStep = FL_EXIT_TRANSFER_STEP;
            }
            else
            {
                /* empty */
            }

            FldownloadStatus.activeJob = FL_JOB_IDLE;
        }
        else
        {
            /* save parameter for program active job */
            FldownloadStatus.dataBuff = sourceBuff;
            FlProgramLength = length;

            /* set the download active state to program */
            FldownloadStatus.activeJob = FL_JOB_PROGRAMMING;
            FldownloadStatus.errorCode = (uint8)FL_OK;
        }
    }
    else
    {
        /* empty */
    }

    if (ret != (uint8)FL_OK)
    {
        /* initialize the flash download state */
        FL_InitState();
    }
    else
    {
        /* empty */
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <0x37 service download finish>
 *
 * <download finish for current logical block> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
FL_ResultType FL_ExitTransferData(void)
{
    FL_ResultType ret = (uint8)FL_OK;

    if (FL_EXIT_TRANSFER_STEP == FldownloadStatus.downloadStep)
    {
        /* set the download step */
        FldownloadStatus.downloadStep = FL_CHECKSUM_STEP;
        FldownloadStatus.activeJob = FL_JOB_IDLE;
    }
    else
    {
        /* initialize the flash download state */
        ret = (uint8)FL_ERR_SEQUENCE;
        FL_InitState();
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <get service status>
 *
 * <supply active job status for service witch can not response with in 50ms,
 *  and send pending> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <NON Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <error (OUT)>
 * @param[in/out]       <NONE>
 * @return              <boolean>
 */
/******************************************************************************/
boolean FL_ServiceFinished(FL_ResultType *error)
{
    boolean ret = FALSE;

    /* check if service is finished */
    if ((FL_JOB_IDLE == FldownloadStatus.activeJob) && (FldownloadStatus.errorCode != (uint8)FL_UPDATING_NVM))
    {
        *error = FldownloadStatus.errorCode;
        ret = TRUE;
    }
    else
    {
        *error = (uint8)FL_OK;
        ret = FALSE;
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <flash main function for active job>
 *
 * <flash main function for active job,process checksum,erase and program> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
void FL_MainFunction(void)
{
    switch (FldownloadStatus.activeJob)
    {
    case FL_JOB_ERASING:
        /* do the flash erase */
        FldownloadStatus.errorCode = FL_Erasing();
        break;

    case FL_JOB_PROGRAMMING:
        /* do the flash program */
        FldownloadStatus.errorCode = FL_Programming();
        break;

    case FL_JOB_CHECKING:
        /* do the flash checksum */
        FldownloadStatus.errorCode = FL_CheckSuming();
        break;

    default:
        break;
    }

    if (FldownloadStatus.errorCode != (uint8)FL_OK)
    {
        /* initialize the flash download state */
        FL_InitState();
    }
    else
    {
        /* empty */
    }

    FldownloadStatus.activeJob = FL_JOB_IDLE;

    return;
}

/******************************************************************************/
/**
 * @brief               <program bootloader infomation to EEPROM>
 *
 * <program bootloader infomation to EEPROM,e.g. block valid,checksum,
 *  fingerprint..> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
STATIC FL_ResultType FL_UpdateNvm(void)
{
    FL_ResultType ret = (uint8)FL_FAILED;
    /* CRC32 parameter */
    SecM_CRCParamType crcParam;

    /* Initialize CRC32 parameter */
    crcParam.crcState = (uint8)SECM_CRC_INIT;
    crcParam.crcSourceBuffer = (const uint8 *)&FL_NvmInfo;
    crcParam.crcByteCount = (uint16)(sizeof(FL_NvmInfoType) - (uint8)4);

    /* compute CRC of the block information */
    (void)SecM_ComputeCRC(&crcParam);
    crcParam.crcState = (uint8)SECM_CRC_COMPUTE;
    (void)SecM_ComputeCRC(&crcParam);
    crcParam.crcState = (uint8)SECM_CRC_FINALIZE;
    (void)SecM_ComputeCRC(&crcParam);

    /* program computed CRC value to flash */
    FL_NvmInfo.infoChecksum = crcParam.currentCRC;

    /* set input parameter of flash driver interface */
    flashParamInfo.data = (const uint8 *)&FL_NvmInfo;
    flashParamInfo.address = FL_NVM_INFO_ADDRESS;
    flashParamInfo.length = (uint32)sizeof(FL_NvmInfoType);

    /* erase flash block witch store the blocks information */
    BLFlash_InfoPtr->flashEraseFct(&flashParamInfo);

    if ((uint8)kFlashOk == flashParamInfo.errorCode)
    {
        /* program blocks information */
        flashParamInfo.length = (uint32)sizeof(FL_NvmInfoType);

        BLFlash_InfoPtr->flashWriteFct(&flashParamInfo);
    }
    else
    {
        /* empty */
    }

    if ((uint8)kFlashOk == flashParamInfo.errorCode)
    {
        /* finger print is not written to NVM*/
        FldownloadStatus.fingerPrintWrittenFlag = TRUE;

        ret = (uint8)FL_OK;
    }
    else
    {
        /* empty */
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <active job erase>
 *
 * <erase the current logical block witch requested by 0x31 service> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
STATIC FL_ResultType FL_Erasing(void)
{
    FL_ResultType ret = (uint8)FL_OK;
    tFlashLength tempLength;
    uint8 pendingIndex = 0u;
    /* update the bootloader information to EEPROM */
    ret = FL_UpdateNvm();

    if ((uint8)FL_OK == ret)
    {
        /* set flash driver input parameter */
        flashParamInfo.address = FL_BlkInfo[FldownloadStatus.blockIndex].address;
        flashParamInfo.length = FL_BlkInfo[FldownloadStatus.blockIndex].length;

        if (flashParamInfo.length <= 0x30000U)
        {
            /* erase the flash of the requested block */
            BLFlash_InfoPtr->flashEraseFct(&flashParamInfo);
        }
        else
        {
            tempLength = flashParamInfo.length - 0x30000U;
            flashParamInfo.length = 0x30000U;
            BLFlash_InfoPtr->flashEraseFct(&flashParamInfo);
            flashParamInfo.address += flashParamInfo.length;
            /* force sending pending */
            Dcm_SendNcr(0x78U);
            CanTp_MainFunction();
            CanIf_MainFunction();

            while (((uint8)kFlashOk == flashParamInfo.errorCode) && (tempLength > 0x40000u) && (flashParamInfo.address < 0xA0200000u))
            {
                pendingIndex++;
                flashParamInfo.length = 0x40000U;
                BLFlash_InfoPtr->flashEraseFct(&flashParamInfo);
                flashParamInfo.address += flashParamInfo.length;
                tempLength -= 0x40000U;

                if (2u == pendingIndex)
                {
                    pendingIndex = 0u;
                    /* force sending pending */
                    Dcm_SendNcr(0x78U);
                    CanTp_MainFunction();
                    CanIf_MainFunction();
                }
            }

            if (flashParamInfo.address >= 0xA0200000u)
            {
                /* force sending pending */
                Dcm_SendNcr(0x78U);
                CanTp_MainFunction();
                CanIf_MainFunction();

                pendingIndex = 0u;
                while (((uint8)kFlashOk == flashParamInfo.errorCode) && (tempLength >= 0x40000U))
                {
                    pendingIndex++;

                    flashParamInfo.length = 0x40000U;
                    BLFlash_InfoPtr->flashEraseFct(&flashParamInfo);
                    flashParamInfo.address += flashParamInfo.length;
                    tempLength -= 0x40000U;
                    if (2u == pendingIndex)
                    {
                        pendingIndex = 0u;
                        /* force sending pending */
                        Dcm_SendNcr(0x78U);
                        CanTp_MainFunction();
                        CanIf_MainFunction();
                    }
                }
            }
        }

        /* check if erase success */
        if ((uint8)kFlashOk == flashParamInfo.errorCode)
        {
            /* set the block erased */
            FldownloadStatus.blockErased = TRUE;
        }
        else
        {
            ret = (uint8)FL_FAILED;
        }
    }
    else
    {
        /* empty */
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <check segment address>
 *
 * <check if the transfered address is in current block,and if the address is
 *  increased by segment> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
STATIC FL_ResultType FL_CheckDownloadSegment(void)
{
    FL_ResultType ret = (uint8)FL_OK;
    FL_SegmentInfoType *curSegment;
    uint8 segmentIndex;

    /*
     ** check if block is erased, if current num of segment is less than maximum,
     ** if address if in current block.
     */
    if (FALSE == FldownloadStatus.blockErased)
    {
        ret = (uint8)FL_ERR_SEQUENCE;
    }
    else
    {
        if ((FldownloadStatus.segmentList.nrOfSegments < FL_MAX_SEGMENTS) && (FldownloadStatus.startAddr >= FL_BlkInfo[FldownloadStatus.blockIndex].address) && ((FldownloadStatus.startAddr + FldownloadStatus.downLength) <= (FL_BlkInfo[FldownloadStatus.blockIndex].address + FL_BlkInfo[FldownloadStatus.blockIndex].length)))
        {
            /* get current segment number */
            segmentIndex = FldownloadStatus.segmentList.nrOfSegments;

            /* check if segment address is increase */
            if (segmentIndex > (uint8)0x00u)
            {
                curSegment = &FldownloadStatus.segmentList.segmentInfo[segmentIndex - (uint8)1];

                /* check if download address is in front segment range */
                if (FldownloadStatus.startAddr < (curSegment->address + curSegment->length))
                {
                    ret = (uint8)FL_ERR_ADDR_LENGTH;
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

            if ((uint8)FL_OK == ret)
            {
                /* set the flash download info */
                curSegment = &FldownloadStatus.segmentList.segmentInfo[segmentIndex];
                FldownloadStatus.segmentList.nrOfSegments++;
                curSegment->address = FldownloadStatus.startAddr;
                curSegment->length = FldownloadStatus.downLength;
            }
            else
            {
                /* empty */
            }
        }
        else
        {
            ret = (uint8)FL_ERR_ADDR_LENGTH;
        }
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <FL_DownloadRemainData>
 *
 *
 *
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
STATIC FL_ResultType FL_DownloadRemainData(void)
{
    FL_ResultType ret = (uint8)FL_OK;

    if (FL_RemainDataStruct.remainLength != 0UL)
    {
        /*
         *  two segments are not in one page or checking sum is started,
         *  so download the last segment first
         */
        flashParamInfo.address = FL_RemainDataStruct.remainAddr;
        /* fill pad for the left data */
        Appl_Memset(&FlProgramData[FL_RemainDataStruct.remainLength],
                    FL_GAP_FILL_VALUE,
                    FL_PROGRAM_SIZE - FL_RemainDataStruct.remainLength);

        /* set the flash download info */
        flashParamInfo.data = &FlProgramData[0];
        flashParamInfo.length = FL_PROGRAM_SIZE;
        FL_RemainDataStruct.remainLength = 0x00U;
        /* write the last 0x36 aligned data */
        BLFlash_InfoPtr->flashWriteFct(&flashParamInfo);

        if (flashParamInfo.errorCode != (uint8)kFlashOk)
        {
            ret = (uint8)FL_FAILED;
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

    return ret;
}

/******************************************************************************/
/**
 * @brief               <FL_HandleRemainData>
 *
 *
 *
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
STATIC FL_ResultType FL_HandleRemainData(void)
{
    FL_ResultType ret = (uint8)FL_OK;

    if (FL_RemainDataStruct.remainLength != 0UL)
    {
        if ((FL_RemainDataStruct.remainAddr & ~(FL_PROGRAM_SIZE - (uint32)1)) == (FldownloadStatus.startAddr & ~(FL_PROGRAM_SIZE - (uint32)1)))
        {
            /*
             *  link the remain data and new segment because one page,
             *  and download together via 0x36
             */
            Appl_Memset(&FlProgramData[FL_RemainDataStruct.remainLength],
                        FL_GAP_FILL_VALUE,
                        FldownloadStatus.startAddr - FL_RemainDataStruct.remainLength - FL_RemainDataStruct.remainAddr);
            FL_RemainDataStruct.remainLength = FldownloadStatus.startAddr - FL_RemainDataStruct.remainAddr;
        }
        else
        {
            ret = FL_DownloadRemainData();
            FL_RemainDataStruct.remainLength = FldownloadStatus.startAddr & (FL_PROGRAM_SIZE - (uint32)1);
            /* initialize the program buffer */
            Appl_Memset(&FlProgramData[0], FL_GAP_FILL_VALUE, (uint32)FL_PROGRAM_SIZE);
        }
    }
    else
    {
        FL_RemainDataStruct.remainLength = FldownloadStatus.startAddr & (FL_PROGRAM_SIZE - (uint32)1);
        /* initialize the program buffer */
        Appl_Memset(&FlProgramData[0], (uint8)FL_GAP_FILL_VALUE, (uint32)FL_PROGRAM_SIZE);
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <program data>
 *
 * <program the aligned data transfered by 0x36 service request > .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
STATIC FL_ResultType FL_ProgrammingData(void)
{
    FL_ResultType ret = (uint8)FL_OK;

    /* check the program length and program status */
    while ((FlProgramLength > 0UL) && ((uint8)FL_OK == ret))
    {
        /* check if the program size is more than maximum size of program buffer */
        if ((FlProgramLength + FL_RemainDataStruct.remainLength) >= (uint32)FL_PROGRAM_SIZE)
        {
            /* get the download data */
            Appl_Memcpy(&FlProgramData[FL_RemainDataStruct.remainLength],
                        FldownloadStatus.dataBuff,
                        (uint32)FL_PROGRAM_SIZE - FL_RemainDataStruct.remainLength);

            /* index the databuff point in transfered buffer */
            FldownloadStatus.dataBuff += FL_PROGRAM_SIZE - FL_RemainDataStruct.remainLength;

            /* index the total program length */
            FlProgramLength -= FL_PROGRAM_SIZE - FL_RemainDataStruct.remainLength;

            /* set the flash driver input parameter */
            flashParamInfo.address = FldownloadStatus.startAddr - FL_RemainDataStruct.remainLength;
            flashParamInfo.length = (uint32)FL_PROGRAM_SIZE;
            flashParamInfo.data = &FlProgramData[0];

            /* program the data */
            BLFlash_InfoPtr->flashWriteFct(&flashParamInfo);

            /* index the start address and length that record in 0x34 service */
            FldownloadStatus.startAddr += FL_PROGRAM_SIZE - FL_RemainDataStruct.remainLength;
            FldownloadStatus.downLength -= FL_PROGRAM_SIZE - FL_RemainDataStruct.remainLength;
            FL_RemainDataStruct.remainAddr = FldownloadStatus.startAddr;
            FL_RemainDataStruct.remainLength = 0x00U;
            /* check if program success */
            if (flashParamInfo.errorCode != (uint8)kFlashOk)
            {
                ret = (uint8)FL_FAILED;
            }
            else
            {
                /* empty */
            }
        }
        else
        {
            /* set the last data for write of current service 0x36 */
            Appl_Memcpy(&FlProgramData[FL_RemainDataStruct.remainLength],
                        FldownloadStatus.dataBuff,
                        FlProgramLength);
            FL_RemainDataStruct.remainAddr = FldownloadStatus.startAddr - FL_RemainDataStruct.remainLength;
            FL_RemainDataStruct.remainLength += FlProgramLength;
            /* index the start address and length that record in 0x34 service */
            FldownloadStatus.startAddr += FlProgramLength;
            FldownloadStatus.downLength -= FlProgramLength;
            /* end of current service 0x36 */
            FlProgramLength = 0UL;
        }
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <active job program>
 *
 * <program the data transfered by 0x36 service request > .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
STATIC FL_ResultType FL_Programming(void)
{
    FL_ResultType ret = (uint8)FL_OK;

    /* program buffer aligned data */
    ret = FL_ProgrammingData();

    /* check if the last not aligned data should be programmed */
    if ((0UL == FldownloadStatus.downLength) && ((uint8)FL_OK == ret))
    {
        FldownloadStatus.downloadStep = FL_EXIT_TRANSFER_STEP;
    }
    else
    {
        /* empty */
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <fill pad>
 *
 * <fill the pad between segments of current block> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
#if (STD_ON == FL_USE_GAP_FILL)
STATIC FL_ResultType FL_FillGap(void)
{
    FL_ResultType ret = FL_OK;
    uint8 segmentIndex = 0;
    uint32 startAddress = FL_BlkInfo[FldownloadStatus.blockIndex].address;
    uint32 gapLength;

    /* set the download data with FL_GAP_FILL_VALUE */
    Appl_Memset((uint8 *)&FlProgramData[0], (uint8)FL_GAP_FILL_VALUE, (uint32)FL_PROGRAM_SIZE);

    flashParamInfo.data = (uint8 *)&FlProgramData[0];

    while ((segmentIndex <= FldownloadStatus.segmentList.nrOfSegments) && (FL_OK == ret))
    {
        /* find the length of the gap in the segment */
        if (segmentIndex < FldownloadStatus.segmentList.nrOfSegments)
        {
            gapLength = FldownloadStatus.segmentList.segmentInfo[segmentIndex].address - startAddress;
        }
        else
        {
            gapLength = (FL_BlkInfo[FldownloadStatus.blockIndex].address + FL_BlkInfo[FldownloadStatus.blockIndex].length) - startAddress;
        }

        gapLength &= ~(FL_FLASH_ALIGN_SIZE - 1);

        /* set the flash download address of gap */
        flashParamInfo.address = startAddress;

        while ((gapLength > 0) && (FL_OK == ret))
        {
            if (gapLength >= FL_PROGRAM_SIZE)
            {
                /* set the download length */
                flashParamInfo.length = FL_PROGRAM_SIZE;

                /* update the gap length */
                gapLength -= FL_PROGRAM_SIZE;
            }
            else
            {
                /* the last gap */
                flashParamInfo.length = gapLength;
                gapLength = 0;
            }

            /* write the flash of the FlashParam for gap */
            BLFlash_Info.flashWriteFct(&flashParamInfo);

            flashParamInfo.address += flashParamInfo.length;

            /* check if program pad success */
            if (flashParamInfo.errorCode != kFlashOk)
            {
                ret = FL_FAILED;
            }
        }

        if (segmentIndex < FldownloadStatus.segmentList.nrOfSegments)
        {
            /* set the next start address */
            startAddress = FldownloadStatus.segmentList.segmentInfo[segmentIndex].address + FldownloadStatus.segmentList.segmentInfo[segmentIndex].length;

            if ((startAddress & (FL_FLASH_ALIGN_SIZE - 1)) > 0)
            {
                startAddress &= ~(FL_FLASH_ALIGN_SIZE - 1);
                startAddress += FL_FLASH_ALIGN_SIZE;
            }
        }

        segmentIndex++;
    }

    return ret;
}
#endif

/******************************************************************************/
/**
 * @brief               <active job checksum>
 *
 * <active checksum that request by 0x31 service> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>
 */
/******************************************************************************/
STATIC FL_ResultType FL_CheckSuming(void)
{
    FL_ResultType ret = (uint8)FL_OK;
    SecM_StatusType secMStatus;
    SecM_VerifyParamType verifyParam;

    /* set verification API input parameter */
    verifyParam.segmentList = &FldownloadStatus.segmentList;
    verifyParam.verificationData = FldownloadStatus.dataBuff;

    /* CRC compute and verification */
    secMStatus = SecM_Verification(&verifyParam);

    /* set block not erased */
    FldownloadStatus.blockErased = FALSE;

    /* check if CRC if correct */
    if (SECM_OK == secMStatus)
    {
        /* check if flash driver is downloaded */
        if (FALSE == FldownloadStatus.flDrvDownloaded)
        {
            /* flash driver initialize */
            BLFlash_InfoPtr->flashInitFct(&flashParamInfo);

            /* check if flash driver is initialized success */
            if (flashParamInfo.errorCode != (uint8)kFlashOk)
            {
                ret = (uint8)FL_FAILED;
            }
            else
            {
                FldownloadStatus.flDrvDownloaded = TRUE;
            }
        }
        else
        {
            /* set current block is valid */
            FL_NvmInfo.blockInfo[FldownloadStatus.blockIndex].blkValid = TRUE;

            /* save computed CRC to NVM if CRC success */
            FL_NvmInfo.blockInfo[FldownloadStatus.blockIndex].blkChecksum = verifyParam.crcTotle;

            /* fill the gap if configured */
#if (STD_ON == FL_USE_GAP_FILL)
            ret = FL_FillGap();

            if ((uint8)FL_OK == ret)
#endif
            {
                /* response pending (7F 2E 78) during updating nvm */
                FldownloadStatus.errorCode = (uint8)FL_UPDATING_NVM;

                /* update nvm */
                ret = FL_UpdateNvm();
            }

            /* check if EEPROM UPDATE failed */
            if (ret != (uint8)FL_OK)
            {
                FL_NvmInfo.blockInfo[FldownloadStatus.blockIndex].blkValid = FALSE;
            }
            else
            {
                /* empty */
            }
        }
    }
    else
    {
        ret = (uint8)FL_FAILED;
    }

    /* reset download step */
    FldownloadStatus.downloadStep = FL_REQUEST_STEP;

    return ret;
}

/******************************************************************************/
/**
 * @brief               <update prog cnt>
 *
 * <update prog cnt> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <void>
 */
/******************************************************************************/
void FL_updateProgCnt(void)
{
    uint8 index;

    /* prog counter increment */
    for (index = (uint8)0; index < FL_NUM_LOGICAL_BLOCKS; index++)
    {
        if (FL_NvmInfo.blockInfo[index].blkProgCounter < (uint8)0xFFu)
        {
            FL_NvmInfo.blockInfo[index].blkProgCounter++;
        }
        else
        {
            FL_NvmInfo.blockInfo[index].blkProgCounter = (uint8)0;
        }
    }

    /* response pending (7F xx 78) during updating nvm */
    FldownloadStatus.errorCode = (uint8)FL_UPDATING_NVM;

    /* update Nvm */
    (void)FL_UpdateNvm();

    FldownloadStatus.errorCode = (uint8)FL_OK;

    return;
}

uint16 Fl_GetActiveJob(void)
{
    return FldownloadStatus.activeJob;
}
/*=======[E N D   O F   F I L E]==============================================*/

