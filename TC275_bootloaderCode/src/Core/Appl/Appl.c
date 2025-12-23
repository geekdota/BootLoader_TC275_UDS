/*============================================================================*/
/** Copyright (C) 2009-2017, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *  
 *  All rights reserved. This software is iSOFT property. Duplication 
 *  or disclosure without iSOFT written authorization is prohibited.
 *  
 *  @file       <Appl.c>
 *  @brief      <App Loader>
 *  
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *  
 *  @author     <cywang>
 *  @date       <2016-08-01>
 */
/*============================================================================*/

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/* <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *   V1.0    20121227   Gary       Initial version
 *
 *   V1.1    20161015   cywang     update
 *
 *   V1.2    20180515   CChen      update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "FL.h"
#include "appl.h"
#include "mcu.h"
#include "STM.h"
#include "wdg.h"
#include "SecM.h"
#include "canif.h"
#include "Can.h"
#include "cantp.h"
#include "Dcm.h"
#include "fls.h"

#ifdef LED_DEBUG
#include "IfxPort_reg.h"
#endif

/* Init STATE255 */
uint8 FunctService255 = 0;

/*=======[I N T E R N A L   D A T A]==========================================*/
/** timer counter for ECU shut down */
#if (FL_SLEEP_TIMER > 0)
STATIC uint32 ApplShutDownTimer = 0UL;
#endif

/** timer delay when ECU start up */
#if (FL_MODE_STAY_TIME > 0)
STATIC uint32 ApplBootStayTimer = 0UL;
#endif


/*=======[I N T E R N A L   F U N C T I O N   D E C L A R A T I O N S]========*/
STATIC void Appl_InitNvmInfo(void);

#if (FL_SLEEP_TIMER > 0)
STATIC void Appl_EcuShutdownTimer(void);
#endif

#if (FL_MODE_STAY_TIME > 0)
STATIC void Appl_BootStayTimer(void);
#endif

STATIC void Appl_JumpApp(void);

STATIC void Appl_GotoAppSW(void);

/*=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/

/******************************************************************************/
/**
 * @brief               <ECU initialize>
 * 
 * <This routine shall be called by the boot code contained in the flash loader
 *  to initialize common hardware.> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
FL_ResultType Appl_EcuStartup(void)
{
    /* result of software consistency */
    FL_ResultType consist;

    (void)Appl_FlStartup();

    Appl_InitNvmInfo();
	
	if(MCU_POWER_ON_RESET == Mcu_GetResetReason())
    {
    	 *(uint32 *)FL_BOOT_MODE = 0x00000000U;
    	 *(uint32 *)FL_APPL_UPDATE = 0x00000000U;
    }


    /* check if bootloader program is requested in app */
    if ((uint32)FL_EXT_PROG_REQUEST_RECEIVED == *((uint32 *)FL_BOOT_MODE))
    {
        /* clear bootloader request flash and app update flag */
        *(uint32 *)FL_BOOT_MODE = 0x00000000U;
        *(uint32 *)FL_APPL_UPDATE = 0x00000000U;

        Wdg_Start();

        /* DCM go programming mode and response programming session service */
        Dcm_ProgramInit(DCM_SESSION_PROGRAMMING);
    }
    else
    {
        /* check if session mode changed from non default session to default session */
        if ((uint32)FL_BOOT_DEFAULT_FROM_PROG == *(uint32 *)FL_BOOT_DEFAULT)
        {
            /* clear flag */
            *(uint32 *)FL_BOOT_DEFAULT = 0x00000000U;
            *(uint32 *)FL_APPL_UPDATE = 0x00000000U;

            Wdg_Start();

            /* DCM go default mode and response */
            Dcm_ProgramInit(DCM_SESSION_DEFAULT);
        }
        else
        {
            /* empty */
        }

        /* check if application software is consistency */
        consist = Appl_CheckConsistency();
        /* check result */
        if ((uint8)FL_OK == consist)
        {
#if (FL_MODE_STAY_TIME > 0)
            /* set boot stay timer */
            ApplBootStayTimer = (uint32)(FL_MODE_STAY_TIME / STM0_CMP0_PERIOD_TIME);
#else
            /* jump to App */
            Appl_GotoAppSW();
#endif
        }
        else
        {
        	Wdg_Start();
        }
    }

    return (uint8)FL_OK;
}

