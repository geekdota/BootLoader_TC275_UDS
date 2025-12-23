/*******************************************************************************
**                                                                            **
** Copyright (C) Infineon Technologies (2012)                                 **
**                                                                            **
** All rights reserved.                                                       **
**                                                                            **
** This document contains proprietary information belonging to Infineon       **
** Technologies. Passing on and copying of this document, and communication   **
** of its contents is not permitted without prior written authorization.      **
**                                                                            **
********************************************************************************
**                                                                            **
**  FILENAME  : Mcu_Cfg.h                                                     **
**                                                                            **
**  $CC VERSION : \main\65 $                                                 **
**                                                                            **
**  DATE, TIME: 2017-12-08, 08:56:45                                          **
**                                                                            **
**  GENERATOR : Build b141014-0350                                            **
**                                                                            **
**  AUTHOR    : DL-AUTOSAR-Engineering                                        **
**                                                                            **
**  VENDOR    : Infineon Technologies                                         **
**                                                                            **
**  DESCRIPTION  : MCU configuration generated out of ECU configuration       **
**                 file (Mcu.bmd)                                             **
**                                                                            **
**  MAY BE CHANGED BY USER [yes/no]: no                                       **
**                                                                            **
*******************************************************************************/

#ifndef MCU_CFG_H
#define MCU_CFG_H

/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/

/*******************************************************************************
**                      Global Macro Definitions                              **
*******************************************************************************/

/* Autosar version of the module */
#define MCU_AS_VERSION (403)

/* AUTOSAR specification version numbers */
#define MCU_AR_RELEASE_MAJOR_VERSION      (4U)
#define MCU_AR_RELEASE_MINOR_VERSION      (0U)
#define MCU_AR_RELEASE_REVISION_VERSION   (3U)

/* Vendor specific implementation version information */
#define MCU_SW_MAJOR_VERSION   (4U)
#define MCU_SW_MINOR_VERSION   (4U)
#define MCU_SW_PATCH_VERSION   (1U)





/* MCU module instance ID*/
#define MCU_MODULE_INSTANCE             ((uint8)0)

/*
              Container : McuGeneralConfiguration
              Multiplicity: 1 -1
*/
                    /* Total no. of config sets */
                    
#define MCU_CONFIG_COUNT   (1U)

/*
   Configuration: MCU_DEV_ERROR_DETECT  MCU118, MCU013, MCU100
   Preprocessor switch for enabling the development error detection and
   reporting.
   ON  : DET is Enabled
   OFF : DET is Disabled
*/
#define MCU_DEV_ERROR_DETECT  (STD_OFF)

/*
   Configuration: MCU_VERSION_INFO_API MCU118_Conf , MCU168_Conf
   Preprocessor switch to enable / disable the API to read out the modules
   version information.
   ON  : version info API is enabled
   OFF : version info API is disabled
*/
#define MCU_VERSION_INFO_API (STD_OFF)

/*
    Configuration : MCU_PERFORM_RESET_API  MCU118, MCU055, MCU167
    Preprocessor switch to enable / disable the use of the
    function Mcu_PerformReset()
    ENABLED  : Mcu_PerformReset function is available
    DISABLED : Mcu_PerformReset function is not available
*/
#define  MCU_PERFORM_RESET_API   (STD_ON)

/*
    MCU181_Conf:
    Configuration : MCU_GET_RAM_STATE_API (McuGetRamStateApi)
    Pre-processor switch to enable/disable the API Mcu_GetRamState.
    (If the H/W does not support the functionality, this parameter
    can be used to disable the Api).
    Since this feature is not present in the hardware, it is permanently
    stuck at STD_OFF.
*/
#define  MCU_GET_RAM_STATE_API (STD_OFF)

/*
    Configuration : MCU_CLR_COLD_RESET_STAT_API (McuClearColdResetApi)
    Pre-processor switch to enable/disable the API Mcu_ClearColdResetStatus.
*/
#define  MCU_CLR_COLD_RESET_STAT_API (STD_OFF)

