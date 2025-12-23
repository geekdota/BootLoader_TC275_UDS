/*============================================================================*/
/*  Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 *  @file       <Can.c>
 *  @brief      <Can driver Module source file>
 *
 * <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <jianan.liu>
 *  @date       <03-18-2018>
 */
/*============================================================================*/

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/*  <VERSION>    <DATE>    <AUTHOR>    <REVISION LOG>
 *  V1.0.0       20180318  jianan.liu   Initial version
 *                                      these features not support in this version:
 *                                      1. multiple Can Drivers.
 *                                      2. Can transceiver.
 *                                      3. Sleep and weakup
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "can.h"
#include "canif.h"
#include "can_regs.h"
#include "mcu.h"
#include "STM_cfg.h"
#include "appl.h"
#include "wdtcon.h"
/*=======[M A C R O S]========================================================*/
/* Can Hardware Number of Mailboxes */
#define CAN_HW_MAX_MAILBOXES         64

/* DEM Report function empty */
#if (STD_OFF == CAN_DEM_ERROR_DETECT)
#define Dem_ReportErrorStatus(eventId, eventStatus)   do{}while(0)
#endif


#define CAN_CNTRL_CFG(controller)         (Can_ControllerConfigData[controller])
#define CAN_HWOBJ_CFG(mbId)               (Can_HardwareObjectConfigData[mbId])
#define CAN_HWOBJ_ID(hth)                 (Can_HohConfigData[hth])
#define CAN_HWOBJ_NUM                     CAN_MAX_HARDWAREOBJECTS
#define CAN_HOH_NUM                       CAN_MAX_HOHS


/* CCU Clock Control Register 1 */
//#define BL_SCU_CCUCON1               (*((uint32 volatile *) 0xF0036034U))


#define CAN_CNTRL_HOH_NUM(controller)     (uint8)(CAN_CNTRL_CFG(controller).CanRxHwObjCount + CAN_CNTRL_CFG((controller)).CanTxHwObjCount)

/* MISRA RULE 11.3:303 VIOLATION: Hardware register address operation */
/* Controller Register */
#define CAN_CNTRL_REG(controller) \
    ((volatile Can_NodeRegType *)(Can_ControllerPCConfigData[controller].CanControllerBaseAddr))

/* Mb Register */
#define CAN_MB_REG(mb) ((volatile Can_MbRegType *)(CAN_MB_BASE_ADDR + ((mb) * CAN_MB_ADDR_OFFSET)))

/* SRC Register */
#define CAN_SRC_REG(controller) ((volatile Can_SrcRegType *)(CAN_SRC_BASE_ADDR - ((controller) * CAN_SRC_ADDR_OFFSET)))
/* controller PC config */
#define CAN_CNTRL_PCCFG(controller)  (Can_ControllerPCConfigData[controller])

/* hardware object ID to mailbox ID */
#define CAN_MB_ID(hwObjId)  \
    ((uint8)((hwObjId) - CAN_CNTRL_CFG(CAN_HWOBJ_CFG(hwObjId).CanControllerRef).CanRxHwObjFirst))

/* mailbox ID to hardware object ID */
#define CAN_MBID_TO_HWOBJ(Controller, mbId) \
    ((uint8)((mbId) + CAN_CNTRL_CFG(Controller).CanRxHwObjFirst))

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
typedef enum
{
    CAN_CS_UNINT = 0,

    CAN_CS_STOPPED = 1,

    CAN_CS_STARTED = 2,

    CAN_CS_SLEEP = 3

} Can_ControllerModeType;

/* Controller Runtime Structure */
typedef struct
{
	/* controller mode */
    Can_ControllerModeType CntrlMode;

    uint32 IntLockCount;

    /* Transmit PDU handles for TxConfirmation call back to CANIF */
    PduIdType TxPduHandles[CAN_HW_MAX_MAILBOXES];

} Can_ControllerStatusType;

/*=======[I N T E R N A L   D A T A]==========================================*/
/* Global Config Pointer */
STATIC const Can_ConfigType* Can_GlobalConfigPtr;

/* Controller Runtime structure */
STATIC Can_ControllerStatusType Can_Cntrl[CAN_MAX_CONTROLLERS] =
{
    {
        CAN_CS_UNINT,
        0,
        {
            0
        }
    }
};

/*=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
/* Mode Control */
STATIC Can_ReturnType Can_StartMode(uint8 Controller);
STATIC Can_ReturnType Can_StopMode(uint8 Controller);
STATIC Can_ReturnType Can_SleepMode(uint8 Controller);

STATIC void Can_InitHwCntrl(uint8 Controller, const Can_ControllerConfigType* Config);
STATIC void Can_InitMB(uint8 Controller);
STATIC void Can_DeInitMB(uint8 Controller);

STATIC void Can_TxProcess(uint8 Controller);
STATIC void Can_RxProcess(uint8 Controller);

#if (STD_ON == CAN_HW_TRANSMIT_CANCELLATION)
STATIC void Can_TxCancel(uint8 HwObjId, uint8 Controller);
#endif /* STD_ON == CAN_HW_TRANSMIT_CANCELLATION */

STATIC void Can_BusOff_Handler(uint8 Controller);

#if (STD_ON == CAN_MULTIPLEXED_TRANSMISSION)
STATIC uint8 Can_FindLowPriorityMb(uint8 Hth);
#endif /* STD_ON == CAN_MULTIPLEXED_TRANSMISSION */

STATIC boolean Can_IsTxMbFree(uint8 HwObjId);
STATIC void Can_WriteMb(uint8 HwObjId, const Can_PduType* PduInfo);
#if ((STD_ON == CAN_HW_TRANSMIT_CANCELLATION) || (STD_ON == CAN_MULTIPLEXED_TRANSMISSION))
STATIC uint32 Can_GetMBCanId(uint8 HwObjId);
#endif
STATIC void Can_GetMBInfo(uint8 HwObjId, Can_PduType* pdu);

#if ((STD_ON == CAN_HW_TRANSMIT_CANCELLATION) || (STD_ON == CAN_MULTIPLEXED_TRANSMISSION))
STATIC boolean Can_PriorityHigher(Can_IdType destId, Can_IdType srcId);
#endif /* (STD_ON==CAN_HW_TRANSMIT_CANCELLATION)||(STD_ON==CAN_MULTIPLEXED_TRANSMISSION) */

STATIC void Can_DisableInt(uint8 Controller);

STATIC void Can_StartStateEnableInt(uint8 Controller);

STATIC void Can_ResetPnd(uint8 HwObjId);

STATIC Can_ReturnType Can_WakeupMode(uint8 Controller);

STATIC void Can_InitPort(uint8 Controller);

