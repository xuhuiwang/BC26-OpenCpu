#include "custom_feature_def.h"
#include "ql_common.h"
#include "ql_stdlib.h"
#include "ql_error.h"
#include "ql_trace.h"
#include "ql_timer.h"
#include "ql_system.h"
#include "ql_memory.h"
#include "ril_def.h"
#include "ril.h"
#include "ril_util.h"
//#include "ril_sms.h"
//#include "ril_telephony.h"

#ifdef __OCPU_RIL_SUPPORT__

/**************************************************************
 * Local Param
 **************************************************************/
static s32  m_thisTaskId = TaskId_End;   //current task id
static bool m_isRILInited = FALSE;       //URC initialization flag
static bool m_IsATHandling = FALSE;         //AT send status
static u32  m_nAtMutexId = 0;            //mutex ID

static char  m_strATCmd[RIL_MAX_AT_LEN]; // AT send buffer
static void* m_userdata = NULL;          // point to void *userdata

static char* m_pATRspLineBuf = NULL;    // point to a malloc block,which is used to store a line of AT response
static char* m_pATRspBuffer = NULL;      // point to m_RxBuf_VirtualPort1
static u32   m_nATRspBufLen = 0;         //The length of the data in m_RxBuf_VirtualPort1
static u8    m_RxBuf_VirtualPort1[SERIAL_RX_BUFFER_LEN];// AT response buffer

static s32  m_nATRsp_Result = 0;  //The results returned by the AT callback
static Callback_ATResponse m_atRsp_callback = NULL;     //AT callback function

//#define VIRTUAL_PORT3  (VIRTUAL_PORT2 + 1)
//static Enum_SerialPort m_ATSendPort = VIRTUAL_PORT3;    // Virtual serial port of RIL

static ST_RILGetLine m_data;

/**************************************************************
 * Global Param
 **************************************************************/
static u32  g_nEventGrpId = 0;


/**************************************************************
 * Debug Param
 **************************************************************/
#if RIL_DEBUG_ENABLE > 0
static char DBG_Buffer[512];
#endif

/**************************************************************
 * Local function declaration
 **************************************************************/
static void *Ql_RIL_GetLine(u8* pBuffer, u32 len);

/**************************************************************
 * extern  variable
 **************************************************************/
extern u32 g_ril_auxTaskId;
extern bool g_ril_auxTaskReady;

/**************************************************************
 * extern  function
 **************************************************************/
extern s32 Default_atRsp_callback(char* pAtRsp, u32 len, void* userdata);
extern s32 Ql_RIL_IsURCStr(const char* strRsp);

/**************************************************************
 * Timer Definition
 **************************************************************/
static ST_RILTimer m_atTimer = {0x1, 0, FALSE, FALSE};
static s32 ril_StartATTimer(void)
{
    s32 ret = QL_RET_OK;
    if (m_atTimer.runState != TRUE)
    {
        RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: AT Monitoring Timer Starts-->\r\n");
        m_atTimer.runState = TRUE;
        ret = Ql_Timer_Start(m_atTimer.id, m_atTimer.interval, m_atTimer.autoRepeat);
    }
    return ret;
}
static s32 ril_StopATTimer(void)
{
    s32 ret = QL_RET_OK;
    if (m_atTimer.runState)
    {
        RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: AT Monitoring Timer Stops-->\r\n");
        m_atTimer.runState = FALSE;
        ret = Ql_Timer_Stop(m_atTimer.id);
    }
    return ret;
}

/**************************************************************
 * Define the result of AT sended.
 **************************************************************/
static s32 m_nATErrCode = 0;
static void set_errCode(s32 errCode)
{
    m_nATErrCode = errCode;
}
static s32 get_errCode(void)
{
    return m_nATErrCode;
}

bool get_ril_init_status(void)
{
    return m_isRILInited;
}

#if 0
/**************************************************************
 * Uart callback function
 **************************************************************/
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
        RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Fail to read from port[%d]-->\r\n", port);
        return -99;
    }
    return rdTotalLen;
}

