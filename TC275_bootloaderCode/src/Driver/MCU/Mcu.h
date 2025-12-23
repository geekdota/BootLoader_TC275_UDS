/******************************************************************************
**                                                                           **
** Copyright (C) Infineon Technologies (2013)                                **
**                                                                           **
** All rights reserved.                                                      **
**                                                                           **
** This document contains proprietary information belonging to Infineon      **
** Technologies. Passing on and copying of this document, and communication  **
** of its contents is not permitted without prior written authorization.     **
**                                                                           **
*******************************************************************************
**                                                                           **
**  $FILENAME   : Mcu.h $                                                    **
**                                                                           **
**  $CC VERSION : \main\89 $                                                 **
**                                                                           **
**  $DATE       : 2015-10-08 $                                               **
**                                                                           **
**  AUTHOR      : DL-AUTOSAR-Engineering                                     **
**                                                                           **
**  VENDOR      : Infineon Technologies                                      **
**                                                                           **
**  DESCRIPTION : This file contains functionality of MCU driver.            **
**                                                                           **
**  MAY BE CHANGED BY USER [yes/no]: No                                      **
**                                                                           **
******************************************************************************/
/*  TRACEABILITY : [cover parentID=DS_AS_MCU152,
                   SAS_AS_MCU108_MCU211_MCU215_MCU109_MCU111_MCU213,
                   SAS_MCAL_MCU_0424,SAS_NAS_ALL_PR128,SAS_NAS_ALL_PR746,
                   DS_AS40X_MCU201]
                   [/cover]                                                   */

#ifndef MCU_H
#define MCU_H


/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/

  /* Inclusion of Platform_Types.h and Compiler.h */
#include "Std_Types.h"
#include "Mcal.h"


  /* Inclusion of controller sfr file */
#include "IfxScu_reg.h"

  /* Post_Build configuration parameters for MCU */
  /* MCU108 :Inclusion of Mcu_Cfg.h file */
#include "Mcu_Cfg.h"

#if (MCU_GTM_USED == STD_ON)
/* Inclusion of Gtm.h file */
 #include "Gtm.h"
#endif
#if ((MCU_E_CLOCK_FAILURE_DEM_REPORT == MCU_ENABLE_DEM_REPORT)||\
     (MCU_E_ERAY_TIMEOUT_DEM_REPORT == MCU_ENABLE_DEM_REPORT)||\
     (MCU_E_OSC_FAILURE_DEM_REPORT == MCU_ENABLE_DEM_REPORT))
#include "Dem.h"
#endif
/** Traceability : [cover parentID=DS_AS_MCU163]*/

#if ( MCU_DEV_ERROR_DETECT == STD_ON)
/* Import of DET functionality */
#include "Det.h"
#endif  /* End Of MCU_DEV_ERROR_DETECT */
/*[/cover]   */

/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/

/*
  Requirement key - MCU 121, MCU 037 :
  Module Identification, Vendor Identification
*/
  /* Module ID */
#define MCU_MODULE_ID       ((uint16)101)

  /* MCU Vendor ID */
#define MCU_VENDOR_ID       ((uint16)17)

/* Maximum timeout value to acquire the Safety endinit */
#define MCU_SAFETY_ENDINT_TIMEOUT ((uint32)150000U)

  /* [cover parentID=DS_AS_MCU112, DS_AS_MCU012] */
  /* Development error values */
  /* [/cover]  */

#if (MCU_DEV_ERROR_DETECT == STD_ON) || ( MCU_SAFETY_ENABLE == STD_ON )
  /* MCU012, MCU015: Error Value Classification & naming convention */
  /* MCU112 : Development errors are of type uint8 */

  /* MCU018 : MCU_E_PARAM_CONFIG */
  /* API Service called with wrong parameter */
#define MCU_E_PARAM_CONFIG        ((uint8)(10))

  /* MCU019 : MCU_E_PARAM_CLOCK */
  /* API Service called with invalid clocksetting */
#define MCU_E_PARAM_CLOCK         ((uint8)(11))

  /*MCU020 : MCU_E_PARAM_MODE */
  /* API Service called with invalid mode */
#define MCU_E_PARAM_MODE          ((uint8)(12))

  /* MCU021 : MCU_E_PARAM_RAMSECTION */
  /*API Service called with invalid ramsection */
#define MCU_E_PARAM_RAMSECTION    ((uint8)(13))

  /* MCU122 : MCU_E_PLL_NOT_LOCKED */
  /* If the status of the PLL is detected as not locked when
   Mcu_DistributePllClock() then this DET is reported */
#define MCU_E_PLL_NOT_LOCKED      ((uint8)(14))

  /* MCU125 : MCU_E_UNINIT */
  /* The service Mcu_Init() shall be called first before calling
    any other MCU services */
#define MCU_E_UNINIT              ((uint8)(15))

  /* MCU012 : MCU_E_PARAM_POINTER */
  /* When the passed pointer is NULL_PTR */
#define MCU_E_PARAM_POINTER       ((uint8)(0x10U))

/* CLC enable failed */
#define MCU_E_CLC_ERROR           ((uint8)(0x20U))


