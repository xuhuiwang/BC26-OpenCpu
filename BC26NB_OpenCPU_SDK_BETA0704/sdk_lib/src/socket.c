#if 0
#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_socket.h"
#include "ql_error.h "

static _api_SOC_Register_t m_SOC_Register=NULL;
static _api_SOC_Create_t m_SOC_Create=NULL;
static _api_SOC_CreateEx_t m_SOC_CreateEx=NULL;
static _api_Ql_SOC_Close_t m_Ql_SOC_Close=NULL;
static _api_SOC_Connect_t m_SOC_Connect=NULL;
static _api_SOC_ConnectEx_t m_SOC_ConnectEx=NULL;
static _api_SOC_Send_t m_SOC_Send=NULL;
static _api_Ql_SOC_Recv_t m_Ql_SOC_Recv=NULL;
static _api_SOC_GetAckNumber_t m_Ql_SOC_GetAckNumber=NULL;
static _api_SOC_SendTo_t m_SOC_SendTo=NULL;
static _api_SOC_RecvFrom_t m_SOC_RecvFrom=NULL;
static _api_SOC_Bind_t m_SOC_Bind=NULL;
static _api_SOC_Listen_t m_SOC_Listen=NULL;
static _api_SOC_Accept_t m_SOC_Accept=NULL;
static _api_IpHelper_GetIPByHostName_t m_IpHelper_GetIPByHostName=NULL;
static _api_IpHelper_GetIPByHostNameEx_t m_IpHelper_GetIPByHostNameEx=NULL;
static _api_IpHelper_ConvertIpAddr_t m_IpHelper_ConvertIpAddr=NULL;
static _api_SOC_SetOption_t m_SOC_SetOption=NULL;
static _api_SOC_SSL_InjectCertification_t m_SOC_SSL_InjectCertification=NULL;


s32 Ql_SOC_Register(ST_SOC_Callback cb, void* customParam)
{
	if (m_SOC_Register==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_SOC_Register");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SOC_Register");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_SOC_Register=(_api_SOC_Register_t)ptr;
    }
	return m_SOC_Register(cb, customParam);
}

s32  Ql_SOC_Create(u8 contextId, u8 socketType)
{
	if (m_SOC_Create==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_SOC_Create");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SOC_Create");		
			return -127;
        }
		m_SOC_Create=(_api_SOC_Create_t)ptr;
    }
	 return m_SOC_Create(contextId, socketType);	
}

s32  Ql_SOC_CreateEx(u8 contextId, u8 socketType,s32 taskId, ST_SOC_Callback cb)
{
    if (NULL == m_SOC_CreateEx)
    {
        s32 ptr = m_get_api_ptr("Ql_SOC_CreateEx");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_SOC_CreateEx");		
            return -127;
        }
        m_SOC_CreateEx=(_api_SOC_CreateEx_t)ptr;
    }
    return m_SOC_CreateEx(contextId, socketType, taskId, cb);	
}

s32 Ql_SOC_Close(s32 socketId)
{
	if (m_Ql_SOC_Close==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_SOC_Close");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SOC_Close");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_Ql_SOC_Close=(_api_Ql_SOC_Close_t)ptr;
    }
	 return m_Ql_SOC_Close(socketId);
}

s32 Ql_SOC_Connect(s32 socketId, u32 remoteIP, u16 remotePort)
{
	if (m_SOC_Connect==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_SOC_Connect");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SOC_Connect");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_SOC_Connect=(_api_SOC_Connect_t)ptr;
    }
	return m_SOC_Connect(socketId, remoteIP, remotePort);
}

s32 Ql_SOC_ConnectEx(s32 socketId, u32 remoteIP, u16 remotePort, bool isBlocking)
{
    if (NULL == m_SOC_ConnectEx)
    {
        s32 ptr = m_get_api_ptr("Ql_SOC_ConnectEx");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_SOC_ConnectEx");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_SOC_ConnectEx = (_api_SOC_ConnectEx_t)ptr;
    }
    return m_SOC_ConnectEx(socketId, remoteIP, remotePort, isBlocking);
}

s32 Ql_SOC_Send(s32 socketId, u8* pData, s32 dataLen)
{
	if (m_SOC_Send==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_SOC_Send");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SOC_Send");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_SOC_Send=(_api_SOC_Send_t)ptr;
    }
	 return m_SOC_Send(socketId, pData, dataLen);	
}

s32 Ql_SOC_Recv(s32 socketId, u8* pBuffer, s32 dataLen)
{
    if (m_Ql_SOC_Recv==NULL)
    {
    	s32 ptr = m_get_api_ptr("Ql_SOC_Recv");
    	if (0 == ptr)
    	{
    		//Ql_Debug_Trace(api_notsup, "Ql_SOC_Recv");		
    		return Ql_RET_NOT_SUPPORT;
        }
    	m_Ql_SOC_Recv=(_api_Ql_SOC_Recv_t)ptr;
    }
    return m_Ql_SOC_Recv(socketId, pBuffer, dataLen);
}

