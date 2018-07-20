#include "ql_type.h"
#include "ql_gpio.h"
#include "ql_error.h"
#include "ql_spi.h"
#include "ql_func.h"

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
<       Definition for  OpenCPU SPI APIs
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/


/************************************************************************************************
*   SPI
*************************************************************************************************/
typedef s32 (* _api_SPI_Init_t)(u32 chnnlNo,Enum_PinName pinClk,Enum_PinName pinMiso,Enum_PinName pinMosi,Enum_PinName pinCs,bool spiType);
typedef s32 (* _api_SPI_Config_t)(u32 chnnlNo,bool isHost, bool cpol, bool cpha, u32 clkSpeed);
typedef s32 (* _api_SPI_Write_t)(u32 chnnlNo,u8 * pData,u32 len);
typedef s32 (* _api_SPI_WriteRead_t)(u32 chnnlNo,u8 *pData,u32 wrtLen,u8 * pBuffer,u32 rdLen);
typedef s32 (* _api_SPI_Uninit_t)(u32 chnnlNo);


/************************************************************************************************
*   SPI
*************************************************************************************************/
_api_SPI_Init_t m_SPI_Init=NULL;

_api_SPI_Config_t m_SPI_Config=NULL;

_api_SPI_Write_t m_SPI_Write=NULL;

_api_SPI_WriteRead_t m_SPI_WriteRead=NULL;

_api_SPI_Uninit_t m_SPI_Uninit=NULL;


s32 Ql_SPI_Init(u32 chnnlNo, Enum_PinName pinClk, Enum_PinName pinMiso, Enum_PinName pinMosi, Enum_PinName pinCs, bool spiType)
{   
	if (NULL == m_SPI_Init)
	{
		s32 ptr = m_get_api_ptr("Ql_SPI_Init");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SPI_Init");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_SPI_Init=(_api_SPI_Init_t)ptr;
    }
	return m_SPI_Init(chnnlNo,pinClk,pinMiso,pinMosi,pinCs,spiType);
}

s32 Ql_SPI_Config(u32 chnnlNo,bool isHost, bool cpol, bool cpha, u32 clkSpeed)
{
	if (NULL == m_SPI_Config)
	{
		s32 ptr = m_get_api_ptr("Ql_SPI_Config");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SPI_Config");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_SPI_Config=(_api_SPI_Config_t)ptr;
    }
	return m_SPI_Config(chnnlNo,isHost,cpol,cpha,clkSpeed);
}

s32 Ql_SPI_Write(u32 chnnlNo,u8 * pData,u32 len)
{   
	if (NULL == m_SPI_Write)
	{
		s32 ptr = m_get_api_ptr("Ql_SPI_Write");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SPI_Write");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_SPI_Write=(_api_SPI_Write_t)ptr;
    }
	return m_SPI_Write(chnnlNo,pData,len);
}


s32 Ql_SPI_WriteRead(u32 chnnlNo,u8 *pData,u32 wrtLen,u8 * pBuffer,u32 rdLen)
{   
	if (NULL == m_SPI_WriteRead)
	{
		s32 ptr = m_get_api_ptr("Ql_SPI_WriteRead");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SPI_WriteRead");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_SPI_WriteRead=(_api_SPI_WriteRead_t)ptr;
    }
	return m_SPI_WriteRead(chnnlNo,pData,wrtLen,pBuffer,rdLen);
}



s32 Ql_SPI_Uninit(u32 chnnlNo)
{   
	if (NULL == m_SPI_Uninit)
	{
		s32 ptr = m_get_api_ptr("Ql_SPI_Uninit");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_SPI_Uninit");	
			return Ql_RET_NOT_SUPPORT;
        }
		m_SPI_Uninit=(_api_SPI_Uninit_t)ptr;
    }
	return m_SPI_Uninit(chnnlNo);
}