/* Service ID for Mcu_Init */
#define MCU_SID_INIT                 ((uint8)0)
/* Service ID for Mcu_InitRamSection */
#define MCU_SID_INITRAMSECTION       ((uint8)1)
/* Service ID for Mcu_InitClock */
#define MCU_SID_INITCLOCK            ((uint8)2)
/* Service ID for Mcu_DistributePllClock */
#define MCU_SID_DISTRIBUTEPLLCLOCK   ((uint8)3)
/* Service ID for Mcu_GetPllStatus */
#define MCU_SID_GETPLLSTATUS         ((uint8)4)
/* Service ID for Mcu_GetResetReason */
#define MCU_SID_GETRESETREASON       ((uint8)5)
/* Service ID for Mcu_GetResetRawValue */
#define MCU_SID_GETRESETRAWVALUE     ((uint8)6)
/* Service ID for Mcu_PerformReset*/
#define MCU_SID_PERFORMRESET         ((uint8)7)
/* Service ID for Mcu_SetMode */
#define MCU_SID_SETMODE              ((uint8)8)
/* Service ID for  Mcu_GetVersionInfo  */
#define MCU_SID_GETVERSIONINFO       ((uint8)9)
/* Service ID for  Mcu_GetRamState  */
#define MCU_SID_GETRAMSTATE          ((uint8)10)
/* Service ID for Mcu_ClearColdResetStatus */
#define MCU_SID_CLRCOLDRESETSTAT     ((uint8)80)
/* Service ID for Mcu_DeInit */
#define MCU_SID_DEINIT               ((uint8)81)

/* Return value when DET error found*/
#define MCU_ERR_FOUND                ((uint32)1)
/* return value when DET error is not found*/
#define MCU_ERR_NOT_FOUND            ((uint32)0)

#endif
/* End of (MCU_DEV_ERROR_DETECT == STD_ON) */

/* Reset is undefined
   When function Mcu_GetResetReason() is called prior to calling Mcu_Init() */
#define MCU_RAW_RESET_UNDEFINED     ((Mcu_RawResetType)(0xFFFFFFFFU))
/* DPR0 RAR Redundancy data area */
#define MCU_STANDBY_DSPR0_RAR_AREA   (0x70002000U)
#define MCU_STANDBY_DSPR0_RAR_SIZE   (16U) /* in words (32 bit) */

/* No Endinit Protection */
#define MCU_NO_ENDINIT_PROTECTION            (MCAL_TIN_NO_ENDINIT)
/* CPU Endinit Protection */
#define MCU_CPU_ENDINIT_PROTECTION           (MCAL_TIN_CPU_ENDINIT)
/* Safety Endinit Protection */
#define MCU_SAFETY_ENDINIT_PROTECTION        (MCAL_TIN_SAFETY_ENDINIT)


/*******************************************************************************
**                      Global Type Definitions                               **
*******************************************************************************/
  /* [cover parentID=DS_AS40X_MCU232,DS_AS40X_MCU233,DS_AS_MCU_PR458] */
  /* Identification for clock setting, which is configured in the
   configuration structure */
typedef uint32   Mcu_ClockType;
  /* [/cover]  */

  /* [cover parentID=DS_AS40X_MCU237,DS_AS_MCU_PR460, DS_AS40X_MCU238 ] */
  /* Identification for MCU mode, which is configured in the
   configuration structure */
typedef uint32   Mcu_ModeType;
  /* [/cover]  */

/*[cover parentID=DS_AS_MCU_PR462, DS_AS40X_MCU240, DS_AS40X_MCU239]*/
/* Identification for a RAM section, which is configured in the
   configuration structure */
typedef uint32   Mcu_RamSectionType;
  /* [/cover]  */

  /* [cover parentID=DS_AS40X_MCU235, DS_AS40X_MCU236]  */
  /* This type specifies the reset reason in raw register format
read from a reset status register */
typedef uint32   Mcu_RawResetType;
  /* [/cover]  */

  /* RAM Section base address */
typedef void    *Mcu_RamBaseAdrType;

  /* RAM Section Size */
typedef uint32  Mcu_RamSizeType;

  /* Data Pre-setting to be initialized */
typedef uint8  Mcu_RamPrstDatType;
  /* [/cover]  */


#if (MCU_GET_RAM_STATE_API == STD_ON)
  /* MCUxxx: Structure definition for Ram State  */
    /* [cover parentID=DS_AS40X_MCU_PR463] */
typedef enum Mcu_RamStateType
{
  MCU_RAMSTATE_INVALID=0x00U, /* Ram content is not valid or unknown */
  MCU_RAMSTATE_VALID=0x01U   /* Ram content is valid */
}Mcu_RamStateType;
/* [/cover] */
#endif /*End of Mcu_GetRamState */

/* [cover parentID=DS_AS_MCU_PR461,DS_NAS_MCU_PR643_1,
    DS_AS40X_MCU231,DS_NAS_MCU_PR643_2] */
/* Enumeration for Definitions of Pll status needed for user */
typedef enum Mcu_PllStatusType
{
  MCU_PLL_UNLOCKED,           /* PLL is unlocked */
  MCU_PLL_LOCKED,             /* PLL is locked */
  MCU_PLL_STATUS_UNDEFINED    /* PLL Status is unknown */
}Mcu_PllStatusType;
/* [/cover]  */

