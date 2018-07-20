#ifdef __OCPU_COMPILER_RVCT__
//#include <rt_misc.h>
#include <locale.h>
#endif
#include "ql_type.h"
#include "ql_stdlib.h"
#include "ql_common.h"
#include "ql_api_type.h"
#include "ql_ocpu_api.h"
#include "ql_gpio.h"
#include "ql_trace.h"
#include "ql_timer.h"
#include "ql_func.h"
#include "ql_system.h"
#include "ql_error.h"


#define   QL_OPENCPU_FLAG         "QUECTEL-OPENCPU-V1.0\0"

extern u32 HeapMemSize;
extern u8 OpenCPUHeapSpace[];
extern ST_TaskConfig TaskConfig[];
extern ST_SystemConfig SystemCfg;
extern ST_GPIOConfig GpioConfigTbl[];

void ql_InitVFunc(QL_OCPU_Funcs* vf);

extern void proc_main_task(void);
extern void funcRegionInit(void);
void ocpu_main(QlRunInfo *p_runinfo);


QlRunInfo runinfo = {0};
#define Ql_DebugTrace2 (runinfo.Fun_Ql_DebugTrace)
s32 (*Ql_Debug_Trace)(char* fmt, ...);
s32 (*Ql_sprintf)(char *, const char *, ...);
s32 (*Ql_snprintf)(char *, u32, const char *, ...);
s32 (*Ql_sscanf)(const char*, const char*, ...);




#ifdef __OCPU_COMPILER_GCC__
extern void _OCPU_InitUserMem(void);
__attribute__((section(".initdata")))
#elif defined(__OCPU_COMPILER_RVCT__)
extern void funcRegionInit(void);
#pragma arm section rodata="GABADISP"
#else
#error "Please define a compiler"
#endif
const ST_OpenCPUEntry GADLENTRY = 
{
        ocpu_main,
#ifdef __OCPU_COMPILER_GCC__
        _OCPU_InitUserMem,
#else
		funcRegionInit,
#endif
        //(u32*)&HeapMemSize,
        //(u8 *)OpenCPUHeapSpace,
        (u32*)NULL,
        (u8 *)NULL,
        (u32*)&(TaskConfig[0].TaskStackSize),
        (u32*)&(TaskConfig[0].rev1),
        (u32*)&(TaskConfig[0].rev2),
        (void*)&SystemCfg,
        (void*)GpioConfigTbl,
        (ST_TaskConfig *)&(TaskConfig[1]),
        (u8*)QL_OPENCPU_FLAG
};
#ifdef __OCPU_COMPILER_RVCT__
#pragma arm section rodata
#endif

void ocpu_main(QlRunInfo *p_runinfo)
{
    char sdk_ver[30] = {'\0'};

#ifdef __OCPU_COMPILER_RVCT__
    /* Error: L6915E: Library reports error: scatter-load file declares no heap or stack regions 
     *        and __user_initial_stackheap is not defined
     */
    //__rt_lib_init(0, 0);
    setlocale(LC_ALL, "C");
#endif
    runinfo.Fun_Ql_DebugTrace= p_runinfo->Fun_Ql_DebugTrace;
    if (p_runinfo->vFunc != NULL)
    {
        //Ql_DebugTrace2("p_runinfo->vFunc: %x\r\n", p_runinfo->vFunc);
        ql_InitVFunc((QL_OCPU_Funcs*)p_runinfo->vFunc);
    }
    else
    {
        Ql_DebugTrace2("p_runinfo->vFunc = NULL\r\n");
    }

    // ∆Ù∂Ø¥ÚSDK∞Ê±æ∫≈
    Ql_memset((u8 *)sdk_ver,0x00,sizeof(sdk_ver));
    Ql_GetSDKVer((u8 *)sdk_ver, sizeof(sdk_ver));
    Ql_DebugTrace2("%s\r\n", sdk_ver);
    
   // Ql_DebugTrace2("%x\r\n", TaskConfig[0].proc_taskEntry);


    TaskConfig[0].proc_taskEntry(TaskConfig[0].TaskId); /* customer's entrance*/
}

static u32 init_size;
#define INIT_VFUNC(vp, func, cnt) ((cnt<init_size)?(vp=func):(vp=NULL))

void ql_InitVFunc(QL_OCPU_Funcs* vf)
{
    u32 vfunc_tbl_size;
    u32 i = 1;

    //Ql_DebugTrace2("DynamicAPI: Enter ql_InitVFunc().\r\n");
    vfunc_tbl_size = sizeof(QL_OCPU_Funcs) / sizeof(void*);
    init_size = (vfunc_tbl_size < *((u32*)vf->m_size)) ? vfunc_tbl_size : *(vf->m_size);
    m_create_api_hash=(_create_api_hash_t)vf->m_create_api_hash;	
    m_get_api_ptr=(_get_api_ptr_t)vf->m_get_api_ptr;	
    INIT_VFUNC(Ql_Debug_Trace, vf->m_api_DebugTrace, i++);	
    INIT_VFUNC(Ql_sprintf  , vf->m_api_sprintf, i++);
    INIT_VFUNC(Ql_snprintf , vf->m_api_snprintf, i++);
    INIT_VFUNC(Ql_sscanf   , vf->m_api_sscanf, i++);
    //Ql_DebugTrace2("vfunc_tbl_size=%d, vf->m_size=%d, init_size=%d\r\n", vfunc_tbl_size, *(vf->m_size), init_size);
   //Ql_DebugTrace2("Ql_DebugTrace hash %x\r\n",m_get_api_ptr);

    //Ql_DebugTrace2("DynamicAPI: ql_InitVFunc() end.\r\n");
}
