
/** Copyright (C) 2009-2018, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is 10086 property. Duplication

 *
 *  @file       <Flash.c>
 *  @brief      <Flash Driver For TC27x>
 *
 *  <The Driver based on the structure of the TC27x memory ,realize
 *   the flash erasing and programming method>
 *
 *  @author     <10086>
 *  @date       <2017-09-09>
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "fls.h"
#include "Fl.h"

/*=======[M A C R O S]========================================================*/
#if (STD_ON == FLS_USED)
/** data flash0 start address */
#define TFLASH_D0START_ADDR (uint32)0xAFE00000

/** data flash0 end address */
#define TFLASH_D0END_ADDR (uint32)0xAFE10000

/** data flash1 start address */
#define TFLASH_D1START_ADDR (uint32)0xAFE20000

/** data flash1 end address */
#define TFLASH_D1END_ADDR (uint32)0xAFE30000

/** data flash sector size */
#define TFLASH_DSECTOR_SIZE (uint32)0x00010000

#define TFLASH_PBASE_MASK (uint32)(~0x001FFFFF)

#define TFLASH_DBASE_MASK (uint32)(~0x0000FFFF)

/** code flash sector size */
#define FLASH_SECTOR_SIZE256 (uint32)0x00040000
#define FLASH_SECTOR_SIZE128 (uint32)0x00020000
#define FLASH_SECTOR_SIZE16 (uint32)0x00004000

#define FLASH0_FSR (*((volatile uint32 *)0xF8002010))

#define TFLS_CMD_ADDRESS1(ADD) (*((volatile uint32 *)(0x00005554 + (ADD))))
#define TFLS_CMD_ADDRESS2(ADD) (*((volatile uint64 *)(0x000055F0 + (ADD))))
#define TFLS_CMD_ADDRESS3(ADD) (*((volatile uint32 *)(0x0000AAA8 + (ADD))))

#endif

/** code flash start address */
#define TFLASH_PSTART_ADDR 0xA0000000U

/** code flash end address */
#define TFLASH_PEND_ADDR (uint32)0xA0400000U

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/** flash type */
#if (STD_ON == FLS_USED)
typedef enum
{
    /* code flash */
    TFLASH_P,

    /* data flash0 */
    TFLASH_D0,

    /* data flash1 */
    TFLASH_D1,

    /* other flash type */
    TFLASH_NON

} tFlash_Type;

/* flash state */
typedef enum
{
    /* busy */
    TFLASH_BUSY,

    /* free */
    TFLASH_FREE

} tFlash_State;
#endif

/*=======[E X T E R N A L   D A T A]==========================================*/
#if (STD_OFF == FLS_USED)
const tFlash_InfoType *BLFlash_InfoPtr = (tFlash_InfoType *)FL_DEV_BASE_ADDRESS;

#else

/** flash driver header define */
const tFlash_InfoType BLFlash_Info /*__at(0xd4003000ul)*/ =
    {
        TFLASH_DRIVER_VERSION_MCUTYPE,
        TFLASH_DRIVER_VERSION_MASKTYPE,
        0x00,
        TFLASH_DRIVER_VERSION_INTERFACE,
        &tFlash_Init,
        &tFlash_Deinit,
        &tFlash_Erase,
        &tFlash_Write,
};

const tFlash_InfoType *BLFlash_InfoPtr = &BLFlash_Info;

/*=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
__indirect STATIC tFlash_Type tFlash_AddrCheck(uint32 addr, uint32 length);

__indirect STATIC uint32 tFlash_AlignSector(uint32 addr, const tFlash_Type flashType);

__indirect STATIC tFlashResult tFlash_EraseExcute(const uint32 addr,
                                                  const tFlash_Type tFlashType,
                                                  uint32 *sectorSize, void (*wdgFunc)(void));

__indirect STATIC tFlashResult tFlash_WriteExcute(const uint32 addr,
                                                  const uint8 *data,
                                                  const tFlash_Type tFlashType,
                                                  const uint16 length);

__indirect STATIC tFlash_State tFlash_CheckState(tFlash_Type tFlashType);

/*=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/
/******************************************************************************/
/**
 * @brief               <flash initialize function>
 *
 * <process version check and flash register initialize> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <flashParam (IN/OUT)>
 * @return              <NONE>
 */