/* [cover parentID=DS_AS40X_MCU234,DS_AS_MCU134,DS_AS_MCU_PR465] */
/* Reset Status types of MCU needed for user as published in XDM */
typedef enum Mcu_ResetType
{
  MCU_ESR0_RESET=0x00,               /* ESR0 reset */
  MCU_ESR1_RESET=0x01,               /* ESR1 reset */
  MCU_SMU_RESET=0x02,                /* SMU reset */
  MCU_SW_RESET=0x03,                 /* Software reset */
  MCU_STM0_RESET=0x04,               /* STM0 reset */
  MCU_STM1_RESET=0x05,               /* STM1 reset */
  MCU_STM2_RESET=0x06,               /* STM2 reset */
  MCU_POWER_ON_RESET=0x07,           /* Power On reset */
  MCU_CB0_RESET=0x08,                /* CB0 reset */
  MCU_CB1_RESET=0x09,                /* CB1 reset */
  MCU_CB3_RESET=0x0A,                /* CB3 reset */
  MCU_TP_RESET=0x0B,                 /* Tuning Protection reset */
  MCU_EVR13_RESET=0x0C,              /* EVR13 Regualtor Watchdog reset */
  MCU_EVR33_RESET=0x0D,              /* EVR33 Regualtor Watchdog reset */
  MCU_SUPPLY_WDOG_RESET=0x0E,        /* Supply Watchdog reset */
  MCU_STBYR_RESET=0x0F,              /* Standby Regualtor Watchdog reset */
  MCU_RESET_MULTIPLE=0xFEU,           /* Multiple Reset Reasons found */
  MCU_RESET_UNDEFINED=0xFFU          /* Reset is undefined */
}Mcu_ResetType;
/* [/cover]  */

  /* MCU031: Clock and Reset Configuration structure */
typedef struct Mcu_ClockCfgType
{
  /*K2DIV : valid values are 0 to 127.
  Even values are preferred to get 50% duty cycle*/
  uint8 K2div[8];

  uint32 K2RampToPllDelayTicks [8];
  #if (MCU_RAMP_TO_BACKUP_FREQ_API == STD_ON)
  uint32 K2RampToBackUpDelayTicks [8];
  #endif /* End of MCU_RAMP_TO_BACKUP_FREQ_API */

  struct
  {
    unsigned_int K1div:7;             /* Main PLL K1 Divider Value */
    unsigned_int K3div:7;             /* Main PLL K3 Divider Value */
    unsigned_int Ndiv:7;              /* Main PLL N Divider Value */
    unsigned_int Pdiv:4;              /* Main PLL P Divider Value */
    unsigned_int K2steps:4;           /* Main PLL K2 Divider Steps */
    unsigned_int PllMode:1;           /* Main PLL Mode */
    unsigned_int Reserved:2;          /* reserved */
  }Mcu_ClockDivValues;

  #if (MCU_DISABLE_ERAY_PLL == STD_OFF)

  struct
  {
    unsigned_int McuErayNDivider:5;   /* Eray PLL N Divider Value */
    unsigned_int McuErayK2Divider:7;  /* Eray PLL K2 Divider Value */
    unsigned_int McuErayK3Divider:7;  /* Eray PLL K3 Divider Value */
    unsigned_int McuErayPDivider:4;   /* Eray PLL P Divider Value */
    unsigned_int Reserved:9;          /* reserved */
  }MCU_ErayPllDivValues;

  #endif
  /* End of MCU_DISABLE_ERAY_PLL == STD_OFF */

  uint32 Ccucon0;
  uint32 Ccucon1;
  uint32 Ccucon2;
  uint32 Ccucon5;
  uint32 Ccucon6;
  uint32 Ccucon7;
  uint32 Ccucon8;
  #if (MCU_ENABLE_CLOCK_MONITORING == STD_ON)
  uint32  Ccucon3;
  uint32  Ccucon4;
  #endif
  #if (MCU_FMPLL_ENABLE == STD_ON)
  uint32 Pllcon2;
  #endif

  /* Delay in us for ramping close to back up clock frequency*/
  uint8 K2RampToPllDelayConf;

}Mcu_ClockCfgType;
  /* MCU030: Structure definition for RAM settings */
typedef struct Mcu_RamCfgType
{
  Mcu_RamBaseAdrType RamBaseAdrPtr; /* RAM section base address */
  Mcu_RamSizeType RamSize;          /* RAM section size */
  Mcu_RamPrstDatType RamPrstData;   /*Data pre-setting to be initialized */
}Mcu_RamCfgType;

typedef struct Mcu_StandbyModeType
{
  uint32 PMSWCR0;                  /* PMSWCR0 register contents */
  uint8 CrcCheckEnable;            /* CRC check to be done or not */
}Mcu_StandbyModeType;
/******************************************************************************
**                      Mcu_Configtype                                        *
*******************************************************************************/

/* MCU022: Structure definition for MCU driver Configuration
Mcu_ConfigType is used to store the configuration data of the MCU Driver

In Post Build Variant the actual configuration is held by the variable
"Mcu_Configuration" defined in Mcu_PBCfg.c. The MCU driver directly refers to
the variable for accessing the configuration. */

/* MCU022: Structure definition for MCU driver Configuration */
/* [cover parentID=DS_AS_MCU131] */
typedef struct Mcu_ConfigType
{
  #if (MCU_SAFETY_ENABLE == STD_ON)
  uint32  Marker;
  #endif
  const Mcu_ClockCfgType *ClockCfgPtr;  /* Ptr to Clock Setting Config Struct*/

  #if (MCU_RAM_SECTORS != 0U)
  const Mcu_RamCfgType  *RamCfgPtr;  /* Ptr to RAM Section config structure*/
  #endif /* End For MCU_RAM_SECTORS*/

  #if (MCU_GTM_USED == STD_ON)
  const Gtm_ConfigType     *GtmConfigRootPtr;  /*Ptr to Gtm_ConfigType*/
  #endif /*End for MCU_GTM_USED*/

  uint32 ResetCfg;  /* Reset request configuration */

  Mcu_ClockType NoOfClockCfg;  /* Total number of Clock Settings */

  Mcu_RamSectionType NoOfRamCfg;  /* Total number of RAM Sectors */
  Mcu_ModeType MaxMode;  /* The maximum mode that is supported
                            by this module instance */
  const Mcu_StandbyModeType *StandbyCfgPtr; /* Ptr to Standby configuration */
}Mcu_ConfigType;
/* [/cover]  */

