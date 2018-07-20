#ifndef __QL_OCPU_API_H__
#define __QL_OCPU_API_H__
#include "ql_type.h"
#include "ql_common.h"
//#include "ql_audio.h"
#include "ql_uart.h"
//#include "ql_fs.h"
//#include "ql_fota.h"
#include "ql_system.h"
#include "ql_memory.h"
#include "ql_gpio.h"
#include "ql_timer.h"
#include "ql_trace.h"
#include "ql_api_type.h"
//#include "ql_tts.h"
#include "ql_power.h"
//#include "ql_wtd.h"
#include "ql_time.h"
#include "ql_error.h"
//#include "ql_socket.h"
//#include "ql_gprs.h"
#include "ril.h"
//#include "vadefs.h"
#include "ql_rtc.h"
/************************************************************************************************
*   Common Type
*************************************************************************************************/
typedef void (*OCPU_API_voidvoid)(void);
typedef u32  (*OCPU_API_u32void)(void);
typedef s32  (*OCPU_API_s32void)(void);
typedef u64  (*OCPU_API_u64void)(void);

typedef s32 (*OCPU_API_DebugTrace)(char* fmt, ... );
typedef s32 (*OCPU_API_sprintf)(char *, const char *, ...);
typedef s32 (*OCPU_API_snprintf)(char*, u32, const char*, ...);
typedef s32 (*OCPU_API_sscanf)(const char*, const char*, ...);

/************************************************************************************************
*   Utility
*************************************************************************************************/
typedef s32 (*_api_ConvertCodec_t)(u8 srcCode, u8* src_p, u8 dstCode, u8* dst_p);


/************************************************************************************************
*   System
*************************************************************************************************/
typedef void (*_api_Sleep_t)(u32 msec);
typedef void (*_api_Reset_t)(u8 resettype);
typedef void (*_api_PowerDown_t)(u8 powertype);
typedef s32 (*_api_WTD_Start_t)(u32 interval, u32 allowedOverCount, s32 resetType);
typedef void (*_api_LockPower_t)(void);
typedef void (*_api_WTD_Feed_t)(void);
typedef void (*_api_WTD_Stop_t)(void);
typedef s32  (*_api_GetPowerOnReason_t)(void);
typedef s32  (*_api_SetSleepMode_t)(u8 mod);
typedef s32  (*_api_SleepEnable_t)(void);
typedef s32  (*_api_SleepDisable_t)(void);
typedef s32  (*_api_GetPowerSupply_t)(u8* charge_stat, u32* capacity, u32* voltage);
//typedef s32  (*_api_VoltageInd_Register_t)(Callback_PowerVol_Ind volInd_callback);
//typedef s32  (*_api_PwrKey_Register_t)(Callback_PowerKey_Ind pwrKey_callback); // 20120509 Ramos.Zhang
typedef s32  (*_api_GenericURC_Init_t)(OCPU_CB_GENERIC_URC urc_callback);
//typedef s32 (*_api_ChangeUsbMode_t)(Enum_UsbMode usb_mod);
typedef s32  (*_api_RIL_request_custom_command_t)(char *cmd_string, Callback_ATResponse callback, void *user_data);


/************************************************************************************************
*   Network
*************************************************************************************************/
//typedef s32 (*_api_NW_GetCardState_t)(void);
//typedef s32 (*_api_NW_GetNetworkState_t)(s32* creg, s32* cgreg);
//typedef s32 (*_api_NW_GetSignalQuality_t)(u32* rssi, u32* ber);

/************************************************************************************************
*   Memory
*************************************************************************************************/
typedef void*(*_api_MEM_Alloc_t)(u32 size);
typedef void (*_api_MEM_Free_t)(void *ptr);

