/*============================================================================*/
/** Copyright (C) 2009-2018, iSOFT INFRASTRUCTURE SOFTWARE CO.,LTD.
 *
 *  All rights reserved. This software is iSOFT property. Duplication
 *  or disclosure without iSOFT written authorization is prohibited.
 *
 *  @file       <Dcm_Cfg.c>
 *  @brief      <UDS Service - ISO14229>
 *
 *  <This Diagnostic Communication Manager file contained UDS services
 *   which used for bootloader project>
 *
 *  <Compiler: HighTec4.6    MCU:TC27x>
 *
 *  @author     <Gary Chen>
 *  @date       <2012-11-09>
 */
/*============================================================================*/

/*=======[R E V I S I O N   H I S T O R Y]====================================*/
/** <VERSION>  <DATE>  <AUTHOR>     <REVISION LOG>
 *  V1.0    20121109    Gary       Initial version
 *
 *  V1.1    20130517    liuyp      modified version
 *
 *  V1.2    20130913    ccl        update
 *
 *  V1.3    20161015    cywang     update

 *  V1.4    20180522    CC         update
 */
/*============================================================================*/

/*=======[I N C L U D E S]====================================================*/
#include "dcm_types.h"
#include "dcm_dsp.h"
#include "dcm_internel.h"

/*=======[E X T E R N A L   D A T A]==========================================*/
const Dcm_ServiceTableType Dcm_ServiceTable[DCM_SERVICE_NUM] =
{
    /* SID, funcAddrSupport,  sessionSupp,      ServiceFct,  pendingFct */
    /* @type:uint8 range:0x00~0xFF note:select unique service Id */
    /* @type:boolean range:TRUE,FALSE note:NONE */
    /* @type:uint8 range:session value note:select defined session value */
    /* @type:API range:NONE note:auto generate */
    /* @type:API range:NONE note:auto generate */
    {
        (uint8)0x10u, (uint8)DCM_PHYSICAL_ADDRESSING|(uint8)DCM_FUNCTIONAL_ADDRESSING,
        (uint8)DCM_SESSION_DEFAULT|(uint8)DCM_SESSION_PROGRAMMING|(uint8)DCM_SESSION_EXTEND,
        &Dcm_RecvMsg10, NULL_PTR
    },
#if (STD_ON == DCM_SERVICE_11_ENABLE)
    {
        (uint8)0x11u, (uint8)DCM_PHYSICAL_ADDRESSING|(uint8)DCM_FUNCTIONAL_ADDRESSING,
        (uint8)DCM_SESSION_DEFAULT|(uint8)DCM_SESSION_PROGRAMMING|(uint8)DCM_SESSION_EXTEND,
        &Dcm_RecvMsg11, NULL_PTR
    },
#endif
#if (STD_ON == DCM_SERVICE_22_ENABLE)
    {
        (uint8)0x22u, (uint8)DCM_PHYSICAL_ADDRESSING,
        (uint8)DCM_SESSION_DEFAULT|(uint8)DCM_SESSION_PROGRAMMING|(uint8)DCM_SESSION_EXTEND,
        &Dcm_RecvMsg22, NULL_PTR
    },
#endif
    {
        (uint8)0x27u, (uint8)DCM_PHYSICAL_ADDRESSING,
        (uint8)DCM_SESSION_PROGRAMMING,
        &Dcm_RecvMsg27, NULL_PTR
    },
    {
        (uint8)0x28u, (uint8)DCM_PHYSICAL_ADDRESSING|(uint8)DCM_FUNCTIONAL_ADDRESSING,
        (uint8)DCM_SESSION_EXTEND,
        &Dcm_RecvMsg28, NULL_PTR
    },
    {
        (uint8)0x2Eu, (uint8)DCM_PHYSICAL_ADDRESSING,
         (uint8)DCM_SESSION_PROGRAMMING,
        &Dcm_RecvMsg2E, &Dcm_Pending2E
    },
    {
        (uint8)0x31u, (uint8)DCM_PHYSICAL_ADDRESSING,
        (uint8)DCM_SESSION_PROGRAMMING|(uint8)DCM_SESSION_EXTEND,
        &Dcm_RecvMsg31, &Dcm_Pending31
    },
    {
        (uint8)0x34u, (uint8)DCM_PHYSICAL_ADDRESSING,
        (uint8)DCM_SESSION_PROGRAMMING,
        &Dcm_RecvMsg34, NULL_PTR
    },
    {
        (uint8)0x36u, (uint8)DCM_PHYSICAL_ADDRESSING,
        (uint8)DCM_SESSION_PROGRAMMING,
        &Dcm_RecvMsg36, &Dcm_Pending36
    },
    {
        (uint8)0x37u, (uint8)DCM_PHYSICAL_ADDRESSING,
        (uint8)DCM_SESSION_PROGRAMMING,
        &Dcm_RecvMsg37, NULL_PTR
    },
    {
        (uint8)0x3Eu, (uint8)DCM_PHYSICAL_ADDRESSING|(uint8)DCM_FUNCTIONAL_ADDRESSING,
        (uint8)DCM_SESSION_DEFAULT|(uint8)DCM_SESSION_PROGRAMMING|(uint8)DCM_SESSION_EXTEND,
        &Dcm_RecvMsg3E, NULL_PTR
    },
    {
        (uint8)0x85u, (uint8)DCM_PHYSICAL_ADDRESSING|(uint8)DCM_FUNCTIONAL_ADDRESSING,
        (uint8)DCM_SESSION_EXTEND,
        &Dcm_RecvMsg85, NULL_PTR
    }
};

