/*==========================================================================
 |               Quectel OpenCPU --  Library source files
 |
 |              Copyright (c) 2013 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      This file defines interfaces which are used in 'ril_sms.c'.
 |      NOTE: The interfaces are based on portable codes. Therefore they are not related to platform.
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by     :   Vicent GAO
 |  Coded    by     :   Vicent GAO
 |  Tested   by     :   Vicent GAO
 |--------------------------------------------------------------------------
 | Revision History
 | ----------------
 |  2013/11/21       Vicent GAO        Create this file by ROTVG00006-P01
 |  2015/06/03       Vicent GAO        Add support for read/send con-sms by ROTVG00006-P05
 |  ------------------------------------------------------------------------
 \=========================================================================*/

#include "custom_feature_def.h"

#if (defined(__OCPU_RIL_SUPPORT__) && defined(__OCPU_RIL_SMS_SUPPORT__))

#include "Ql_type.h"
#include "Ql_stdlib.h"
#include "Ql_uart.h"
#include "Ql_trace.h"
#include "lib_ril_sms.h"

/////////////////////////////////////////////////////////////
// MACRO CONSTANT DEFINITIONS
/////////////////////////////////////////////////////////////
#define DBG_SWITCH    (FALSE)
#define DBG_PORT      (UART_PORT2)

#define DELIVER_TPDU_MIN_LEN        (13)
#define DELIVER_TPDU_MAX_LEN        (163)
#define SUBMIT_TPDU_MIN_LEN         (7)     //<VP> is not present
#define SUBMIT_TPDU_MAX_LEN         (164)   //<VPF> is 'LIB_SMS_VPF_TYPE_ABSOLUTE'

#define PDU_UD_MAX_BYTE_LEN     (140)   //Unit is : byte

#define SCTS_FIXED_BYTE_LEN   (7)
#define SCTS_TIME_ZONE_MAX    (48)
#define SCTS_TIME_ZONE_MIN    (-48)

//Macros for Alpha <---> CharSet
#define ALPHA_DEFAULT_EXTEND_CHAR_FLAG   (0x1B)
#define ALPHA_DEFAULT_EXTEND_CHAR_COUNT  (10)

#define UCS2_CHAR_DEFAULT          (LIB_SMS_CHAR_QM)
#define ALPHA_DEFAULT_CHAR_DEFAULT (LIB_SMS_CHAR_QM)
#define ALPHA_DEFAULT_CHAR_COUNT   (128)

//<2015/06/04-ROTVG00006-P08-Vicent.Gao,<[SMS] Add support for CharSet: IRA.>
#define CHARSET_IRA_CHAR_COUNT     (128)
#define IRA_CHAR_DEFAULT           (UCS2_CHAR_DEFAULT)
//>2015/06/04-ROTVG00006-P08-Vicent.Gao

//<2015/06/05-ROTVG00006-P09-Vicent.Gao,<[SMS] Add support for CharSet: 8859-1.>
#define CHARSET_88591_CHAR_COUNT     (256)
#define ISO88591_CHAR_DEFAULT        (UCS2_CHAR_DEFAULT)
//>2015/06/05-ROTVG00006-P09-Vicent.Gao

#define UDH_ID_BYTE_VAL_CON_8_BIT    (0x00)  //Warning: Please NOT-CHANGE this value!!
#define UDH_ID_BYTE_VAL_CON_16_BIT   (0x08)  //Warning: Please NOT-CHANGE this value!!

#define SUBMIT_PDU_DEFAULT_MR_VAL    (0x00)

/////////////////////////////////////////////////////////////
// ENUM TYPE DEFINITIONS
/////////////////////////////////////////////////////////////

typedef enum
{
    DCS_MSG_TYPE_DATA_CODING_IND = 0,
    DCS_MSG_TYPE_WAITING_DISCARD_IND = 1,
    DCS_MSG_TYPE_WAITING_STORE_IND = 2,
    DCS_MSG_TYPE_RESERVED = 3,
    
    DCS_MSG_TYPE_INVALID = 0xFF
} LIB_SMS_DCSMsgTypeEnum;

/////////////////////////////////////////////////////////////
// STRUCT TYPE DEFINITIONS
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS
/////////////////////////////////////////////////////////////

static bool ConvBCDNumberToAscii(u8 *pBCDNumber,u8 uBCDLen, u8 *pNumber, u8 *pNumberLen);
//<2015/03/23-ROTVG00006-P04-Vicent.Gao,<[SMS] Segment 4==>Fix issues of RIL SMS LIB.>
static bool ConvANNumberToAscii(u8 *pANNumber,u8 uANLen, u8 *pNumber, u8 *pNumberLen);
//>2015/03/23-ROTVG00006-P04-Vicent.Gao

static bool ConvAsciiNumberToBCD(u8 *pNumber,u8 uNumberLen,u8 *pBCD,u16 *pBCDLen);

static u16 EncodeGSM7Bit(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 uDestLen);
static bool EncodeTimeStamp(LIB_SMS_TimeStampStruct *pTimeStamp,u8 *pPDU,u16 *pPDULen);
static bool EncodeAddress(LIB_SMS_PhoneNumberStruct *pNumber,u8 *pPDU,u16 *pPDULen,bool bWTSFlag);
static bool EncodeValidityPeriod(u8 uVPF,LIB_SMS_ValidityPeriodUnion *pVP,u8 *pPDU,u16 *pPDULen);
static bool EncodeUserDataHeader(LIB_SMS_ConSMSParamStruct *pConSMSParam,u8 *pPDU,u16 *pPDULen);
static bool EncodeUserDataBody(u8 uAlpha,u8 uUDType,LIB_SMS_UserDataStruct *pUserData,u8 *pPDU,u16 *pPDULen);

static u16 DecodeGSM7Bit(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 uDestLen);
static bool DecodeTimeStamp(u8 *pBCDTS,u8 uBCDTSLen,LIB_SMS_TimeStampStruct *pTimeStamp);
static bool DecodeAddress(u8 *pAddress,u8 uAddrByte,LIB_SMS_PhoneNumberStruct *pNumber);
static bool DecodeUserDataHeader(u8 *pUDH,u8 uUDByte,LIB_SMS_ConSMSParamStruct *pConSMSParam);
static bool DecordUserDataBody(u8 *pUD,u8 uUDLen,u8 uAlpha,u8 LIB_SMS_UD_TYPE_INVALID,LIB_SMS_UserDataStruct *pUserData);

static bool DecodeDeliverTPDUOct(u8 *pTPDUOct, u16 uTPDUOctLen, LIB_SMS_DeliverPDUParamStruct *pParam);
static bool DecodeSubmitTPDUOct(u8 *pTPDUOct, u16 uTPDUOctLen, LIB_SMS_SubmitPDUParamStruct *pParam);

static bool ConvAlphaDefaultToCharSetUCS2(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen);
static bool ConvAlpha8BitToCharSet(u8 *pSrc,u16 uSrcLen,u8 uCharSet,u8 *pDest,u16 *pDestLen);
static bool ConvAlphaDefaultToCharSet(u8 *pSrc,u16 uSrcLen,u8 uCharSet,u8 *pDest,u16 *pDestLen);

static bool ConvCharSetUCS2ToAlphaDefault(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen);
static bool ConvCharSetGSMToAlphaUCS2(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen);
static bool ConvCharSetUCS2ToAlpha(u8 *pSrc,u16 uSrcLen,u8 uAlpha,u8 *pDest,u16 *pDestLen);
static bool ConvCharSetGSMToAlpha(u8 *pSrc,u16 uSrcLen,u8 uAlpha,u8 *pDest,u16 *pDestLen);
static bool ConvCharSetHEXToAlpha(u8 *pSrc,u16 uSrcLen,u8 uAlpha,u8 *pDest,u16 *pDestLen);

//<2015/06/04-ROTVG00006-P08-Vicent.Gao,<[SMS] Add support for CharSet: IRA.>
static bool ConvAlphaDefaultExtendCharToIRA(u16 uDefaultEChar,u8 *pIRAChar);
static bool ConvCharSetIRACharToAlphaDefaultExtend(u8 uIRAChar, u16 *pDefaultEChar);
static u16 ConvCharSetIRALenToAlpha(LIB_SMS_DCSAlphaEnum eAlhpa,u8 *pCSData,u16 uCSLen);
static bool ConvAlphaDefaultToCharSetIRA(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen);
static bool ConvCharSetIRAToAlphaDefault(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen);
static bool ConvCharSetIRAToAlphaUCS2(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen);
static bool ConvCharSetIRAToAlpha(u8 *pSrc,u16 uSrcLen,u8 uAlpha,u8 *pDest,u16 *pDestLen);
//>2015/06/04-ROTVG00006-P08-Vicent.Gao

//<2015/06/05-ROTVG00006-P09-Vicent.Gao,<[SMS] Add support for CharSet: 8859-1.>
static bool ConvAlphaDefaultExtendCharTo88591(u16 uDefaultEChar,u8 *p88591Char);
static bool ConvCharSet88591CharToAlphaDefaultExtend(u8 u88591Char, u16 *pDefaultEChar);
static u16 ConvCharSet88591LenToAlpha(LIB_SMS_DCSAlphaEnum eAlhpa,u8 *pCSData,u16 uCSLen);
static bool ConvAlphaDefaultToCharSet88591(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen);
static bool ConvCharSet88591ToAlphaDefault(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen);
static bool ConvCharSet88591ToAlphaUCS2(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen);
static bool ConvCharSet88591ToAlpha(u8 *pSrc,u16 uSrcLen,u8 uAlpha,u8 *pDest,u16 *pDestLen);
//>2015/06/05-ROTVG00006-P09-Vicent.Gao

/////////////////////////////////////////////////////////////
// GLOBAL DATA DEFINITIONS
/////////////////////////////////////////////////////////////
static u16 sg_aAlphaDefaultExtendChars[ALPHA_DEFAULT_EXTEND_CHAR_COUNT] = 
{
    /*<FF>   ^      {      }      \  */
    0x1B0A,0x1B14,0x1B28,0x1B29,0x1B2F,
    /*[      ~      ]      |      €  */
    0x1B3C,0x1B3D,0x1B3E,0x1B40,0x1B65
};

static u16 sg_aAlphaDefaultExtendCharsUCS2Map[ALPHA_DEFAULT_EXTEND_CHAR_COUNT] = 
{
    /*<FF>   ^      {      }      \  */
    0x000C,0x005E,0x007B,0x007D,0x005C,
    /*[      ~      ]      |      €  */
    0x005B,0x007E,0x005D,0x007C,0x20AC
};

#define CDAA   (UCS2_CHAR_DEFAULT)

static u16 sg_aAlphaDefaultCharsUCS2Map[ALPHA_DEFAULT_CHAR_COUNT] = 
{
    /*@      ¡ê      $     £¤      ¨¨     ¨¦     ¨´     ¨¬     ¨°     ?      <LF>   ?      ?      <CR>   ?      ? */
    0x0040,0x00A3,0x0024,0x00A5,0x00E8,0x00E9,0x00F9,0x00EC,0x00F2,0x00C7,0x000A,0x00D8,0x00F8,0x000D,0x00C5,0x00E5,
    /*¦¤     _       ¦µ    ¦£     ¦«     ¦¸     ¦°     ¦·     ¦²     ¦¨     ¦®     ?              ?      ?      ?      ¨¦*/
    0x0394,0x005F,0x03A6,0x0393,0x039B,0x03A9,0x03A0,0x03A8,0x03A3,0x0398,0x039E,CDAA,0x00C6,0x00E6,0x00DF,0x00C9,    
    /*<SP>   !      "      #      ¡è     %      &      '      (      )      *      +      ,      -      .      / */
    0x0020,0x0021,0x0022,0x0023,0x00A4,0x0025,0x0026,0x0027,0x0028,0x0029,0x002A,0x002B,0x002C,0x002D,0x002E,0x002F,
    /*0      1      2      3      4      5      6      7      8      9      :      ;      <      =      >      ? */
    0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,0x0038,0x0039,0x003A,0x003B,0x003C,0x003D,0x003E,0x003F,
    /*?      A      B      C      D      E      F      G      H      I      J      K      L      M      N      O*/
    0x00A1,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,0x0048,0x0049,0x004A,0x004B,0x004C,0x004D,0x004E,0x004F,
    /*P      Q      R      S      T      U      V      W      X      Y      Z      ?      ?      ?      ¨¹     ¡ì*/
    0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,0x0058,0x0059,0x005A,0x00C4,0x00D6,0x00D1,0x00DC,0x00A7,
    /*?      a      b      c      d      e      f      g      h      i      j      k      l      m      n      o*/
    0x00BF,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006A,0x006B,0x006C,0x006D,0x006E,0x006F,
    /*p      q      r      s      t      u      v      w      x      y      z      ?      ?      ?      ¨¹     ¨¤*/
    0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007A,0x00E4,0x00F6,0x00F1,0x00FC,0x00E0  
};

//<2015/06/04-ROTVG00006-P08-Vicent.Gao,<[SMS] Add support for CharSet: IRA.>
#define CDAB  (IRA_CHAR_DEFAULT)

static u8 sg_aAlphaDefaultExtendCharsIRAMap[ALPHA_DEFAULT_EXTEND_CHAR_COUNT] = 
{
    /*<FF>   ^      {      }      \  */
    0x0C,    0x5E,  0x7B,  0x7D,  0x5C,
    /*[      ~      ]      |      €  */
    0x5B,    0x7E,  0x5D,  0x7C,  CDAB
};

static u8 sg_aAlphaDefaultCharsIRAMap[ALPHA_DEFAULT_CHAR_COUNT] = 
{
    /*@      ?      $      ?      ¨¨     ¨¦     ¨´     ¨¬     ¨°     ?      <LF>   ?      ?      <CR>   ?      ? */
    0x40,  CDAB,  0x24,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  0x0A,  CDAB,  CDAB,  0x0D,  CDAB,  CDAB,
    /*¦¤     _      ¦µ     ¦£     ¦«     ¦¸     ¦°     ¦·     ¦²     ¦¨     ¦®     <NUL>  ?      ?      ?      ? */
    CDAB,  0x5F,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,
    /*<SP>   !      "      #      ¡è     %      &      '      (      )      *      +      ,      -      .      / */
    0x20,  0x21,  0x22,  0x23,  CDAB,  0x25,  0x26,  0x27,  0x28,  0x29,  0x2A,  0x2B,  0x2C,  0x2D,  0x2E,  0x2F,
    /*0      1      2      3      4      5      6      7      8      9      :      ;      <      =      >      ? */
    0x30,  0x31,  0x32,  0x33,  0x34,  0x35,  0x36,  0x37,  0x38,  0x39,  0x3A,  0x3B,  0x3C,  0x3D,  0x3E,  0x3F,
    /*?      A      B      C      D      E      F      G      H      I      J      K      L      M      N      O*/
    CDAB,  0x41,  0x42,  0x43,  0x44,  0x45,  0x46,  0x47,  0x48,  0x49,  0x4A,  0x4B,  0x4C,  0x4D,  0x4E,  0x4F,
    /*P      Q      R      S      T      U      V      W      X      Y      Z      ?      ?      ?      ¨¹     ¡ì*/
    0x50,  0x51,  0x52,  0x53,  0x54,  0x55,  0x56,  0x57,  0x58,  0x59,  0x5A,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,
    /*?      a      b      c      d      e      f      g      h      i      j      k      l      m      n      o */
    CDAB,  0x61,  0x62,  0x63,  0x64,  0x65,  0x66,  0x67,  0x68,  0x69,  0x6A,  0x6B,  0x6C,  0x6D,  0x6E,  0x6F,
    /*p      q      r      s      t      u      v      w      x      y      z      ?      ?      ?      ¨¹     ¨¤*/
    0x70,  0x71,  0x72,  0x73,  0x74,  0x75,  0x76,  0x77,  0x78,  0x79,  0x7A,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB
};

static u8 sg_aCharSetIRACharsGSMMap[CHARSET_IRA_CHAR_COUNT] = 
{
    /*?      ?      ?      ?      ?      ?      ?      ?      ?      ?      <LF>   ?      ?      <CR>   ?      ? */
    CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  0x0A,  CDAB,  CDAB,  0x0D,  CDAB,  CDAB,
    /*?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ? */
    CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB,
    /*<SP>   !      "      #      $      %      &      '      (      )      *      +      ,      -      .      / */
    0x20,  0x21,  0x22,  0x23,  0x02,  0x25,  0x26,  0x27,  0x28,  0x29,  0x2A,  0x2B,  0x2C,  0x2D,  0x2E,  0x2F,
    /*0      1      2      3      4      5      6      7      8      9      :      ;      <      =      >      ? */
    0x30,  0x31,  0x32,  0x33,  0x34,  0x35,  0x36,  0x37,  0x38,  0x39,  0x3A,  0x3B,  0x3C,  0x3D,  0x3E,  0x3F,
    /*@      A      B      C      D      E      F      G      H      I      J      K      L      M      N      O */
    0x00,  0x41,  0x42,  0x43,  0x44,  0x45,  0x46,  0x47,  0x48,  0x49,  0x4A,  0x4B,  0x4C,  0x4D,  0x4E,  0x4F,
    /*P      Q      R      S      T      U      V      W      X      Y      Z      [      \      ]      ^      _ */
    0x50,  0x51,  0x52,  0x53,  0x54,  0x55,  0x56,  0x57,  0x58,  0x59,  0x5A,  CDAB,  CDAB,  CDAB,  CDAB,  0x11,
    /*`      a      b      c      d      e      f      g      h      i      j      k      l      m      n      o */
    CDAB,  0x61,  0x62,  0x63,  0x64,  0x65,  0x66,  0x67,  0x68,  0x69,  0x6A,  0x6B,  0x6C,  0x6D,  0x6E,  0x6F,
    /*p      q      r      s      t      u      v      w      x      y      z      {      |      }      ~      ¨¤*/
    0x70,  0x71,  0x72,  0x73,  0x74,  0x75,  0x76,  0x77,  0x78,  0x79,  0x7A,  CDAB,  CDAB,  CDAB,  CDAB,  CDAB
};
//>2015/06/04-ROTVG00006-P08-Vicent.Gao

//<2015/06/05-ROTVG00006-P09-Vicent.Gao,<[SMS] Add support for CharSet: 8859-1.>
#define CDAC  (ISO88591_CHAR_DEFAULT)

static u8 sg_aAlphaDefaultExtendChars88591Map[ALPHA_DEFAULT_EXTEND_CHAR_COUNT] = 
{
    /*<FF>   ^      {      }      \  */
    0x0C,    0x5E,  0x7B,  0x7D,  0x5C,
    /*[      ~      ]      |      €  */
    0x5B,    0x7E,  0x5D,  0x7C,  CDAC
};

static u8 sg_aAlphaDefaultChars88591Map[ALPHA_DEFAULT_CHAR_COUNT] = 
{
    /*@      ?      $      ?      ¨¨     ¨¦     ¨´     ¨¬     ¨°     ?      <LF>   ?      ?      <CR>   ?      ? */
    0x40,  0xA3,  0x24,  0xA5,  0xE8,  0xE9,  0xF9,  0xEC,  0xF2,  0xC7,  0x0A,  0xD8,  0xF8,  0x0D,  0xC5,  0xE5,
    /*¦¤     _      ¦µ     ¦£     ¦«     ¦¸     ¦°     ¦·     ¦²     ¦¨     ¦®     <NUL>  ?      ?      ?      ? */
    CDAC,  0x5F,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  0xC6,  0xE6,  0xDF,  0xC9,
    /*<SP>   !      "      #      ¡è     %      &      '      (      )      *      +      ,      -      .      / */
    0x20,  0x21,  0x22,  0x23,  0xA4,  0x25,  0x26,  0x27,  0x28,  0x29,  0x2A,  0x2B,  0x2C,  0x2D,  0x2E,  0x2F,
    /*0      1      2      3      4      5      6      7      8      9      :      ;      <      =      >      ? */
    0x30,  0x31,  0x32,  0x33,  0x34,  0x35,  0x36,  0x37,  0x38,  0x39,  0x3A,  0x3B,  0x3C,  0x3D,  0x3E,  0x3F,
    /*?      A      B      C      D      E      F      G      H      I      J      K      L      M      N      O*/
    0xA1,  0x41,  0x42,  0x43,  0x44,  0x45,  0x46,  0x47,  0x48,  0x49,  0x4A,  0x4B,  0x4C,  0x4D,  0x4E,  0x4F,
    /*P      Q      R      S      T      U      V      W      X      Y      Z      ?      ?      ?      ¨¹     ¡ì*/
    0x50,  0x51,  0x52,  0x53,  0x54,  0x55,  0x56,  0x57,  0x58,  0x59,  0x5A,  0xC4,  0xD6,  0xD1,  0xDC,  0xA7,
    /*?      a      b      c      d      e      f      g      h      i      j      k      l      m      n      o */
    0xBF,  0x61,  0x62,  0x63,  0x64,  0x65,  0x66,  0x67,  0x68,  0x69,  0x6A,  0x6B,  0x6C,  0x6D,  0x6E,  0x6F,
    /*p      q      r      s      t      u      v      w      x      y      z      ?      ?      ?      ¨¹     ¨¤*/
    0x70,  0x71,  0x72,  0x73,  0x74,  0x75,  0x76,  0x77,  0x78,  0x79,  0x7A,  0xE4,  0xF6,  0xF1,  0xFC,  0xE0
};

