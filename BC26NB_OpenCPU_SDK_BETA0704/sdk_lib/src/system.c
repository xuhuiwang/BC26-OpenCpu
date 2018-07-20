#include "ql_stdlib.h"
#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_system.h"
#include "ql_error.h "

// OpenCPU SDK °æ±¾ºÅ
#define SDK_VERSION_NUMBER "OpenCPU_GS4_SDK_V1.0\0"

static _api_Sleep_t             m_Sleep             = NULL;
static _api_Reset_t             m_Reset             = NULL;

void Ql_Sleep(u32 msec)
{
    if (m_Sleep==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_Sleep");
        if (0 == ptr)
        {
            Ql_Debug_Trace(api_notsup, "Ql_Sleep");		
            return;
        }
        m_Sleep=(_api_Sleep_t)ptr;
    }
    m_Sleep(msec);
}

void Ql_Reset(u8 resettype)
{
    if (m_Reset==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_Reset");
        if (0 == ptr)
        {
            Ql_Debug_Trace(api_notsup, "Ql_Reset");		
            return;
        }
        m_Reset=(_api_Reset_t)ptr;
    }
    m_Reset(resettype);
}


s32  Ql_GetSDKVer(u8* ptrVer, u32 len)
{
    u16 sdk_ver_len;
    if (NULL == ptrVer || 0 == len)
    {
        return QL_RET_ERR_INVALID_PARAMETER;
    }
    sdk_ver_len = Ql_strlen(SDK_VERSION_NUMBER);
    len = (len >= sdk_ver_len) ? sdk_ver_len : len;
    Ql_strncpy((char*)ptrVer, SDK_VERSION_NUMBER, len);
    return len;
}