const Dcm_SessionRowType Dcm_SessionRow[DCM_SESSION_NUM] =
{
    /* sessionType, sessionSupp,     securitySupp */
    /* @type:uint8 range:0x00~0xFF note:select unique session value */
    /* @type:uint8 range:session value note:select defined session value */
    /* @type:uint8 range:security value note:select defined security value */
    {
        (uint8)DCM_SESSION_DEFAULT,
        DCM_SESSION_DEFAULT|DCM_SESSION_PROGRAMMING|DCM_SESSION_EXTEND,
        DCM_SECURITY_LOCKED|DCM_SECURITY_LEV1
    },
    {
        (uint8)DCM_SESSION_PROGRAMMING,
        DCM_SESSION_PROGRAMMING|DCM_SESSION_EXTEND,
        DCM_SECURITY_LOCKED|DCM_SECURITY_LEV1
    },
    {
        (uint8)DCM_SESSION_EXTEND,
        DCM_SESSION_DEFAULT|DCM_SESSION_EXTEND,
        DCM_SECURITY_LOCKED|DCM_SECURITY_LEV1
    }
};

#if (STD_ON == DCM_SERVICE_11_ENABLE)
const Dcm_ResetRowType Dcm_ResetRow[DCM_RESET_NUM] =
{
    /* resetType, sessionSupp,     securitySupp */
    /* @type:uint8 range:0x00~0xFF note:NONE */
    /* @type:uint8 range:session value note:select defined session value */
    /* @type:uint8 range:security value note:select defined security value */
    {
        (uint8)DCM_HARD_RESET,
       	DCM_SESSION_DEFAULT|DCM_SESSION_PROGRAMMING|DCM_SESSION_EXTEND,
        DCM_SECURITY_LOCKED|DCM_SECURITY_LEV1
    }
};
#endif

#if ((DCM_READDID_NUM > 0) && (STD_ON == DCM_SERVICE_22_ENABLE))
const Dcm_ReadDidRowType Dcm_ReadDidRow[DCM_READDID_NUM] =
{
    /* DID, securitySupp,readDataFct */
    /* @type:uint16 range:0x0000~0xFFFF note:NONE */
    /* @type:uint8 range:security value note:select defined security value */
    /* @type:API range:NONE note:input APIname */

    {
		 0xF183u,
		 DCM_SECURITY_LOCKED|DCM_SECURITY_LEV1,
		 &FL_ReadBootSWReferenceNumber
    },
    {
		 0xF184u,
		 DCM_SECURITY_LOCKED|DCM_SECURITY_LEV1,
		 &FL_ReadFingerPrint
    }

};
#endif

const Dcm_SecurityRowType Dcm_SecurityRow[DCM_SECURITY_NUM] =
{
    /* seedId,keyId,secAccessLevel,   sessionSupp,     securitySupp */
    /* @type:uint8 range:0x00~0xFF note:NONE */
    /* @type:uint8 range:0x00~0xFF note:NONE */
    /* @type:define range:DCM_SECURITY_LEV1~DCM_SECURITY_LEV2 note:NONE */
    /* @type:uint8 range:session value note:select defined session value */
    /* @type:uint8 range:security value note:select defined security value */
    /* @type:API range:NONE note:input APIname */
    /* @type:API range:NONE note:input APIname */
    {
        (uint8)0x11u, (uint8)0x12u, (uint8)DCM_SECURITY_LEV1,
        DCM_SESSION_PROGRAMMING|DCM_SESSION_EXTEND,
        DCM_SECURITY_LOCKED|DCM_SECURITY_LEV1,
        &SecM_GenerateSeed, &SecM_CompareKey
    }
};

