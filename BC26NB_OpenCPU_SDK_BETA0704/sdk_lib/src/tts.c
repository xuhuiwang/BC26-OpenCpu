#include "ql_option.def"
#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_tts.h"

#ifdef __OCPU_TTS_ENABLE__

static _api_TTS_Initialize_t         m_TTS_Initialize = NULL;
static _api_TTS_Play_t               m_TTS_Play = NULL;
static _api_TTS_PlayInCall_t         m_TTS_PlayInCall = NULL;
static _api_TTS_Stop_t               m_TTS_Stop = NULL;
static _api_TTS_Query_t              m_TTS_Query = NULL;



s32 Ql_TTS_Initialize(Callback_TTS_Play callback_play)
{
	if (m_TTS_Initialize==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_TTS_Initialize");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_TTS_Initialize");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_TTS_Initialize=(_api_TTS_Initialize_t)ptr;
    }
	return m_TTS_Initialize(callback_play);
}

s32 Ql_TTS_Play(u8* content, u8 len)
{
	if (m_TTS_Play==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_TTS_Play");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_TTS_Play");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_TTS_Play=(_api_TTS_Play_t)ptr;
    }
	return m_TTS_Play(content, len);
}

s32 Ql_TTS_PlayInCall(u8* content, u8 len,  u8 upLinkVolume, u8 downLinkVolume)
{
	if (NULL == m_TTS_PlayInCall)
	{
		s32 ptr = m_get_api_ptr("Ql_TTS_PlayInCall");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_TTS_PlayInCall");		
			return Ql_RET_NOT_SUPPORT;
		}
		m_TTS_PlayInCall = (_api_TTS_PlayInCall_t)ptr;
	}
    return m_TTS_PlayInCall(content, len, upLinkVolume, downLinkVolume);
}

s32 Ql_TTS_Stop(void)
{
	if (m_TTS_Stop==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_TTS_Stop");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_TTS_Stop");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_TTS_Stop=(_api_TTS_Stop_t)ptr;
    }
	return m_TTS_Stop();
}

s32 Ql_TTS_Query(void)
{
	if (m_TTS_Query==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_TTS_Query");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_TTS_Query");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_TTS_Query=(_api_TTS_Query_t)ptr;
    }
	return m_TTS_Query();
}
#endif  // __OCPU_TTS_ENABLE__