static void CallBack_SerialPort_Hdlr(Enum_SerialPort port, Enum_UARTEventType serialEvent, bool level, void* para)
{
    //QL_RIL_MAIN_DEBUG(DBG_Buffer,"<--[Ril_Task]: CallBack_VirtalSerialPort_Hdlr: port=%d, event=%d, level=%d, p=%x-->\r\n", port, serialEvent, level, para);
    switch (serialEvent)
    {
    case EVENT_UART_DCD_IND:
        {
            if (0)//(port == m_ATSendPort)
            {
                RIL_DEBUG(DBG_Buffer, "<-- Data mode: port=%d, level=%d, dcd_state=%d -->\r\n", port, level, Ql_UART_GetPinStatus(port, UART_PIN_DCD));
            }
            break;
        }
    case EVENT_UART_READY_TO_READ:
        {
            if (0)//(port == m_ATSendPort)
            {
                s32 dcd_sts;
                s32 totalBytes;
                totalBytes = ReadSerialPort(port, m_RxBuf_VirtualPort1, SERIAL_RX_BUFFER_LEN);
                if (totalBytes <= 0)
                {
                    RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: No data in UART buffer! -->\r\n");
                    return;
                }
                
                m_nATRspBufLen = totalBytes;
                m_pATRspBuffer = (char*)m_RxBuf_VirtualPort1;
                m_RxBuf_VirtualPort1[m_nATRspBufLen] = '\0';
                RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Receive serial data: %s -->\r\n", (char*)m_RxBuf_VirtualPort1);

                //dcd_sts = Ql_UART_GetPinStatus(port, UART_PIN_DCD);
                if (0 == dcd_sts)
                {// TODO: GPRS数据如何传递给上层App？？ Stanley 27Jun2013
                    RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Receive gprs data -->\r\n");
                    //ret = Ql_UART_Write(UART_PORT1, m_RxBuf_VirtualPort1, totalBytes);
                    //
                    // [Stanley][26Jun2015]:
                    // 根据DCD状态传递数据到App中，以实现对数据的支持。 
                    Ql_RIL_RcvDataFrmCore((u8*)m_pATRspBuffer, m_nATRspBufLen, NULL);
                }else{
                    do
                    {
                        ST_RILGetLine* data = NULL;
                        data = (ST_RILGetLine *)Ql_RIL_GetLine((u8*)m_pATRspBuffer, m_nATRspBufLen);
                        RIL_DEBUG(DBG_Buffer,"<-- [Ril_Task]: Ql_RIL_GetLine:%s, m_IsATHandling:%d, m_strATCmd:%s -->\r\n", data->line, m_IsATHandling, m_strATCmd);
                        if (TRUE == m_IsATHandling)//sending AT
                        {
                            // Is echo ?
                            if (Ql_strncmp(m_strATCmd, data->line, data->len) == 0)
                            {
                                // Ignore echo, go to get next line.
                                RIL_DEBUG(DBG_Buffer,"<-- [Ril_Task]: Ignore echo, go to get next line -->\r\n");
                            } 
                            else // AT response or URC
                            {
                                if (m_atRsp_callback)
                                {
                                    m_nATRsp_Result = m_atRsp_callback(data->line, data->len, m_userdata);
                                }else{
                                    m_nATRsp_Result = Default_atRsp_callback(data->line, data->len, m_userdata);//default callback;
                                }
                                RIL_DEBUG(DBG_Buffer,"<-- [Ril_Task]: m_nATRsp_Resul=%d -->\r\n", m_nATRsp_Result);

                                // 在上面的回调处理中，非简单类型的AT相应有可能也返回 "RIL_ATRSP_CONTINUE"，继而引发死等。 Stanely 25Oct2013
                                if (RIL_ATRSP_CONTINUE != m_nATRsp_Result)
                                {                        
                                    if (m_atRsp_callback)
                                    {
                                        m_atRsp_callback = NULL;
                                    }

                                    if (RIL_ATRSP_SUCCESS == m_nATRsp_Result)
                                    {
                                        set_errCode(RIL_AT_SUCCESS);
                                    }
                                    else if (RIL_ATRSP_FAILED == m_nATRsp_Result)
                                    {
                                        set_errCode(RIL_AT_FAILED);
                                    }

                                    RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Release Token -->\r\n");
                                    ril_StopATTimer();
                                    Ql_OS_SetEvent(g_nEventGrpId, RIL_EVENT_AT_RESPONSE);
                                    m_IsATHandling = FALSE;
                                }
                                else  //RIL_ATRSP_CONTINUE, URC(defined, undefined)
                                {
                                    if (Ql_RIL_IsURCStr(data->line))
                                    {
                                        char* pUrcData = NULL;
                                        u32 urcLen;

                                        RIL_DEBUG(DBG_Buffer,"<--1.[Ril_Task]: Receive URC from port[%d] -->\r\n", port);
                                        if (g_ril_auxTaskReady)
                                        {
                                            urcLen = data->len;
                                            pUrcData = (char*)Ql_MEM_Alloc(urcLen + 1);
                                            if (pUrcData)
                                            {
                                                Ql_memset(pUrcData, 0x0, urcLen + 1);
                                                Ql_memcpy(pUrcData, (char*)data->line, urcLen);
                                                Ql_OS_SendMessage(g_ril_auxTaskId, MSG_ID_URC_IND, urcLen, (u32)pUrcData);
                                            }else{
                                                RIL_DEBUG(DBG_Buffer,"<--1.[Ril_Task]: Fail to get memory for urc -->\r\n");
                                            }
                                        }else{
                                            RIL_DEBUG(DBG_Buffer,"<--1.[Ril_Task]: RIL aux task isn't ready, discard URC -->\r\n", port);
                                        }
                                    }
                                    else // Undefined URC, or non-final AT response
                                    {
                                        RIL_DEBUG(DBG_Buffer, "<-- 1.[Ril_Task]: Undefined URC, or non-final AT response -->\r\n");
                                    }
                                }
                            }
                        }
                        else  //not send AT
                        {
                            // All is URC except the cases above.  Stanley 23Oct2013
                            //if (Ql_RIL_IsURCStr(data->line))
                            {
                                char* pUrcData = NULL;
                                u32 urcLen;

                                RIL_DEBUG(DBG_Buffer,"<--2.[Ril_Task]: Receive URC from port[%d] -->\r\n", port);
                                if (g_ril_auxTaskReady)
                                {
                                    urcLen = data->len;
                                    pUrcData = (char*)Ql_MEM_Alloc(urcLen + 1);
                                    if (pUrcData)
                                    {
                                        Ql_memset(pUrcData, 0x0, urcLen + 1);
                                        Ql_memcpy(pUrcData, (char*)data->line, urcLen);
                                        Ql_OS_SendMessage(g_ril_auxTaskId, MSG_ID_URC_IND, urcLen, (u32)pUrcData);
                                    }else{
                                        RIL_DEBUG(DBG_Buffer,"<--2.[Ril_Task]: Fail to get memory for urc -->\r\n");
                                    }
                                }else{
                                    RIL_DEBUG(DBG_Buffer,"<-- 2.[Ril_Task]: RIL aux task isn't ready, discard URC -->\r\n", port);
                                }
                            }
                            /*
                            else
                            {
                                RIL_DEBUG(DBG_Buffer, "<-- [Ril_Task]: Undefined URC -->\r\n");
                            }*/
                        }


                        m_pATRspBuffer += data->len;
                        m_nATRspBufLen -= data->len;
                    } while (m_nATRspBufLen > 0);
                }
            }
            else if (UART_PORT1 == port || UART_PORT2 == port || UART_PORT3 == port)
            {// No UART data in this case...
            }
            break;
        }
    default:
        break;
    }
}
#endif