/************************************************************************************************
*   File System
*************************************************************************************************/
typedef s32 (*_api_FS_Open_t)(char* lpFileName, u32 flag);
typedef s32 (*_api_FS_OpenRAMFile_t)(char *lpFileName, u32 flag, u32 ramFileSize);
typedef s32 (*_api_FS_Read_t)(s32 fileHandle, u8 *readBuffer, u32 numberOfBytesToRead, u32 *numberOfBytesRead);
typedef s32 (*_api_FS_Write_t)(s32 fileHandle, u8 *writeBuffer, u32 numberOfBytesToWrite, u32 *numberOfBytesWritten);
typedef s32 (*_api_FS_Seek_t)(s32 fileHandle, s32 offset, u32 whence);
typedef s32 (*_api_FS_GetFilePosition_t)(s32 fileHandle);
typedef s32 (*_api_FS_Truncate_t)(s32 fileHandle);
typedef void (*_api_FS_Flush_t)(s32 fileHandle);
typedef void (*_api_FS_Close_t)(s32 fileHandle);
typedef s32 (*_api_FS_GetSize_t)(char *lpFileName);
typedef s32 (*_api_FS_Delete_t)(char *lpFileName);
typedef s32 (*_api_FS_Check_t)(char *lpFileName);
typedef s32 (*_api_FS_Rename_t)(char *lpFileName, char *newLpFileName);
typedef s32 (*_api_FS_CreateDir_t)(char *lpDirName);
typedef s32 (*_api_FS_DeleteDir_t)(char *lpDirName);
typedef s32 (*_api_FS_CheckDir_t)(char *lpDirName);
typedef s32 (*_api_FS_FindFirst_t)(char *lpPath, char *lpFileName, u32 fileNameLength, u32 *fileSize, bool *isDir);
typedef s32 (*_api_FS_FindNext_t)(s32 handle, char *lpFileName, u32 fileNameLength, u32 *fileSize, bool *isDir);
typedef void (*_api_FS_FindClose_t)(s32 handle);
typedef s32  (*_api_FS_XDelete_t)(char* lpPath, u32 flag);
typedef s32  (*_api_FS_XMove_t)(char* lpSrcPath, char* lpDestPath, u32 flag);
typedef s64  (*_api_FS_GetFreeSpace_t)(u32 storage);
typedef s64  (*_api_FS_GetTotalSpace_t)(u32 storage);
typedef s32 (*_api_FS_Format_t)(u8 storage);

/************************************************************************************************
*   Flash
*************************************************************************************************/
//typedef s32 (*_api_FlashWriteData_t)(u16 ID, u16 len, u8* data);
//typedef s32 (*_api_FlashReadData_t)(u16 ID, u16 len, u8* data);
//typedef s32 (*_api_FlashGetLen_t)(u16 ID, u16* len);
//typedef s32 (*_api_FlashGetFreeSize)(void);
//typedef s32 (*_api_FlashGetUseSize)(void);
//typedef s32 (*_api_FlashCleanAll)(void);



/************************************************************************************************
*   Keyboard
*************************************************************************************************/
#ifdef __OCPU_KEYBOARD_ENABLE__
typedef s32 (*_api_KB_Register_t)(Callback_Keyboard  callback_kb);
typedef s32 (*_api_KB_Config_t)(s32 debounce, s32 reserved);
typedef s32 (*_api_KB_Init_t)(Enum_PinName kbPinName[], u32 count);
typedef s32 (*_api_KB_Uninit_t)(Enum_PinName kbPinName[], u32 count);
#endif

/************************************************************************************************
*   Audio
*************************************************************************************************/
typedef s32 (*_api_StartPlayAudioStream_t)(u8* stream, u32 streamsize, s32 streamformat, bool repeat, u8 volume, u8 audiopath);
typedef s32 (*_api_StopPlayAudioStream_t)(void);

/************************************************************************************************
*   Timer
*************************************************************************************************/
typedef s32  (*_api_SetLocalTime_t)(ST_Time* datetime);
typedef ST_Time* (*_api_GetLocalTime_t)(ST_Time* datetime);
typedef u64   (*_api_MKTime_t)(ST_Time* datetime);
typedef ST_Time*   (*_api_MKTime2CalendarTime_t)(u64 seconds, ST_Time* pOutDateTime);
typedef bool  (*_api_CalendarTime2LocalTime_t)(u32 seconds, ST_Time *pSysTime, u32 baseyear);

