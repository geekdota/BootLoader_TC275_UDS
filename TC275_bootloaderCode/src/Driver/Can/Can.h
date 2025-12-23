/*============================================================================*/
/*  Copyright (C) 2009-2018, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is 10086 property. Duplication
 *  or disclosure without 10086 written authorization is prohibited.
 *
 *
 *  @file       <Can.h>
 *  @brief      <Can driver module header file>
 *
 * <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <10086>
 *  @date       <03-18-2018>
 */
/*============================================================================*/
#ifndef CAN_H
#define CAN_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/*  <VERSION>    <DATE>    <AUTHOR>    <REVISION LOG>
 *  V1.0.0       20180318  10086   Initial version
 *                                      these features not support in this version:
 *                                      1. multiple Can Drivers.
 *                                      2. Can transceiver.
 *                                      3. Sleep and weakup
 */
/*============================================================================*/

/*=======[V E R S I O N  I N F O R M A T I O N]===============================*/
/* @req <CAN085> Source File Version Information  */
#define CAN_H_AR_MAJOR_VERSION 2
#define CAN_H_AR_MINOR_VERSION 4
#define CAN_H_AR_PATCH_VERSION 0

#define CAN_H_SW_MAJOR_VERSION 1
#define CAN_H_SW_MINOR_VERSION 0
#define CAN_H_SW_PATCH_VERSION 1

/*=======[I N C L U D E S]====================================================*/
/*@req <CAN047>*/
#include "comstack_types.h"
#include "can_cfg.h"
#if (STD_ON == CAN_WAKEUP_SUPPORT)
#include "ecum_types.h"
#endif /* STD_ON == CAN_WAKEUP_SUPPORT */
#include "dcm.h"

/*=======[M A C R O S]========================================================*/
/* Service (API) ID for DET reporting */
#define CAN_INIT_ID 0x00U
#define CAN_MAINFUCTION_WRITE_ID 0x01U
#define CAN_INITCONTROLLER_ID 0x02U
#define CAN_SETCONTROLLERMODE_ID 0x03U
#define CAN_DISABLECONTROLLERINTERRUPTS_ID 0x04U
#define CAN_ENABLECONTROLLERINTERRUPTS_ID 0x05U
#define CAN_WRITE_ID 0x06U
#define CAN_GETVERSIONINFO_ID 0x07U
#define CAN_MAINFUNCTION_READ_ID 0x08U
#define CAN_MAINFUNCTION_BUSOFF_ID 0x09U
#define CAN_MAINFUNCTION_WAKEUP_ID 0x0AU
#define CAN_CBK_CHECKWAKEUP_ID 0x0BU

/* API parameter checking */
#define CAN_E_PARAM_POINTER 0x01U
#define CAN_E_PARAM_HANDLE 0x02U
#define CAN_E_PARAM_DLC 0x03U
#define CAN_E_PARAM_CONTROLLER 0x04U

/* CAN state checking */
#define CAN_E_UNINIT 0x05U
#define CAN_E_TRANSITION 0x06U

/*=======[T Y P E   D E F I N I T I O N S]====================================*/
/* Return values of CAN Driver API.*/
/*@req <CAN039>*/
typedef enum
{
    CAN_OK = 0,

    CAN_NOT_OK = 1,

    CAN_BUSY = 2

} Can_ReturnType;

/* State transitions that are used by the function CAN_SetControllerMode.*/
typedef enum
{
    CAN_T_START = 0,

    CAN_T_STOP = 1,

    CAN_T_SLEEP = 2,

    CAN_T_WAKEUP = 3,

    CAN_T_CNT = 4

} Can_StateTransitionType;

/* Represents the Identifier of an L-PDU.*/
typedef uint32 Can_IdType;

/*  This type is used to provide ID,DLC and SDU from CAN interface to CAN driver.*/
typedef struct
{
    /* Pointer to L-PDU */
    uint8 *sdu;

    /* CANID */
    Can_IdType id;

    /* Handle */
    PduIdType swPduHandle;

    /* DLC */
    uint8 length;

} Can_PduType;

/* the Can_CurrentConfigPtruration (parameters) of the CAN Filter Mask(s).*/
/*@req <CAN351>*/
typedef uint32 Can_FilterMaskType;

/*
 * Specifies if the HardwareObject is used as Transmit or as Receive object.
 */
typedef enum
{
    CAN_OBJECT_TYPE_RECEIVE,

    CAN_OBJECT_TYPE_TRANSMIT

} Can_ObjectTypeType;

/** This enum specifies whether the IdValue is of type
 * - standard identifier - extended identifier - mixed mode
 * ImplementationType: Can_IdType_Type
 */
typedef enum
{
    CAN_ID_TYPE_EXTENDED,

    CAN_ID_TYPE_STANDARD,

    CAN_ID_TYPE_MIXED

} Can_IdTypeType;

/*
 * Specifies the type (Full-CAN or Basic-CAN) of a hardware object.
 */
typedef enum
{
    /* several L-PDUS are handled */
    CAN_HANDLE_TYPE_BASIC,

    /* only one L-PDU is handled */
    CAN_HANDLE_TYPE_FULL

} Can_HohType;

/* Enumeration Interrupt or Polling mode */
typedef enum
{
    CAN_PROCESS_TYPE_INTERRUPT,

    CAN_PROCESS_TYPE_POLLING

} Can_ProcessType;

/*=======[C O N F I G  T Y P E   D E F I N I T I O N S]=======================*/
/* the Can_CurrentConfigPtruration (parameters) of CAN Hardware Objects. */
typedef struct
{
    Can_HohType CanHandleType;

    Can_IdTypeType CanIdType;

    uint32 CanIdValue;

    uint8 CanObjectId;

    Can_ObjectTypeType CanObjectType;

    uint8 CanControllerRef;

    Can_FilterMaskType CanFilterMask;

} Can_HardwareObjectType;

