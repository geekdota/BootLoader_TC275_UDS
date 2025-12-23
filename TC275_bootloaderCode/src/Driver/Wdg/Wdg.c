/*============================================================================*/
/*  Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 *
 *  @file       <Wdg.C>
 *  @brief      <This is Wdg C file>
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <jianan.liu>
 *  @date       <2014-5-30>
 */
/*============================================================================*/

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/*  <VERSION>    <DATE>    <AUTHOR>      <REVISION LOG>
 *  V1.0.0       20140530   jianan.liu   Initial version
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "wdg.h"
#include "mcu.h"
#include "wdtcon.h"
/*=======[M A C R O S]========================================================*/
typedef volatile struct
{
    unsigned int ENDINIT:1;    	/*[0:0] End-of-Initialization Control Bit (rwh) */
    unsigned int LCK:1;        	/*[1:1] Lock Bit to Control Access to WDTxCON0 (rwh) */
    unsigned int PW:14;			/*[15:2] User-Definable Password Field for Access to WDTxCON0 (rwh) */
    unsigned int REL:16;		/*[31:16] Reload Value for the WDT also Time Check Value (rw) */
} SCU_WDTCPU_CON0_Type;

typedef volatile struct
{
    unsigned int reserved_0:2;	/*internal Reserved */
    unsigned int IR0:1;      	/*[2:2] Input Frequency Request Control (rw) */
    unsigned int DR:1;        	/*[3:3] Disable Request Control Bit (rw) */
    unsigned int reserved_4:1; 	/*internal Reserved */
    unsigned int IR1:1;       	/*[5:5] Input Frequency Request Control (rw) */
    unsigned int UR:1;        	/*[6:6] Unlock Restriction Request Control Bit (rw) */
    unsigned int PAR:1;       	/*[7:7] Password Auto-sequence Request Bit (rw) */
    unsigned int TCR:1;       	/*[8:8] Counter Check Request Bit (rw) */
    unsigned int TCTR:7;       	/*[15:9] Timer Check Tolerance Request (rh) */
    unsigned int reserved_16:16;/*internal Reserved */
} SCU_WDTCPU_CON1_Type;

/* WDT Control Register 0 */
#define BL_SCU_WDTCPU0_CON0	      ((SCU_WDTCPU_CON0_Type *) 0xF0036100U)

/* WDT Control Register 1 */
#define BL_SCU_WDTCPU0_CON1	      ((SCU_WDTCPU_CON1_Type *) 0xF0036104U)

/* Software Reset Configuration Register. */
#define BL_SCU_SWRSTCON              (*((uint32 volatile *) 0xF0036060u))


#define SCUWDT_RESET_PASSWORD     (0x3CU)


/*=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/
void ScuWdt_ClearCpuEndinit(uint16 password)
{
    /* Read Config_0 register */
	SCU_WDTCPU_CON0_Type wdt_con0;
    wdt_con0 = *BL_SCU_WDTCPU0_CON0;

    if (wdt_con0.LCK)
    {
        /* see Table 1 (Pass.word Access Bit Pattern Requirements) */
        wdt_con0.ENDINIT = 1;
        wdt_con0.LCK     = 0;
        wdt_con0.PW      = password;

        /* Password ready. Store it to WDT_CON0 to unprotect the register */
        *BL_SCU_WDTCPU0_CON0 = wdt_con0;
    }

    /* Clear ENDINT and set LCK bit in Config_0 register */
    wdt_con0.ENDINIT = 0;
    wdt_con0.LCK     = 1;
    *BL_SCU_WDTCPU0_CON0   = wdt_con0;

    /* read back ENDINIT and wait until it has been cleared */
    while (BL_SCU_WDTCPU0_CON0->ENDINIT == 1)
    {}
}

void ScuWdt_setCpuEndinit(uint16 password)
{
    /* Read Config_0 register */
	SCU_WDTCPU_CON0_Type wdt_con0;
    wdt_con0 = *BL_SCU_WDTCPU0_CON0;

    if (wdt_con0.LCK)
    {
        /* see Table 1 (Password Access Bit Pattern Requirements) */
        wdt_con0.ENDINIT = 1;
        wdt_con0.LCK     = 0;
        wdt_con0.PW      = password;

        /* Password ready. Store it to WDT_CON0 to unprotect the register */
        *BL_SCU_WDTCPU0_CON0 = wdt_con0;
    }

    /* Set ENDINT and set LCK bit in Config_0 register */
    wdt_con0.ENDINIT = 1;
    wdt_con0.LCK     = 1;
    *BL_SCU_WDTCPU0_CON0   = wdt_con0;

    /* read back ENDINIT and wait until it has been set */
    while (BL_SCU_WDTCPU0_CON0->ENDINIT == 0)
    {}
}

void ScuWdt_initCpuWatchdog(uint16 reloadValue)
{
	SCU_WDTCPU_CON0_Type wdt_con0;

    /* Read Config_0 register and clear wdt_con1 variable */
    wdt_con0 = *BL_SCU_WDTCPU0_CON0;

    if (wdt_con0.LCK)
    {
        /* see Table 1 (Password Access Bit Pattern Requirements) */
        wdt_con0.ENDINIT = 1;
        wdt_con0.LCK     = 0;
        wdt_con0.PW     ^= 0x003F;

        /* Password ready. Store it to WDT_CON0 to unprotect the register */
        *BL_SCU_WDTCPU0_CON0 = wdt_con0;
    }

    /* Initialize CON0 register, with modify access, with user defined parameters
     * Clear ENDINT bit to unprotect CON1 register for initialization
     * see Table 3 (Modify Access Bit Pattern Requirements) */
    wdt_con0.ENDINIT = 0;
    wdt_con0.LCK     = 1;
    wdt_con0.PW      = SCUWDT_RESET_PASSWORD; //user defined password
    wdt_con0.REL     = reloadValue;   //user defined reload value

    /* Modify access ready - write WDT_CON0 register */
    *BL_SCU_WDTCPU0_CON0 = wdt_con0;

    /* read back ENDINIT and wait until it has been cleared */
    while (BL_SCU_WDTCPU0_CON0->ENDINIT == 1)
    {}

    BL_SCU_WDTCPU0_CON1->DR = 0;

    /* Initialization finished - set CPU ENDINIT protection */
    ScuWdt_setCpuEndinit(SCUWDT_RESET_PASSWORD);
}

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
void Wdg_Start(void)
{
	/* counting from 0xFB00, Fspb = 100M, time is 210ms*/
	ScuWdt_initCpuWatchdog(0xFB00U);
    return;
}

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
void Wdg_Stop(void)
{
	unlock_safety_wdtcon();
	unlock_wdtcon();
	*((volatile uint32 *)BL_SCU_WDTCPU0_CON1) |= 0x00000008u;
	lock_safety_wdtcon();
	lock_wdtcon();
    return;
}

/******************************************************************************/
/*
 * Brief               <This Function Triggers the Watch dog Hardware>
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
void Wdg_Kick(void)
{
    /* trigger watch dog */
	ScuWdt_initCpuWatchdog(WDG_TIME_PERIOD);
    return;
}

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
void Wdg_SystemReset(void)
{
    Mcu_PerformReset();
    return;
}
