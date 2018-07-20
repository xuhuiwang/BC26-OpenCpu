#if 0
#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_fs.h"

static _api_FS_Open_t               m_FS_Open = NULL;
static _api_FS_OpenRAMFile_t        m_FS_OpenRAMFile = NULL;
static _api_FS_Read_t               m_FS_Read = NULL;
static _api_FS_Write_t              m_FS_Write = NULL;
static _api_FS_Seek_t               m_FS_Seek = NULL;
static _api_FS_GetFilePosition_t    m_FS_GetFilePosition = NULL;
static _api_FS_Truncate_t           m_FS_Truncate = NULL;
static _api_FS_Flush_t              m_FS_Flush = NULL;
static _api_FS_Close_t              m_FS_Close = NULL;
static _api_FS_GetSize_t            m_FS_GetSize = NULL;
static _api_FS_Delete_t             m_FS_Delete = NULL;
static _api_FS_Check_t              m_FS_Check = NULL;
static _api_FS_Rename_t             m_FS_Rename = NULL;
static _api_FS_CreateDir_t          m_FS_CreateDir = NULL;
static _api_FS_DeleteDir_t          m_FS_DeleteDir = NULL;
static _api_FS_CheckDir_t           m_FS_CheckDir = NULL;
static _api_FS_FindFirst_t          m_FS_FindFirst = NULL;
static _api_FS_FindNext_t           m_FS_FindNext = NULL;
static _api_FS_FindClose_t          m_FS_FindClose = NULL;
static _api_FS_XDelete_t            m_FS_XDelete = NULL;
static _api_FS_XMove_t              m_FS_XMove = NULL;
static _api_FS_GetFreeSpace_t       m_FS_GetFreeSpace = NULL;
static _api_FS_GetTotalSpace_t      m_FS_GetTotalSpace = NULL;
static _api_FS_Format_t             m_FS_Format = NULL;



s32 Ql_FS_Open(char* lpFileName, u32 flag)
{
    if (NULL == m_FS_Open)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_Open");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_Open");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_Open = (_api_FS_Open_t)ptr;
    }
    return m_FS_Open(lpFileName, flag);
}

s32 Ql_FS_OpenRAMFile(char *lpFileName, u32 flag, u32 ramFileSize)
{
    if (NULL == m_FS_OpenRAMFile)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_OpenRAMFile");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_OpenRAMFile");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_OpenRAMFile = (_api_FS_OpenRAMFile_t)ptr;
    }
    return m_FS_OpenRAMFile(lpFileName, flag, ramFileSize);
}

s32 Ql_FS_Read(s32 fileHandle, u8 *readBuffer, u32 numberOfBytesToRead, u32 *numberOfBytesRead)
{
    if (NULL == m_FS_Read)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_Read");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_Read");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_Read = (_api_FS_Read_t)ptr;
    }
    return m_FS_Read(fileHandle, readBuffer, numberOfBytesToRead, numberOfBytesRead);
}

s32 Ql_FS_Write(s32 fileHandle, u8 *writeBuffer, u32 numberOfBytesToWrite, u32 *numberOfBytesWritten)
{
    if (NULL == m_FS_Write)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_Write");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_Write");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_Write = (_api_FS_Write_t)ptr;
    }
    return m_FS_Write(fileHandle, writeBuffer, numberOfBytesToWrite, numberOfBytesWritten);
}

s32 Ql_FS_Seek(s32 fileHandle, s32 offset, u32 whence)
{
    if (NULL == m_FS_Seek)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_Seek");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_Seek");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_Seek = (_api_FS_Seek_t)ptr;
    }
    return m_FS_Seek(fileHandle, offset, whence);
}

s32 Ql_FS_GetFilePosition(s32 fileHandle)
{
    if (NULL == m_FS_GetFilePosition)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_GetFilePosition");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_GetFilePosition");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_GetFilePosition = (_api_FS_GetFilePosition_t)ptr;
    }
    return m_FS_GetFilePosition(fileHandle);
}

s32 Ql_FS_Truncate(s32 fileHandle)
{
    if (NULL == m_FS_Truncate)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_Truncate");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_Truncate");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_Truncate = (_api_FS_Truncate_t)ptr;
    }
    return m_FS_Truncate(fileHandle);
}

void Ql_FS_Flush(s32 fileHandle)
{
    if (NULL == m_FS_Flush)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_Flush");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_Flush");		
            return;
        }
        m_FS_Flush = (_api_FS_Flush_t)ptr;
    }
    m_FS_Flush(fileHandle);
}

void Ql_FS_Close(s32 fileHandle)
{
    if (NULL == m_FS_Close)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_Close");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_Close");		
            return;
        }
        m_FS_Close = (_api_FS_Close_t)ptr;
    }
    m_FS_Close(fileHandle);
}

