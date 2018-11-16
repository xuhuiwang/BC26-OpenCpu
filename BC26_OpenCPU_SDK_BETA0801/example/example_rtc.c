/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2013
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   example_rtc.c
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   This example demonstrates how to use rtc function with APIs in OpenCPU.
 *
 * Usage:
 * ------
 *   Compile & Run:
 *
 *     Set "C_PREDEF=-D __EXAMPLE_RTC__" in gcc_makefile file. And compile the 
 *     app using "make clean/new".
 *     Download image bin to module to run.
 * 
 *   Operation:
 *             
 *
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * 
 ****************************************************************************/
#ifdef __EXAMPLE_RTC__
#include <string.h>
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_rtc.h"
#include "ql_uart.h"
#include "ql_stdlib.h"
#include "ql_error.h"
#include "ql_power.h"


#define DEBUG_ENABLE 1
#if DEBUG_ENABLE > 0
#define DEBUG_PORT  UART_PORT0
#define DBG_BUF_LEN   512
static char DBG_BUFFER[DBG_BUF_LEN];
#define APP_DEBUG(FORMAT,...) {\
    Ql_memset(DBG_BUFFER, 0, DBG_BUF_LEN);\
    Ql_sprintf(DBG_BUFFER,FORMAT,##__VA_ARGS__); \
    if (UART_PORT2 == (DEBUG_PORT)) \
    {\
        Ql_Debug_Trace(DBG_BUFFER);\
    } else {\
        Ql_UART_Write((Enum_SerialPort)(DEBUG_PORT), (u8*)(DBG_BUFFER), Ql_strlen((const char *)(DBG_BUFFER)));\
    }\
}
#else
#define APP_DEBUG(FORMAT,...) 
#endif


static Enum_SerialPort m_myUartPort  = UART_PORT0;


static u32 Rtc_id = 0x101; 
static u32 Rtc_Interval = 60*10*100;
static s32 m_param = 0;

static void Rtc_handler(u32 rtcId, void* param);

static void CallBack_UART_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara)
{
     
}

void proc_main_task(s32 taskId)
{
    s32 ret;
    ST_MSG msg;

    // Register & open UART port
    ret = Ql_UART_Register(m_myUartPort, CallBack_UART_Hdlr, NULL);
    if (ret < QL_RET_OK)
    {
        Ql_Debug_Trace("Fail to register serial port[%d], ret=%d\r\n", m_myUartPort, ret);
    }
    ret = Ql_UART_Open(m_myUartPort, 115200, FC_NONE);
    if (ret < QL_RET_OK)
    {
        Ql_Debug_Trace("Fail to open serial port[%d], ret=%d\r\n", m_myUartPort, ret);
    }
    
    APP_DEBUG("\r\n<--OpenCPU: Rtc_timer TEST!-->\r\n");  

    //register  a rtc
    ret = Ql_Rtc_RegisterFast(Rtc_id, Rtc_handler, &m_param);
    if(ret <0)
    {
        APP_DEBUG("\r\n<--failed!!, Ql_Rtc_RegisterFast: RTC id(%d) fail ,ret = %d -->\r\n",Rtc_id,ret);
    }
    APP_DEBUG("\r\n<--Ql_Rtc_RegisterFast: RTC id=%d, param = %d,ret = %d -->\r\n", Rtc_id ,m_param,ret); 

    //start a rtc ,repeat=true;
    ret = Ql_Rtc_Start(Rtc_id,Rtc_Interval,TRUE);
    if(ret < 0)
    {
        APP_DEBUG("\r\n<--failed!! Ql_Rtc_Start ret=%d-->\r\n",ret);        
    }
    APP_DEBUG("\r\n<--Ql_Rtc_Start(ID=%d,Interval=%d,) ret=%d-->\r\n",Rtc_id,Rtc_Interval,ret);

    while (1)
    {
         Ql_OS_GetMessage(&msg);
        switch(msg.message)
        {
        case 0:
            break;
        default:
            break;
        }
        
    }
}
// rtc callback function
void Rtc_handler(u32 rtcId, void* param)
{
	*((s32*)param) +=1;

    if(Rtc_id == rtcId)
    {
		//Ql_SleepDisable();
    }
}



#endif
