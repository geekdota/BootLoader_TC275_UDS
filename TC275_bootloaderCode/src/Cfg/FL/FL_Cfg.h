#ifndef FL_CFG_H
#define FL_CFG_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *    V1.0    20121227   Gary       Initial version
 *
 *    V1.1    20161015  cywang      update
 *
 *    V1.2    20180522  CChen       updata
 */
/*============================================================================*/
#include "std_types.h"

/*=======[M A C R O S]========================================================*/
/** The physical memory location of boot request flag. LOCAL address */
/* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
#define FL_BOOT_MODE     0xc0005800U

/** The physical memory location of application software update flag. LOCAL address */
/* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
#define FL_APPL_UPDATE   0xc0005804U

/** The physical memory location of boot default session from prog. flag. LOCAL address */
/* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
#define FL_BOOT_DEFAULT  0xc0005808U

/** Sleep time count:if configured 0,then no sleep function. */
/* @type:uint16 range:0~65535 note:unit ms */
#define FL_SLEEP_TIMER   1000

/** Totol num of program blocks. */
/* @type:uint8 range:1~255 note:reference to num of FL_BlkInfo */
#define FL_NUM_LOGICAL_BLOCKS                (uint8)5
/* Added by fast project Custom requirements. */
#define FL_NUM_NVRAM_BLOCK_LOCATION_IN_TAB   (uint8)6

/** Maximum of segment in one block. */
/* @type:uint8 range:1~255 note:NONE */
#define FL_MAX_SEGMENTS                      (uint8)60

/** Value for fill gap,if configured 0xFF,then not execute. */
/* @type:uint8 range:0x00~0xFF note:NONE */
#define FL_GAP_FILL_VALUE                    0xFFU

/* @type:define range:NONE note:auto generate */
#if (0xFFU == FL_GAP_FILL_VALUE)
#define FL_USE_GAP_FILL STD_OFF
#else
#define FL_USE_GAP_FILL STD_ON
#endif

/** Start address of flash driver in RAM. LOCAL address */
/* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
#define FL_DEV_BASE_ADDRESS  0xC0003000U

/** Length of flash driver in RAM. */
/* @type:uint32 range:0x00000000~0xFFFFFFFF note:NONE */
#define FL_DEV_SIZE 0x2800U

/* not standard configured parameter */
/** Application RCHW address */
/* @type:uint32 range:0x0000C000~0xFFFFFFFF note:NONE */
#define FL_APP_RCHW_ADDR    0xa0080000U

/** timer delay in bootloader when ECU is startup */
/* @type:uint16 range:0~65535 note:unit ms */
#define FL_MODE_STAY_TIME    200

/** Length of repair shop code */
/* @type:uint8 range:1~255 note:NONE */
#define FL_REPAIR_SHOP_CODE_LENGTH 0x10U

/** Length of finger print information */
/* @type:uint16 range:1~65535 note:NONE */
#define FL_FINGER_PRINT_LENGTH     0x09U
#define FL_FINGER_PRINT_OFFSET     0x04U

/** bootloader information global address in NVM */
/* @type:uint32 range:NONE note:auto generate */
#define FL_NVM_INFO_ADDRESS  0xAF000000U

/** code flash size that should program once time */
/* @type:uint32 range:NONE note:auto generate */
#define FL_PROGRAM_SIZE    0x100U

#define FL_BLOCK_INFO_SIZE      18

#define FL_REF_NUM_LENGTH		10



#define FL_CHECKSUM_OFFSET	0xCu
#endif/* endof FL_CFG_H */

/*=======[E N D   O F   F I L E]==============================================*/
