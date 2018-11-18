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
 *   example_gpio.c
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   This example demonstrates how to program a GPIO pin in OpenCPU.
 *   This example choose PINNAME_NETLIGHT pin as GPIO.
 *
 *   All debug information will be output through DEBUG port.
 *
 *   The "Enum_PinName" enumeration defines all the GPIO pins.
 *
 * Usage:
 * ------
 *   Compile & Run:
 *
 *     Set "C_PREDEF=-D __EXAMPLE_GPIO__" in gcc_makefile file. And compile the 
 *     app using "make clean/new".
 *     Download image bin to module to run.
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
#ifdef __EXAMPLE_GPIO__
#include "ql_trace.h"
#include "ql_system.h"
#include "ql_gpio.h"
#include "ql_stdlib.h"
#include "ql_error.h"
#include "ql_uart.h"

//UART_PORT0 ->主串口
//UART_PORT1 ->AUX串口
//UART_PORT2 ->DEBUG串口

#define DEBUG_ENABLE 1
#if DEBUG_ENABLE > 0
#define DEBUG_PORT  UART_PORT2
#define DBG_BUF_LEN   512
static char DBG_BUFFER[DBG_BUF_LEN];
#define APP_DEBUG(FORMAT,...) {\
    Ql_memset(DBG_BUFFER, 0, DBG_BUF_LEN);\
    Ql_sprintf(DBG_BUFFER,FORMAT,##__VA_ARGS__); \
    if (UART_PORT2 == (DEBUG_PORT)) \
    {\
        Ql_Debug_Trace(DBG_BUFFER);\
    } else {\
        Ql_UART_Write((Enum_SerialPort)(DEBUG_PORT), (u8*)(DBG_BUFFER), Ql_strlen((const char *)(DBG_BUFFER)));\
    }\
}
#else
#define APP_DEBUG(FORMAT,...) 
#endif

// Define the UART port 
static Enum_SerialPort m_myUartPort  = DEBUG_PORT;
static void CallBack_UART_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara)
{
     
}


/*GPIO 电平反转函数*/
void GPIO_TogglePin(Enum_PinName pinName)
{
    if(Ql_GPIO_GetLevel(pinName) == 0)
    {
       Ql_GPIO_SetLevel(pinName,PINLEVEL_HIGH ); 
    }
    else
    {
        Ql_GPIO_SetLevel(pinName,PINLEVEL_LOW ); 
    }

}

static void GPIO_Program(void)
{
    // Specify a GPIO pin
    Enum_PinName  gpioPin = PINNAME_NETLIGHT;

    // Define the initial level for GPIO pin
    Enum_PinLevel gpioLvl = PINLEVEL_HIGH;

    // Initialize the GPIO pin (output high level, pull up)
    Ql_GPIO_Init(gpioPin, PINDIRECTION_OUT, gpioLvl, PINPULLSEL_PULLUP);  
    Ql_GPIO_Init(PINNAME_GPIO1, PINDIRECTION_IN, PINLEVEL_HIGH, PINPULLSEL_PULLUP);
    Ql_GPIO_Init(PINNAME_GPIO2, PINDIRECTION_IN, PINLEVEL_HIGH, PINPULLSEL_PULLUP);
    Ql_GPIO_Init(PINNAME_GPIO3, PINDIRECTION_IN, PINLEVEL_HIGH, PINPULLSEL_PULLUP);
    APP_DEBUG("<-- Initialize PINNAME_NETLIGHT: output, high level, pull up -->\r\n");
    APP_DEBUG("<-- Initialize PINNAME_GPIO1: output, high level, pull up -->\r\n");
/*
    // Get the direction of GPIO
    APP_DEBUG("<-- Get the GPIO direction: %d -->\r\n", Ql_GPIO_GetDirection(gpioPin));

    // Get the level value of GPIO
    APP_DEBUG("<-- Get the GPIO level value: %d -->\r\n\r\n", Ql_GPIO_GetLevel(gpioPin));

    // Set the GPIO level to low after 500ms.
    APP_DEBUG("<-- Set the GPIO level to low after 500ms -->\r\n");
    Ql_Sleep(500);
    Ql_GPIO_SetLevel(gpioPin, PINLEVEL_LOW);
    APP_DEBUG("<-- Get the GPIO level value: %d -->\r\n\r\n", Ql_GPIO_GetLevel(gpioPin));

    // Set the GPIO level to high after 500ms.
    APP_DEBUG("<-- Set the GPIO level to high after 500ms -->\r\n");
    Ql_Sleep(500);
    Ql_GPIO_SetLevel(gpioPin, PINLEVEL_LOW);
    APP_DEBUG("<-- Get the GPIO level value: %d -->\r\n", Ql_GPIO_GetLevel(gpioPin));
 */
}

