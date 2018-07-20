/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 |
 | File Description
 | ----------------
 |      Definitions for audio parameters & APIs
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by    :    Jay XIN
 |  Coded by       :    Jay XIN
 |  Tested by      :    Stanley YONG
 |
 \=========================================================================*/

#ifndef __QL_AUDIO_H__
#define __QL_AUDIO_H__

#include "Ql_type.h"


/****************************************************************************
 *  Built-in Audio Resource
 ***************************************************************************/
typedef enum QlAudioNameTag
{
    QL_AUDIO_EMS_CHIMES_HI = 1,
    QL_AUDIO_EMS_CHIMES_LO,
    QL_AUDIO_EMS_DING,
    QL_AUDIO_EMS_TADA,
    QL_AUDIO_EMS_NOTIFY,
    QL_AUDIO_EMS_DRUM,
    QL_AUDIO_EMS_CLAPS,
    QL_AUDIO_EMS_FANFARE,
    QL_AUDIO_EMS_CHORD_HI,
    QL_AUDIO_EMS_CHORD_LO,
    QL_AUDIO_1,
    QL_AUDIO_2,
    QL_AUDIO_3,
    QL_AUDIO_4,
    QL_AUDIO_5,
    QL_AUDIO_6,
    QL_AUDIO_7,
    QL_AUDIO_8,
    QL_AUDIO_9,
    QL_AUDIO_10,
    QL_AUDIO_11,
    QL_AUDIO_12,
    QL_AUDIO_13,
    QL_AUDIO_14,
    QL_AUDIO_15,
    QL_AUDIO_16,
    QL_AUDIO_17,
    QL_AUDIO_18,
    QL_AUDIO_19,
    QL_AUDIO_END
}
QlAudioName;


/****************************************************************************
 *  Audio Volume Level Definition
 ***************************************************************************/
typedef enum QlAudioVolumeLeveltag
{
    QL_AUDIO_VOLUME_LEVEL1 = 0,
    QL_AUDIO_VOLUME_LEVEL2,
    QL_AUDIO_VOLUME_LEVEL3,
    QL_AUDIO_VOLUME_LEVEL4,
    QL_AUDIO_VOLUME_LEVEL5,
    QL_AUDIO_VOLUME_LEVEL6,
    QL_AUDIO_VOLUME_LEVEL7,
    QL_AUDIO_VOLUME_MUTE,
    QL_AUDIO_VOLUME_LEVEL_END
}QlAudioVolumeLevel;


/****************************************************************************
 *  Audio Output Source
 ***************************************************************************/
typedef enum QlAudioPlayPathTag
{
    QL_AUDIO_PATH_NORMAL = 0,       /* speak1 */
    QL_AUDIO_PATH_HEADSET = 1,      /* earphone */
    QL_AUDIO_PATH_LOUDSPEAKER = 2,  /* speaker2, loudspeaker for free sound */
    QL_AUDIO_PATH_END
}QlAudioPlayPath;


/****************************************************************************
 *  Audio Format
 ***************************************************************************/
typedef enum QlAudioStreamFormattag
{
    QL_AUDIO_STREAMFORMAT_MP3 = 1,
    QL_AUDIO_STREAMFORMAT_AMR = 2,
    QL_AUDIO_STREAMFORMAT_WAV = 3,
    QL_AUDIO_STREAMFORMAT_END
}QlAudioStreamFormat;

typedef enum
{
    QL_AUD_REC_FMT_BY_EXTENSION = -1,
    QL_AUD_REC_FMT_AMR = 0,
    QL_AUD_REC_FMT_WAV,
    QL_AUD_REC_FMT_WAV_ALAW,
    QL_AUD_REC_FMT_WAV_ULAW,
    QL_AUD_REC_FMT_WAV_DVI_ADPCM
}QlAudioRecordFormat;

typedef enum
{
    QL_AUD_REC_QUALITY_LOW = 0,
    QL_AUD_REC_QUALITY_HIGH
}QlAudioRecordQuality;

typedef enum
{
    QL_AUD_REC_MIC1 = 0,    /*A1*/
    QL_AUD_REC_MIC2         /*A2,Headset*/
}QlAudioRecordDevice;