/**************************************************************
 * Timer callback function
 **************************************************************/
static void Callback_OnATTimer(u32 tmrId, void* param)
{
    RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: AT command timeout!-->\r\n");
    if (!m_atTimer.runState)
    {
        RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: AT Monitoring Timer timeout wrongly!!!-->\r\n");
        return;
    }
    ril_StopATTimer();
    set_errCode(RIL_AT_TIMEOUT);
    Ql_OS_SetEvent(g_nEventGrpId, RIL_EVENT_AT_RESPONSE);
}

/**************************************************************
 * main function
 **************************************************************/
void proc_reserved1(s32 TaskId)
{
    s32 ret;
    ST_MSG msg;
    
    RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: OpenCPU: RIL Starts.-->\r\n");

    #if 0
    // Register & open Modem port
    ret = Ql_UART_Register(m_ATSendPort, CallBack_SerialPort_Hdlr, NULL);
    if (ret < QL_RET_OK)
    {
        RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Fail to register serial port[%d], ret=%d-->\r\n", m_ATSendPort, ret);
    }
    
    ret = Ql_UART_Open(m_ATSendPort, 0, FC_NONE);
    if (ret < QL_RET_OK)
    {
        RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Fail to open serial port[%d], ret=%d-->\r\n", m_ATSendPort, ret);
    }
    #endif

    m_thisTaskId = TaskId;
    m_nAtMutexId = Ql_OS_CreateMutex();
    g_nEventGrpId = Ql_OS_CreateEvent();
    RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: g_nEventGrpId=0x%x -->\r\n", g_nEventGrpId);

    Ql_Timer_Register(m_atTimer.id, Callback_OnATTimer, NULL);
    Ql_OS_SendMessage(main_task_id, MSG_ID_RIL_READY, 0, 0);

    m_pATRspLineBuf = Ql_MEM_Alloc(SERIAL_RX_BUFFER_LEN + 1);
    if (!m_pATRspLineBuf)
    {
        RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Fail to get memory for line buffer -->\r\n");
    }
    
    while(TRUE)
    {
        Ql_OS_GetMessage(&msg);
        
        switch(msg.message)
        {
        case MSG_ID_SEND_AT:
            // Handle '+++'
            if (Ql_strncmp(m_strATCmd, "+++\r", 4) == 0)
            {
                //ret = Ql_UART_SendEscap(m_ATSendPort);
                set_errCode(RIL_AT_SUCCESS);
                if (ret < 0)
                {
                    RIL_DEBUG(DBG_Buffer, "[Ril_Task] Fail to send +++, cause:%d", ret);
                    ril_StopATTimer();
                    set_errCode(RIL_AT_FAILED);
                }
                Ql_OS_SetEvent(g_nEventGrpId, RIL_EVENT_AT_RESPONSE);
                break;
            }
           
            // AT command
            //ret = Ql_UART_Write(m_ATSendPort, (u8*)m_strATCmd, Ql_strlen(m_strATCmd));
            ret = Ql_ril_request_command(m_strATCmd, NULL, NULL);
            RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: send AT command ret=%d, timer=%d-->\r\n", ret, m_atTimer.runState);
            if (ret < 0)
            {
                RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Fail to send AT command!-->\r\n");
                ril_StopATTimer();
                set_errCode(RIL_AT_FAILED);
                Ql_OS_SetEvent(g_nEventGrpId, RIL_EVENT_AT_RESPONSE);
                return;
            }
            if (0 == m_atTimer.interval)
            {
                m_atTimer.interval = RIL_MAX_AT_TIMEOUT;
            }
            ril_StartATTimer();
            break;

        case MSG_ID_MD_URC_IND:
        case MSG_ID_MD_ATRSP_IND:
        {
            #if 0
            s32 dcd_sts;
            s32 totalBytes;
            totalBytes = ReadSerialPort(port, m_RxBuf_VirtualPort1, SERIAL_RX_BUFFER_LEN);
            if (totalBytes <= 0)
            {
                RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: No data in UART buffer! -->\r\n");
                return;
            }
            #endif
            
            m_nATRspBufLen = msg.param1 - 1; // cut \0
            m_pATRspBuffer = (char*)msg.param2;
            m_pATRspBuffer[m_nATRspBufLen] = '\0';

            RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Receive data from modem: len=%d, %s -->\r\n", m_nATRspBufLen, (char*)m_pATRspBuffer);

            #if 0
            dcd_sts = Ql_UART_GetPinStatus(port, UART_PIN_DCD);
            if (0 == dcd_sts)
            {// TODO: GPRS数据如何传递给上层App？？ Stanley 27Jun2013
                RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Receive gprs data -->\r\n");
                //ret = Ql_UART_Write(UART_PORT1, m_RxBuf_VirtualPort1, totalBytes);
                //
                // [Stanley][26Jun2015]:
                // 根据DCD状态传递数据到App中，以实现对数据的支持。 
                Ql_RIL_RcvDataFrmCore((u8*)m_pATRspBuffer, m_nATRspBufLen, NULL);
            }
            else
            #endif
            {
                do
                {
                    ST_RILGetLine* data = NULL;
                    data = (ST_RILGetLine *)Ql_RIL_GetLine((u8*)m_pATRspBuffer, m_nATRspBufLen);
                    RIL_DEBUG(DBG_Buffer,"<-- [Ril_Task]: Ql_RIL_GetLine:%s, m_IsATHandling:%d, m_strATCmd:%s -->\r\n", data->line, m_IsATHandling, m_strATCmd);

                    if (TRUE == m_IsATHandling)//sending AT
                    {
                        // Is echo ?
                        if (Ql_strncmp(m_strATCmd, data->line, data->len) == 0)
                        {
                            // Ignore echo, go to get next line.
                            RIL_DEBUG(DBG_Buffer,"<-- [Ril_Task]: Ignore echo, go to get next line -->\r\n");
                        } 
                        else // AT response or URC
                        {
                            if (m_atRsp_callback)
                            {
                                m_nATRsp_Result = m_atRsp_callback(data->line, data->len, m_userdata);
                            }else{
                                m_nATRsp_Result = Default_atRsp_callback(data->line, data->len, m_userdata);//default callback;
                            }
                            RIL_DEBUG(DBG_Buffer,"<-- [Ril_Task]: m_nATRsp_Resul=%d -->\r\n", m_nATRsp_Result);

                            // 在上面的回调处理中，非简单类型的AT相应有可能也返回 "RIL_ATRSP_CONTINUE"，继而引发死等。 Stanely 25Oct2013
                            if (RIL_ATRSP_CONTINUE != m_nATRsp_Result)
                            {                        
                                if (m_atRsp_callback)
                                {
                                    m_atRsp_callback = NULL;
                                }

                                if (RIL_ATRSP_SUCCESS == m_nATRsp_Result)
                                {
                                    set_errCode(RIL_AT_SUCCESS);
                                }
                                else if (RIL_ATRSP_FAILED == m_nATRsp_Result)
                                {
                                    set_errCode(RIL_AT_FAILED);
                                }

                                RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Release Token -->\r\n");
                                ril_StopATTimer();
                                Ql_OS_SetEvent(g_nEventGrpId, RIL_EVENT_AT_RESPONSE);
                                m_IsATHandling = FALSE;
                            }
                            else  //RIL_ATRSP_CONTINUE, URC(defined, undefined)
                            {
                                if (Ql_RIL_IsURCStr(data->line))
                                {
                                    char* pUrcData = NULL;
                                    u32 urcLen;

                                    RIL_DEBUG(DBG_Buffer,"<--1.[Ril_Task]: Receive URC from modem -->\r\n");
                                    if (g_ril_auxTaskReady)
                                    {
                                        urcLen = data->len;
                                        pUrcData = (char*)Ql_MEM_Alloc(urcLen + 1);
                                        if (pUrcData)
                                        {
                                            Ql_memset(pUrcData, 0x0, urcLen + 1);
                                            Ql_memcpy(pUrcData, (char*)data->line, urcLen);
                                            Ql_OS_SendMessage(g_ril_auxTaskId, MSG_ID_URC_IND, urcLen, (u32)pUrcData);
                                        }
                                        else
                                        {
                                            RIL_DEBUG(DBG_Buffer,"<--1.[Ril_Task]: Fail to get memory for urc -->\r\n");
                                        }
                                    }
                                    else
                                    {
                                        RIL_DEBUG(DBG_Buffer,"<--1.[Ril_Task]: RIL aux task isn't ready, discard URC -->\r\n");
                                    }
                                }
                                else // Undefined URC, or non-final AT response
                                {
                                    RIL_DEBUG(DBG_Buffer, "<-- 1.[Ril_Task]: Undefined URC, or non-final AT response -->\r\n");
                                }
                            }
                        }
                    }
                    else  //not send AT
                    {
                        // All is URC except the cases above.  Stanley 23Oct2013
                        //if (Ql_RIL_IsURCStr(data->line))
                        {
                            char* pUrcData = NULL;
                            u32 urcLen;

                            RIL_DEBUG(DBG_Buffer,"<--2.[Ril_Task]: Receive URC from modem -->\r\n");
                            if (g_ril_auxTaskReady)
                            {
                                urcLen = data->len;
                                pUrcData = (char*)Ql_MEM_Alloc(urcLen + 1);
                                if (pUrcData)
                                {
                                    Ql_memset(pUrcData, 0x0, urcLen + 1);
                                    Ql_memcpy(pUrcData, (char*)data->line, urcLen);
                                    Ql_OS_SendMessage(g_ril_auxTaskId, MSG_ID_URC_IND, urcLen, (u32)pUrcData);
                                }
                                else
                                {
                                    RIL_DEBUG(DBG_Buffer,"<--2.[Ril_Task]: Fail to get memory for urc -->\r\n");
                                }
                            }
                            else
                            {
                                RIL_DEBUG(DBG_Buffer,"<-- 2.[Ril_Task]: RIL aux task isn't ready, discard URC -->\r\n");
                            }
                        }
                        #if 0
                        else
                        {
                            RIL_DEBUG(DBG_Buffer, "<-- [Ril_Task]: Undefined URC -->\r\n");
                        }
                        #endif
                    }

                    m_pATRspBuffer += data->len;
                    m_nATRspBufLen -= data->len;
                } while (m_nATRspBufLen > 0);
            }
          
            Ql_MEM_Free(msg.param2); // free msg buf which alloc in resp/urc dispatch
            break;
        }
        default:
            break;
        }
    }
}