/*
    Configuration : MCU_DEINIT_API (McuDeInitApi)
    Pre-processor switch to enable/disable the API Mcu_DeInit.
*/
#define MCU_DEINIT_API (STD_ON)

/*
    Configuration : MCU_SAFETY_ENABLE
    Pre-processor switch to enable/disable the MCU driver safety features.
*/
#define MCU_SAFETY_ENABLE (STD_OFF)


/*
    Configuration : MCU_FMPLL_ENABLE (McuFmPllEnable)
    Pre-processor switch to enable/disable the PLL frequency modulation.
*/
#define MCU_FMPLL_ENABLE (STD_OFF)


/* Reset value of PMSWCR0. TRISTEN is set, so that TRISTREQ can be written */
#define MCU_PMSWCR0_DEINIT_VALUE             ((uint32)0x002002D0U)

/*
    Configuration : MCU_ENABLE_CLOCK_MONITORING (McuClockMonitoringEnable)
    Pre-processor switch to enable/disable clock monitoring safety measure.
*/
#define MCU_ENABLE_CLOCK_MONITORING (STD_OFF)
/*
    Configuration : MCU_DISABLE_IRADIS (McuIdleReqAckSeqDisable)
    Pre-processor switch to enable/disable IRADIS bit in PMSWCR1.
*/
#define MCU_DISABLE_IRADIS (STD_OFF)

/*
    Configuration : MCU_RAMP_TO_BACKUP_FREQ_API (McuRampToBackupFreqApi)
    Pre-processor switch to enable/disable the API Mcu_RampToBackUpClockFreq.
*/
#define MCU_RAMP_TO_BACKUP_FREQ_API (STD_OFF)

/*
    Configuration : MCU_SET_STANDBY_CONTROL_API (McuSetStandbyWakeupControlApi)
    Pre-processor switch to enable/disable the API Mcu_SetStandbyCtrlReg.
*/
#define MCU_SET_STANDBY_CONTROL_API (STD_OFF)

/*
    Configuration : MCU_INIT_CLOCK
    Preprocessor switch to enable / disable the use of the
    function Mcu_InitClock()
    ENABLED  : Mcu_InitClock function is available
    DISABLED : Mcu_InitClock function is not available
*/

#define  MCU_INIT_CLOCK (STD_ON)


/* Configuration : McuPBFixedAddress
   This parameter enables the user to switch ON/OFF the
   PostBuild Fixed Address Feature
*/
#define MCU_PB_FIXEDADDR                (STD_OFF)

/*
    Configuration: MCU_MAIN_OSC_FREQ  (MHz)
    This parameter defines external Oscillator frequency in MHz
    This parameter should be configured to the external Oscillator frequency
    value
*/
#define MCU_MAIN_OSC_FREQ  (20U)


#define MCU_DELAY_COUNT_DIVIDER   (2U)

/*
    Configuration: MCU_DISABLE_ERAY_PLL
    This parameter defines if ERAY PLL should be enabled/disabled
    ERAY PLL shall be disabled if Oscillator frequency < 16 MHz
*/
#define MCU_DISABLE_ERAY_PLL (STD_OFF)


/*
    Configuration: MCU_BACKUP_FREQ  (MHz)
    This parameter defines back up frequency in MHz
*/
#define MCU_BACKUP_FREQ    (100U)

/* OSC Frequency Value
    This bit field defines the divider value that generates
    the reference clock that is supervised by the
    oscillator watchdog. fOSC is divided by OSCVAL + 1
    in order to generate fOSCREF.
    Equation is OSCVAL = (Fosc/2.5 - 1). Xpath eq. becomes (10*Fosc/25 - 1)*/
#define MCU_OSCVAL_REG_VALUE  (0x7U)