typedef enum
{
    QL_RING_TONE_ENABLE = 0,  /*normal mode*/
    QL_RING_TONE_DISABLE       /*quiet mode*/
}QlRingToneEnable;

typedef void (*OCPU_CB_GET_VOLUME)(s8 vol_level);

/* Define volume type.*/
typedef enum {
    VOL_TYPE_CTN  = 0 ,   /* MMI can apply to associate volume; call tone attribute */
    VOL_TYPE_KEY      ,   /* MMI can apply to associate volume; keypad tone attribute */
    VOL_TYPE_MIC      ,   /* microphone attribute */
    VOL_TYPE_GMI      ,   /* MMI can apply to associate volume; melody, imelody, midi attribute */
    VOL_TYPE_SPH      ,   /* MMI can apply to associate volume; speech sound attribute */
    VOL_TYPE_SID	  ,   /* side tone attribute */
    VOL_TYPE_MEDIA    ,   /* MMI can apply to associate volume; As MP3, Wave,... attribute */
    MAX_VOL_TYPE           
}volume_type_enum;


typedef enum
{
    QL_CONFIG_100MS_DETECT_THRESHOLD= 0, /*config 1400Hz and 2300Hz 100ms detect threshold*/
    QL_CONFIG_400MS_DETECT_THRESHOLD, /*config 1400Hz and 2300Hz 400ms detect threshold*/
    QL_CONFIG_DTMF_DETECT_THRESHOLD,  /*config DTMF detect threshold,excluding 1400Hz and 2300Hz */
}QlDTMFMODE;

 typedef struct
{
    QlDTMFMODE mode;//  mode config
    u32 prefixpause; //prefix pause number
    u32 lowthreshold; //low threshold value
    u32 highthreshold; //high threshold value
}QlDTMFCONFIG;

 typedef struct
{
    s16 key;   //<dtmfcode> DTMF tone code corresponding ASSCII 
                    //48  DTMF 0 
                    //49  DTMF 1 
                    //50  DTMF 2 
                    //51  DTMF 3 
                    //52  DTMF 4 
                    //53  DTMF 5 
                    //54  DTMF 6 
                    //55  DTMF 7 
                    //56  DTMF 8 
                    //57  DTMF 9 
                    //65  DTMF A 
                    //66  DTMF B
                    //67  DTMF C 
                    //68  DTMF D 
                    //42  DTMF * 
                    //35  DTMF # 
                    //69  1400Hz frequency 
                    //70  2300Hz frequency 
    s16 persisttime; //< persistence time> 
                              //100  100ms of the tone detected, only 1400Hz and 2300 Hz 
                              //400  400ms of the tone detected, only 1400Hz and 2300 Hz 
                              // 0    other tone.
}QlDTMFCode;

typedef void (*OCPU_CB_DTMF_INFO)(QlDTMFCode dtmfcode); //callback dtmf code
 
/*****************************************************************
* Function:     Ql_PlayAudio 
* 
* Description:
*               Plays the built-in audio resource.
*               When audio plays ended,
*               a EVENT_MEDIA_FINISH event will be received by embedded application.
* Parameters:
*               name:
*                       Audio resource built in Core System
*
*               repeat:
*                       TRUE or FALSE, indicates that Core System
*                       play the audio repeatedly or just once.
* Return:        
*               0 indicates the function suceeds
*               <0 indicates failure
*****************************************************************/
s32 Ql_PlayAudio(QlAudioName name, bool repeat);


/*****************************************************************
* Function:     Ql_StopAudio 
* 
* Description:
*               Stops playing the audio.
* Parameters:
*               name:
*                       Audio resource built in Core System
* Return:        
*               0 indicates the function suceeds
*               <0 indicates failure
*****************************************************************/
s32 Ql_StopAudio(QlAudioName name);


