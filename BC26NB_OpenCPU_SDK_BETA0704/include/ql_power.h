/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2013
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   ql_power.h 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   Power APIs defines.
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * 
 ****************************************************************************/
 

#ifndef __POWER_H__
#define __POWER_H__
#include "ql_type.h"


/*****************************************************************
* Function:     Ql_SleepEnable 
* 
* Description:
*               Set the module into sleep mode at once
*
* Return:        
*               QL_RET_OK indicates this function successes.
*		   Ql_RET_NOT_SUPPORT this function not support.	
*****************************************************************/
s32 Ql_SleepEnable(void);

/*****************************************************************
* Function:     Ql_SleepDisable 
* 
* Description:
*               Exit  the sleep mode 
*
* Return:        
*               QL_RET_OK indicates this function successes.
*		   Ql_RET_NOT_SUPPORT this function not support.	
*****************************************************************/
 s32 Ql_SleepDisable(void);

#endif