/*=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/

/******************************************************************************/
/*
 * Brief               <This function initializes the CAN driver>
 * ServiceId           <0x00>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-Name[in]      <Config:Pointer to driver configuration>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
void Can_Init(const Can_ConfigType* Config)
{
    uint8  controller = 0;
    uint32 timeOut = 0;

    /* backup config pointer */
    Can_GlobalConfigPtr = Config;

    /*@req <CAN245> init each controller */
    for (controller = 0; controller < CAN_MAX_CONTROLLERS; controller++)
    {
        Can_Cntrl[controller].IntLockCount = 0UL;
        Can_Cntrl[controller].CntrlMode = CAN_CS_STOPPED; /* @req <CAN259> */
    }

    /*unlock and then lock, CCUCON1 enable*/
	unlock_safety_wdtcon();
	while(0U != (BL_SCU_CCUCON1 >> 31));
	{   /*Wait till ccucon registers can be written with new value */
		/*No "timeout" required, because if it hangs, Safety Endinit will give a trap */
	}
	BL_SCU_CCUCON1 &= 0xFFFFFFF0U;
	BL_SCU_CCUCON1 |= 0x50000002U;
	lock_safety_wdtcon();

     timeOut = CAN_TIMEOUT_DURATION;

   /*unlock and then lock, Enable Can module reg */
     unlock_wdtcon();
    /* Enable Can module, CAN_CLC->DISR =0 enable the CAN clock, */
    CAN_CLC_REG = CAN_ENABLE_MODLE;    
    /*confirm whether the CAN clock is enable via CAN_CLC_DISS*/
    while ((timeOut > 0UL) && (CAN_DISABLE_STATE == (CAN_CLC_REG & CAN_DISABLE_STATE)))
    {
        timeOut--;
    }

    if (0UL == timeOut)
    {
        lock_wdtcon();
        Dem_ReportErrorStatus(0, 0);
    }
    else
    {
        /* Set Divider */
        CAN_FDR_REG = (uint32)((CAN_DIVDER_MODE << 12) | CAN_STEP);

        timeOut = CAN_TIMEOUT_DURATION;
        while ((timeOut > 0UL) && (CAN_PANEL_BUSY == (CAN_PANCTR_REG & CAN_PANEL_BUSY)))
        {
            timeOut--;
        }
        lock_wdtcon();
        if (0UL == timeOut)
        {
            Dem_ReportErrorStatus(0, 0);
        }		
       CAN_MCR_REG = 0x0;
	/*select fasysn as fcan*/
       CAN_MCR_REG = 0x1;		
    }
    return;
}

/******************************************************************************/
/*
 * Brief               <This function initializing only Can controller specific settings>
 * ServiceId           <0x02>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-Name[in]      <Controller-CAN controller to be initialized>
 * Param-Name[out]     <Config-pointer to controller configuration>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
void Can_InitController(uint8 Controller, const Can_ControllerConfigType* Config)
{
    Can_InitHwCntrl(Controller, Config);

    Can_Cntrl[Controller].CntrlMode = CAN_CS_STOPPED;  /* @req <CAN256> */

    return;
}

/******************************************************************************/
/*
 * Brief               <This function performs software triggered state transitions 
 *                         of the CAN controller State machine.>
 * ServiceId           <0x03>
 * Sync/Async          <Asynchronous>
 * Reentrancy          <Non Reentrant>
 * Param-Name[in]      <Controller-CAN controller for which the status shall be changed>
 * Param-Name[in]      <Transition-Possible transitions>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <Can_ReturnType-CAN_OK or CAN_NOT_OK>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
/*@req <CAN230> @req <CAN017> */
Can_ReturnType Can_SetControllerMode(uint8 Controller, Can_StateTransitionType Transition)
{
    Can_ReturnType ret = CAN_NOT_OK;

    switch (Transition)
    {
    case CAN_T_START:
        ret = Can_StartMode(Controller);
        break;

    case CAN_T_STOP:
        ret = Can_StopMode(Controller);
        break;

    case CAN_T_SLEEP:
        ret = Can_SleepMode(Controller);
        break;

    case CAN_T_WAKEUP:
        ret = Can_WakeupMode(Controller);
        break;

    default:
        ret = CAN_NOT_OK;
        break;
    }

    return ret;
}

/******************************************************************************/
/*
 * Brief               <This function disable all interrupt for this controller. >
 * ServiceId           <0x04>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Reentrant>
 * Param-Name[in]      <Controller- CAN controller for which interrupts shall be disabled>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
/*@req <CAN0231>*/
void Can_DisableControllerInterrupts(uint8 Controller)
{
    /* Disable interrupt */
    Can_DisableInt(Controller);

    Can_Cntrl[Controller].IntLockCount++;

    return;
}

/******************************************************************************/
/*
 * Brief               <This function enable all allowed interrupts. >
 * ServiceId           <0x05>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Reentrant>
 * Param-Name[in]      <Controller- CAN controller for which interrupts shall be disabled>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
/* @req <CAN0232> @req <CAN050> */
void Can_EnableControllerInterrupts(uint8 Controller)
{
    /*@req <CAN0209>*/
    /*@req <CAN0210>*/
    if (CAN_CS_STARTED == Can_Cntrl[Controller].CntrlMode)
    {
        /* @req <CAN0209> enable interrupt call before disable interrupt no action */
        if (Can_Cntrl[Controller].IntLockCount > 0UL)
        {
            Can_Cntrl[Controller].IntLockCount--;

            if (0UL == Can_Cntrl[Controller].IntLockCount)
            {
                Can_StartStateEnableInt(Controller);
            }
        }
    }

    return;
}

/******************************************************************************/
/*
 * Brief               <This function perform HW-Transmit handle transmit. >
 * ServiceId           <0x06>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Reentrant>
 * Param-Name[in]      <Hth-information which HW-transmit handle shall be used for transmit.>
 * Param-Name[in]      <PduInfo-Pointer to SDU user memory,DLC and Identifier>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <Can_ReturnType-Returns CAN_OK,CAN_NOT_OK or CAN_BUSY>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
/* @req <CAN276> @req <CAN0233> @req <CAN0212> @req <CAN0275> */
Can_ReturnType Can_Write(uint8 Hth, const Can_PduType* PduInfo)
{
    Can_ReturnType ret = CAN_NOT_OK;
    uint8          controller;
    uint8          hwObjId = 0;

    hwObjId = CAN_HWOBJ_ID(Hth);
    controller = CAN_HWOBJ_CFG(hwObjId).CanControllerRef;

    if (CAN_CS_STARTED == Can_Cntrl[controller].CntrlMode)
    {
#if (STD_ON == CAN_MULTIPLEXED_TRANSMISSION)
        hwObjId = Can_FindLowPriorityMb(Hth);
#endif /* STD_ON == CAN_MULTIPLEXED_TRANSMISSION */

        if (TRUE == Can_IsTxMbFree(hwObjId))
        {
            Can_WriteMb(hwObjId, PduInfo);
            ret = CAN_OK;
        }
        else
        {
#if (STD_ON == CAN_HW_TRANSMIT_CANCELLATION)
            if (TRUE == Can_PriorityHigher(PduInfo->id, Can_GetMBCanId(hwObjId)))
            {
                Can_TxCancel(hwObjId, controller);
            }
#endif /* STD_ON == CAN_HW_TRANSMIT_CANCELLATION */

            ret = CAN_BUSY;
        }
    }

    return ret;
}