/*****************************************************************
* Function:     Ql_StartPlayAudioFile 
* 
* Description:
*               Plays an audio file, and sets the items as below:
*                   - Repeatedly or just Once
*                   - Volume level
*                   - Audio output source.
*               When audio plays ended,
*               a EVENT_MEDIA_FINISH event will be received by embedded application.
* Parameters:
*               asciifilename:
*                       Audio file name
*
*               repeat:
*                       TRUE or FALSE, indicates that Core System
*                       play the audio repeatedly or just once.
*
*               volumelevel:
*                       Volume level reference "QlAudioVolumeLevel"
*
*               audiopath:
*                       Audio output source
* Return:        
*               QL_RET_OK, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32 Ql_StartPlayAudioFile(u8 * asciifilename, bool repeat, u8 volumelevel, u8 audiopath);


/*****************************************************************
* Function:     Ql_PlayAudioFile_8k 
* 
* Description:
*               Plays an audio file with 8 kHz sample.
*               When audio plays ended,
*               a EVENT_MEDIA_FINISH event will be received by embedded application.
* Parameters:
*               file_name:
*                       Audio file name
*
*               repeat:
*                       TRUE or FALSE, indicates that Core System
*                       play the audio repeatedly or just once.
*
*               volume:
*                      the volume at the remote end. The Volume level reference "QlAudioVolumeLevel"
*
*               path:
*                       Audio output source
*               dlvolume:
*                       The volume at the local.The Volume level reference "QlAudioVolumeLevel"                  
* Return:        
*               QL_RET_OK, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32 Ql_PlayAudioFile_8k(u8* file_name, bool repeat, u8 volume, u8 path, u8 dlvolume);

/*****************************************************************
* Function:     Ql_StopAudioFile_8k 
* 
* Description:
*               Stops playing the audio 8K file.
* Parameters:
*               None
* Return:        
*               0 indicates the function suceeds
*               <0 indicates failure
*****************************************************************/
s32 Ql_StopAudioFile_8k(void);

/*****************************************************************
* Function:     Ql_StopPlayAudioFile 
* 
* Description:
*               Stops playing the audio file.
* Parameters:
*               None
* Return:        
*               0 indicates the function suceeds
*               <0 indicates failure
*****************************************************************/
s32 Ql_StopPlayAudioFile(void);


/*****************************************************************
* Function:     Ql_StartPlayAudioStream 
* 
* Description:
*               Plays an audio stream with stream size and stream format,
*               and sets the items as below:
*                   - Repeatedly or just Once
*                   - Volume level
*                   - Audio output source.
*               When audio plays ended,
*               a EVENT_MEDIA_FINISH event will be received by embedded application.
* Parameters:
*               stream:
*                       Audio stream
*
*               streamsize:
*                       Audio stream size
*
*               streamformat:
*                       Audio stream format
*
*               repeat:
*                       TRUE or FALSE, indicates that Core System
*                       play the audio repeatedly or just once.
*
*               volumelevel:
*                       Volume level reference "QlAudioVolumeLevel"
*
*               audiopath:
*                       Audio output source
* Return:        
*               QL_RET_OK, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32 Ql_StartPlayAudioStream(u8 * stream, u32 streamsize, s32 streamformat, bool repeat, u8 volume, u8 audiopath);


/*****************************************************************
* Function:     Ql_StopPlayAudioStream 
* 
* Description:
*               Stops playing the audio stream.
* Parameters:
*               None
* Return:        
*               0, indicates the function suceeds
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32 Ql_StopPlayAudioStream(void);

/*****************************************************************
* Function:     Ql_StartRecord
* 
* Description:  Start recording.
*               
* Parameters:	szFileName
*					file name. must be NUL terminated string.
*                   prefix with "SD:" to access the file in card.
*				format
*					recording format. Refer to QlAudioRecordFormat.
*               
*               
* Return:       QL_RET_OK if successful.  
*               or other error codes, please see 'Error Code Definition'
* Remark:       Keep this functiion for backward compatibility.
*               
*****************************************************************/

s32 Ql_StartRecord(char *szFileName, QlAudioRecordFormat format);