/**************************************************************
 * RIL Initialize
 **************************************************************/
void Ql_RIL_Initialize(void)
{
    extern const char* g_InitCmds[];
    extern u32 RIL_GetInitCmdCnt(void);
    
    s32 i,Cmdcnt;
    Cmdcnt = RIL_GetInitCmdCnt();
    RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Start Ql_RIL_Initialize... -->\r\n");
    
    for (i = 0 ; i < Cmdcnt; i++) 
    {
        RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Send AT: %s... -->\r\n", g_InitCmds[i]);
        
        m_IsATHandling = TRUE;
        Ql_memset(m_strATCmd, 0x0, sizeof(m_strATCmd));
        Ql_memcpy(m_strATCmd, g_InitCmds[i], Ql_strlen(g_InitCmds[i]));
        
        //Ql_UART_Write(m_ATSendPort, (u8*)g_InitCmds[i], Ql_strlen(g_InitCmds[i]));
        Ql_ril_request_command(m_strATCmd, NULL, NULL);
        
        Ql_OS_WaitEvent(g_nEventGrpId, RIL_EVENT_AT_RESPONSE, 0xffffffff);
        m_IsATHandling = FALSE;
    }
    RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Ql_RIL_Initialize finish -->\r\n");
    m_isRILInited = TRUE;
}

