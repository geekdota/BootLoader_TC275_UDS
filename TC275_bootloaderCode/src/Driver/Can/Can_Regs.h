/*============================================================================*/
/*  Copyright (C) 2009-2018, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is 10086 property. Duplication
 *  or disclosure without 10086 written authorization is prohibited.
 *
 *
 *  @file       <Can.h>
 *  @brief      <Can Registers define>
 *
 * <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <10086>
 *  @date       <03-18-2018>
 */
/*============================================================================*/
#ifndef CAN_REGS_H
#define CAN_REGS_H
/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/*  <VERSION>    <DATE>    <AUTHOR>    <REVISION LOG>
 *  V1.0.0       20180318  10086   Initial version
 */
/*============================================================================*/

/*=======[V E R S I O N  I N F O R M A T I O N]===============================*/
#define CAN_REG_H_AR_MAJOR_VERSION 2
#define CAN_REG_H_AR_MINOR_VERSION 4
#define CAN_REG_H_AR_PATCH_VERSION 0
#define CAN_REG_H_SW_MAJOR_VERSION 1
#define CAN_REG_H_SW_MINOR_VERSION 0
#define CAN_REG_H_SW_PATCH_VERSION 1

/*=======[I N C L U D E S]====================================================*/
#include "comstack_types.h"
#include "can_cfg.h"
/*=======[M A C R O S]========================================================*/

/* Enable CAN module */
#define CAN_ENABLE_MODLE 0

/* State of CAN diable*/
#define CAN_DISABLE_STATE 0x2U

/* Panel busy flag */
#define CAN_PANEL_BUSY 0x100U

/* Disable can node and enbale configuration change */
#define CAN_NODE_DISABLE 0x41U

/* Port control register initial value */
#define CAN_PORT_REG_VALUE 1 /* set the P20.7 & P20.8 as the CAN0 TX&RX Henry2017.05.06 */

/* Error control register initial value */
#define CAN_ERROR_REG_VALUE 0x600000U

/* CAN frame counter initial value */
#define CAN_COUNTER_REG_VALUE 0

/* Reseive Mb value */
#define CAN_RX_MB_SET 0xA00000U

/* Reseive Mb reset value */
#define CAN_RX_MB_RESET 0xA0U

/* Transmit Mb value */
#define CAN_TX_MB_SET 0xE080000U

/* Transmit Mb reset value */
#define CAN_TX_MB_RESET 0xE08U

/* Reset/Set Message Valid */
#define CAN_MSG_VALID 0x200000U

/* Rx SRN base number */
#define CAN_RX_SRN_BASE 0x4U

/* Tx SRN base number */
#define CAN_TX_SRN_BASE 0x7U

/* Enabel rx interrupt of mb */
#define CAN_MB_RX_INT_ENABLE 0x10000U

/* Enabel tx interrupt of mb */
#define CAN_MB_TX_INT_ENABLE 0x20000U

/* Enable Mask IDE */
#define CAN_ENABLE_MIDE 0x20000000U

/* IDE for extended ID */
#define CAN_MBID_ID_EXTENDED 0x20000000U

/* IDE for standard ID */
#define CAN_MBID_ID_STANDARD 0x1FFC0000U

/* priority class is CAN ID */
#define CAN_PRI_CLASS_ID 0x80000000U

/* Allocate message object to a list statically */
#define CAN_PANCMD_STATIC_ALLOCATE 0x2U
#define CAN_PANCMD_INIT_LIST 0x1U

/* Set start mode */
#define CAN_SET_START_MODE 0xFFFFFFBEU

/* Set stop mode */
#define CAN_SET_STOP_MODE 0x1U

/* Enabel transfer interrupt */
#define CAN_ENABLE_TR_INT 0x2U

/* Enabel busoff interrupt */
#define CAN_ENABLE_BUSOFF_INT 0x8U

/* Data length */
#define CAN_DATA_LENGTH 0x8U

/* IDE bit, 0=standard ID, 1=extended ID */
#define CAN_IDE 0x20000000U

/* Set TX request */
#define CAN_SET_TXRQ 0x7000000U

