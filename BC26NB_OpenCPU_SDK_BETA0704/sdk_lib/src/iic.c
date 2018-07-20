#include "ql_type.h"
#include "ql_gpio.h"
#include "ql_error.h"
#include "ql_iic.h"
#include "ql_func.h"

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
<       Definition for  OpenCPU IIC APIs
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

/************************************************************************************************
*   IIC
*************************************************************************************************/
typedef s32 (*_api_IIC_Init_t)(u32 chnnlNo,Enum_PinName pinSCL,Enum_PinName pinSDA,bool IICtype);
typedef s32  (*_api_IIC_Config_t)(u32 chnnlNo, bool isHost, u8 slaveAddr, u32 IicSpeed);
//typedef s32 (*_api_IIC_SetOption_t)(u32 chnnlNo, u8 slaveAddr, Enum_IIC_OPTION optionType , u32 optionParam);
typedef s32 (* _api_IIC_Write_t)(u32 chnnlNo,u8 slaveAddr,u8 *pData,u32 len);
typedef s32 (* _api_IIC_Read_t)(u32 chnnlNo,u8 slaveAddr,u8 *pBuffer,u32 len);
typedef s32 (* _api_IIC_Write_Read_t)(u32 chnnlNo,u8 slaveAddr,u8 * pData,u32 wrtLen,u8 * pBuffer,u32 rdLen);
typedef s32 (* _api_IIC_Uninit_t)(u32 chnnlNo);



/************************************************************************************************
*   IIC
*************************************************************************************************/
_api_IIC_Init_t m_IIC_Init=NULL;

_api_IIC_Config_t m_IIC_Config=NULL;

//_api_IIC_SetOption_t m_IIC_SetOption=NULL;

_api_IIC_Write_t m_IIC_Write=NULL;

_api_IIC_Read_t m_IIC_Read=NULL;

_api_IIC_Write_Read_t m_IIC_Write_Read=NULL;

_api_IIC_Uninit_t m_IIC_Uninit=NULL;


s32 Ql_IIC_Init(u32 chnnlNo,Enum_PinName pinSCL,Enum_PinName pinSDA,bool IICtype)
{   
	if (NULL == m_IIC_Init)
	{
		s32 ptr = m_get_api_ptr("Ql_IIC_Init");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_IIC_Init");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_IIC_Init=(_api_IIC_Init_t)ptr;
    }
	return m_IIC_Init(chnnlNo,pinSCL,pinSDA,IICtype);
}

s32  Ql_IIC_Config(u32 chnnlNo, bool isHost, u8 slaveAddr, u32 IicSpeed)
{   
	if (NULL == m_IIC_Config)
	{
		s32 ptr = m_get_api_ptr("Ql_IIC_Config");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_IIC_Config");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_IIC_Config=(_api_IIC_Config_t)ptr;
    }
	return m_IIC_Config(chnnlNo,isHost,slaveAddr,IicSpeed);
}
/*
s32 Ql_IIC_SetOption(u32 chnnlNo, u8 slaveAddr, Enum_IIC_OPTION optionType , u32 optionParam)
{   
	if (NULL == m_IIC_SetOption)
	{
		s32 ptr = m_get_api_ptr("Ql_IIC_SetOption");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_IIC_SetOption");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_IIC_SetOption=(_api_IIC_SetOption_t)ptr;
    }
	return m_IIC_SetOption(chnnlNo,slaveAddr,optionType,optionParam);
}   
*/

s32 Ql_IIC_Write(u32 chnnlNo,u8 slaveAddr,u8 *pData,u32 len)
{   
	if (NULL == m_IIC_Write)
	{
		s32 ptr = m_get_api_ptr("Ql_IIC_Write");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_IIC_Write");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_IIC_Write=(_api_IIC_Write_t)ptr;
    }
	return m_IIC_Write(chnnlNo,slaveAddr,pData,len);
}


s32 Ql_IIC_Read(u32 chnnlNo,u8 slaveAddr,u8 *pBuffer,u32 len)
{   
	if (NULL == m_IIC_Read)
	{
		s32 ptr = m_get_api_ptr("Ql_IIC_Read");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_IIC_Read");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_IIC_Read=(_api_IIC_Read_t)ptr;
    }
	return m_IIC_Read(chnnlNo,slaveAddr,pBuffer,len);
}

s32 Ql_IIC_Write_Read(u32 chnnlNo,u8 slaveAddr,u8 * pData,u32 wrtLen,u8 * pBuffer,u32 rdLen)
{   
	if (NULL == m_IIC_Write_Read)
	{
		s32 ptr = m_get_api_ptr("Ql_IIC_Write_Read");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_IIC_Write_Read");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_IIC_Write_Read=(_api_IIC_Write_Read_t)ptr;
    }
	return m_IIC_Write_Read(chnnlNo,slaveAddr,pData,wrtLen,pBuffer,rdLen);
}


s32 Ql_IIC_Uninit(u32 chnnlNo)
{   
	if (NULL == m_IIC_Uninit)
	{
		s32 ptr = m_get_api_ptr("Ql_IIC_Uninit");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_IIC_Uninit");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_IIC_Uninit=(_api_IIC_Uninit_t)ptr;
    }
	return m_IIC_Uninit(chnnlNo);
}

