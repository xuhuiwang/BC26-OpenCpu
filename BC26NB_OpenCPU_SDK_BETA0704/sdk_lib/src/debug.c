/*
#include <stdarg.h>
#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_trace.h"
#include "ql_error.h "
#include "ql_uart.h"

static _api_Debug_Trace_t    m_Debug_Trace   = NULL;

s32  Ql_Debug_Trace(char* fmt, ...)
{
    s32 ret;
    va_list arg;

    if (NULL == m_Debug_Trace)
    {
        s32 ptr = m_get_api_ptr("Ql_Debug_Trace");
        if (0 == ptr)
        {
            Ql_Debug_Trace(api_notsup, "Ql_Debug_Trace");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_Debug_Trace = (_api_Debug_Trace_t)ptr;
    }
    va_start(arg, fmt);
    ret = m_Debug_Trace(fmt, arg);
    va_end(arg);
    return ret;
}
*/