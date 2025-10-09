#include "OLED.h"




// ================ 构造函数 ================
OLED::OLED()
{

			
	for (uint32_t i = 0; i < 1000; i++)			//上电延时
	{
		for (uint32_t j = 0; j < 1000; j++);
	}


	Write(0xAE);	//关闭显示

	Write(0xD5);	//设置显示时钟分频比/振荡器频率
	Write(0x80);

	Write(0xA8);	//设置多路复用率
	Write(0x3F);

	Write(0xD3);	//设置显示偏移
	Write(0x00);

	Write(0x40);	//设置显示开始行

	Write(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置

	Write(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	Write(0xDA);	//设置COM引脚硬件配置
	Write(0x12);

	Write(0x81);	//设置对比度控制
	Write(0xCF);

	Write(0xD9);	//设置预充电周期
	Write(0xF1);

	Write(0xDB);	//设置VCOMH取消选择级别
	Write(0x30);

	Write(0xA4);	//设置整个显示打开/关闭

	Write(0xA6);	//设置正常/倒转显示

	Write(0x8D);	//设置充电泵
	Write(0x14);

	Write(0xAF);	//开启显示
		
	Clear();				//OLED清屏
}



// ================ 清屏 ================
void OLED::Clear(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			Write(0x00);
		}
	}
}



/**
 * @brief  OLED显示一个字符
 * @param  Line 行位置，范围：1~4
 * @param  Column 列位置，范围：1~16
 * @param  Char 要显示的一个字符，范围：ASCII可见字符
 * @retval 无
 */
void OLED::ShowChar(uint8_t Line, uint8_t Column, const char Char)
{      	
	// 参数边界检查
	if (Line < 1 || Line > 4 || Column < 1 || Column > 16) {
		return; // 超出显示范围
	}
	
	// 字符范围检查，防止数组越界
	if (Char < ' ' || Char > '~') {
		return; // 字符超出字体表范围
	}
	
	uint8_t i;
	SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		Write(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		Write(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}
/**
 * @brief  OLED显示字符串
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  String 要显示的字符串，范围：ASCII可见字符
 * @retval 无
 */
void OLED::ShowString(uint8_t Line, uint8_t Column, const char *String)
{
	// 参数边界检查
	if (Line < 1 || Line > 4 || Column < 1 || Column > 16 || String == nullptr) {
		return; // 参数错误
	}
	
	uint8_t i;
	for (i = 0; String[i] != '\0' && (Column + i) <= 16; i++)  // 防止超出屏幕右边界
	{
		ShowChar(Line, Column + i, String[i]);
	}
}


/**
 * @brief  OLED显示数字（十进制，正数）
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：0~4294967295
 * @param  Length 要显示数字的长度，范围：1~10
 * @retval 无
 */
void OLED::ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	// 参数边界检查
	if (Line < 1 || Line > 4 || Column < 1 || Column > 16 || Length == 0 || (Column + Length - 1) > 16) {
		return; // 参数错误或会超出屏幕边界
	}
	
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		ShowChar(Line, Column + i, Number / Pow(10, Length - i - 1) % 10 + '0');
	}
}
	/**
 * @brief  OLED显示数字（十进制，带符号数）
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：-2147483648~2147483647
 * @param  Length 要显示数字的长度，范围：1~10
 * @retval 无
 */
void OLED::ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		ShowChar(Line, Column + i + 1, Number1 / Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
 * @brief  OLED显示浮点数（十进制，带符号数）
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的浮点数
 * @param  IntLength 整数部分长度
 * @retval 无
 */
void OLED::ShowFloatNum(uint8_t Line, uint8_t Column, float Number, uint8_t IntLength)
{
	uint32_t integerPart, fractionalPart;
	uint8_t i;
	
	if (Number >= 0)
	{
		ShowChar(Line, Column, '+');
	}
	else
	{
		ShowChar(Line, Column, '-');
		Number = -Number;
	}
	
	integerPart = (uint32_t)Number;
	fractionalPart = (uint32_t)(((Number - integerPart) * 10000) + 0.5);
	
	if (fractionalPart >= 10000)
	{
		integerPart++;
		fractionalPart -= 10000;
	}
	
	for (i = 0; i < IntLength; i++)
	{
		ShowChar(Line, Column + 1 + i, integerPart / Pow(10, IntLength - 1 - i) % 10 + '0');
	}
	
	ShowChar(Line, Column + 1 + IntLength, '.');
	
	ShowChar(Line, Column + 2 + IntLength, fractionalPart / 1000 + '0');
	ShowChar(Line, Column + 3 + IntLength, fractionalPart / 100 % 10 + '0');
	ShowChar(Line, Column + 4 + IntLength, fractionalPart / 10 % 10 + '0');
	ShowChar(Line, Column + 5 + IntLength, fractionalPart % 10 + '0');
}

/**
 * @brief  OLED显示数字（十六进制，正数）
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
 * @param  Length 要显示数字的长度，范围：1~8
 * @retval 无
 */
void OLED::ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
 * @brief  OLED显示数字（二进制，正数）
 * @param  Line 起始行位置，范围：1~4
 * @param  Column 起始列位置，范围：1~16
 * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
 * @param  Length 要显示数字的长度，范围：1~16
 * @retval 无
 */
void OLED::ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		ShowChar(Line, Column + i, Number / Pow(2, Length - i - 1) % 2 + '0');
	}
}






