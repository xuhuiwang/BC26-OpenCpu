#ifndef __RIL_DEF_H__
#define __RIL_DEF_H__
#include "ql_uart.h"
#include "ql_stdlib.h"
#include "ql_trace.h"

/**************************************************************
 * User Message ID Definition
 **************************************************************/
#define     MSG_ID_QL_START     0x8000
#define     MSG_ID_SEND_AT      0x06
#define     MSG_ID_URC_IND      0x07
#define     MSG_ID_ATRSP_IND    0x03
#define     MSG_ID_MD_URC_IND   0x04
#define     MSG_ID_MD_ATRSP_IND 0x05


#define  RIL_MUTEX_NAME       "RIL_MUTEX"
#define  RIL_EVTGRP_NAME      "RIL_EVETNGRP"

#define  RIL_MAX_AT_LEN    2*512
#define  RIL_MAX_URC_LEN   512  /* Max length of the URC,Does not contain flash letter*/
#define  SERIAL_RX_BUFFER_LEN  2048
#define  RIL_MAX_AT_TIMEOUT    3*60*1000 // AT send timeout 3min

typedef enum {
    RIL_EVENT_NOWAIT       = (0x00000000),
    RIL_EVENT_AT_RESPONSE  = (0x00000001),
    RIL_EVENT_AT_CALLBACK  = (0x00000002)
}Enum_RIL_WaitEvent;

typedef struct {
    s32  id;
    u32  interval;
    bool autoRepeat;
    bool runState;
}ST_RILTimer;

typedef struct {
    char *line;
    u32 len;
}ST_RILGetLine;

#define RIL_DEBUG_ENABLE 0
#define RIL_DEBUG_PORT  UART_PORT1  // Need to open the port if non-uart_port2 is specified
#if RIL_DEBUG_ENABLE > 0
#define RIL_DEBUG(BUF,...) \
    Ql_memset((char *)(BUF), 0, sizeof(BUF)); \
    Ql_sprintf((char *)(BUF),__VA_ARGS__); \
    if (UART_PORT1 == RIL_DEBUG_PORT) \
    {\
        Ql_Debug_Trace(BUF);\
    } else {\
        Ql_UART_Write(RIL_DEBUG_PORT, (u8*)(BUF), Ql_strlen((const char *) (BUF)));\
    }
#else
#define RIL_DEBUG(BUF,...) 
#endif

#endif  // __RIL_DEF_H__