/******************************************************************************/
/**
 * @brief               <flashloader initialize>
 * 
 * <This routine shall be called by the flash loader to initialize the software
 *  environment needed for ECU reprogramming (after the decision to switch to 
 *  flash loader mode).> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
FL_ResultType Appl_FlStartup(void)
{
	/* MCU Clock init. */
	Mcu_Init(Mcu_ConfigRoot);

	Mcu_InitClock(0);

	while (MCU_PLL_UNLOCKED == Mcu_GetPllStatus())
	{
		/* wait for PLL locked */
	}

	Mcu_DistributePllClock();

    STM_Init();

    CanIf_Init();

    CanTp_Init();

    Dcm_Init();

    FL_InitState();

#if (FL_SLEEP_TIMER > 0)
    /* set ECU shut down timer */
    ApplShutDownTimer = (uint32)(FL_SLEEP_TIMER / STM0_CMP0_PERIOD_TIME);
#endif

#if (FL_MODE_STAY_TIME > 0)
    /* clear boot stay timer */
    ApplBootStayTimer = 0UL;
#endif

#ifdef LED_DEBUG

    P10_IOCR4.B.PC7 = 0b10000; /* General purpose push-pull */

    P10_OUT.B.P7 = 0x1; /* All off */
#endif
    return (uint8)FL_OK;
}

#if (FL_SLEEP_TIMER > 0)
/******************************************************************************/
/**
 * @brief               <ECU goto sleep mode>
 * 
 * <This routine shall be called by the flash loader to shut down the ECU (put
 *  the ECU to sleep).> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
void Appl_EcuShutdown(void)
{
    /* do nothing */
    /* reserved for user, application related */
    return;
}
#endif

/******************************************************************************/
/**
 * @brief               <ECU reset>
 * 
 * <This routine shall be called by the flashloader to reset the ECU upon 
 *  request from the external programming device.>
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
void Appl_EcuReset(void)
{
    /* clear flash driver in RAM */
    Appl_Memset((uint8 *)FL_DEV_BASE_ADDRESS, 0x00U, (uint32)FL_DEV_SIZE);

    Wdg_SystemReset();

    return;
}

/******************************************************************************/
/**
 * @brief               <check application software consistency>
 * 
 * <This routine shall be called by the flashloader to check whether the 
 *  individual parts (logical blocks) that make up the application software of 
 *  an ECU are consistent with each other.> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <FL_ResultType>    
 */
/******************************************************************************/
FL_ResultType Appl_CheckConsistency(void)
{
    /* return result */
    FL_ResultType ret = (uint8)FL_OK;
    /* block number */
    uint8         blockIndex = (uint8)0;

    for (blockIndex = (uint8)0;
         (blockIndex < FL_NUM_LOGICAL_BLOCKS) && ((uint8)FL_OK == ret);
         blockIndex++)
    {
        /* check if all block is valid */
        if (FALSE == FL_NvmInfo.blockInfo[blockIndex].blkValid
        	/* Added by fast project Custom requirements.
        	 * NO write operation for NVRAM_BLOCK, the block should not check the consistent */
		    && blockIndex != FL_NUM_NVRAM_BLOCK_LOCATION_IN_TAB)
        {
            ret = (uint8)FL_FAILED;
        }
        else
        {
            /* empty */
        }
    }

    return ret;
}

/******************************************************************************/
/**
 * @brief               <10ms task>
 * 
 * <This routine shall be called by functions of the flash loader runtime
 *  environment and the security module at least every 500 milliseconds.> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
void Appl_UpdateTriggerCondition(void)
{
    /* 10ms time out flag */
    boolean stmTime;
