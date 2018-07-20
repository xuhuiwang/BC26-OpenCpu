#include "ql_type.h"
#include "ql_gpio.h"
#include "ql_error.h"
#include "ql_eint.h"
#include "ql_func.h"

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
<       Definition for  OpenCPU EINT APIs
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

/************************************************************************************************
*   EINT
*************************************************************************************************/
typedef s32 (*_api_EINT_Register_t)(Enum_PinName eintPinName, Callback_EINT_Handle callback_eint,void* customParam);
typedef s32 (* _api_EINT_RegisterFast_t)(Enum_PinName eintPinName, Callback_EINT_Handle callback_eint, void* customParam);
typedef s32 (* _api_EINT_Init_t)(Enum_PinName eintPinName,Enum_EintType eintType,u32 hwDebounce,u32  swDebounce, bool automask);
typedef s32 (*_api_EINT_Uninit_t)(Enum_PinName eintPinName);
typedef s32 (* _api_EINT_GetLevel_t)(Enum_PinName eintPinName);
typedef void (* _api_EINT_Mask_t)(Enum_PinName eintPinName);
typedef void (* _api_EINT_Unmask_t)(Enum_PinName eintPinName);


/************************************************************************************************
*   EINT
*************************************************************************************************/
_api_EINT_Register_t m_EINT_Register=NULL;

_api_EINT_RegisterFast_t m_EINT_RegisterFast=NULL;

_api_EINT_Init_t m_EINT_Init=NULL;

_api_EINT_Uninit_t m_EINT_Uninit=NULL;

_api_EINT_GetLevel_t m_EINT_GetLevel=NULL;

_api_EINT_Mask_t m_EINT_Mask=NULL;

_api_EINT_Unmask_t m_EINT_Unmask=NULL;


s32 Ql_EINT_Register(Enum_PinName eintPinName, Callback_EINT_Handle callback_eint,void* customParam)
{   
	if (NULL == m_EINT_Register)
	{
		s32 ptr = m_get_api_ptr("Ql_EINT_Register");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_EINT_Register");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_EINT_Register=(_api_EINT_Register_t)ptr;
    }
	return m_EINT_Register(eintPinName,callback_eint,customParam);
}


s32 Ql_EINT_RegisterFast(Enum_PinName eintPinName, Callback_EINT_Handle callback_eint, void* customParam)
{   
	if (NULL == m_EINT_RegisterFast)
	{
		s32 ptr = m_get_api_ptr("Ql_EINT_RegisterFast");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_EINT_RegisterFast");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_EINT_RegisterFast=(_api_EINT_RegisterFast_t)ptr;
    }
	return m_EINT_RegisterFast(eintPinName,callback_eint,customParam);
}

s32 Ql_EINT_Init(Enum_PinName eintPinName,Enum_EintType eintType,u32 hwDebounce,u32  swDebounce, bool automask)
{   
	if (NULL == m_EINT_Init)
	{
		s32 ptr = m_get_api_ptr("Ql_EINT_Init");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_EINT_Init");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_EINT_Init=(_api_EINT_Init_t)ptr;
    }
	return m_EINT_Init(eintPinName,eintType,hwDebounce,swDebounce,automask);
}


s32 Ql_EINT_Uninit(Enum_PinName eintPinName)
{   
	if (NULL == m_EINT_Uninit)
	{
		s32 ptr = m_get_api_ptr("Ql_EINT_Uninit");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_EINT_Uninit");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_EINT_Uninit=(_api_EINT_Uninit_t)ptr;
    }
	return m_EINT_Uninit(eintPinName);
}


s32 Ql_EINT_GetLevel(Enum_PinName eintPinName)
{   
	if (NULL == m_EINT_GetLevel)
	{
		s32 ptr = m_get_api_ptr("Ql_EINT_GetLevel");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_EINT_GetLevel");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_EINT_GetLevel=(_api_EINT_GetLevel_t)ptr;
    }
	return m_EINT_GetLevel(eintPinName);
}

void Ql_EINT_Mask(Enum_PinName eintPinName)
{   
	if (NULL == m_EINT_Mask)
	{
		s32 ptr = m_get_api_ptr("Ql_EINT_Mask");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_EINT_Mask");		
			return;
        }
		m_EINT_Mask=(_api_EINT_Mask_t)ptr;
    }
	m_EINT_Mask(eintPinName);
}
void Ql_EINT_Unmask(Enum_PinName eintPinName)
{   
	if (NULL == m_EINT_Unmask)
	{
		s32 ptr = m_get_api_ptr("Ql_EINT_Unmask");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_EINT_Unmask");		
			return ;
        }
		m_EINT_Unmask=(_api_EINT_Unmask_t)ptr;
    }
	m_EINT_Unmask(eintPinName);
}