static u8 sg_aCharSet88591CharsGSMMap[CHARSET_88591_CHAR_COUNT] = 
{
    /*?      ?      ?      ?      ?      ?      ?      ?      ?      ?      <LF>   ?      ?      <CR>   ?      ? */
    CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  0x0A,  CDAC,  CDAC,  0x0D,  CDAC,  CDAC,
    /*?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ? */
    CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,
    /*<SP>   !      "      #      $      %      &      '      (      )      *      +      ,      -      .      / */
    0x20,  0x21,  0x22,  0x23,  0x02,  0x25,  0x26,  0x27,  0x28,  0x29,  0x2A,  0x2B,  0x2C,  0x2D,  0x2E,  0x2F,
    /*0      1      2      3      4      5      6      7      8      9      :      ;      <      =      >      ? */
    0x30,  0x31,  0x32,  0x33,  0x34,  0x35,  0x36,  0x37,  0x38,  0x39,  0x3A,  0x3B,  0x3C,  0x3D,  0x3E,  0x3F,
    /*@      A      B      C      D      E      F      G      H      I      J      K      L      M      N      O */
    0x00,  0x41,  0x42,  0x43,  0x44,  0x45,  0x46,  0x47,  0x48,  0x49,  0x4A,  0x4B,  0x4C,  0x4D,  0x4E,  0x4F,
    /*P      Q      R      S      T      U      V      W      X      Y      Z      [      \      ]      ^      _ */
    0x50,  0x51,  0x52,  0x53,  0x54,  0x55,  0x56,  0x57,  0x58,  0x59,  0x5A,  CDAC,  CDAC,  CDAC,  CDAC,  0x11,
    /*`      a      b      c      d      e      f      g      h      i      j      k      l      m      n      o */
    CDAC,  0x61,  0x62,  0x63,  0x64,  0x65,  0x66,  0x67,  0x68,  0x69,  0x6A,  0x6B,  0x6C,  0x6D,  0x6E,  0x6F,
    /*p      q      r      s      t      u      v      w      x      y      z      {      |      }      ~      ¨¤*/
    0x70,  0x71,  0x72,  0x73,  0x74,  0x75,  0x76,  0x77,  0x78,  0x79,  0x7A,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,
    /*?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ? */
    CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,
    /*?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ?      ? */
    CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,
    /*<NBSP> ?      ?      ?      ¡è     ?      ?      ¡ì     ¡§     ?      ?      ?      ?      <SHY>       */
    CDAC,  0x40,  CDAC,  0x01,  0x24,  0x03,  CDAC,  0x5F,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,
    /*¡ã     ¡À     ?      ?      ?      ?      ¡¤     ?      ?      ?      ?      ?      ?      ?      ?      ? */
    CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  CDAC,  0x60,
    /*                                                                           */
    0x41,  0x41,  0x41,  0x41,  0x5B,  0x0E,  0x1C,  0x09,  0x45,  0x1F,  0x45,  0x45,  0x49,  0x49,  0x49,  0x49,
    /*                                   ¡Á                                        */
    CDAC,  0x5D,  0x4F,  0x4F,  0x4F,  0x4F,  0x5C,  CDAC,  0x0B,  0x55,  0x55,  0x55,  0x5E,  0x59,  CDAC,  0x1E,
    /*¨¤    ¨¢                                    ¨¨     ¨¦     ¨º          ¨¬     ¨ª          */
    0x7F,  0x61,  0x61,  0x61,  0x7B,  0x0F,  0x1D,  0x09,  0x04,  0x05,  0x65,  0x65,  0x07,  0x69,  0x69,  0x69,
    /*          ¨°     ¨®                    ¡Â         ¨´      ¨²          ¨¹               */
    CDAC,  0x7D,  0x08,  0x6F,  0x6F,  0x6F,  0x7C,  CDAC,  0x0C,  0x06,  0x75,  0x75,  0x7E,  0x79,  CDAC,  0x79
};
//>2015/06/05-ROTVG00006-P09-Vicent.Gao

/////////////////////////////////////////////////////////////
// MACRO FUNCTION DEFINITIONS
/////////////////////////////////////////////////////////////
#if DBG_SWITCH == (TRUE)

static char sg_aDbgBuf[640];

#define DBG_TRACE(Buffer,...)   \
    do  \
    {   \
        Ql_memset((Buffer), 0, sizeof(Buffer)); \
        Ql_sprintf((char *)(Buffer),__VA_ARGS__);   \
            \
        if(UART_PORT2 == DBG_PORT) \
        {   \
            Ql_Debug_Trace((char*)(Buffer));    \
        }   \
        else    \
        {   \
            Ql_UART_Write(DBG_PORT,(u8*)(Buffer),Ql_strlen((char*)(Buffer)));  \
        }   \
    }while(0)

#else

#define DBG_TRACE(Buffer,...)

#endif  //#if DBG_SWITCH == (TRUE)

#define DBG_SHOW_MEMORY_HEX_VALUE(Memory,Byte)   \
    do  \
    {   \
        u16 iz = 0; \
        char *pTmpZ = NULL;   \
            \
        pTmpZ = (char*)Ql_MEM_Alloc(((Byte) * 3) + 1); \
        if(NULL == pTmpZ)   \
        {   \
            break;  \
        }   \
            \
        for(iz = 0; iz < ((Byte) * 3); iz += 3)  \
        {   \
            Ql_sprintf((pTmpZ + iz),"%02x ", *(((u8*)(Memory)) + (iz / 3)));    \
        }   \
            \
        DBG_TRACE(sg_aDbgBuf,"%s",pTmpZ);   \
            \
        Ql_MEM_Free(pTmpZ); \
    } while(0)

///////////////////////////////////////////////////////
//Base math functions
///////////////////////////////////////////////////////

#define GET_MIN_VALUE(V1,V2)    (((V1) > (V2)) ? (V2) : (V1))
#define GET_MAX_VALUE(V1,V2)    (((V1) > (V2)) ? (V1) : (V2))

#define GET_SIGN_INTEGER_ABSOLUTE_VALUE(v)  \
(   \
    (((s32)(v)) < 0) ? ((-1) * ((s32)(v))) : ((s32)(v)) \
)

#define MAKE_UNSIGNED_INTEGER_SUBTRACTION(V1,V2)    \
(   \
    (((u32)(V1)) < ((u32)(V2))) ? 0 : (((u32)(V1)) - ((u32)(V2)))   \
)

///////////////////////////////////////////////////////
//Base PACKET/UNPACKET/REVERSE functions between base data type
///////////////////////////////////////////////////////

#define SEMI_PACKET_TO_U8(High,Low)    \
(   \
    (((u8)(High)) << 4) | (((u8)(Low)) & 0x0F) \
)

#define U8_PACKET_TO_U16(High,Low) \
(   \
    (((u16)(High)) << 8) | (((u16)(Low)) & 0xFF) \
)

#define U8_UNPACKET_TO_SEMI(Value,High,Low)    \
    do  \
    {   \
        (High) = (((u8)(Value)) >> 4); \
        (Low) = (((u8)(Value)) & 0x0F);    \
    } while(0)

#define U16_UNPACKET_TO_U8(Value,High,Low)    \
    do  \
    {   \
        (High) = (((u16)(Value)) >> 8); \
        (Low) = (((u16)(Value)) & 0xFF); \
    } while(0)

#define U8_SEMI_REVERSE(Value,Result) \
(   \
    (Result) = ((((u8)(Value)) >> 4) | (((u8)(Value)) << 4)) \
)

#define U16_BYTE_REVERSE(Value,Result)  \
(   \
    (Result) = ((((u16)(Value)) >> 8) | (((u16)(Value)) << 8))  \
)

///////////////////////////////////////////////////////
//Base functions about HEX_CHAR <--> data type
///////////////////////////////////////////////////////

#define IS_VALID_DIGIT_OCT(DigitOct)  \
(   \
    (   \
           (((DigitOct) >= 0) && ((DigitOct) <= 9)) \
    ) ? TRUE : FALSE    \
)

#define IS_VALID_HEX_CHAR(HexChar) \
(  \
    (   \
           (((HexChar) >= LIB_SMS_CHAR_A) && ((HexChar) <= LIB_SMS_CHAR_F)) \
        || (((HexChar) >= LIB_SMS_CHAR_a) && ((HexChar) <= LIB_SMS_CHAR_f)) \
        || (((HexChar) >= LIB_SMS_CHAR_0) && ((HexChar) <= LIB_SMS_CHAR_9)) \
    ) ? TRUE : FALSE    \
)

#define CONV_HEX_CHAR_TO_INT(HexChar,Integer)  \
    do  \
    {   \
        if(((HexChar) >= LIB_SMS_CHAR_A) && ((HexChar) <= LIB_SMS_CHAR_F))  \
        {   \
            (Integer) = ((HexChar) - LIB_SMS_CHAR_A + 10);  \
        }   \
        else if(((HexChar) >= LIB_SMS_CHAR_a) && ((HexChar) <= LIB_SMS_CHAR_f)) \
        {   \
            (Integer) = ((HexChar) - LIB_SMS_CHAR_a + 10);  \
        }   \
        else if(((HexChar) >= LIB_SMS_CHAR_0) && ((HexChar) <= LIB_SMS_CHAR_9)) \
        {   \
            (Integer) = ((HexChar) - LIB_SMS_CHAR_0 + 0);   \
        }   \
        else    \
        {   \
            (Integer) = 0;  \
        }   \
    } while(0)

#define CONV_INT_TO_HEX_CHAR(Integer,HexChar)   \
    do  \
    {   \
        if(((Integer) >= 0x00) && ((Integer) <= 0x09))  \
        {   \
            (HexChar) = (((Integer) - 0x00) + LIB_SMS_CHAR_0);  \
        }   \
        else if(((Integer) >= 0x0A) && ((Integer) <= 0x0F)) \
        {   \
            (HexChar) = (((Integer) - 0x0A) + LIB_SMS_CHAR_A);  \
        }   \
        else    \
        {   \
            (HexChar) = LIB_SMS_CHAR_F; \
        }   \
    } while(0)

#define HEX_CHAR_PACKET_TO_U8(High,Low,Value) \
    do  \
    {   \
        u8 uHighTmpZA = 0;   \
        u8 uLowTmpZA = 0;    \
            \
        CONV_HEX_CHAR_TO_INT(High,uHighTmpZA);  \
        CONV_HEX_CHAR_TO_INT(Low,uLowTmpZA);    \
            \
        (Value) = SEMI_PACKET_TO_U8(uHighTmpZA,uLowTmpZA);  \
    } while(0)

#define U8_UNPACKET_TO_HEX_CHAR(Value,Char0,Char1)  \
    do  \
    {   \
        u8 uHighTmpZA = 0;  \
        u8 uLowTmpZA = 0;   \
            \
        U8_UNPACKET_TO_SEMI((Value),uHighTmpZA,uLowTmpZA);  \
        CONV_INT_TO_HEX_CHAR(uHighTmpZA,(Char0)); \
        CONV_INT_TO_HEX_CHAR(uLowTmpZA,(Char1));  \
    } while(0)

#define U16_UNPACKET_TO_HEX_CHAR(Value,Char0,Char1,Char2,Char3) \
    do  \
    {   \
        u8 uHighTmpZB = 0;   \
        u8 uLowTmpZB = 0;    \
            \
        U16_UNPACKET_TO_U8(Value,uHighTmpZB,uLowTmpZB); \
            \
        U8_UNPACKET_TO_HEX_CHAR(uHighTmpZB,(Char0),(Char1));    \
        U8_UNPACKET_TO_HEX_CHAR(uLowTmpZB,(Char2),(Char3)); \
    } while(0)

///////////////////////////////////////////////////////
//Base functions about BCD Byte <--> integer
///////////////////////////////////////////////////////

#define CONV_BCD_BYTE_TO_INT(BCDByte,Integer)  \
    do  \
    {   \
        u8 uTmpValZA = 0;    \
        u8 uHighTmpZA = 0;  \
        u8 uLowTmpZA = 0;   \
            \
        U8_SEMI_REVERSE((BCDByte),uTmpValZA);    \
        U8_UNPACKET_TO_SEMI(uTmpValZA,uHighTmpZA,uLowTmpZA);    \
            \
        (Integer) = ((uHighTmpZA * 10) + uLowTmpZA);    \
    } while(0)

#define CONV_INT_TO_BCD_BYTE(Integer,BCDByte)   \
    do  \
    {   \
        u8 uHighTmpZA = 0;  \
        u8 uLowTmpZA = 0;   \
            \
        uHighTmpZA = (((u8)(Integer)) % 10);  \
        uLowTmpZA = (((u8)(Integer)) / 10);   \
            \
        (BCDByte) = SEMI_PACKET_TO_U8(uHighTmpZA,uLowTmpZA);    \
    } while(0)

#define COUNT_BCD_LEN_TO_BYTE(BCDLen)   \
(   \
    ((BCDLen) % 2) ? (((BCDLen) / 2) + 1) : ((BCDLen) / 2)  \
)
    
#define COUNT_SEPTET_TO_OCTET(v)  ((((v) * 7) % 8) ? ((((v) * 7) / 8) + 1) : (((v) * 7) / 8))
#define COUNT_OCTET_TO_SEPTET(v)  (((v) * 8) / 7)

/////////////////////////////////////////////////////
//SMS Library supported parameters
/////////////////////////////////////////////////////

#define IS_SUPPORT_PID(GivenPID)   \
    (   \
        (   \
            (0x00 == (GivenPID))   \
         || (0x40 == (GivenPID))    \
        ) ? TRUE : FALSE    \
    )

#define  IS_SUPPORT_VPF(GivenVPF)  \
    (   \
        (   \
            (LIB_SMS_VPF_TYPE_NOT_PRESENT == (GivenVPF))    \
         || (LIB_SMS_VPF_TYPE_RELATIVE == (GivenVPF))   \
         || (LIB_SMS_VPF_TYPE_ABSOLUTE == (GivenVPF))   \
        ) ? TRUE : FALSE    \
    )
   
#define IS_SUPPORT_DCS_MSG_TYPE(DCSMsgType)    \
    (   \
        (   \
            (DCS_MSG_TYPE_DATA_CODING_IND == (DCSMsgType))  \
        ) ? TRUE : FALSE    \
    )

#define IS_SUPPORT_DCS_COMPRESS(DCSCompress)    \
    (   \
        (   \
            (LIB_SMS_DCS_COMP_UNCOMPRESSED == (DCSCompress))  \
        ) ? TRUE : FALSE    \
    )
    
#define IS_SUPPORT_PHONE_NUMBER_TYPE(NumberType)    \
    (   \
        (   \
                (LIB_SMS_PHONE_NUMBER_TYPE_INTERNATIONAL == (NumberType))   \
            ||  (LIB_SMS_PHONE_NUMBER_TYPE_NATIONAL == (NumberType))    \
            ||  (LIB_SMS_PHONE_NUMBER_TYPE_UNKNOWN == (NumberType)) \
        ) ? TRUE : FALSE    \
    )

#define IS_SUPPORT_ALPHA(Alphabet)  \
    (   \
        (   \
                (LIB_SMS_DCS_ALPHA_DEFAULT == (Alphabet))   \
            ||  (LIB_SMS_DCS_ALPHA_8BIT_DATA == (Alphabet)) \
            ||  (LIB_SMS_DCS_ALPHA_UCS2 == (Alphabet))  \
        ) ? TRUE : FALSE    \
    )

#define IS_SUPPORT_USER_DATA_TYPE(UDType)    \
    (   \
        (   \
                (LIB_SMS_UD_TYPE_NORMAL == (UDType))  \
            ||  (LIB_SMS_UD_TYPE_CON_6_BYTE == (UDType))  \
            ||  (LIB_SMS_UD_TYPE_CON_7_BYTE == (UDType))  \
        ) ? TRUE : FALSE    \
    )

#define IS_VALID_DCS_IN_PDU(uDCS,bResult)  \
    do  \
    {   \
        u8 uDCSMsgTypeZ = 0;    \
        u8 uDCSCompressZ = 0;   \
            \
        LIB_SMS_DecodeDCS((uDCS),&uDCSMsgTypeZ,NULL,NULL,&uDCSCompressZ);   \
            \
        if(FALSE == IS_SUPPORT_DCS_MSG_TYPE(uDCSMsgTypeZ))  \
        {   \
            (bResult) = FALSE;  \
            break;  \
        }   \
            \
        if(FALSE == IS_SUPPORT_DCS_COMPRESS(uDCSCompressZ)) \
        {   \
            (bResult) = FALSE;  \
            break;  \
        }   \
            \
        (bResult) = TRUE;   \
    } while(0)

#define IS_VALID_CON_SMS_MSG_TYPE(MsgType)  \
(   \
    (   \
            (LIB_SMS_UD_TYPE_CON_6_BYTE == (MsgType))  \
        ||  (LIB_SMS_UD_TYPE_CON_7_BYTE == (MsgType))  \
    ) ? TRUE : FALSE    \
)

#define IS_VALID_CON_SMS_PARAM_STRUCT_DATA(pConSMSParam,bResult)    \
    do  \
    {   \
        u8 uMsgTypeZ = 0;   \
        u8 uMsgSegZ = 0;    \
        u8 uMsgTotZ = 0;    \
            \
        uMsgTypeZ = ((pConSMSParam)->uMsgType); \
        uMsgSegZ = ((pConSMSParam)->uMsgSeg);   \
        uMsgTotZ = ((pConSMSParam)->uMsgTot);   \
            \
        if(FALSE == IS_VALID_CON_SMS_MSG_TYPE(uMsgTypeZ))   \
        {   \
            (bResult) = FALSE;  \
            break;  \
        }   \
            \
        if((uMsgSegZ < 1) || (uMsgSegZ > uMsgTotZ)) \
        {   \
            (bResult) = FALSE;  \
            break;  \
        }   \
            \
        (bResult) = TRUE;   \
    } while(0)

#define SMS_MEM_COPY_ONLY(pSrc,uSrcLen,pDest,pDestLen)   \
    do  \
    {   \
        Ql_memcpy((pDest),(pSrc),(uSrcLen));  \
        (*(pDestLen)) = (uSrcLen);  \
    } while(0)

#define CONV_TO_SUPP_PHONE_NUMBER_TYPE(GivenPhoneNumber,SuppPhoneNumber)    \
    do  \
    {   \
        if(((GivenPhoneNumber) & 0x70) == 0x50)    \
        {   \
            (SuppPhoneNumber) = LIB_SMS_PHONE_NUMBER_TYPE_ALPHANUMERIC;   \
        }   \    
        else if(FALSE == IS_SUPPORT_PHONE_NUMBER_TYPE((GivenPhoneNumber)))   \
        {   \
            (SuppPhoneNumber) = LIB_SMS_PHONE_NUMBER_TYPE_UNKNOWN; \
        }   \
        else    \
        {   \
            (SuppPhoneNumber) = (GivenPhoneNumber);  \
        }   \
    } while(0)

#define GET_VALIDITY_PERIOD_BYTE(VPF,Bytes)    \
    do  \
    {   \
        if(LIB_SMS_VPF_TYPE_RELATIVE == (VPF))  \
        {   \
            (Bytes) = 1;    \
        }   \
        else if(LIB_SMS_VPF_TYPE_ABSOLUTE == (VPF)) \
        {   \
            (Bytes) = SCTS_FIXED_BYTE_LEN;  \
        }   \
        else    \
        {   \
            (Bytes) = 0;    \
        }   \
    } while(0)

#define GET_USER_DATA_HEADER_BYTE(MsgType,Bytes)  \
    do  \
    {   \
        if(LIB_SMS_UD_TYPE_CON_6_BYTE == (MsgType))    \
        {   \
            (Bytes) = 6;  \
        }   \
        else if(LIB_SMS_UD_TYPE_CON_7_BYTE == (MsgType))   \
        {   \
            (Bytes) = 7;  \
        }   \
        else    \
        {   \
            (Bytes) = 0;  \
        }   \
    } while(0)

#define GET_USER_DATA_HEADER_LEN(MsgType,Alpha,UDHLen)  \
    do  \
    {   \
        u8 uByteTmpZA = 0;    \
        \
        GET_USER_DATA_HEADER_BYTE(MsgType,uByteTmpZA);    \
        \
        if(LIB_SMS_DCS_ALPHA_DEFAULT == (Alpha))    \
        {   \
            (UDHLen) = COUNT_OCTET_TO_SEPTET(uByteTmpZA); \
        }   \
        else if(LIB_SMS_DCS_ALPHA_UCS2 == (Alpha))  \
        {   \
            (UDHLen) = uByteTmpZA;    \
        }   \
        else    \
        {   \
            (UDHLen) = uByteTmpZA;    \
        }   \
    } while(0)

#define GET_USER_DATA_BODY_BYTE(Alpha,MsgType,UDBLen,UDBByte) \
    do  \
    {   \
        if((LIB_SMS_UD_TYPE_CON_6_BYTE == (MsgType)) && (LIB_SMS_DCS_ALPHA_DEFAULT == (Alpha)))   \
        {   \
            if((UDBLen) > 0) \
            {   \
                (UDBByte) = 1 + COUNT_SEPTET_TO_OCTET(((UDBLen) - 1)); \
            }   \
            else    \
            {   \
                (UDBByte) = 0;    \
            }   \
        }   \
        else if(LIB_SMS_DCS_ALPHA_DEFAULT == (Alpha)) \
        {   \
            (UDBByte) = COUNT_SEPTET_TO_OCTET((UDBLen));   \
        }   \
        else    \
        {   \
            (UDBByte) = (UDBLen);  \
        }   \
    } while(0)

#define GET_USER_DATA_LEN(Alpha,MsgType,UDBLen,UDLen)   \
    do  \
    {   \
        u8 uByteTmpZA = 0;   \
            \
        GET_USER_DATA_HEADER_BYTE((MsgType),uByteTmpZA); \
            \
        if((LIB_SMS_UD_TYPE_CON_6_BYTE == (MsgType)) && (LIB_SMS_DCS_ALPHA_DEFAULT == (Alpha)))  \
        {   \
            if((UDBLen) > 0)    \
            {   \
                (UDLen) = (COUNT_OCTET_TO_SEPTET(uByteTmpZA) + 1 + (UDBLen));    \
            }   \
            else    \
            {   \
                (UDLen) = COUNT_OCTET_TO_SEPTET(uByteTmpZA); \
            }   \
        }   \
        else if(LIB_SMS_DCS_ALPHA_DEFAULT == (Alpha)) \
        {   \
            (UDLen) = (COUNT_OCTET_TO_SEPTET(uByteTmpZA) + (UDBLen));    \
        }   \
        else    \
        {   \
            (UDLen) = ((uByteTmpZA) + (UDBLen));    \
        }   \
    } while(0)

#define GET_USER_DATA_BYTE(Alpha,UDLen,UDByte) \
    do  \
    {   \
        if(LIB_SMS_DCS_ALPHA_DEFAULT == (Alpha))    \
        {   \
            (UDByte) = COUNT_SEPTET_TO_OCTET((UDLen));  \
        }   \
        else if(LIB_SMS_DCS_ALPHA_UCS2 == (Alpha))  \
        {   \
            (UDByte) = (UDLen); \
        }   \
        else    \
        {   \
            (UDByte) = (UDLen); \
        }   \
    } while(0)

