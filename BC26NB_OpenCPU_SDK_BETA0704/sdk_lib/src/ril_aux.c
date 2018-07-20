#include "custom_feature_def.h"
#include "ril.h"
#include "ril_def.h"
#include "ril_util.h"
#include "ql_error.h"
#include "ql_trace.h"
#include "ql_stdlib.h"
#include "ql_memory.h"
#include "ql_system.h"
#include "ql_uart.h"

#ifdef __OCPU_RIL_SUPPORT__

#if RIL_DEBUG_ENABLE > 0
static char DBG_Buffer[512];
#endif


//////////////////////////////////////////////////////////////////////////
//////////////////////////// RIL Aux Task ////////////////////////////////
//////////////////////////////////////////////////////////////////////////
extern void OnURCHandler(const char* strURC, void* reserved);
u32 g_ril_auxTaskId  = TaskId_End;
bool g_ril_auxTaskReady = FALSE;
void proc_reserved2(s32 TaskId)
{
    ST_MSG msg;

    RIL_DEBUG(DBG_Buffer,"<--[ril_aux_task]:OpenCPU: RIL Aux Task Starts.-->\r\n");
    g_ril_auxTaskId = TaskId;
    g_ril_auxTaskReady = TRUE;
    while(TRUE)
    {
        Ql_OS_GetMessage(&msg);
        switch(msg.message)
        {
        case MSG_ID_URC_IND:
            {
                OnURCHandler((char*)msg.param2, NULL);
                FREE_MEM(msg.param2);
                break;
            }
        default:
            break;
        }
    }
}

#endif  //__OCPU_RIL_SUPPORT__
