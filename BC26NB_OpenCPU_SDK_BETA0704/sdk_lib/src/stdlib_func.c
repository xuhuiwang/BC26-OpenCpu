#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_error.h "

static _api_itoa_t      m_itoa      = NULL;
static _api_atof_t      m_atof      = NULL;
static _api_ui64toa_t   m_ui64toa   = NULL;
static _api_strcpy_t    m_strcpy    = NULL;
static _api_strncpy_t   m_strncpy   = NULL;
static _api_strcat_t    m_strcat    = NULL;
static _api_strncat_t   m_strncat   = NULL;
static _api_strlen_t    m_strlen    = NULL;
static _api_strcmp_t    m_strcmp    = NULL;
static _api_strncmp_t   m_strncmp   = NULL;
static _api_memset_t    m_memset    = NULL;
static _api_memcpy_t    m_memcpy    = NULL;
static _api_memcmp_t    m_memcmp    = NULL;
static _api_memmove_t   m_memmove   = NULL;
static _api_strstr_t    m_strstr    = NULL;
static _api_strchr_t    m_strchr    = NULL;

s32  Ql_create_api_hash(char *str)
{
	if (m_create_api_hash != NULL)
	{
		return m_create_api_hash(str);
	}else{
		//Ql_Debug_Trace(api_notsup, "Ql_create_api_hash");
		return -1;
	}
}

void Ql_ui64toa(char* buf, u64 i, s32 base)
{
	if (m_ui64toa == NULL)
	{
		char funcName[] = "Ql_ui64toa";
		s32 ptr = m_get_api_ptr(funcName);
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, funcName);		
			return;
		}
        m_ui64toa = (_api_ui64toa_t)ptr;
	}
	m_ui64toa(buf, i, base);
}

void  Ql_itoa(char *buf, s32 i, s32 base)
{
	if (m_itoa == NULL)
	{
		char funcName[] = "Ql_itoa";
		s32 ptr = m_get_api_ptr(funcName);
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, funcName);		
			return;
		}
		m_itoa =(_api_itoa_t)ptr;
	}
	m_itoa(buf, i, base);
}

double Ql_atof(const char* s)
{
	if (m_atof == NULL)
	{
		char funcName[] = "Ql_atof";
		s32 ptr = m_get_api_ptr(funcName);
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, funcName);		
			return 0;
        }
		m_atof =(_api_atof_t)ptr;
    }
	return m_atof(s);		
}
char* Ql_strcpy(char* dest, const char* src)
{
	if (m_strcpy == NULL)
	{
		char funcName[] = "Ql_strcpy";
		s32 ptr = m_get_api_ptr(funcName);
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, funcName);		
			return NULL;
        }
		m_strcpy =(_api_strcpy_t)ptr;
    }
	return m_strcpy(dest, src);	
}
char* Ql_strncpy(char* dest, const char* src, u32 size)
{
	if (m_strncpy ==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_strncpy");
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, "Ql_strncpy");		
			return NULL;
        }
		m_strncpy =(_api_strncpy_t)ptr;
    }
	return m_strncpy(dest, src, size);	
}
char* Ql_strcat(char* s1, const char* s2)
{
	if (m_strcat == NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_strcat");
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, "Ql_strcat");		
			return NULL;
        }
		m_strcat =(_api_strcat_t)ptr;
    }
	return m_strcat(s1, s2);		
}
char* Ql_strncat(char* s1, const char* s2, u32 size)
{
	if (m_strncat == NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_strncat");
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, "Ql_strncat");
			return NULL;
        }
		m_strncat =(_api_strncat_t)ptr;
    }
	return m_strncat(s1, s2, size);	
}
u32   Ql_strlen(const char* str)
{
	if (m_strlen == NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_strlen");
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, "Ql_strlen");		
			return 0;
        }
		m_strlen =(_api_strlen_t)ptr;
    }
	return m_strlen(str);	
}
s32   Ql_strcmp(const char*s1, const char*s2)
{
	if (m_strcmp == NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_strcmp");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_strcmp");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_strcmp =(_api_strcmp_t)ptr;
    }
	return m_strcmp(s1, s2);	
}
s32   Ql_strncmp(const char* s1, const char* s2, u32 size)
{
	if (m_strncmp == NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_strncmp");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_strncmp");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_strncmp =(_api_strncmp_t)ptr;
    }
	return m_strncmp(s1, s2, size);
}

