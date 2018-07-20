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
 *   ril_network.c 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   The module implements network related APIs.
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
#include "custom_feature_def.h"
#include "ril_network.h"
#include "ril.h"
#include "ril_util.h"
#include "ql_stdlib.h"
#include "ql_trace.h"
#include "ql_error.h"
#include "ql_system.h"
#include "ql_trace.h"
#include "ql_common.h"
#include "ril_LwM2M.h"
#include "ql_uart.h"

#ifdef __OCPU_RIL_SUPPORT__

#define RIL_LwM2M_DEBUG_ENABLE 0
#if RIL_LwM2M_DEBUG_ENABLE > 0
#define RIL_LwM2M_DEBUG_PORT  UART_PORT0
static char DBG_Buffer[512];
#define RIL_LwM2M_DEBUG(BUF,...)  QL_TRACE_LOG(RIL_LwM2M_DEBUG_PORT,BUF,512,__VA_ARGS__)
#else
#define RIL_LwM2M_DEBUG(BUF,...) 
#endif

static LwM2M_Recv_Param Lwm2m_recv_param;

static s32 ATResponse_Handler(char* line, u32 len, void* userData)
{
    RIL_LwM2M_DEBUG(DBG_Buffer,"[ATResponse_Handler] %s\r\n", (u8*)line);
    
    if (Ql_RIL_FindLine(line, len, "OK"))
    {  
        return  RIL_ATRSP_SUCCESS;
    }
    else if (Ql_RIL_FindLine(line, len, "ERROR"))
    {  
        return  RIL_ATRSP_FAILED;
    }
    else if (Ql_RIL_FindString(line, len, "+CME ERROR"))
    {
        return  RIL_ATRSP_FAILED;
    }
    else if (Ql_RIL_FindString(line, len, "+CMS ERROR:"))
    {
        return  RIL_ATRSP_FAILED;
    }
    
    return RIL_ATRSP_CONTINUE; //continue wait
}


static s32 ATResponse_Update_Handler(char* line, u32 len, void* userData)
{
	RIL_LwM2M_DEBUG(DBG_Buffer,"[ATResponse_Handler] %s\r\n", (u8*)line);
	
	if (Ql_RIL_FindLine(line, len, "UPDATE OK"))
	{  
		return	RIL_ATRSP_SUCCESS;
	}
	else if (Ql_RIL_FindLine(line, len, "ERROR"))
	{  
		return	RIL_ATRSP_FAILED;
	}
	else if (Ql_RIL_FindString(line, len, "+CME ERROR"))
	{
		return	RIL_ATRSP_FAILED;
	}
	else if (Ql_RIL_FindString(line, len, "+CMS ERROR:"))
	{
		return	RIL_ATRSP_FAILED;
	}
	else if (Ql_RIL_FindString(line, len, "UPDATE FAIL"))
	{
		return	RIL_ATRSP_FAILED;
	}
	
	return RIL_ATRSP_CONTINUE; //continue wait
}

static s32 ATResponse_Send_Handler(char* line, u32 len, void* userData)
{
	RIL_LwM2M_DEBUG(DBG_Buffer,"[ATResponse_Handler] %s\r\n", (u8*)line);
	
	if (Ql_RIL_FindLine(line, len, "SEND OK"))
	{  
		return	RIL_ATRSP_SUCCESS;
	}
	else if (Ql_RIL_FindLine(line, len, "ERROR"))
	{  
		return	RIL_ATRSP_FAILED;
	}
	else if (Ql_RIL_FindString(line, len, "+CME ERROR"))
	{
		return	RIL_ATRSP_FAILED;
	}
	else if (Ql_RIL_FindString(line, len, "+CMS ERROR:"))
	{
		return	RIL_ATRSP_FAILED;
	}
	else if (Ql_RIL_FindString(line, len, "SEND FAIL"))
	{
		return	RIL_ATRSP_FAILED;
	}
	
	return RIL_ATRSP_CONTINUE; //continue wait
}


