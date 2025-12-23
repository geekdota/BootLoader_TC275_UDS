/*============================================================================*/
/*  Copyright (C) 2009-2017, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is 10086 property. Duplication
 *  or disclosure without 10086 written authorization is prohibited.
 *
 *
 *  @file       <Wdg.h>
 *  @brief      <This is Wdg header file>
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <10086>
 *  @date       <2014-5-30>
 */
/*============================================================================*/
/******************************************************************************/
#ifndef WDG_H
#define WDG_H

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/*  <VERSION>    <DATE>    <AUTHOR>    <REVISION LOG>
 *  V1.0.0       20140530  10086  Initial version
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "std_types.h"
#include "wdg_cfg.h"

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
/******************************************************************************/
/*
 * Brief               <Initializes the WDG driver>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non-Reentrant>
 * Param-Name[in]      <None>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <APIName>
 */
/******************************************************************************/
extern void Wdg_Start(void);

/******************************************************************************/
/*
 * Brief               <Stop the WDG driver>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non-Reentrant>
 * Param-Name[in]      <None>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <APIName>
 */
/******************************************************************************/
extern void Wdg_Stop(void);

/******************************************************************************/
/*
 * Brief               <This Funtion Triggers the Watchdog Hardware>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non-Reentrant>
 * Param-Name[in]      <None>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <APIName>
 */
/******************************************************************************/
extern void Wdg_Kick(void);

/******************************************************************************/
/*
 * Brief               <This Funtion resets the Wdg module>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non-Reentrant>
 * Param-Name[in]      <None>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <APIName>
 */
/******************************************************************************/
extern void Wdg_SystemReset(void);

#endif

