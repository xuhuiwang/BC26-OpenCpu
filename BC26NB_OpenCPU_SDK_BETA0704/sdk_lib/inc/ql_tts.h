#ifndef __QL_TTS_H__
#define __QL_TTS_H__

#include "ql_type.h"

/*TTS Playing State */
typedef enum
{
   TTS_STATUS_IDLE    = 0,
   TTS_STATUS_PLAYING = 1
}Enum_TTSStatus;

/* constants for values of parameter TTS_PARAM_VOICE_SPEED */
/* the range of voice speed value is from -32768 to +32767 */
#define TTS_SPEED_MIN					-32768		/* slowest voice speed */
#define TTS_SPEED_NORMAL				0			/* normal voice speed (default) */
#define TTS_SPEED_MAX					+32767		/* fastest voice speed */

/* constants for values of parameter TTS_PARAM_VOLUME */
/* the range of volume value is from -32768 to +32767 */
#define TTS_VOLUME_MIN				-32768		/* minimized volume */
#define TTS_VOLUME_NORMAL				0			/* normal volume */
#define TTS_VOLUME_MAX				+32767		/* maximized volume (default) */

/* constants for values of parameter TTS_PARAM_VEMODE */
#define TTS_VEMODE_NONE				    0			/* none */
#define TTS_VEMODE_WANDER				1			/* wander */
#define TTS_VEMODE_ECHO				    2			/* echo */
#define TTS_VEMODE_ROBERT				3			/* robert */
#define TTS_VEMODE_CHROUS				4			/* chorus */
#define TTS_VEMODE_UNDERWATER			5			/* underwater */
#define TTS_VEMODE_REVERB				6			/* reverb */
#define TTS_VEMODE_ECCENTRIC			7			/* eccentric */

typedef void (*Callback_TTS_Play)(s32 res);


/*****************************************************************
* Function:     Ql_TTS_Initialize 
* 
* Description:
*               Initialize the TTS function.
* Parameters:
*               callback_play:
*                     A pointer to callback function.
* Return:        
*               QL_RET_OK,success
*               QL_RET_ERR_INVALID_PARAMETER, the param invalid.
*****************************************************************/
s32 Ql_TTS_Initialize(Callback_TTS_Play callback_play);

/*****************************************************************
* Function:     Ql_TTS_Play 
* 
* Description:
*               Play TTS Text.
* Parameters:
*               content:
*                    [in]pointer to text.
*               len:
*                    [in]The lenth of text to speech. len <= 1024
* Return:        
*               QL_RET_OK, suceess
*               or other error codes, please see 'TTS Error Code Definition'
*****************************************************************/
s32 Ql_TTS_Play(u8* content, u8 len);


/*****************************************************************
* Function:     Ql_TTS_PlayInCall 
* 
* Description:
*               Play TTS Text as background audio during calling.
* Parameters:
*               content:
*                     [in]pointer to text.
*               len:
*                     [in]The lenth of text to speech. len <= 1024
*               upLinkVolume:
*                     [in]the volume at the remote end. The value ranges
*                     from 0 to 7.
*               downLinkVolume:
*                     [in]The volume at the local.The value ranges
*                     from 0 to 7. If set to 0, the side will not hear
*                     the TTS sound.
* Return:        
*               QL_RET_OK, suceess
*               QL_RET_ERR_GENERAL_FAILURE, fail to play tts.
*               Ql_RET_NOT_SUPPORT, not support this function.
*****************************************************************/
s32 Ql_TTS_PlayInCall(u8* content, u8 len, u8 upLinkVolume, u8 downLinkVolume);


/*****************************************************************
* Function:     Ql_TTS_Stop 
* 
* Description:
*               Stop playing.
* Parameters:
*               None.
* Return:        
*               QL_RET_OK, suceess
*****************************************************************/
s32 Ql_TTS_Stop(void);


/*****************************************************************
* Function:     Ql_TTS_Query 
* 
* Description:
*               Check status of TTS.
* Parameters:
*               None.
*  return:
*	          0 = ivTTS_STATUS_IDLE,		 invalid 
*	          1 = ivTTS_STATUS_PLAYING,		 playing 
*               Negative indicates failed. Please see 'Error Code Definition'
******************************************************************/
s32 Ql_TTS_Query(void);
#endif  // End-of __QL_TTS_H__

