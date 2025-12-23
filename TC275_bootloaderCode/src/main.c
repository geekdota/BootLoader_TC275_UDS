/*=======[I N C L U D E S]====================================================*/
#include "appl.h"

/*=======[F U N C T I O N   I M P L E M E N T A T I O N S]====================*/
/******************************************************************************/
/**
 * @brief               <main function>
 *
 * <task schedule and process all API. 10ms task for watch dog trigger> .
 * Service ID   :       <NONE>
 * Sync/Async   :       <Synchronous>
 * Reentrancy           <Non Reentrant>
 * @param[in]           <NONE>
 * @param[out]          <NONE>
 * @param[in/out]       <NONE>
 * @return              <NONE>
 */
/******************************************************************************/
int main(void)
{
    /* ECU Initialize */
    (void)Appl_EcuStartup();


    for ( ; ; )
    {
        /* 10ms task and watch dog trigger */
        Appl_UpdateTriggerCondition();

        /* task for flash driver and checksum process */
        FL_MainFunction();


    }

    return 1;
}

/*=======[E N D   O F   F I L E]==============================================*/