#define GET_USER_DATA_MAX_LEN(Alpha,UDLen)   \
    do  \
    {   \
        if(LIB_SMS_DCS_ALPHA_DEFAULT == (Alpha))    \
        {   \
            (UDLen) = COUNT_OCTET_TO_SEPTET(PDU_UD_MAX_BYTE_LEN);  \
        }   \
        else if(LIB_SMS_DCS_ALPHA_UCS2 == (Alpha))  \
        {   \
            (UDLen) = PDU_UD_MAX_BYTE_LEN; \
        }   \
        else    \
        {   \
            (UDLen) = PDU_UD_MAX_BYTE_LEN; \
        }   \
    } while(0)

#define GET_CON_SMS_FLAG_VAL(uFO,bConSmsFlag)   \
    do  \
    {   \
        if(0x00 != LIB_SMS_GET_UDHI_IN_PDU((uFO)))  \
        {   \
            (bConSmsFlag) = TRUE;   \
        }   \
        else    \
        {   \
            (bConSmsFlag) = FALSE;  \
        }   \
    } while(0)

#define GET_BUFFER_REST_BYTE(TotBytes,HeaderAddr,NowAddr,RestByte)   \
    do  \
    {   \
        if((NowAddr) < (HeaderAddr))    \
        {   \
            (RestByte) = 0; \
        }   \
        else if(((NowAddr) - (HeaderAddr)) > (TotBytes))    \
        {   \
            (RestByte) = 0; \
        }   \
        else    \
        {   \
            (RestByte) = ((TotBytes) - ((NowAddr) - (HeaderAddr))); \
        }   \
    } while(0)

/////////////////////////////////////////////////////
//Functions about Alpha <----> CharSet
/////////////////////////////////////////////////////

#define GET_UCS2_EXTEND_CHAR_MAP_INDEX(UniChar,Index,Result)    \
    do  \
    {   \
        u8 iza = 0; \
        \
        for(iza = 0; iza < ALPHA_DEFAULT_EXTEND_CHAR_COUNT; iza++) \
        {   \
            if((UniChar) == sg_aAlphaDefaultExtendCharsUCS2Map[iza]) \
            {   \
                break;  \
            }   \
        }   \
        \
        if(iza >= ALPHA_DEFAULT_EXTEND_CHAR_COUNT)   \
        {   \
            (Result) = FALSE;   \
            break;  \
        }   \
        \
        (Index) = iza;  \
        (Result) = TRUE;    \
    } while(0)

#define GET_ALPHA_DEFAULT_TO_CHARSET_LEN(CharSet,AlphaData,AlphaLen,LimitLen) \
    do  \
    {   \
        u32 iz = 0; \
        \
        if(LIB_SMS_CHARSET_GSM == (CharSet))    \
        {   \
            (LimitLen) = (AlphaLen);    \
        }   \
        else if(LIB_SMS_CHARSET_UCS2 == (CharSet))  \
        {   \
            (LimitLen) = 0; \
            while(iz < (AlphaLen))  \
            {   \
                if(ALPHA_DEFAULT_EXTEND_CHAR_FLAG == ((AlphaData)[iz]))   \
                {   \
                    iz += 2;    \
                }   \
                else    \
                {   \
                    iz += 1;    \
                }   \
                    \
                (LimitLen) += 1;    \
            }   \
                \
            (LimitLen) *= 4;    \
        }   \
        else    \
        {   \
            (LimitLen) = ((AlphaLen) * 2);  \
        }   \
    } while(0)

#define GET_ALPHA_8BIT_TO_CHARSET_LEN(CharSet,AlphaData,AlphaLen,LimitLen) \
    do  \
    {   \
        if(LIB_SMS_CHARSET_GSM == (CharSet))    \
        {   \
            (LimitLen) = (AlphaLen);    \
        }   \
        else if(LIB_SMS_CHARSET_UCS2 == (CharSet))  \
        {   \
             (LimitLen) = ((AlphaLen) * 2); \
        }   \
        else    \
        {   \
            (LimitLen) = ((AlphaLen) * 2);  \
        }   \
    } while(0)

#define GET_CHARSET_UCS2_TO_ALPHA_LEN(Alhpa,CharSetData,CharSetLen,LimitLen) \
    do  \
    {   \
        u16 izb = 0; \
        u16 uTmpZA = 0; \
        bool bResultZ = 0;  \
            \
        if(LIB_SMS_DCS_ALPHA_DEFAULT == (Alhpa))    \
        {   \
            (LimitLen) = 0; \
            for(izb = 0; izb < (CharSetLen); izb += 2)    \
            {   \
                GET_UCS2_EXTEND_CHAR_MAP_INDEX((*((u16*)(((u8*)(CharSetData)) + izb))),uTmpZA,bResultZ);    \
                if(TRUE == bResultZ)    \
                {   \
                    (LimitLen) += 2;    \
                }   \
                else    \
                {   \
                    (LimitLen) += 1;    \
                }   \
            }   \
        }   \
        else if(LIB_SMS_DCS_ALPHA_UCS2 == (Alhpa))  \
        {   \
            (LimitLen) = (CharSetLen);  \
        }   \
        else    \
        {   \
            (LimitLen) = (CharSetLen);  \
        }   \
    } while(0)

#define GET_CHARSET_GSM_TO_ALPHA_LEN(Alhpa,CharSetData,CharSetLen,LimitLen) \
    do  \
    {   \
        u16 iz = 0; \
        \
        if(LIB_SMS_DCS_ALPHA_DEFAULT == (Alhpa))    \
        {   \
            (LimitLen) = (CharSetLen);  \
        }   \
        else if(LIB_SMS_DCS_ALPHA_UCS2 == (Alhpa))  \
        {   \
            (LimitLen) = 0; \
            while(iz < (CharSetLen))    \
            {   \
                if(ALPHA_DEFAULT_EXTEND_CHAR_FLAG == ((CharSetData)[iz]))   \
                {   \
                    iz += 2;    \
                }   \
                else    \
                {   \
                    iz += 1;    \
                }   \
                    \
                (LimitLen) += 1;    \
            }   \
                \
            (LimitLen) *= 2;    \
        }   \
        else    \
        {   \
            (LimitLen) = (CharSetLen);  \
        }   \
    } while(0)

#define CONV_ALPHA_DEFAUTL_CHAR_TO_UCS2(DefaultChar,UCS2Char,Result)    \
    do  \
    {   \
        if((DefaultChar) >= ALPHA_DEFAULT_CHAR_COUNT)   \
        {   \
            (Result) = FALSE;   \
            break;  \
        }   \
            \
        (UCS2Char) = sg_aAlphaDefaultCharsUCS2Map[(DefaultChar)];   \
            \
        (Result) = TRUE;    \
    } while(0)

#define CONV_ALPHA_DEFAUTL_EXTEND_CHAR_TO_UCS2(DefaultEChar,UCS2Char,Result)    \
    do  \
    {   \
        u8 iz = 0;  \
            \
        for(iz = 0; iz < ALPHA_DEFAULT_EXTEND_CHAR_COUNT; iz++) \
        {   \
            if((DefaultEChar) == sg_aAlphaDefaultExtendChars[iz])   \
            {   \
                break;  \
            }   \
        }   \
            \
        if(iz >= ALPHA_DEFAULT_EXTEND_CHAR_COUNT)   \
        {   \
            (Result) = FALSE;   \
            break;  \
        }   \
            \
        (UCS2Char) = sg_aAlphaDefaultExtendCharsUCS2Map[iz];    \
            \
        (Result) = TRUE;    \
    } while(0)

#define CONV_CHARSET_UCS2_TO_DEFAULT_CHAR(UniChar,DefaultChar,Result)   \
    do  \
    {   \
        u16 iz = 0; \
            \
        for(iz = 0; iz < ALPHA_DEFAULT_CHAR_COUNT; iz++)    \
        {   \
            if((UniChar) == sg_aAlphaDefaultCharsUCS2Map[iz])   \
            {   \
                break;  \
            }   \
        }   \
            \
        if(iz >= ALPHA_DEFAULT_CHAR_COUNT)  \
        {   \
            (Result) = FALSE;   \
            break;  \
        }   \
            \
        (DefaultChar) = iz; \
        (Result) = TRUE;    \
    } while(0)

#define CONV_CHARSET_UCS2_TO_DEFAULT_EXTEND_CHAR(UniChar,DefaultEChar,Result)   \
    do  \
    {   \
        u8 uIndexZA = 0;    \
            \
        GET_UCS2_EXTEND_CHAR_MAP_INDEX(UniChar,uIndexZA,(Result));  \
        if(FALSE == (Result))   \
        {   \
            break;  \
        }   \
            \
        (DefaultEChar) = sg_aAlphaDefaultExtendChars[uIndexZA];   \
        (Result) = TRUE;    \
    } while(0)

#define GET_BCD_NUMBER_TO_ASCII_LEN(pBCDNumber,uBCDLen,uLimitLen) \
    do  \
    {   \
        u8 iz = 0;  \
        u8 uHighZ = 0;  \
        u8 uLowZ = 0;   \
            \
        (uLimitLen) = 0;    \
        for(iz = 0; iz < (uBCDLen); iz++)   \
        {   \
            U8_UNPACKET_TO_SEMI((pBCDNumber)[iz],uLowZ,uHighZ); \
            if(0x0F == uHighZ)  \
            {   \
                break;  \
            }   \
                \
            (uLimitLen) += 1;   \
                \
            if(0x0F == uLowZ)   \
            {   \
                break;  \
            }   \
                \
            (uLimitLen) += 1;   \
        }   \
    } while(0)

#define CONV_BCD_OCT_TO_ASCII_CHAR(BCDOct,AsciiChar)    \
    do  \
    {   \
        if(TRUE == IS_VALID_DIGIT_OCT((BCDOct)))  \
        {   \
            (AsciiChar) = ((BCDOct) + LIB_SMS_CHAR_0);  \
        }   \
        else if(0x0A == (BCDOct))   \
        {   \
            (AsciiChar) = LIB_SMS_CHAR_STAR;    \
        }   \
        else if(0x0B == (BCDOct))   \
        {   \
            (AsciiChar) = LIB_SMS_CHAR_POUND;   \
        }   \
        else if(0x0C == (BCDOct))   \
        {   \
            (AsciiChar) = LIB_SMS_CHAR_A;   \
        }   \
        else if(0x0D == (BCDOct))   \
        {   \
            (AsciiChar) = LIB_SMS_CHAR_B;   \
        }   \
        else if(0x0E == (BCDOct))   \
        {   \
            (AsciiChar) = LIB_SMS_CHAR_C;   \
        }   \
        else    \
        {   \
            (AsciiChar) = 0x00; \
        }   \
    } while(0)

#define CONV_ASCII_CHAR_TO_BCD_OCT(AsciiChar,BCDOct)    \
    do  \
    {   \
        if(((AsciiChar) >= LIB_SMS_CHAR_0) && ((AsciiChar) <= LIB_SMS_CHAR_9))  \
        {   \
            (BCDOct) = ((AsciiChar) - LIB_SMS_CHAR_0);  \
        }   \
        else if(((AsciiChar) >= LIB_SMS_CHAR_A) && ((AsciiChar) <= LIB_SMS_CHAR_C)) \
        {   \
            (BCDOct) = ((AsciiChar) - LIB_SMS_CHAR_A + 0x0C);   \
        }   \
        else if(((AsciiChar) >= LIB_SMS_CHAR_a) && ((AsciiChar) <= LIB_SMS_CHAR_c))  \
        {   \
            (BCDOct) = ((AsciiChar) - LIB_SMS_CHAR_a + 0x0C);   \
        }   \
        else if(LIB_SMS_CHAR_STAR == (AsciiChar))   \
        {   \
            (BCDOct) = 0x0A;    \
        }   \
        else if(LIB_SMS_CHAR_POUND == (AsciiChar))  \
        {   \
            (BCDOct) = 0x0B;    \
        }   \
        else    \
        {   \
            (BCDOct) = 0x0F;    \
        }   \
    } while(0)

/////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS ==> Static functions
// NOTE: Static functions ONLY are used internal in this file.
/////////////////////////////////////////////////////////////

/*****************************************************************************
 * FUNCTION
 *  ConvBCDNumberToAscii
 *
 * DESCRIPTION
 *  Convert BCD number to ASCII number
 *  
 * PARAMETERS
 *   <pBCDNumber>  The pointer of BCD number
 *   <uBCDLen>     The length of BCD number
 *   <pNumber>     The pointer of ASCII number
 *   <pNumberLen>  The length of ASCII number
 *
 * RETURNS
 *   VOID
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pNumberLen> to a value which specify the rest bytes of <pNumber> buffer.
 *  3. If this function works SUCCESS,<*pNumberLen> will return the bytes of <pNumber> buffer that has been written.
 *****************************************************************************/