#ifdef LED_DEBUG
    static uint8 led_cnt = 0;
#endif
    Wdg_Kick();

    if (1 == Fl_GetActiveJob())
    {
        uint16 delayloop = 1000;

        /* force sending 78 for every sector if it is doing erase job */
        Dcm_ForcePending();
        Dcm_MainFunction();
        CanTp_MainFunction();
        CanIf_MainFunction();

        while (delayloop > 0)
        {
            delayloop--;
        }
    }

    stmTime = STM_GetFlag();
    /* check if tick time is overflow */
    if (TRUE == stmTime)
    {
        CanTp_MainFunction();
        Dcm_MainFunction();

#if (FL_SLEEP_TIMER > 0)
        Appl_EcuShutdownTimer();
#endif

#if (FL_MODE_STAY_TIME > 0)
        Appl_BootStayTimer();
#endif
        /* if state255 is received, reponse 0x28 */
        if (2 == FunctService255)
         {
               PosResponse255();
               /* keep in state255 */
               FunctService255 = 3;
         }
#ifdef LED_DEBUG
        led_cnt++;
        if (led_cnt >= 30){
        	led_cnt = 0;
        	P10_OUT.B.P7 = ~(P10_OUT.B.P7);
        }
#endif
    }
    else
    {
        /* empty */
    }
#if 0
    if (0xFFFFFFF0 == SecM_Seed)
       {
           SecM_Seed = 0x0;
       }
     SecM_Seed++;
#endif   // add by ghc 2018.4.9

    CanIf_MainFunction();

    return;
}

/******************************************************************************/
/**
 * @brief               <memory copy>
 * 
 * <MISRA C 2004 forbid to use memcpy() lib, only used  to copy data buffer of
 *  indirect address.> .
 * @param[in]           <source (IN), length (IN)>
 * @param[out]          <dest (OUT)>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
void Appl_Memcpy(uint8 * dest, const uint8 *source, uint32 length)
{
    while (length > 0UL)
    {
        if ((dest != NULL_PTR) && (source != NULL_PTR))
        {
            *dest = *source;
            dest++;
            source++;
        }
        else
        {
            break;
        }

        length--;
    }

    return;
}

/******************************************************************************/
/**
 * @brief               <memory set>
 * 
 * <MISRA C 2004 forbid to use memset() lib, only used  to set data buffer of
 *  indirect address.> .
 * @param[in]           <source (IN), length (IN)>
 * @param[out]          <dest (OUT)>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
void Appl_Memset(uint8 * dest, const uint8 source, uint32 length)
{
    while (length > 0UL)
    {
        if (dest != NULL_PTR)
        {
            *dest = source;
            dest++;
        }
        else
        {
            break;
        }

        length--;
    }

    return;
}

/******************************************************************************/
/**
 * @brief               <jump to application software>
 * 
 * <jump to application software> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
STATIC void Appl_JumpApp(void)
{
    __asm("ja (0xa0080000)");
}

/******************************************************************************/
/**
 * @brief               <get NVM information>
 * 
 * <get information from flash about bootloader information(e.g.fingerprint)> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
STATIC void Appl_InitNvmInfo(void)
{
    /* block number */
    uint8             blockIndex = (uint8)0;

    /* CRC32 parameter */
    SecM_CRCParamType crcParam;

    /* get block information from flash */
    FlashReadMemory((uint8 *)&FL_NvmInfo, FL_NVM_INFO_ADDRESS, (uint32)sizeof(FL_NvmInfoType));
    /* compute CRC for NVM information */
    crcParam.crcState = (uint8)SECM_CRC_INIT;
    crcParam.crcSourceBuffer = (const uint8 *)&FL_NvmInfo;
    crcParam.crcByteCount = (uint16)(sizeof(FL_NvmInfoType) - (uint16)FOUR);
    (void)SecM_ComputeCRC(&crcParam);
    crcParam.crcState = (uint8)SECM_CRC_COMPUTE;
    (void)SecM_ComputeCRC(&crcParam);
    crcParam.crcState = (uint8)SECM_CRC_FINALIZE;
    (void)SecM_ComputeCRC(&crcParam);

    /* compare CRC for NVM information */
    if (crcParam.currentCRC != FL_NvmInfo.infoChecksum)
    {
        /* if there is no information in NVM, then initialize the information */
        for (blockIndex = (uint8)0; blockIndex < FL_NUM_LOGICAL_BLOCKS; blockIndex++)
        {
            FL_NvmInfo.blockInfo[blockIndex].blkChecksum = 0UL;
            FL_NvmInfo.blockInfo[blockIndex].blkProgAttempt = (uint8)0x00U;
            FL_NvmInfo.blockInfo[blockIndex].blkProgCounter = (uint8)0x00U;
            FL_NvmInfo.blockInfo[blockIndex].blkValid = FALSE;
            Appl_Memset(FL_NvmInfo.blockInfo[blockIndex].fingerPrint,
                        0x00U, FL_FINGER_PRINT_LENGTH);
        }

        FL_NvmInfo.secAccessErr = (uint8)0x00U;
        FL_NvmInfo.infoChecksum = 0UL;
    }
    else
    {
        /* empty */
    }

    return;
}