/*
  Type of power management mode status - Mapped directly to PMST bits of
  SCU_PMCSRx where x is for Core 0,1 or 2
*/
typedef enum
{
  MCU_UNDEFINED_MODE = 0x0U,    /* CPU undefined mode */
  MCU_NORMAL_RUN_MODE = 0x1U,   /* CPU running in normal mode */
  MCU_CPUIDLE_REQ_MODE = 0x2U,  /* CPU Idle mode requested */
  MCU_CPUIDLE_ACK_MODE = 0x3U,  /* CPU Idle mode acknowledged */
  MCU_SLEEP_REQ_MODE = 0x4U,    /* CPU Sleep mode requested */
  MCU_STANDBY_REQ_MODE = 0x6U,  /* CPU Standby mode requested */
}Mcu_ModeStateType;

/*******************************************************************************
**                      Global Constant Declarations                          **
*******************************************************************************/
#define MCU_START_SEC_POSTBUILDCFG
#include "MemMap.h"
/* Declaration of Mcu Post Build Configuration */
extern const Mcu_ConfigType Mcu_ConfigRoot[MCU_CONFIG_COUNT];
#define MCU_STOP_SEC_POSTBUILDCFG
#include "MemMap.h"

/*******************************************************************************
**                      Global Variable Declarations                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Declarations                          **
*******************************************************************************/
#define MCU_START_SEC_CODE
#include "MemMap.h"
/*******************************************************************************
** Syntax : extern void Mcu_Init(const Mcu_ConfigType* ConfigPtr)             **
**                                                                            **
** Service ID:    0                                                           **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Non reentrant                                               **
**                                                                            **
** Parameters (in):    ConfigPtr - Pointer to configuration set               **
**                                                                            **
** Parameters (out):   None                                                   **
**                                                                            **
** Return value:       None                                                   **
**                                                                            **
** Description : This routine initializes the MCU driver. The intention of    **
**                 this function is to make the configuration settings for    **
**                 power down, clock and RAM sections visible within the MCU  **
**                 driver.After execution of Mcu_Init() the configuration data**
**                 are accessible and can be used from the Mcu services       **
**                 like Mcu_InitRamSection()                                  **
**                                                                            **
*******************************************************************************/
extern void Mcu_Init(const Mcu_ConfigType* ConfigPtr);

#if (MCU_DEINIT_API == STD_ON)
/*******************************************************************************
** Traceability    : [cover parentID=[/cover]                                 **
**                                                                            **
** Syntax          : void Mcu_DeInit(void)                                    **
**                                                                            **
**                                                                            **
** Service ID      :    81                                                    **
**                                                                            **
** Sync/Async      :    Synchronous                                           **
**                                                                            **
** Reentrancy      :    Non Reentrant                                         **
**                                                                            **
** Parameters (in) :    None                                                  **
**                                                                            **
** Parameters (out):    None                                                  **
**                                                                            **
** Return value    :    None                                                  **
**                                                                            **
** Description     : This routine de-initializes the MCU driver. This
                     function puts all the resources used by MCU
                     for reset configuration and power management in
                     reset state. PLL will not be de-initialized by this
                     function.
**                                                                            **
*******************************************************************************/
void Mcu_DeInit(void);

#else
/*IFX_MISRA_RULE_19_07_STATUS=Function like macro used to generate
 compile time error in case of incorrect configuration*/
#define Mcu_DeInit() (ERROR_Mcu_DeInit_NOT_SELECTED)

#endif

/*******************************************************************************
** Syntax : Std_ReturnType Mcu_InitRamSection                                 **
**           (                                                                **
**              Mcu_RamSectionType    RamSection                              **
**           )                                                                **
**                                                                            **
** Service ID:    1                                                           **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Non Reentrant                                               **
**                                                                            **
** Parameters (in):    RamSection: Selects RAM memory section provided in     **
**                      configuration set                                     **
**                                                                            **
** Parameters (out):   None                                                   **
**                                                                            **
** Return value:       E_OK: command has been accepted                        **
**                     E_NOT_OK: command has not been accepted                **
**                      e.g. due to parameter error                           **
**                                                                            **
** Description : This function initializes the RAM section wise. The          **
**                 definition of the section and the initialization value is  **
**                 provided from the configuration structure                  **
**                                                                            **
*******************************************************************************/
extern Std_ReturnType Mcu_InitRamSection(Mcu_RamSectionType RamSection );