float tt=1.25;

/************************************************************************/
/* The entrance for this example application                            */
/************************************************************************/
void proc_main_task(s32 taskId)
{
    s32 ret;
    ST_MSG msg;
    u16 count = 0;

     GPIO_Program();
    if(Ql_GPIO_GetLevel(PINNAME_GPIO1) == 0)
         APP_DEBUG("\r\n<--DaBai PINNAME_GPIO1 EINT -->\r\n");
      if(Ql_GPIO_GetLevel(PINNAME_GPIO2) == 0)
         APP_DEBUG("\r\n<--DaBai PINNAME_GPIO2 EINT -->\r\n");
      if(Ql_GPIO_GetLevel(PINNAME_GPIO3) == 0)
         APP_DEBUG("\r\n<--DaBai PINNAME_GPIO3 EINT -->\r\n");
    // Register & open UART port
    ret = Ql_UART_Register(m_myUartPort, CallBack_UART_Hdlr, NULL);
    if (ret < QL_RET_OK)
    {
        Ql_Debug_Trace("Fail to register serial port[%d], ret=%d\r\n", m_myUartPort, ret);
    }
    ret = Ql_UART_Open(m_myUartPort, 115200, FC_NONE);
    if (ret < QL_RET_OK)
    {
        Ql_Debug_Trace("Fail to open serial port[%d], ret=%d\r\n", m_myUartPort, ret);
    }
    
     APP_DEBUG("\r\n<-- DaBai OpenCPU: GPIO Example -->\r\n");
    // Start to program GPIO pin

    //Ql_SleepDisable();
    Ql_SleepEnable();
    // Start message loop of this task
    while (TRUE)
    {
        //Ql_OS_GetMessage(&msg);

        if(Ql_GPIO_GetLevel(PINNAME_GPIO1) == 0)
             APP_DEBUG("\r\n<--DaBai PINNAME_GPIO1 EINT -->\r\n");
          if(Ql_GPIO_GetLevel(PINNAME_GPIO2) == 0)
             APP_DEBUG("\r\n<--DaBai PINNAME_GPIO2 EINT -->\r\n");
          if(Ql_GPIO_GetLevel(PINNAME_GPIO3) == 0)
             APP_DEBUG("\r\n<--DaBai PINNAME_GPIO3 EINT -->\r\n");
        Ql_Sleep(100);
        GPIO_TogglePin(PINNAME_NETLIGHT);
/*
        //GPIO_TogglePin(PINNAME_GPIO1);
        //APP_DEBUG("\r\n<--DaBai OpenCPU:  GPIO_Toggle loop -->\r\n");

        if(count++ > 100)
        {
            count = 101;
            Ql_GPIO_SetLevel(PINNAME_NETLIGHT,PINLEVEL_LOW ); 
            //Ql_GPIO_SetLevel(PINNAME_GPIO1,PINLEVEL_LOW ); 
            Ql_Sleep(100000);
            Ql_SleepEnable();
        }    
    */         
        switch(msg.message)
        {
        case MSG_ID_USER_START:
            break;
        default:
            break;
        }
    }
}

#endif //__EXAMPLE_GPIO__