/************************************************************************************************
*   Timer
*************************************************************************************************/
typedef s32   (*_api_Timer_Register_t)(u32 timerId, Callback_Timer_OnTimer callback_onTimer, void* param);
typedef s32   (*_api_Timer_RegisterFast_t)(u32 timerId, Callback_Timer_OnTimer callback_onTimer, void* param);
typedef u32   (*_api_Timer_Start_t)(u32 timerId, u32 interval, bool autoRepeat);
typedef s16   (*_api_Timer_Stop_t)(u32 timerId);
typedef u32   (*_api_GetRelativeTime_Counter_t)(void);
typedef ticks (*_api_SecondToTicks_t)(u32 seconds);
typedef ticks (*_api_MillisecondToTicks_t)(u32 milliseconds);
typedef bool  (*_api_LocalTime_t)(ST_Time * datetime);
typedef ticks (*_api_ToTicks_t)(u32 s_or_ms);

/************************************************************************************************
*   RTC
*************************************************************************************************/
typedef s32   (*_api_Rtc_RegisterFast_t)(u32 rtcId, Callback_Rtc_Func callback_onRtc, void* param);
typedef u32   (*_api_Rtc_Start_t)(u32 rtcId, u32 interval, bool autoRepeat);
typedef s16   (*_api_Rtc_Stop_t)(u32 rtcId);

/************************************************************************************************
*   FCM
*************************************************************************************************/
typedef s32 (*_api_Ql_UART_Register_t)(Enum_SerialPort port, CallBack_UART_Notify fp,void * customizePara);
typedef s32 (*_api_Ql_UART_Open_t)(Enum_SerialPort port,u32 baud, Enum_FlowCtrl  flowCtrl);
typedef s32(*_api_Ql_UART_OpenEx_t)(Enum_SerialPort port, ST_UARTDCB *dcb);
typedef s32 (*_api_Ql_UART_Write_t)(Enum_SerialPort port, u8* data, u32 writeLen);
typedef s32 (*_api_Ql_UART_Read_t)(Enum_SerialPort port, u8* data, u32 readLen);
typedef void (*_api_Ql_UART_Close_t)(Enum_SerialPort port);



/************************************************************************************************
*   TCP/IP
*************************************************************************************************/
typedef s32 (*_api_GPRS_GetPDPCntxtState_t)(u8 contextId);
//typedef s32 (*_api_GPRS_Config_t)(u8 contextId, ST_GprsConfig* cfg);
//typedef s32 (*_api_GPRS_Register_t)(u8 contextId,  ST_PDPContxt_Callback * callback_func, void* customParam);
typedef s32 (*_api_GPRS_Activate_t)(u8 contxtid);
typedef s32 (*_api_GPRS_ActivateEx_t)(u8 contxtid, bool isBlocking);
typedef s32  (*_api_GPRS_Deactivate_t)(u8 contxtid);
typedef s32  (*_api_GPRS_DeactivateEx_t)(u8 contxtid, bool isBlocking);
typedef s32 (*_api_GPRS_GetLocalIPAddress_t)(u8 contxtId, u32* ipAddr);
typedef s32 (*_api_GPRS_GetDNSAddress_t)(u8 contextId, u32* primaryAddr, u32* bkAddr);
typedef s32 (*_api_GPRS_SetDNSAddress_t)(u8 contextId, u32 primaryAddr, u32 bkAddr);

//typedef s32 (*_api_SOC_Register_t)(ST_SOC_Callback cb, void* customParam);
typedef s32 (*_api_SOC_Create_t)(u8 contextId, u8 socketType);
//typedef s8 (*_api_SOC_CreateEx_t)(u8 contextId, u8 socketType,s32 taskId, ST_SOC_Callback cb);
typedef s32 (*_api_Ql_SOC_Close_t)(s32 socketId);
typedef s32 (*_api_SOC_Connect_t)(s32 socketId, u32 remoteIP, u16 remotePort);
typedef s32 (*_api_SOC_ConnectEx_t)(s32 socketId, u32 remoteIP, u16 remotePort, bool isBlocking);
typedef s32 (*_api_SOC_Send_t)(s32 socketId, u8* pData, s32 dataLen);
typedef s32 (*_api_Ql_SOC_Recv_t)(s32 socketId, u8* pData, s32 dataLen);
typedef s32 (*_api_SOC_GetAckNumber_t)(s32 socketId, u64* ackNum);
typedef s32 (*_api_SOC_SendTo_t)(s32 socketId, u8* pData, s32 dataLen, u32 remoteIP, u16 remotePort);
typedef s32 (*_api_SOC_RecvFrom_t)(s32 socketId, u8* pData, s32 recvLen, u32* remoteIP, u16* remotePort);
typedef s32 (*_api_SOC_Bind_t)(s32 socketId, u16 localPort);
typedef s32 (*_api_SOC_Listen_t)(s32 listenSocketId, s32 maxClientNum);
typedef s8 (*_api_SOC_Accept_t)(s32 listenSocketId, u32 * remoteIP, u16* remotePort);
//typedef s32 (*_api_IpHelper_GetIPByHostName_t)( u8 contextId, 
///                                                u8 requestId,
//                                                u8 *hostname, 
 //                                               Callback_IpHelper_GetIpByName callback_getIpByName);