/******************************************************************************/
/*
 * Brief               <This function checks if a wakeup has occurred for the given controller. >
 * ServiceId           <0x0b>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-Name[in]      <Controller- Controller to be checked for a wakeup>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <Std_ReturnType- Returns-E_OK or E_NOT_OK>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
Std_ReturnType Can_Cbk_CheckWakeup(uint8 Controller)
{
    Std_ReturnType ret = (uint8)E_NOT_OK;
    if(Controller != 0) /* daizhunsheng add */
    {
    	/* do nothing */
    }
    return ret;
}

/******************************************************************************/
/*
 * Brief               <This function performs the polling of TX confirmation and TX cancellation 
 *                          confirmation when.CAN_TX_PROCESSING is set to POLLING. >
 * ServiceId           <0x01>
 * Param-Name[in]      <None>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
/* @req <CAN0225> @req <CAN031> */
void Can_MainFunction_Write(void)
{
#if (STD_ON == CAN_TX_POLLING)
    uint8 Controller;

    /* loop each Controller TX confirmation and TX cancel confirmation */
    for (Controller = 0; Controller < CAN_MAX_CONTROLLERS; Controller++)
    {
        /*@req <CAN178>*/
        if ((CAN_PROCESS_TYPE_POLLING == CAN_CNTRL_PCCFG(Controller).CanTxProcessing)
         && (CAN_CS_STARTED == Can_Cntrl[Controller].CntrlMode))
        {
            Can_TxProcess(Controller);
        }
    }
#endif /* STD_ON == CAN_TX_POLLING */

    return;
}

/******************************************************************************/
/*
 * Brief               <This function performs the polling of RX indications when 
 *                          CAN_RX_PROCESSING is set to POLLING.> 
 * ServiceId           <0x08>
 * Param-Name[in]      <None>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
 /*@req <CAN012>
[heguarantee that neither the ISRs 
 nor the function Can_MainFunction_Read can be interrupted by itself. ]
*/
/* @req <CAN226> @req <CAN108> @req <CAN180> */
void Can_MainFunction_Read(void)
{
#if (STD_ON == CAN_RX_POLLING)
    uint8 controller;

    /* scan each Controller */
    for (controller = 0; controller < CAN_MAX_CONTROLLERS; controller++)
    {
        if ((CAN_PROCESS_TYPE_POLLING == CAN_CNTRL_PCCFG(controller).CanRxProcessing)
         && (CAN_CS_STARTED == Can_Cntrl[controller].CntrlMode))
        {
            Can_RxProcess(controller);
        }
    }
#endif /* STD_ON == CAN_RX_POLLING */

    return;
}

/******************************************************************************/
/*
 * Brief               <This function performs the polling of bus-off events that are configured
 *                          statically as "to be polled".> 
 * ServiceId           <0x09>
 * Param-Name[in]      <None>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
void Can_MainFunction_BusOff(void)
{
#if (STD_ON == CAN_BUSOFF_POLLING)
    volatile Can_NodeRegType* canRegs;
    uint8                     controller;

    for (controller = 0; controller < CAN_MAX_CONTROLLERS; controller++)
    {
        if ((CAN_PROCESS_TYPE_POLLING == CAN_CNTRL_PCCFG(controller).CanBusOffProcessing)
         && (CAN_CS_STARTED == Can_Cntrl[controller].CntrlMode))
        {
            canRegs = CAN_CNTRL_REG(controller);
            if (CAN_ESR_BOFFINT == (CAN_ESR_BOFFINT & canRegs->Nsr))
            {
                Can_BusOff_Handler(controller);
            }
        }
    }
#endif /* STD_ON == CAN_BUSOFF_POLLING */

    return;
}

/******************************************************************************/
/*
 * Brief               <This function performs the polling of wake-up events that are configured
 *                          statically as "to be polled".> 
 * ServiceId           <0x0a>
 * Param-Name[in]      <None>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
void Can_MainFunction_Wakeup(void)
{
    return;
}

/*=======[I N T E R N A L  F U N C T I O N   I M P L E M E N T A T I O N S]====================*/

/******************************************************************************/
/*
 * Brief               <This function performs bus-off  process>
 * Param-Name[in]      <Controller- CAN controller to be Tw/Rw/bus-off process>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC void Can_BusOff_Handler(uint8 Controller)
{
    volatile Can_NodeRegType* canRegs;
    Can_ReturnType            ret_val = CAN_NOT_OK;

    canRegs = CAN_CNTRL_REG(Controller);

    if (CAN_ESR_BOFFINT == (canRegs->Nsr & CAN_ESR_BOFFINT))
    {
    	ret_val = Can_StopMode(Controller);
        if (CAN_OK == ret_val)
        {
            CanIf_ControllerBusOff(Controller);
        }
        canRegs->Nsr &= CAN_BUSOFF_CLEAR;
        canRegs->Ncr &= 0xFFFFFFFEU;
    }
    else
    {
        canRegs->Nsr = CAN_ESR_BOFFINT;
    }

    return;
}

/******************************************************************************/
/*
 * Brief               <This function performs Tx confirmation and Tx cancellation process>
 * Param-Name[in]      <Controller- CAN controller to be Tx process>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC void Can_TxProcess(uint8 Controller)
{
    volatile Can_NodeRegType* canRegs;
    volatile Can_MbRegType*   mbRegs;
    uint8                     hwObjId = 0;
    uint8                     endHwObjId = 0;

    canRegs = CAN_CNTRL_REG(Controller);

    if (CAN_TX_OK == (canRegs->Nsr & CAN_TX_OK))
    {
        /* Clear tx successful flag */
        canRegs->Nsr &= ~CAN_TX_OK;

        endHwObjId = (uint8)(CAN_CNTRL_CFG(Controller).CanTxHwObjFirst
                           + CAN_CNTRL_CFG(Controller).CanTxHwObjCount);

        for (hwObjId = CAN_CNTRL_CFG(Controller).CanTxHwObjFirst; hwObjId < endHwObjId; hwObjId++)
        {
            mbRegs = CAN_MB_REG(hwObjId);
            if (CAN_MB_TX_MASK == (mbRegs->Mctstr & CAN_MB_TX_MASK))
            {
                /* Clear new data flag */
                mbRegs->Mctstr = CAN_RES_NEWDAT_MASK;

                /* Reset TXPND */
                mbRegs->Mctstr = CAN_MB_TX_MASK;

                /* Reset MSGVAL */
                mbRegs->Mctstr = CAN_RES_MSGVAL;

                CanIf_TxConfirmation(Can_Cntrl[Controller].TxPduHandles[hwObjId]);
            }

            /* Reset PND bit */
            Can_ResetPnd(hwObjId);
        }
    }

    return;
}