/**************************************************************
 * Related functions about AT send.
 **************************************************************/
extern const char* g_LimitedCmds[];
static s32 isLimitedCmd(const char* strCmd)
{
    s32 i;
    extern u32 RIL_GetLimitedCmdCnt(void);
    for (i = 0; i < RIL_GetLimitedCmdCnt(); i++){
        if (Ql_StrPrefixMatch(strCmd, g_LimitedCmds[i])){
            return 1;
        }
    }
    return 0;
}

static const u8 m_SpecialChar[] = {
    0x0A,   // LF
    0x0D,   // CR
    0x1A,   // Ctrl+Z
    0x1B    // ESC
};
static s32 startWithSpecialChar(const char* strCmd)
{
    s32 i;
    for (i = 0; i < NUM_ELEMS(m_SpecialChar); i++){
        if (*((u8*)strCmd) == m_SpecialChar[i]){
            return 1;
        }
    }
    return 0;
}

/******************************************************************************
* Function:     Ql_RIL_SendATCmd
*  
* Description:
*                This function implements sending AT command with the result  
*                being returned synchronously.The response of the AT command 
*                will be reported to the callback function,you can get the results
*                you want in it.
*
* Parameters:    
*                atCmd:     
*                     [in]AT command string.
*                atCmdLen:  
*                     [in]The length of AT command string.
*                atRsp_callBack: 
*                     [in]Callback function for handle the response of the AT command.
*                userData:  
*                     [out]Used to transfer the customer's parameter.
*                timeOut:   
*                     [in]Timeout for the AT command, unit in ms. If set it to 0,
*                           RIL uses the default timeout time (3min).
*
* Return:  
*                RIL_AT_SUCCESS,send AT successfully.
*                RIL_AT_FAILED, send AT failed.
*                RIL_AT_TIMEOUT,send AT timeout.
*                RIL_AT_BUSY,   sending AT.
*                RIL_AT_INVALID_PARAM, invalid input parameter.
*                RIL_AT_UNINITIALIZED, RIL is not ready, need to wait for MSG_ID_RIL_READY
*                                      and then call Ql_RIL_Initialize to initialize RIL.
******************************************************************************/
s32 Ql_RIL_SendATCmd(char*  atCmd, u32 atCmdLen, Callback_ATResponse atRsp_callBack, void* userData, u32 timeOut)
{
    s32 retVal;
    s32 cmdLen;
    char* p1 = NULL;
    char* p2 = NULL;
    extern s32 Ql_RIL_AT_SetErrCode(s32 errCode);

    Ql_RIL_AT_SetErrCode(RIL_ATRSP_FAILED); // 设置错误码的初始值，此错误码在AT的响应里被设置。如果timeout，就采用这里设置的初始化错误码。
    
    if (!m_isRILInited)
    {
        return RIL_AT_UNINITIALIZED;
    }

    /* DEL: 此判断会引起多任务AT在这里返回的问题。
     *      应该允许多Task中调用此接口，若busy时则pending在 Ql_OS_TakeMutex(m_nAtMutexId)处。
     * Stanley 09Nov2015
    */
    //if (TRUE == m_IsATHandling)
    //{
    //    return RIL_AT_BUSY;
    //}

    // Check validity of the input command
    if ((NULL == atCmd) || startWithSpecialChar(atCmd))
    {
        return RIL_AT_INVALID_PARAM;
    }

    /* If the AT command string containing\r\n,be set to NULL.
      In order to avoid the command execution many times.*/
    p1 = Ql_strstr(atCmd, "\r");
    if (p1)
    {
        *p1 = '\0';
    }
    p2 = Ql_strstr(atCmd, "\n");
    if (p2)
    {
        *p2 = '\0';
    }

    // RIL depends on some AT commands, which is not allowed for App.
    if (isLimitedCmd(atCmd))
    {
        return Ql_RET_NOT_SUPPORT;
    }

    Ql_OS_TakeMutex(m_nAtMutexId, 0xffffffff);

    // Clean AT buffer
    Ql_memset(m_strATCmd, 0x0, sizeof(m_strATCmd));
    cmdLen = Ql_strlen(atCmd);
    if (0 == cmdLen)
    {
        return RIL_AT_INVALID_PARAM;
    }

    // Convert AT to uppercase
    p1 = Ql_strstr(atCmd, "=");
    if (p1)
    {
        s32 atHeadLen = p1 - atCmd;
        Ql_memcpy(m_strATCmd, atCmd, atHeadLen);
        Ql_StrToUpper(m_strATCmd);
        Ql_memcpy(m_strATCmd + atHeadLen, atCmd + atHeadLen, cmdLen - atHeadLen);
    }else{
        Ql_memcpy(m_strATCmd, atCmd, cmdLen);
        Ql_StrToUpper(m_strATCmd);
    }

    // 自动添加回车符号
    m_strATCmd[cmdLen] = '\r';
    m_strATCmd[cmdLen + 1] = '\0';

    m_userdata = userData;
    m_atRsp_callback = atRsp_callBack;
    m_atTimer.interval = timeOut;

    m_IsATHandling = TRUE;
    Ql_OS_SendMessage(m_thisTaskId, MSG_ID_SEND_AT, 0, 0);
    RIL_DEBUG(DBG_Buffer,"\r\n\r\n<--[Ril_Task]: Send AT: %s -->\r\n", m_strATCmd);
    
    // Wait for the result...
    RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Wait for the at result... -->\r\n");
    retVal = Ql_OS_WaitEvent(g_nEventGrpId, RIL_EVENT_AT_RESPONSE, 0xffffffff);
    RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: Ql_OS_WaitEvent()=%d-->\r\n", retVal);
    
    retVal = get_errCode();
    Ql_OS_GiveMutex(m_nAtMutexId);
    
    RIL_DEBUG(DBG_Buffer,"<--[Ril_Task]: At response is returned, error code: %d\r\n", retVal);
    m_IsATHandling = FALSE;
    
    return retVal;
}


