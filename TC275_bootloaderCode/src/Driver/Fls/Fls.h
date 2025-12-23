/*============================================================================*/
/** Copyright (C) 2009-2018, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is 10086 property. Duplication
 *  or disclosure without 10086 written authorization is prohibited.
 *
 *
 *  @file       <Flash.h>
 *  @brief      <Macros,Types defines and function declarations for Flash Driver
 *              Module>
 *
 *  @author     <10086>
 *  @date       <2017-09-09>
 */
/*============================================================================*/
#ifndef FLASH_H
#define FLASH_H

/*=======[I N C L U D E S]====================================================*/
#include "std_types.h"
#include "fls_cfg.h"

/*=======[M A C R O S]========================================================*/
/** result values of flash driver routines */
#define kFlashOk 0x00u
#define kFlashFailed 0x01u

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/** flash driver major version number */
typedef uint8 tMajorVersion;

/** flash driver minor version number */
typedef uint8 tMinorVersion;

/** bugfix / patchlevel number */
typedef uint8 tBugfixVersion;

/** result of flash driver routines */
typedef uint8 tFlashResult;

/** logical address */
typedef uint32 tFlashAddress;

/** length (in bytes) */
typedef uint32 tFlashLength;

/** ambiguous data */
typedef uint8 tFlashData;

/** watchdog trigger routine */
typedef void (*tWDTriggerFct)(void);

/** initialization: input parameters */
typedef struct
{
    /* flash driver patch level version */
    tBugfixVersion patchLevel;

    /* flash driver minor version number */
    tMajorVersion minorNumber;

    /* flash driver major version number */
    tMinorVersion majorNumber;

    /* reserved for future use, set to 0x00 for now */
    uint8 reserved1;

    /* retrun value / error code: output parameters */
    tFlashResult errorCode;

    /* reserved for future use, set to 0x0000 for now */
    uint16 reserved2;

    /* erase / write: input parameters */
    /* logical target address */
    tFlashAddress address;

    /* lenght information (in bytes) */
    tFlashLength length;

    /* pointer to data buffer */
    const tFlashData *data;

    /* pointer to watchdog trigger routine */
    tWDTriggerFct wdTriggerFct;

} tFlashParam;

/** prototype of flash driver routine */
typedef void (*tFlashFct)(tFlashParam *flashParam);

/** flash infomation table */
typedef struct
{
    /* Motorola Star12 */
    const uint8 mcuType;

    /* some mask number */
    const uint8 maskType;

    /* byte reserved for future use */
    const uint8 reserve;

    /* interface version number */
    const uint8 interface;

    /* flash initialize function */
    const tFlashFct flashInitFct;

    /* flash de-initialize function */
    const tFlashFct flashDeInitFct;

    /* flash erase function */
    const tFlashFct flashEraseFct;

    /* flash program function */
    const tFlashFct flashWriteFct;

} tFlash_InfoType;

/*=======[E X T E R N A L   D A T A]==========================================*/
extern const tFlash_InfoType *BLFlash_InfoPtr;

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
extern void tFlash_Init(tFlashParam *flashParam);
extern void tFlash_Deinit(tFlashParam *flashParam);
extern void tFlash_Erase(tFlashParam *flashParam);
extern void tFlash_Write(tFlashParam *flashParam);
extern void FlashReadMemory(uint8 *DataBuf, uint32 Addr, uint32 Length);
extern uint8 FlashReadByte(uint32 globalAddr);

#endif /* endof FLASH_H */

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20120909    Gary       Initial version
 *  V1.1    20180511    CChen      update
 */
/*=======[E N D   O F   F I L E]==============================================*/

