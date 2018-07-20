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
 *   example_tcpclient.c
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   This example demonstrates how to establish a TCP connection, when the module
 *   is used for the client.
 *
 * Usage:
 * ------
 *   Compile & Run:
 *
 *     Set "C_PREDEF=-D __EXAMPLE_TCPCLIENT__" in gcc_makefile file. And compile the 
 *     app using "make clean/new".
 *     Download image bin to module to run.
 * 
 *   Operation:
 *            set server parameter, which is you want to connect.
 *            Command:Set_Srv_Param=<srv ip>,<srv port>
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
#ifdef __EXAMPLE_TCPCLIENT__  
#include "custom_feature_def.h"
#include "ql_stdlib.h"
#include "ql_common.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_error.h"
#include "ql_uart.h"
#include "ql_uart.h"
#include "ql_timer.h"
#include "ril_network.h"
#include "ril_socket.h"
#include "ril.h"
#include "ril_util.h"


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


/*****************************************************************
* define process state
******************************************************************/
typedef enum{
    STATE_NW_QUERY_STATE,
    STATE_SOC_CREATE,
    STATE_SOC_CONNECT,
    STATE_SOC_SEND,
    STATE_SOC_CLOSE,
    STATE_TOTAL_NUM
}Enum_UDPSTATE;
static u8 m_udp_state = STATE_NW_QUERY_STATE;

/*****************************************************************
* UART Param
******************************************************************/
#define SERIAL_RX_BUFFER_LEN  2048
static u8 m_RxBuf_Uart[SERIAL_RX_BUFFER_LEN];

/*****************************************************************
* timer param
******************************************************************/
#define UDP_TIMER_ID         TIMER_ID_USER_START
#define UDP_TIMER_PERIOD     800
#define SEND_TIMER_PERIOD     2000


/*****************************************************************
* Server Param
******************************************************************/
#define SRVADDR_BUFFER_LEN  100
#define SEND_BUFFER_LEN     2048
#define RECV_BUFFER_LEN     2048
static Enum_SerialPort m_myUartPort  = UART_PORT0;



static u8 m_send_buf[SEND_BUFFER_LEN];
static u8 m_recv_buf[RECV_BUFFER_LEN];

static u8  m_SrvADDR[SRVADDR_BUFFER_LEN] = "220.180.239.212\0";
static u8  m_ipaddress[5];  //only save the number of server ip, remove the comma
static u32 m_SrvPort = 8055;
static s32 m_socketid = -1; 

static u8 send_buffer[10] = "12345678\0";  //hex data
#define SEND_LENGTH   4

/*****************************************************************
* uart callback function
******************************************************************/
static void CallBack_UART_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara);

/*****************************************************************
* timer callback function
******************************************************************/
static void Callback_Timer(u32 timerId, void* param);

/*****************************************************************
* other subroutines
******************************************************************/
extern s32 Analyse_Command(u8* src_str,s32 symbol_num,u8 symbol, u8* dest_buf);
static s32 ReadSerialPort(Enum_SerialPort port, /*[out]*/u8* pBuffer, /*[in]*/u32 bufLen);
static void proc_handle(u8 *pData,s32 len);



static s32 ret;
void proc_main_task(s32 taskId)
{
    ST_MSG msg;


    // Register & open UART port
    ret = Ql_UART_Register(m_myUartPort, CallBack_UART_Hdlr, NULL);
    ret = Ql_UART_Open(m_myUartPort, 115200, FC_NONE);
    APP_DEBUG("<--OpenCPU: TCP Client.-->\r\n");

    //register & start timer 
    Ql_Timer_Register(UDP_TIMER_ID, Callback_Timer, NULL);

    while(TRUE)
    {
        Ql_OS_GetMessage(&msg);
        switch(msg.message)
        {
#ifdef __OCPU_RIL_SUPPORT__
        case MSG_ID_RIL_READY:
            APP_DEBUG("<-- RIL is ready -->\r\n");
            Ql_RIL_Initialize();
            break;
#endif
		case MSG_ID_URC_INDICATION:
		{     
			switch (msg.param1)
            {
    		    case URC_SIM_CARD_STATE_IND:
    			APP_DEBUG("<-- SIM Card Status:%d -->\r\n", msg.param2);
				if(SIM_STAT_READY == msg.param2)
				{
					//RIL_SM_LOCK();
                   Ql_Timer_Start(UDP_TIMER_ID, UDP_TIMER_PERIOD, TRUE);
				}
    			break;			  
    		    case URC_SOCKET_RECV_DATA:
					
				Ql_Timer_Start(UDP_TIMER_ID, SEND_TIMER_PERIOD, TRUE);
		        APP_DEBUG("<-- RECV_DATA:%s -->\r\n", msg.param2);
    			break;
		        default:
    		    //APP_DEBUG("<-- Other URC: type=%d\r\n", msg.param1);
    	        break;
			}
		}
		break;
	default:
         break;
        }
    }
}
 
static void CallBack_UART_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara)
{
    switch (msg)
    {
    case EVENT_UART_READY_TO_READ:
        {

           s32 totalBytes = ReadSerialPort(port, m_RxBuf_Uart, sizeof(m_RxBuf_Uart));
           if (totalBytes > 0)
           {
               proc_handle(m_RxBuf_Uart,sizeof(m_RxBuf_Uart));
           }
           break;
        }
    case EVENT_UART_READY_TO_WRITE:
        break;
    default:
        break;
    }
}