/******************************************************************************/
/*
 * Brief               <This function performs Rx indications  process>
 * Param-Name[in]      <Controller- CAN controller to be Rx process>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC void Can_RxProcess(uint8 Controller)
{
    volatile Can_NodeRegType* canRegs;
    volatile Can_MbRegType*   mbRegs;
    uint8                     hwObjId = 0;
    uint8                     endHwObjId = 0;
    Can_PduType               pdu;
    uint8                     rxData[EIGHT];
    /* uint16 					  mbId = 0; *//*daizhunsheng do */

    canRegs = CAN_CNTRL_REG(Controller);

    if (CAN_RX_OK == (canRegs->Nsr & CAN_RX_OK))
    {
        /* Clear tx successful flag */
        canRegs->Nsr &= ~CAN_RX_OK;

        endHwObjId = (uint8)(CAN_CNTRL_CFG(Controller).CanRxHwObjFirst
                           + CAN_CNTRL_CFG(Controller).CanRxHwObjCount);

        for (hwObjId = CAN_CNTRL_CFG(Controller).CanRxHwObjFirst; hwObjId < endHwObjId; hwObjId++)
        {
            mbRegs = CAN_MB_REG(hwObjId);
            if (CAN_MB_RX_MASK == (mbRegs->Mctstr & CAN_MB_RX_MASK))
            {
                if (CAN_RES_NEWDAT_MASK == (mbRegs->Mctstr & CAN_RES_NEWDAT_MASK))
                {
                    /* Message has lost */
                    if (CAN_RES_MSGLST_MASK == (mbRegs->Mctstr & CAN_RES_MSGLST_MASK))
                    {
                        /* Reset MSGLST bit */
                        mbRegs->Mctstr = CAN_RES_MSGLST_MASK;
                    }
					/* Get pdu */
					pdu.sdu = rxData;
					Can_GetMBInfo(hwObjId, &pdu);

					/* receive 255 after 28 03 */
					if ((2 == hwObjId) && (1 == FunctService255))
					{
						if ((0x214 == pdu.id) && (0x00 == pdu.sdu[0]) && (0xFF == pdu.sdu[1]))
						{
						  /* the state of responsing 28 03 */
						  FunctService255 = 2;
						}
					}
					else if((2 != hwObjId) && (1 != FunctService255))
					{
						if ((2 == pdu.sdu[0]) && (0x3EU == pdu.sdu[1]) && (0x80U == pdu.sdu[2]))
						{
						  Dcm_StartP3cTimer();
						}
						else
						{
						  CanIf_RxIndication(CAN_HWOBJ_CFG(hwObjId).CanObjectId, pdu.id, pdu.length, pdu.sdu);
						}
					}
					else
					{
						/* clear IFALG */

					}
                    /* Rest NEWDAT bit */
                    mbRegs->Mctstr = CAN_RES_NEWDAT_MASK;
                }
		            /* Reset MSGLST bit */
                mbRegs->Mctstr = CAN_RES_MSGLST_MASK;		
                /* Reset RXPND bit */
                mbRegs->Mctstr = CAN_MB_RX_MASK;
            }
            /* Reset PND bit */
            Can_ResetPnd(hwObjId);
        }
    }

    return;
}

#if (STD_ON == CAN_HW_TRANSMIT_CANCELLATION)
/******************************************************************************/
/*
 * Brief               <This function performs Tx cancellation process>
 * Param-Name[in]      <Controller- CAN controller to be Tx process>
 * Param-Name[in]      <mbId- CAN controller message buffer index>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC void Can_TxCancel(uint8 HwObjId, uint8 Controller)
{
    volatile Can_MbRegType* mbRegs;
    uint8 txData[8];
    Can_PduType pdu;

    /* Mb address */
    mbRegs = CAN_MB_REG(HwObjId);

    /* Cancel transmit */
    mbRegs->Mctstr = CAN_RES_MSGVAL;

    pdu.sdu = txData;
    Can_GetMBInfo(HwObjId, &pdu);
    pdu.swPduHandle = Can_Cntrl[Controller].TxPduHandles[HwObjId];

    /* clear abort flag */
    CanIf_CancelTxConfirmation(&pdu);

    return;
}
#endif /* STD_ON == CAN_HW_TRANSMIT_CANCELLATION */

/******************************************************************************/
/*
 * Brief               <This function performs controller start Mode  process>
 * Param-Name[in]      <Controller- CAN controller to be Mode process>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
/* @req <CAN261> */
STATIC Can_ReturnType Can_StartMode(uint8 Controller)
{
    volatile Can_NodeRegType* canRegs;
    Can_ReturnType            ret = CAN_NOT_OK;

    if (CAN_CS_STARTED == Can_Cntrl[Controller].CntrlMode)
    {
        ret = CAN_OK;
    }
    else if (CAN_CS_STOPPED == Can_Cntrl[Controller].CntrlMode)
    {
        canRegs = CAN_CNTRL_REG(Controller);

        /* Enable Rx/Tx/Busoff interrupt, set interrupt pointer */
        if (0UL == Can_Cntrl[Controller].IntLockCount)
        {
            Can_StartStateEnableInt(Controller);
        }
        /* goto normal mode */
        canRegs->Ncr &= CAN_SET_START_MODE;
        Can_Cntrl[Controller].CntrlMode = CAN_CS_STARTED;

        ret = CAN_OK;
    }
    else
    {
        ret = CAN_NOT_OK;
    }

    return ret;
}