/* Master Core for triggering System Modes for controller
 1 - Core 0
 2 - Core 1
 4 - Core 2
 7 - All Cores Unanimously
*/
#define MCU_SYSTEM_MODE_CORE            ((uint8)1U)

/* Master Core for triggering Idle Mode for controller
 1 - Core 0
 2 - Core 1
 4 - Core 2
 0 - All Cores Individually
*/
#define MCU_IDLE_MODE_CORE              ((uint8)0U)

/*
              Container: GtmConfiguration
              Multilplicity: 0 -1
*/
    /* Check if GTM is configured */

#define MCU_GTM_USED (STD_OFF)

/* CRC Initialization
   FCE Clock and configuration based on the FCE Kernel used
*/
#define MCU_CRC_HW_USED                 (STD_OFF)


/* DMA Initialization */
#define MCU_DMA_USED                    (STD_OFF)

/* MCU SW Reset configuration */
/*
              Container: McuRamSectorSettingConf
              Multiplicity: 1 -*
*/
/*    Configuration  MCU_RAM_SECTORS MCU120_Conf
      This parameter shall represent the number
      of RAM sectors available for the MCU
*/

#define MCU_RAM_SECTORS     (0U)

/*
              Container: McuModeSettingConf
              Multiplicity: 1 -*
*/
/*
    Configuration Options:
    For MCU power down modes Mcu_ModeType MCU035:
    IDLE Mode: CPU clock is disabled and peripheral clock is running
    as configured
*/
#define MCU_IDLE  ((Mcu_ModeType)(0U))
/*
    Configuration Options:
    For MCU power down modes Mcu_ModeType MCU035:
    SLEEP Mode: CPU clock is disabled and Peripherals go to sleep if
    configured for sleep mode
*/
#define MCU_SLEEP ((Mcu_ModeType)(1U))
/*
    Configuration Options:
    For MCU power down modes Mcu_ModeType MCU035:
    STANDBY Mode: PLL and ERAY_PLL are disabled
*/
#define MCU_STANDBY ((Mcu_ModeType)(2U))
/*
    Maximum number of modes which are configurable is 3
*/
#define MCU_MAX_NO_MODES ((Mcu_ModeType)(3U))

/*
    Configuration: Mode Id
    This id corresponds to the container name of the McuModeSettingConfiguration
    Use this as the parameter for API: Mcu_SetMode
*/


/* To prevent double declaration */
#ifndef McuConf_McuModeSettingConf_McuModeSettingConf_0
#define McuConf_McuModeSettingConf_McuModeSettingConf_0  ((Mcu_ModeType)(0U))
#endif

/*
    Configuration : MCU_MODE  MCU123_Conf
    This parameter shall represent the different MCU Modes
    available to a MCU .For AURIX the maximum number of modes available are 3
    The parameter is bitwise representation.
    Value = 0(Bit 0)- Used for Idle Mode.If 1 - idle mode can be requested.
    Value = 2(Bit 1)- Used for Sleep Mode.If 1 - sleep mode can be requested.
    Value = 4(Bit 2)- Used for Standby Mode.If 1 -standby mode can be requested.
*/

#define MCU_MODE_MAX0     ((Mcu_ModeType)(1U))

/*
              Container: McuClockSettingConfig
              Multiplicity: 1 -*
*/

/*To prevent double declaration*/
#ifndef McuConf_McuClockSettingConfig_McuClockSettingConfig_0
#define McuConf_McuClockSettingConfig_McuClockSettingConfig_0  ((Mcu_ClockType)(0U))
#endif



/*  PLL delay ticks calculated as per back up clock Freqeuency (100MHz) */
#define MCU_CONF_DELAY_PLL  (500U)



/* ERAY PLL delay ticks calculated as per back up clock Freqeuency (100MHz) */
#define MCU_CONF_DELAY_ERAY_PLL  (500U)