static s32 ReadSerialPort(Enum_SerialPort port, /*[out]*/u8* pBuffer, /*[in]*/u32 bufLen)
{
    s32 rdLen = 0;
    s32 rdTotalLen = 0;
    if (NULL == pBuffer || 0 == bufLen)
    {
        return -1;
    }
    Ql_memset(pBuffer, 0x0, bufLen);
    while (1)
    {
        rdLen = Ql_UART_Read(port, pBuffer + rdTotalLen, bufLen - rdTotalLen);
        if (rdLen <= 0)  // All data is read out, or Serial Port Error!
        {
            break;
        }
        rdTotalLen += rdLen;
        // Continue to read...
    }
    if (rdLen < 0) // Serial Port Error!
    {
        APP_DEBUG("<--Fail to read from port[%d]-->\r\n", port);
        return -99;
    }
    return rdTotalLen;
}

static void proc_handle(u8 *pData,s32 len)
{
    u8 srvport[10];
	u8 *p = NULL;
    //command: Set_Srv_Param=<srv ip>,<srv port>
    p = Ql_strstr(pData,"Set_Srv_Param=");
    if (p)
    {
        Ql_memset(m_SrvADDR, 0, SRVADDR_BUFFER_LEN);
        if (Analyse_Command(pData, 1, '>', m_SrvADDR))
        {
            APP_DEBUG("<--Server Address Parameter Error.-->\r\n");
            return;
        }
        Ql_memset(srvport, 0, 10);
        if (Analyse_Command(pData, 2, '>', srvport))
        {
            APP_DEBUG("<--Server Port Parameter Error.-->\r\n");
            return;
        }
        m_SrvPort = Ql_atoi(srvport);
        APP_DEBUG("<--Set UDP Server Parameter Successfully<%s>,<%d>.-->\r\n",m_SrvADDR,m_SrvPort);

        m_udp_state = STATE_NW_QUERY_STATE;
        APP_DEBUG("<--Restart the UDP connection process.-->\r\n");

        return;
    }

    
}


static void Callback_Timer(u32 timerId, void* param)
{
    if (UDP_TIMER_ID == timerId)
    {
        //APP_DEBUG("<--...........m_udp_state=%d..................-->\r\n",m_udp_state);
        switch (m_udp_state)
        {        
            case STATE_NW_QUERY_STATE:
            {
                s32 cgreg = 0;
                ret = RIL_NW_GetEGPRSState(&cgreg);
                APP_DEBUG("<--Network State:cgreg=%d-->\r\n",cgreg);
                if((cgreg == NW_STAT_REGISTERED)||(cgreg == NW_STAT_REGISTERED_ROAMING))
                {
                    m_udp_state = STATE_SOC_CREATE;
                }
                break;
            }

            case STATE_SOC_CREATE:
            {
                m_socketid = RIL_QSOC_Create(1,1,1);//ipv4,tcp,ip
                if (m_socketid >= 0)
                {
                    APP_DEBUG("<--Create socket id successfully,socketid=%d.-->\r\n",m_socketid);
                    m_udp_state = STATE_SOC_CONNECT;
                }else
                {
                    APP_DEBUG("<--Create socket id failure,error=%d.-->\r\n",m_socketid);
                }
                break;
            }

			case STATE_SOC_CONNECT:
            {
                ret = RIL_QSOC_CONN(m_socketid,m_SrvPort, m_SrvADDR);
                if (ret == 0)
                {
                    APP_DEBUG("<-- connect socket successfully,%d\r\n",m_socketid);
                    m_udp_state = STATE_SOC_SEND;
                }else
                {
                    APP_DEBUG("<--connect socket id failure,error=%d.-->\r\n",ret);
                }
                break;
            }
            case STATE_SOC_SEND:
            {
 
                ret = RIL_QSOC_SEND(m_socketid,SEND_LENGTH,send_buffer);
				if(ret ==0)
				{
                    Ql_memset(m_send_buf,0,SEND_BUFFER_LEN);
                    m_udp_state = STATE_SOC_SEND;
                    APP_DEBUG("<--Send data successfully->\r\n");
					
					Ql_Timer_Stop(UDP_TIMER_ID);
				    break;
				}
                else if(ret <= 0)
                {
                    APP_DEBUG("<--Send data failure,ret=%d.-->\r\n",ret);
                    APP_DEBUG("<-- Close socket.-->\r\n");
					m_udp_state = STATE_SOC_CLOSE;
                }
                break;
            }
            case STATE_SOC_CLOSE:
            {
                ret = RIL_QSOC_CLOSE(m_socketid);
                if (ret == 0)
                {
                    APP_DEBUG("<-- closed socket successfully\r\n");
                    m_udp_state = STATE_TOTAL_NUM;
                }else
                {
                    APP_DEBUG("<--closed socket id failure,error=%d.-->\r\n",ret);
                }
                break;
            }
            default:
                break;
        }    
    }
}



#endif // __EXAMPLE_TCPCLIENT__