/******************************************************************************/
/*
 * Brief               <This function performs controller stop Mode  process>
 * Param-Name[in]      <Controller- CAN controller to be Mode process>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
/* @req <CAN283> */
STATIC Can_ReturnType Can_StopMode(uint8 Controller)
{
    volatile Can_NodeRegType* canRegs;
    Can_ReturnType            ret = CAN_NOT_OK;

    if (CAN_CS_STOPPED == Can_Cntrl[Controller].CntrlMode)
    {
        ret = CAN_OK;
    }
    else if (CAN_CS_STARTED == Can_Cntrl[Controller].CntrlMode)
    {
        canRegs = CAN_CNTRL_REG(Controller);

        /* goto normal mode */
        canRegs->Ncr |= CAN_SET_STOP_MODE;

        /* Disable interrupt */
        Can_DisableInt(Controller);

        Can_Cntrl[Controller].CntrlMode = CAN_CS_STOPPED;
        ret = CAN_OK;
    }
    else
    {
        ret = CAN_NOT_OK;
    }

    return ret;
}

/******************************************************************************/
/*
 * Brief               <This function performs controller sleep Mode  process>
 * Param-Name[in]      <Controller- CAN controller to be Mode process>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
/* @req <CAN265> */
STATIC Can_ReturnType Can_SleepMode(uint8 Controller)
{
    Can_ReturnType ret = CAN_NOT_OK;

    /* Don't Support sleep & wake up in MPC5602D */
    if (CAN_CS_SLEEP == Can_Cntrl[Controller].CntrlMode)
    {
        ret = CAN_OK;
    }
    else if (CAN_CS_STOPPED == Can_Cntrl[Controller].CntrlMode)
    {
        ret = CAN_OK;
    }
    else
    {
        ret = CAN_NOT_OK;
    }

    return ret;
}

/******************************************************************************/
/*
 * Brief               <This function performs controller wakeup Mode  process>
 * Param-Name[in]      <Controller- CAN controller to be Mode process>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC void Can_InitHwCntrl(uint8 Controller, const Can_ControllerConfigType* Config)
{
    volatile Can_NodeRegType* canRegs;

    canRegs = CAN_CNTRL_REG(Controller);

    /* Terminates the participation of this node in the CAN traffic */
    canRegs->Ncr = CAN_NODE_DISABLE;

    /* Reset port control register */
    canRegs->Npcr = CAN_PORT_REG_VALUE;

    /* Rest error control register */
    canRegs->Necnt = CAN_ERROR_REG_VALUE;

    /* Set bit timing register */
    canRegs->Nbtr = Config->CanCtrlValue;

    /* Rest CAN frame counter register */
    canRegs->Nfcr = CAN_COUNTER_REG_VALUE;

    /* initialize all mailboxes */
    Can_InitMB(Controller);

    /* Initialize Port */
    Can_InitPort(Controller);

    return;
}

/******************************************************************************/
/*
 * Brief               <This function performs controller init message buffer  process>
 * Param-Name[in]      <Controller- CAN controller to be Mode process>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC void Can_InitMB(uint8 Controller)
{
    volatile Can_MbRegType* mbRegs;
    uint8                   hwObjId = 0;
    uint8                   endHwObj = 0;
    uint32                  timeOut = 0;
    uint8                   realController = 0;
    uint32                  idType = 0;

    realController = CAN_CNTRL_PCCFG(Controller).CanControllerRelId;

    /* Initialize Receive Hardware Object */
    endHwObj = (uint8)(CAN_CNTRL_CFG(Controller).CanRxHwObjFirst
                     + CAN_CNTRL_CFG(Controller).CanRxHwObjCount);

    for (hwObjId = CAN_CNTRL_CFG(Controller).CanRxHwObjFirst; hwObjId < endHwObj; hwObjId++)
    {
        /* Allocate MOs for list */
        CAN_PANCTR_REG = ((uint32)(realController + 1) << 24)
                      | ((uint32)hwObjId << 16)
                      | CAN_PANCMD_STATIC_ALLOCATE;

        timeOut = CAN_TIMEOUT_DURATION;
        while ((timeOut > 0UL) && (CAN_PANEL_BUSY == (CAN_PANCTR_REG & CAN_PANEL_BUSY)))
        {
            timeOut--;
        }

        if (0UL == timeOut)
        {
            Dem_ReportErrorStatus(CAN_E_TIMEOUT, DEM_EVENT_STATUS_FAILED);
            break;
        }

        /* Mb address */
        mbRegs = CAN_MB_REG(hwObjId);

        /* Set rx mb register */
        mbRegs->Mctstr = CAN_RX_MB_SET;

        /* Set mb rx interrupt */
        if (CAN_PROCESS_TYPE_INTERRUPT == CAN_CNTRL_PCCFG(Controller).CanRxProcessing)
        {
            mbRegs->Moipr = CAN_RX_SRN_BASE + (uint32)realController;
            mbRegs->Mofcr = CAN_MB_RX_INT_ENABLE;
            /* Set message pending number */
            mbRegs->Moipr |= (uint32)hwObjId << 8;
        }

        idType = 0x1U & (~(CAN_HWOBJ_CFG(hwObjId).CanIdValue & 0x80000000U) >> 31);

        mbRegs->Moar = (CAN_HWOBJ_CFG(hwObjId).CanIdValue << (18 * idType)) \
                     | (CAN_MBID_ID_EXTENDED & (~(idType << 29))) | CAN_PRI_CLASS_ID;

        mbRegs->Moamr = CAN_HWOBJ_CFG(hwObjId).CanFilterMask
                      | (CAN_ENABLE_MIDE & (~((uint32)CAN_HWOBJ_CFG(hwObjId).CanIdType << 28)));

        /* Set Message Valid */
        mbRegs->Mctstr = CAN_MSG_VALID;
    }

    /* Initialize Transmit Hardware Object */
    endHwObj = (uint8)(CAN_CNTRL_CFG(Controller).CanTxHwObjFirst
                     + CAN_CNTRL_CFG(Controller).CanTxHwObjCount);

	while(CAN_PANCTR_REG & (0x100 |0x200));
   
     /*for(2;3;++)*/
    for (hwObjId = CAN_CNTRL_CFG(Controller).CanTxHwObjFirst; hwObjId < endHwObj; hwObjId++)
    {
        /* Allocate MOs for list */
        CAN_PANCTR_REG = ((uint32)(realController + 1) << 24)
                      | ((uint32)hwObjId << 16) | CAN_PANCMD_STATIC_ALLOCATE;

        timeOut = CAN_TIMEOUT_DURATION;
        while ((timeOut > 0UL) && (CAN_PANEL_BUSY == (CAN_PANCTR_REG & CAN_PANEL_BUSY)))
        {
            timeOut--;
        }

        if (0UL == timeOut)
        {
            Dem_ReportErrorStatus(CAN_E_TIMEOUT, DEM_EVENT_STATUS_FAILED);
            break;
        }

        /* Mb address */
        mbRegs = CAN_MB_REG(hwObjId);

        /* Set tx mb register */
        mbRegs->Mctstr = CAN_TX_MB_SET;

        /* Set mb tx interrupt */
        if (CAN_PROCESS_TYPE_INTERRUPT == CAN_CNTRL_PCCFG(Controller).CanTxProcessing)
        {
            mbRegs->Moipr = (CAN_TX_SRN_BASE + (uint32)realController) << 4;
            mbRegs->Mofcr = CAN_MB_TX_INT_ENABLE;
            /* Set message pending number */
            mbRegs->Moipr |= (uint32)hwObjId << 8;
        }
    }

    return;
}

