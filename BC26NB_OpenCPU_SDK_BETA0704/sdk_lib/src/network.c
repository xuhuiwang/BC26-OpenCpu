#if 0
#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_network.h"
#include "ql_error.h "

static _api_NW_GetCardState_t           m_NW_GetCardState       = NULL;
static _api_NW_GetNetworkState_t        m_NW_GetNetworkState    = NULL;
static _api_NW_GetSignalQuality_t       m_NW_GetSignalQuality   = NULL;

s32  Ql_NW_GetSIMCardState(void)
{
    if (NULL == m_NW_GetCardState)
    {
        s32 ptr = m_get_api_ptr("Ql_NW_GetSIMCardState");
        if (0 == ptr)
        {
            Ql_Debug_Trace(api_notsup, "Ql_NW_GetSIMCardState");
            return Ql_RET_NOT_SUPPORT;
        }
        m_NW_GetCardState = (_api_NW_GetCardState_t)ptr;
    }
    return m_NW_GetCardState();
}

s32  Ql_NW_GetNetworkState(s32* creg, s32* cgreg)
{
    if (NULL == m_NW_GetNetworkState)
    {
        s32 ptr = m_get_api_ptr("Ql_NW_GetNetworkState");
        if (0 == ptr)
        {
            Ql_Debug_Trace(api_notsup, "Ql_NW_GetNetworkState");
            return Ql_RET_NOT_SUPPORT;
        }
        m_NW_GetNetworkState = (_api_NW_GetNetworkState_t)ptr;
    }
    return m_NW_GetNetworkState(creg, cgreg);
}

s32  Ql_NW_GetSignalQuality(u32* rssi, u32* ber)
{
    if (NULL == m_NW_GetSignalQuality)
    {
        s32 ptr = m_get_api_ptr("Ql_NW_GetSignalQuality");
        if (0 == ptr)
        {
            Ql_Debug_Trace(api_notsup, "Ql_NW_GetSignalQuality");
            return Ql_RET_NOT_SUPPORT;
        }
        m_NW_GetSignalQuality = (_api_NW_GetSignalQuality_t)ptr;
    }
    return m_NW_GetSignalQuality(rssi, ber);
}
#endif
