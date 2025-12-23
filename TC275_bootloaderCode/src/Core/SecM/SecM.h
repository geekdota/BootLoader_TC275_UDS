/*============================================================================*/
/** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 *
 *  @file       <SecM.h>
 *  @brief      <Macros,Types defines and function declarations for Security
 *              Module>
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <10086>
 *  @date       <2012-11-09>
 */
/*============================================================================*/
#ifndef SECM_H
#define SECM_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121109    Gary       Initial version
 *
 *  V1.1    20130913    ccl        update
 *
 *  V1.2    20180511    CChen      update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "std_types.h"
#include "fl.h"
#include "secm_cfg.h"

/*=======[M A C R O S]========================================================*/

/** CRC buffer length */
#define SECM_CRC_BUFFER_LEN 100

#define SECM_OK (uint8)0x00U
#define SECM_NOT_OK (uint8)0x01U

/** CRC step */
#define SECM_CRC_INIT 0x00u
#define SECM_CRC_COMPUTE 0x01u
#define SECM_CRC_FINALIZE 0x02u

#if (CAL_CRC32 == CAL_METHOD)
#define SECM_CRC_LENGTH 0x04u
#else
#define SECM_CRC_LENGTH 0x02u
#endif

/* frc base address */
#define FRC_REG_BASE (0x40u)
#define FRC_TCNTH (0x04u)
#define FRC_TCNTL (0x05u)
#define FRC_TSCR1 (0x06u)

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/** return type for SecM module */
typedef uint8 SecM_StatusType;

/** type for Seed */
typedef uint32 SecM_WordType;

/** struct type for Seed */
typedef uint32 SecM_SeedType;

/** type for Key */
typedef SecM_WordType SecM_KeyType;

/** type for Crc value */
#if (CAL_CRC32 == CAL_METHOD)
typedef uint32 SecM_CRCType;
#else
typedef uint16 SecM_CRCType;
#endif

/** struct type for Crc */
typedef struct
{
    /* current CRC value */
    SecM_CRCType currentCRC;

    /* CRC step */
    uint8 crcState;

    /* CRC buffer point */
    const uint8 *crcSourceBuffer;

    /* CRC length */
    uint16 crcByteCount;

} SecM_CRCParamType;

/** struct type for verify parameter list */
typedef struct
{
    /* segment list for block */
    FL_SegmentListType *segmentList;

    /* Crc value transfered by UDS */
    const uint8 *verificationData;

    /* Crc value totle */
    SecM_CRCType crcTotle;

} SecM_VerifyParamType;

/*=======[E X T E R N A L   D A T A]==========================================*/
/** CRC buffer */
extern uint8 SecM_CrcDataBuffer[SECM_CRC_BUFFER_LEN];

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern SecM_StatusType SecM_GenerateSeed(SecM_SeedType *seed);

extern SecM_StatusType SecM_ComputeKey(SecM_SeedType seed,
                                       SecM_WordType mask,
                                       SecM_KeyType *key);

extern SecM_StatusType SecM_CompareKey(SecM_KeyType key, SecM_SeedType seed);

extern SecM_StatusType SecM_ComputeCRC(SecM_CRCParamType *crcParam);

extern SecM_StatusType SecM_Verification(SecM_VerifyParamType *verifyParam);
#if 0
extern void Frc_Init(void);
extern void Frc_Uninit(void);
#endif
#endif /* endof SECM_H */

/*=======[E N D   O F   F I L E]==============================================*/
