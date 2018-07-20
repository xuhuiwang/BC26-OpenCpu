#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_time.h"
#include "ql_error.h "

static _api_SetLocalTime_t          m_SetLocalTime        = NULL;
static _api_GetLocalTime_t          m_GetLocalTime        = NULL;
static _api_MKTime_t                m_Mktime              = NULL;
static _api_MKTime2CalendarTime_t   m_MKTime2CalendarTime = NULL;

s32 Ql_SetLocalTime(ST_Time* dateTime)
{
    if (NULL == m_SetLocalTime)
    {
        s32 ptr = m_get_api_ptr("Ql_SetLocalTime");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_SetLocalTime");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_SetLocalTime = (_api_SetLocalTime_t)ptr;
    }
    return m_SetLocalTime(dateTime);
}

ST_Time* Ql_GetLocalTime(ST_Time* dateTime)
{
    if (NULL == m_GetLocalTime)
    {
        s32 ptr = m_get_api_ptr("Ql_GetLocalTime");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_GetLocalTime");		
            return NULL;
        }
        m_GetLocalTime = (_api_GetLocalTime_t)ptr;
    }
    return m_GetLocalTime(dateTime);
}

u64 Ql_Mktime(ST_Time* dateTime)
{
    if (NULL == m_Mktime)
    {
        s32 ptr = m_get_api_ptr("Ql_Mktime");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_Mktime");		
            return 0;
        }
        m_Mktime = (_api_MKTime_t)ptr;
    }
    return m_Mktime(dateTime);
}

ST_Time* Ql_MKTime2CalendarTime(u64 seconds, ST_Time* pOutDateTime)
{
    if (NULL == m_MKTime2CalendarTime)
    {
        s32 ptr = m_get_api_ptr("Ql_MKTime2CalendarTime");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_MKTime2CalendarTime");		
            return NULL;
        }
        m_MKTime2CalendarTime = (_api_MKTime2CalendarTime_t)ptr;
    }
    return m_MKTime2CalendarTime(seconds, pOutDateTime);
}