/*****************************************************************
* Function:     Ql_StartRecord_FullFunc
* 
* Description:  Start recording.
*               
* Parameters:	szFileName
*					file name. must be NUL terminated string.
*                   prefix with "SD:" to access the file in card.
*				format
*					recording format. Refer to QlAudioRecordFormat.
*               quality
*                   record quality. 1 = high, 0 = low.
*               fUseDefInput
*                   whether to use the default input device.
*               inputDev
*                   input source
*               size_limit
*                   Max record size in byte. 0: unlimited
*               time_limit
*                   Max record time in second. 0: unlimited
*               
* Return:       QL_RET_OK if successful.  
*              or other error codes, please see 'Error Code Definition' 
*****************************************************************/
s32 Ql_StartRecord_FullFunc(char               *szFileName, 
                      QlAudioRecordFormat   format, 
                      u8                    quality, 
                      bool                  fUseDefInput, 
                      u8                    inputDev,
                      u32                   size_limit,
                      u32                   time_limit
                      );

/*****************************************************************
* Function:     Ql_StopRecord
* 
* Description:  Stop the recording.
*               
* Parameters:
*               
* Return:       QL_RET_OK if success 
*               or other error codes, please see 'Error Code Definition' 
*****************************************************************/
s32 Ql_StopRecord(void);


/*****************************************************************
* Function:     Ql_PlayTone 
* 
* Description:
*               Start playing tone
* Parameters:
*               freq
                    frequency of the tone
                period_on
                    duration(ms)of tone is ON in one cycle.
                period_off
                    duration(ms) of mute in one cycle.
                duration
                    total duration(ms).
* Return:        
*               0, indicates the function suceeds
*               <0, indicates failure
*****************************************************************/
s32 Ql_PlayTone(u32 freq, u32 period_on, u32 period_off, u32 duration);


/*****************************************************************
* Function:     Ql_StopTone 
* 
* Description:
*               Stops the tone.
* Parameters:
*               None
* Return:        
*               0, indicates the function suceeds
*               <0, indicates failure
*****************************************************************/
s32 Ql_StopTone(void);

/*****************************************************************
* Function:     Ql_SelectRingTone 
* 
* Description:
*               Select the ring tone.
* Parameters:
*               tone_id
                    tone id : range from 1 to 10
* Return:        
*               0, indicates the function suceeds
*               <0, indicates failure
*****************************************************************/
s32 Ql_SelectRingTone(u8 tone_id);


/*****************************************************************
* Function:     Ql_VoiceCallChangePath 
* 
* Description:
*               Switches voice output source.
* Parameters:
*               path:
*                   Voice output source
* Return:        
*               TRUE, indicates the function suceeds
*               FALSE, indicates failure
*****************************************************************/
bool Ql_VoiceCallChangePath(QlAudioPlayPath path);


/*****************************************************************
* Function:     Ql_VoiceCallGetCurrentPath 
* 
* Description:
*               Retrieves the current voice output source.
* Parameters:
*               None
* Return:        
*               The current voice output source
*****************************************************************/
QlAudioPlayPath Ql_VoiceCallGetCurrentPath(void);


/*****************************************************************
* Function:     Ql_SetVolume 
* 
* Description:
*               Set the volume during calling.
* Parameters:
*               vol_type:
*                        0 = VOL_TYPE_CTN ,     call tone attribute 
*                        1 = VOL_TYPE_KEY ,     keypad tone attribute 
*                        2 = VOL_TYPE_MIC ,     microphone attribute  
*                        3 = VOL_TYPE_GMI ,     melody, imelody, midi attribute 
*                        4 = VOL_TYPE_SPH ,     speech sound attribute  
*                        5 = VOL_TYPE_SID ,     side tone attribute  
*                        6 = VOL_TYPE_MEDIA ,   As MP3, Wave,... attribute  
*               vol_level:
*                        Range is 1-100.
* Return:              
*               QL_RET_OK, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32  Ql_SetVolume(u8 vol_type/*volume_type_enum*/, u8 vol_level/*0~100*/);


