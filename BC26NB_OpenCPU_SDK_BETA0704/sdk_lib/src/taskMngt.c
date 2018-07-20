#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_system.h"
#include "ql_error.h "

static _api_OS_GetMessage_t                 m_OS_GetMessage                 = NULL;
static _api_OS_SendMessage_t                m_OS_SendMessage                = NULL;
static _api_OS_SendMessage_t                m_OS_SendMessageFromISR         = NULL;
static _api_OS_CreateMutex_t                m_OS_CreateMutex                = NULL;
static _api_OS_TakeMutex_t                  m_OS_TakeMutex                  = NULL;
static _api_OS_GiveMutex_t                  m_OS_GiveMutex                  = NULL;
static _api_OS_CreateSemaphore_t            m_OS_CreateSemaphore            = NULL;
static _api_OS_TakeSemaphore_t              m_OS_TakeSemaphore              = NULL;
static _api_OS_GiveSemaphore_t              m_OS_GiveSemaphore              = NULL;
static _api_SetLastErrorCode_t              m_SetLastErrorCode              = NULL;
static _api_GetLastErrorCode_t              m_GetLastErrorCode              = NULL;
static _api_OS_GetCurrentTaskPriority_t     m_OS_GetCurrentTaskPriority     = NULL;
static _api_OS_GetCurrenTaskLeftStackSize_t m_OS_GetCurrenTaskLeftStackSize = NULL;
static _api_OS_CreateEvent_t                m_OS_CreateEvent                = NULL;
static _api_OS_WaitEvent_t                  m_OS_WaitEvent                  = NULL;
static _api_OS_SetEvent_t                   m_OS_SetEvent                   = NULL;
static _api_OS_GetActiveTaskId_t            m_OS_GetActiveTaskId            = NULL;
static _api_RIL_request_custom_command_t    m_RIL_request_custom_command    = NULL;


s32 Ql_OS_GetMessage(ST_MSG* msg)
{
    if (m_OS_GetMessage==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_GetMessage");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_GetMessage");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_OS_GetMessage = (_api_OS_GetMessage_t)ptr;
    }
    return m_OS_GetMessage(msg);
}

s32  Ql_OS_SendMessage(s32 destTaskId, u32 msgId, u32 param1, u32 param2)
{
    if (m_OS_SendMessage==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_SendMessage");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_SendMessage");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_OS_SendMessage=(_api_OS_SendMessage_t)ptr;
    }
    return m_OS_SendMessage(destTaskId, msgId, param1, param2);
}

s32  Ql_OS_SendMessageFromISR(s32 destTaskId, u32 msgId, u32 param1, u32 param2)
{
    if (m_OS_SendMessageFromISR==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_SendMessageFromISR");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_SendMessage");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_OS_SendMessageFromISR=(_api_OS_SendMessage_t)ptr;
    }
    return m_OS_SendMessageFromISR(destTaskId, msgId, param1, param2);
}




u32  Ql_OS_CreateMutex(void)
{
    if (m_OS_CreateMutex==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_CreateMutex");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_CreateMutex");		
            return 0;
        }
        m_OS_CreateMutex=(_api_OS_CreateMutex_t)ptr;
    }
    return m_OS_CreateMutex();
}

void Ql_OS_TakeMutex(u32 mutexId,u32 block_time)
{
    if (m_OS_TakeMutex==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_TakeMutex");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_TakeMutex");		
            return;
        }
        m_OS_TakeMutex=(_api_OS_TakeMutex_t)ptr;
    }
    m_OS_TakeMutex(mutexId,block_time);
}

void Ql_OS_GiveMutex(u32 mutexId)
{
    if (m_OS_GiveMutex==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_GiveMutex");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_GiveMutex");		
            return ;
        }
        m_OS_GiveMutex=(_api_OS_GiveMutex_t)ptr;
    }
    m_OS_GiveMutex(mutexId);	
}

u32 Ql_OS_CreateSemaphore(u32 maxCount,u32 InitialCount)
{
    if (m_OS_CreateSemaphore==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_CreateSemaphore");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_CreateSemaphore");		
            return 0;
        }
        m_OS_CreateSemaphore=(_api_OS_CreateSemaphore_t)ptr;
    }
    return m_OS_CreateSemaphore(maxCount, InitialCount);
}

