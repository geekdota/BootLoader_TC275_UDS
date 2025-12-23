/*============================================================================*/
/*  Copyright (C) 2009-2018, 10086 INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is 10086 property. Duplication
 *  or disclosure without 10086 written authorization is prohibited.
 *
 *
 *  @file       <STM.C>
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

/*=======[I N C L U D E S]====================================================*/
#include "mcu.h"
#include "STM.h"
#include "wdtcon.h"
/*=======[M A C R O S]========================================================*/

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
void STM_Init(void)
{
	/* Access to Endinit-protected registers is permitted */
	unlock_safety_wdtcon();

	while (0U != (BL_SCU_CCUCON1 >> 31))
		;
	{	/*Wait till ccucon registers can be written with new value */
		/*No "timeout" required, because if it hangs, Safety Endinit will give a trap */
	}
	/*set .UP to 1,.INSEL to 1,fstm = fsource/2*/
	BL_SCU_CCUCON1 &= 0xFFFFF0FF;
	BL_SCU_CCUCON1 |= 0x50000200;

	lock_safety_wdtcon();

	/* prepare compare register */
	BL_STM0_CMP0 = BL_STM0_TIM0 + CFG_STM0_CMP0_VALUE;
	/* Compare Match cfg */
	BL_STM0_CMCON |= 0x0000001F;
	/* Interrupt on compare match with CMP0 enabled */
	BL_STM0_ICR |= 0x00000001;
	/* reset interrupt flag */
	BL_STM0_ISCR = BL_STM0_ISCR | 0x00000001;
}

/******************************************************************************/
/*
 * Brief               <Gpt Deinit>
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
void STM_Deinit(void)
{
	/* Access to Endinit-protected registers is permitted */
	unlock_safety_wdtcon();

	BL_SCU_CCUCON1 = 0x2211U | (0x1U << 30);
	BL_STM0_ISCR = 0x0U;
	BL_STM0_CMP0 = 0x0U;
	BL_STM0_CMCON = 0x0U;
	BL_STM0_ICR = 0x0U;

	lock_safety_wdtcon();

	return;
}

/******************************************************************************/
/*
 * Brief               <Get time out flag>
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
boolean STM_GetFlag(void)
{
	boolean timOut = FALSE;

	/* Judge whether a time-out occured */
	if (0x02 == (BL_STM0_ICR & 0x00000002))
	{
		timOut = TRUE;
		/* Reload value for compare */
		BL_STM0_CMP0 = CFG_STM0_CMP0_VALUE + BL_STM0_CMP0;
		if (BL_STM0_CMP0 < BL_STM0_TIM0)
		{
			BL_STM0_CMP0 = CFG_STM0_CMP0_VALUE + BL_STM0_TIM0;
		}
		/* clear interrupt flag */
		BL_STM0_ISCR = BL_STM0_ISCR | 0x00000001;
	}

	return timOut;
}