#if (FL_SLEEP_TIMER > 0)
/******************************************************************************/
/**
 * @brief               <time count for ECU shut down>
 * 
 * <time count for ECU shut down> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
STATIC void Appl_EcuShutdownTimer(void)
{
    /* current security level */
    Dcm_SecurityType currentSecLev;
    /* current session mode */
    Dcm_SessionType  currentSesMod;

    currentSecLev = Dcm_GetSecurityLevel();
    currentSesMod = Dcm_GetSessionMode();

    /* if diagnostic leave default session mode or security unlocked, stop sleep timer count */
    if ((ApplShutDownTimer > 0UL)
     && ((uint8)DCM_SECURITY_LOCKED == currentSecLev)
     && ((uint8)DCM_SESSION_DEFAULT == currentSesMod))
    {
        ApplShutDownTimer--;
        /* if shut down time out, set ECU to sleep state */
        if (0UL == ApplShutDownTimer)
        {
            Appl_EcuShutdown();
        }
        else
        {
            /* empty */
        }
    }
    else
    {
        ApplShutDownTimer = 0UL;
    }

    return;
}
#endif

#if (FL_MODE_STAY_TIME > 0)
/******************************************************************************/
/**
 * @brief               <time count for ECU stay in boot>
 * 
 * <time count for ECU stay in boot> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
STATIC void Appl_BootStayTimer(void)
{
    /* current security level */
    Dcm_SecurityType currentSecLev;
    /* current session mode */
    Dcm_SessionType  currentSesMod;

    currentSecLev = Dcm_GetSecurityLevel();
    currentSesMod = Dcm_GetSessionMode();

    /* if diagnostic leave default session mode or security unlocked, stop boot stay timer count */
    if ((ApplBootStayTimer > 0UL)
     && ((uint8)DCM_SECURITY_LOCKED == currentSecLev)
     && ((uint8)DCM_SESSION_DEFAULT == currentSesMod))
    {
        ApplBootStayTimer--;
        /* if boot stay time out, jump to App */
        if (0UL == ApplBootStayTimer)
        {
            Appl_GotoAppSW();
        }
        else
        {
            /* empty */
        }
    }
    else
    {
    	Wdg_Start();
        ApplBootStayTimer = 0UL;
    }

    return;
}
#endif

/******************************************************************************/
/**
 * @brief               <go to application software>
 * 
 * <go to application software> .
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>    
 */
/******************************************************************************/
STATIC void Appl_GotoAppSW(void)
{
    /* de-initialize hardware module */
    Can_Deinit();
    STM_Deinit();

    /* jump to APP */
    Appl_JumpApp();

    return;
}

/*=======[E N D   O F   F I L E]==============================================*/