/*
    Configuration : MCU_NUMBER_OF_CLOCK_SETTINGS MCU124
    This parameter shall represent the number of clock settings
    available for the MCU
*/
#define MCU_NUMBER_OF_CLOCK_SETTINGS ((Mcu_ClockType)(1U))

/*
              Container: McuRamSectorSettingConf
              Multiplicity: 0 -*
*/


/*
              Container: McuResetReasonConf
              Multiplicity: 1-1
*/
#ifndef McuConf_McuResetReasonConf_MCU_ESR0_RESET
#define McuConf_McuResetReasonConf_MCU_ESR0_RESET  (0)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_ESR1_RESET
#define McuConf_McuResetReasonConf_MCU_ESR1_RESET  (1)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_SMU_RESET
#define McuConf_McuResetReasonConf_MCU_SMU_RESET  (2)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_SW_RESET
#define McuConf_McuResetReasonConf_MCU_SW_RESET  (3)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_STM0_RESET
#define McuConf_McuResetReasonConf_MCU_STM0_RESET  (4)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_STM1_RESET
#define McuConf_McuResetReasonConf_MCU_STM1_RESET  (5)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_STM2_RESET
#define McuConf_McuResetReasonConf_MCU_STM2_RESET  (6)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_POWER_ON_RESET
#define McuConf_McuResetReasonConf_MCU_POWER_ON_RESET  (7)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_CB0_RESET
#define McuConf_McuResetReasonConf_MCU_CB0_RESET  (8)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_CB1_RESET
#define McuConf_McuResetReasonConf_MCU_CB1_RESET  (9)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_CB3_RESET
#define McuConf_McuResetReasonConf_MCU_CB3_RESET  (10)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_TP_RESET
#define McuConf_McuResetReasonConf_MCU_TP_RESET  (11)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_EVR13_RESET
#define McuConf_McuResetReasonConf_MCU_EVR13_RESET  (12)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_EVR33_RESET
#define McuConf_McuResetReasonConf_MCU_EVR33_RESET  (13)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_SUPPLY_WDOG_RESET
#define McuConf_McuResetReasonConf_MCU_SUPPLY_WDOG_RESET  (14)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_STBYR_RESET
#define McuConf_McuResetReasonConf_MCU_STBYR_RESET  (15)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_RESET_MULTIPLE
#define McuConf_McuResetReasonConf_MCU_RESET_MULTIPLE  (254)
#endif
#ifndef McuConf_McuResetReasonConf_MCU_RESET_UNDEFINED
#define McuConf_McuResetReasonConf_MCU_RESET_UNDEFINED  (255)
#endif

/* Dem reporting enable/disabled macro */
#define MCU_ENABLE_DEM_REPORT  (1U)
#define MCU_DISABLE_DEM_REPORT (0U)

/* DEM enable/disable for Clock Failure */
#define MCU_E_CLOCK_FAILURE_DEM_REPORT   (MCU_DISABLE_DEM_REPORT)

/* DEM enable/disable for Eray PLL Timeout */
#define MCU_E_ERAY_TIMEOUT_DEM_REPORT    (MCU_DISABLE_DEM_REPORT)

/* DEM enable/disable for Oscillator Failure */
#define MCU_E_OSC_FAILURE_DEM_REPORT     (MCU_DISABLE_DEM_REPORT)

/*******************************************************************************
**                      Global Type Definitions                               **
*******************************************************************************/

/*******************************************************************************
**                      Global Constant Declarations                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Variable Declarations                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Declarations                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Inline Function Definitions                    **
*******************************************************************************/

#endif /* end of MCU_CFG_H */

  /* General Notes
   MCU105:The code file structure shall not be defined within this specification
   completely.At this point it shall be pointed out that the code-file structure
   shall include the following file named:
   Mcu_PBcfg.c ?for post build time configurable parameters.
   This file shall contain all  post-build time configurable parameters.

  */
