#if 0
#include "ql_func.h"
#include "ql_wtd.h"
#include "ql_error.h"
#include "ql_trace.h"
#include "ql_system.h"
#include "ql_gpio.h"

#define WTD_TICK10MS  200//100       // GPT timer always runs with the period of 200ms
#define WTD_LIST_COUNT 10
#define GPTIMERID 0xF1
#define MIN_INTERVAL_TIME (2*WTD_TICK10MS)//200

typedef struct st_watchdog
{
    s32     WtdId;
    u32     Interval;
    s32     OverCounter;
}ST_WatchDog;

typedef struct watchdog_list
{
    ST_WatchDog wtd;
    struct watchdog_list* pNext;
}WatchDog_list;

static WatchDog_list* pWtdList = NULL;
static u8 WtdListCount  = 0;
static u8 WtdOverCounter = 0;
static s32 WtdResetMode = 0;
static Enum_PinName WtdPin = 0;
static u32 WtdInterval = 0;
static bool isWtdInit = FALSE;
static Enum_PinLevel Wtd_GPIO_LEVEL = PINLEVEL_LOW;
static void Ql_WTD_Calback(u32 timerId, void* param);

///////////////////////////////////////////////////////////////////
/// [BEGIN]:
/// Adopt the combination of GPT timer and Stack timer to feed the 
/// external watchdog, which can resolve the problem that module 
/// cannot go into saving power mode with GPT timer. 
/// Stanley 25Sep2014
///////////////////////////////////////////////////////////////////
typedef void (*OnWatchdog)(u32 timerId, void* param);
typedef s32 (*_api_OCPU_StartWatchdog_t)(u32 interval, OnWatchdog cb_wtd, void* param);
typedef s32 (*_api_OCPU_StopWatchdog_t)(void);

bool OCPU_WTD_GetInitState(void)
{
    return isWtdInit;
}

void OCPU_WTD_ChangeGPIOLevel(void)
{
    if(Wtd_GPIO_LEVEL == PINLEVEL_LOW)
    {
        Wtd_GPIO_LEVEL = PINLEVEL_HIGH;
    }else{
        Wtd_GPIO_LEVEL = PINLEVEL_LOW;
    }
    Ql_GPIO_SetLevel(WtdPin,Wtd_GPIO_LEVEL);
}
s32 OCPU_WTD_StartExtWatchdog_WithGPTimer(void)
{
    s32 ret;
    ret = Ql_Timer_RegisterFast(GPTIMERID, Ql_WTD_Calback, NULL);
    if (ret != QL_RET_OK)
    {
        return QL_RET_ERR_PARAM;
    }
//    ret = Ql_Timer_Start(GPTIMERID, WTD_TICK10MS, TRUE);
    //Ql_Debug_Trace("\r\n<### WTD: start GPT timer ###>\r\n\r\n");
    return ret;
}
s32 OCPU_WTD_StopExtWatchdog_WithGPTimer(void)
{
    //Ql_Debug_Trace("\r\n<### WTD: stop GPT timer ###>\r\n\r\n");
    return Ql_Timer_Stop(GPTIMERID);
}
s32 OCPU_WTD_StartExtWatchdog_WithStackTimer(void)
{
	s32 ptr = m_get_api_ptr("OCPU_StartWatchdog");
	if (0 == ptr)
	{
		//Ql_Debug_Trace(api_notsup, "OCPU_StartWatchdog");
		return Ql_RET_NOT_SUPPORT;
    }
    //Ql_Debug_Trace("\r\n<### WTD: start stack timer ###>\r\n\r\n");
	return ((_api_OCPU_StartWatchdog_t)ptr)(/*interval*/WTD_TICK10MS, Ql_WTD_Calback, NULL);
}
s32 OCPU_WTD_StopExtWatchdog_WithStackTimer(void)
{
	s32 ptr = m_get_api_ptr("OCPU_StopWatchdog");
	if (0 == ptr)
	{
		//Ql_Debug_Trace(api_notsup, "OCPU_StartWatchdog");
		return Ql_RET_NOT_SUPPORT;
    }
    //Ql_Debug_Trace("\r\n<### WTD: stop stack timer ###>\r\n\r\n");
	return ((_api_OCPU_StopWatchdog_t)ptr)();
}
///////////////////////////////////////////////////////////////////
/// [END]:
/// Adopt the combination of GPT timer and Stack timer to feed the 
/// external watchdog, which can resolve the problem that module 
/// cannot go into saving power mode with GPT timer. 
/// Stanley 25Sep2014
///////////////////////////////////////////////////////////////////

