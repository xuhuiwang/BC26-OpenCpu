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
 *   ril_socket.h 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   The file declares some API functions, which are related to socket
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
#ifndef __RIL_LwM2M_H__
#define __RIL_LwM2M_H__
#include "ql_type.h"


typedef struct{
u32 actual_length;
u32 remain_length;
u8* buffer;
u8* prefix;

}LwM2M_Recv_Param;

/******************************************************************************
* Function:     RIL_QLwM2M_Serv
*  
* Description:
*               Configure the IoT Platform Address and Port
*
* Parameters:    
*               ip_addr:
*                   [in] 
*                   The IoT platform ip address
*               port:
*                   [in]
*                   The IoT platform port.
* Return:  
*                The socket ID, or other Error Codes. 
*                RIL_AT_FAILED, send AT failed.
*                RIL_AT_TIMEOUT,send AT timeout.
*                RIL_AT_BUSY,   sending AT.
*                RIL_AT_INVALID_PARAM, invalid input parameter.
*                RIL_AT_UNINITIALIZED, RIL is not ready, need to wait for MSG_ID_RIL_READY
*                                      and then call Ql_RIL_Initialize to initialize RIL.
******************************************************************************/
s32 RIL_QLwM2M_Serv(u8* ip_addr,u32 port);


/******************************************************************************
* Function:     RIL_QLwM2M_Conf
*  
* Description:
*               Configure the IoT Platform Parameters
*
* Parameters:    
*               endpoint:
*                   [in] 
*                 The device's endpoint name.
* Return:  
*                The socket ID, or other Error Codes. 
*                RIL_AT_FAILED, send AT failed.
*                RIL_AT_TIMEOUT,send AT timeout.
*                RIL_AT_BUSY,   sending AT.
*                RIL_AT_INVALID_PARAM, invalid input parameter.
*                RIL_AT_UNINITIALIZED, RIL is not ready, need to wait for MSG_ID_RIL_READY
*                                      and then call Ql_RIL_Initialize to initialize RIL.
******************************************************************************/
s32 RIL_QLwM2M_Conf(u8* endpoint);


/******************************************************************************
* Function:     RIL_QLwM2M_Aaddobj
*  
* Description:
*                   Add a LwM2M Object
*
* Parameters:    
*               obj_id:
*                   [in] 
*                   Object id. The max object id number is 65535.
*               ins_id:
*                   [in] 
*                   Instance id.
*               res_num:
*                   [in] 
*                   Resources id number.
*               res_id:
*                   [in] 
*                   Resources id.



* Return:  
*                The socket ID, or other Error Codes. 
*                RIL_AT_FAILED, send AT failed.
*                RIL_AT_TIMEOUT,send AT timeout.
*                RIL_AT_BUSY,   sending AT.
*                RIL_AT_INVALID_PARAM, invalid input parameter.
*                RIL_AT_UNINITIALIZED, RIL is not ready, need to wait for MSG_ID_RIL_READY
*                                      and then call Ql_RIL_Initialize to initialize RIL.
******************************************************************************/
s32 RIL_QLwM2M_Aaddobj(u8 obj_id,u8 ins_id,u8 res_num,u8* res_id);


/******************************************************************************
* Function:     RIL_QLwM2M_Open
*  
* Description:
*                   Send a Register Request
*
* Parameters:    
*                 mode
* Return:  
*                The socket ID, or other Error Codes. 
*                RIL_AT_FAILED, send AT failed.
*                RIL_AT_TIMEOUT,send AT timeout.
*                RIL_AT_BUSY,   sending AT.
*                RIL_AT_INVALID_PARAM, invalid input parameter.
*                RIL_AT_UNINITIALIZED, RIL is not ready, need to wait for MSG_ID_RIL_READY
*                                      and then call Ql_RIL_Initialize to initialize RIL.
******************************************************************************/
s32 RIL_QLwM2M_Open(u8 mode );


/******************************************************************************
* Function:     RIL_QLwM2M_Update
*  
* Description:
*                  Send an Update Request
*
* Parameters:    
*                void
* Return:  
*                The socket ID, or other Error Codes. 
*                RIL_AT_FAILED, send AT failed.
*                RIL_AT_TIMEOUT,send AT timeout.
*                RIL_AT_BUSY,   sending AT.
*                RIL_AT_INVALID_PARAM, invalid input parameter.
*                RIL_AT_UNINITIALIZED, RIL is not ready, need to wait for MSG_ID_RIL_READY
*                                      and then call Ql_RIL_Initialize to initialize RIL.
******************************************************************************/
s32 RIL_QLwM2M_Update(void);


