#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__
#include "ql_type.h"
#include "ql_gpio.h"

typedef enum {
    KBR0_KBC0   = 0x01,
    KBR0_KBC1   = 0x02,
    KBR0_KBC2   = 0x04,
    KBR0_KBC3   = 0x05,
    KBR0_KBC4   = 0x06,
    KBR1_KBC0   = 0x12,
    KBR1_KBC1   = 0x13,
    KBR1_KBC2   = 0x15,
    KBR1_KBC3   = 0x16,
    KBR1_KBC4   = 0x1a,
    KBR2_KBC0   = 0x25,
    KBR2_KBC1   = 0x26,
    KBR2_KBC2   = 0x28,
    KBR2_KBC3   = 0x29,
    KBR2_KBC4   = 0x2a,
    KBR3_KBC0   = 0x2e,
    KBR3_KBC1   = 0x2f,
    KBR3_KBC2   = 0x31,
    KBR3_KBC3   = 0x32,
    KBR3_KBC4   = 0x33,
    KBR4_KBC0   = 0x40,
    KBR4_KBC1   = 0x41,
    KBR4_KBC2   = 0x43,
    KBR4_KBC3   = 0x44,
    KBR4_KBC4   = 0x45,
    KEYBORAD_END
}Enum_KeyCode;

/******************************************************************************
* Function:     Ql_KB_Register
*  
* Description:
*                This function registers a callback for keyboard. The callback
*                is invoked when some key is pressed down or released.
*
* Parameters:    
*                callback_kb:
*                   A pointer to Callback_Keyboard.
*                       
*                keyCode:
*                   The key code value of the pressed down key.
*                isKeyDown:
*                   If the key is pressed down or not. TRUE is key-down, FALSE is key up.
* Return:  
*               QL_RET_OK indicates success.
*               QL_RET_ERR_INVALID_PARAMETER indicates callback_kb is null. 
******************************************************************************/
typedef s32 (*Callback_Keyboard)(u32 keyCode, bool isKeyDown);
s32 Ql_KB_Register(Callback_Keyboard  callback_kb);

/******************************************************************************
* Function:     Ql_KB_Config
*  
* Description:
*                This function configures keyboard's debounce. The default debounce
*                for keyboard is 16ms. And the maximum value that can be set is 512ms.
*
* Parameters:    
*                debounce:
*                   Keyboard debounce, unit in ms. The max value is 512.
*                       
*                reserved:
*                   Reserved parameter, the parameter will be ignored whatever is input.
* Return:  
*               QL_RET_OK indicates success.
*               QL_RET_ERR_INVALID_PARAMETER indicates callback_kb is null. 
******************************************************************************/
s32 Ql_KB_Config(s32 debounce, s32 reserved);

/******************************************************************************
* Function:     Ql_KB_Init
*  
* Description:
*                This function enables the keyboard function of the specified 
*                group of keys.
*
* Parameters:    
*                kbPinName:
*                   Keyboard matrix of m*n.
*                       
*                count:
*                   The key number that kbPinName contains keys.
* Return:  
*               QL_RET_OK indicates success.
*               QL_RET_ERR_INVALID_PARAMETER indicates something wrong for input parameters. 
******************************************************************************/
s32 Ql_KB_Init(Enum_PinName kbPinName[], u32 count);

/******************************************************************************
* Function:     Ql_KB_Uninit
*  
* Description:
*                This function disables the keyboard function of the specified 
*                group of keys that is enabled by calling Ql_KB_Init.
*
* Parameters:    
*                kbPinName:
*                   Keyboard matrix of m*n.
*                       
*                count:
*                   The key number that kbPinName contains keys.
* Return:  
*               QL_RET_OK indicates success.
*               QL_RET_ERR_INVALID_PARAMETER indicates something wrong for input parameters. 
******************************************************************************/
s32 Ql_KB_Uninit(Enum_PinName kbPinName[], u32 count);

#endif