typedef s32 (*_api_IpHelper_GetIPByHostNameEx_t)(u8 contextId, 
                                                 u8 requestId,
                                                 u8 *hostName, 
                                                 u32* ipCount,
                                                 u32* ipAddress);

typedef s32 (*_api_IpHelper_ConvertIpAddr_t)(u8 *addressstring, u32* ipaddr);


typedef s32 (*_api_SOC_SetOption_t)(s32 socketId, s32 optionType,u32 optionParam);

typedef s32(*_api_SOC_SSL_InjectCertification_t)(u8 type, u32* certFilePath);

typedef s32 (*_api_SocketSetRecvBufferSize_t)(s8 socket, u32 bufferSize);
typedef void (*_api_GetDeviceCurrentRunState_t)(s32 *simcard, s32 *creg, s32 *cgreg, u8 *rssi, u8 *ber);
typedef u32 (*_api_SocketHtonl_t)(u32 a);
typedef u16 (*_api_SocketHtons_t)(u16 a);
typedef s32 (*_api_SwitchHeartBeat_t)(u8 enable, s16 cycle_time, s16 repeat_time);

/************************************************************************************************
*   Multitasking
*************************************************************************************************/
typedef s32 (*_api_OS_GetMessage_t)(ST_MSG* msg);
typedef s32 (*_api_OS_SendMessage_t)(s32 destTaskId, u32 msgId, u32 param1, u32 param2);
typedef u32 (*_api_OS_CreateMutex_t)(void);
typedef void (*_api_OS_TakeMutex_t)(u32 mutexId,u32 block_time);
typedef void (*_api_OS_GiveMutex_t)(u32 mutexId);
typedef u32 (*_api_OS_CreateSemaphore_t)(u32 maxCount,u32 InitialCount);
typedef u32 (*_api_OS_TakeSemaphore_t)(u32 mutexId, u32 block_time);
typedef void (*_api_OS_GiveSemaphore_t)(u32 mutexId);
typedef s32 (*_api_SetLastErrorCode_t)(s32 errCode);
typedef s32 (*_api_GetLastErrorCode_t)(void);
typedef u32 (*_api_OS_GetCurrentTaskPriority_t)(void);
typedef u32 (*_api_OS_GetCurrenTaskLeftStackSize_t)(void);
typedef u32 (*_api_osChangeTaskPriority_t)(s32 desttaskid, u32 newpriority);

typedef void (*_api_osMutex_t)(u32 mutexid);

typedef u32 (*_api_OS_CreateEvent_t)(void);
typedef s32 (*_api_OS_WaitEvent_t)(u32 evtId, u32 evtFlag,u32 block_time);
typedef s32 (*_api_OS_SetEvent_t)(u32 evtId, u32 evtFlag);
typedef s32 (*_api_OS_GetActiveTaskId_t)(void);

/************************************************************************************************
*   FOTA
*************************************************************************************************/
//typedef s32 (*_api_FOTA_Init_t)(ST_FotaConfig * pFotaCfg);
typedef s32 (*_api_FOTA_WriteData_t)(s32 length, s8* buffer);
typedef s32 (*_api_FOTA_Finish_t)(void);  
typedef s32 (*_api_FOTA_ReadData_t)(u32 offset, u32 len, u8* pBuffer);  
typedef s32 (*_api_FOTA_Update_t)(void);