/******************************************************************************
* Function:     RIL_QLwM2M_Cfg
*  
* Description:
*               Configure Optional Parameters
*
* Parameters:    
*               send_format:
*                   [in] 
*                   send data format
*               recv_format:
*                   [in] 
*                   receive data format
*
* Return:  
*                The socket ID, or other Error Codes. 
*                RIL_AT_FAILED, send AT failed.
*                RIL_AT_TIMEOUT,send AT timeout.
*                RIL_AT_BUSY,   sending AT.
*                RIL_AT_INVALID_PARAM, invalid input parameter.
*                RIL_AT_UNINITIALIZED, RIL is not ready, need to wait for MSG_ID_RIL_READY
*                                      and then call Ql_RIL_Initialize to initialize RIL.
******************************************************************************/
s32 RIL_QLwM2M_Cfg(u8 send_format,u8 recv_format);


/******************************************************************************
* Function:     RIL_QLwM2M_Send
*  
* Description:
*                Send Data with Mode
*
* Parameters:    
*               obj_id:
*                   [in] 
*                   Object ID.
*               ins_id:
*                   [in] 
*                   Instance ID.
*               res_id:
*                   [in] 
*                   Resources ID.
*               length:
*                   [in] 
*                   Length of data sent.
*               data:
*                   [in] 
*                   Hexadecimal format string
*               mode:
*                   [in] 
*                 0x0000     Send NON message
*                 0x0100	   Send CON message
* Return:  
*                The socket ID, or other Error Codes. 
*                RIL_AT_FAILED, send AT failed.
*                RIL_AT_TIMEOUT,send AT timeout.
*                RIL_AT_BUSY,   sending AT.
*                RIL_AT_INVALID_PARAM, invalid input parameter.
*                RIL_AT_UNINITIALIZED, RIL is not ready, need to wait for MSG_ID_RIL_READY
*                                      and then call Ql_RIL_Initialize to initialize RIL.
******************************************************************************/
s32 RIL_QLwM2M_Send(u8 obj_id,u8 ins_id,u8 res_id,u32 length,u8* data,u8* mode);


/******************************************************************************
* Function:     RIL_QLwM2M_RD
*  
* Description:
*                Receive Data from Buffer
*
* Parameters:    
*                   rd_length:
*                   [in] 
*                   The maximum length of data to be retrieved, the range is 0-1024
*                   actual_length:
*                   [out] 
*                   The actual length of received data
*                   remain_length:
*                   [out] 
*                   The unread length of received data
*                   data_buffer:
*                   [out] 
*                   received data

* Return:  
*                The socket ID, or other Error Codes. 
*                RIL_AT_FAILED, send AT failed.
*                RIL_AT_TIMEOUT,send AT timeout.
*                RIL_AT_BUSY,   sending AT.
*                RIL_AT_INVALID_PARAM, invalid input parameter.
*                RIL_AT_UNINITIALIZED, RIL is not ready, need to wait for MSG_ID_RIL_READY
*                                      and then call Ql_RIL_Initialize to initialize RIL.
******************************************************************************/
s32 RIL_QLwM2M_RD(u32 rd_length,u32* actual_length,u32* remain_length,u8* data_buffer);


/******************************************************************************
* Function:     RIL_QLwM2M_Close
*  
* Description:
*               Send a Deregister Request
*
* Parameters:    
*               void
*
* Return:  
*                The socket ID, or other Error Codes. 
*                RIL_AT_FAILED, send AT failed.
*                RIL_AT_TIMEOUT,send AT timeout.
*                RIL_AT_BUSY,   sending AT.
*                RIL_AT_INVALID_PARAM, invalid input parameter.
*                RIL_AT_UNINITIALIZED, RIL is not ready, need to wait for MSG_ID_RIL_READY
*                                      and then call Ql_RIL_Initialize to initialize RIL.
******************************************************************************/
s32 RIL_QLwM2M_Close(void);


#endif // __RIL_NETWORK_H__
