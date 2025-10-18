#include "main.h"

uint8_t receive_buffer[255];

extern void task_display(void*);

#define PIN_SDA 8
#define PIN_CLK 9

Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
uint16_t packetSize = 42;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU


extern "C" int app_main(void)
{

    i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = (gpio_num_t)PIN_SDA;
	conf.scl_io_num = (gpio_num_t)PIN_CLK;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 400000;
	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));

    vTaskDelay(500/portTICK_PERIOD_MS);
    xTaskCreate(&task_display, "disp_task", 8192, NULL, 5, NULL);
    return 0;
}



void task_display(void*){
    MPU6050 mpu = MPU6050();
    mpu.initialize();
    mpu.dmpInitialize();
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
  
      mpu.setDMPEnabled(true);
  
      while(1){
  
        mpuIntStatus = mpu.getIntStatus();
        fifoCount = mpu.getFIFOCount();
  
          if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
              mpu.resetFIFO();
  
          } else if (mpuIntStatus & 0x02) {
              while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
  
  
            mpu.getFIFOBytes(fifoBuffer, packetSize);
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            ESP_LOGI(TAG, "YAW: %3.1f, PITCH: %3.1f, ROLL: %3.1f", 
                     ypr[0] * 180/M_PI, ypr[1] * 180/M_PI, ypr[2] * 180/M_PI);
          }
      }
  
      vTaskDelete(NULL);
  }
  