
#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_power.h"
#include "ql_error.h "


static _api_SleepEnable_t          	m_SleepEnable      =   NULL;
static _api_SleepDisable_t  		m_SleepDisable = NULL;   


s32 Ql_SleepEnable(void)
{
    s32 ret;
    if (NULL == m_SleepEnable)
    {
        s32 ptr = m_get_api_ptr("Ql_SleepEnable");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_SleepEnable");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_SleepEnable=(_api_SleepEnable_t)ptr;
    }
    ret = m_SleepEnable();

    return ret;
}

s32 Ql_SleepDisable(void)
{
    s32 ret;
    if (NULL == m_SleepDisable)
    {
        s32 ptr = m_get_api_ptr("Ql_SleepDisable");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_SleepDisable");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_SleepDisable=(_api_SleepDisable_t)ptr;
    }
    ret = m_SleepDisable();

    return ret;
}