/************************************************************************************************
*   TTS
*************************************************************************************************/
//typedef s32 (*_api_TTS_Initialize_t)(Callback_TTS_Play callback_play);
typedef s32 (*_api_TTS_Play_t)(u8* content, u8 len);
typedef s32 (*_api_TTS_PlayInCall_t)(u8* content, u8 len, u8 upLinkVolume, u8 downLinkVolume);
typedef s32 (*_api_TTS_Stop_t)(void);
typedef s32 (*_api_TTS_Query_t)(void);


/************************************************************************************************
*   Font
*************************************************************************************************/
//typedef s32 (*_api_GetFontData_t)(char word[2], u8 fontType, u8* fontData);

/************************************************************************************************
*   FTP
*************************************************************************************************/
/*
typedef s32 (*_api_FTP_SetUserName_t)(u8 *username);                                                               
typedef s32 (*_api_FTP_SetPasswd_t)(u8 *password);                                                                 
typedef s32 (*_api_FTP_Connect_t)(u8 *svr_addr, u16 svr_port,Ql_CallBack_FtpConnect callback_ftpconnect);          
typedef s32 (*_api_FTP_Disconnect_t)(void);                                                                         
typedef s32 (*_api_FTP_SetConnMode_t)(FTP_ConnMode mod);                                                            
typedef s32 (*_api_FTP_SetTransMode_t)(FTP_TransMode mod);                                                           
typedef s32 (*_api_FTP_SetResumePoint_t)(u32 resume_point);                                                          
typedef s32 (*_api_FTP_SetLocalPos_t)(u8 *local_pos);                                                                
typedef s32 (*_api_FTP_SetSrvPath_t)(u8 *path_name );                                                                
typedef s32 (*_api_FTP_Upload_t)(u8 *filename,s32 filesize, s32 maxtime,Ql_CallBack_Ftp_Upload callback_ftp_upload);
typedef s32 (*_api_FTP_Download_t)(u8 *filename, Ql_CallBack_Ftp_Download  callback_ftp_download);                  
typedef s32 (*_api_FTP_GetState_t)(void);                                                                            
typedef s32 (*_api_FTP_GetTransLength_t)(void); 
*/

/************************************************************************************************
*   Debug Trace
*************************************************************************************************/
typedef s32   (*_api_Debug_Trace_t)(char* fmt, ...);


/************************************************************************************************
*   Standard Libraries
*************************************************************************************************/
typedef void  (*_api_itoa_t)(char *buf, s32 i, s32 base);
typedef void  (*_api_ui64toa_t)(char* str, u64 val, s32 radix);
typedef double (*_api_atof_t)(const char* s);
typedef char* (*_api_strcpy_t)(char* dest, const char* src);
typedef char* (*_api_strncpy_t)(char* dest, const char* src, u32 size);
typedef char* (*_api_strcat_t)(char* s1, const char* s2);
typedef char* (*_api_strncat_t)(char* s1, const char* s2, u32 size);
typedef u16  (*_api_strlen_t)(const char* str);
typedef s32  (*_api_strcmp_t)(const char*s1, const char*s2);
typedef s32  (*_api_strncmp_t)(const char* s1, const char* s2, u32 size);
typedef void* (*_api_memset_t)(void* dest, u8 value, u32 size);
typedef void* (*_api_memcpy_t)(void* dest, const void* src, u32 size);
typedef s32  (*_api_memcmp_t)(const void* dest, const void*src, u32 size);
typedef void* (*_api_memmove_t)(void* dest, const void* src, u32 size);
typedef char* (*_api_strstr_t)(const char* s1, const char* s2);
typedef char* (*_api_strchr_t)(const char* s1, s32 ch);


typedef s32 (*_create_api_hash_t)(char* api);
typedef s32 (*_get_api_ptr_t)(char* func_name );

/************************************************************************************************
*   Virtual Functions Table for All OpenCPU APIs
*************************************************************************************************/
typedef struct tagQL_OCPU_Funcs{
    u32* m_size; // 本结构体大小澹(包含此字段)
    OCPU_API_DebugTrace                         m_api_DebugTrace ;
    OCPU_API_sprintf                            m_api_sprintf;	 
    OCPU_API_snprintf                           m_api_snprintf;
    OCPU_API_sscanf                             m_api_sscanf;
    void * m_create_api_hash;
    void * m_get_api_ptr;
} QL_OCPU_Funcs;

#endif  // End-of __QL_OCPU_API_H__

