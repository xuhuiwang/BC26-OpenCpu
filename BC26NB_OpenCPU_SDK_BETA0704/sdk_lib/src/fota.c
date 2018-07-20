#if 0
#include "ql_ocpu_api.h"
#include "ql_func.h"

static _api_FOTA_Init_t         m_FOTA_Init         = NULL;
static _api_FOTA_WriteData_t    m_FOTA_WriteData    = NULL;
static _api_FOTA_Finish_t       m_FOTA_Finish       = NULL;
static _api_FOTA_ReadData_t     m_FOTA_ReadData     = NULL;
static _api_FOTA_Update_t       m_FOTA_Update       = NULL;



s32 Ql_FOTA_Init(ST_FotaConfig * pFotaCfg)
{
	if (m_FOTA_Init==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_FOTA_Init");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_FOTA_Init");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_FOTA_Init=(_api_FOTA_Init_t)ptr;
    }
	 return m_FOTA_Init(pFotaCfg);
}

s32  Ql_FOTA_WriteData(s32 length, s8* buffer)
{
	if (m_FOTA_WriteData==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_FOTA_WriteData");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_FOTA_WriteData");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_FOTA_WriteData=(_api_FOTA_WriteData_t)ptr;
    }
	 return m_FOTA_WriteData(length, buffer); 
}

s32 Ql_FOTA_Finish(void)
{
	if (m_FOTA_Finish==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_FOTA_Finish");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_FOTA_Finish");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_FOTA_Finish=(_api_FOTA_Finish_t)ptr;
    }
	 return m_FOTA_Finish(); 
}

s32 Ql_FOTA_ReadData(u32 offset, u32 len, u8* pBuffer)
{
    if (NULL == m_FOTA_ReadData)
    {
        s32 ptr = m_get_api_ptr("Ql_FOTA_ReadData");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FOTA_ReadData");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FOTA_ReadData = (_api_FOTA_ReadData_t)ptr;
    }
    return m_FOTA_ReadData(offset, len, pBuffer); 
}

s32 Ql_FOTA_Update(void)
{
	if (m_FOTA_Update==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_FOTA_Update");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_FOTA_Update");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_FOTA_Update=(_api_FOTA_Update_t)ptr;
    }
	 return m_FOTA_Update(); 
}
#endif