/******************************************************************************/
__indirect void tFlash_Init(tFlashParam *flashParam)
{
    /* check flash version */
    if ((FLASH_DRIVER_VERSION_PATCH == flashParam->patchLevel) &&
        (FLASH_DRIVER_VERSION_MINOR == flashParam->minorNumber) &&
        (FLASH_DRIVER_VERSION_MAJOR == flashParam->majorNumber))
    {

        flashParam->errorCode = kFlashOk;
    }
    else
    {
        flashParam->errorCode = kFlashFailed;
    }

    return;
}

/******************************************************************************/
/**
 * @brief               <flash de-initialize function>
 *
 * <process flash register de-initialize> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <flashParam (IN/OUT)>
 * @return              <NONE>
 */
/******************************************************************************/
__indirect void tFlash_Deinit(tFlashParam *flashParam)
{

    flashParam->data = NULL_PTR;
    flashParam->length = 0;
    flashParam->errorCode = kFlashOk;

    return;
}

/******************************************************************************/
/**
 * @brief               <flash erase function>
 *
 * <process flash erase for given address and length in parameter flashParam> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <flashParam (IN/OUT)>
 * @return              <NONE>
 */
/******************************************************************************/
__indirect void tFlash_Erase(tFlashParam *flashParam)
{
    tFlash_Type tFlashType;
    uint32 eraseAddr = flashParam->address;
    uint32 eraseLength = flashParam->length;
    uint32 sectorSize = 0;

    if (kFlashOk == flashParam->errorCode)
    {
        /* get flash type */
        tFlashType = tFlash_AddrCheck(flashParam->address, flashParam->length);

        if (TFLASH_NON != tFlashType)
        {
            /* get aligned earse address */
            eraseAddr -= tFlash_AlignSector(flashParam->address, tFlashType);

            /* get aligned earse length */
            eraseLength += tFlash_AlignSector(flashParam->address, tFlashType);

            /* set flash return code is ok */
            flashParam->errorCode = kFlashOk;

            while ((eraseLength > 0) && (kFlashOk == flashParam->errorCode))
            {
                /* process watch dog function */
                if (flashParam->wdTriggerFct != NULL_PTR)
                {
                    flashParam->wdTriggerFct();
                }

                /* erase flash */
                flashParam->errorCode = tFlash_EraseExcute(eraseAddr, tFlashType, &sectorSize, flashParam->wdTriggerFct);

                if (eraseLength > sectorSize)
                {
                    /* index erase address and length */
                    eraseAddr += sectorSize;
                    eraseLength -= sectorSize;
                }
                else
                {
                    /* erase end */
                    eraseLength = 0;
                }
            }
        }
        else
        {
            /* for other flash type, return code is error */
            flashParam->errorCode = kFlashFailed;
        }
    }
    return;
}

/******************************************************************************/
/**
 * @brief               <flash program function>
 *
 * <process flash program for given address, data point and length in parameter
 *  flashParam> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <flashParam (IN/OUT)>
 * @return              <NONE>
 */
