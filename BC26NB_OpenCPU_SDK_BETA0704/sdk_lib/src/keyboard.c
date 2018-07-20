#include "ql_option.def"
#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_keyboard.h"
#include "ql_error.h "

#ifdef __OCPU_KEYBOARD_ENABLE__

static _api_KB_Register_t   m_KB_Register   = NULL;
static _api_KB_Config_t     m_KB_Config     = NULL;
static _api_KB_Init_t       m_KB_Init       = NULL;
static _api_KB_Uninit_t     m_KB_Uninit     = NULL;

s32 Ql_KB_Register(Callback_Keyboard  callback_kb)
{
    if (NULL == m_KB_Register)
    {
        s32 ptr = m_get_api_ptr("Ql_KB_Register");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_KB_Register");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_KB_Register = (_api_KB_Register_t)ptr;
    }
    return m_KB_Register(callback_kb);
}

s32 Ql_KB_Config(s32 debounce, s32 reserved)
{
    if (NULL == m_KB_Config)
    {
        s32 ptr = m_get_api_ptr("Ql_KB_Config");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_KB_Config");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_KB_Config = (_api_KB_Config_t)ptr;
    }
    return m_KB_Config(debounce, reserved);
}

s32 Ql_KB_Init(Enum_PinName kbPinName[], u32 count)
{
    if (NULL == m_KB_Init)
    {
        s32 ptr = m_get_api_ptr("Ql_KB_Init");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_KB_Init");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_KB_Init = (_api_KB_Init_t)ptr;
    }
    return m_KB_Init(kbPinName, count);
}

s32 Ql_KB_Uninit(Enum_PinName kbPinName[], u32 count)
{
    if (NULL == m_KB_Uninit)
    {
        s32 ptr = m_get_api_ptr("Ql_KB_Uninit");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_KB_Uninit");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_KB_Uninit = (_api_KB_Uninit_t)ptr;
    }
    return m_KB_Uninit(kbPinName, count);
}

#endif // __OCPU_KEYBOARD_ENABLE__