/* This container contains the configruration parameters of the CAN controller. */
typedef struct
{
    uint32 CanCtrlValue;

    uint8 CanRxHwObjFirst;

    uint8 CanRxHwObjCount;

    uint8 CanTxHwObjFirst;

    uint8 CanTxHwObjCount;

} Can_ControllerConfigType;

typedef struct
{
    uint32 CanControllerBaseAddr;

    uint8 CanControllerRelId;

    uint8 CanControllerId;

    boolean CanControllerActivation;

    /* SRN0-2 */
    Can_ProcessType CanBusOffProcessing;

    /* SRN3-5 */
    Can_ProcessType CanRxProcessing;

    /* SRN6-8 */
    Can_ProcessType CanTxProcessing;

#if (STD_ON == CAN_WAKEUP_SUPPORT)
    Can_ProcessType CanWakeupProcessing;

    EcuM_WakeupSourceType CanWakeupSourceRef;
#endif /* #if(STD_ON == CAN_WAKEUP_SUPPORT) */

    /* unit is Hz */
    uint32 CanCpuClock;

} Can_ControllerPCConfigType;

/* This container contains the parameters related each CAN Driver Unit. */
typedef struct
{
    const Can_ControllerConfigType *CanController;

    const Can_HardwareObjectType *CanHardwareObject;

    uint8 CanHardwareObjectNum;

    const uint8 CanHoh;

    uint8 CanHohNum;

} Can_ConfigType;

/*=======[E X T E R N A L   D A T A]==========================================*/
extern const Can_ControllerPCConfigType Can_ControllerPCConfigData[CAN_MAX_CONTROLLERS];

extern const uint8 Can_HohConfigData[CAN_MAX_HOHS];
extern const Can_HardwareObjectType Can_HardwareObjectConfigData[CAN_MAX_HARDWAREOBJECTS];
extern const Can_ControllerConfigType Can_ControllerConfigData[CAN_MAX_CONTROLLERS];

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
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
extern void Can_Init(const Can_ConfigType *Config);

#if (STD_ON == CAN_VERSION_INFO_API)
/******************************************************************************/
/*
 * Brief               <This function returns the version information of this module>
 * ServiceId           <0x07>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-Name[in]      <None>
 * Param-Name[out]     <versioninfo-pointer to location to store version information>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
/*@req <CAN252>*/
#if (STD_ON == CAN_DEV_ERROR_DETECT)
#define Can_GetVersionInfo(versionInfo)                                                      \
    do                                                                                       \
    {                                                                                        \
        if (NULL_PTR == (versionInfo))                                                       \
        {                                                                                    \
            Det_ReportError(CAN_MODULE_ID, CAN_INSTANCE,                                     \
                            CAN_GETVERSIONINFO_ID, CAN_E_PARAM_POINTER);                     \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
            ((Std_VersionInfoType *)(versionInfo))->vendorID = CAN_VENDOR_ID;                \
            ((Std_VersionInfoType *)(versionInfo))->moduleID = CAN_MODULE_ID;                \
            ((Std_VersionInfoType *)(versionInfo))->instanceID = CAN_INSTANCE;               \
            ((Std_VersionInfoType *)(versionInfo))->sw_major_version = CAN_SW_MAJOR_VERSION; \
            ((Std_VersionInfoType *)(versionInfo))->sw_minor_version = CAN_SW_MINOR_VERSION; \
            ((Std_VersionInfoType *)(versionInfo))->sw_patch_version = CAN_SW_PATCH_VERSION; \
        }                                                                                    \
    } while (0)
#else
#define Can_GetVersionInfo(versionInfo)                                                  \
    do                                                                                   \
    {                                                                                    \
        ((Std_VersionInfoType *)(versionInfo))->vendorID = CAN_VENDOR_ID;                \
        ((Std_VersionInfoType *)(versionInfo))->moduleID = CAN_MODULE_ID;                \
        ((Std_VersionInfoType *)(versionInfo))->instanceID = CAN_INSTANCE;               \
        ((Std_VersionInfoType *)(versionInfo))->sw_major_version = CAN_SW_MAJOR_VERSION; \
        ((Std_VersionInfoType *)(versionInfo))->sw_minor_version = CAN_SW_MINOR_VERSION; \
        ((Std_VersionInfoType *)(versionInfo))->sw_patch_version = CAN_SW_PATCH_VERSION; \
    } while (0)
#endif /* STD_ON == CAN_DEV_ERROR_DETECT */

#endif /* STD_ON == CAN_VERSION_INFO_API */

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
extern void Can_InitController(uint8 Controller, const Can_ControllerConfigType *Config);

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
extern Can_ReturnType Can_SetControllerMode(uint8 Controller, Can_StateTransitionType Transition);

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
extern void Can_DisableControllerInterrupts(uint8 Controller);

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
extern void Can_EnableControllerInterrupts(uint8 Controller);

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
extern Can_ReturnType Can_Write(uint8 Hth, const Can_PduType *PduInfo);

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
extern Std_ReturnType Can_Cbk_CheckWakeup(uint8 Controller);

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
extern void Can_MainFunction_Write(void);

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
extern void Can_MainFunction_Read(void);

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
extern void Can_MainFunction_BusOff(void);

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
extern void Can_MainFunction_Wakeup(void);

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
extern void Can_Deinit(void);

#endif /* #ifndef  CAN_H */

/*=======[E N D   O F   F I L E]==============================================*/

