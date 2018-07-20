#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_timer.h"
#include "ql_error.h "

static _api_Timer_Register_t        m_Timer_Register        = NULL;
static _api_Timer_RegisterFast_t    m_Timer_RegisterFast    = NULL;
static _api_Timer_Start_t           m_Timer_Start           = NULL;
static _api_Timer_Stop_t            m_Timer_Stop            = NULL;

s32 Ql_Timer_Register(u32 timerId, Callback_Timer_OnTimer callback_onTimer, void* param)
{
    if (NULL == m_Timer_Register)
    {
        s32 ptr = m_get_api_ptr("Ql_Timer_Register");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_Timer_Register");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_Timer_Register = (_api_Timer_Register_t)ptr;
    }
    return m_Timer_Register(timerId, callback_onTimer, param);
}

s32 Ql_Timer_RegisterFast(u32 timerId, Callback_Timer_OnTimer callback_onTimer, void* param)
{
    if (NULL == m_Timer_RegisterFast)
    {
        s32 ptr = m_get_api_ptr("Ql_Timer_RegisterFast");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_Timer_RegisterFast");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_Timer_RegisterFast = (_api_Timer_RegisterFast_t)ptr;
    }
    return m_Timer_RegisterFast(timerId, callback_onTimer, param);
}

s32 Ql_Timer_Start(u32 timerId, u32 interval, bool autoRepeat)
{
    if (NULL == m_Timer_Start)
    {
        s32 ptr = m_get_api_ptr("Ql_Timer_Start");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_Timer_Start");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_Timer_Start = (_api_Timer_Start_t)ptr;
    }
    return m_Timer_Start(timerId, interval, autoRepeat);
}

s32 Ql_Timer_Stop(u32 timerId)
{
    if (NULL == m_Timer_Stop)
    {
        s32 ptr = m_get_api_ptr("Ql_Timer_Stop");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_Timer_Stop");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_Timer_Stop=(_api_Timer_Stop_t)ptr;
    }
    return m_Timer_Stop(timerId);
}