static s32 ATResponse_Recv_Handler(char* line, u32 len, void* userData)
{
	 LwM2M_Recv_Param*lwm2m_param = (LwM2M_Recv_Param *)userData;
     u8* head = Ql_RIL_FindString(line, len, lwm2m_param->prefix); //continue wait
    if(head)
    {
            char strTmp[10];
            char* p1 = NULL;
            char* p2 = NULL;
            Ql_memset(strTmp, 0x0, sizeof(strTmp));
            p1 = Ql_strstr(head, ":");
			p1++;
			if(0 == ((*p1)-0x30))
			{
                lwm2m_param->actual_length = 0;
			    lwm2m_param->remain_length = 0;
			    lwm2m_param->buffer = NULL;
			    return RIL_ATRSP_SUCCESS;
			}
			else
			{
				p2 = Ql_strstr(p1, "\r\n");
             	*p2 = '\0';
				QSDK_Get_Str(p1,strTmp,0);
				lwm2m_param->actual_length= Ql_atoi(strTmp);
				
				Ql_memset(strTmp, 0x0, sizeof(strTmp));
				QSDK_Get_Str(p1,strTmp,1);
				lwm2m_param->remain_length= Ql_atoi(strTmp);

    			return  RIL_ATRSP_CONTINUE;
			}

    }
	if(0 != lwm2m_param->actual_length)
	{
		head = Ql_RIL_FindString(line, len,"\r\n"); //continue wait
		*head = '\0';
		Ql_memcpy(lwm2m_param->buffer,head,len-2);
		
		
		return	RIL_ATRSP_CONTINUE;

	}
    head = Ql_RIL_FindString(line, len, "OK");
    if(head)
    {  
        return  RIL_ATRSP_CONTINUE;  
    }
    head = Ql_RIL_FindString(line, len, "ERROR");
    if(head)
    {  
        return  RIL_ATRSP_FAILED;
    }
    head = Ql_RIL_FindString(line, len, "+CME ERROR:");//fail
    if(head)
    {
        return  RIL_ATRSP_FAILED;
    }
    head = Ql_RIL_FindString(line, len, "+CMS ERROR:");//fail
    if(head)
    {
        return  RIL_ATRSP_FAILED;
    }
    return RIL_ATRSP_CONTINUE; //continue wait
}

s32  RIL_QLwM2M_Serv(u8* ip_addr,u32 port)
{
    s32 retRes = 0;
	s32 socid = 0;
    char strAT[200] ;

    Ql_memset(strAT,0x00, sizeof(strAT));
    Ql_sprintf(strAT,"AT+QLWSERV=\"%s\",%d\n",ip_addr,port);
    
    retRes = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT), ATResponse_Handler,NULL,0);
	RIL_LwM2M_DEBUG(DBG_Buffer,"<-- Send AT:%s, ret = %d -->\r\n",strAT,retRes);
    return retRes;

}



s32 RIL_QLwM2M_Conf(u8* endpoint) 
{
    s32 ret = RIL_AT_SUCCESS;
    char strAT[200];

    Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT,"AT+QLWCONF=\"%s\"\n",endpoint);
    ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Handler,NULL,0);
	RIL_LwM2M_DEBUG(DBG_Buffer,"<-- Send AT:%s, ret = %d -->\r\n",strAT, ret);
    return ret;
    
}

s32 RIL_QLwM2M_Aaddobj(u8 obj_id,u8 ins_id,u8 res_num,u8* res_id) 
{
    s32 ret = RIL_AT_SUCCESS;
    char strAT[200];
	
    Ql_memset(strAT, 0, sizeof(strAT));
	if(NULL == res_id)
	{
		 Ql_sprintf(strAT, "AT+QLWADDOBJ=%d,%d\n",obj_id,ins_id);
	}
	else 
	{
		Ql_sprintf(strAT, "AT+QLWADDOBJ=%d,%d,%d,\"%s\"\n",obj_id,ins_id, res_num,res_id);

	}
    ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Handler,NULL,0);
	RIL_LwM2M_DEBUG(DBG_Buffer,"<--Send AT:%s, ret = %d -->\r\n",strAT, ret);
    return ret;
    
}