/******************************************************************************/
__indirect void tFlash_Write(tFlashParam *flashParam)
{
    tFlash_Type flashType;
    uint32 writeAddr = flashParam->address;
    uint32 writeLength = flashParam->length;
    const uint8 *writeData = flashParam->data;
    uint32 phraseSize = 0;

    if (kFlashOk == flashParam->errorCode)
    {
        /* check flash type */
        flashType = tFlash_AddrCheck(writeAddr, flashParam->length);

        if (TFLASH_NON != flashType)
        {
            while ((writeLength > 0) && (kFlashOk == flashParam->errorCode))
            {
                /* process watch dog function */
                if (flashParam->wdTriggerFct != NULL_PTR)
                {
                    flashParam->wdTriggerFct();
                }

                /* get program length */
                if ((writeLength > 0x100u) && (TFLASH_P == flashType))
                {
                    phraseSize = 0x100u;
                    writeLength -= 0x100u;
                }
                else if ((writeLength > 0x80u) && ((TFLASH_D0 == flashType) || (TFLASH_D1 == flashType)))
                {
                    phraseSize = 0x80u;
                    writeLength -= 0x80u;
                }
                else
                {
                    phraseSize = writeLength;
                    writeLength = 0;
                }

                flashParam->errorCode = tFlash_WriteExcute(writeAddr, writeData,
                                                           flashType, (uint16)phraseSize);
                /* index program address and data point */
                writeAddr += phraseSize;
                writeData += phraseSize;
            }
        }
        else
        {
            /* for other flash type, return code is error */
            flashParam->errorCode = kFlashFailed;
        }
    }
    return;
}

/******************************************************************************/
/**
 * @brief               <check flash type>
 *
 * <check witch type of flash for given address and length> .
 * @param[in]           <addr (IN), length (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <tFlash_Type>
 */
/******************************************************************************/
__indirect STATIC tFlash_Type tFlash_AddrCheck(uint32 addr, uint32 length)
{
    tFlash_Type flashType = TFLASH_NON;

    if ((addr >= TFLASH_PSTART_ADDR) && ((addr + length) <= TFLASH_PEND_ADDR))
    {
        /* flash type is code falsh */
        flashType = TFLASH_P;
    }
    else if ((addr >= TFLASH_D0START_ADDR) && ((addr + length) <= TFLASH_D0END_ADDR))
    {
        /* flash type is data falsh0 */
        flashType = TFLASH_D0;
    }
    else if ((addr >= TFLASH_D1START_ADDR) && ((addr + length) <= TFLASH_D1END_ADDR))
    {
        /* flash type is data falsh0 */
        flashType = TFLASH_D1;
    }
    else
    {
        /* other flash type */
        flashType = TFLASH_NON;
    }
    return flashType;
}

/******************************************************************************/
/**
 * @brief               <get aligned length for flash sector>
 *
 * <get aligned length for given address and flashType> .
 * @param[in]           <addr (IN), flashType (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <uint32>
 */
/******************************************************************************/
__indirect STATIC uint32 tFlash_AlignSector(uint32 addr, const tFlash_Type flashType)
{
    uint32 alignLength = 0;
    if (TFLASH_P == flashType)
    {
        addr = addr - 0xA0000000;

        /* code flash sector size is 16KB */
        if (addr < 0x00020000)
        {
            alignLength = addr % FLASH_SECTOR_SIZE16;
        }
        else if (addr < 0x00040000)
        {
            alignLength = (addr - 0x00020000) % FLASH_SECTOR_SIZE128;
        }
        else if (addr < 0x00280000)
        {
            alignLength = (addr - 0x00040000) % FLASH_SECTOR_SIZE256;
        }
    }
    else if ((TFLASH_D0 == flashType) || (TFLASH_D1 == flashType))
    {
        /* data flash sector size is 256Byte */
        alignLength = (addr - 0xAFE00000) % TFLASH_DSECTOR_SIZE;
    }
    else
    {
        /* other flash type do nothing */
    }

    return alignLength;
}

/******************************************************************************/
/**
 * @brief               <check flash state>
 *
 * <check whitch state of flash for given tyep> .
 * @param[in]           <tFlashType(IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <tFlash_State>
 */
