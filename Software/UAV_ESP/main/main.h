#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "../Hardware/WIFI/WIFI.h"
#include "../Hardware/UDP/UDP.h"
#include "../Hardware/MPU6050/MPU6050.h"
#include "../System/delay/delay.h"
#include "../System/sys/sys.h"


#include <driver/i2c.h>
#include <esp_log.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "../Hardware/MPU6050/MPU6050.h"
#include "../Hardware/MPU6050/MPU6050_6Axis_MotionApps20.h"
#include "sdkconfig.h"
// 定义TAG
static const char *TAG = "MAIN";


#endif