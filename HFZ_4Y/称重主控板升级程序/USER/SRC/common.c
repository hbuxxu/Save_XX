/****************************************************************************
* 文件名:    common.c
* 文件历史:
* 版本号:    V1.0
* 日期:      2016.09.29    
* 作者:      qgk
* 说明:      串口信息输出
*****************************************************************************/
/*=============================================================================
*******修改日志*******
1、日    期：
地    点：
修 改 人：
添加内容：
版本号:
2、日    期：
地    点：
修 改 人：
添加内容：
版本号:
================================================================================*/

/* 包含头文件 *****************************************************************/
#include "common.h"
/* 变量 ----------------------------------------------------------------------*/
uint32_t FlashDestination = ApplicationAddress;
pFunction Jump_To_Application;
uint32_t JumpAddress;
uint32_t BlockNbr = 0, UserMemoryMask = 0;
__IO uint32_t FlashProtection = 0;
extern uint32_t FlashDestination;
/*******************************************************************************
* @函数名称	SerialPutChar
* @函数说明   串口发送一个字符
* @输入参数   C:需发送的字符
* @输出参数   i无
* @返回参数   无
*******************************************************************************/
void SerialPutChar(uint8_t c)
{
    USART_SendData(USART1, c);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }
}
/*******************************************************************************
* @函数名称	SerialPutChar
* @函数说明   串口发送一个字符串
* @输入参数   *s:需发送的字符串
* @输出参数   无
* @返回参数   无
*******************************************************************************/
void Serial_PutString(uint8_t *s)
{
    while (*s != '\0')
    {
        SerialPutChar(*s);
        s++;
    }
}
/*******************************************************************************
* @函数名称	FLASH_DisableWriteProtectionPages
* @函数说明   解除Flash写保护
* @输入参数   无
* @输出参数   无
* @返回参数   无
*******************************************************************************/
void FLASH_DisableWriteProtectionPages(void)
{
    uint32_t useroptionbyte = 0, WRPR = 0;
    uint16_t var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
    FLASH_Status status = FLASH_BUSY;
    WRPR = FLASH_GetWriteProtectionOptionByte();   //1写保护失效  0写保护生效
          //测试是否写保护
    if ((WRPR & UserMemoryMask) != UserMemoryMask)
    {
        useroptionbyte = FLASH_GetUserOptionByte();
        UserMemoryMask |= WRPR;
        status = FLASH_EraseOptionBytes();
        if (UserMemoryMask != 0xFFFFFFFF)
        {
            status = FLASH_EnableWriteProtection((uint32_t)~UserMemoryMask);
        }
            //用处选项字是否有编程
        if ((useroptionbyte & 0x07) != 0x07)
        {
            //重新保存选项字
            if ((useroptionbyte & 0x01) == 0x0)
            {
                var1 = OB_IWDG_HW;
            }
            if ((useroptionbyte & 0x02) == 0x0)
            {
                var2 = OB_STOP_RST;
            }
            if ((useroptionbyte & 0x04) == 0x0)
            {
                var3 = OB_STDBY_RST;
            }
            FLASH_UserOptionByteConfig(var1, var2, var3);
        }
        if (status == FLASH_COMPLETE)
        {
            //系统复位重新加载选项字
            NVIC_SystemReset();
        }
        else
        {
            SerialPutString("Error: Flash write unprotection failed...\r\n");
        }
    }
    else
    {
        SerialPutString("Flash memory not write protected\r\n");
    }
}


/*******************************文件结束***************************************/
