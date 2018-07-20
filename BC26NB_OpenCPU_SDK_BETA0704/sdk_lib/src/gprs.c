#if 0
#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_gprs.h"
#include "ql_error.h "

static _api_GPRS_GetPDPCntxtState_t m_GPRS_GetPDPCntxtState=NULL;
static _api_GPRS_Config_t m_GPRS_Config=NULL;
static _api_GPRS_Register_t m_GPRS_Register=NULL;
static _api_GPRS_Activate_t m_GPRS_Activate=NULL;
static _api_GPRS_ActivateEx_t m_GPRS_ActivateEx=NULL;
static _api_GPRS_Deactivate_t m_GPRS_Deactivate=NULL;
static _api_GPRS_DeactivateEx_t m_GPRS_DeactivateEx=NULL;
static _api_GPRS_GetLocalIPAddress_t m_GPRS_GetLocalIPAddress=NULL;
static _api_GPRS_GetDNSAddress_t m_GPRS_GetDNSAddress=NULL;
static _api_GPRS_SetDNSAddress_t m_GPRS_SetDNSAddress=NULL;

s32 Ql_GPRS_GetPDPCntxtState(u8 contextId)
{
    if (NULL == m_GPRS_GetPDPCntxtState)
    {
        s32 ptr = m_get_api_ptr("Ql_GPRS_GetPDPCntxtState");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_GPRS_GetPDPCntxtState");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_GPRS_GetPDPCntxtState = (_api_GPRS_GetPDPCntxtState_t)ptr;
    }
    return m_GPRS_GetPDPCntxtState(contextId);
}

s32 Ql_GPRS_GetPDPContextId(void)
{
    if (Ql_GPRS_GetPDPCntxtState(0) == 0 || Ql_GPRS_GetPDPCntxtState(0) == 1)
    {
        return 0;
    }
    else if (Ql_GPRS_GetPDPCntxtState(1) == 0 || Ql_GPRS_GetPDPCntxtState(1) == 1)
    {
        return 1;
    }else{
        return -1;
    }
}

s32 Ql_GPRS_Config(u8 contextId, ST_GprsConfig* cfg)
{
    if (m_GPRS_Config==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_GPRS_Config");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_GPRS_Config");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_GPRS_Config=(_api_GPRS_Config_t)ptr;
    }
    return m_GPRS_Config(contextId, cfg);
}


s32 Ql_GPRS_Register(u8 contextId,  ST_PDPContxt_Callback * callback_func, void* customParam)
{
    if (m_GPRS_Register==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_GPRS_Register");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_GPRS_Register");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_GPRS_Register=(_api_GPRS_Register_t)ptr;
    }
    return m_GPRS_Register(contextId,   callback_func, customParam);
}



s32 Ql_GPRS_Activate (u8 contextId)
{
    if (m_GPRS_Activate==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_GPRS_Activate");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_GPRS_Activate");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_GPRS_Activate=(_api_GPRS_Activate_t)ptr;
    }
    return m_GPRS_Activate(contextId);
}

s32 Ql_GPRS_ActivateEx(u8 contextId, bool isBlocking)
{
    if (m_GPRS_ActivateEx==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_GPRS_ActivateEx");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_GPRS_ActivateEx");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_GPRS_ActivateEx=(_api_GPRS_ActivateEx_t)ptr;
    }
    return m_GPRS_ActivateEx(contextId, isBlocking);
}

s32 Ql_GPRS_Deactivate(u8 contextId)
{
    if (m_GPRS_Deactivate==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_GPRS_Deactivate");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_GPRS_Deactivate");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_GPRS_Deactivate=(_api_GPRS_Deactivate_t)ptr;
    }
    return m_GPRS_Deactivate(contextId);
}

s32 Ql_GPRS_DeactivateEx(u8 contextId, bool isBlocking)
{
    if (m_GPRS_DeactivateEx==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_GPRS_DeactivateEx");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_GPRS_DeactivateEx");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_GPRS_DeactivateEx = (_api_GPRS_DeactivateEx_t)ptr;
    }
    return m_GPRS_DeactivateEx(contextId, isBlocking);
}

s32 Ql_GPRS_GetLocalIPAddress(u8 contxtId, u32* ipAddr)
{
    if (m_GPRS_GetLocalIPAddress==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_GPRS_GetLocalIPAddress");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_GPRS_GetLocalIPAddress");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_GPRS_GetLocalIPAddress=(_api_GPRS_GetLocalIPAddress_t)ptr;
    }
    return m_GPRS_GetLocalIPAddress(contxtId, ipAddr);	
}

s32 Ql_GPRS_GetDNSAddress(u8 contextId, u32* firstAddr, u32* seconAddr)
{
    if (m_GPRS_GetDNSAddress==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_GPRS_GetDNSAddress");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_GPRS_GetDNSAddress");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_GPRS_GetDNSAddress=(_api_GPRS_GetDNSAddress_t)ptr;
    }
    return m_GPRS_GetDNSAddress(contextId,  firstAddr, seconAddr);
}    

s32 Ql_GPRS_SetDNSAddress(u8 contextId, u32 firstAddr, u32 seconAddr)
{
    if (m_GPRS_SetDNSAddress==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_GPRS_SetDNSAddress");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_GPRS_SetDNSAddress");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_GPRS_SetDNSAddress=(_api_GPRS_SetDNSAddress_t)ptr;
    }
    return m_GPRS_SetDNSAddress(contextId, firstAddr, seconAddr);
}
#endif