/*****************************************************************
* Function:     Ql_SetVolume_Ex 
* 
* Description:
*               Set the volume of audio.
* Parameters:
*               vol_type:
*                        0 = VOL_TYPE_CTN ,     call tone attribute 
*                        1 = VOL_TYPE_KEY ,     keypad tone attribute 
*                        2 = VOL_TYPE_MIC ,     microphone attribute  
*                        3 = VOL_TYPE_GMI ,     melody, imelody, midi attribute 
*                        4 = VOL_TYPE_SPH ,     speech sound attribute  
*                        5 = VOL_TYPE_SID ,     side tone attribute  
*                        6 = VOL_TYPE_MEDIA ,   As MP3, Wave,... attribute  
*                vol_path:
*                        0 = AUD_MODE_NORMAL   ,   Normal Mode  
*                        1 = AUD_MODE_HEADSET  ,   HeadSet (Earphone) Mode 
*                        2 = AUD_MODE_LOUDSPK  ,   Loudspeaker Mode  
*                vol_level:
*                        Range is 1-100.
* Return:              
*               QL_RET_OK, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32  Ql_SetVolume_Ex(u8 vol_type/*volume_type_enum*/, u8 vol_path/*QlAudioPlayPath*/, u8 vol_level);


/*****************************************************************
* Function:     GetVolume 
* 
* Description:
*               Get the volume level of audio.
* Parameters:
*               vol_type:
*                        0 = VOL_TYPE_CTN ,     call tone attribute 
*                        1 = VOL_TYPE_KEY ,     keypad tone attribute 
*                        2 = VOL_TYPE_MIC ,     microphone attribute  
*                        3 = VOL_TYPE_GMI ,     melody, imelody, midi attribute 
*                        4 = VOL_TYPE_SPH ,     speech sound attribute  
*                        5 = VOL_TYPE_SID ,     side tone attribute  
*                        6 = VOL_TYPE_MEDIA ,   As MP3, Wave,... attribute  
* Return:              
*               The volume level, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32  Ql_GetVolume(u8 vol_type/*volume_type_enum*/);


/*****************************************************************
* Function:     Ql_SetMicGain 
* 
* Description:
*               Change the micphone gain level.
* Parameters:
*                channel:
*                        0 = AUD_MODE_NORMAL   ,   Normal Mode  
*                        1 = AUD_MODE_HEADSET  ,   HeadSet (Earphone) Mode 
*                        2 = AUD_MODE_LOUDSPK  ,   Loudspeaker Mode  
*                gain_level:
*                        Range is 1-15.
* Return:              
*               QL_RET_OK, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32  Ql_SetMicGain(u8 channel/*QlAudioPlayPath*/, u8 gain_level/*0~15*/);


/*****************************************************************
* Function:     Ql_GetMicGain 
* 
* Description:
*               Get the micphone gain level.
* Parameters:
*                channel:
*                        0 = AUD_MODE_NORMAL   ,   Normal Mode  
*                        1 = AUD_MODE_HEADSET  ,   HeadSet (Earphone) Mode 
*                        2 = AUD_MODE_LOUDSPK  ,   Loudspeaker Mode  
*                gain_level:
*                        Range is 1-15.
* Return:              
*               The gain level, suceess
*               or "-51" QL_RET_ERR_INVALID_PARAMETER
*****************************************************************/
s32  Ql_GetMicGain(u8 channel/*QlAudioPlayPath*/);


/*****************************************************************
* Function:     Ql_SetSideToneGain 
* 
* Description:
*               Change the sidetone gain level.
* Parameters:
*                channel:
*                        0 = AUD_MODE_NORMAL   ,   Normal Mode  
*                        1 = AUD_MODE_HEADSET  ,   HeadSet (Earphone) Mode 
*                        2 = AUD_MODE_LOUDSPK  ,   Loudspeaker Mode  
*                gain_level:
*                        Range is 1-15.
* Return:              
*               QL_RET_OK, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32  Ql_SetSideToneGain(u8 channel/*QlAudioPlayPath*/, u8 gain_level/*0~15*/);


/*****************************************************************
* Function:     Ql_GetSideToneGain 
* 
* Description:
*               Get the sidetone gain level.
* Parameters:
*                channel:
*                        0 = AUD_MODE_NORMAL   ,   Normal Mode  
*                        1 = AUD_MODE_HEADSET  ,   HeadSet (Earphone) Mode 
*                        2 = AUD_MODE_LOUDSPK  ,   Loudspeaker Mode  
* Return:              
*               The sidetone gain level, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32  Ql_GetSideToneGain(u8 channel/*QlAudioPlayPath*/);


