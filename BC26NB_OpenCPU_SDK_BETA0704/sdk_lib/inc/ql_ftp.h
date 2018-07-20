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
 *   ql_ftp.h 
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   Ftp  API defines.
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
#if 0

#ifndef __QL_FTP_H__
#define __QL_FTP_H__

#include "ql_type.h"

typedef enum tagFTP_ConnMode
{
	FTP_ConnMode_Active = 0,          //Active  Mode
	FTP_ConnMode_Passive              //Passive Mode
} FTP_ConnMode;



typedef enum tagFTP_TransMode
{
	FTP_TransMode_Bin = 0,           //binary  
	FTP_TransMode_Ascii               //ASCII
} FTP_TransMode;


typedef enum tagFTP_State
{
	FTP_STATUS_IDLE = 0,           //No FTP service.   
	FTP_STATUS_OPENING,         //Opening an FTP service.   
	FTP_STATUS_OPENED,	         //The FTP service is opened and idle.   
	FTP_STATUS_WORKING,   	 //Sending FTP commands to the FTP server and receiving response from the FTP server to start data transfer.
	FTP_STATUS_TRANSFER,  	 //Transferring data between the module and the FTP server.   
	FTP_STATUS_CLOSING,    	 //Closing the FTP service.   
	FTP_STATUS_CLOSED     	 //The FTP service is closed
} FTP_State;


typedef void (*Ql_CallBack_FtpConnect)(s32 result);
typedef void (*Ql_CallBack_Ftp_Upload)(s32 result,s32 size);
typedef void (*Ql_CallBack_Ftp_Download)(s32 result,s32 size);

/*****************************************************************

* Function:       Ql_FTP_SetUserName
* 
* Description:
*               	Set the user name to connect FTP server.
*               
*
* Parameters:
*			[in]username:The maxinum size of the password is 30.
*                                                         
*                                                           
*                   
* Return:        
*			QL_RET_OK, indicates this function successes.
*			Negative, indicates failure, please see 'Error Code Definition'
*****************************************************************/

s32 Ql_FTP_SetUserName(u8 *username);

 
/*****************************************************************

* Function:       Ql_FTP_SetPasswd
* 
* Description:
*               	Set the password to connect FTP server.
*               
*
* Parameters:
*			[in]password: The maxinum size of the password is 30.
*                                                         
*                                                           
*                   
* Return:        
*			QL_RET_OK, indicates this function successes.
*			Negative, indicates failure, please see 'Error Code Definition'
*****************************************************************/

s32 Ql_FTP_SetPasswd(u8 *password);


/*****************************************************************

* Function:       Ql_FTP_Connect
* 
* Description:
*               	Connect the FTP service.
*               
*
* Parameters:
*			[in]svr_addr: The address of the FTP server.
*                      [in]svr_port: The port of the FTP server.                                 
*                      callback_ftpconnect:
*                                           Callback the ruslt of the connect.
*                   
* Return:        
*			QL_RET_OK, indicates this function successes.
*			Negative, indicates failure, please see 'Error Code Definition'
*****************************************************************/

s32 Ql_FTP_Connect(u8 *svr_addr, u16 svr_port,Ql_CallBack_FtpConnect callback_ftpconnect);


/*****************************************************************

* Function:       Ql_FTP_Disconnect
* 
* Description:
*               	Disconnect the FTP service.
*               
*
* Parameters:
*			None.
*                                                         
*                                                           
*                   
* Return:        
*			QL_RET_OK, indicates this function successes.
*			Negative, indicates failure, please see 'Error Code Definition'
*****************************************************************/

s32 Ql_FTP_Disconnect(void);


/*****************************************************************

* Function:       Ql_FTP_SetConnMode
* 
* Description:
*               	Set the mode of the data connection.
*               
*
* Parameters:
*			[in]mod: FTP_ConnMode_Active,indicate select active  Mode
*                                    FTP_ConnMode_Passive,indicate select passive Mode                    
*                                                           
*                   
* Return:        
*			QL_RET_OK, indicates this function successes.
*			Negative, indicates failure, please see 'Error Code Definition'
*****************************************************************/

s32 Ql_FTP_SetConnMode(FTP_ConnMode mod);


/*****************************************************************

* Function:       Ql_FTP_SetTransMode
* 
* Description:
*               	Set the transfer type.
*               
*
* Parameters:
*			[in]mod: 	FTP_TransMode_Bin, indicate the type is binary.  
*					FTP_TransMode_Ascii, indicate the type is ASCII                                                       
*                                                           
*                   
* Return:        
*			QL_RET_OK, indicates this function successes.
*			Negative, indicates failure, please see 'Error Code Definition'
*****************************************************************/