#if (DCM_COM_CONTROL_NUM > 0)
const Dcm_ComControlRowType Dcm_ComControlRow[DCM_COM_CONTROL_NUM] =
{
    /* controlType,sessionSupp,     securitySupp */
    /* @type:uint8 range:0x00~0xFF note:NONE */
    /* @type:uint8 range:session value note:select defined session value */
    /* @type:uint8 range:security value note:select defined security value */
    {
        (uint8)DCM_ENABLE_RXANDTX,
        (uint8)DCM_SESSION_EXTEND,
        DCM_SECURITY_LOCKED|DCM_SECURITY_LEV1
    },
	{
		(uint8)DCM_ENABLE_RXANDDISTX,
		(uint8)DCM_SESSION_EXTEND,
		DCM_SECURITY_LOCKED|DCM_SECURITY_LEV1
	},
	{
	    (uint8)DCM_DISABLE_RXANDTX,
	    (uint8)DCM_SESSION_EXTEND,
	    DCM_SECURITY_LOCKED|DCM_SECURITY_LEV1
	}
};
#endif

const Dcm_WriteDidRowType Dcm_WriteDidRow[DCM_WRITEDID_NUM] =
{
    /* DID,  dataLength,securitySupp,writeDataFct */
    /* @type:uint16 range:0x0000~0xFFFF note:NONE */
    /* @type:uint16 range:0x0001~0xFFFF note:NONE */
    /* @type:uint8 range:security value note:select defined security value */
    /* @type:API range:NONE note:input APIname */

    {
	     (uint16)0xF184u, (uint16)0x9u,
	     (uint8)DCM_SECURITY_LEV1,
	     &FL_WriteFingerPrint
	}
};

const Dcm_RoutineControlRowType Dcm_RoutineControlRow[DCM_ROUTINE_CONTROL_NUM] =
{
    /* routineId,optionLength,funcAddrSupp,sessionSupp,    securitySupp,routineControl */
    /* @type:uint16 range:0x0000~0xFFFF note:select unique routine Id */
    /* @type:uint16 range:0x0000~0xFFFF note:auto generate */
    /* @type:boolean range:TRUE,FALSE note:auto generate */
    /* @type:uint8 range:session value note:select defined session value */
    /* @type:uint8 range:security value note:select defined security value */
    /* @type:API range:NONE note:auto generate */
	{
	    (uint16)0x0203u, 0x00u, TRUE,
	    DCM_SESSION_EXTEND,
	    DCM_SECURITY_LOCKED|DCM_SECURITY_LEV1,
	    &Dcm_CheckProgPreCondition
	},
    {
        (uint16)0xFF00u, (uint8)0x09u, FALSE,
        DCM_SESSION_PROGRAMMING,
        (uint8)DCM_SECURITY_LEV1,
        &Dcm_EraseMemory
    },
    {
        /*(uint16)0x0202u, (uint8)SECM_CRC_LENGTH, FALSE,*//*daizhunsheng mask */
    	(uint16)0x0202u, (uint8)0xd, FALSE,
        DCM_SESSION_PROGRAMMING,
        (uint8)DCM_SECURITY_LEV1,
        &Dcm_CheckMemory
    },
    {
        (uint16)0xFF01u, (uint8)0x00u, FALSE,
        DCM_SESSION_PROGRAMMING,
        (uint8)DCM_SECURITY_LEV1,
        &Dcm_CheckProgDependencies
    }
};

const Dcm_DownloadRowType Dcm_DownloadRow =
{
    /* securitySupp */
    /* @type:uint8 range:security value note:select defined security value */

        (uint8)DCM_SECURITY_LEV1

};

const Dcm_testPresentRowType Dcm_testPresentRow[DCM_TESTPRESENT_NUM] =
{
    /* testPresentSupp */
    /* @type:uint8 range:zeroSubFunc value note:select defined zeroSubFunc value */
    {
        (uint8)DCM_ZERO_SUB_FUNC
    }
};

#if (DCM_DTC_SET_NUM > 0)
const Dcm_DTCSettingRowType Dcm_DTCSettingRow[DCM_DTC_SET_NUM] =
{
    /* DTCSet,sessionSupp,     securitySupp */
    /* @type:uint8 range:0x00~0xFF note:NONE */
    /* @type:uint8 range:session value note:select defined session value */
    /* @type:uint8 range:security value note:select defined security value */
    {
        (uint8)DCM_DTC_SETTING_ON,
        (uint8)DCM_SESSION_EXTEND,
        DCM_SECURITY_LOCKED|DCM_SECURITY_LEV1
    },
    {
        (uint8)DCM_DTC_SETTING_OFF,
        (uint8)DCM_SESSION_EXTEND,
        DCM_SECURITY_LOCKED|DCM_SECURITY_LEV1
    }
};
#endif

/*=======[E N D   O F   F I L E]==============================================*/
