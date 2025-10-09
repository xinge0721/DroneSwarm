#ifndef __OLED_H
#define __OLED_H

#include "../../IIC/IIC.h"

#include "OLED_Font.h"
#include "../../System/sys/sys.h"



// ================ 类定义 ================
class OLED {
private:
	// ================ 写入命令 ================
	inline void Write(uint8_t command) {
		iic.Write(0x78, 0x00, command);
	}

	// ================ 设置光标 ================
	inline void SetCursor(uint8_t Y, uint8_t X)
	{
		Write(0xB0 | Y);					//设置Y位置
		Write(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
		Write(0x00 | (X & 0x0F));			//设置X位置低4位
	}


	/**
	 * @brief  OLED次方函数
	 * @retval 返回值等于X的Y次方
	 */
	inline uint32_t Pow(uint32_t X, uint32_t Y)
	{
		uint32_t Result = 1;
		while (Y--)
		{
			Result *= X;
		}
		return Result;
	}
	
public:

	// ================ 构造函数 ================
    OLED();



	// ================ 清屏 ================
	void Clear(void);

	/**
	 * @brief  OLED显示一个字符
	 * @param  Line 行位置，范围：1~4
	 * @param  Column 列位置，范围：1~16
	 * @param  Char 要显示的一个字符，范围：ASCII可见字符
	 * @retval 无
	 */
	void ShowChar(uint8_t Line, uint8_t Column, const char Char);

	/**
	 * @brief  OLED显示字符串
	 * @param  Line 起始行位置，范围：1~4
	 * @param  Column 起始列位置，范围：1~16
	 * @param  String 要显示的字符串，范围：ASCII可见字符
	 * @retval 无
	 */
	void ShowString(uint8_t Line, uint8_t Column, const char *String);
	

	/**
	 * @brief  OLED显示数字（十进制，正数）
	 * @param  Line 起始行位置，范围：1~4
	 * @param  Column 起始列位置，范围：1~16
	 * @param  Number 要显示的数字，范围：0~4294967295
	 * @param  Length 要显示数字的长度，范围：1~10
	 * @retval 无
	 */
	void ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

		/**
	 * @brief  OLED显示数字（十进制，带符号数）
	 * @param  Line 起始行位置，范围：1~4
	 * @param  Column 起始列位置，范围：1~16
	 * @param  Number 要显示的数字，范围：-2147483648~2147483647
	 * @param  Length 要显示数字的长度，范围：1~10
	 * @retval 无
	 */
	void ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);

	/**
	 * @brief  OLED显示浮点数（十进制，带符号数）
	 * @param  Line 起始行位置，范围：1~4
	 * @param  Column 起始列位置，范围：1~16
	 * @param  Number 要显示的浮点数
	 * @param  IntLength 整数部分长度
	 * @retval 无
	 */
	void ShowFloatNum(uint8_t Line, uint8_t Column, float Number, uint8_t IntLength);

	/**
	 * @brief  OLED显示数字（十六进制，正数）
	 * @param  Line 起始行位置，范围：1~4
	 * @param  Column 起始列位置，范围：1~16
	 * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
	 * @param  Length 要显示数字的长度，范围：1~8
	 * @retval 无
	 */
	void ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

	/**
	 * @brief  OLED显示数字（二进制，正数）
	 * @param  Line 起始行位置，范围：1~4
	 * @param  Column 起始列位置，范围：1~16
	 * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
	 * @param  Length 要显示数字的长度，范围：1~16
	 * @retval 无
	 */
	void ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
};








#endif
