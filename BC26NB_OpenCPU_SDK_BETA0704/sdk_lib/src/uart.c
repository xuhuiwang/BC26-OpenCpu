#include "ql_ocpu_api.h"
#include "ql_func.h"
#include "ql_uart.h"
#include "ql_error.h "

#define GPIO_MODE0_REG_ADDR  0xA0020C00
#define GPIO_MODE1_REG_ADDR  0xA0020C10

static _api_Ql_UART_Register_t          m_Ql_UART_Register      = NULL;
static _api_Ql_UART_Open_t              m_Ql_UART_Open          = NULL;
static _api_Ql_UART_OpenEx_t            m_Ql_UART_OpenEx        = NULL;
static _api_Ql_UART_Write_t             m_Ql_UART_Write         = NULL;
static _api_Ql_UART_Read_t              m_Ql_UART_Read          = NULL;
static _api_Ql_UART_Close_t             m_Ql_UART_Close         = NULL;

s32 Ql_UART_Register(Enum_SerialPort port, CallBack_UART_Notify callback_uart,void * customizePara)
{
    if (m_Ql_UART_Register==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_UART_Register");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_UART_Register");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_Ql_UART_Register=(_api_Ql_UART_Register_t)ptr;
    }
    return m_Ql_UART_Register(port,callback_uart,customizePara);
}
s32 Ql_UART_Open(Enum_SerialPort port,u32 baudrate, Enum_FlowCtrl flowCtrl)
{
    if (m_Ql_UART_Open==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_UART_Open");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_UART_Open");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_Ql_UART_Open=(_api_Ql_UART_Open_t)ptr;
    }
    return m_Ql_UART_Open(port,baudrate,flowCtrl);
}
s32 Ql_UART_OpenEx(Enum_SerialPort port, ST_UARTDCB *dcb)
{
    if (m_Ql_UART_OpenEx==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_UART_OpenEx");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_UART_OpenEx");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_Ql_UART_OpenEx=(_api_Ql_UART_OpenEx_t)ptr;
    }
    return m_Ql_UART_OpenEx(port,dcb);
}
s32 Ql_UART_Write(Enum_SerialPort port, u8* data, u32 writeLen )
{
    if (m_Ql_UART_Write==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_UART_Write");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_UART_Write");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_Ql_UART_Write=(_api_Ql_UART_Write_t)ptr;
    }
    return m_Ql_UART_Write(port,data,writeLen);
}	
s32 Ql_UART_Read(Enum_SerialPort port, u8* data, u32 readLen)
{
    if (m_Ql_UART_Read==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_UART_Read");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_UART_Read");		
            return Ql_RET_NOT_SUPPORT;
        }
        m_Ql_UART_Read=(_api_Ql_UART_Read_t)ptr;
    }
    return m_Ql_UART_Read(port,data,readLen);
}		

void Ql_UART_Close(Enum_SerialPort port)
{
    if (m_Ql_UART_Close==NULL)
    {
        s32 ptr = m_get_api_ptr("Ql_UART_Close");
        if (0 == ptr)
        {
            //Ql_Debug_Trace(api_notsup, "Ql_UART_Close");		
            return ;
        }
        m_Ql_UART_Close=(_api_Ql_UART_Close_t)ptr;
    }
    m_Ql_UART_Close(port);
}