static void Ql_WTD_Calback(u32 timerId, void* param)
{
    WatchDog_list* pWtdListCurrent = NULL;
    
    if(pWtdList == NULL)
    {// Customer App doesn't create logic watchdog, then will not feed external watchdog.
        return;
    }    
    
    pWtdListCurrent = pWtdList;

    if(WtdResetMode == 0)
    {
        while(pWtdListCurrent != NULL)
        {                        
            if(pWtdListCurrent->wtd.OverCounter == 1)
            {
               return;
            }
            pWtdListCurrent->wtd.OverCounter--;
            pWtdListCurrent = pWtdListCurrent->pNext;
        }
        
        if(WtdOverCounter == 1)  // New watchdog sln without GPT timer. Stanley 24Sep2014
        {
            //Ql_Debug_Trace("<### Time to feed external WTD ###>\r\n");
            OCPU_WTD_ChangeGPIOLevel();                
            WtdOverCounter = WtdInterval / WTD_TICK10MS;
            return;
        }        
        WtdOverCounter--;
    }
    else if(WtdResetMode == 1)
    {// Without external watchdog, using sw watchdog
        while(pWtdListCurrent != NULL)
        {                        
            if(pWtdListCurrent->wtd.OverCounter == 1)
            {
                Ql_Reset(0);
                return;
            }
            pWtdListCurrent->wtd.OverCounter--;
            pWtdListCurrent = pWtdListCurrent->pNext;
        }
    }else{
        //
    }
}
s32 Ql_WTD_Init(s32 resetMode, Enum_PinName wtdPin, u32 interval)
{
    s32 ret;
    
    if(isWtdInit)
    {// Ql_WTD_Init() is only needed to be called once.
        return QL_RET_OK;
    }    
    if(resetMode == 0)
    {        
        // init gpio
        if(interval == 0 || interval < WTD_TICK10MS)
        {
            return QL_RET_ERR_PARAM;
        }        
        ret =  Ql_GPIO_Init(wtdPin,PINDIRECTION_OUT,PINLEVEL_LOW,PINPULLSEL_PULLUP);
        if (ret != QL_RET_OK)
        {
            return QL_RET_ERR_PARAM;
        }        

        WtdPin = wtdPin;
        WtdInterval = interval;
        WtdOverCounter= interval / WTD_TICK10MS;
    }
   
    WtdResetMode = resetMode;
        
#if 0   // GPT timer --> stack timer
    ret = Ql_Timer_RegisterFast(GPTIMERID,Ql_WTD_Calback,NULL);
    if(ret != QL_RET_OK)
    {
        return QL_RET_ERR_PARAM;
    }
    ret = Ql_Timer_Start(GPTIMERID,WTD_TICK10MS,TRUE);
#elif 1
    {
		s32 ptr = m_get_api_ptr("OCPU_StartWatchdog");
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, "OCPU_StartWatchdog");
			return Ql_RET_NOT_SUPPORT;
        }
		ret = ((_api_OCPU_StartWatchdog_t)ptr)(/*interval*/WTD_TICK10MS, Ql_WTD_Calback, NULL);
    }
#else
    /// [ADD]:
    /// Adopt the combination of GPT timer and Stack timer to feed the 
    /// external watchdog, which can resolve the problem that module 
    /// cannot go into saving power mode with GPT timer. 
    {
        extern bool OCPU_GetSleepEnableState(void);
        if (OCPU_GetSleepEnableState())
        {
            ret = OCPU_WTD_StartExtWatchdog_WithStackTimer();
        }else{
            ret = OCPU_WTD_StartExtWatchdog_WithGPTimer();
        }
    }
#endif
    if (ret != QL_RET_OK)
    {
        return QL_RET_ERR_PARAM;
    }
        
    isWtdInit = TRUE;
   
    return QL_RET_OK;
}
s32 Ql_WTD_Start(u32 interval)
{
    WatchDog_list* pWtdListCurrent = NULL;
    WatchDog_list* pWtdListTemp = NULL;
     
    if((WtdListCount> WTD_LIST_COUNT )|| (interval == 0) || (interval < MIN_INTERVAL_TIME) || (!isWtdInit))
    {
        return QL_RET_ERR_PARAM;
    }
    pWtdListTemp = (WatchDog_list*)Ql_MEM_Alloc(sizeof(WatchDog_list));
    if(pWtdListTemp ==  NULL)
    {
        return QL_RET_ERR_PARAM;
    }    
    WtdListCount++;    
    pWtdListTemp->wtd.WtdId = WtdListCount; //GPTIMERID + WtdListCount;    
    pWtdListTemp->wtd.Interval= interval;
    pWtdListTemp->wtd.OverCounter = interval /WTD_TICK10MS;    
    pWtdListTemp->pNext = NULL;
    ///add to list
    if(pWtdList == NULL)
    {
        pWtdList = pWtdListTemp;
    }
    else
    {
        pWtdListCurrent = pWtdList;
        while(pWtdListCurrent->pNext != NULL)
        {
            pWtdListCurrent = pWtdListCurrent->pNext;
        }
        pWtdListCurrent->pNext = pWtdListTemp;
    }
    
    return pWtdListTemp->wtd.WtdId;
    
}
void Ql_WTD_Feed(s32 wtdID)
{
    WatchDog_list* pWtdListCurrent = NULL;
    
    if(!isWtdInit)
    {
        return ;
    }

    if(pWtdList == NULL)
    {
        return;
    }
    pWtdListCurrent = pWtdList;
    while(pWtdListCurrent != NULL)
    {
        if(pWtdListCurrent->wtd.WtdId == wtdID)
        {            
            pWtdListCurrent->wtd.OverCounter = pWtdListCurrent->wtd.Interval / WTD_TICK10MS;
            break;
        }
        else
        {
            pWtdListCurrent = pWtdListCurrent->pNext;
        }
    }
}
void Ql_WTD_Stop(s32 wtdID)
{
    WatchDog_list* pWtdListFront = NULL;
    WatchDog_list* pWtdListCurrent = NULL;
    
    if(!isWtdInit)
    {
        return ;
    }

    if(pWtdList == NULL)
    {       
        return;
    }
    
    pWtdListFront = pWtdListCurrent = pWtdList;
    while(pWtdListCurrent != NULL)
    {
        if(pWtdListCurrent->wtd.WtdId == wtdID)
        {
            if(pWtdListFront != pWtdListCurrent)
            {
               pWtdListFront->pNext = pWtdListCurrent->pNext;
            }
            else
            {                
               pWtdList = pWtdListCurrent->pNext;
            }
            //free            
            Ql_MEM_Free(pWtdListCurrent);            
            WtdListCount--;                    
            break;
        }
        pWtdListFront = pWtdListCurrent;
        pWtdListCurrent = pWtdListCurrent->pNext; 
        
    }    
}
#endif