/*******************************************************************************
** Syntax : Std_ReturnType Mcu_InitClock                                      **
**           (                                                                **
**              Mcu_ClockType    ClockSetting                                 **
**           )                                                                **
**                                                                            **
** Service ID:    2                                                           **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Non Reentrant                                               **
**                                                                            **
** Parameters (in):    ClockSetting: Clock setting                            **
**                                                                            **
** Parameters (out):   None                                                   **
**                                                                            **
** Return value:       E_OK: command has been accepted                        **
**                     E_NOT_OK: command has not been accepted                **
**                      e.g. due to parameter error                           **
**                                                                            **
** Description : This function initializes the PLL and other MCU specific     **
**                 clock options. The clock configuration parameters are      **
**                 provided via the configuration structure. In this function **
**                 the PLL lock procedure is started (if PLL shall be         **
**                 initialized) but the function does not wait until the PLL  **
**                 is locked                                                  **
**               Note: Mcu_InitClock only initializes the PLL parameters.     **
**                 PLL clock will be activated to the MCU distribution only   **
**                 after the PLL is locked and then Mcu_DistributePllClock is **
**                 called. i.e The sequence to activate PLL clock to the MCU  **
**                  clock distribution is                                     **
**                  1)Call Mcu_InitClock                                      **
**                  2)Call Mcu_GetPllStatus                                   **
**                  3)Mcu_DistributePllClock                                  **
**                                                                            **
*******************************************************************************/
#if (MCU_INIT_CLOCK == STD_ON)
extern Std_ReturnType Mcu_InitClock(Mcu_ClockType ClockSetting );
#else
#define Mcu_InitClock(ClockSetting)  ERROR_Mcu_InitClock_NOT_SELECTED
#endif
/* End of (MCU_INIT_CLOCK == STD_ON)*/

/*******************************************************************************
** Syntax : void Mcu_DistributePllClock( void )                               **
**                                                                            **
** Service ID:    3                                                           **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Non Reentrant                                               **
**                                                                            **
** Parameters (in):   None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value:      None                                                    **
**                                                                            **
** Description : This function activates the PLL clock to the MCU clock       **
**               distribution. This function shall be executed, if the MCU    **
**               needs a separate request to activate the PLL clock after the **
**               PLL is locked. In this case the current clock source         **
**               (for example internal oscillator clock) is removed from MCU  **
**               clock distribution.                                          **
**                                                                            **
*******************************************************************************/
extern void Mcu_DistributePllClock( void );

/******************************************************************************
** Syntax : Mcu_PllStatusType Mcu_GetPllStatus( void )                        **
**                                                                            **
** Service ID:    4                                                           **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Reentrant                                                   **
**                                                                            **
** Parameters (in):   None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value:   Mcu_PllStatusType                                          **
**                 Returns whether PLL is Locked,Unlocked or Undefined status **
**                                                                            **
** Description : This function provides the lock status of the PLL            **
**                                                                            **
*******************************************************************************/
extern Mcu_PllStatusType Mcu_GetPllStatus( void );

/******************************************************************************
** Syntax : Mcu_ResetType Mcu_GetResetReason( void )                          **
**                                                                            **
** Service ID:    5                                                           **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Reentrant                                                   **
**                                                                            **
** Parameters (in):   None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value:      Mcu_ResetType                                           **
**                    Returns the reset reason                                **
**                                                                            **
** Description : The function reads the reset type from the hardware, if      **
**               supported. If the hardware doesn't support the hardware      **
**               detection of the reset reason, the return value for those    **
**               resets is always MCU_POWER_ON_RESET.                         **
**                                                                            **
*******************************************************************************/
extern Mcu_ResetType Mcu_GetResetReason( void );

/******************************************************************************
** Syntax : Mcu_ResetType Mcu_GetResetRawValue( void )                        **
**                                                                            **
** Service ID:    6                                                           **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Reentrant                                                   **
**                                                                            **
** Parameters (in):   None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value:      Mcu_RawResetType                                        **
**                    Returns the reset reason                                **
**                                                                            **
** Description : The function reads the reset type from the hardware, if      **
**               supported. If the hardware doesn't have reset status register**
**               return value shall be 0x0.                                   **
**                                                                            **
*******************************************************************************/
extern Mcu_RawResetType  Mcu_GetResetRawValue( void );

/* MCU108, MCU007  : This function is available if the runtime parameter
   MCU_PERFORM_RESET_API is set to TRUE. */
#if (MCU_PERFORM_RESET_API == STD_ON)
/******************************************************************************
** Syntax : void Mcu_PerformReset( void )                                     **
**                                                                            **
** Service ID:    7                                                           **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Non reentrant                                               **
**                                                                            **
** Parameters (in):   None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value:      None                                                    **
**                                                                            **
** Description :The function performs a microcontroller reset. This           **
**              is done by using the hardware feature of the microcontroller. **
**              The MCU specific reset type to be performed by this service   **
**              shall be configured in the configuration set.                 **
**                                                                            **
*******************************************************************************/
extern void Mcu_PerformReset( void );
#endif /* End of MCU_PERFORM_RESET_API */

/******************************************************************************
** Syntax : void Mcu_SetMode                                                  **
**          (                                                                 **
**                Mcu_ModeType   McuMode                                      **
**           )                                                                **
**                                                                            **
** Service ID:    8                                                           **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Non reentrant                                               **
**                                                                            **
** Parameters (in):   McuMode : Set different MCU power modes configured in   **
**                     the configuration set                                  **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value:      None                                                    **
**                                                                            **
** Description :This function activates the MCU power modes. In case the      **
**              CPU is switched off, the function is returned after a         **
**              wake-up was performed                                         **
**                                                                            **
*******************************************************************************/
extern void Mcu_SetMode(Mcu_ModeType McuMode);

#if (MCU_RAMP_TO_BACKUP_FREQ_API == STD_ON)
/*******************************************************************************
** Syntax : void Mcu_RampToBackUpClockFreq( void )                            **
**                                                                            **
** Service ID:    NA                                                          **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Non Reentrant                                               **
**                                                                            **
** Parameters (in):   None                                                    **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value:      E_OK: command has been accepted                         **
**                  E_NOT_OK: command has not been accepted                   **
**                                                                            **
** Description : This function ramps up/down from the current PLL frequency   **
**               to the back-up clock frequency. This shall be done in steps  **
**               but in reverse order from the steps done in                  **
**               Mcu_DistributePllClock. This function can be called only     **
**               after Mcu_InitClock and Mcu_DistributePllClock.              **
**                                                                            **
*******************************************************************************/
extern Std_ReturnType Mcu_RampToBackUpClockFreq (void);
#endif