STATIC void Can_DeInitMB(uint8 Controller)
{
    volatile Can_MbRegType* mbRegs;
    uint8                   hwObjId = 0;
    uint8                   endHwObj = 0;
#if 0 /* daizhumsheng do*/
    uint32                  timeOut = 0;
    uint8                   realController = 0;
    uint32                  idType = 0;

    realController = CAN_CNTRL_PCCFG(Controller).CanControllerRelId;
#endif
    /* Initialize Receive Hardware Object */
    endHwObj = (uint8)(CAN_CNTRL_CFG(Controller).CanRxHwObjFirst
                     + CAN_CNTRL_CFG(Controller).CanRxHwObjCount);

    for (hwObjId = CAN_CNTRL_CFG(Controller).CanRxHwObjFirst; hwObjId < endHwObj; hwObjId++)
    {
        /* Mb address */
        mbRegs = CAN_MB_REG(hwObjId);

        /* reset rx mb register */
        mbRegs->Mctstr = CAN_RX_MB_RESET;

        mbRegs->Moar = 0;
    }

    /* Initialize Transmit Hardware Object */
    endHwObj = (uint8)(CAN_CNTRL_CFG(Controller).CanTxHwObjFirst
                     + CAN_CNTRL_CFG(Controller).CanTxHwObjCount);

	while(CAN_PANCTR_REG & (0x100 |0x200));

     /*for(2;3;++)*/
    for (hwObjId = CAN_CNTRL_CFG(Controller).CanTxHwObjFirst; hwObjId < endHwObj; hwObjId++)
    {
        /* Mb address */
        mbRegs = CAN_MB_REG(hwObjId);

        /* Set tx mb register */
        mbRegs->Mctstr = CAN_TX_MB_RESET;

        mbRegs->Moar = 0;
    }

}

#if (STD_ON == CAN_MULTIPLEXED_TRANSMISSION)
/******************************************************************************/
/*
 * Brief               <find lowest priority mailbox for same hth>
 * Param-Name[in]      <Hth - HW-transmit handle>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC uint8 Can_FindLowPriorityMb(uint8 Hth)
{
    uint8  hwObjId = 0;
    uint8  retHwObj = CAN_HWOBJ_ID(Hth);
    uint32 CanId = 0;

    if (FALSE == Can_IsTxMbFree(retHwObj))
    {
        CanId = Can_GetMBCanId(retHwObj);
        hwObjId = (uint8)(retHwObj+1);
        while ((hwObjId < CAN_HWOBJ_NUM) && (Hth == CAN_HWOBJ_CFG(hwObjId).CanObjectId))
        {
            if (TRUE == Can_IsTxMbFree(hwObjId))
            {
                retHwObj = hwObjId;
                break;
            }
            else if (TRUE == Can_PriorityHigher(CanId, Can_GetMBCanId(hwObjId)))
            {
                retHwObj = hwObjId;
                CanId = Can_GetMBCanId(hwObjId);
            }
            else
            {
                /* do nothing */
            }

            hwObjId++;
        }
    }

    return retHwObj;
}
#endif /* STD_ON == CAN_MULTIPLEXED_TRANSMISSION */

/******************************************************************************/
/*
 * Brief               <check a mailbox if free, can transmit pdu>
 * Param-Name[in]      <HwObjId - hardware object index>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC boolean Can_IsTxMbFree(uint8 HwObjId)
{
    volatile Can_MbRegType* mbRegs;
    boolean                 retVal = FALSE;

    /* Mb address */
    mbRegs = CAN_MB_REG(HwObjId);

    /* Mb is free */
    if (0UL == (mbRegs->Mctstr & CAN_RES_MSGVAL))
    {
        retVal = TRUE;
    }

    return retVal;
}

/******************************************************************************/
/*
 * Brief               <Write PDU into mailbox to Transmit request>
 * Param-Name[in]      <mbId    - mailbox index>
 * Param-Name[in]      <PduInfo - pdu information>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC void Can_WriteMb(uint8 HwObjId, const Can_PduType* PduInfo)
{
    volatile Can_MbRegType* mbRegs;
    uint8                   controller = 0;
    uint8                   i = 0;
    uint32                  idType = 0;

    controller = CAN_HWOBJ_CFG(HwObjId).CanControllerRef;

    /* Mb address */
    mbRegs = CAN_MB_REG(HwObjId);

    /* Set tx mb register */
    mbRegs->Mctstr = 0x8U;

    /* Id */
    idType = 0x1U & (~(PduInfo->id & 0x80000000U) >> 31);

    mbRegs->Moar = (PduInfo->id << (18 * idType)) \
                 | (CAN_MBID_ID_EXTENDED & (~(idType << 29))) | CAN_PRI_CLASS_ID;

    /* DLC */
    /* Clear DLC */
    mbRegs->Mofcr &= 0xF0FFFFFFU;
    mbRegs->Mofcr |= (uint32)PduInfo->length << 24;

    /* SDU */
    /* Clear MODATA */
    mbRegs->Modata[0] = 0;
    mbRegs->Modata[1] = 0;

    for (i = 0; i < PduInfo->length; i++)
    {
        /* MISRA RULE 17.4:491 VIOLATION: Array subscripting applied to an object pointer */
        if (i < 4)
        {
            mbRegs->Modata[0] |= (uint32)PduInfo->sdu[i] << (i * 8);
        }
        else
        {
            mbRegs->Modata[1] |= (uint32)PduInfo->sdu[i] << ((i - 4) * 8);
        }
    }

    /* save pdu handle */
    Can_Cntrl[controller].TxPduHandles[HwObjId] = PduInfo->swPduHandle;

    /* Set Message Valid */
    mbRegs->Mctstr = CAN_MSG_VALID;

    /* send request */
    mbRegs->Mctstr = CAN_SET_TXRQ;

    return;
}