static bool ConvBCDNumberToAscii(u8 *pBCDNumber,u8 uBCDLen, u8 *pNumber, u8 *pNumberLen)
{
    u8 i = 0;
    u8 j = 0;
    u8 uByteHigh = 0;
    u8 uByteLow = 0;
    u8 uLimitLen = 0;
    u8 uTmpVal = 0;

    if((NULL == pBCDNumber) || (NULL == pNumber) || (NULL == pNumberLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvBCDNumberToAscii,FAIL! Parameter is NULL.");
        return FALSE;
    }

    GET_BCD_NUMBER_TO_ASCII_LEN(pBCDNumber,uBCDLen,uLimitLen);
    if((*pNumberLen) < uLimitLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvBCDNumberToAscii,FAIL! (*pNumberLen) is less. (*pNumberLen):%u,uLimitLen:%u",(*pNumberLen),uLimitLen);
        return FALSE;
    }
    
    for(i = 0; i < uBCDLen; i++)
    {
        U8_SEMI_REVERSE(pBCDNumber[i],uTmpVal);
        U8_UNPACKET_TO_SEMI(uTmpVal,uByteHigh,uByteLow);

        if(0x0F == uByteHigh)
        {
            break;
        }

        CONV_BCD_OCT_TO_ASCII_CHAR(uByteHigh,pNumber[j]);
                
        j += 1;

        if(0x0F == uByteLow)
        {
            break;
        }

        CONV_BCD_OCT_TO_ASCII_CHAR(uByteLow,pNumber[j]);
                
        j += 1;
    }
    
    (*pNumberLen) = uLimitLen;
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvBCDNumberToAscii,SUCCESS. uBCDLen:%u,(*pNumberLen):%u",uBCDLen,(*pNumberLen));
    
    return TRUE;
}

//<2015/03/23-ROTVG00006-P04-Vicent.Gao,<[SMS] Segment 4==>Fix issues of RIL SMS LIB.>
/*****************************************************************************
 * FUNCTION
 *  ConvANNumberToAscii
 *
 * DESCRIPTION
 *  Convert Alphanumeric number to ASCII number
 *  
 * PARAMETERS
 *   <pANNumber>  The pointer of Alphanumeric number
 *   <uANLen>     The length of Alphanumeric number
 *   <pNumber>     The pointer of ASCII number
 *   <pNumberLen>  The length of ASCII number
 *
 * RETURNS
 *   VOID
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pNumberLen> to a value which specify the rest bytes of <pNumber> buffer.
 *  3. If this function works SUCCESS,<*pNumberLen> will return the bytes of <pNumber> buffer that has been written.
 *****************************************************************************/
static bool ConvANNumberToAscii(u8 *pANNumber,u8 uANLen, u8 *pNumber, u8 *pNumberLen)
{
	u8 uLimitLen = 0;

    if((NULL == pANNumber) || (NULL == pNumber) || (NULL == pNumberLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvANNumberToAscii,FAIL! Parameter is NULL.");
        return FALSE;
    }

	uLimitLen = COUNT_OCTET_TO_SEPTET(uANLen);
    if((*pNumberLen) < uLimitLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvANNumberToAscii,FAIL! (*pNumberLen) is less. (*pNumberLen):%u,uLimitLen:%u",(*pNumberLen),uLimitLen);
        return FALSE;
    }
	
	(*pNumberLen) = DecodeGSM7Bit(pANNumber,uANLen,pNumber,(*pNumberLen));
	
    DBG_TRACE(sg_aDbgBuf,"Enter ConvANNumberToAscii,SUCCESS. uANLen:%u,(*pNumberLen):%u",uANLen,(*pNumberLen));
    
    return TRUE;	
}
//>2015/03/23-ROTVG00006-P04-Vicent.Gao

/*****************************************************************************
 * FUNCTION
 *  ConvAsciiNumberToBCD
 *
 * DESCRIPTION
 *  Convert ASCII number to BCD number
 *  
 * PARAMETERS
 *   <pNumber>     The pointer of ASCII number
 *   <uNumberLen>  The length of ASCII number 
 *   <pBCDNumber>  The pointer of BCD number
 *   <pBCDLen>     The length of BCD number
 *
 * RETURNS
 *   TRUE:   This function works SUCCESS.
 *   FALSE:  This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pBCDLen> to a value which specify the rest bytes of <pBCD> buffer.
 *  3. If this function works SUCCESS,<*pBCDLen> will return the bytes of <pBCD> buffer that has been written.
 *****************************************************************************/
static bool ConvAsciiNumberToBCD(u8 *pNumber,u8 uNumberLen,u8 *pBCD,u16 *pBCDLen)
{
    u8 i = 0;
    u8 uSemiHigh = 0;
    u8 uSemiLow = 0;
    u8 uLimitLen = 0;

    if((NULL == pNumber) || (NULL == pBCD) || (NULL == pBCDLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvAsciiNumberToBCD,FAIL! Parameter is NULL.");
        return FALSE;
    }

    uLimitLen = COUNT_BCD_LEN_TO_BYTE(uNumberLen);
    
    if((*pBCDLen) < uLimitLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvAsciiNumberToBCD,FAIL! (*pBCDLen) is less. uLimitLen: %u",uLimitLen);
        return FALSE;
    }

    //Check the validity of <pNumber>
    for(i = 0; i < uNumberLen; i++)
    {
        if(FALSE == LIB_SMS_IS_VALID_ASCII_NUMBER_CHAR(pNumber[i]))
        {
            DBG_TRACE(sg_aDbgBuf,"Enter ConvAsciiNumberToBCD,FAIL! LIB_SMS_IS_VALID_ASCII_NUMBER_CHAR FAIL! pNumber[i]:0x%x",i,pNumber[i]);
            return FALSE;
        }
    }

    i = 0;
    while(i < uNumberLen)
    {
        if((i+1) < uNumberLen)
        {
            CONV_ASCII_CHAR_TO_BCD_OCT(pNumber[i+1],uSemiHigh);
        }
        else
        {
            uSemiHigh = 0x0F;
        }

        CONV_ASCII_CHAR_TO_BCD_OCT(pNumber[i+0],uSemiLow);
        pBCD[i/2] = SEMI_PACKET_TO_U8(uSemiHigh,uSemiLow);

        i += 2;
    }
    
    (*pBCDLen) = uLimitLen;
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvAsciiNumberToBCD,SUCCESS. (*pBCDLen):%u",(*pBCDLen));    
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  EncodeGSM7Bit
 *
 * DESCRIPTION
 *  This function is used to encode GSM-8bit to GSM-7bit.
 *  
 * PARAMETERS
 *   <pSrc>      The pointer of source buffer
 *   <uSrcLen>   The length of source buffer
 *   <pDest>     The pointer of destination buffer
 *   <uDestLen>  The length of destination buffer
 *
 * RETURNS
 *   0,  This function works FAIL!  
 *   OTHER VALUES, The length of encoded GSM-8bit buffer,unit is byte.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. <uDestLen> is the length of <pDest> buffer, it MUST be larger enough.
 *****************************************************************************/ 
static u16 EncodeGSM7Bit(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 uDestLen)
{
    u16 i = 0;
    u16 uGID = 0;     //Group ID,range from 0 ~ MAX
    u16 uGIdx = 0;    //Group index,range from 0 ~ 7
    u16 uDestPos = 0; //Position of <pDest>
    
    u8 uHeadSize = 0;
    u8 uHead = 0;
    u8 uTailSize = 0;
    u8 uTail = 0;

    u16 uLimitDestLen = 0;  //The limit length of <uDestLen>
    
    //Check the validity of the parameters    
    if((NULL == pSrc) || (0 == uSrcLen) || (NULL == pDest))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeGSM7Bit,FAIL! Parameter is NULL.");
        return 0;
    }
    
    uLimitDestLen = COUNT_SEPTET_TO_OCTET(uSrcLen);
    
    if(uDestLen < uLimitDestLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeGSM7Bit,FAIL! uDestLen is too small! uDestLen:%u,uLimitDestLen:%u,uSrcLen:%u",uDestLen,uLimitDestLen,uSrcLen);
        return 0;
    }    
    
    //Initialize
    Ql_memset(pDest,0x00,uDestLen);        
        
    i = 0;
    while(i < uSrcLen)
    {
        uGID = (i / 8);
        uGIdx = (i % 8);
        
        uDestPos = ((uGID * 7) + uGIdx);
        
        uTailSize = uGIdx;
        uHeadSize = (8 - uGIdx);
        
        uTail = ((~(0xFF << uTailSize)) & pSrc[i]);
        uHead = ((~(0xFF >> uHeadSize)) & pSrc[i]);
        
        uTail <<= uHeadSize;
        uHead >>= uTailSize;
        
        if(uGIdx != 0)
        {
            pDest[uDestPos-1] |= uTail;
        }
        
        if(uGIdx != 7)
        {
            pDest[uDestPos] = uHead;
        }
        
        i++;
    }

    DBG_TRACE(sg_aDbgBuf,"Enter EncodeGSM7Bit,SUCCESS. uDestLen:%u,uLimitDestLen:%u,uSrcLen:%u",uDestLen,uLimitDestLen,uSrcLen);
    
    return uLimitDestLen;
}

/*****************************************************************************
 * FUNCTION
 *  EncodeTimeStamp
 *
 * DESCRIPTION
 *  Encode 'LIB_SMS_TimeStampStruct' data in TPDU
 *  
 * PARAMETERS
 *  <pTimeStamp>    The pointer of 'LIB_SMS_TimeStampStruct' data
 *  <pPDU>    The pointer of PDU buffer
 *  <pPDULen> The pointer of PDU length
 *
 * RETURNS
 *  TRUE:  This function works SUCESS.
 *  FALSE: This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pPDULen> to a value which specify the rest bytes of PDU buffer.
 *  3. If this function works SUCCESS,<*pPDULen> will return the bytes of PDU buffer that has been written.
 *****************************************************************************/
static bool EncodeTimeStamp(LIB_SMS_TimeStampStruct *pTimeStamp,u8 *pPDU,u16 *pPDULen)
{
    u8 uTmpVal = 0;

    if((NULL == pTimeStamp) || (NULL == pPDU) || (NULL == pPDULen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeTimeStamp,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if((*pPDULen) < SCTS_FIXED_BYTE_LEN)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeTimeStamp,FAIL! (*pPDULen) is less. (*pPDULen):%u,Need length:%u",(*pPDULen),SCTS_FIXED_BYTE_LEN);
        return FALSE;
    }
    
    CONV_INT_TO_BCD_BYTE((pTimeStamp->uYear),pPDU[0]);
    CONV_INT_TO_BCD_BYTE((pTimeStamp->uMonth),pPDU[1]);
    CONV_INT_TO_BCD_BYTE((pTimeStamp->uDay),pPDU[2]);
    CONV_INT_TO_BCD_BYTE((pTimeStamp->uHour),pPDU[3]);
    CONV_INT_TO_BCD_BYTE((pTimeStamp->uMinute),pPDU[4]);
    CONV_INT_TO_BCD_BYTE((pTimeStamp->uSecond),pPDU[5]);
    
    uTmpVal = (u8)GET_SIGN_INTEGER_ABSOLUTE_VALUE(pTimeStamp->iTimeZone);
    if(0 != (uTmpVal % 4))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeTimeStamp,FAIL! ABS(iTimeZone) % 4 NOT zero. uTmpVal:%u",uTmpVal);
        return FALSE;
    }
    
    CONV_INT_TO_BCD_BYTE(uTmpVal,pPDU[6]);
    
    if((pTimeStamp->iTimeZone) < 0) //Negative number
    {
        pPDU[6] |= 0x80;
    }
    
    (*pPDULen) = SCTS_FIXED_BYTE_LEN;
    
    DBG_TRACE(sg_aDbgBuf,"Enter EncodeTimeStamp,SUCCESS. pPDU[6]:0x%x,(*pPDULen):%u",pPDU[6],(*pPDULen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  EncodeAddress
 *
 * DESCRIPTION
 *  Encode 'LIB_SMS_PhoneNumberStruct' data in TPDU
 *  
 * PARAMETERS
 *  <pNumber>    The pointer of 'LIB_SMS_PhoneNumberStruct' data
 *  <pPDU>    The pointer of PDU buffer
 *  <pPDULen> The pointer of PDU length
 *
 * RETURNS
 *  TRUE:  This function works SUCESS.
 *  FALSE: This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pPDULen> to a value which specify the rest bytes of PDU buffer.
 *  3. If this function works SUCCESS,<*pPDULen> will return the bytes of PDU buffer that has been written.
 *****************************************************************************/
static bool EncodeAddress(LIB_SMS_PhoneNumberStruct *pNumber,u8 *pPDU,u16 *pPDULen,bool bWTSFlag)
{
    u8 uLimitLen = 0;
    bool bResult = FALSE;
    u8 *pTmp = NULL;
    u16 uTmpLen = 0;

    if((NULL == pNumber) || (NULL == pPDU) || (NULL == pPDULen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeAddress,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if((pNumber->uLen) > sizeof(pNumber->aNumber))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeAddress,FAIL! uLen is too larger. uLen:%d,Max length:%d",(pNumber->uLen),sizeof(pNumber->aNumber));
        return FALSE;
    }
    
    uLimitLen = COUNT_BCD_LEN_TO_BYTE((pNumber->uLen));
    if(!((FALSE == bWTSFlag) && (0 == (pNumber->uLen))))
    {
        uLimitLen += 1;  //Add 1 byte for <Phone number type>
    }
    
    if((*pPDULen) < uLimitLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeAddress,FAIL! (*pPDULen) is too less. (*pPDULen):%u,uLimitLen:%u",(*pPDULen),uLimitLen);
        return FALSE;
    }
    
    if(!((FALSE == bWTSFlag) && (0 == (pNumber->uLen))))
    {
        CONV_TO_SUPP_PHONE_NUMBER_TYPE((pNumber->uType),pPDU[0]);
        pTmp = (pPDU + 1);
        uTmpLen = ((*pPDULen) - 1);
    }    
    else
    {
        pTmp = (pPDU + 0);
        uTmpLen = ((*pPDULen) - 0);
    }
    
    bResult = ConvAsciiNumberToBCD((pNumber->aNumber),(pNumber->uLen),pTmp,&uTmpLen);
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeAddress,FAIL! ConvAsciiNumberToBCD FAIL! (pNumber->uLen):%d,uTmpLen:%d",(pNumber->uLen),uTmpLen);
        return FALSE;
    }
    
    (*pPDULen) = uLimitLen;
    
    DBG_TRACE(sg_aDbgBuf,"Enter EncodeAddress,SUCCESS. (pNumber->uLen):%d,uLimitLen:%d,bWTSFlag:%d",(pNumber->uLen),uLimitLen,bWTSFlag);
        
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  EncodeValidityPeriod
 *
 * DESCRIPTION
 *  Encode <VP> in TPDU
 *  
 * PARAMETERS
 *  <uVPF>    Validity format,it's value is same as 'LIB_SMS_VPFTypeEnum'
 *  <pVP>     The pointer of 'LIB_SMS_ValidityPeriodUnion' data
 *  <pPDU>    The pointer of PDU buffer
 *  <pPDULen> The pointer of PDU length
 *
 * RETURNS
 *  TRUE:  This function works SUCESS.
 *  FALSE: This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pPDULen> to a value which specify the rest bytes of PDU buffer.
 *  3. If this function works SUCCESS,<*pPDULen> will return the bytes of PDU buffer that has been written.
 *****************************************************************************/
static bool EncodeValidityPeriod(u8 uVPF,LIB_SMS_ValidityPeriodUnion *pVP,u8 *pPDU,u16 *pPDULen)
{
    u16 uLimitLen = 0;
    bool bResult = FALSE;

    if((NULL == pVP) || (NULL == pPDU) || (NULL == pPDULen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeValidityPeriod,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if(FALSE == IS_SUPPORT_VPF(uVPF))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeValidityPeriod,FAIL! IS_SUPPORT_VPF FAIL! uVPF:%u",uVPF);
        return FALSE;
    }
    
    GET_VALIDITY_PERIOD_BYTE(uVPF,uLimitLen);
    
    if((*pPDULen) < uLimitLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeValidityPeriod,FAIL! (*pPDULen) is less. uLimitLen:%u,(*pPDULen):%u",uLimitLen,(*pPDULen));
        return FALSE;
    }
    
    if(LIB_SMS_VPF_TYPE_RELATIVE == uVPF)
    {
        (*pPDU) = (pVP->uRelative);
    }
    else if(LIB_SMS_VPF_TYPE_ABSOLUTE == uVPF)
    {
        bResult = EncodeTimeStamp(&(pVP->sAbsolute),pPDU,pPDULen);
        if(FALSE == bResult)
        {
            DBG_TRACE(sg_aDbgBuf,"Enter EncodeValidityPeriod,FAIL! EncodeTimeStamp FAIL!");
            return FALSE;
        }
    }

    (*pPDULen) = uLimitLen;

    DBG_TRACE(sg_aDbgBuf,"Enter EncodeValidityPeriod,SUCCESS. uVPF:%u,(*pPDULen):%u",uVPF,(*pPDULen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  EncodeUserDataHeader
 *
 * DESCRIPTION
 *  Encode <UDH> in TPDU
 *  
 * PARAMETERS
 *  <pConSMSParam>   The pointer of 'LIB_SMS_ConSMSParamStruct' data
 *  <pPDU>        The pointer of PDU buffer
 *  <pPDULen>     The pointer of PDU length
 *
 * RETURNS
 *  TRUE:  This function works SUCESS.
 *  FALSE: This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pPDULen> to a value which specify the rest bytes of PDU buffer.
 *  3. If this function works SUCCESS,<*pPDULen> will return the bytes of PDU buffer that has been written.
 *****************************************************************************/
static bool EncodeUserDataHeader(LIB_SMS_ConSMSParamStruct *pConSMSParam,u8 *pPDU,u16 *pPDULen)
{
    bool bResult = FALSE;
    u8 *pTmp = NULL;
    u16 uLimitLen = 0;

    if((NULL == pConSMSParam) || (NULL == pPDU) || (NULL == pPDULen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeUserDataHeader,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    //Check parameters of <pConSMSParam>
    IS_VALID_CON_SMS_PARAM_STRUCT_DATA(pConSMSParam,bResult);
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeUserDataHeader,FAIL! IS_VALID_CON_SMS_MSG_TYPE FAIL! uMsgType:%u,uMsgSeg:%u,uMsgTot:%u",(pConSMSParam->uMsgType),(pConSMSParam->uMsgSeg),(pConSMSParam->uMsgTot));
        return FALSE;
    }

    GET_USER_DATA_HEADER_BYTE((pConSMSParam->uMsgType),uLimitLen);
    if((*pPDULen) < uLimitLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeUserDataHeader,FAIL! (*pPDULen) is less. (*pPDULen):%d,uLimitLen:%d",(*pPDULen),uLimitLen);
        return FALSE;
    }
    
    pTmp = pPDU;
    if(LIB_SMS_UD_TYPE_CON_6_BYTE == (pConSMSParam->uMsgType))
    {
        (*(pTmp++)) = 0x05;
        (*(pTmp++)) = 0x00;
        (*(pTmp++)) = 0x03;
        
        (*(pTmp++)) = (pConSMSParam->uMsgRef);
        (*(pTmp++)) = (pConSMSParam->uMsgTot);
        (*(pTmp++)) = (pConSMSParam->uMsgSeg);        
    }
    else
    {
        (*(pTmp++)) = 0x06;
        (*(pTmp++)) = 0x08;
        (*(pTmp++)) = 0x04;
        
        U16_UNPACKET_TO_U8((pConSMSParam->uMsgRef),pTmp[0],pTmp[1]);
        pTmp += 2;
        
        (*(pTmp++)) = (pConSMSParam->uMsgTot);
        (*(pTmp++)) = (pConSMSParam->uMsgSeg);
    }
    
    (*pPDULen) = uLimitLen;
    
    DBG_TRACE(sg_aDbgBuf,"Enter EncodeUserDataHeader,SUCCESS. uMsgType:%u,uMsgSeg:%u,uMsgTot:%u,(*pPDULen):%u",(pConSMSParam->uMsgType),(pConSMSParam->uMsgSeg),(pConSMSParam->uMsgTot),(*pPDULen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  EncodeUserDataBody
 *
 * DESCRIPTION
 *  Encode <UDB> in TPDU
 *  
 * PARAMETERS
 *  <uAlpha>    Alphabet,it's value is same as 'LIB_SMS_DCSAlphaEnum'
 *  <uUDType>   User data type,it's value is same as 'LIB_SMS_UDTypeEnum'
 *  <pUserData> The pointer of 'LIB_SMS_UserDataStruct' data
 *  <pPDU>        The pointer of PDU buffer
 *  <pPDULen>     The pointer of PDU length
 *
 * RETURNS
 *  TRUE:  This function works SUCESS.
 *  FALSE: This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pPDULen> to a value which specify the rest bytes of PDU buffer.
 *  3. If this function works SUCCESS,<*pPDULen> will return the bytes of PDU buffer that has been written.
 *****************************************************************************/
static bool EncodeUserDataBody(u8 uAlpha,u8 uUDType,LIB_SMS_UserDataStruct *pUserData,u8 *pPDU,u16 *pPDULen)
{
    u16 uLimitLen = 0;

    if((NULL == pUserData) || (NULL == pPDU) || (NULL == pPDULen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeUserDataBody,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if(FALSE == IS_SUPPORT_ALPHA(uAlpha))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeUserDataBody,FAIL! IS_SUPPORT_ALPHA FAIL! uAlpha:%d",uAlpha);
        return FALSE;
    }

    if(FALSE == IS_SUPPORT_USER_DATA_TYPE(uUDType))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeUserDataBody,FAIL! IS_SUPPORT_USER_DATA_TYPE FAIL! uUDType:%d",uUDType);
        return FALSE;
    }
    
    if((pUserData->uLen) > sizeof(pUserData->aUserData))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeUserDataBody,FAIL! UserData is INVALID. length is too larger. uLen:%d,limit length:%d",(pUserData->uLen),sizeof(pUserData->aUserData));
        return FALSE;
    }
    
    GET_USER_DATA_BODY_BYTE(uAlpha,uUDType,(pUserData->uLen),uLimitLen);
    if((*pPDULen) < uLimitLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter EncodeUserDataBody,FAIL! (*pPDULen) is too small. (*pPDULen):%u,uLimitLen:%u",(*pPDULen),uLimitLen);
        return FALSE;
    }
    
    if((LIB_SMS_UD_TYPE_CON_6_BYTE == uUDType) && (LIB_SMS_DCS_ALPHA_DEFAULT == uAlpha))
    {
        if((pUserData->uLen) > 0)
        {
            pPDU[0] = ((*(pUserData->aUserData)) << 1);
            
            EncodeGSM7Bit(((pUserData->aUserData) + 1),((pUserData->uLen) - 1),(pPDU + 1), ((*pPDULen) - 1));
        }
    }
    else if(LIB_SMS_DCS_ALPHA_DEFAULT == uAlpha)
    {
        EncodeGSM7Bit((pUserData->aUserData),(pUserData->uLen),pPDU,(*pPDULen));
    }
    else
    {
        Ql_memcpy(pPDU,(pUserData->aUserData),(pUserData->uLen));
    }
    
    (*pPDULen) = uLimitLen;
    
    DBG_TRACE(sg_aDbgBuf,"Enter EncodeUserDataBody,SUCCESS. uLen:%u,uLimitLen:%u",(pUserData->uLen),uLimitLen);
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  DecodeGSM7Bit
 *
 * DESCRIPTION
 *  This function is used to decode GSM-7bit to GSM-8bit.
 *  
 * PARAMETERS
 *   <pSrc>      The pointer of source buffer
 *   <uSrcLen>   The length of source buffer
 *   <pDest>     The pointer of destination buffer
 *   <uDestLen>  The length of destination buffer
 *
 * RETURNS
 *   0,  This function works FAIL!  
 *   OTHER VALUES, The length of decoded GSM-7bit buffer,unit is byte.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. <uDestLen> is the length of <pDest> buffer, it MUST be larger enough.
 *****************************************************************************/ 
static u16 DecodeGSM7Bit(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 uDestLen)
{
    u16 i = 0;
    u16 uGID = 0;     //Group ID,range from 0 ~ MAX
    u16 uGIdx = 0;    //Group index,range from 0 ~ 6
    u16 uDestPos = 0; //Position of <pDest>
    
    u8 uHeadSize = 0;
    u8 uHead = 0;
    u8 uTailSize = 0;
    u8 uTail = 0;
    
    u16 uLimitDestLen = 0;  //The limit length of <uDestLen>
    
    //Check the validity of the parameters    
    if((NULL == pSrc) || (0 == uSrcLen) || (NULL == pDest))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeGSM7Bit,FAIL! Parameter is NULL.");
        return 0;
    }
    
    uLimitDestLen = COUNT_OCTET_TO_SEPTET(uSrcLen);
    
    if(uDestLen < uLimitDestLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeGSM7Bit,FAIL! uDestLen is too small! uDestLen:%u,uLimitDestLen:%u,uSrcLen:%u",uDestLen,uLimitDestLen,uSrcLen);
        return 0;
    }
    
    //Initialize
    Ql_memset(pDest,0x00,uDestLen);    
    
    i = 0;
    while(i < uSrcLen)
    {
        uGID = (i / 7);
        uGIdx = (i % 7);
        
        uDestPos = ((uGID * 8) + uGIdx);
        
        uTailSize = (uGIdx + 1);
        uHeadSize = (7 - uGIdx);
        
        uTail = ((~(0xFF >> uTailSize)) & pSrc[i]);
        uHead = ((~(0xFF << uHeadSize)) & pSrc[i]);
        
        uTail >>= uHeadSize;
        uHead <<= (uTailSize-1);
                
        pDest[uDestPos] |= uHead;

        if((uDestPos+1) < uDestLen)
        {
            pDest[uDestPos+1] = uTail;
        }
        
        i++;
    }

    DBG_TRACE(sg_aDbgBuf,"Enter DecodeGSM7Bit,SUCCESS. uDestLen:%u,uLimitDestLen:%u,uSrcLen:%u",uDestLen,uLimitDestLen,uSrcLen);
    
    return uLimitDestLen;
}

/*****************************************************************************
 * FUNCTION
 *  DecodeTimeStamp
 *
 * DESCRIPTION
 *  Decode BCD time stamp element to 'LIB_SMS_TimeStampStruct' data if the data is given.
 *  
 * PARAMETERS
 *   <pSCTS>      The pointer of BCD time stamp element
 *   <uSCTSLen>   The length of BCD time stamp element,NOTE: it MUST be larger than 'SCTS_FIXED_BYTE_LEN'
 *   <pTimeStamp> The pointer of 'LIB_SMS_TimeStampStruct' data
 *
 * RETURNS
 *   TRUE,  This function works SUCCESS.
 *   FALSE, This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *****************************************************************************/ 
static bool DecodeTimeStamp(u8 *pBCDTS,u8 uBCDTSLen,LIB_SMS_TimeStampStruct *pTimeStamp)
{
    u8 i = 0;
    u8 uByteHigh = 0;
    u8 uByteLow = 0;
    s8 iTimeZone = 0;
    u8 uTimeZoneVal = 0;
        
    if((NULL == pBCDTS) || (NULL == pTimeStamp) || (uBCDTSLen < SCTS_FIXED_BYTE_LEN))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeTimeStamp,FAIL! Parameter is INVALID. pBCDTS:0x%x,uBCDTSLen:%u",pBCDTS,uBCDTSLen);
        return FALSE;
    }

    //Check <SCTS> is VALID or not.
    for(i = 0; i < (SCTS_FIXED_BYTE_LEN - 1); i++)
    {
        U8_UNPACKET_TO_SEMI(pBCDTS[i],uByteHigh,uByteLow);

        if(FALSE == IS_VALID_DIGIT_OCT(uByteHigh))
        {
            DBG_TRACE(sg_aDbgBuf,"Enter DecodeTimeStamp,FAIL! IS_DIGIT_OCT FAIL! uByteHigh:0x%x,i:%u",uByteHigh,i);
            return FALSE;
        }

        if(FALSE == IS_VALID_DIGIT_OCT(uByteLow))
        {
            DBG_TRACE(sg_aDbgBuf,"Enter DecodeTimeStamp,FAIL! IS_DIGIT_OCT FAIL! uByteLow:0x%x,i:%u",uByteLow,i);
            return FALSE;
        }        
    }

    //<2016/07/25-ROTVG00006C001-P01-Vicent.Gao, <[COMMON] Segment 1==> Fix common issues>
    //CONV_BCD_BYTE_TO_INT((pBCDTS[SCTS_FIXED_BYTE_LEN - 1] & 0x7F),uTimeZoneVal);
    CONV_BCD_BYTE_TO_INT((pBCDTS[SCTS_FIXED_BYTE_LEN - 1] & 0xF7),uTimeZoneVal);
    //>2016/07/25-ROTVG00006C001-P01-Vicent.Gao
    
//<2015/03/23-ROTVG00006-P04-Vicent.Gao,<[SMS] Segment 4==>Fix issues of RIL SMS LIB.>
#if 1
    //<NULL>
#else //#if 1
    if((uTimeZoneVal % 4) != 0)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeTimeStamp,FAIL! uTimeZoneVal is INVALID. (TimeZoneVal % 4) != 0");
        return FALSE;
    }
#endif //#if 1
//>2015/03/23-ROTVG00006-P04-Vicent.Gao

    //<2016/07/25-ROTVG00006C001-P01-Vicent.Gao, <[COMMON] Segment 1==> Fix common issues>
    //if(pBCDTS[SCTS_FIXED_BYTE_LEN - 1] & 0x80)  //negative time zone
    if(pBCDTS[SCTS_FIXED_BYTE_LEN - 1] & 0x08)  //negative time zone
    //>2016/07/25-ROTVG00006C001-P01-Vicent.Gao
    {
        iTimeZone = (-1) * uTimeZoneVal;
    }
    else  //positive time zone
    {
        iTimeZone = uTimeZoneVal;
    }

    if((iTimeZone < SCTS_TIME_ZONE_MIN) || (iTimeZone > SCTS_TIME_ZONE_MAX))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeTimeStamp,FAIL! iTimeZone is INVALID. iTimeZone:%d",iTimeZone);
        return FALSE;
    }

    //Now decode <SCTS>
    CONV_BCD_BYTE_TO_INT(pBCDTS[0],(pTimeStamp->uYear));
    CONV_BCD_BYTE_TO_INT(pBCDTS[1],(pTimeStamp->uMonth));
    CONV_BCD_BYTE_TO_INT(pBCDTS[2],(pTimeStamp->uDay));
    CONV_BCD_BYTE_TO_INT(pBCDTS[3],(pTimeStamp->uHour));
    CONV_BCD_BYTE_TO_INT(pBCDTS[4],(pTimeStamp->uMinute));
    CONV_BCD_BYTE_TO_INT(pBCDTS[5],(pTimeStamp->uSecond));

    (pTimeStamp->iTimeZone) = iTimeZone;
        
    DBG_TRACE(sg_aDbgBuf,"Enter DecodeTimeStamp,SUCCESS. uBCDTSLen:%u,iTimeZone:%d",uBCDTSLen,iTimeZone);    
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  DecodeAddress
 *
 * DESCRIPTION
 *  Decode address element in TPDU,EX: <SCA>,<OA>,<DA>
 *  
 * PARAMETERS
 *   <pAddress>      The pointer of address element
 *   <uAddrByteLen>  The byte count of address element,NOTE: it contains 1 byte of "BCD Number Type" + Remain bytes of "BCD Number"
 *   <pNumber>       The pointer of 'LIB_SMS_PhoneNumberStruct' number
 *
 * RETURNS
 *   TRUE:  This function works SUCCESS.
 *   FALSE: This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *****************************************************************************/
static bool DecodeAddress(u8 *pAddress,u8 uAddrByteLen,LIB_SMS_PhoneNumberStruct *pNumber)
{
    u8 uBCDByteLen = 0;
    bool bResult = FALSE;

    if((NULL == pAddress) || (NULL == pNumber))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeAddress,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    //Initialize the <pNumber>
    (pNumber->uType) = LIB_SMS_PHONE_NUMBER_TYPE_UNKNOWN;
    Ql_memset((pNumber->aNumber),0x00,sizeof(pNumber->aNumber));
    (pNumber->uLen) = 0;
    
    if(uAddrByteLen < 1)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeAddress,SUCCESS. Note: uAddrByteLen is 0.");
        return TRUE;
    }

    uBCDByteLen = (uAddrByteLen-1);

    if(uBCDByteLen > (LIB_SMS_PHONE_NUMBER_MAX_LEN / 2))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeAddress,FAIL! AddrLen is too larger. AddrLen:%u,LimitLen:%u",(uAddrByteLen-1),(LIB_SMS_PHONE_NUMBER_MAX_LEN / 2));
        return FALSE;
    }

    CONV_TO_SUPP_PHONE_NUMBER_TYPE(pAddress[0],(pNumber->uType));

    (pNumber->uLen) = sizeof(pNumber->aNumber);
//<2015/03/23-ROTVG00006-P04-Vicent.Gao,<[SMS] Segment 4==>Fix issues of RIL SMS LIB.>
#if 1
    if(LIB_SMS_PHONE_NUMBER_TYPE_ALPHANUMERIC == (pNumber->uType))
    {
        bResult = ConvANNumberToAscii(&pAddress[1],uBCDByteLen,(pNumber->aNumber),&(pNumber->uLen));
    }
    else
    {
        bResult = ConvBCDNumberToAscii(&pAddress[1],uBCDByteLen,(pNumber->aNumber),&(pNumber->uLen));
    }
#else //#if 1
    bResult = ConvBCDNumberToAscii(&pAddress[1],uBCDByteLen,(pNumber->aNumber),&(pNumber->uLen));
#endif //#if 1
//>2015/03/23-ROTVG00006-P04-Vicent.Gao
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeAddress,FAIL! Conv AN/BCD number FAIL! uBCDByteLen:%u,uType:%u",uBCDByteLen,(pNumber->uType));
        return FALSE;
    }
    
    DBG_TRACE(sg_aDbgBuf,"Enter DecodeAddress,SUCCESS. uBCDByteLen:%u,uType:%u,uLen:%u",uBCDByteLen,(pNumber->uType),(pNumber->uLen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  DecodeUserDataHeader
 *
 * DESCRIPTION
 *  Decode <UDH> to 'LIB_SMS_ConSMSParam' data if data is present.
 *  
 * PARAMETERS
 *   <pUDH>          The pointer of <UDH> element
 *   <uUDByte>      The bytes length of <UD> element
 *   <pConSMSParam>  The pointer of 'LIB_SMS_ConSMSParam' data
 *
 * RETURNS
 *   TRUE,  This function works SUCCESS.
 *   FALSE, This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *****************************************************************************/ 
static bool DecodeUserDataHeader(u8 *pUDH,u8 uUDByte,LIB_SMS_ConSMSParamStruct *pConSMSParam)
{
    u8 *pTmp = NULL;
    
    u8 uUDHIDByteVal = 0;
        
    if((NULL == pUDH) || (NULL == pConSMSParam))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeUserDataHeader,FAIL! Parameter is NULL.");
        return FALSE;
    }

    pTmp = pUDH;
    
    //Check <UDH> length byte
    if(((*pTmp) != 0x05) && ((*pTmp) != 0x06))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeUserDataHeader,FAIL! INVALID UDH element. UDH length byte: %u",(*pTmp));
        return FALSE;
    }

    if(((*pTmp)+1) > uUDByte)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeUserDataHeader,FAIL! uUDByte is too less. uUDByte:%u,Need byte:%u",uUDByte,((*pTmp)+1));
        return FALSE;
    }

    pTmp += 1;  //Skip <UDH> length byte

    //Check <UDH> ID byte
    uUDHIDByteVal = (*(pTmp++));
    if(UDH_ID_BYTE_VAL_CON_8_BIT == uUDHIDByteVal)
    {
        if(0x03 != (*(pTmp++)))
        {
            DBG_TRACE(sg_aDbgBuf,"Enter DecodeUserDataHeader,FAIL! UDH_ID_BYTE_VAL_CON_8_BIT message.UDH element length NOT 0x03. It's %u",(*(pTmp-1)));
            return FALSE;
        }
        
        (pConSMSParam->uMsgRef) = (*pTmp);
        (pConSMSParam->uMsgType) = LIB_SMS_UD_TYPE_CON_6_BYTE;
        
        pTmp += 1;
    }
    else if(UDH_ID_BYTE_VAL_CON_16_BIT == uUDHIDByteVal)
    {
        if(0x04 != (*(pTmp++)))
        {
            DBG_TRACE(sg_aDbgBuf,"Enter DecodeUserDataHeader,FAIL! UDH_ID_BYTE_VAL_CON_16_BIT message.UDH element length NOT 0x04. It's %u",(*(pTmp-1)));
            return FALSE;
        }

        (pConSMSParam->uMsgRef) = U8_PACKET_TO_U16(pTmp[0],pTmp[1]);
        (pConSMSParam->uMsgType) = LIB_SMS_UD_TYPE_CON_7_BYTE;
        
        pTmp += 2;
    }
    else
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeUserDataHeader,FAIL! UDH Type is INVALID. uUDHIDByteVal: %u",uUDHIDByteVal);
        return FALSE;
    }

    (pConSMSParam->uMsgTot) = (*(pTmp++));
    (pConSMSParam->uMsgSeg) = (*(pTmp++));   
    
    if(((pConSMSParam->uMsgSeg) < 1) || ((pConSMSParam->uMsgSeg) > (pConSMSParam->uMsgTot)))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeUserDataHeader,FAIL! uConMsgSeg is LARGER than uConMsgTot. uConMsgSeg:%u,uConMsgTot:%u",(pConSMSParam->uMsgSeg),(pConSMSParam->uMsgTot));
        return FALSE;
    }
    
    DBG_TRACE(sg_aDbgBuf,"Enter DecodeUserDataHeader,SUCCESS. uUDByte:%u,uUDHIDByteVal:%u,uConMsgSeg:%u,uConMsgTot:%u,uMsgRef:%u,uMsgType:%u",uUDByte,uUDHIDByteVal,(pConSMSParam->uMsgSeg),(pConSMSParam->uMsgTot),(pConSMSParam->uMsgRef),(pConSMSParam->uMsgType));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  DecordUserDataBody
 *
 * DESCRIPTION
 *  Decode <UD> to 'LIB_SMS_UserDataStruct' data if data is present.
 *  
 * PARAMETERS
 *   <pUD>         The pointer of <UD> element
 *   <uUDLen>      The value of <UDL> element
 *   <uAlpha>      Alphabet enum, it's value is same as 'LIB_SMS_DCSAlphaEnum'
 *   <uUDType>     The type of user data,same as 'LIB_SMS_UDTypeEnum'
 *   <pUserData>   The pointer of 'LIB_SMS_UserDataStruct' data
 *
 * RETURNS
 *   TRUE,  This function works SUCCESS.
 *   FALSE, This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *****************************************************************************/ 
static bool DecordUserDataBody(u8 *pUD,u8 uUDLen,u8 uAlpha,u8 uUDType,LIB_SMS_UserDataStruct *pUserData)
{
    u16 uTmpLen = 0;
    u8 uUDBLen = 0;
    u8 uUDHLen = 0;
    u8 uUDMaxLen = 0;
    u8 uUDBByte = 0;
    
    if((NULL == pUD) || (NULL == pUserData))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecordUserDataBody,FAIL! Parameter is NULL.");
        return FALSE;
    }

    if(FALSE == IS_SUPPORT_USER_DATA_TYPE(uUDType))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecordUserDataBody,FAIL! IS_SUPPORT_USER_DATA_TYPE FAIL! uUDType:%d",uUDType);
        return FALSE;
    }

    GET_USER_DATA_MAX_LEN(uAlpha,uUDMaxLen);
    GET_USER_DATA_HEADER_LEN(uUDType,uAlpha,uUDHLen);
    
    if((uUDLen < uUDHLen) || (uUDLen > uUDMaxLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecordUserDataBody,FAIL! uUDLen is INVALID. uUDLen:%d,uUDHLen:%d,uUDMaxLen:%d",uUDLen,uUDHLen,uUDMaxLen);
        return FALSE;
    }

    uUDBLen = (uUDLen - uUDHLen);
    
    if((LIB_SMS_DCS_ALPHA_DEFAULT == uAlpha) && (LIB_SMS_UD_TYPE_CON_6_BYTE == uUDType))
    {
        uUDBLen = MAKE_UNSIGNED_INTEGER_SUBTRACTION(uUDBLen,1);

        if(uUDBLen > 0)
        {
            uUDBByte = COUNT_SEPTET_TO_OCTET((uUDBLen - 1));
        
            (*(pUserData->aUserData)) = ((pUD[0] & 0xFE) >> 1);

            uTmpLen = DecodeGSM7Bit(
                    (pUD + 1),
                    uUDBByte,
                    ((pUserData->aUserData) + 1),
                    (sizeof(pUserData->aUserData) - 1)
            );

            uUDBLen = GET_MIN_VALUE(uUDBLen, (uTmpLen+1));
        }
        
        (pUserData->uLen) = uUDBLen;
    }
    else if(LIB_SMS_DCS_ALPHA_DEFAULT == uAlpha)
    {
        uUDBByte = COUNT_SEPTET_TO_OCTET(uUDBLen);
        uTmpLen = DecodeGSM7Bit(pUD,uUDBByte,(pUserData->aUserData),sizeof(pUserData->aUserData));
        uUDBLen = GET_MIN_VALUE(uUDBLen,uTmpLen);
        
        (pUserData->uLen) = uUDBLen;
    }
    else if(LIB_SMS_DCS_ALPHA_8BIT_DATA == uAlpha)
    {
        Ql_memcpy((pUserData->aUserData),pUD,uUDBLen);
        (pUserData->uLen) = uUDBLen;
    }
    else if(LIB_SMS_DCS_ALPHA_UCS2 == uAlpha)
    {
        Ql_memcpy((pUserData->aUserData),pUD,uUDBLen);
        (pUserData->uLen) = uUDBLen;
    }
    else
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecordUserDataBody,FAIL! uAlpha is INVALID. uAlpha:%d",uAlpha);
        return FALSE;
    }
    
    DBG_TRACE(sg_aDbgBuf,"Enter DecordUserDataBody,SUCCESS. uAlpha:%u,uUDLen:%u,uUDType:%u,uUDBLen:%u",uAlpha,uUDLen,uUDType,uUDBLen);
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  DecodeDeliverTPDUOct
 *
 * DESCRIPTION
 *  Decode DELIVER-PDU octet string to 'LIB_SMS_DeliverPDUParamStruct' data
 *  
 * PARAMETERS
 *   <pTPDUOct>      The pointer of DELIVER-PDU octet string
 *   <uTPDUOctLen>   The length of DELIVER-PDU octet string
 *   <pParam>        The pointer of 'LIB_SMS_DeliverPDUParamStruct' data
 *
 * RETURNS
 *   TRUE,  This function works SUCCESS.
 *   FALSE, This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. If you ONLY want to check <pTPDUOct> is VALID or not,please set <pParam> to NULL.
 *****************************************************************************/
static bool DecodeDeliverTPDUOct(u8 *pTPDUOct, u16 uTPDUOctLen, LIB_SMS_DeliverPDUParamStruct *pParam)
{
    u8 uMTI = 0;
    u8 uOALen = 0;      //Length of <DA>,unit is: BCD characters
    u8 *pTmp = NULL;
    bool bResult = FALSE;
    u16 uUDMaxLen = 0;  //The maximum length of <UD>,it's related to <DCS>
    u8 uUDByte = 0;
    u8 uUDLen = 0;
    u8 uDCSAlpha = 0;
    bool bConSMSFlag = FALSE; //TRUE: a concatenate message, FALSE: a normal message    
    u8 uUDType = LIB_SMS_UD_TYPE_NORMAL;  //MUST set default value to 'LIB_SMS_UD_TYPE_NORMAL'
    u16 uTmpLen = 0;
        
    if((NULL == pTPDUOct) || (NULL == pParam) || (uTPDUOctLen < DELIVER_TPDU_MIN_LEN) || (uTPDUOctLen > DELIVER_TPDU_MAX_LEN))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeDeliverTPDUOct,FAIL! Parameter is INVALID. uTPDUOctLen:%u,SLIB_DELIVER_PDU_MIN_LEN:%u,SLIB_DELIVER_PDU_MAX_LEN:%u",uTPDUOctLen,DELIVER_TPDU_MIN_LEN,DELIVER_TPDU_MAX_LEN);
        return FALSE;
    }
    
    pTmp = &pTPDUOct[0];  //Pointer to <FO>
    GET_CON_SMS_FLAG_VAL((*pTmp),bConSMSFlag);
    
    uMTI = LIB_SMS_GET_MSG_TYPE_IN_PDU_FO((*(pTmp++)));
    if(uMTI != LIB_SMS_PDU_TYPE_DELIVER)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeDeliverTPDUOct,FAIL! NOT Deliver-PDU! uMTI:%u",uMTI);
        return FALSE;
    }
    
    uOALen = (*(pTmp++));  //Get <OA> length

    bResult = DecodeAddress(pTmp,(1 + COUNT_BCD_LEN_TO_BYTE(uOALen)),(&(pParam->sOA)));
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeDeliverTPDUOct,FAIL! DecodeAddress FAIL! ");
        return FALSE;
    }

    pTmp += (1 + COUNT_BCD_LEN_TO_BYTE(uOALen)); //Skip <OA> number
    
    if(FALSE == IS_SUPPORT_PID(*pTmp))
    {
    //<2015/02/26-ROTVG00006-P03-Vicent.Gao,<[SMS] MS shall interpret PID as 0x00 if it¡¯s not support>
    #if 1
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeDeliverTPDUOct,WARNING! IS_SUPPORT_PID FAIL! PID:%u will be interpreted as 0x00",*(pTmp));
    #else
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeDeliverTPDUOct,FAIL! IS_SUPPORT_PID FAIL! PID:%u",*(pTmp-1));
        return FALSE;
    #endif //#if 1
    //>2015/02/26-ROTVG00006-P03-Vicent.Gao
    }
    
    (pParam->uPID) = (*(pTmp++)); //Get <PID> info

    IS_VALID_DCS_IN_PDU((*pTmp),bResult);
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeDeliverTPDUOct,FAIL! IS_VALID_DCS_IN_PDU FAIL! DCS:0x%x",(*pTmp));
        return FALSE;
    }

    (pParam->uDCS) = (*pTmp);  //Get <uDCS> info
    
    LIB_SMS_DecodeDCS((*(pTmp++)),NULL,&uDCSAlpha,NULL,NULL);  //Decode <DCS>

    bResult = DecodeTimeStamp(pTmp,SCTS_FIXED_BYTE_LEN,&(pParam->sSCTS));
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeDeliverTPDUOct,FAIL! DecodeTimeStamp FAIL!");
        return FALSE;
    }
    
    pTmp += SCTS_FIXED_BYTE_LEN;  //Skip <SCST>
    
    //Check the parameter <UDL>
    GET_USER_DATA_MAX_LEN(uDCSAlpha,uUDMaxLen);

    uUDLen = (*(pTmp++));
    if(uUDLen > uUDMaxLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeDeliverTPDUOct,FAIL! <UDL> is INVALID. UDL:%u,uUDMaxLen:%u",*(pTmp-1),uUDMaxLen);
        return FALSE;
    }

    GET_USER_DATA_BYTE(uDCSAlpha,uUDLen,uUDByte);
    
    if(uUDByte > (uTPDUOctLen - (pTmp - pTPDUOct)))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeDeliverTPDUOct,FAIL! uUDByte is INVALID. uTPDUOctLen:%u,uUDByte:%u,Now UD byte:%u",uTPDUOctLen,uUDByte,(uTPDUOctLen - (pTmp - pTPDUOct)));
        return FALSE;
    }
    
    //Check the parameter <UDH>
    if(TRUE == bConSMSFlag)  //This is a concatenate message
    {
        if(FALSE == DecodeUserDataHeader(pTmp,uUDByte,&(pParam->sConSMSParam)))
        {
            DBG_TRACE(sg_aDbgBuf,"Enter CheckDeliverTPDUOct,FAIL! DecodeUserDataHeader FAIL! uUDLen:%u,uUDByte:%u,uDCSAlpha:%u",uUDLen,uUDByte,uDCSAlpha);
            return FALSE;
        }

        uUDType = ((pParam->sConSMSParam).uMsgType);
        GET_USER_DATA_HEADER_BYTE(uUDType,uTmpLen);
        pTmp += uTmpLen;  //Skip <UDH>
    }
    
    bResult = DecordUserDataBody(pTmp,uUDLen,uDCSAlpha,uUDType,&(pParam->sUserData));
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeDeliverTPDUOct,FAIL! DecordUserDataBody FAIL!");
        return FALSE;
    }
    
    DBG_TRACE(sg_aDbgBuf,"Enter DecodeDeliverTPDUOct,SUCCESS. ");    
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  DecodeSubmitTPDUOct
 *
 * DESCRIPTION
 *  Decode SUBMIT-PDU octet string to 'LIB_SMS_SubmitPDUParamStruct' data
 *  
 * PARAMETERS
 *   <pTPDUOct>      The pointer of DELIVER-PDU octet string
 *   <uTPDUOctLen>   The length of DELIVER-PDU octet string
 *   <pParam>        The pointer of 'LIB_SMS_SubmitPDUParamStruct' data
 *
 * RETURNS
 *   TRUE,  This function works SUCCESS.
 *   FALSE, This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *****************************************************************************/
static bool DecodeSubmitTPDUOct(u8 *pTPDUOct, u16 uTPDUOctLen, LIB_SMS_SubmitPDUParamStruct *pParam)
{
    u8 uMTI = 0;
    bool bResult = FALSE;
    u16 uUDMaxLen = 0;  //The maximum length of <UD>,it's related to <DCS>
    u8 uUDByte = 0;  //The bytes of <UD>
    u8 uUDLen = 0;   //The values of <UDL>
    u8 uDCSAlpha = 0;
    u8 uDALen = 0;      //Length of <DA>,unit is: BCD characters
    bool bConSMSFlag = FALSE; //TRUE: a concatenate message, FALSE: a normal message
    u8 uVPF = LIB_SMS_VPF_TYPE_NOT_PRESENT;   //<VPF> contained in <FO>
    u8 *pTmp = NULL;    
    u8 uUDType = LIB_SMS_UD_TYPE_NORMAL;  //MUST set the default value to 'LIB_SMS_UD_TYPE_NORMAL'
    u16 uTmpLen = 0;
        
    if((NULL == pTPDUOct) || (NULL == pParam) || (uTPDUOctLen < SUBMIT_TPDU_MIN_LEN) || (uTPDUOctLen > SUBMIT_TPDU_MAX_LEN))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,FAIL! Parameter is INVALID. uTPDUOctLen:%u,SUBMIT_TPDU_MIN_LEN:%u,SUBMIT_TPDU_MAX_LEN:%u",uTPDUOctLen,SUBMIT_TPDU_MIN_LEN,SUBMIT_TPDU_MAX_LEN);
        return FALSE;
    }
    
    pTmp = &pTPDUOct[0];  //Pointer to <FO>

    uMTI = LIB_SMS_GET_MSG_TYPE_IN_PDU_FO((*pTmp));
    if(uMTI != LIB_SMS_PDU_TYPE_SUBMIT)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,FAIL! NOT Submit-PDU! uMTI:%u",uMTI);
        return FALSE;
    }
    
    uVPF = LIB_SMS_GET_VPF_IN_SUBMIT_PDU_FO((*pTmp));
    if(FALSE == IS_SUPPORT_VPF(uVPF))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,FAIL! IS_SUPPORT_VPF FAIL! uVPF:%u",uVPF);
        return FALSE;
    }

    GET_CON_SMS_FLAG_VAL((*pTmp),bConSMSFlag);

    pTmp += 1;  //Skip <FO>
    pTmp += 1;  //Skip <MR>

    //Decode <DA>
    uDALen = (*(pTmp++));  //Get <DA> length
    if(uDALen > LIB_SMS_PHONE_NUMBER_MAX_LEN)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,FAIL! uDALen is INVALID. uDALen:%u,LIB_SMS_PHONE_NUMBER_MAX_LEN:%u",uDALen,LIB_SMS_PHONE_NUMBER_MAX_LEN);
        return FALSE;
    }

    bResult = DecodeAddress(pTmp,(1 + COUNT_BCD_LEN_TO_BYTE(uDALen)),(&(pParam->sDA)));
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,FAIL! DecodeAddress FAIL! ");
        return FALSE;
    }

    pTmp += (1 + COUNT_BCD_LEN_TO_BYTE(uDALen)); //Skip <DA> number

    //Decord <PID>
    if(FALSE == IS_SUPPORT_PID(*pTmp))
    {
    //<2015/02/26-ROTVG00006-P03-Vicent.Gao,<[SMS] MS shall interpret PID as 0x00 if it¡¯s not support>
    #if 1
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,WARNING! IS_SUPPORT_PID FAIL! PID:%u will be interpreted as 0x00",(*pTmp));
    #else
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,FAIL! IS_SUPPORT_PID FAIL! PID:%u",*(pTmp-1));
        return FALSE;
    #endif //#if 1
    //>2015/02/26-ROTVG00006-P03-Vicent.Gao        
    }
    
    (pParam->uPID) = (*(pTmp++));

    //Decord <DCS>
    IS_VALID_DCS_IN_PDU((*pTmp),bResult);
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,FAIL! IS_VALID_DCS_IN_PDU FAIL! DCS:0x%x",(*pTmp));
        return FALSE;
    }

    (pParam->uDCS) = (*pTmp);
    
    LIB_SMS_DecodeDCS((*(pTmp++)),NULL,&uDCSAlpha,NULL,NULL);  //Decode <DCS>
    
    //Decode <VP>
    if(LIB_SMS_VPF_TYPE_RELATIVE == uVPF)
    {
        ((pParam->sVP).uRelative) = (*pTmp);
        pTmp += 1;
    }
    else if(LIB_SMS_VPF_TYPE_ABSOLUTE == uVPF)
    {
        bResult = DecodeTimeStamp(pTmp,SCTS_FIXED_BYTE_LEN,&((pParam->sVP).sAbsolute));
        if(FALSE == bResult)
        {
            DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,FAIL! DecodeTimeStamp FAIL!");
            return FALSE;
        }
        
        pTmp += SCTS_FIXED_BYTE_LEN;
    }
    
    //Check the parameter <UDL>
    GET_USER_DATA_MAX_LEN(uDCSAlpha,uUDMaxLen);

    uUDLen = (*(pTmp++));  //Get <UDL>
    if(uUDLen > uUDMaxLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,FAIL! <UDL> is too larger. UDL:%u,uUDMaxLen:%u",*(pTmp-1),uUDMaxLen);
        return FALSE;
    }

    GET_USER_DATA_BYTE(uDCSAlpha,uUDLen,uUDByte);
    
    if(uUDByte > (uTPDUOctLen - (pTmp - pTPDUOct)))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,FAIL! uUDByte is INVALID. uUDByte:%u,Now UD byte:%u",uUDByte,(uTPDUOctLen - (pTmp - pTPDUOct)));
        return FALSE;
    }

    //Check the parameter <UDH>
    if(TRUE == bConSMSFlag)  //This is a concatenate message
    {
        if(FALSE == DecodeUserDataHeader(pTmp,uUDByte,&(pParam->sConSMSParam)))
        {
            DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,FAIL! DecodeUserDataHeader FAIL!");
            return FALSE;
        }

        uUDType = ((pParam->sConSMSParam).uMsgType);
        GET_USER_DATA_HEADER_BYTE(uUDType,uTmpLen);
        pTmp += uTmpLen;  //Skip <UDH>
    }
    
    bResult = DecordUserDataBody(pTmp,uUDLen,uDCSAlpha,uUDType,&(pParam->sUserData));
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,FAIL! DecordUserDataBody FAIL!");
        return FALSE;
    }
    
    DBG_TRACE(sg_aDbgBuf,"Enter DecodeSubmitTPDUOct,SUCCESS. uVPF:%d,uUDLen:%d,uUDByte:%d,uUDType:%d,uUDMaxLen:%d",uVPF,uUDLen,uUDByte,uUDType,uUDMaxLen);
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConvAlphaDefaultToCharSetUCS2
 *
 * DESCRIPTION
 *  Convert alpha 'default' data to charset 'UCS2' data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.s
 *****************************************************************************/ 