/*****************************************************************
* Function:     Ql_CtrlEchoCancel 
* 
* Description:
*               Echo cancellation control.
* Parameters:
*                ctrlWord:
*                        221 for handset 
*                        224 for handfree
*                        0 for disable  
*                nlp:
*                        Range is 0-2048.The greater the value, the more reduction of echo. 
*                        0 means disabling the NLP algorithm.
*                suppression:
*                        Range is 0-32767.The smaller the vallue, the more reduction of echo. 
*                        0 means disabling the echo suppression algorithm. 
*                        224 for handfree
*                        0 for disable  
*                nr:
*                        Noise reduction controller.Should NOT be set to 0. 
*                        849 suitable for handset and handset for applications. 
*                        374 suitable for handfree. 
*                channel:
*                        0 = AUD_MODE_NORMAL   ,   Normal Mode  
*                        1 = AUD_MODE_HEADSET  ,   HeadSet (Earphone) Mode 
*                        2 = AUD_MODE_LOUDSPK  ,   Loudspeaker Mode  
* Return:              
*               QL_RET_OK, suceess
*               or other error codes, please see 'Error Code Definition'
*****************************************************************/
s32  Ql_CtrlEchoCancel(u16 ctrlWord,      /* 221 for handset, 224 for handfree, 0: disable*/
                                 u16 nlp,           /* 0-2048 */
                                 u16 suppression,   /* 0-32767 */
                                 u16 nr,            /* 849 for handset, 374 for handfree, */
                                 u8  channel        /* one value of 'volume_category_enum' */
                                 );



/*****************************************************************
* Function:     Ql_VTS
* 
* Description:
*                 The function allows the transmission of DTMF tones and arbitrary tones in voice mode.
*                 These tones may be used (for example) when annoncing the start of recording period.
* Parameters:
*                dtmf_str:
*                         "<dtmf>" or "<dtmf>, <duration>" or "<dtmf>, <duration>,<dtmf>, <duration> ....."  
*                         duration value: must less then 50, unit in 100ms.
*                       Case 1: If only input "<dtmf>", eg.: dtmf_str = "1234", then the default duration "1" will 
*                                be adopted (i.e. 1 * 100ms = 100ms). i.e: there's 100ms pause between '1','2','3' and '4'.
*                       Case 2: If input DTMF string as the format of "<dtmf>, <duration>", 
*                               eg.: dtmf_str = "1234,5", then each DTMF tone will pause 500ms (=5*100ms). i.e: there's 500ms pause between '1','2','3' and '4'.
*                       Case 3: If input DTMF string as the format of "<dtmf>, <duration>,<dtmf>, <duration> .....",
*                               which means developer can define the pause time for each group of DTMF string, 
*                               eg: dtmf_str = "1234,5,67,9", then there's 500ms pause between '1','2','3' and '4', and 900ms pause between '4','6' and '7'.
*                           
*                len:
*                        The length of 'dtmf_str'. The maximum value is 20.
* Return:              
*               QL_RET_OK:          suceess
*               QL_RET_ERR_PARAM:   the parameter is error
*               QL_ERR_DTMF_BUSY:   DTMF playing is busy
*               QL_ERR_DTMF_NO_CALLING: No call session.
* Note:  
*               The DTMF codes' range is [0..9, A..D,*,#], and each group of DTMF code should be less than 20. 
*               And the total of DTMF codes must be less than 20 too.
*****************************************************************/
s32 Ql_VTS(u8* dtmf_str, u8 len);
/*****************************************************************
* Function:     Ql_WDTMF
* 
* Description:
*                 The function allows the transmission of DTMF tones and arbitrary tones in voice mode.
*                 
* Parameters:
*                ul_volume:
*                        Uplink channel of the volume.  range 0~7
*                dl_volume:
*                        downlink channel of the volume. range 0~7
*                str:
*                     "<dtmfcode>,<continuancetime>,<mutetime>" or "<dtmfcode>,<continuancetime>,<mutetime>......" 
*                      the unit for continuancetime and mutetime is ms
*
*                     Case1: If input DTMF string as the format of  "<dtmfcode>,<continuancetime>,<mutetime>"
*                            eg.: str = "1234,30,50",then each DTMF tone will last 30ms, and 50ms pause between two DTMF tones.
*                            i.e: each tone of '1','2','3'and'4' lasts 30ms, and pauses 50ms between '1','2','3' and '4'.
*
*                     Case2:If input DTMF string as the format of "<dtmfcode>,<continuancetime>,<mutetime>......",
*                           then developer can define multi-group DTMF codes, and the time attributions for each group of DTMF code.
*                           eg: str = "123,30,50,45,60,100" 
*                               each tone of '1','2' and '3' lasts 30ms, and pauses 50ms.
*                               each tone of '4' and '5' lasts 60ms, and pauses 100ms between '4' and '5'.
*                len:
*                     the length of 'str'. The maximux value is 400.
* Return:              
*               QL_RET_OK, suceess
*               QL_RET_ERR_PARAM :  the parameter is error
*               QL_ERR_DTMFSTRING_TOO_LONG: the DTMF String is too long
*               Ql_RET_ERR_MEM_FULL:        no memory, means 'str' is NULL or fails to get memory.
*               QL_ERR_WDTMF_PS_BUSY:       WDTMF playing is busy
*
* Note:  
*               The WDTMF codes' range is [0..9, A..D,*,#,E,F,G]. 
*                   'E' means the tone with 1400Hz frequency
*                   'F' means the tone with 2300Hz frequency
*                   'G' means the tone with 1KHz   frequency
*****************************************************************/
s32 Ql_WDTMF(u8 ul_volume, u8 dl_volume, char *str ,u16 len);

