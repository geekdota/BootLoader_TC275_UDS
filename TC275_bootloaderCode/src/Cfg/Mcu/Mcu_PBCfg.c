/*******************************************************************************
**                                                                            **
** Copyright (C) Infineon Technologies (2013)                                 **
**                                                                            **
** All rights reserved.                                                       **
**                                                                            **
** This document contains proprietary information belonging to Infineon       **
** Technologies. Passing on and copying of this document, and communication   **
** of its contents is not permitted without prior written authorization.      **
**                                                                            **
********************************************************************************
**                                                                            **
**  FILENAME  : Mcu_PBCfg.c                                                   **
**                                                                            **
**  $CC VERSION : \main\75 $                                                 **
**                                                                            **
**  DATE, TIME: 2017-12-08, 08:56:44                                          **
**                                                                            **
**  GENERATOR : Build b141014-0350                                            **
**                                                                            **
**  AUTHOR    : DL-AUTOSAR-Engineering                                        **
**                                                                            **
**  VENDOR    : Infineon Technologies                                         **
**                                                                            **
**  DESCRIPTION  : MCU configuration generated out of ECU configuration       **
**                 file  (Mcu.bmd)                                            **
**                                                                            **
**  MAY BE CHANGED BY USER [yes/no]: no                                       **
**                                                                            **
*******************************************************************************/

/*******************************************************************************
**                       Includes                                             **
*******************************************************************************/

/* Include module header file */
#include "Mcu.h"
/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/

/*
   Configuration Options:
   For the clock setting configuration
   PLL status
*/

/* Normal Mode */
#define MCU_NORMAL_MODE        (0x01U)

/******************************************************************************
**                      Global Constant Definitions                           **
*******************************************************************************/

/* Memory Mapping the configuration constant */
#define MCU_START_SEC_POSTBUILDCFG
#include "MemMap.h"

/*
   Configuration : Mcu_ConfigType
   For Mcu_ConfigType: MCU specific configuration ID
   This id corresponds to the container name of the McuConfiguration.
   Use this as the parameter for API: Mcu_Init
    - MCU035 : Configuration of MCU
*/
/*
                     Container: McuClockSettingConfig
                     Multilplicity : 1 - *
*/

static const Mcu_ClockCfgType Mcu_kClockConfiguration0[] =
{
    /*McuClockSettingConfig_0*/
  {
    {
      /*K2 divider steps*/
      5U, /* 100.0 MHz */ 
      4U, /* 120.0 MHz  */
      3U, /* 150.0 MHz  */
      2U, /* 200.0 MHz  */
      2U, /* 200.0 MHz  */
      2U,  /* 200.0 MHz */ 
      0U,
      0U,
    },
    {
      500U,  /* ramp up delay for K2 div step 1*/
      600U, /* ramp up delay for K2 div step 2*/
      750U, /* ramp up delay for K2 div step 3*/
      1000U, /* ramp up delay for K2 div step 4*/
      1000U, /* ramp up delay for K2 div step 5*/
      1000U, /* ramp up delay for K2 div step 6*/
      0U, /* ramp up delay for K2 div step 7*/
      0U, /* ramp up delay for K2 div step 8*/
    },

    {
      1U,                              /* K1DIV */
      5U,                              /* K3DIV */
      29U,                             /* NDIV */
      0U,                              /* PDIV */
      4U,                              /* K2DivSteps */
      MCU_NORMAL_MODE,                 /* NORMAL/PRESCALER MODE */
      0U                               /* reserved */
    },
    {
      19U,                             /* PLL ERAY N divider */
      1U,                              /* PLL ERAY K2 divider */
      3U,                              /* PLL ERAY K3 divider */
      0U,                              /* PLL ERAY P Divider */
      0U                               /* reserved */
    },
    /*
      SRI Clock     = 200.0MHz
      SPB Clock     = 100.0MHz
    */
    (uint32)0x02120122U, /* CCUCON0 Register Configuration*/
    (uint32)0x02212242U, /* CCUCON1 Register Configuration*/
    (uint32)0x00000002U, /* CCUCON2 Register Configuration*/
    (uint32)0x00000041U,/* CCUCON5 Register Configuration*/
    (uint32)0x00000000U, /* CCUCON6 Register Configuration*/
    (uint32)0x00000000U, /* CCUCON7 Register Configuration*/
    (uint32)0x00000000U,  /* CCUCON8 Register Configuration*/
    10U, /* Ramp to PLL delay configured in us */

  }/*McuClockSettingConfig_0*/

};


/* Main MCU Configuration Structure */
const Mcu_ConfigType Mcu_ConfigRoot[1] =
{
  {
    /*McuModuleConfiguration_0*/
    
    /*Mcu Clock Configuration*/
    Mcu_kClockConfiguration0,
    #if (MCU_RAM_SECTORS != 0U)
    /*Mcu Ram Configuration is NULL_PTR as there is nothing configured here */
    NULL_PTR,
    #endif /* #if (MCU_RAM_SECTORS != 0U) */
    /* Reset request configuration  */
    /* 
     * Upper 16-bits contain ARSTDIS register configuration value, 
     * lower 16-bits contain RSTCON register configuration value
     */
    0x200U,
    /* Total number of Clock settings */
    ((Mcu_ClockType)1U),
    /* Total number of RAM Sectors */
    ((Mcu_RamSectionType)0U),
    /* The maximum mode that is supported by this module instance */
    (MCU_MODE_MAX0),
    /* Ptr to Standby Mode in config structure */
    NULL_PTR,
  }
};

#define MCU_STOP_SEC_POSTBUILDCFG
#include "MemMap.h"
