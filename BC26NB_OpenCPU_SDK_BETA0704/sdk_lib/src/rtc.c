#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_rtc.h"
#include "ql_error.h "

static _api_Rtc_RegisterFast_t    m_Rtc_RegisterFast    = NULL;
static _api_Rtc_Start_t           m_Rtc_Start           = NULL;
static _api_Rtc_Stop_t            m_Rtc_Stop            = NULL;

s32 Ql_Rtc_RegisterFast(u32 rtcId, Callback_Rtc_Func callback_onRtc, void* param)
{
    if (NULL == m_Rtc_RegisterFast)
    {
        s32 ptr = m_get_api_ptr("Ql_Rtc_RegisterFast");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_Rtc_RegisterFast");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_Rtc_RegisterFast = (_api_Rtc_RegisterFast_t)ptr;
    }
    return m_Rtc_RegisterFast(rtcId, callback_onRtc, param);
}

s32 Ql_Rtc_Start(u32 rtcId, u32 interval, bool autoRepeat)
{
    if (NULL == m_Rtc_Start)
    {
        s32 ptr = m_get_api_ptr("Ql_Rtc_Start");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_Rtc_Start");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_Rtc_Start = (_api_Rtc_Start_t)ptr;
    }
    return m_Rtc_Start(rtcId, interval, autoRepeat);
}

s32 Ql_Rtc_Stop(u32 rtcId)
{
    if (NULL == m_Rtc_Stop)
    {
        s32 ptr = m_get_api_ptr("Ql_Rtc_Stop");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_Rtc_Stop");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_Rtc_Stop=(_api_Rtc_Stop_t)ptr;
    }
    return m_Rtc_Stop(rtcId);
}