static bool ConvAlphaDefaultToCharSetUCS2(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen)
{
    u16 i = 0;
    u16 j = 0;
    u16 uChar = 0;
    u16 uUniChar = 0;
    bool bResult = FALSE;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvAlphaDefaultToCharSetUCS2,FAIL! Parameter is NULL.");
        return FALSE;
    }

    i = 0;
    j = 0;
    while(i < uSrcLen)
    {
        if(ALPHA_DEFAULT_EXTEND_CHAR_FLAG == pSrc[i])
        {
            if((i+1) == uSrcLen)  //0x1B at the end position,and there is none characters after it.
            {
                uUniChar = UCS2_CHAR_DEFAULT;
                
                U16_UNPACKET_TO_HEX_CHAR(uUniChar,pDest[j+0],pDest[j+1],pDest[j+2],pDest[j+3]);
                j += 4;
                break;
            }
            
            uChar = U8_PACKET_TO_U16(pSrc[i],pSrc[i+1]);
            CONV_ALPHA_DEFAUTL_EXTEND_CHAR_TO_UCS2(uChar,uUniChar,bResult);
            if(FALSE == bResult)
            {
                uUniChar = UCS2_CHAR_DEFAULT;
            }
            
            i += 2;
        }
        else
        {
            CONV_ALPHA_DEFAUTL_CHAR_TO_UCS2(pSrc[i],uUniChar,bResult);
            if(FALSE == bResult)
            {
                uUniChar = UCS2_CHAR_DEFAULT;
            }
            
            i += 1;
        }
        
        U16_UNPACKET_TO_HEX_CHAR(uUniChar,pDest[j+0],pDest[j+1],pDest[j+2],pDest[j+3]);
        j += 4;
    }
    
    (*pDestLen) = j;
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvAlphaDefaultToCharSetUCS2,SUCCESS. uSrcLen:%u,(*pDestLen):%u",uSrcLen,(*pDestLen));
    
    return TRUE;
}