/*****************************************************************
* Function:     Ql_RIL_WriteDataToCore 
* 
* Description:
*               This function is used to send data to the core.   
* Parameters:
*               [in]data:
*                       Pointer to data to be send
*               [in]writeLen:
*                       The length of the data to be written
*
* Return:        
*               If >= 0, indicates success, and the return value 
*                        is the length of actual write data length
*               If <  0, indicates failure to write      
*
*****************************************************************/
s32 Ql_RIL_WriteDataToCore (u8* data, u32 writeLen )
{
    s32 ret = 0;
    
//    ret = Ql_UART_Write(m_ATSendPort, data,writeLen);
    return ret;
}

 /******************************************************************************
* Function:     Ql_RIL_FindLine
*  
* Description:
*                This function is used to get a line of data from the virtual serial
*                port buffer.
*
*
* Parameters:    
*                pBuffer:  pointer to virtual port buffer.
*                len: The length of buffer data.
*
* Return:  
                The function returns a pointer that contains the line address and line
                length. or a null pointer if the buffer is NULL.
******************************************************************************/
static void *Ql_RIL_GetLine(u8* pBuffer, u32 len)
{
    char* pHead  = NULL;
    char* pTail1 = NULL;
    char* pTail2 = NULL;
    char* pTail3 = NULL;
    char* pTemp  = NULL;

    pHead = (char*)pBuffer;
    Ql_memset(m_pATRspLineBuf, 0x0, SERIAL_RX_BUFFER_LEN + 1);

    if (Ql_StrPrefixMatch((char*)pBuffer, "\r\n"))
    {
        pTemp = (char*)(pBuffer + 2);
    } else {
        pTemp = (char*)pBuffer;
    }
    pTail1 = Ql_strstr(pTemp, "\r\n");
    pTail2 = Ql_strstr(pTemp, "\r");
    pTail3 = Ql_strstr(pTemp, "\n");
    if (pTail1 || pTail2 || pTail3)
    {
        u32 cpyLen;
        if (pTail1)
        {
            pTemp = pTail1;
            if (pTail2 && (pTail2 <= pTemp))
            {
                pTemp = pTail2;
            }
            if (pTail3 && (pTail3 <= pTemp))
            {
                pTemp = pTail3;
            }
        }else{
            if (pTail2)
            {
                pTemp = pTail2;
                if (pTail3 && (pTail3 <= pTemp))
                {
                    pTemp = pTail3;
                }
            }else{
                pTemp = pTail3;
            }
        }
        if (pTemp == pTail1)
        {
            cpyLen = pTemp - pHead + 2;
        }else{
            cpyLen = pTemp - pHead + 1;
        }
        Ql_memcpy(m_pATRspLineBuf, (char*)pBuffer, cpyLen);
        m_data.line = m_pATRspLineBuf;
        m_data.len = cpyLen;
    }
    else //no \r and no \n
    {
        Ql_memcpy(m_pATRspLineBuf, pHead, len);
        m_data.line = m_pATRspLineBuf;
        m_data.len = len;
    }
    return &m_data;
}

#endif // __OCPU_RIL_SUPPORT__