#if ((STD_ON == CAN_HW_TRANSMIT_CANCELLATION) || (STD_ON == CAN_MULTIPLEXED_TRANSMISSION))
/******************************************************************************/
/*
 * Brief               <get can id from specific mailbox hardware>
 * Param-Name[in]      <mbId    - mailbox index>
 * Param-Name[in/out]  <None>
 * Return              <Can ID>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC uint32 Can_GetMBCanId(uint8 HwObjId)
{
    volatile Can_MbRegType* mbRegs;
    uint32                  canId = 0;

    /* Mb address */
    mbRegs = CAN_MB_REG(HwObjId);

    /* extended frame */
    if (CAN_MBID_ID_EXTENDED == (mbRegs->Moar & CAN_MBID_ID_EXTENDED))
    {
        canId = mbRegs->Moar & (~CAN_MBID_ID_EXTENDED) & (~CAN_PRI_CLASS_ID);
        canId |= 0x80000000U;  /* CanIf need extended Canid set 31 bit */
    }
    /* standard frame */
    else
    {
        canId = (mbRegs->Moar & CAN_MBID_ID_STANDARD) >> 18;
    }

    return canId;
}
#endif

/******************************************************************************/
/*
 * Brief               <get PDU from specific mailbox hardware>
 * Param-Name[in]      <mbId    - mailbox index>
 * Param-Name[in/out]  <pdu     - pdu>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC void Can_GetMBInfo(uint8 HwObjId, Can_PduType* pdu)
{
    volatile Can_MbRegType* mbRegs;
    uint8                   i = 0;

    /* Mb address */
    mbRegs = CAN_MB_REG(HwObjId);

    /* ID */
    if ((mbRegs->Moar & CAN_IDE) != 0)  /* extended frame */
    {
        pdu->id = mbRegs->Moar & (~CAN_MBID_ID_EXTENDED) & (~CAN_PRI_CLASS_ID);
        pdu->id |= 0x80000000U;   /* CanIf need extended Canid set 31 bit */
    }
    else /* standard frame */
    {
        pdu->id = (mbRegs->Moar & CAN_MBID_ID_STANDARD) >> 18;
    }

    /* DLC */
    pdu->length = (uint8)((mbRegs->Mofcr & CAN_MBCS_LENGTH) >> 24);

    /* SDU */
    for (i = 0; i < pdu->length; i++) /* @req <CAN299> */
    {
        /* MISRA RULE 17.4:491 VIOLATION: Array subscripting applied to an object pointer */
        if (i < 4)
        {
            pdu->sdu[i] = (uint8)((mbRegs->Modata[0] & (0xFFU << (i * 8))) >> (i * 8));
        }
        else
        {
            pdu->sdu[i] = (uint8)((mbRegs->Modata[1] & (0xFFU << ((i - 4) * 8))) >> ((i - 4) * 8));
        }
    }

    return;
}

#if ((STD_ON == CAN_HW_TRANSMIT_CANCELLATION) || (STD_ON == CAN_MULTIPLEXED_TRANSMISSION))
/******************************************************************************/
/*
 * Brief               <compare two canid priority>
 * Param-Name[in]      <destId - destination Can ID>
 * Param-Name[in]      <srcId  - source Can ID>
 * Return              <TRUE: destId higher than srcId, 
 *                      FALSE: destId not higher than srcId>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC boolean Can_PriorityHigher(Can_IdType destId, Can_IdType srcId)
{
    boolean ret = FALSE;

    /* clear bit 29, 30 */
    destId &= 0x9FFFFFFFU;
    srcId &= 0x9FFFFFFFU;

    /* low 11 bit same, then compare high 21 bit */
    if ((destId & 0x7FFU) == (srcId & 0x7FFU))
    {
        /* compare high 21 bit */
        if ((destId & 0xFFFFF800U) < (srcId & 0xFFFFF800U))
        {
            ret = TRUE;
        }
        else
        {
            ret = FALSE;
        }
    }
    else if ((destId & 0x7FFU) < (srcId & 0x7FFU))
    {
        ret = TRUE;
    }
    else
    {
        ret = FALSE;
    }

    return ret;
}
#endif /* (STD_ON==CAN_HW_TRANSMIT_CANCELLATION)||(STD_ON==CAN_MULTIPLEXED_TRANSMISSION) */

/******************************************************************************/
/*
 * Brief               <Enable interrupt when state is started>
 * Param-Name[in]      <Controller>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC void Can_StartStateEnableInt(uint8 Controller)
{
    volatile Can_NodeRegType* canRegs;
    volatile Can_MbRegType*   mbRegs;
    volatile Can_SrcRegType*  srcRegs;
    uint8                     hwObjId = 0;
    uint8                     endHwObj = 0;
    uint8                     realController = 0;

    realController = CAN_CNTRL_PCCFG(Controller).CanControllerRelId;
    canRegs = CAN_CNTRL_REG(Controller);
    srcRegs = CAN_SRC_REG(realController);

    /* Enable Rx/Tx/Busoff interrupt, set interrupt pointer */
    if (CAN_PROCESS_TYPE_INTERRUPT == CAN_CNTRL_PCCFG(Controller).CanRxProcessing)
    {
        srcRegs->RXI = 0x1000U | ((uint32)realController + (uint32)0x4U);
        canRegs->Ncr |= 0x2U;
        /* Enable mb interrupt */
        endHwObj = (uint8)(CAN_CNTRL_CFG(Controller).CanRxHwObjFirst
                         + CAN_CNTRL_CFG(Controller).CanRxHwObjCount);

        for (hwObjId = CAN_CNTRL_CFG(Controller).CanRxHwObjFirst; hwObjId < endHwObj; hwObjId++)
        {
            /* Mb address */
            mbRegs = CAN_MB_REG(hwObjId);

            /* Enable mb rx interrupt */
            mbRegs->Mofcr |= CAN_MB_RX_INT_ENABLE;
        }
    }

    if (CAN_PROCESS_TYPE_INTERRUPT == CAN_CNTRL_PCCFG(Controller).CanTxProcessing)
    {
        srcRegs->TXI = 0x1000U | ((uint32)realController + (uint32)0x7U);
        canRegs->Ncr |= 0x2U;
        endHwObj = (uint8)(CAN_CNTRL_CFG(Controller).CanTxHwObjFirst
                         + CAN_CNTRL_CFG(Controller).CanTxHwObjCount);

        for (hwObjId = CAN_CNTRL_CFG(Controller).CanTxHwObjFirst; hwObjId < endHwObj; hwObjId++)
        {
            /* Mb address */
            mbRegs = CAN_MB_REG(hwObjId);

            /* Enable mb tx interrupt */
            mbRegs->Mofcr |= CAN_MB_TX_INT_ENABLE;
        }
    }

    if (CAN_PROCESS_TYPE_INTERRUPT == CAN_CNTRL_PCCFG(Controller).CanBusOffProcessing)
    {
        srcRegs->BOFI = 0x1000U | ((uint32)realController + (uint32)0x1U);
        /* Enable busoff interrupt */
        canRegs->Ncr |= CAN_ENABLE_BUSOFF_INT;
        canRegs->Nipr |= realController + 1;
    }

    return;
}

