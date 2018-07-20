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
 *   example_LwM2M.c
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   This example demonstrates how to use LwM2M function with APIs in OpenCPU.
 *
 * Usage:
 * ------
 *   Compile & Run:
 *
 *     Set "C_PREDEF=-D __EXAMPLE_LwM2M__" in gcc_makefile file. And compile the 
 *     app using "make clean/new".
 *     Download image bin to module to run.
 * 
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * 
 ****************************************************************************/
#ifdef __EXAMPLE_LwM2M__
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
#include "ril_LwM2M.h"
#include "ril.h"
#include "ril_util.h"
#include "ril_system.h"



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
    STATE_LwM2M_SERV,
    STATE_LwM2M_CONF,
    STATE_LwM2M_ADDOBJ,
    STATE_LwM2M_OPEN,
    STATE_LwM2M_UPDATE,
    STATE_LwM2M_CFG,
    STATE_LwM2M_SEND,
    STATE_LwM2M_CLOSE,
    STATE_TOTAL_NUM
}Enum_UDPSTATE;
static u8 m_udp_state = STATE_NW_QUERY_STATE;


/*****************************************************************
* LwM2M param
******************************************************************/
#define LwM2M_TIMER_ID         TIMER_ID_USER_START
#define LwM2M_TIMER_PERIOD     1000


/*****************************************************************
* Server Param
******************************************************************/
#define SRVADDR_BUFFER_LEN  100
#define SEND_BUFFER_LEN     2048
#define RECV_BUFFER_LEN     2048
static Enum_SerialPort m_myUartPort  = UART_PORT0;



static u8 m_send_buf[SEND_BUFFER_LEN]={0};
static u8 m_recv_buf[RECV_BUFFER_LEN]={0};

static u8  m_SrvADDR[SRVADDR_BUFFER_LEN] = "180.101.147.115\0";
static u8  m_ipaddress[5];  //only save the number of server ip, remove the comma
static u32 m_SrvPort = 5683;
static s32 m_socketid = -1; 


u8 obj_id = 19;  //Object id. The max object id number is 65535
u8 ins_id_0 = 0;   //Instance id
u8 ins_id_1 = 1;   //Instance id
u8 res_num = 1;   //Resources id number
u8 res_id[5] = "0\0";    // Resources id.
u8 res_id_int = 0;    // Resources id.

u8 mode = 0;//Direct push mode


u8 send_format = 0;//0:Text mode,1:Hex mode
u8 recv_format = 1;//0:Text mode,1:Hex mode

static u8 test_data[256] = "0111\0"; //send data
u8 length =0; //send length
u8 send_mode[8] = "0x0000";////0x0000:Send NON message,0x0100: Send CON message

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
    APP_DEBUG("<--OpenCPU: LwM2M Client.-->\r\n");

    //register & start timer 
    Ql_Timer_Register(LwM2M_TIMER_ID, Callback_Timer, NULL);

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
				   //ret = Ql_RIL_SendATCmd("AT+SM=LOCK\r\n",Ql_strlen("AT+SM=LOCK\r\n"),NULL,NULL,0);
                   //APP_DEBUG("Ql_RIL_SendATCmd = %d", ret);
                   Ql_Timer_Start(LwM2M_TIMER_ID, LwM2M_TIMER_PERIOD, TRUE);
				}
    			break;	
				case URC_LwM2M_OBSERVE:
				{
					if(msg.param2 == 0)
					{
     					APP_DEBUG("<-- URC_LwM2M_OBSERVE-->\r\n");
     					m_udp_state = STATE_LwM2M_CFG;
     					Ql_Timer_Start(LwM2M_TIMER_ID, LwM2M_TIMER_PERIOD, TRUE);
					}
				}
				break;
    		    case URC_LwM2M_RECV_DATA:
				{
    				
    		        APP_DEBUG("<-- RECV_DATA:%s -->\r\n", msg.param2);
					Ql_Timer_Start(LwM2M_TIMER_ID, LwM2M_TIMER_PERIOD, FALSE);
					m_udp_state = STATE_LwM2M_SEND;
    		    }
    			break;
	
		        default:
    		    APP_DEBUG("<-- Other URC: type=%d\r\n", msg.param1);
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
           break;
        }
    case EVENT_UART_READY_TO_WRITE:
        break;
    default:
        break;
    }
}