//<2015/06/04-ROTVG00006-P08-Vicent.Gao,<[SMS] Add support for CharSet: IRA.>
/*****************************************************************************
 * FUNCTION
 *  ConvAlphaDefaultExtendCharToIRA
 *
 * DESCRIPTION
 *  Convert Alpha 'Default' extend character to CharSet 'IRA' character
 *  
 * PARAMETERS
 *   <uDefaultEChar>   Alpha 'Default' extend character
 *   <pIRAChar>        The pointer of CharSet 'IRA' character
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvAlphaDefaultExtendCharToIRA(u16 uDefaultEChar,u8 *pIRAChar)
{
    u16 i = 0;

    if(NULL == pIRAChar)
    {
        return FALSE;
    }

    for(i = 0; i < ALPHA_DEFAULT_EXTEND_CHAR_COUNT; i++) 
    {   
        if(uDefaultEChar == sg_aAlphaDefaultExtendChars[i])   
        {   
            break;  
        }   
    }   

    if(i >= ALPHA_DEFAULT_EXTEND_CHAR_COUNT)   
    {   
        return FALSE;
    }  

    *pIRAChar = sg_aAlphaDefaultExtendCharsIRAMap[i];
        
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSetIRACharToAlphaDefaultExtend
 *
 * DESCRIPTION
 *  Convert CharSet 'IRA' character to Alpha 'Default' extend character.
 *  
 * PARAMETERS
 *   <uIRAChar>        The CharSet 'IRA' character
 *   <pDefaultEChar>   The pointer of Alpha 'Default' extend character
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. If you ONLY want to know <uIRAChar> is a Alpha 'Default' extend character or not, you can set <pDefaultEChar> to NULL
 *****************************************************************************/ 
static bool ConvCharSetIRACharToAlphaDefaultExtend(u8 uIRAChar, u16 *pDefaultEChar)
{
    u16 i = 0;

    for(i = 0; i < ALPHA_DEFAULT_EXTEND_CHAR_COUNT; i++)
    {
        if((uIRAChar != IRA_CHAR_DEFAULT) && (uIRAChar == sg_aAlphaDefaultExtendCharsIRAMap[i]))
        {
            break;
        }
    }
    
    if(i >= ALPHA_DEFAULT_EXTEND_CHAR_COUNT)   
    {   
        return FALSE;
    }   

    if(pDefaultEChar != NULL)
    {
        *pDefaultEChar = sg_aAlphaDefaultExtendChars[i];
    }
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSetIRALenToAlpha
 *
 * DESCRIPTION
 *  Convert the length of CharSet 'IRA' data to the length of Alpha data
 *  
 * PARAMETERS
 *   <eAlhpa>      Alphabet,It's value is in 'LIB_SMS_DCSAlphaEnum'
 *   <pCSData>     The pointer of CharSet 'IRA' data
 *   <uCSLen>      The length of CharSet 'IRA' data
 *
 * RETURNS
 *   The length of Alpha data
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static u16 ConvCharSetIRALenToAlpha(LIB_SMS_DCSAlphaEnum eAlhpa,u8 *pCSData,u16 uCSLen)
{
    u16 i = 0;
    u16 uAlphaLen = 0;
    u16 uIndex = 0;
    bool bResult = FALSE;

    if(NULL == pCSData)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetIRALenToAlpha,WARNING! Parameter is NULL. pCSData:%x",pCSData);
        return 0;
    }

    if(LIB_SMS_DCS_ALPHA_DEFAULT == eAlhpa)
    {
        uAlphaLen = 0;
        for(i = 0; i < uCSLen; i++)
        {
            bResult = ConvCharSetIRACharToAlphaDefaultExtend(pCSData[i],NULL);
            if(TRUE == bResult)    
            {   
                uAlphaLen += 2;    
            }   
            else    
            {   
                uAlphaLen += 1;    
            }   
        }
    }
    else if(LIB_SMS_DCS_ALPHA_UCS2 == eAlhpa)
    {
        uAlphaLen = 2 * uCSLen;
    }
    else
    {
        uAlphaLen = uCSLen;
    }

    return uAlphaLen;
}

/*****************************************************************************
 * FUNCTION
 *  ConvAlphaDefaultToCharSetIRA
 *
 * DESCRIPTION
 *  Convert alpha 'default' data to charset 'UCS2' data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.s
 *****************************************************************************/ 
