#ifndef STM_CFG_H
#define STM_CFG_H


/*==========================================================================*/
/*      G L O B A L   M A C R O   D E F I N I T I O N S                     */
/*==========================================================================*/

/* Register */
#define BL_SCU_CCUCON1		(*((uint32 volatile *) 0xF0036034))
/* Compare Match Control Register */
#define	BL_STM0_CMCON		(*((uint32 volatile *) 0xF0000038))
/* Compare Register */
#define BL_STM0_CMP0		(*((uint32 volatile *) 0xF0000030))
/* Timer Register 0 */
#define BL_STM0_TIM0		(*((uint32 volatile *) 0xF0000010))
/* Interrupt Control Register */
#define BL_STM0_ICR		(*((uint32 volatile *) 0xF000003C))
/* Interrupt Set/Clear Register */
#define	BL_STM0_ISCR		(*((uint32 volatile *) 0xF0000040))

/* System timer register value define */
#define CFG_STM0_CMP0_VALUE		1000000U
#define STM0_CMP0_PERIOD_TIME   10 /* 10ms */


#endif /*#ifndef STM_CFG*/