/******************************************************************************/
/*
 * Brief               <Disable interrupt >
 * Param-Name[in]      <Controller>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC void Can_DisableInt(uint8 Controller)
{
    volatile Can_NodeRegType* canRegs;
    volatile Can_MbRegType*   mbRegs;
    uint8                     hwObjId = 0;
    uint8                     endHwObj = 0;

    canRegs = CAN_CNTRL_REG(Controller);

    if (CAN_PROCESS_TYPE_INTERRUPT == CAN_CNTRL_PCCFG(Controller).CanRxProcessing)
    {
        /* Enable mb interrupt */
        endHwObj = (uint8)(CAN_CNTRL_CFG(Controller).CanRxHwObjFirst
                         + CAN_CNTRL_CFG(Controller).CanRxHwObjCount);

        for (hwObjId = CAN_CNTRL_CFG(Controller).CanRxHwObjFirst; hwObjId < endHwObj; hwObjId++)
        {
            /* Mb address */
            mbRegs = CAN_MB_REG(hwObjId);

            /* Disable mb rx interrupt */
            mbRegs->Mofcr &= ~CAN_MB_RX_INT_ENABLE;
        }
    }

    if (CAN_PROCESS_TYPE_INTERRUPT == CAN_CNTRL_PCCFG(Controller).CanTxProcessing)
    {
        endHwObj = (uint8)(CAN_CNTRL_CFG(Controller).CanTxHwObjFirst
                         + CAN_CNTRL_CFG(Controller).CanTxHwObjCount);

        for (hwObjId = CAN_CNTRL_CFG(Controller).CanTxHwObjFirst; hwObjId < endHwObj; hwObjId++)
        {
            /* Mb address */
            mbRegs = CAN_MB_REG(hwObjId);

            /* Set mb tx interrupt */
            mbRegs->Mofcr &= ~CAN_MB_TX_INT_ENABLE;
        }
    }

    if (CAN_PROCESS_TYPE_INTERRUPT == CAN_CNTRL_PCCFG(Controller).CanBusOffProcessing)
    {
        /* Disable busoff interrupt */
        canRegs->Ncr &= ~CAN_ENABLE_BUSOFF_INT;
    }

    return;
}

/******************************************************************************/
/*
 * Brief               <Disable interrupt >
 * Param-Name[in]      <Controller>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC void Can_ResetPnd(uint8 HwObjId)
{
    switch (HwObjId & CAN_MSPND_MASK)
    {
    case CAN_MSPND0:
        if (CAN_MSPND0 != 0)
        {
            CAN_MSPND_0 = ~(uint32)(0x1U << HwObjId);
        }
        break;

    case CAN_MSPND1:
        if (CAN_MSPND1 != 0)
        {
            CAN_MSPND_1 = ~(uint32)(0x1U << (HwObjId - 32));
        }
        break;

    case CAN_MSPND2:
        if (CAN_MSPND2 != 0)
        {
            CAN_MSPND_2 =~(uint32)(0x1U << (HwObjId - 64));
        }
        break;

    case CAN_MSPND3:
        if (CAN_MSPND3 != 0)
        {
            CAN_MSPND_3 = ~(uint32)(0x1U << (HwObjId - 96));
        }
        break;

    default:
        break;
    }

    return;
}

/******************************************************************************/
/*
 * Brief               <This function performs controller wakeup Mode  process>
 * Param-Name[in]      <Controller- CAN controller to be Mode process>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC Can_ReturnType Can_WakeupMode(uint8 Controller)
{
    Can_ReturnType ret = CAN_NOT_OK;

    /* Don't Support Sleep & Wake up in MPC5602D */
    if (CAN_CS_STOPPED == Can_Cntrl[Controller].CntrlMode)
    {
        ret = CAN_OK;
    }
    else
    {
        ret = CAN_NOT_OK;
    }

    return ret;
}

/******************************************************************************/
/*
 * Brief               <This function initialize port for can>
 * Param-Name[in]      <Controller- CAN controller to be Mode process>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
STATIC void Can_InitPort(uint8 Controller)
{
    uint8 realController = CAN_CNTRL_PCCFG(Controller).CanControllerRelId;

    switch (realController)
    {
    case 0:
        P20_IOCR4 = (P20_IOCR4 & ~0xF8000000U) | 0x20000000U;    /*P20.7 RX*/
        P20_IOCR8 = (P20_IOCR8 & ~0xF8U) | 0xA8U;   /*P20.8 TX*/
        break;

    case 1:
    	P14_IOCR0 = (P14_IOCR0 & ~0xF800U) | 0x2000U;  /*P14.1 RX*/
    	P14_IOCR0 = (P14_IOCR0 & ~0xF8U) | 0xA8U; /*P14.0 TX*/
        break;

    case 2:
        P4_IOCR0 = (P4_IOCR0 & ~0xF0U) | 0x20U;
        P4_IOCR0 = (P4_IOCR0 & ~0xF000U) | 0xB000U;
        break;

    default:
        break;
    }
}

/******************************************************************************/
/*
 * Brief               <This function de-initialize can>
 * Param-Name[in]      <Controller- CAN controller to be Mode process>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
void Can_Deinit(void)
{
    volatile Can_NodeRegType* canRegs;
    uint8                     i = 0;

    /* Access to Endinit-protected registers is permitted */
    unlock_wdtcon();	
    CAN_MSPND_0 = 0;
    CAN_MSPND_1 = 0;
    CAN_MSPND_2 = 0;
    CAN_MSPND_3 = 0;
    CAN_FDR_REG = 0;

    for (i = 0; i < CAN_MAX_CONTROLLERS; i++)
    {
    	Can_DeInitMB(i);
        canRegs = CAN_CNTRL_REG(i);
        canRegs->Ncr = CAN_NODE_DISABLE;
        canRegs->Nipr = 0;
        canRegs->Npcr = CAN_PORT_REG_VALUE;
        canRegs->Nbtr = 0;
        canRegs->Necnt = CAN_ERROR_REG_VALUE;
        canRegs->Nfcr = CAN_COUNTER_REG_VALUE;
    }

    /* initialize MOs list */
    CAN_PANCTR_REG = CAN_PANCMD_INIT_LIST;

    CAN_CLC_REG = 0x3U;
    lock_wdtcon();
}

/*=======[E N D   O F   F I L E]==============================================*/