static bool ConvAlphaDefaultToCharSetIRA(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen)
{
    u16 i = 0;
    u16 j = 0;
    u16 uChar = 0;
    u8 uIRAChar = 0;
    bool bResult = FALSE;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvAlphaDefaultToCharSetIRA,FAIL! Parameter is NULL.");
        return FALSE;
    }

    i = 0;
    j = 0;
    while(i < uSrcLen)
    {
        if(ALPHA_DEFAULT_EXTEND_CHAR_FLAG == pSrc[i])
        {
            if((i+1) == uSrcLen)  //0x1B at the end position,and there is none characters after it.
            {
                pDest[j++] = IRA_CHAR_DEFAULT;

                break;
            }
            
            uChar = U8_PACKET_TO_U16(pSrc[i],pSrc[i+1]);
            bResult = ConvAlphaDefaultExtendCharToIRA(uChar,&uIRAChar);
            if(FALSE == bResult)
            {
                uIRAChar = IRA_CHAR_DEFAULT;
            }
            
            i += 2;
        }
        else
        {
            uIRAChar = sg_aAlphaDefaultCharsIRAMap[pSrc[i]];
            
            i += 1;
        }

        pDest[j++] = uIRAChar;
    }
    
    (*pDestLen) = j;
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvAlphaDefaultToCharSetIRA,SUCCESS. uSrcLen:%u,(*pDestLen):%u",uSrcLen,(*pDestLen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSetIRAToAlphaDefault
 *
 * DESCRIPTION
 *  Convert charset 'IRA' data to alpha 'Default' data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uAlpha>      Alphabet,It's value is in 'LIB_SMS_DCSAlphaEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvCharSetIRAToAlphaDefault(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen)
{
    u16 i = 0;
    u16 j = 0;
    bool bResult = FALSE;
    u16 uChar = 0;
    //<2015/06/12-ROTVG00006-P08-A01-Vicent.Gao,<[SMS] Sending text r\ may cause dump use  IRA.>
    u8 uHigh = 0;
    u8 uLow = 0;
    //>2015/06/12-ROTVG00006-P08-A01-Vicent.Gao

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetIRAToAlphaDefault,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    for(i = 0; i < uSrcLen; i++)
    {
        if(pSrc[i] >= CHARSET_IRA_CHAR_COUNT)
        {
            DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetIRAToAlphaDefault,FAIL! pSrc[%d]:%d is >= %d",i,pSrc[i],CHARSET_IRA_CHAR_COUNT);
            return FALSE;
        }
    
        bResult = ConvCharSetIRACharToAlphaDefaultExtend(pSrc[i],&uChar);
        if(TRUE == bResult)
        {
        //<2015/06/12-ROTVG00006-P08-A01-Vicent.Gao,<[SMS] Sending text r\ may cause dump use  IRA.>
        #if 1
            U16_UNPACKET_TO_U8(uChar,uHigh,uLow);
            pDest[j + 0] = uHigh;
            pDest[j + 1] = uLow;
        #else
            U16_BYTE_REVERSE(uChar, uChar); //MUST byte reverse
            (*((u16*)(pDest + j))) = uChar;
        #endif //#if 1
        //>2015/06/12-ROTVG00006-P08-A01-Vicent.Gao
            
            j += 2;
        }
        else
        {
            pDest[j] = sg_aCharSetIRACharsGSMMap[pSrc[i]];
            j += 1;
        }
    }
    
    (*pDestLen) = j;
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetIRAToAlphaDefault,SUCCESS. uSrcLen:%u,(*pDestLen):%u",uSrcLen,(*pDestLen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSetIRAToAlphaUCS2
 *
 * DESCRIPTION
 *  Convert charset 'IRA' data to alpha 'UCS2' data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uAlpha>      Alphabet,It's value is in 'LIB_SMS_DCSAlphaEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvCharSetIRAToAlphaUCS2(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen)
{
    u16 i = 0;
    u16 j = 0;
    u16 uChar = 0;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetIRAToAlphaUCS2,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    for(i = 0; i < uSrcLen; i++)
    {
        if(pSrc[i] >= CHARSET_IRA_CHAR_COUNT)
        {
            DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetIRAToAlphaUCS2,FAIL! pSrc[%d]:%d is >= %d",i,pSrc[i],CHARSET_IRA_CHAR_COUNT);
            return FALSE;
        }

        uChar = pSrc[i];
        U16_BYTE_REVERSE(uChar, uChar); //MUST byte reverse
        (*((u16*)(pDest + j))) = uChar;
        
        j += 2;
    }
    
    (*pDestLen) = j;
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetIRAToAlphaUCS2,SUCCESS. uSrcLen:%u,(*pDestLen):%u",uSrcLen,(*pDestLen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSetIRAToAlpha
 *
 * DESCRIPTION
 *  Convert charset 'IRA' data to alpha data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uAlpha>      Alphabet,It's value is in 'LIB_SMS_DCSAlphaEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvCharSetIRAToAlpha(u8 *pSrc,u16 uSrcLen,u8 uAlpha,u8 *pDest,u16 *pDestLen)
{
    bool bResult = FALSE;
    u16 uLimitLen = 0;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetIRAToAlpha,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if(FALSE == IS_SUPPORT_ALPHA(uAlpha))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetIRAToAlpha,FAIL! IS_SUPPORT_ALPHA FAIL! uAlpha:%u",uAlpha);
        return FALSE;
    }

    uLimitLen = ConvCharSetIRALenToAlpha(uAlpha,pSrc,uSrcLen);
    if((*pDestLen) < uLimitLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetIRAToAlpha,FAIL! (*pDestLen) is too less. (*pDestLen):%u",(*pDestLen));
        return FALSE;
    }
    
    if(LIB_SMS_DCS_ALPHA_DEFAULT == uAlpha)
    {
        bResult = ConvCharSetIRAToAlphaDefault(pSrc,uSrcLen,pDest,pDestLen);
    }
    else if(LIB_SMS_DCS_ALPHA_UCS2 == uAlpha)
    {
        bResult = ConvCharSetIRAToAlphaUCS2(pSrc,uSrcLen,pDest,pDestLen);
    }
    else
    {
        bResult = TRUE;
        SMS_MEM_COPY_ONLY(pSrc,uSrcLen,pDest,pDestLen);        
    }    
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetIRAToAlpha,SUCCESS. bResult:%d,(*pDestLen):%u",bResult,(*pDestLen));    
    
    return bResult;
}
//>2015/06/04-ROTVG00006-P08-Vicent.Gao

//<2015/06/05-ROTVG00006-P09-Vicent.Gao,<[SMS] Add support for CharSet: 8859-1.>
/*****************************************************************************
 * FUNCTION
 *  ConvAlphaDefaultExtendCharTo88591
 *
 * DESCRIPTION
 *  Convert Alpha 'Default' extend character to CharSet 'ISO 8859-1' character
 *  
 * PARAMETERS
 *   <uDefaultEChar>   Alpha 'Default' extend character
 *   <p88591Char>      The pointer of CharSet 'ISO 8859-1' character
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvAlphaDefaultExtendCharTo88591(u16 uDefaultEChar,u8 *p88591Char)
{
    u16 i = 0;

    for(i = 0; i < ALPHA_DEFAULT_EXTEND_CHAR_COUNT; i++) 
    {   
        if(uDefaultEChar == sg_aAlphaDefaultExtendChars[i])   
        {   
            break;  
        }   
    }   

    if(i >= ALPHA_DEFAULT_EXTEND_CHAR_COUNT)   
    {   
        return FALSE;
    }  

    if(p88591Char != NULL)
    {
        *p88591Char = sg_aAlphaDefaultExtendChars88591Map[i];
    }
        
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSet88591CharToAlphaDefaultExtend
 *
 * DESCRIPTION
 *  Convert CharSet 'ISO 8859-1' character to Alpha 'Default' extend character.
 *  
 * PARAMETERS
 *   <u88591Char>      The CharSet 'ISO 8859-1' character
 *   <pDefaultEChar>   The pointer of Alpha 'Default' extend character
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. If you ONLY want to know <u88591Char> is a Alpha 'Default' extend character or not, you can set <pDefaultEChar> to NULL
 *****************************************************************************/ 
static bool ConvCharSet88591CharToAlphaDefaultExtend(u8 u88591Char, u16 *pDefaultEChar)
{
    u16 i = 0;

    for(i = 0; i < ALPHA_DEFAULT_EXTEND_CHAR_COUNT; i++)
    {
        if((u88591Char != ISO88591_CHAR_DEFAULT) && (u88591Char == sg_aAlphaDefaultExtendChars88591Map[i]))
        {
            break;
        }
    }
    
    if(i >= ALPHA_DEFAULT_EXTEND_CHAR_COUNT)   
    {   
        return FALSE;
    }   

    if(pDefaultEChar != NULL)
    {
        *pDefaultEChar = sg_aAlphaDefaultExtendChars[i];
    }
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSet88591LenToAlpha
 *
 * DESCRIPTION
 *  Convert the length of CharSet 'ISO 8859-1' data to the length of Alpha data
 *  
 * PARAMETERS
 *   <eAlhpa>      Alphabet,It's value is in 'LIB_SMS_DCSAlphaEnum'
 *   <pCSData>     The pointer of CharSet 'ISO 8859-1' data
 *   <uCSLen>      The length of CharSet 'ISO 8859-1' data
 *
 * RETURNS
 *   The length of Alpha data
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static u16 ConvCharSet88591LenToAlpha(LIB_SMS_DCSAlphaEnum eAlhpa,u8 *pCSData,u16 uCSLen)
{
    u16 i = 0;
    u16 uAlphaLen = 0;
    u16 uIndex = 0;
    bool bResult = FALSE;

    if(NULL == pCSData)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSet88591LenToAlpha,WARNING! Parameter is NULL. pCSData:%x",pCSData);
        return 0;
    }

    if(LIB_SMS_DCS_ALPHA_DEFAULT == eAlhpa)
    {
        uAlphaLen = 0;
        for(i = 0; i < uCSLen; i++)
        {
            bResult = ConvCharSet88591CharToAlphaDefaultExtend(pCSData[i],NULL);
            if(TRUE == bResult)    
            {   
                uAlphaLen += 2;    
            }   
            else    
            {   
                uAlphaLen += 1;    
            }   
        }
    }
    else if(LIB_SMS_DCS_ALPHA_UCS2 == eAlhpa)
    {
        uAlphaLen = 2 * uCSLen;
    }
    else
    {
        uAlphaLen = uCSLen;
    }

    return uAlphaLen;
}

/*****************************************************************************
 * FUNCTION
 *  ConvAlphaDefaultToCharSet88591
 *
 * DESCRIPTION
 *  Convert Alpha 'Default' data to CharSet 'ISO 8859-1' data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.s
 *****************************************************************************/ 
static bool ConvAlphaDefaultToCharSet88591(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen)
{
    u16 i = 0;
    u16 j = 0;
    u16 uChar = 0;
    u8 u88591Char = 0;
    bool bResult = FALSE;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvAlphaDefaultToCharSet88591,FAIL! Parameter is NULL.");
        return FALSE;
    }

    i = 0;
    j = 0;
    while(i < uSrcLen)
    {
        if(ALPHA_DEFAULT_EXTEND_CHAR_FLAG == pSrc[i])
        {
            if((i+1) == uSrcLen)  //0x1B at the end position,and there is none characters after it.
            {
                pDest[j++] = ISO88591_CHAR_DEFAULT;

                break;
            }
            
            uChar = U8_PACKET_TO_U16(pSrc[i],pSrc[i+1]);
            bResult = ConvAlphaDefaultExtendCharTo88591(uChar,&u88591Char);
            if(FALSE == bResult)
            {
                u88591Char = ISO88591_CHAR_DEFAULT;
            }
            
            i += 2;
        }
        else
        {
            u88591Char = sg_aAlphaDefaultChars88591Map[pSrc[i]];
            
            i += 1;
        }

        pDest[j++] = u88591Char;
    }
    
    (*pDestLen) = j;
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvAlphaDefaultToCharSet88591,SUCCESS. uSrcLen:%u,(*pDestLen):%u",uSrcLen,(*pDestLen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSet88591ToAlphaDefault
 *
 * DESCRIPTION
 *  Convert CharSet 'ISO 8859-1' data to alpha 'Default' data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uAlpha>      Alphabet,It's value is in 'LIB_SMS_DCSAlphaEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvCharSet88591ToAlphaDefault(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen)
{
    u16 i = 0;
    u16 j = 0;
    bool bResult = FALSE;
    u16 uChar = 0;
    //<2015/06/12-ROTVG00006-P09-A01-Vicent.Gao,<[SMS] Sending text r\ may cause dump use 8859-1.>
    u8 uHigh = 0;
    u8 uLow = 0;
    //>2015/06/12-ROTVG00006-P09-A01-Vicent.Gao

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSet88591ToAlphaDefault,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    for(i = 0; i < uSrcLen; i++)
    {
        bResult = ConvCharSet88591CharToAlphaDefaultExtend(pSrc[i],&uChar);
        if(TRUE == bResult)
        {
        //<2015/06/12-ROTVG00006-P09-A01-Vicent.Gao,<[SMS] Sending text r\ may cause dump use 8859-1.>
        #if 1
            U16_UNPACKET_TO_U8(uChar,uHigh,uLow);
            pDest[j + 0] = uHigh;
            pDest[j + 1] = uLow;
        #else //#if 1
            U16_BYTE_REVERSE(uChar, uChar); //MUST byte reverse
            (*((u16*)(pDest + j))) = uChar;
        #endif //#if 1
        //>2015/06/12-ROTVG00006-P09-A01-Vicent.Gao
            
            j += 2;
        }
        else
        {
            pDest[j] = sg_aCharSet88591CharsGSMMap[pSrc[i]];
            j += 1;
        }
    }
    
    (*pDestLen) = j;
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSet88591ToAlphaDefault,SUCCESS. uSrcLen:%u,(*pDestLen):%u",uSrcLen,(*pDestLen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSet88591ToAlphaUCS2
 *
 * DESCRIPTION
 *  Convert CharSet 'ISO 8859-1' data to alpha 'UCS2' data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uAlpha>      Alphabet,It's value is in 'LIB_SMS_DCSAlphaEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvCharSet88591ToAlphaUCS2(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen)
{
    u16 i = 0;
    u16 j = 0;
    u16 uChar = 0;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSet88591ToAlphaUCS2,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    for(i = 0; i < uSrcLen; i++)
    {
        uChar = pSrc[i];
        U16_BYTE_REVERSE(uChar, uChar); //MUST byte reverse
        (*((u16*)(pDest + j))) = uChar;
        
        j += 2;
    }
    
    (*pDestLen) = j;
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSet88591ToAlphaUCS2,SUCCESS. uSrcLen:%u,(*pDestLen):%u",uSrcLen,(*pDestLen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSet88591ToAlpha
 *
 * DESCRIPTION
 *  Convert CharSet 'ISO 8859-1' data to alpha data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uAlpha>      Alphabet,It's value is in 'LIB_SMS_DCSAlphaEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvCharSet88591ToAlpha(u8 *pSrc,u16 uSrcLen,u8 uAlpha,u8 *pDest,u16 *pDestLen)
{
    bool bResult = FALSE;
    u16 uLimitLen = 0;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSet88591ToAlpha,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if(FALSE == IS_SUPPORT_ALPHA(uAlpha))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSet88591ToAlpha,FAIL! IS_SUPPORT_ALPHA FAIL! uAlpha:%u",uAlpha);
        return FALSE;
    }

    uLimitLen = ConvCharSet88591LenToAlpha(uAlpha,pSrc,uSrcLen);
    if((*pDestLen) < uLimitLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSet88591ToAlpha,FAIL! (*pDestLen) is too less. (*pDestLen):%u",(*pDestLen));
        return FALSE;
    }
    
    if(LIB_SMS_DCS_ALPHA_DEFAULT == uAlpha)
    {
        bResult = ConvCharSet88591ToAlphaDefault(pSrc,uSrcLen,pDest,pDestLen);
    }
    else if(LIB_SMS_DCS_ALPHA_UCS2 == uAlpha)
    {
        bResult = ConvCharSet88591ToAlphaUCS2(pSrc,uSrcLen,pDest,pDestLen);
    }
    else
    {
        bResult = TRUE;
        SMS_MEM_COPY_ONLY(pSrc,uSrcLen,pDest,pDestLen);        
    }    
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSet88591ToAlpha,SUCCESS. bResult:%d,(*pDestLen):%u",bResult,(*pDestLen));    
    
    return bResult;
}
//>2015/06/05-ROTVG00006-P09-Vicent.Gao

/*****************************************************************************
 * FUNCTION
 *  ConvAlpha8BitToCharSet
 *
 * DESCRIPTION
 *  Convert alpha '8Bit data' data to charset data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uCharSet>    Character set,It's value is in 'LIB_SMS_CharSetEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvAlpha8BitToCharSet(u8 *pSrc,u16 uSrcLen,u8 uCharSet,u8 *pDest,u16 *pDestLen)
{
    bool bResult = FALSE;
    u16 uLimitLen = 0;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvAlpha8BitToCharSet,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if(FALSE == LIB_SMS_IS_SUPPORT_CHARSET(uCharSet))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvAlpha8BitToCharSet,FAIL! LIB_SMS_IS_SUPPORT_CHARSET FAIL! uCharSet:%d",uCharSet);
        return FALSE;
    }

    GET_ALPHA_8BIT_TO_CHARSET_LEN(uCharSet,pSrc,uSrcLen,uLimitLen);
    if((*pDestLen) < uLimitLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvAlpha8BitToCharSet,FAIL! (*pDestLen) is less. (*pDestLen):%d,uLimitLen:%u",(*pDestLen),uLimitLen);
        return FALSE;
    }
    
    if(LIB_SMS_CHARSET_GSM == uCharSet)  //Copy only
    {
        bResult = TRUE;
        SMS_MEM_COPY_ONLY(pSrc,uSrcLen,pDest,pDestLen);
    }
    else if(LIB_SMS_CHARSET_UCS2 == uCharSet) //Copy only
    {
        bResult = LIB_SMS_ConvHexOctToHexStr(pSrc,uSrcLen,(char*)pDest,pDestLen);
    }
    //<2015/06/04-ROTVG00006-P08-Vicent.Gao,<[SMS] Add support for CharSet: IRA.>
    else if(LIB_SMS_CHARSET_IRA == uCharSet)  //Copy only
    {
        bResult = TRUE;
        SMS_MEM_COPY_ONLY(pSrc,uSrcLen,pDest,pDestLen);
    }
    //>2015/06/04-ROTVG00006-P08-Vicent.Gao    
    //<2015/06/05-ROTVG00006-P09-Vicent.Gao,<[SMS] Add support for CharSet: 8859-1.>
    else if(LIB_SMS_CHARSET_8859_1 == uCharSet)  //Copy only
    {
        bResult = TRUE;
        SMS_MEM_COPY_ONLY(pSrc,uSrcLen,pDest,pDestLen);
    }
    //>2015/06/05-ROTVG00006-P09-Vicent.Gao    
    else
    {
        bResult = LIB_SMS_ConvHexOctToHexStr(pSrc,uSrcLen,(char*)pDest,pDestLen);
    }
    
    DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvAlpha8BitToCharSet,SUCCESS. uSrcLen:%u,uCharSet:%d,bResult:%u",uSrcLen,uCharSet,bResult);
    
    return bResult;    
}

/*****************************************************************************
 * FUNCTION
 *  ConvAlphaDefaultToCharSet
 *
 * DESCRIPTION
 *  Convert alpha 'default' data to charset data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uCharSet>    Character set,It's value is in 'LIB_SMS_CharSetEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvAlphaDefaultToCharSet(u8 *pSrc,u16 uSrcLen,u8 uCharSet,u8 *pDest,u16 *pDestLen)
{
    bool bResult = FALSE;
    u16 uLimitLen = 0;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvAlphaDefaultToCharSet,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if(FALSE == LIB_SMS_IS_SUPPORT_CHARSET(uCharSet))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvAlphaDefaultToCharSet,FAIL! LIB_SMS_IS_SUPPORT_CHARSET FAIL! uCharSet:%d",uCharSet);
        return FALSE;
    }

    GET_ALPHA_DEFAULT_TO_CHARSET_LEN(uCharSet,pSrc,uSrcLen,uLimitLen);
    if((*pDestLen) < uLimitLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvAlphaDefaultToCharSet,FAIL! (*pDestLen) is less. (*pDestLen):%u,uLimitLen:%u",(*pDestLen),uLimitLen);
        return FALSE;
    }

    if(LIB_SMS_CHARSET_GSM == uCharSet)
    {
        bResult = TRUE;
        SMS_MEM_COPY_ONLY(pSrc,uSrcLen,pDest,pDestLen);
    }
    else if(LIB_SMS_CHARSET_UCS2 == uCharSet)
    {
        bResult = ConvAlphaDefaultToCharSetUCS2(pSrc,uSrcLen,pDest,pDestLen);
    }
    //<2015/06/04-ROTVG00006-P08-Vicent.Gao,<[SMS] Add support for CharSet: IRA.>
    else if(LIB_SMS_CHARSET_IRA == uCharSet)    
    {
        bResult = ConvAlphaDefaultToCharSetIRA(pSrc,uSrcLen,pDest,pDestLen);
    }
    //>2015/06/04-ROTVG00006-P08-Vicent.Gao    
    //<2015/06/05-ROTVG00006-P09-Vicent.Gao,<[SMS] Add support for CharSet: 8859-1.>
    else if(LIB_SMS_CHARSET_8859_1 == uCharSet)    
    {
        bResult = ConvAlphaDefaultToCharSet88591(pSrc,uSrcLen,pDest,pDestLen);
    }
    //>2015/06/05-ROTVG00006-P09-Vicent.Gao    
    else
    {
        bResult = LIB_SMS_ConvHexOctToHexStr(pSrc,uSrcLen,(char*)pDest,pDestLen); //Ignore <CharSet>,only display its hex string
    }
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvAlphaDefaultToCharSet,SUCCESS. uSrcLen:%u,uCharSet:%d,bResult:%u,(*pDestLen):%u",uSrcLen,uCharSet,bResult,(*pDestLen));
    
    return bResult;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSetHEXToAlpha
 *
 * DESCRIPTION
 *  Convert charset 'GSM' data to alpha 'Default' data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uAlpha>      Alphabet,It's value is in 'LIB_SMS_DCSAlphaEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvCharSetUCS2ToAlphaDefault(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen)
{
    u16 i = 0;
    u16 j = 0;
    bool bResult = FALSE;
    u16 uUniChar = 0;
    u16 uChar = 0;
    //<2015/06/12-ROTVG00006-P01-A01-Vicent.Gao,<[SMS] Tiny modify 1 of segment 1 ==> RIL SMS LIB>
    u8 uHigh = 0;
    u8 uLow = 0;
    //>2015/06/12-ROTVG00006-P01-A01-Vicent.Gao

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetUCS2ToAlphaDefault,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    for(i = 0; i < uSrcLen; i += 2)
    {
        uUniChar = (*((u16*)(pSrc + i)));
        //<2015/06/12-ROTVG00006-P01-A01-Vicent.Gao,<[SMS] Tiny modify 1 of segment 1 ==> RIL SMS LIB>
        U16_BYTE_REVERSE(uUniChar,uUniChar);
        //>2015/06/12-ROTVG00006-P01-A01-Vicent.Gao        
        
        CONV_CHARSET_UCS2_TO_DEFAULT_EXTEND_CHAR(uUniChar,uChar,bResult);
        if(TRUE == bResult)
        {
        //<2015/06/12-ROTVG00006-P01-A01-Vicent.Gao,<[SMS] Tiny modify 1 of segment 1 ==> RIL SMS LIB>
        #if 1
            U16_UNPACKET_TO_U8(uChar, uHigh, uLow);
            pDest[j + 0] = uHigh;
            pDest[j + 1] = uLow;
        #else //#if 1
            (*((u16*)(pDest + j))) = uChar;
        #endif //#if 1
        //>2015/06/12-ROTVG00006-P01-A01-Vicent.Gao        
        
            j += 2;
        }
        else
        {
            CONV_CHARSET_UCS2_TO_DEFAULT_CHAR(uUniChar,uChar,bResult);
            if((FALSE == bResult) || (ALPHA_DEFAULT_EXTEND_CHAR_FLAG == uChar))
            {
                pDest[j] = ALPHA_DEFAULT_CHAR_DEFAULT;
            }
            else
            {
                pDest[j] = uChar;
            }
            
            j += 1;
        }
    }
    
    (*pDestLen) = j;
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetUCS2ToAlphaDefault,SUCCESS. uSrcLen:%u,(*pDestLen):%u",uSrcLen,(*pDestLen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSetHEXToAlpha
 *
 * DESCRIPTION
 *  Convert charset 'GSM' data to alpha 'UCS2' data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uAlpha>      Alphabet,It's value is in 'LIB_SMS_DCSAlphaEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvCharSetGSMToAlphaUCS2(u8 *pSrc,u16 uSrcLen,u8 *pDest,u16 *pDestLen)
{
    u16 i = 0;
    u16 j = 0;
    u16 uChar = 0;
    u16 uUniChar = 0;
    bool bResult = FALSE;
    
    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetGSMToAlphaUCS2,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    i = 0;
    j = 0;
    while(i < uSrcLen)
    {
        if(ALPHA_DEFAULT_EXTEND_CHAR_FLAG == pSrc[i])
        {
            if((i+1) == uSrcLen)  //0x1B at the end position,and there is none characters after it.
            {
                uUniChar = UCS2_CHAR_DEFAULT;
                
                U16_BYTE_REVERSE(uUniChar,(*(u16*)(pDest+j)));
                j += 2;
                
                break;
            }
            
            uChar = U8_PACKET_TO_U16(pSrc[i],pSrc[i+1]);
            CONV_ALPHA_DEFAUTL_EXTEND_CHAR_TO_UCS2(uChar,uUniChar,bResult);
            if(FALSE == bResult)
            {
                uUniChar = UCS2_CHAR_DEFAULT;
            }
            
            i += 2;
        }
        else
        {
            CONV_ALPHA_DEFAUTL_CHAR_TO_UCS2(pSrc[i],uUniChar,bResult);
            if(FALSE == bResult)
            {
                uUniChar = UCS2_CHAR_DEFAULT;
            }
            
            i += 1;
        }
        
        U16_BYTE_REVERSE(uUniChar,(*(u16*)(pDest+j)));
        j += 2;
    }
    
    (*pDestLen) = j;
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetGSMToAlphaUCS2,SUCCESS. uSrcLen:%u,(*pDestLen):%u",uSrcLen,(*pDestLen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSetHEXToAlpha
 *
 * DESCRIPTION
 *  Convert charset 'UCS2' data to alpha data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uAlpha>      Alphabet,It's value is in 'LIB_SMS_DCSAlphaEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvCharSetUCS2ToAlpha(u8 *pSrc,u16 uSrcLen,u8 uAlpha,u8 *pDest,u16 *pDestLen)
{
    u8 *pHexOct = NULL;
    u16 uHexOctLen = 0;
    bool bResult = FALSE;
    u16 uLimitLen = 0;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetUCS2ToAlpha,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if(FALSE == IS_SUPPORT_ALPHA(uAlpha))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetUCS2ToAlpha,FAIL! IS_SUPPORT_ALPHA FAIL! uAlpha:%u",uAlpha);
        return FALSE;
    }
    
    if((uSrcLen % 4) != 0)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetUCS2ToAlpha,FAIL! (uSrcLen % 4) != 0");
        return FALSE;
    }
    
    pHexOct = Ql_MEM_Alloc(uSrcLen / 2);
    if(NULL == pHexOct)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetUCS2ToAlpha,FAIL! Ql_MEM_Alloc FAIL! size:%u",(uSrcLen / 2));
        return FALSE;
    }
    
    uHexOctLen = (uSrcLen / 2);
    bResult = LIB_SMS_ConvHexStrToHexOct((char*)pSrc,uSrcLen,pHexOct,&uHexOctLen);
    if(FALSE == bResult)
    {
        Ql_MEM_Free(pHexOct);
        
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetUCS2ToAlpha,FAIL! LIB_SMS_ConvHexStrToHexOct FAIL!");
        return FALSE;
    }
    
    GET_CHARSET_UCS2_TO_ALPHA_LEN(uAlpha,pHexOct,uHexOctLen,uLimitLen);
    if((*pDestLen) < uLimitLen)
    {
        Ql_MEM_Free(pHexOct);
        
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetUCS2ToAlpha,FAIL! (*pDestLen) is too less. (*pDestLen):%u",(*pDestLen));
        return FALSE;
    }
    
    if(LIB_SMS_DCS_ALPHA_DEFAULT == uAlpha)
    {
        bResult = ConvCharSetUCS2ToAlphaDefault(pHexOct,uHexOctLen,pDest,pDestLen);
    }
    else if(LIB_SMS_DCS_ALPHA_UCS2 == uAlpha)
    {
        bResult = TRUE;
        SMS_MEM_COPY_ONLY(pHexOct,uHexOctLen,pDest,pDestLen);
    }
    else
    {
        bResult = TRUE;
        SMS_MEM_COPY_ONLY(pHexOct,uHexOctLen,pDest,pDestLen);        
    }    
    
    Ql_MEM_Free(pHexOct);
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetUCS2ToAlpha,SUCCESS. bResult:%d,(*pDestLen):%u",bResult,(*pDestLen));    
    
    return bResult;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSetHEXToAlpha
 *
 * DESCRIPTION
 *  Convert charset 'GSM' data to alpha data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uAlpha>      Alphabet,It's value is in 'LIB_SMS_DCSAlphaEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvCharSetGSMToAlpha(u8 *pSrc,u16 uSrcLen,u8 uAlpha,u8 *pDest,u16 *pDestLen)
{
    u16 uLimitLen = 0;
    bool bResult = FALSE;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetGSMToAlpha,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if(FALSE == IS_SUPPORT_ALPHA(uAlpha))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetGSMToAlpha,FAIL! IS_SUPPORT_ALPHA FAIL! uAlpha:%u",uAlpha);
        return FALSE;
    }
    
    GET_CHARSET_GSM_TO_ALPHA_LEN(uAlpha,pSrc,uSrcLen,uLimitLen);
    if((*pDestLen) < uLimitLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetGSMToAlpha,FAIL! (*pDestLen) is too less. (*pDestLen):%u,uLimitLen:%u",(*pDestLen),uLimitLen);
        return FALSE;
    }
    
    if(LIB_SMS_DCS_ALPHA_DEFAULT == uAlpha)
    {
        bResult = TRUE;
        SMS_MEM_COPY_ONLY(pSrc,uSrcLen,pDest,pDestLen);
    }
    else if(LIB_SMS_DCS_ALPHA_UCS2 == uAlpha)
    {
        bResult = ConvCharSetGSMToAlphaUCS2(pSrc,uSrcLen,pDest,pDestLen);
    }
    else
    {
        bResult = TRUE;
        SMS_MEM_COPY_ONLY(pSrc,uSrcLen,pDest,pDestLen);        
    }
    
    (*pDestLen) = uLimitLen;
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetGSMToAlpha,SUCCESS. bResult:%d,(*pDestLen):%u",bResult,(*pDestLen));    
    
    return bResult;
}

/*****************************************************************************
 * FUNCTION
 *  ConvCharSetHEXToAlpha
 *
 * DESCRIPTION
 *  Convert charset 'HEX' data to alpha data.
 *  
 * PARAMETERS
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uAlpha>      Alphabet,It's value is in 'LIB_SMS_DCSAlphaEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
static bool ConvCharSetHEXToAlpha(u8 *pSrc,u16 uSrcLen,u8 uAlpha,u8 *pDest,u16 *pDestLen)
{
    bool bResult = FALSE;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetHEXToAlpha,FAIL! Parameter is NULL.");
        return FALSE;
    }

    if(FALSE == IS_SUPPORT_ALPHA(uAlpha))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetHEXToAlpha,FAIL! IS_SUPPORT_ALPHA FAIL! uAlpha:%u",uAlpha);
        return FALSE;
    }
    
    bResult = LIB_SMS_IsValidHexStr((char*)pSrc,uSrcLen);
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetHEXToAlpha,FAIL! LIB_SMS_IsValidHexStr FAIL! This string is NOT a valid hex string.");
        return FALSE;
    }
    
    if((LIB_SMS_DCS_ALPHA_UCS2 == uAlpha) && ((uSrcLen % 4) != 0))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetHEXToAlpha,FAIL! uAlpha is LIB_SMS_DCS_ALPHA_UCS2,but ((uSrcLen % 4) != 0).");
        return FALSE;
    }
    
    if((*pDestLen) < (uSrcLen / 2))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetHEXToAlpha,FAIL! (*pDestLen) is too less. (*pDestLen):%u,Need length:%u",(*pDestLen),(uSrcLen / 2));
        return FALSE;
    }
    
    //Convert hex string to hex octet
    (*pDestLen) = (uSrcLen / 2);
    bResult = LIB_SMS_ConvHexStrToHexOct((char*)pSrc,uSrcLen,pDest,pDestLen);
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetHEXToAlpha,FAIL! LIB_SMS_ConvHexStrToHexOct FAIL!");
        return FALSE;
    }
    
    DBG_TRACE(sg_aDbgBuf,"Enter ConvCharSetHEXToAlpha,SUCCESS. uSrcLen:%u,(*pDestLen):%u",uSrcLen,(*pDestLen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  LIB_SMS_IsValidHexStr
 *
 * DESCRIPTION
 * Check the hex string is VALID or not.
 *  
 * PARAMETERS
 *  <pHexStr>     The pointer of hex string
 *  <uHexStrLen>  The length of hex string
 *
 * RETURNS
 *  TRUE:  The <pHexStr> is VALID.
 *  FALSE: The <pHexStr> is INVALID.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *****************************************************************************/
bool LIB_SMS_IsValidHexStr(char *pHexStr,u16 uHexStrLen)
{
    u16 i = 0;

    if(NULL == pHexStr)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_IsValidHexStr,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    for(i = 0; i < uHexStrLen; i++)
    {
        if(FALSE == IS_VALID_HEX_CHAR(pHexStr[i]))
        {
            DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_IsValidHexStr,FAIL! Find INVALID hex char. pHexStr[%u]:0x%x",i,pHexStr[i]);
            return FALSE;
        }
    }
    
    DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_IsValidHexStr,SUCCESS. uHexStrLen:%u",uHexStrLen);
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  LIB_SMS_ConvHexOctToHexStr
 *
 * DESCRIPTION
 *  This function is used to convert hex octet to hex string.
 *  
 * PARAMETERS
 *   <pSrc>      The pointer of source buffer
 *   <uSrcLen>   The length of source buffer
 *   <pDest>     The pointer of destination buffer
 *   <uDestLen>  The length of destination buffer
 *
 * RETURNS
 *   TRUE,  This function works SUCCESS.
 *   FALSE, This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
bool LIB_SMS_ConvHexOctToHexStr(const u8 *pSrc,u16 uSrcLen, char *pDest, u16 *pDestLen)
{
    u16 i = 0;
    u8 uByteHigh = 0;
    u8 uByteLow = 0;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvHexOctToHexStr,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if((*pDestLen) < (uSrcLen * 2))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvHexOctToHexStr,FAIL! (*pDestLen) is less. (*pDestLen):%u,Need length:%u",(*pDestLen),(uSrcLen * 2));
        return FALSE;
    }
    
    for(i = 0; i < uSrcLen; i++)
    {
        U8_UNPACKET_TO_SEMI(pSrc[i],uByteHigh,uByteLow);
        
        CONV_INT_TO_HEX_CHAR(uByteHigh,pDest[(2 * i) + 0]);
        CONV_INT_TO_HEX_CHAR(uByteLow,pDest[(2 * i) + 1]);
    }
    
    (*pDestLen) = (uSrcLen * 2);
    
    DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvHexOctToHexStr,SUCCESS. uSrcLen:%u,(*pDestLen):%u",uSrcLen,(*pDestLen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  LIB_SMS_ConvHexStrToHexOct
 *
 * DESCRIPTION
 *  This function is used to convert hex string to hex octet.
 *  
 * PARAMETERS
 *   <pSrc>      The pointer of source buffer
 *   <uSrcLen>   The length of source buffer
 *   <pDest>     The pointer of destination buffer
 *   <uDestLen>  The length of destination buffer
 *
 * RETURNS
 *   TRUE,  This function works SUCCESS.
 *   FALSE, This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
bool LIB_SMS_ConvHexStrToHexOct(const char *pSrc,u16 uSrcLen, u8 *pDest, u16 *pDestLen)
{
    u16 i = 0;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvHexStrToHexOct,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if(((uSrcLen % 2) != 0) || ((*pDestLen) < (uSrcLen / 2)))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvHexStrToHexOct,FAIL! uSrcLen or *pDestLen is INVALID. uSrcLen:%u,*pDestLen:%u",uSrcLen,(*pDestLen));
        return FALSE;
    }
    
    for(i = 0; i < uSrcLen; i += 2)
    {
        HEX_CHAR_PACKET_TO_U8(pSrc[i],pSrc[i+1],pDest[i/2]);
    }
    
    (*pDestLen) = (uSrcLen / 2);
    
    DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvHexStrToHexOct,SUCCESS. uSrcLen:%u,(*pDestLen):%u",uSrcLen,(*pDestLen));
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  LIB_SMS_ConvCharSetToAlpha
 *
 * DESCRIPTION
 *  This function is used to convert 'LIB_SMS_CharSetEnum' data to 'LIB_SMS_DCSAlphaEnum' data.
 *  
 * PARAMETERS
 *   <eCharSet>    Character set,It's value is in 'LIB_SMS_CharSetEnum'
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <uDCS>        <DCS> element in TPDU
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
bool LIB_SMS_ConvCharSetToAlpha(LIB_SMS_CharSetEnum eCharSet,u8 *pSrc,u16 uSrcLen,u8 uDCS,u8 *pDest,u16 *pDestLen)
{
    bool bResult = FALSE;
    u8 uAlpha = 0;
    
    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvCharSetToAlpha,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if(FALSE == LIB_SMS_IS_SUPPORT_CHARSET(eCharSet))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvAlphaToCharSet,FAIL! LIB_SMS_IS_SUPPORT_CHARSET FAIL! eCharSet:%d",eCharSet);
        return FALSE;
    }    
    
    LIB_SMS_GET_ALPHA_IN_PDU_DCS(uDCS,uAlpha);
    
    if(LIB_SMS_CHARSET_GSM == eCharSet)
    {
        bResult = ConvCharSetGSMToAlpha(pSrc,uSrcLen,uAlpha,pDest,pDestLen);
    }
    else if(LIB_SMS_CHARSET_UCS2 == eCharSet)
    {
        bResult = ConvCharSetUCS2ToAlpha(pSrc,uSrcLen,uAlpha,pDest,pDestLen);
    }
    //<2015/06/04-ROTVG00006-P08-Vicent.Gao,<[SMS] Add support for CharSet: IRA.>
    else if(LIB_SMS_CHARSET_IRA == eCharSet)
    {
        bResult = ConvCharSetIRAToAlpha(pSrc,uSrcLen,uAlpha,pDest,pDestLen);
    }
    //>2015/06/04-ROTVG00006-P08-Vicent.Gao    
    //<2015/06/05-ROTVG00006-P09-Vicent.Gao,<[SMS] Add support for CharSet: 8859-1.>
    else if(LIB_SMS_CHARSET_8859_1 == eCharSet)
    {
        bResult = ConvCharSet88591ToAlpha(pSrc,uSrcLen,uAlpha,pDest,pDestLen);
    }
    //>2015/06/05-ROTVG00006-P09-Vicent.Gao    
    else
    {
        bResult = ConvCharSetHEXToAlpha(pSrc,uSrcLen,uAlpha,pDest,pDestLen);
    }
    
    return bResult;
}

/*****************************************************************************
 * FUNCTION
 *  LIB_SMS_ConvAlphaToCharSet
 *
 * DESCRIPTION
 *  This function is used to convert 'LIB_SMS_DCSAlphaEnum' data to 'LIB_SMS_CharSetEnum' data.
 *  
 * PARAMETERS
 *   <uDCS>        <DCS> element in TPDU
 *   <pSrc>        The pointer of source data
 *   <uSrcLen>     The length of source data
 *   <eCharSet>    Character set,It's value is in 'LIB_SMS_CharSetEnum'
 *   <pDest>       The length of destination data
 *   <pDestLen>    The length of destination length
 *
 * RETURNS
 *   FALSE,  This function works FAIL!
 *   TRUE,   This function works SUCCESS.
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *  2. Warning: Before call this function,MUST set <*pDestLen> to a value which specify the rest bytes of <pDest> buffer.
 *  3. If this function works SUCCESS,<*pDestLen> will return the bytes of <pDest> buffer that has been written.
 *****************************************************************************/ 
bool LIB_SMS_ConvAlphaToCharSet(u8 uDCS,u8 *pSrc,u16 uSrcLen,LIB_SMS_CharSetEnum eCharSet,u8 *pDest,u16 *pDestLen)
{
    bool bResult = FALSE;
    u8 uAlpha = 0;

    if((NULL == pSrc) || (NULL == pDest) || (NULL == pDestLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvAlphaToCharSet,FAIL! Parameter is NULL.");
        return FALSE;
    }

    if(FALSE == LIB_SMS_IS_SUPPORT_CHARSET(eCharSet))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvAlphaToCharSet,FAIL! LIB_SMS_IS_SUPPORT_CHARSET FAIL! eCharSet:%d",eCharSet);
        return FALSE;
    }

    LIB_SMS_GET_ALPHA_IN_PDU_DCS(uDCS,uAlpha);
    
    if(LIB_SMS_DCS_ALPHA_DEFAULT == uAlpha)
    {
        bResult = ConvAlphaDefaultToCharSet(pSrc,uSrcLen,eCharSet,pDest,pDestLen);
    }
    else if(LIB_SMS_DCS_ALPHA_UCS2 == uAlpha)
    {
        bResult = LIB_SMS_ConvHexOctToHexStr(pSrc,uSrcLen,(char*)pDest,pDestLen); //Ignore <CharSet>,only display its hex string
    }
    else
    {
        bResult = ConvAlpha8BitToCharSet(pSrc,uSrcLen,eCharSet,pDest,pDestLen);
    }
    
    DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_ConvAlphaToCharSet,SUCCESS. uAlpha:%u,eCharSet:%d,uSrcLen:%u,(*pDestLen):%u",uAlpha,eCharSet,uSrcLen,(*pDestLen));
    
    return bResult;
}

/*****************************************************************************
 * FUNCTION
 *  LIB_SMS_DecodeDCS
 *
 * DESCRIPTION
 *  This function is used to decode <DCS> in TPDU
 *  
 * PARAMETERS
 *   <uDCS>       The <DCS> in TPDU
 *   <pMsgType>   The pointer of message type which is same as 'DCSMsgTypeEnum'
 *   <pAlpha>     The pointer of alphabet which is same as 'DCSAlphaEnum'
 *   <pMsgClass>  The pointer of message class which is same as 'LIB_SMS_DCSMsgClassEnum'
 *   <pCompress>  The pointer of compress flag which is same as 'LIB_SMS_DCSCompressEnum'
 *
 * RETURNS
 *   VOID
 *
 * NOTE
 *  1. If you DONOT want to get certain parameter's value,please set it to NULL.
 *****************************************************************************/
void LIB_SMS_DecodeDCS(u8 uDCS,u8 *pMsgType,u8 *pAlpha,u8 *pMsgClass,u8 *pCompress)
{
    u8 uGroup = ((uDCS & 0xF0) >> 4);
    
    //Default setting
    LIB_SMS_SET_POINTER_VAL(pMsgType,DCS_MSG_TYPE_RESERVED);
    LIB_SMS_SET_POINTER_VAL(pAlpha,LIB_SMS_DCS_ALPHA_DEFAULT);
    LIB_SMS_SET_POINTER_VAL(pMsgClass,LIB_SMS_DCS_MSG_CLASS_RESERVED);
    LIB_SMS_SET_POINTER_VAL(pCompress,LIB_SMS_DCS_COMP_UNCOMPRESSED);
    
    if(0x00 == (uGroup & 0x0C))  //Group: 00xx (General Data Coding indication)
    {
        LIB_SMS_SET_POINTER_VAL(pMsgType,DCS_MSG_TYPE_DATA_CODING_IND);
    
        if(uDCS & 0x20)  //Bit 5, if set to 1, indicates the text is compressed using the GSM standard compression algorithm. ( see GSM TS 03.42)
        {
            LIB_SMS_SET_POINTER_VAL(pCompress,LIB_SMS_DCS_COMP_COMPRESSED);
        }
        else  //Bit 5, if set to 0, indicates the text is uncompressed
        {
            LIB_SMS_SET_POINTER_VAL(pCompress,LIB_SMS_DCS_COMP_UNCOMPRESSED);
        }
        
        if(uDCS & 0x10)  //Bit 4, if set to 1, indicates that bits 1 to 0 have a message class
        {
            LIB_SMS_SET_POINTER_VAL(pMsgClass,(uDCS & 0x03));
        }
        else  //Bit 4, if set to 0, indicates that bits 1 to 0 are reserved and have no message class meaning
        {
            LIB_SMS_SET_POINTER_VAL(pMsgClass,LIB_SMS_DCS_MSG_CLASS_RESERVED);
        }
        
        if(0x03 == ((uDCS & 0x0C) >> 2))
        {
            LIB_SMS_SET_POINTER_VAL(pAlpha,LIB_SMS_DCS_ALPHA_DEFAULT);
        }
        else
        {
            LIB_SMS_SET_POINTER_VAL(pAlpha,((uDCS & 0x0C) >> 2));
        }
    }
    else if(0x0C == uGroup)  //Group: 1100 (Message Waiting Indication Group: Discard Message)
    {
        LIB_SMS_SET_POINTER_VAL(pMsgType,DCS_MSG_TYPE_WAITING_DISCARD_IND);
        LIB_SMS_SET_POINTER_VAL(pAlpha,LIB_SMS_DCS_ALPHA_DEFAULT);
    }
    else if(0x0D == uGroup)  //Group: 1101 (Message Waiting Indication Group: Discard Message)
    {
        LIB_SMS_SET_POINTER_VAL(pMsgType,DCS_MSG_TYPE_WAITING_STORE_IND);
        LIB_SMS_SET_POINTER_VAL(pAlpha,LIB_SMS_DCS_ALPHA_DEFAULT);
    }
    else if(0x0E == uGroup)  //Group: 1110 (Message Waiting Indication Group: Store Message)
    {
        LIB_SMS_SET_POINTER_VAL(pMsgType,DCS_MSG_TYPE_WAITING_STORE_IND);
        LIB_SMS_SET_POINTER_VAL(pAlpha,LIB_SMS_DCS_ALPHA_UCS2);
    }
    else if(0x0F == uGroup)  //Group: 1111 (Data coding/message class)
    {
        LIB_SMS_SET_POINTER_VAL(pMsgType,DCS_MSG_TYPE_DATA_CODING_IND);
    
        //<2016/12/22-ROTVG00006C001-P02-Joyce.Sun,<[COMMON] Segment 2==>Fix common issues>
        //if(uDCS & 0x02)  //Bit 2 Message coding:
        if(uDCS & 0x04)
        //>2016/12/22-ROTVG00006C001-P02-Joyce.Sun
        {
            LIB_SMS_SET_POINTER_VAL(pAlpha,LIB_SMS_DCS_ALPHA_8BIT_DATA);
        }
        else
        {
            LIB_SMS_SET_POINTER_VAL(pAlpha,LIB_SMS_DCS_ALPHA_DEFAULT);
        }
        
        LIB_SMS_SET_POINTER_VAL(pMsgClass,(uDCS & 0x03));
    }
    else
    {
        //Don't do anything. ONLY use default setting
    }
    
    //DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_DecodeDCS,SUCCESS.");    
    
    return;
}

/*****************************************************************************
 * FUNCTION
 *  LIB_SMS_DecodePDUStr
 *
 * DESCRIPTION
 *  Decode PDU string to 'LIB_SMS_PDUParamStruct' data.
 *  
 * PARAMETERS
 *   <pPDUStr>      The pointer of PDU string
 *   <uPDUStrLen>   The length of PDU string
 *   <pParam>       The pointer of 'LIB_SMS_PDUParamStruct' data
 *
 * RETURNS
 *   TRUE,  This function works SUCCESS.
 *   FALSE, This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *****************************************************************************/ 
bool LIB_SMS_DecodePDUStr(char *pPDUStr,u16 uPDUStrLen,LIB_SMS_PDUParamStruct *pParam)
{
    bool bResult = FALSE;
    u8 *pPDUOct = NULL;
    u16 uPDUOctLen = 0;

    u8 uSCALen = 0;  //The length of <SCA>
    u8 uMTI = 0;      //<MTI> in <FO>
        
    if((NULL == pPDUStr) || (NULL == pParam) || (0 == uPDUStrLen))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_DecodePDUStr,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if((uPDUStrLen % 2) != 0)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_DecodePDUStr,FAIL! uPDUStrLen is INVALID. uPDUStrLen:%u",uPDUStrLen);
        return FALSE;
    }
    
    bResult = LIB_SMS_IsValidHexStr(pPDUStr,uPDUStrLen);
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_DecodePDUStr,FAIL! LIB_SMS_IsValidHexStr FAIL!");
        return FALSE;
    }

    //Allocate needed memory
    pPDUOct = (u8*)Ql_MEM_Alloc(LIB_SMS_PDU_BUF_MAX_LEN);
    if(NULL == pPDUOct)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_DecodePDUStr,FAIL! Ql_MEM_Alloc FAIL! size:%u",LIB_SMS_PDU_BUF_MAX_LEN);
        return FALSE;
    }
    
    //Convert PDU string to PDU octet
    uPDUOctLen = LIB_SMS_PDU_BUF_MAX_LEN;
    bResult = LIB_SMS_ConvHexStrToHexOct(pPDUStr,uPDUStrLen,pPDUOct,(u16*)&uPDUOctLen);
    if(FALSE == bResult)
    {
        Ql_MEM_Free(pPDUOct);
        
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_DecodePDUStr,FAIL! LIB_SMS_ConvHexStrToHexOct FAIL!");
        return FALSE;
    }
    
    uSCALen = pPDUOct[0];

    //Decode <SCA>
    bResult = DecodeAddress((pPDUOct + 1),uSCALen,(&(pParam->sSCA)));
    if(FALSE == bResult)
    {
        Ql_MEM_Free(pPDUOct);
        
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_DecodePDUStr,FAIL! DecodeAddress FAIL!");
        return FALSE;
    }

    (pParam->uFO) = pPDUOct[uSCALen+1];  //Record <FO>
    
    uMTI = LIB_SMS_GET_MSG_TYPE_IN_PDU_FO(pPDUOct[uSCALen+1]);
    if(LIB_SMS_PDU_TYPE_DELIVER == uMTI)
    {
        bResult = DecodeDeliverTPDUOct(&pPDUOct[uSCALen+1],(uPDUOctLen - (uSCALen + 1)),&((pParam->sParam).sDeliverParam));
    }
    else if(LIB_SMS_PDU_TYPE_SUBMIT == uMTI)
    {
        bResult = DecodeSubmitTPDUOct(&pPDUOct[uSCALen+1],(uPDUOctLen - (uSCALen + 1)),&((pParam->sParam).sSubmitParam));
    }
    else
    {
        Ql_MEM_Free(pPDUOct);
    
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_DecodePDUStr,FAIL! NOT supported MTI. uMTI:%d",uMTI);
        return FALSE;
    }

    Ql_MEM_Free(pPDUOct);
    
    DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_DecodePDUStr,SUCCESS. uPDUStrLen:%u,uPDUOctLen:%u,uSCALen:%u,uMTI:%d,bResult:%d",uPDUStrLen,uPDUOctLen,uSCALen,uMTI,bResult);
   
    return bResult;
}