void* Ql_memset(void* dest, u8 value, u32 size)
{
	if (m_memset == NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_memset");
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, "Ql_memset");		
			return NULL;
        }
		m_memset =(_api_memset_t)ptr;
    }
	return m_memset(dest, value, size);
}

void* Ql_memcpy(void* dest, const void* src, u32 size)
{
	if (m_memcpy==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_memcpy");
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, "Ql_memcpy");		
			return NULL;
        }
		m_memcpy=(_api_memcpy_t)ptr;
    }
	return m_memcpy(dest, src, size);
}

s32   Ql_memcmp(const void* dest, const void*src, u32 size)
{
	if (m_memcmp==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_memcmp");
		if (0 == ptr)
		{
			//Ql_Debug_Trace(api_notsup, "Ql_memcmp");		
			return Ql_RET_NOT_SUPPORT;
        }
		m_memcmp=(_api_memcmp_t)ptr;
    }
	return m_memcmp(dest, src, size);
}
void* Ql_memmove(void* dest, const void* src, u32 size)
{
	if (m_memmove==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_memmove");
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, "Ql_memmove");		
			return NULL;
        }
		m_memmove=(_api_memmove_t)ptr;
    }
	return m_memmove(dest, src, size);
}
char* Ql_strstr(const char* s1, const char* s2)
{
	if (m_strstr==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_strstr");
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, "Ql_strstr");
			return NULL;
        }
		m_strstr=(_api_strstr_t)ptr;
    }
	return m_strstr(s1, s2);
}
char* Ql_strchr(const char* s1, s32 ch)
{
	if (m_strchr==NULL)
	{
		s32 ptr = m_get_api_ptr("Ql_strchr");
		if (0 == ptr)
		{
			Ql_Debug_Trace(api_notsup, "Ql_strchr");
			return NULL;
        }
		m_strchr=(_api_strchr_t)ptr;
    }
	return m_strchr(s1, ch);
}
/*
int   Ql_sprintf(char * buffer, const char * format, ...)
{
	if (m_sprintf != NULL)
	{
		return m_sprintf(buffer, format, ...);
	}else{
		Ql_Debug_Trace(api_notsup, "Ql_sprintf");
		return Ql_RET_NOT_SUPPORT;
	}
}
*/
s32   Ql_toupper(s32 c)
{
    return (c -'a'+'A');
}
s32   Ql_tolower(s32 c)
{
    return (c -'A'+'a');
}
s32   Ql_isdigit(char c)
{
    if ((c >= '0') && (c <= '9'))
    {
        return 1;
    }else{
        return 0;
    }
}

static bool ocpu_isspace(int c)
{
    if(c==' '||c=='\t'||c=='\n'||c=='\f'||c=='\b'||c=='\r')
        return 1;
    else 
        return 0;
}
s32 Ql_atoi(const char *s)
{
    int c;              /* current char */
    int total;         /* current total */
    int sign;           /* if '-', then negative, otherwise positive */

    /* skip whitespace */
    while ( ocpu_isspace((int)(unsigned char)*s) )
        ++s;

    c = (int)(unsigned char)*s++;
    sign = c;           /* save sign indication */
    if (c == '-' || c == '+')
        c = (int)(unsigned char)*s++;    /* skip sign */

    total = 0;

    while (Ql_isdigit(c)) {
        total = 10 * total + (c - '0');     /* accumulate digit */
        c = (int)(unsigned char)*s++;    /* get next char */
    }

    if (sign == '-')
        return -total;
    else
        return total;   /* return result, negated if necessary */
}
