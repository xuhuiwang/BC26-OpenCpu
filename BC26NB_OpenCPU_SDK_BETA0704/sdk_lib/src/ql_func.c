#include "ql_type.h"
#include "ql_common.h"
//#include "ql_api_type.h"
#include "ql_ocpu_api.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_system.h"
#include "ql_error.h"
#include "ql_func.h"


/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
<       Function Pointer Definition for All OpenCPU APIs
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
_create_api_hash_t m_create_api_hash=NULL;
_get_api_ptr_t m_get_api_ptr=NULL;

/************************************************************************************************
*   Standard Libraries
*************************************************************************************************/


/************************************************************************************************
*   Audio
*************************************************************************************************/
_api_StartPlayAudioStream_t m_StartPlayAudioStream = NULL;
_api_StopPlayAudioStream_t  m_StopPlayAudioStream  = NULL;

/************************************************************************************************
*   Timer
*************************************************************************************************/
_api_GetRelativeTime_Counter_t m_GetRelativeTime_Counter=NULL;
_api_SecondToTicks_t m_SecondToTicks=NULL;
_api_MillisecondToTicks_t m_MillisecondToTicks=NULL;
_api_CalendarTime2LocalTime_t m_CalendarTime2LocalTime = NULL;

/************************************************************************************************
*   Multitasking
*************************************************************************************************/
_api_osChangeTaskPriority_t m_osChangeTaskPriority=NULL;

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>  End-of Definition for All OpenCPU APIs
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

extern _get_api_ptr_t m_get_api_ptr;

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
<       Definition for All OpenCPU APIs
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
char api_notsup[] = "%s() is not supported\r\n";


s32 Ql_StartPlayAudioStream(u8 * stream, u32 streamsize, s32 streamformat, bool repeat, u8 volume, u8 audiopath)
{
	if (m_StartPlayAudioStream==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_StartPlayAudioStream");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_StartPlayAudioStream");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_StartPlayAudioStream=(_api_StartPlayAudioStream_t)ptr;
    }
	return m_StartPlayAudioStream(stream,  streamsize,  streamformat,  repeat,  volume,  audiopath);
}

s32 Ql_StopPlayAudioStream(void)
{
	if (m_StopPlayAudioStream==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_StopPlayAudioStream");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_StopPlayAudioStream");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_StopPlayAudioStream=(_api_StopPlayAudioStream_t)ptr;
    }
	return m_StopPlayAudioStream();
}


/************************************************************************************************
*   Timer
*************************************************************************************************/

u32   Ql_GetRelativeTime_Counter(void)
{
	if (m_GetRelativeTime_Counter==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_GetRelativeTime_Counter");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_GetRelativeTime_Counter");		
			return 0;
        }
		m_GetRelativeTime_Counter=(_api_GetRelativeTime_Counter_t)ptr;
    }
	return m_GetRelativeTime_Counter();
}

ticks Ql_SecondToTicks(u32 seconds)
{
	if (m_SecondToTicks==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_SecondToTicks");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SecondToTicks");		
			return 0;
        }
		m_SecondToTicks=(_api_SecondToTicks_t)ptr;
    }
	  return m_SecondToTicks(seconds);
}

ticks Ql_MillisecondToTicks(u32 milliseconds)
{
	if (m_MillisecondToTicks==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_MillisecondToTicks");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_MillisecondToTicks");		
			return 0;
        }
		m_MillisecondToTicks=(_api_MillisecondToTicks_t)ptr;
    }
	return m_MillisecondToTicks(milliseconds);
}


bool Ql_CalendarTime2LocalTime(u32 seconds, ST_Time *pSysTime, u32 baseyear)
{
	if (m_CalendarTime2LocalTime==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_CalendarTime2LocalTime");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_CalendarTime2LocalTime");		
			return 0;
        }
		m_CalendarTime2LocalTime=(_api_CalendarTime2LocalTime_t)ptr;
    }
	return m_CalendarTime2LocalTime(seconds, pSysTime, baseyear);
}

/************************************************************************************************
*   Multitasking
*************************************************************************************************/

u32  Ql_osChangeTaskPriority(s32 desttaskid, u32 newpriority)
{
	if (m_osChangeTaskPriority==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_osChangeTaskPriority");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_osChangeTaskPriority");		
			return 0;
        }
		m_osChangeTaskPriority=(_api_osChangeTaskPriority_t)ptr;
    }
	 return m_osChangeTaskPriority(desttaskid, newpriority);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>  End-of Definition for All OpenCPU APIs
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
