#if 0
#include "ql_type.h"
#include "ql_gpio.h"
#include "ql_error.h"
//#include "ql_clock.h"
#include "ql_func.h"


/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
<       Definition for  OpenCPU clock APIs
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/


/************************************************************************************************
*   clock
*************************************************************************************************/
typedef s32 (*_api_CLK_Init_t)(Enum_PinName clkName,Enum_ClockSource clkSrc);
typedef s32 (* _api_CLK_Uninit_t)(Enum_PinName clkName);
typedef s32 (* _api_CLK_Output_t)(Enum_PinName pinName,bool clkOnOff);


/************************************************************************************************
*   clock
*************************************************************************************************/
_api_CLK_Init_t m_CLK_Init=NULL;

_api_CLK_Uninit_t m_CLK_Uninit=NULL;

_api_CLK_Output_t m_CLK_Output=NULL;


s32 Ql_CLK_Init(Enum_PinName clkName,Enum_ClockSource clkSrc)
{   
	if (NULL == m_CLK_Init)
	{
		s32 ptr = m_get_api_ptr("Ql_CLK_Init");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_CLK_Init");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_CLK_Init=(_api_CLK_Init_t)ptr;
    }
	return m_CLK_Init(clkName,clkSrc);
}


s32 Ql_CLK_Uninit(Enum_PinName clkName)
{   
	if (NULL == m_CLK_Uninit)
	{
		s32 ptr = m_get_api_ptr("Ql_CLK_Uninit");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_CLK_Uninit");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_CLK_Uninit=(_api_CLK_Uninit_t)ptr;
    }
	return m_CLK_Uninit(clkName);
}

s32 Ql_CLK_Output(Enum_PinName pinName,bool clkOnOff)
{   
	if (NULL == m_CLK_Output)
	{
		s32 ptr = m_get_api_ptr("Ql_CLK_Output");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_CLK_Output");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_CLK_Output=(_api_CLK_Output_t)ptr;
    }
	return m_CLK_Output(pinName,clkOnOff);
}

#endif

