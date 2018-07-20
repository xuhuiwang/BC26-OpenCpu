#include "ql_type.h"
#include "ql_gpio.h"
#include "ql_error.h"
#include "ql_adc.h"
#include "ql_func.h"

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
<       Definition for  OpenCPU ADC APIs
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

/************************************************************************************************
*   ADC
*************************************************************************************************/
typedef s32 (*_api_ADC_Register_t)(Enum_ADCPin adcPin,Callback_ADC callback_adc,void *customParam);
typedef s32 (* _api_ADC_Init_t)(Enum_ADCPin adcPin,u32 count,u32 interval);
typedef s32 (* _api_ADC_Sampling_t)(Enum_ADCPin adcPin,bool enable);



/************************************************************************************************
*   ADC
*************************************************************************************************/
_api_ADC_Register_t m_ADC_Register=NULL;

_api_ADC_Init_t m_ADC_Init=NULL;

_api_ADC_Sampling_t m_ADC_Sampling=NULL;


s32 Ql_ADC_Register(Enum_ADCPin adcPin,Callback_ADC callback_adc,void *customParam)
{   
	if (NULL == m_ADC_Register)
	{
		s32 ptr = m_get_api_ptr("Ql_ADC_Register");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_ADC_Register");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_ADC_Register=(_api_ADC_Register_t)ptr;
    }
	return m_ADC_Register(adcPin,callback_adc,customParam);
}


s32 Ql_ADC_Init(Enum_ADCPin adcPin,u32 count,u32 interval)
{   
	if (NULL == m_ADC_Init)
	{
		s32 ptr = m_get_api_ptr("Ql_ADC_Init");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_ADC_Init");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_ADC_Init=(_api_ADC_Init_t)ptr;
    }
	return m_ADC_Init(adcPin,count,interval);
}


s32 Ql_ADC_Sampling(Enum_ADCPin adcPin,bool enable)
{   
	if (NULL == m_ADC_Sampling)
	{
		s32 ptr = m_get_api_ptr("Ql_ADC_Sampling");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_ADC_Sampling");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_ADC_Sampling=(_api_ADC_Sampling_t)ptr;
    }
	return m_ADC_Sampling(adcPin,enable);
}