/* MCU207 : This service is only available if
   the preprocessor switch MCU_GET_RAM_STATE_API is set to TRUE */
#if (MCU_GET_RAM_STATE_API == STD_ON)
/*******************************************************************************
** Syntax           : Mcu_RamStateType  Mcu_GetRamState                       **
**                    (                                                       **
**                      void                                                  **
**                    )                                                       **
**                                                                            **
** Service ID       : 4                                                       **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Reentrant                                               **
**                                                                            **
** Parameters (in)  : None                                                    **
**                                                                            **
** Parameters (out) : None                                                    **
**                                                                            **
** Return value     : Mcu_RamStateType  - Ram status                          **
**                                                                            **
** Description      :                                                         **
**   - provides the actual status of the microcontroller Ram                  **
**                                                                            **
*******************************************************************************/
extern Mcu_RamStateType Mcu_GetRamState( void );
#else
#define Mcu_GetRamState() ERROR_Mcu_GetRamState_NOT_SELECTED
#endif  /*End of MCU_GET_RAM_STATE_API */

#if (MCU_E_CLOCK_FAILURE_DEM_REPORT == MCU_ENABLE_DEM_REPORT)
/******************************************************************************
** Syntax :           void Mcu_ClockFailureNotification(void)                **
**                                                                           **
** Service ID:        none                                                   **
**                                                                           **
** Sync/Async:        Synchronous                                            **
**                                                                           **
** Reentrancy:        reentrant                                              **
**                                                                           **
** Parameters (in):   None                                                   **
**                                                                           **
** Parameters (out):  None                                                   **
**                                                                           **
** Return value:      void                                                   **
**                                                                           **
** Description :      Function reports clock fail error to DEM               **
**                                                                           **
*****************************************************************************/
extern void Mcu_ClockFailureNotification(void);
#else
/*IFX_MISRA_RULE_19_07_STATUS=Function like macro used to generate
 compile time error in case of incorrect configuration*/
#define Mcu_ClockFailureNotification()                               \
(ERROR_Mcu_ClockFailureNotification_NOT_SELECTED)
#endif  /* #if (MCU_E_CLOCK_FAILURE_DEM_REPORT == MCU_ENABLE_DEM_REPORT) */

#if (MCU_SAFETY_ENABLE == STD_ON)

#if (MCU_INITCHECK_API == STD_ON)
/*******************************************************************************
**                                                                            **
** Syntax          : void Mcu_InitCheck (const Mcu_ConfigType* ConfigPtr)     **
**                                                                            **
** Service ID      :    None                                                  **
**                                                                            **
** Sync/Async      :    Synchronous                                           **
**                                                                            **
** Reentrancy      :    Non Reentrant                                         **
**                                                                            **
** Parameters (in) :    ConfigPtr - Pointer to MCU Driver configuration set   **
**                                                                            **
** Parameters (out):    None                                                  **
**                                                                            **
** Return value    :    None                                                  **
**                                                                            **
** Description     : This routine verifies the initialization the MCU driver. **
**                   Note: The sequence should be                             **
**                    1)Call Mcu_InitClock                                    **
**                    2)Call Mcu_GetPllStatus                                 **
**                    3)Call Mcu_DistributePllClock                           **
**                    4)Call Mcu_InitCheck                                    **
**                                                                            **
*******************************************************************************/
extern Std_ReturnType Mcu_InitCheck (const Mcu_ConfigType* ConfigPtr);

#endif /* (MCU_INIT_CHECK_API == STD_ON) */

#if (MCU_GETMODE_API == STD_ON)
/*******************************************************************************
** Syntax           : Mcu_ModeStateType  Mcu_GetMode                          **
**                    (                                                       **
**                      void                                                  **
**                    )                                                       **
**                                                                            **
** Service ID       : None                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Non reentrant                                           **
**                                                                            **
** Parameters (in):   CoreId : The Core for which the mode needs to be        **
**                    dertermined                                             **
**                                                                            **
** Parameters (out) : None                                                    **
**                                                                            **
** Return value     : Mcu_ModeStateType  - Power mode status                  **
**                                                                            **
** Description      :                                                         **
**   - provides the mode the CPU is currently in                              **
**                                                                            **
*******************************************************************************/
extern Mcu_ModeStateType Mcu_GetMode( uint8 CoreId );
#endif /* (MCU_GETMODE_API == STD_ON) */

#endif  /*End of MCU_SAFETY_ENABLE */

#if (MCU_CLR_COLD_RESET_STAT_API == STD_ON)
/*******************************************************************************
** Syntax           : void  Mcu_ClearColdResetStatus                          **
**                    (                                                       **
**                      void                                                  **
**                    )                                                       **
**                                                                            **
** Service ID       : None                                                    **
**                                                                            **
** Sync/Async       : Synchronous                                             **
**                                                                            **
** Reentrancy       : Non reentrant                                           **
**                                                                            **
** Parameters (in):   None                                                    **
**                                                                            **
** Parameters (out) : None                                                    **
**                                                                            **
** Return value:       None                                                   **
**                                                                            **
** Description      : clear the SCU_RSTSTAT register value                    **
**                                                                            **
*******************************************************************************/
extern void Mcu_ClearColdResetStatus( void );

