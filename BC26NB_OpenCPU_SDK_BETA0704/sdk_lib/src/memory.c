#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_memory.h"

static _api_MEM_Alloc_t              m_MEM_Alloc = NULL;
static _api_MEM_Free_t               m_MEM_Free = NULL;

void* Ql_MEM_Alloc(u32 size)
{
	if (m_MEM_Alloc==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_MEM_Alloc");
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, "Ql_MEM_Alloc");		
			return NULL;
        }
		m_MEM_Alloc=(_api_MEM_Alloc_t)ptr;
    }
	return m_MEM_Alloc(size);
}

void Ql_MEM_Free(void *ptr)
{
	if (m_MEM_Free==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_MEM_Free");
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, "Ql_MEM_Free");		
			return;
        }
		m_MEM_Free=(_api_MEM_Free_t)ptr;
    }
	m_MEM_Free(ptr);
}

