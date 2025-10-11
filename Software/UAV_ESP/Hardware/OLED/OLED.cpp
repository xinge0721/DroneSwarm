#include "OLED_Font.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h"

OLED::OLED(gpio_num_t scl_pin, gpio_num_t sda_pin)
{
    this->scl_pin = scl_pin;
    this->sda_pin = sda_pin;

    // 配置GPIO为输出模式
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << scl_pin) | (1ULL << sda_pin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    
    // 初始状态为关闭
    gpio_set_level(scl_pin, 0);
    gpio_set_level(sda_pin, 0);

	Init();
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED::Start(void)
{
	write_sda(1);
	Delay::i2c_delay();
	write_scl(1);
	Delay::i2c_delay();
	write_sda(0);
	Delay::i2c_delay();
	write_scl(0);
	Delay::i2c_delay();
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED::Stop(void)
{
	write_sda(0);
	Delay::i2c_delay();
	write_scl(1);
	Delay::i2c_delay();
	write_sda(1);
	Delay::i2c_delay();
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
void OLED::SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		write_sda(Byte & (0x80 >> i));
		Delay::i2c_delay();
		write_scl(1);
		Delay::i2c_delay();
		write_scl(0);
		Delay::i2c_delay();
	}
	write_scl(1);	//额外的一个时钟，不处理应答信号
	Delay::i2c_delay();
	write_sda(1);
	Delay::i2c_delay();
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED::SendCommand(uint8_t Command)
{
	Start();
	SendByte(0x78);		//从机地址
	SendByte(0x00);		//写命令
	SendByte(Command); 
	Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED::WriteData(uint8_t Data)
{
	Start();
	SendByte(0x78);		//从机地址
	SendByte(0x40);		//写数据
	SendByte(Data);
	Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED::SetCursor(uint8_t Y, uint8_t X)
{
	SendCommand(0xB0 | Y);					//设置Y位置
	SendCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	SendCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED::Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			WriteData(0x00);
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
void OLED::ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED::ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED::Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
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

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED::Init(void)
{
	uint32_t i, j;
	
	Delay::delay_ms(100); 
	
	
	SendCommand(0xAE);	//关闭显示
	
	SendCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	SendCommand(0x80);
	
	SendCommand(0xA8);	//设置多路复用率
	SendCommand(0x3F);
	
	SendCommand(0xD3);	//设置显示偏移
	SendCommand(0x00);
	
	SendCommand(0x40);	//设置显示开始行
	
	SendCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	SendCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	SendCommand(0xDA);	//设置COM引脚硬件配置
	SendCommand(0x12);
	
	SendCommand(0x81);	//设置对比度控制
	SendCommand(0xCF);

	SendCommand(0xD9);	//设置预充电周期
	SendCommand(0xF1);

	SendCommand(0xDB);	//设置VCOMH取消选择级别
	SendCommand(0x30);

	SendCommand(0xA4);	//设置整个显示打开/关闭

	SendCommand(0xA6);	//设置正常/倒转显示

	SendCommand(0x8D);	//设置充电泵
	SendCommand(0x14);

	SendCommand(0xAF);	//开启显示
		
	Clear();				//OLED清屏
}