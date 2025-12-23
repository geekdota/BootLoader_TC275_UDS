/*============================================================================*/
/** Copyright (C) 2009-2018, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is 10086 property. Duplication
 *  or disclosure without 10086 written authorization is prohibited.
 *
 *  @file       <FL.h>
 *  @brief      <Macros,Types defines and function declarations for Flash Driver
 *              Module>
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <10086>
 *  @date       <2016-10-25>
 */
/*============================================================================*/
#ifndef FLASHLOADER_H_
#define FLASHLOADER_H_

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
#include "std_types.h"
#include "fl_cfg.h"

/*=======[M A C R O S]========================================================*/
/* FL module execute result */
#define FL_OK 0x00U
#define FL_FAILED 0x01U
#define FL_ERR_SEQUENCE 0x02U
#define FL_NO_FINGERPRINT 0x03U
#define FL_NO_FLASHDRIVER 0x04U
#define FL_ERR_ADDR_LENGTH 0x05U
#define FL_INVALID_DATA 0x06U
#define FL_UPDATING_NVM 0x07U

/** Value indicate an external programming request. */
#define FL_EXT_PROG_REQUEST_RECEIVED 0x5AA5A55AU

/** Value indicate an update of the application software. */
#define FL_APPL_UPDATED 0xA55A5AA5u

/** Value indicate default session request from prog. of the bootloader software. */
#define FL_BOOT_DEFAULT_FROM_PROG 0xA55AAAAAU

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/** Standard return type for call back routines. */
typedef uint8 FL_ResultType;

/* needed in the interface between flashloader runtime environment and security module */
typedef struct
{
    /* block start global address */
    const uint32 address;

    /* block length */
    const uint32 length;

    /* maximum program cycle */
    const uint16 maxProgAttempt;

} FL_BlockDescriptorType;

/** Segment list information of the block */
typedef struct
{
    /* Start global address of the segment in flash */
    uint32 address;

    /* Length of the segment */
    uint32 length;

} FL_SegmentInfoType;

/** Needed in interface to the security module. */
typedef struct
{
    /* number of segment */
    uint8 nrOfSegments;

    /* segments information */
    FL_SegmentInfoType segmentInfo[FL_MAX_SEGMENTS];

} FL_SegmentListType;

/** Each logical block information */
typedef struct
{
    boolean blkValid;

    uint8 blkProgAttempt;

    uint8 blkProgCounter;

    uint8 reserved;

    uint8 fingerPrint[FL_FINGER_PRINT_LENGTH];

    uint32 blkChecksum;

} FL_blockInfoType;

/** Total block information */
typedef struct
{
    FL_blockInfoType blockInfo[FL_NUM_LOGICAL_BLOCKS];

    /* Security access error counter */
    uint8 secAccessErr;

    uint8 reserved1;

    uint8 reserved2;

    uint8 reserved3;

    /* NVM information checksum */
    uint32 infoChecksum;

} FL_NvmInfoType;

/*=======[E X T E R N A L   D A T A]==========================================*/
/* information which shall be programmed to EEPROM */
extern FL_NvmInfoType FL_NvmInfo;

/* logical block information that configured by user */
extern const FL_BlockDescriptorType FL_BlkInfo[FL_NUM_LOGICAL_BLOCKS];

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern void FL_InitState(void);

extern uint16 Fl_GetActiveJob(void);

extern uint16 FL_ReadProgCounter(uint8 *readData);

extern uint16 FL_ReadProgAttemptCounter(uint8 *readData);

extern uint16 FL_ReadBootSWReferenceNumber(uint8 *readData);

extern uint16 FL_ReadFingerPrint(uint8 *readData);

extern uint16 FL_ReadSysECUHWVer(uint8 *readData);

extern uint32 FL_ReadMemory(uint32 address, uint32 length, uint8 *data);

extern uint8 FL_CheckProgPreCondition(uint8 *conditions);

extern FL_ResultType FL_WriteRepairShopCode(const uint8 *data, const uint16 length);

extern FL_ResultType FL_WriteFingerPrint(const uint8 *data, const uint16 length);

extern FL_ResultType FL_CheckSumRoutine(const uint8 *checkSum);

extern FL_ResultType FL_EraseRoutine(const uint8 index);

extern FL_ResultType FL_DownloadRequestValid(const uint32 startAdd,
                                             const uint32 length);

extern FL_ResultType FL_FlashProgramRegion(const uint32 destAddr,
                                           const uint8 *sourceBuff,
                                           const uint32 length);

extern FL_ResultType FL_ExitTransferData(void);

extern boolean FL_ServiceFinished(FL_ResultType *error);

extern void FL_MainFunction(void);

extern void FL_updateProgCnt(void);

#endif /* endof FL_H */

/*=======[E N D   O F   F I L E]==============================================*/