/* DLC (data length code) */
#define CAN_MBCS_LENGTH 0xF000000U

/* Bus off interrupt flag */
#define CAN_ESR_BOFFINT 0x80U

/* Bus off clear */
#define CAN_BUSOFF_CLEAR 0xFFFFFF5FU

/* LEC flag */
#define CAN_ESR_LEC 0x5U

/* Normal divider mode */
#define CAN_DIVDER_MODE 0x4U

/* TX OK */
#define CAN_TX_OK 0x8U

/* RX OK */
#define CAN_RX_OK 0x10U

/* Mb Tx OK */
#define CAN_MB_TX_MASK 0x2U

/* Mb Rx OK */
#define CAN_MB_RX_MASK 0x1U

/* New data flag */
#define CAN_RES_NEWDAT_MASK 0x8U

/* Message lost flag */
#define CAN_RES_MSGLST_MASK 0x10U

/* MB is invalid */
#define CAN_RES_MSGVAL 0x20U

/* CAN SRC base address */
#define CAN_SRC_BASE_ADDR (CAN_BASE_ADDR + 0xC0U)

/* CAN SRC address offset */
#define CAN_SRC_ADDR_OFFSET 0x4U

/* CAN Mb address offset */
#define CAN_MB_ADDR_OFFSET 0x20U

/* CAN_MSPND */
#define CAN_MSPND0 0x0U
#define CAN_MSPND1 0x20U
#define CAN_MSPND2 0x40U
#define CAN_MSPND3 0x60U
#define CAN_MSPND_MASK 0xE0U

#define CAN_CLC_REG (*((uint32 volatile *)CAN_BASE_ADDR))
#define CAN_FDR_REG (*((uint32 volatile *)(CAN_BASE_ADDR + 0xCU)))
#define CAN_PANCTR_REG (*((uint32 volatile *)(CAN_BASE_ADDR + 0x1C4U)))

#define CAN_MCR_REG (*((uint32 volatile *)(CAN_BASE_ADDR + 0x1C8U)))

#define CAN_MSPND_0 (*((uint32 volatile *)(CAN_BASE_ADDR + 0x140U)))
#define CAN_MSPND_1 (*((uint32 volatile *)(CAN_BASE_ADDR + 0x144U)))
#define CAN_MSPND_2 (*((uint32 volatile *)(CAN_BASE_ADDR + 0x148U)))
#define CAN_MSPND_3 (*((uint32 volatile *)(CAN_BASE_ADDR + 0x14CU)))

#define P3_IOCR0 (*(volatile uint32 *)0xf0000f10U)
#define P20_IOCR4 (*(volatile uint32 *)0xf003C014U)
#define P20_IOCR8 (*(volatile uint32 *)0xf003C018U)
#define P3_OUT (*(volatile uint32 *)0xf0000f00U)
#define P3_IOCR12 (*(volatile uint32 *)0xf0000f1cU)
#define P4_IOCR0 (*(volatile uint32 *)0xF0001010U)
#define P14_IOCR0 (*(volatile uint32 *)0xF003B410u)

/* CAN Node Registers */
typedef struct
{
    uint32 Ncr;

    uint32 Nsr;

    uint32 Nipr;

    uint32 Npcr;

    uint32 Nbtr;

    uint32 Necnt;

    uint32 Nfcr;

} Can_NodeRegType;

/* CAN Node Registers */
typedef struct
{
    uint32 Mofcr;

    uint32 Mofgpr;

    uint32 Moipr;

    uint32 Moamr;

    uint32 Modata[TWO];

    uint32 Moar;

    uint32 Mctstr;

} Can_MbRegType;

/* CAN SRC Registers */
typedef struct
{
    uint32 reserved0[EIGHT];

    uint32 TXI;

    uint32 reserved1[TWO];

    uint32 RXI;

    uint32 reserved2[TWO];

    uint32 BOFI;

    uint32 reserved3;

} Can_SrcRegType;

#endif /* #ifndef  CAN_REGS_H */

/*=======[E N D   O F   F I L E]==============================================*/