/******************************************************************************/
__indirect STATIC tFlash_State tFlash_CheckState(tFlash_Type tFlashType)
{
    tFlash_State tFlashState = TFLASH_BUSY;
    if (TFLASH_P == tFlashType)
    {
        if (0 == (FLASH0_FSR & 0x00000001))
        {
            tFlashState = TFLASH_FREE;
        }
    }
    else if (TFLASH_D0 == tFlashType)
    {
        if (0 == (FLASH0_FSR & 0x00000004))
        {
            tFlashState = TFLASH_FREE;
        }
    }
    else
    {
        if (0 == (FLASH0_FSR & 0x00000008))
        {
            tFlashState = TFLASH_FREE;
        }
    }
    return tFlashState;
}
/******************************************************************************/
/**
 * @brief               <excute flash earse>
 *
 * <excute flash earse or program> .
 * @param[in]           <addr (IN), tFlashType(IN), sectorSize(IN), length (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <tFlashResult>
 */
/******************************************************************************/
__indirect STATIC tFlashResult tFlash_EraseExcute(const uint32 addr,
                                                  const tFlash_Type tFlashType,
                                                  uint32 *sectorSize, void (*wdgFunc)(void))
{
    tFlashResult excuteState = kFlashOk;
    tFlash_State tFlashState = TFLASH_BUSY;
    uint32 pdAddr = 0;
    uint32 fsr = 0;
    uint16 i = 0;

    tFlashState = tFlash_CheckState(tFlashType);

    if (TFLASH_FREE == tFlashState)
    {
        if (tFlashType == TFLASH_P)
        {
            pdAddr = addr & TFLASH_PBASE_MASK;
        }
        else
        {
            pdAddr = addr & TFLASH_DBASE_MASK;
        }
        /* clear status register */
        TFLS_CMD_ADDRESS1(pdAddr) = 0x000000F5;

        /* erase physical sector */
        TFLS_CMD_ADDRESS1(pdAddr) = 0x000000AA;
        TFLS_CMD_ADDRESS3(pdAddr) = 0x00000055;
        TFLS_CMD_ADDRESS1(pdAddr) = 0x00000080;
        TFLS_CMD_ADDRESS1(pdAddr) = 0x000000AA;
        TFLS_CMD_ADDRESS3(pdAddr) = 0x00000055;

        if (tFlashType == TFLASH_P)
        {
            *((volatile uint32 *)(addr)) = 0x00000030;

            if (addr < 0xA0020000)

            {
                *sectorSize = FLASH_SECTOR_SIZE16;
            }
            else if (addr < 0xA0040000)
            {
                *sectorSize = FLASH_SECTOR_SIZE128;
            }
            else if (addr < 0xA0280000)
            {
                *sectorSize = FLASH_SECTOR_SIZE256;
            }
        }
        else
        {
            *((volatile uint32 *)(addr)) = 0x00000040;

            *sectorSize = TFLASH_DSECTOR_SIZE;
        }
        do
        {
            fsr = FLASH0_FSR;
            /* process watch dog function */
            if ((i == 100) && (wdgFunc != NULL_PTR))
            {
                wdgFunc();
                i = 0;
            }
            i++;

        } while (fsr & 0x0000000D);
    }
    else
    {
        /* excute error when flash operating is busy */
        excuteState = kFlashFailed;
    }

    return excuteState;
}
/******************************************************************************/
/**
 * @brief               <excute flash program>
 *
 * <excute flash earse or program> .
 * @param[in]           <addr (IN), data(IN), tFlashType(IN), length (IN)>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <tFlashResult>
 */
