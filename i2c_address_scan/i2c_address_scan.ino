#include <Wire.h>

//SDA  SDA (default is GPIO 21)
//SCL SCL (default is GPIO 22)
//Wire.begin(I2C_SDA, I2C_SCL);
#define pi 3.14159
#define totalStep 1000
#define  MAX_RANGE 0xFFF

unsigned short sinbuf[totalStep];

void setup() {
  Wire.begin(21,22,400000);
  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
  for(int i =0 ; i < totalStep ; i++)
  {
      sinbuf[i] = sin(2 * pi * i / totalStep)*2047+2048;
  }
}


void change_data(unsigned short DAC_data)
{
  Wire.beginTransmission(0x5b); // transmit to device #4
  Wire.write(0x02);        // sends five bytes
  Wire.write(DAC_data%256);  //数据 低八位                      
  Wire.write(DAC_data>>8); //数据 高四位    
  Wire.endTransmission();    // stop transmitting
}

void loop() {
#if 1
  for(unsigned short i = 0; i < totalStep; i+=10)
  {
       change_data(sinbuf[i]);
       delay(1);
  }
 #endif
 
}