s32 Ql_FS_GetSize(char *lpFileName)
{
    if (NULL == m_FS_GetSize)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_GetSize");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_GetSize");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_GetSize = (_api_FS_GetSize_t)ptr;
    }
    return m_FS_GetSize(lpFileName);
}

s32 Ql_FS_Delete(char *lpFileName)
{
    if (NULL == m_FS_Delete)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_Delete");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_Delete");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_Delete = (_api_FS_Delete_t)ptr;
    }
    return m_FS_Delete(lpFileName);
}

s32 Ql_FS_Check(char *lpFileName)
{
    if (NULL == m_FS_Check)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_Check");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_Check");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_Check = (_api_FS_Check_t)ptr;
    }
    return m_FS_Check(lpFileName);
}

s32 Ql_FS_Rename(char *lpFileName, char *newLpFileName)
{
    if (NULL == m_FS_Rename)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_Rename");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_Rename");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_Rename = (_api_FS_Rename_t)ptr;
    }
    return m_FS_Rename(lpFileName, newLpFileName); 
}

s32 Ql_FS_CreateDir(char *lpDirName)
{
    if (NULL == m_FS_CreateDir)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_CreateDir");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_CreateDir");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_CreateDir = (_api_FS_CreateDir_t)ptr;
    }
    return m_FS_CreateDir(lpDirName);
}

s32 Ql_FS_DeleteDir(char *lpDirName)
{
    if (NULL == m_FS_DeleteDir)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_DeleteDir");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_DeleteDir");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_DeleteDir = (_api_FS_DeleteDir_t)ptr;
    }
    return m_FS_DeleteDir(lpDirName);
}

s32 Ql_FS_CheckDir(char *lpDirName)
{
    if (NULL == m_FS_CheckDir)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_CheckDir");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_CheckDir");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_CheckDir = (_api_FS_CheckDir_t)ptr;
    }
    return m_FS_CheckDir(lpDirName);
}

s32 Ql_FS_FindFirst(char *lpPath, char *lpFileName, u32 fileNameLength, u32 *fileSize, bool *isDir)
{
    if (NULL == m_FS_FindFirst)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_FindFirst");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_FindFirst");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_FindFirst = (_api_FS_FindFirst_t)ptr;
    }
    return m_FS_FindFirst(lpPath, lpFileName, fileNameLength, fileSize, isDir);
}

s32 Ql_FS_FindNext(s32 handle, char *lpFileName, u32 fileNameLength, u32 *fileSize, bool *isDir)
{
    if (NULL == m_FS_FindNext)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_FindNext");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_FindNext");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_FindNext = (_api_FS_FindNext_t)ptr;
    }
    return m_FS_FindNext(handle, lpFileName, fileNameLength, fileSize, isDir);
}

void Ql_FS_FindClose(s32 handle)
{    
    if (NULL == m_FS_FindClose)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_FindClose");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_FindClose");		
            return;
        }
        m_FS_FindClose = (_api_FS_FindClose_t)ptr;
    }
    m_FS_FindClose(handle);
}

s32  Ql_FS_XDelete(char* lpPath, u32 flag)
{    
    if (NULL == m_FS_XDelete)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_XDelete");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_XDelete");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_XDelete = (_api_FS_XDelete_t)ptr;
    }
    return m_FS_XDelete(lpPath, flag);
}

s32  Ql_FS_XMove(char* lpSrcPath, char* lpDestPath, u32 flag)
{
    if (NULL == m_FS_XMove)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_XMove");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_XMove");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_XMove = (_api_FS_XMove_t)ptr;
    }
    return m_FS_XMove(lpSrcPath, lpDestPath, flag);
}

s64  Ql_FS_GetFreeSpace(u32 storage)
{
    if (NULL == m_FS_GetFreeSpace)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_GetFreeSpace");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_GetFreeSpace");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_GetFreeSpace = (_api_FS_GetFreeSpace_t)ptr;
    }
    return m_FS_GetFreeSpace(storage);
}

s64  Ql_FS_GetTotalSpace(u32 storage)
{
    if (NULL == m_FS_GetTotalSpace)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_GetTotalSpace");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_GetTotalSpace");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_GetTotalSpace = (_api_FS_GetTotalSpace_t)ptr;
    }
    return m_FS_GetTotalSpace(storage);
}

s32 Ql_FS_Format(u8 storage)
{
    if (NULL == m_FS_Format)
    {
        s32 ptr = m_get_api_ptr("Ql_FS_Format");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_FS_Format");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_FS_Format = (_api_FS_Format_t)ptr;
    }
    return m_FS_Format(storage);
}
#endif