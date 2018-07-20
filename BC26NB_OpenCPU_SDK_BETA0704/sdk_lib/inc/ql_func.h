#ifndef __QL_FUNC_H__
#define __QL_FUNC_H__
#include "ql_common.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_ocpu_api.h"

typedef struct          
{
    s32 (*Fun_Ql_DebugTrace) (char *fmt, ... );
    void* vFunc;
}QlRunInfo;


/* Entry Point */
typedef struct 
{
    void    (*qlmain)(QlRunInfo *p_runinfo);
    void    (*qlreginit)(void);
    u32     *AppHeapMemSize;
    u8      *OpenCPUHeapMem;
    u32     *MainTaskStackSize;
    u32     *MainTaskPriority;
    u32     *MainTaskExtqsize;
    void    *customerUserQlconfig;
    void    *customerGpioConfigTable;    /* pointer to QlPinConfigTable_t */
    ST_TaskConfig *QlMutitaskArray;
    u8      *OpenCPUFlag;
}ST_OpenCPUEntry;

extern _create_api_hash_t m_create_api_hash;
extern _get_api_ptr_t m_get_api_ptr;
extern char api_notsup[];

#endif  // End-of '__QL_FUNC_H__'