s32 Ql_FTP_SetTransMode(FTP_TransMode mod);


/*****************************************************************

* Function:       Ql_FTP_SetResumePoint
* 
* Description:
*               	Set resuming point to resume file transfer.
*               
*
* Parameters:
*			[in]resume_point: Set the position of the pointer. Default is 0.
*                                                         
*                                                           
*                   
* Return:        
*			QL_RET_OK, indicates this function successes.
*			Negative, indicates failure, please see 'Error Code Definition'
*****************************************************************/

s32 Ql_FTP_SetResumePoint(u32 resume_point);


/*****************************************************************

* Function:       Ql_FTP_SetLocalPos
* 
* Description:
*               	Set a local postion for upload or download.
*               
*
* Parameters:
*			[in]local_pos: local postion,such as "/COM/","/UFS/","/SD/","/RAM/",Default is "/COM/".
*                                                         
*                                                           
*                   
* Return:        
*			QL_RET_OK, indicates this function successes.
*			Negative, indicates failure, please see 'Error Code Definition'
*****************************************************************/

s32 Ql_FTP_SetLocalPos(u8 *local_pos);


/*****************************************************************

* Function:       Ql_FTP_SetSrvPath
* 
* Description:
*               	Set the path in the FTP service to upload or download file.
*               
*
* Parameters:
*			[in]path_name: the name of the path to set.The maxinum size of the name is 100.
*
*
*
* Return:        
*			QL_RET_OK, indicates this function successes.
*			Negative, indicates failure, please see 'Error Code Definition'
*****************************************************************/

s32 Ql_FTP_SetSrvPath(u8 *path_name );


/*****************************************************************

* Function:       Ql_FTP_Upload
* 
* Description:
*               	Upload a file to FTP service.
*               
*
* Parameters:
*			[in]filename: The name of the file to upload.
*                      [in]filesize:    The size of the file to upload.if set to 0,it will upload the real size of the file.                         
*                      [in]maxtime: The max time allowed to upload file data. the unit is second,the default value is 900s.
*                      callback_ftp_upload:
*                                           CallBack the result and the real size of the uploaded file.     
* Return:        
*			QL_RET_OK, indicates this function successes.
*			Negative, indicates failure, please see 'Error Code Definition'
*****************************************************************/

s32 Ql_FTP_Upload(u8 *filename,s32 filesize, s32 maxtime,Ql_CallBack_Ftp_Upload callback_ftp_upload);

/*****************************************************************

* Function:       Ql_FTP_Download
* 
* Description:
*               	Download a file from FTP service.
*               
*
* Parameters:
*			[in]filename: The name of the file to download.
*                      callback_ftp_download:                                   
*                                          CallBack the result and the size of the downloaded file.             
*                   
* Return:        
*			QL_RET_OK, indicates this function successes.
*			Negative, indicates failure, please see 'Error Code Definition'
*****************************************************************/

s32 Ql_FTP_Download(u8 *filename, Ql_CallBack_Ftp_Download  callback_ftp_download);


/*****************************************************************

* Function:       Ql_FTP_GetState
* 
* Description:
*               	Query status of FTP service.
*               
*
* Parameters:
*			None.
*                                                         
*                                                           
*                   
* Return:        
*			A string indicate the current status of FTP service.
*	                FTP_STATUS_IDLE, indicates no FTP service.   
*	                FTP_STATUS_OPENING, indicates opening an FTP service.   
*	                FTP_STATUS_OPENED, indicates the FTP service is opened and idle.   
*	                FTP_STATUS_WORKING, indicates sending FTP commands to the FTP server and receiving response from the FTP server to start data transfer.
*	                FTP_STATUS_TRANSFER, indicates transferring data between the module and the FTP server.   
*	                FTP_STATUS_CLOSING, indicates closing the FTP service.   
*	                FTP_STATUS_CLOSED, indicates the FTP service is closed
*
*****************************************************************/

s32 Ql_FTP_GetState(void);


/*****************************************************************

* Function:       Ql_FTP_GetTransLength
* 
* Description:
*               	Query the real size transferred in the lastest transfer.
*               
*
* Parameters:
*			None.
*                                                         
*                                                           
*                   
* Return:        
*			A numeric to indicate the real size transferred
*                      in the lastest transfer operation.
*****************************************************************/

s32 Ql_FTP_GetTransLength(void);

#endif  // __QL_FTP_H__
#endif  // #if 0