/*****************************************************************************
 * FUNCTION
 *  LIB_SMS_EncodeSubmitPDU
 *
 * DESCRIPTION
 *  This function is used to encode SUBMIT-PDU according to given parameters
 *  
 * PARAMETERS
 *  <pParam>    The pointer of 'LIB_SMS_PDUParamStruct' data
 *  <pInfo>     The pointer of 'LIB_SMS_PDUInfoStruct' data
 *
 * RETURNS
 *  TRUE:  This function works SUCESS.
 *  FALSE: This function works FAIL!
 *
 * NOTE
 *  1. Please ensure all parameters are VALID.
 *****************************************************************************/
bool LIB_SMS_EncodeSubmitPDU(LIB_SMS_PDUParamStruct *pParam,LIB_SMS_PDUInfoStruct *pInfo)
{
    bool bResult = FALSE;
    LIB_SMS_SubmitPDUParamStruct *pSubmitParam = NULL;
    u8 *pTmp = NULL;
    u16 uTmpLen = 0;
    u8 uVPF = 0;
    u8 uAlpha = 0;
    bool bConSmsFlag = 0;
    u8 uUDType = 0;
    u16 uUDMaxLen = 0;

    const u16 uTotBytes = sizeof(pInfo->aPDUOct);
    const u8 *pHeadAddr = (pInfo->aPDUOct);

	if((NULL == pParam) || (NULL == pInfo))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_EncodeSubmitPDU,FAIL! Parameter is NULL.");
        return FALSE;
    }
    
    if(LIB_SMS_PDU_TYPE_SUBMIT != LIB_SMS_GET_MSG_TYPE_IN_PDU_FO(pParam->uFO))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_EncodeSubmitPDU,FAIL! NOT a SUBMIT-PDU. uFO:0x%x",(pParam->uFO));
        return FALSE;
    }
    
    pTmp = (pInfo->aPDUOct);
    
    uVPF = LIB_SMS_GET_VPF_IN_SUBMIT_PDU_FO(pParam->uFO);
    GET_CON_SMS_FLAG_VAL((pParam->uFO),bConSmsFlag);
    
    //Encode <SCA>
    (*pTmp) = COUNT_BCD_LEN_TO_BYTE((pParam->sSCA).uLen);   //Encode <SCA> length
    if((*pTmp) > 0)
    {
        (*pTmp) += 1;  //Add 1 byte for SCA type byte
    }

    pTmp += 1;  //Skip <SCA> length byte

    GET_BUFFER_REST_BYTE(uTotBytes,pHeadAddr,pTmp,uTmpLen);
    bResult = EncodeAddress(&(pParam->sSCA),pTmp,&uTmpLen,FALSE);
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_EncodeSubmitPDU,FAIL! EncodeAddress FAIL!");
        return FALSE;
    }
    
    pTmp += uTmpLen;  //Skip <SCA> number
    
    (*(pTmp++)) = (pParam->uFO);  //Encode <FO>
    (*(pTmp++)) = SUBMIT_PDU_DEFAULT_MR_VAL;  //Encode <MR>
    
    pSubmitParam = &((pParam->sParam).sSubmitParam);    
    
    //Encode <DA>    
    (*(pTmp++)) = ((pSubmitParam->sDA).uLen); //Encode <DA> length

    GET_BUFFER_REST_BYTE(uTotBytes,pHeadAddr,pTmp,uTmpLen);
    bResult = EncodeAddress(&(pSubmitParam->sDA),pTmp,&uTmpLen,TRUE);
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_EncodeSubmitPDU,FAIL! EncodeAddress FAIL!");
        return FALSE;
    }
    
    pTmp += uTmpLen;  //Skip <DA> number
    
    if(FALSE == IS_SUPPORT_PID(pSubmitParam->uPID))
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_EncodeSubmitPDU,FAIL! IS_SUPPORT_PID FAIL! uPID:%d",(pSubmitParam->uPID));
        return FALSE;
    }
    
    (*(pTmp++)) = (pSubmitParam->uPID);  //Encode <PID>

    IS_VALID_DCS_IN_PDU((pSubmitParam->uDCS),bResult);
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_EncodeSubmitPDU,FAIL! IS_VALID_DCS_IN_PDU FAIL! DCS:0x%x",(pSubmitParam->uDCS));
        return FALSE;
    }
    
    (*(pTmp++)) = (pSubmitParam->uDCS);  //Encode <DCS>

    LIB_SMS_DecodeDCS((*(pTmp - 1)),NULL,&uAlpha,NULL,NULL);
    
    GET_BUFFER_REST_BYTE(uTotBytes,pHeadAddr,pTmp,uTmpLen);
    bResult = EncodeValidityPeriod(uVPF,&(pSubmitParam->sVP),pTmp,&uTmpLen);
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_EncodeSubmitPDU,FAIL! EncodeValidityPeriod FAIL!");
        return FALSE;
    }    
    
    pTmp += uTmpLen;  //Skip <VP>
    
    if(FALSE == bConSmsFlag)
    {
        uUDType = LIB_SMS_UD_TYPE_NORMAL;
    }
    else
    {
        uUDType = ((pSubmitParam->sConSMSParam).uMsgType);
    }    

    GET_USER_DATA_MAX_LEN(uAlpha,uUDMaxLen);
    GET_USER_DATA_LEN(uAlpha,uUDType,((pSubmitParam->sUserData).uLen),(*pTmp));//Encode <UDL>
    if((*pTmp) > uUDMaxLen)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_EncodeSubmitPDU,FAIL! UDL is too larger. UDL:%d,uUDMaxLen:%d",(*pTmp),uUDMaxLen);
        return FALSE;
    }

    pTmp += 1;  //Skip <UDL>

    if(TRUE == bConSmsFlag)
    {
        GET_BUFFER_REST_BYTE(uTotBytes,pHeadAddr,pTmp,uTmpLen);
        bResult = EncodeUserDataHeader(&(pSubmitParam->sConSMSParam),pTmp,&uTmpLen);
        if(FALSE == bResult)
        {
            DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_EncodeSubmitPDU,FAIL! EncodeUserDataHeader FAIL!");
            return FALSE;
        }
        
        pTmp += uTmpLen;  //Skip <UDH> if <UDH> is present.
    }
    
    GET_BUFFER_REST_BYTE(uTotBytes,pHeadAddr,pTmp,uTmpLen);
    bResult = EncodeUserDataBody(
                uAlpha,
                uUDType,
                &(pSubmitParam->sUserData),
                pTmp,
                &uTmpLen);
                
    if(FALSE == bResult)
    {
        DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_EncodeSubmitPDU,FAIL! EncodeUserDataBody FAIL!");
        return FALSE;
    }
    
    pTmp += uTmpLen;  //Skip <UD> if <UD> is present.
    
    (pInfo->uLen) = (pTmp - (pInfo->aPDUOct));
    
    DBG_TRACE(sg_aDbgBuf,"Enter LIB_SMS_EncodeSubmitPDU,SUCCESS. uLen:%u",(pInfo->uLen));
    
    return TRUE;
}

#endif  //#if (defined(__OCPU_RIL_SUPPORT__) && defined(__OCPU_RIL_SMS_SUPPORT__))

