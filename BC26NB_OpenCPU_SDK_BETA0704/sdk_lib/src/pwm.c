#include "ql_type.h"
#include "ql_gpio.h"
#include "ql_error.h"
#include "ql_pwm.h"
#include "ql_func.h"

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
<       Definition for  OpenCPU PWM APIs
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

/************************************************************************************************
*   PWM
*************************************************************************************************/
typedef s32 (*_api_PWM_Init_t)(Enum_PinName pwmPinName,Enum_PwmSource pwmSrcClk,Enum_PwmSourceDiv pwmDiv,u32 lowPulseNum,u32 highPulseNum);
typedef s32 (* _api_PWM_Uninit_t)(Enum_PinName pwmPinName);
typedef s32 (* _api_PWM_Output_t)(Enum_PinName pwmPinName,bool pwmOnOff);


/************************************************************************************************
*   PWM
*************************************************************************************************/
_api_PWM_Init_t m_PWM_Init=NULL;

_api_PWM_Uninit_t m_PWM_Uninit=NULL;

_api_PWM_Output_t m_PWM_Output=NULL;


s32 Ql_PWM_Init(Enum_PinName pwmPinName,Enum_PwmSource pwmSrcClk,Enum_PwmSourceDiv pwmDiv,u32 lowPulseNum,u32 highPulseNum)
{   
	if (NULL == m_PWM_Init)
	{
		s32 ptr = m_get_api_ptr("Ql_PWM_Init");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_PWM_Init");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_PWM_Init=(_api_PWM_Init_t)ptr;
    }
	return m_PWM_Init(pwmPinName,pwmSrcClk,pwmDiv,lowPulseNum,highPulseNum);
}


s32 Ql_PWM_Uninit(Enum_PinName pwmPinName)
{   
	if (NULL == m_PWM_Uninit)
	{
		s32 ptr = m_get_api_ptr("Ql_PWM_Uninit");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_PWM_Uninit");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_PWM_Uninit=(_api_PWM_Uninit_t)ptr;
    }
	return m_PWM_Uninit(pwmPinName);
}

s32 Ql_PWM_Output(Enum_PinName pwmPinName,bool pwmOnOff)
{   
	if (NULL == m_PWM_Output)
	{
		s32 ptr = m_get_api_ptr("Ql_PWM_Output");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_PWM_Output");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_PWM_Output=(_api_PWM_Output_t)ptr;
    }
	return m_PWM_Output(pwmPinName,pwmOnOff);
}


