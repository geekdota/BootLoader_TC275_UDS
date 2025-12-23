/*============================================================================*/
/*  Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 *
 *  @file       <STM.h>
 *  @brief      <This is STM C file>
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <10086>
 *  @date       <2017-5-09>
 */
/*============================================================================*/

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/*  <VERSION>    <DATE>    <AUTHOR>      <REVISION LOG>
 *  V1.0.0       20170509  10086   Initial version
 *  V1.0.1       20180511  10086          update
 */
/*============================================================================*/

#ifndef STM_H
#define STM_H
/*=======[I N C L U D E S]====================================================*/
#include "std_types.h"
#include "STM_Cfg.h"

/*=======[E X T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
/******************************************************************************/
/*
 * Brief               <This function initializes the STM driver>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-Name[in]      <None>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
extern void STM_Init(void);

/******************************************************************************/
/*
 * Brief               <STM Deinit>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-Name[in]      <None>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <None>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
extern void STM_Deinit(void);

/******************************************************************************/
/*
 * Brief               <STM time out flag>
 * Sync/Async          <Synchronous>
 * Reentrancy          <Non Reentrant>
 * Param-Name[in]      <None>
 * Param-Name[out]     <None>
 * Param-Name[in/out]  <None>
 * Return              <TRUE, FALSE>
 * PreCondition        <None>
 * CallByAPI           <None>
 */
/******************************************************************************/
extern boolean STM_GetFlag(void);

#endif /* STM_H */