#endif /* (MCU_CLR_COLD_RESET_STAT_API == STD_ON) */

/*******************************************************************************
**                      Global Inline Function Definitions                    **
*******************************************************************************/

/* MCU108, MCU104 : This service is only available if
   the preprocessor switch MCU_VERSION_INFO_API is set to TRUE*/
/*******************************************************************************
** Traceability : [cover parentID=DS_AS_MCU101, DS_AS_MCU103,DS_AS_MCU204,
                   DS_AS_MCU162, DS_AS_MCU149]                                **
** Syntax     : void Mcu_GetVersionInfo( Std_VersionInfoType *VersionInfoPtr) **
**                                                                            **
** [/cover]                                                                   **
**                                                                            **
** Service ID   : 9                                                           **
**                                                                            **
** Sync/Async   : Synchronous                                                 **
**                                                                            **
** Reentrancy   : Non-reentrant                                               **
**                                                                            **
** Parameters (in) : None                                                     **
**                                                                            **
** Parameters (out): VersionInfoPtr (Pointer to store the version             **
**                   information of this module)                              **
**                                                                            **
** Return value : None                                                        **
**                                                                            **
** Description  : This service returns the version information of this module.**
**               The version information includes:                            **
**               - Module Id                                                  **
**               - Vendor Id                                                  **
**               - Instance Id (Applicable only for MCU SWS 2.2.0)            **
**               - Vendor specific version numbers (BSW00407)                 **
**                                                                            **
*******************************************************************************/
#if ((MCU_VERSION_INFO_API == STD_ON) && (MCU_DEV_ERROR_DETECT == STD_ON))
/*IFX_MISRA_RULE_19_07_STATUS=GetVersionInfo is declared as a function like
 macro as per AUTOSAR*/
/*IFX_MISRA_RULE_19_04_STATUS=GetVersionInfo cannot be expand to a braced
 initialiser*/
#define Mcu_GetVersionInfo(VersionInfoPtr)                                   \
{                                                                         \
  if ((VersionInfoPtr) == NULL_PTR)                                            \
  {                                                                       \
    /* Report to DET */                                                   \
    Det_ReportError(                                                      \
                     (uint16)MCU_MODULE_ID,                               \
                     MCU_MODULE_INSTANCE,                                 \
                     MCU_SID_GETVERSIONINFO,                              \
                     MCU_E_PARAM_POINTER                                  \
                   );                                                     \
  }                                                                       \
  else                                                                    \
  {                                                                       \
    /* MCU Module ID */                                                   \
    ((Std_VersionInfoType*)(VersionInfoPtr))->moduleID = MCU_MODULE_ID;   \
    /* MCU vendor ID */                                                   \
    ((Std_VersionInfoType*)(VersionInfoPtr))->vendorID = MCU_VENDOR_ID;   \
    /* major version of MCU */                                            \
    ((Std_VersionInfoType*)(VersionInfoPtr))->sw_major_version =          \
                                    (uint8)MCU_SW_MAJOR_VERSION;          \
    /* minor version of MCU */                                            \
    ((Std_VersionInfoType*)(VersionInfoPtr))->sw_minor_version =          \
                                    (uint8)MCU_SW_MINOR_VERSION;          \
    /* patch version of MCU */                                            \
    ((Std_VersionInfoType*)(VersionInfoPtr))->sw_patch_version =          \
                                    (uint8)MCU_SW_PATCH_VERSION;          \
  }                                                                       \
}
#elif ((MCU_VERSION_INFO_API == STD_ON) && (MCU_DEV_ERROR_DETECT == STD_OFF))
/*IFX_MISRA_RULE_19_07_STATUS=GetVersionInfo is declared as a function like
 macro as per AUTOSAR*/
 /*IFX_MISRA_RULE_19_04_STATUS=GetVersionInfo cannot be expand to a braced
 initialiser*/
#define Mcu_GetVersionInfo(VersionInfoPtr)                                \
{                                                                         \
  /* MCU Module ID */                                                     \
  ((Std_VersionInfoType*)(VersionInfoPtr))->moduleID = MCU_MODULE_ID;     \
  /* MCU vendor ID */                                                     \
  ((Std_VersionInfoType*)(VersionInfoPtr))->vendorID = MCU_VENDOR_ID;     \
  /* major version of MCU */                                              \
  ((Std_VersionInfoType*)(VersionInfoPtr))->sw_major_version =            \
                                  (uint8)MCU_SW_MAJOR_VERSION;            \
  /* minor version of MCU */                                              \
  ((Std_VersionInfoType*)(VersionInfoPtr))->sw_minor_version =            \
                                  (uint8)MCU_SW_MINOR_VERSION;            \
  /* patch version of MCU */                                              \
  ((Std_VersionInfoType*)(VersionInfoPtr))->sw_patch_version =            \
                                  (uint8)MCU_SW_PATCH_VERSION;            \
}
#else
/*IFX_MISRA_RULE_19_07_STATUS=Function like macro used to generate compile time
 error in case of incorrect configuration*/
#define Mcu_GetVersionInfo(VersionInfo) (ERROR_Mcu_GetVersionInfo_NOT_SELECTED)
#endif