/*****************************************************************
* Function:     Ql_EnableDTMF
* 
* Description:
*                 The function is used to enable dtmf detecting.
*                 
* Parameters:
*                cb_dtmf_info: 
*                          [out] point to callback function.
*
* Return:              
*               QL_RET_OK, suceess
*               QL_RET_ERR_GENERAL_FAILURE, indicates general failture.
*****************************************************************/
s32 Ql_EnableDTMF(OCPU_CB_DTMF_INFO cb_dtmf_info);

/*****************************************************************
* Function:     Ql_DisableDTMF
* 
* Description:
*                 The function is used to disable dtmf detecting.
*                 
* Parameters:
*                none
*                          
*
* Return:              
*               QL_RET_OK, suceess
*****************************************************************/
s32 Ql_DisableDTMF(void);

/*****************************************************************
* Function:     Ql_GetDTMFConfig
* 
* Description:
*                 The function is used to get DTMF config.
*                 
* Parameters:
*                dtmfconfig : 
*                          [ in ]dtmfconfig.mode  mode config (range 0~2)
*                          [out]dtmfconfig.prefixpause     prefix pause number (range 0~u32)
*                          [out]dtmfconfig.lowthreshold   low threshold tone (range 0~u32) 
*                          [out]dtmfconfig.highthreshold  high threshold tone (range 0~u32)
*
* Return:              
*               QL_RET_OK, suceess
*               QL_RET_ERR_PARAM, indicates error.
*****************************************************************/
s32 Ql_GetDTMFConfig(QlDTMFCONFIG *dtmfconfig);

/*****************************************************************
* Function:     Ql_SetDTMFConfig
* 
* Description:
*                 The function is used to set DTMF config.
*                 
* Parameters:
*                dtmfconfig : 
*                          [in]dtmfconfig.mode  mode config (range 0~2)
*                          [in]dtmfconfig.prefixpause     prefix pause number(range 0~u32)
*                          [in]dtmfconfig.lowthreshold   low threshold tone (range 0~u32)
*                          [in]dtmfconfig.highthreshold  high threshold tone(range 0~u32)
*                          
* Return:              
*               QL_RET_OK, suceess
*               QL_RET_ERR_PARAM, indicates error.
*****************************************************************/
s32 Ql_SetDTMFConfig(QlDTMFCONFIG dtmfconfig);


/*****************************************************************
* Function:     Ql_SetRingToneEnable
* 
* Description:
*                 The function is used to control incoming ring.
*                 
* Parameters:
*                mode: 0:normal mode
*                          1:quiet mode
*
* Return:              
*               QL_RET_OK, suceess
*               QL_RET_ERR_PARAM, indicates error.
*****************************************************************/
s32 Ql_SetRingToneEnable(QlRingToneEnable ring_enable);

#endif // End-of __QL_AUDIO_H__

