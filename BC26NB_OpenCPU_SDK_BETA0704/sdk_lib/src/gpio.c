#include "ql_type.h"
#include "ql_gpio.h"
#include "ql_error.h"
#include "ql_func.h"


/************************************************************************************************
*   gpio
*************************************************************************************************/
typedef s32 (*_api_GPIO_Init_t)(Enum_PinName pinName,Enum_PinDirection dir,Enum_PinLevel level ,Enum_PinPullSel pullsel);
typedef s32 (* _api_GPIO_SetLevel_t)(Enum_PinName pinName, Enum_PinLevel level);
typedef s32 (* _api_GPIO_GetLevel_t)(Enum_PinName pinName);
typedef s32 (* _api_GPIO_SetDirection_t)(Enum_PinName pinName,Enum_PinDirection dir);
typedef s32 (* _api_GPIO_GetDirection_t)(Enum_PinName pinName);
typedef s32 (* _api_GPIO_SetPullSelection_t)(Enum_PinName pinName,Enum_PinPullSel pullSel);
typedef s32 (* _api_GPIO_Uninit_t)(Enum_PinName pinName);


/************************************************************************************************
*   GPIO
*************************************************************************************************/
_api_GPIO_Init_t m_GPIO_Init=NULL;

_api_GPIO_SetLevel_t m_GPIO_SetLevel=NULL;

_api_GPIO_GetLevel_t m_GPIO_GetLevel=NULL;

_api_GPIO_SetDirection_t m_GPIO_SetDirection=NULL;

_api_GPIO_GetDirection_t m_GPIO_GetDirection=NULL;

_api_GPIO_SetPullSelection_t m_GPIO_SetPullSelection=NULL;

_api_GPIO_Uninit_t m_GPIO_Uninit=NULL;


/************************************************************************************************
*   gpio
*************************************************************************************************/
s32 Ql_GPIO_Init(Enum_PinName pinName,Enum_PinDirection dir,Enum_PinLevel  level ,Enum_PinPullSel pullsel)
{   
	if (NULL == m_GPIO_Init)
	{
		s32 ptr = m_get_api_ptr("Ql_GPIO_Init");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_GPIO_Init");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_GPIO_Init=(_api_GPIO_Init_t)ptr;
    }
	return m_GPIO_Init(pinName,dir,level,pullsel);
}

s32 Ql_GPIO_SetLevel(Enum_PinName pinName, Enum_PinLevel level)
{
    if(NULL == m_GPIO_SetLevel)
    {
        s32 ptr = m_get_api_ptr("Ql_GPIO_SetLevel");
        if(0 == ptr)
        {
			//Ql_Debug_Trace(api_notsup, "Ql_GPIO_SetLevel");		
			return Ql_RET_NOT_SUPPORT;
        }
        m_GPIO_SetLevel=(_api_GPIO_SetLevel_t)ptr;
    }
    return m_GPIO_SetLevel(pinName,level);
}

s32 Ql_GPIO_GetLevel(Enum_PinName pinName)
{
    if(NULL == m_GPIO_GetLevel)
    {
        s32 ptr = m_get_api_ptr("Ql_GPIO_GetLevel");
        if(0 == ptr)
        {
			//Ql_Debug_Trace(api_notsup, "Ql_GPIO_GetLevel");		
			return Ql_RET_NOT_SUPPORT;
        }
        m_GPIO_GetLevel=(_api_GPIO_GetLevel_t)ptr;
    }
    return m_GPIO_GetLevel(pinName);

}

s32 Ql_GPIO_SetDirection(Enum_PinName pinName,Enum_PinDirection dir)
{
    if(NULL == m_GPIO_SetDirection)
    {
        s32 ptr = m_get_api_ptr("Ql_GPIO_SetDirection");
        if(0 == ptr)
        {
			//Ql_Debug_Trace(api_notsup, "Ql_GPIO_SetDirection");		
			return Ql_RET_NOT_SUPPORT;
        }
        m_GPIO_SetDirection=(_api_GPIO_SetDirection_t)ptr;
    }
    return m_GPIO_SetDirection(pinName,dir);

}

s32 Ql_GPIO_GetDirection(Enum_PinName pinName)
{
    if(NULL == m_GPIO_GetDirection)
    {
        s32 ptr = m_get_api_ptr("Ql_GPIO_GetDirection");
        if(0 == ptr)
        {
			//Ql_Debug_Trace(api_notsup, "Ql_GPIO_GetDirection");		
			return Ql_RET_NOT_SUPPORT;
        }
        m_GPIO_GetDirection=(_api_GPIO_GetDirection_t)ptr;
    }
    return m_GPIO_GetDirection(pinName);

}

s32 Ql_GPIO_SetPullSelection(Enum_PinName pinName,Enum_PinPullSel pullSel)
{
    if(NULL == m_GPIO_SetPullSelection)
    {
        s32 ptr = m_get_api_ptr("Ql_GPIO_SetPullSelection");
        if(0 == ptr)
        {
			//Ql_Debug_Trace(api_notsup, "Ql_GPIO_SetPullSelection");		
			return Ql_RET_NOT_SUPPORT;
        }
        m_GPIO_SetPullSelection=(_api_GPIO_SetPullSelection_t)ptr;
    }
    return m_GPIO_SetPullSelection(pinName,pullSel);

}

s32 Ql_GPIO_Uninit(Enum_PinName pinName)
{
    if(NULL == m_GPIO_Uninit)
    {
        s32 ptr = m_get_api_ptr("Ql_GPIO_Uninit");
        if(0 == ptr)
        {
			//Ql_Debug_Trace(api_notsup, "Ql_GPIO_Uninit");		
			return Ql_RET_NOT_SUPPORT;
        }
        m_GPIO_Uninit=(_api_GPIO_Uninit_t)ptr;
    }
    return m_GPIO_Uninit(pinName);

}

