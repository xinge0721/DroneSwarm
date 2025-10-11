#ifndef __OLED_H
#define __OLED_H

#include "driver/gpio.h"
#include <stdint.h>
#include "../../System/delay/delay.h"


class OLED {
private:
    gpio_num_t scl_pin;
    gpio_num_t sda_pin;

    // ESP 内联I2C写函数，直接操作引脚（适用于ESP平台）
    inline void write_sda(bool level) {
        gpio_set_level(sda_pin, level);
    }

    inline void write_scl(bool level) {
        gpio_set_level(scl_pin, level);
    }
	
	void Start();
	void Stop();
	void SendByte(uint8_t Byte);
	void SendCommand(uint8_t Command);
	void WriteData(uint8_t Data);
	void SetCursor(uint8_t Y, uint8_t X);
	void Init();
	uint32_t Pow(uint32_t X, uint32_t Y);
	
public:
    OLED(gpio_num_t scl_pin = GPIO_NUM_8, gpio_num_t sda_pin = GPIO_NUM_9);

	// 清屏
	void Clear();
	// 显示字符
	void ShowChar(uint8_t Line, uint8_t Column, char Char);
	// 显示字符串
	void ShowString(uint8_t Line, uint8_t Column, char *String);
	// 显示数字
	void ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
	// 显示有符号数字
	void ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
	// 显示十六进制数字
	void ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
	// 显示二进制数字
	void ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
};


#endif