u32  Ql_OS_TakeSemaphore(u32 semId, u32 block_time)
{
    if (m_OS_TakeSemaphore==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_TakeSemaphore");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_TakeSemaphore");		
            return 0;
        }
        m_OS_TakeSemaphore=(_api_OS_TakeSemaphore_t)ptr;
    }
    return m_OS_TakeSemaphore(semId, block_time);
}

void Ql_OS_GiveSemaphore(u32 semId)
{

    if (m_OS_GiveSemaphore==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_GiveSemaphore");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_GiveSemaphore");		
            return;
        }
        m_OS_GiveSemaphore=(_api_OS_GiveSemaphore_t)ptr;
    }
    m_OS_GiveSemaphore(semId);
}

u32 Ql_OS_CreateEvent(void)
{
    if (NULL == m_OS_CreateEvent)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_CreateEvent");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_CreateEvent");		
            return 0;
        }
        m_OS_CreateEvent=(_api_OS_CreateEvent_t)ptr;
    }
    return m_OS_CreateEvent();
}

s32 Ql_OS_WaitEvent(u32 evtId, u32 evtFlag,u32 block_time)
{
    if (NULL == m_OS_WaitEvent)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_WaitEvent");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_WaitEvent");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_OS_WaitEvent = (_api_OS_WaitEvent_t)ptr;
    }
    return m_OS_WaitEvent(evtId, evtFlag,block_time);
}

s32 Ql_OS_SetEvent(u32 evtId, u32 evtFlag)
{
    if (NULL == m_OS_SetEvent)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_SetEvent");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_SetEvent");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_OS_SetEvent = (_api_OS_SetEvent_t)ptr;
    }
    return m_OS_SetEvent(evtId, evtFlag);
}

s32 Ql_OS_GetActiveTaskId(void)
{
    if (NULL == m_OS_GetActiveTaskId)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_GetActiveTaskId");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_GetActiveTaskId");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_OS_GetActiveTaskId = (_api_OS_GetActiveTaskId_t)ptr;
    }
    return m_OS_GetActiveTaskId();
}

s32 Ql_SetLastErrorCode(s32 errCode)
{
    if (m_SetLastErrorCode==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_SetLastErrorCode");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_SetLastErrorCode");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_SetLastErrorCode=(_api_SetLastErrorCode_t)ptr;
    }
    return m_SetLastErrorCode(errCode);
}

s32 Ql_GetLastErrorCode(void)
{
    if (m_GetLastErrorCode==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_GetLastErrorCode");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_GetLastErrorCode");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_GetLastErrorCode=(_api_GetLastErrorCode_t)ptr;
    }
    return m_GetLastErrorCode();
}

u32  Ql_OS_GetCurrentTaskPriority(void)
{
    if (m_OS_GetCurrentTaskPriority==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_GetCurrentTaskPriority");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_GetCurrentTaskPriority");		
            return 0;
        }
        m_OS_GetCurrentTaskPriority=(_api_OS_GetCurrentTaskPriority_t)ptr;
    }
    return m_OS_GetCurrentTaskPriority();
}

u32  Ql_OS_GetCurrenTaskLeftStackSize(void)
{
    if (m_OS_GetCurrenTaskLeftStackSize==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_OS_GetCurrenTaskLeftStackSize");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_GetCurrenTaskLeftStackSize");		
            return 0;
        }
        m_OS_GetCurrenTaskLeftStackSize=(_api_OS_GetCurrenTaskLeftStackSize_t)ptr;
    }
    return m_OS_GetCurrenTaskLeftStackSize();
}

int Ql_ril_request_command(char *cmd_string, Callback_ATResponse callback, void *user_data)
{
    if (m_RIL_request_custom_command==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_ril_request_command");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_OS_GetCurrenTaskLeftStackSize");		
            return 0;
        }
        m_RIL_request_custom_command=(_api_RIL_request_custom_command_t)ptr;
    }
    return m_RIL_request_custom_command(cmd_string, callback, user_data);
}