/*******************************************************************************
** Traceability : [cover parentID=DS_NAS_MCU_PR2878]                          **
** Syntax : _IFXEXTERN_ IFX_INLINE uint32 Mcu_ChkWkpStdby(void)               **
** [/cover]                                                                   **
**                                                                            **
** Sync/Async:  Synchronous                                                   **
**                                                                            **
** Reentrancy:  non reentrant                                                 **
**                                                                            **
** Parameters (in): none                                                      **
**                                                                            **
** Parameters (out):  none                                                    **
**                                                                            **
** Return value:  Status bits of PMSWSTAT register                            **
**                                                                            **
** Description : Function to return status bits of Wakeup bits                **
**                                                                            **
*******************************************************************************/
_IFXEXTERN_ IFX_INLINE uint32 Mcu_ChkWkpStdby(void)
/*IFX_MISRA_RULE_08_05_STATUS=Allowed for inline functions defined
 in header files*/
{
  return ((uint32)SCU_PMSWSTAT.U);
}
/*******************************************************************************
** Traceability : [cover parentID=DS_NAS_MCU_PR2879]                          **
** Syntax : _IFXEXTERN_ IFX_INLINE void Mcu_ClrWkpStdby(uint32 WkpBits)       **
** [/cover]                                                                   **
**                                                                            **
** Sync/Async:  Synchronous                                                   **
**                                                                            **
** Reentrancy:  non reentrant                                                 **
**                                                                            **
** Parameters (in): WkpBits - Bits to be cleared                              **
**                                                                            **
** Parameters (out):  none                                                    **
**                                                                            **
** Return value:  none                                                        **
**                                                                            **
** Description : Function to clear status bits of Wakeup pins                 **
**                                                                            **
*******************************************************************************/
_IFXEXTERN_ IFX_INLINE void Mcu_ClrWkpStdby(uint32 WkpBits)
/*IFX_MISRA_RULE_08_05_STATUS=Allowed for inline functions defined
 in header files*/
{
  /*IFX_MISRA_RULE_08_05_STATUS=Allowed for inline functions defined
   in header files for lSetMask*/
  /*IFX_MISRA_RULE_08_05_STATUS=Allowed for inline functions defined
   in header files for lClearMask*/
  uint32 lSetMask,lClearMask;
  lSetMask = Mcal_GetSetMask(WkpBits,0xFFFFFFFFU,0U);
  lClearMask = Mcal_GetClearMask(WkpBits,0xFFFFFFFFU,0U);
  #if (MCU_SAFETY_ENABLE == STD_OFF)
  Mcal_UpdateRegSV((volatile uint32 *)(volatile void*)&SCU_PMSWSTATCLR,\
  lSetMask,lClearMask,(uint8)MCU_SAFETY_ENDINIT_PROTECTION,MCU_MODULE_ID);
  #else
  Mcal_UpdateRegSV((volatile uint32 *)(volatile void*)&SCU_PMSWSTATCLR,\
    lSetMask,lClearMask,(uint8)MCU_NO_ENDINIT_PROTECTION,MCU_MODULE_ID);
  #endif
}

#if (MCU_SET_STANDBY_CONTROL_API == STD_ON)
/*******************************************************************************
** Traceability : [cover parentID=DS_NAS_MCU_PR2881]                          **
**                                                                            **
** Syntax : _IFXEXTERN_ IFX_INLINE void Mcu_SetStandbyCtrlReg                 **
**          (                                                                 **
**             uint32   PMSWCR0_RegVal                                        **
**          )                                                                 **
** [/cover]                                                                   **
**                                                                            **
** Service ID:    NA                                                          **
**                                                                            **
** Sync/Async:    Synchronous                                                 **
**                                                                            **
** Reentrancy:    Non reentrant                                               **
**                                                                            **
** Parameters (in):   PMSWCR0_RegVal : PMSWCR0 register value                 **
**                    the configuration set                                   **
**                                                                            **
** Parameters (out):  None                                                    **
**                                                                            **
** Return value:      None                                                    **
**                                                                            **
** Description : This function updates PMSWCR0 during runtime for standby mode**
**                                                                            **
*******************************************************************************/
_IFXEXTERN_ IFX_INLINE void Mcu_SetStandbyCtrlReg(uint32 PMSWCR0_RegVal)
/*IFX_MISRA_RULE_08_05_STATUS=Allowed for inline functions defined
 in header files*/
{
  /*IFX_MISRA_RULE_08_05_STATUS=Allowed for inline functions defined
   in header files for lSetMask*/
  /*IFX_MISRA_RULE_08_05_STATUS=Allowed for inline functions defined
   in header files for lClearMask*/
  uint32 lSetMask,lClearMask;
  lSetMask = Mcal_GetSetMask(PMSWCR0_RegVal,0xFFFFFFFFU,0U);
  lClearMask = Mcal_GetClearMask(PMSWCR0_RegVal,0xFFFFFFFFU,0U);
  #if (MCU_SAFETY_ENABLE == STD_OFF)
  Mcal_UpdateRegSV((volatile uint32 *)(volatile void*)(&MODULE_SCU.PMSWCR0.U),\
      lSetMask,lClearMask,(uint8)MCU_SAFETY_ENDINIT_PROTECTION,MCU_MODULE_ID);
  #else
  Mcal_UpdateRegSV((volatile uint32 *)(volatile void*)(&MODULE_SCU.PMSWCR0.U),\
      lSetMask,lClearMask,(uint8)MCU_NO_ENDINIT_PROTECTION,MCU_MODULE_ID);
  #endif
}/*End of Mcu_SetStandbyCtrlReg()*/
#endif /* (MCU_SET_STANDBY_CONTROL_API == STD_ON) */

#define MCU_STOP_SEC_CODE
#include "MemMap.h"

#endif  /* End of  MCU_H */