/******************************************************************************/
__indirect STATIC tFlashResult tFlash_WriteExcute(const uint32 addr,
                                                  const uint8 *data,
                                                  const tFlash_Type tFlashType,
                                                  const uint16 length)
{
    tFlashResult excuteState = kFlashOk;
    uint64 dataValue = 0;
    uint16 i = 0;
    tFlash_State tFlashState = TFLASH_BUSY;
    uint32 pdAddr = 0;
    uint32 fsr = 0;

    /* check if flash operating is busy */
    tFlashState = tFlash_CheckState(tFlashType);

    if (TFLASH_FREE == tFlashState)
    {

        if (tFlashType == TFLASH_P)
        {
            pdAddr = addr & TFLASH_PBASE_MASK;
            /* Enter Page Mode */
            TFLS_CMD_ADDRESS1(pdAddr) = 0x00000050;
        }
        else
        {
            pdAddr = addr & TFLASH_DBASE_MASK;
            /* Enter Page Mode */
            TFLS_CMD_ADDRESS1(pdAddr) = 0x0000005D;
        }

        /* Clear status register */
        TFLS_CMD_ADDRESS1(pdAddr) = 0x000000F5;

        if (tFlashType == TFLASH_P)
        {
            /* Enter Page Mode */
            TFLS_CMD_ADDRESS1(pdAddr) = 0x00000050;
        }
        else
        {
            /* Enter Page Mode */
            TFLS_CMD_ADDRESS1(pdAddr) = 0x0000005D;
        }
        /* Load Page */
        while (i < length)
        {
            dataValue = (uint64)(data[i] & 0xFF);
            dataValue += ((uint64)(data[i + 1] & 0xFF) << 8);
            dataValue += ((uint64)(data[i + 2] & 0xFF) << 16);
            dataValue += ((uint64)(data[i + 3] & 0xFF) << 24);
            dataValue += ((uint64)(data[i + 4] & 0xFF) << 32);
            dataValue += ((uint64)(data[i + 5] & 0xFF) << 40);
            dataValue += ((uint64)(data[i + 6] & 0xFF) << 48);
            dataValue += ((uint64)(data[i + 7] & 0xFF) << 56);

            TFLS_CMD_ADDRESS2(pdAddr) = dataValue;
            i += 8;
        }
        /* write page */
        TFLS_CMD_ADDRESS1(pdAddr) = 0x000000AA;
        TFLS_CMD_ADDRESS3(pdAddr) = 0x00000055;
        TFLS_CMD_ADDRESS1(pdAddr) = 0x000000A0;
        (*((volatile uint32 *)addr)) = 0x000000AA;

        do
        {
            fsr = FLASH0_FSR;
        } while (fsr & 0x0000000D);
    }
    else
    {
        /* excute error when flash operating is busy */
        excuteState = kFlashFailed;
    }
    return excuteState;
}
#endif

/******************************************************************************/
/*
 * Brief:               FlashReadByte
 * ServiceId:           None
 * Sync/Async:          Synchronous
 * Reentrancy:          Non Reentrant
 * Param-Name[in]:      address
 * Param-Name[out]:     None
 * Param-Name[in/out]:  address
 * Return:              None
 * PreCondition:        --
 * CallByAPI:           --
 */
/******************************************************************************/
uint8 FlashReadByte(uint32 globalAddr)
{
    uint8 readData = 0;

    if ((globalAddr & TFLASH_PSTART_ADDR) == TFLASH_PSTART_ADDR)
    {
        /* read data in local address */
        readData = *(uint8 *)globalAddr;
    }

    return readData;
}
/******************************************************************************/
/*
 * Brief               Read Flash Memory data
 * ServiceId           None
 * Sync/Async          Synchronous
 * Reentrancy          Non Reentrant
 * Param-Name[in]      Addr, Length
 * Param-Name[out]     None
 * Param-Name[in/out]  None
 * Return              None
 * PreCondition        None
 * CallByAPI           FlashReadMemory
 */
/******************************************************************************/
void FlashReadMemory(uint8 *DataBuf, uint32 Addr, uint32 Length)
{
    while (Length > 0)
    {
        *DataBuf = FlashReadByte(Addr);
        Addr++;
        DataBuf++;
        Length--;
    }
}

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20120909    Gary       Initial version
 */
/*=======[E N D   O F   F I L E]==============================================*/