static void Callback_Timer(u32 timerId, void* param)
{
    if (LwM2M_TIMER_ID == timerId)
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
                    m_udp_state = STATE_LwM2M_SERV;
                }
                break;
            }

            case STATE_LwM2M_SERV:
            {
                ret = RIL_QLwM2M_Serv(m_SrvADDR,m_SrvPort);
                if (RIL_AT_SUCCESS == ret)
                {
                    APP_DEBUG("<-configure address and port successfully-->\r\n");
                    m_udp_state = STATE_LwM2M_CONF;
                }else
                {
                    APP_DEBUG("<--configure address and port failure, error=%d.-->\r\n",ret);
                }
                break;
            }

			case STATE_LwM2M_CONF:
            {
				char strImei[30];
               Ql_memset(strImei, 0x0, sizeof(strImei));
        		
        	    ret = RIL_GetIMEI(strImei);
                APP_DEBUG("<-- IMEI:%s,ret=%d -->\r\n", strImei,ret);
                
                ret = RIL_QLwM2M_Conf(strImei);
                if (RIL_AT_SUCCESS == ret)
                {
                    APP_DEBUG("<-- Configure Parameters successfully\r\n");
                    m_udp_state = STATE_LwM2M_ADDOBJ;
                }else
                {
                    APP_DEBUG("<--Configure Parameters failure,error=%d.-->\r\n",ret);
                }
                break;
            }
            case STATE_LwM2M_ADDOBJ:
            {

                ret = RIL_QLwM2M_Aaddobj(obj_id,ins_id_0,res_num,res_id);
				if(RIL_AT_SUCCESS != ret)
                {
                    APP_DEBUG("<--Add object failure,ret=%d.-->\r\n",ret);
                }
                
				ret = RIL_QLwM2M_Aaddobj(obj_id,ins_id_1,res_num,res_id);
				if (RIL_AT_SUCCESS == ret)
				{
                    m_udp_state = STATE_LwM2M_OPEN;
                    APP_DEBUG("<--Add object successfully->\r\n");
					
				    break;
				}
                else if(ret <0)
                {
                    APP_DEBUG("<--Add object failure,ret=%d.-->\r\n",ret);
                }
                break;
            }
            case STATE_LwM2M_OPEN:
            {
				Ql_Timer_Stop(LwM2M_TIMER_ID);
                ret = RIL_QLwM2M_Open(mode);
                if (RIL_AT_SUCCESS == ret)
                {
                    APP_DEBUG("<-- Open a Register Request successfully\r\n");
                }else
                {
                    APP_DEBUG("<--Open a Register Request failure,error=%d.-->\r\n",ret);
                }
                break;
            }

			case STATE_LwM2M_UPDATE:
            {
                ret = RIL_QLwM2M_Update();
                if (RIL_AT_SUCCESS == ret)
                {
                    APP_DEBUG("<-- Send an Update Request successfully\r\n");
                    m_udp_state = STATE_LwM2M_CFG;
                }else
                {
                    APP_DEBUG("<--Send an Update Request failure,error=%d.-->\r\n",ret);
                }
                break;
            }

			case STATE_LwM2M_CFG:
            {
                ret = RIL_QLwM2M_Cfg(send_format,recv_format);
                if (RIL_AT_SUCCESS == ret)
                {
                    APP_DEBUG("<--Configure Optional Parameters successfully\r\n");
                    m_udp_state = STATE_LwM2M_SEND;
                }else
                {
                    APP_DEBUG("<--Configure Optional Parameters failure,error=%d.-->\r\n",ret);
                }
                break;
            }

			 case STATE_LwM2M_SEND:
            {
				length = Ql_strlen(test_data);
				res_id_int = Ql_atoi(res_id);
                ret = RIL_QLwM2M_Send(obj_id,ins_id_0,res_id_int,length,test_data,send_mode) ;
                if (RIL_AT_SUCCESS == ret)
                {
					Ql_Timer_Stop(LwM2M_TIMER_ID);
                    APP_DEBUG("<-- send data successfully\r\n");
                    m_udp_state = STATE_TOTAL_NUM;
					
                }else
                {
                    APP_DEBUG("<--send data failure,error=%d.-->\r\n",ret);
					m_udp_state = STATE_LwM2M_CLOSE;
                }
                break;
            }
			case STATE_LwM2M_CLOSE:
            {
                ret = RIL_QLwM2M_Close();
                if (RIL_AT_SUCCESS == ret)
                {
                    APP_DEBUG("<-- Send a Deregister Request successfully\r\n");
                    m_udp_state = STATE_TOTAL_NUM;
                }else
                {
                    APP_DEBUG("<--Send a Deregister Request failure,error=%d.-->\r\n",ret);
                }
                break;
            }
			case STATE_TOTAL_NUM:
            {
  
              APP_DEBUG("<--lwM2M test case-->\r\n",ret);
			  m_udp_state = STATE_TOTAL_NUM;
            }
            default:
                break;
        }    
    }
}

#endif // __EXAMPLE_LwM2M__