s32 Ql_SOC_GetAckNumber (s32 socketId, u64* ackNum)
{
	if (m_Ql_SOC_GetAckNumber==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_SOC_GetAckNumber");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SOC_GetAckNumber");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_Ql_SOC_GetAckNumber=(_api_SOC_GetAckNumber_t)ptr;
    }
	return m_Ql_SOC_GetAckNumber(socketId, ackNum);
}

s32 Ql_SOC_SendTo (s32 socketId, u8* pData, s32 dataLen, u32 remoteIP, u16 remotePort)
{
	if (m_SOC_SendTo==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_SOC_SendTo");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SOC_SendTo");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_SOC_SendTo=(_api_SOC_SendTo_t)ptr;
    }
	 return m_SOC_SendTo(socketId, pData, dataLen, remoteIP, remotePort);	
}

s32 Ql_SOC_RecvFrom(s32 socketId, u8* pBuffer, s32 recvLen, u32* remoteIP, u16* remotePort)
{
	if (m_SOC_RecvFrom==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_SOC_RecvFrom");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SOC_RecvFrom");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_SOC_RecvFrom=(_api_SOC_RecvFrom_t)ptr;
    }
	  return m_SOC_RecvFrom(socketId, pBuffer, recvLen, remoteIP, remotePort);	
}

s32 Ql_SOC_Bind(s32 socketId, u16 localPort)
{
    if (m_SOC_Bind==NULL)
    {
    	s32 ptr = m_get_api_ptr("Ql_SOC_Bind");
    	if (0 == ptr)
    	{
    		//Ql_Debug_Trace(api_notsup, "Ql_SOC_Bind");		
    		 return Ql_RET_NOT_SUPPORT;
        }
    	m_SOC_Bind=(_api_SOC_Bind_t)ptr;
    }
     return m_SOC_Bind(socketId, localPort);	
}

s32 Ql_SOC_Listen(s32 listenSocketId, s32 maxClientNum)
{
 	if (m_SOC_Listen==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_SOC_Listen");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SOC_Listen");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_SOC_Listen=(_api_SOC_Listen_t)ptr;
    }
	return m_SOC_Listen(listenSocketId, maxClientNum);
}

s32 Ql_SOC_Accept(s32 listenSocketId, u32 * remoteIP, u16* remotePort)
{
	if (m_SOC_Accept==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_SOC_Accept");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SOC_Accept");		
			return -127;
        }
		m_SOC_Accept=(_api_SOC_Accept_t)ptr;
    }
	 return m_SOC_Accept(listenSocketId, remoteIP, remotePort);	
}    

s32 Ql_IpHelper_GetIPByHostName(u8 contextId, 
                                u8 requestId,
                                u8 *hostname, 
                                Callback_IpHelper_GetIpByName callback_GetIpByName)
{
	if (m_IpHelper_GetIPByHostName==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_IpHelper_GetIPByHostName");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_IpHelper_GetIPByHostName");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_IpHelper_GetIPByHostName=(_api_IpHelper_GetIPByHostName_t)ptr;
    }
	return m_IpHelper_GetIPByHostName(contextId, requestId, hostname, callback_GetIpByName);
}

s32 Ql_IpHelper_GetIPByHostNameEx(u8 contextId, 
                                  u8 requestId,
                                  u8 *hostName, 
                                  u32* ipCount,
                                  u32* ipAddress)
{
    if (NULL == m_IpHelper_GetIPByHostNameEx)
    {
        s32 ptr = m_get_api_ptr("Ql_IpHelper_GetIPByHostNameEx");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_IpHelper_GetIPByHostNameEx");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_IpHelper_GetIPByHostNameEx = (_api_IpHelper_GetIPByHostNameEx_t)ptr;
    }
    return m_IpHelper_GetIPByHostNameEx(contextId, requestId, hostName, ipCount, ipAddress);
}


s32 Ql_IpHelper_ConvertIpAddr (u8 *addrString, u32* ipAddr)
 {
	if (m_IpHelper_ConvertIpAddr==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_IpHelper_ConvertIpAddr");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_IpHelper_ConvertIpAddr");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_IpHelper_ConvertIpAddr=(_api_IpHelper_ConvertIpAddr_t)ptr;
    }
	 return m_IpHelper_ConvertIpAddr(addrString, ipAddr);	
}   



s32   Ql_SOC_SetOption(s32 socketId, s32 optionType,u32 optionParam)
{
	if (m_SOC_SetOption==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_SOC_SetOption");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SOC_SetOption");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_SOC_SetOption=(_api_SOC_SetOption_t)ptr;
    }
	 return m_SOC_SetOption(socketId, optionType,optionParam);
}    


s32 Ql_SOC_SSL_InjectCertification(u8 type, u32* certFilePath)
{
	if (m_SOC_SSL_InjectCertification==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_SOC_SSL_InjectCertification");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SOC_SSL_InjectCertification");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_SOC_SSL_InjectCertification=(_api_SOC_SSL_InjectCertification_t)ptr;
    }
	 return m_SOC_SSL_InjectCertification(type, certFilePath);
}
#endif