s32 RIL_QLwM2M_Open(u8 mode) 
{
    s32 ret = RIL_AT_SUCCESS;
    char strAT[200];

    Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT,"AT+QLWOPEN=%d\n",mode);
    ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Handler,NULL,0);
	RIL_LwM2M_DEBUG(DBG_Buffer,"<-- Send AT:%s, ret = %d -->\r\n",strAT, ret);
    return ret;
    
}

s32 RIL_QLwM2M_Update(void) 
{
    s32 ret = RIL_AT_SUCCESS;
    char strAT[200];

    Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT,"AT+QLWUPDATE");
    ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Update_Handler,NULL,0);
	RIL_LwM2M_DEBUG(DBG_Buffer,"<-- Send AT:%s, ret = %d -->\r\n",strAT, ret);
    return ret;
    
}

s32 RIL_QLwM2M_Cfg(u8 send_format,u8 recv_format) 
{
    s32 ret = RIL_AT_SUCCESS;
    char strAT[200];

    Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT,"AT+QLWCFG=\"dataformat\",%d,%d\n",send_format,recv_format);
    ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Handler,NULL,0);
	RIL_LwM2M_DEBUG(DBG_Buffer,"<-- Send AT:%s, ret = %d -->\r\n",strAT, ret);
    return ret;    
}

s32 RIL_QLwM2M_Send(u8 obj_id,u8 ins_id,u8 res_id,u32 length,u8* data,u8* mode) 
{
    s32 ret = RIL_AT_SUCCESS;
    char strAT[200];
	
	if(NULL != data)
	{
      Ql_sprintf(strAT, "AT+QLWDATASEND=%d,%d,%d,%d,%s,%s\n",obj_id,ins_id,res_id,length,data,mode);
	}
	else
	{
		return RIL_AT_INVALID_PARAM;
	}
	if(0== Ql_memcmp(mode,"0x0000",sizeof("0x0000")))
	{
      ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Handler,NULL,0);
	}
	else if(0== Ql_memcmp(mode,"0x0100",sizeof("0x0100")))
	{
		ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Send_Handler,NULL,0);
	}
    RIL_LwM2M_DEBUG(DBG_Buffer,"<-- Send AT:%s, ret = %d -->\r\n",strAT, ret); 
    return ret;    
}


s32 RIL_QLwM2M_RD(u32 rd_length,u32* actual_length,u32* remain_length,u8* data_buffer)
{
    s32 ret = RIL_AT_SUCCESS;
    char strAT[200];
	Lwm2m_recv_param.buffer = data_buffer;
	Lwm2m_recv_param.prefix="+QLWRD:";

    Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT,"AT+QLWRD=%d\n",rd_length);
    ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Recv_Handler,&Lwm2m_recv_param,0);
	*actual_length = Lwm2m_recv_param.actual_length;
	*remain_length = Lwm2m_recv_param.remain_length;
	
	RIL_LwM2M_DEBUG(DBG_Buffer,"<-- Send AT:%s, ret = %d -->\r\n",strAT, ret);
    return ret; 
}


s32 RIL_QLwM2M_Close(void) 
{
    s32 ret = RIL_AT_SUCCESS;
    char strAT[200];

    Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT,"AT+QLWCLOSE\n");
    ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Handler,NULL,0);
	RIL_LwM2M_DEBUG(DBG_Buffer,"<-- Send AT:%s, ret = %d -->\r\n",strAT, ret);
    return ret;    
}


#endif  //__OCPU_RIL_SUPPORT__

