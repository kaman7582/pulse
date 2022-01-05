#include <Arduino.h>
#include <esp32-hal-dac.h>
#define DAC1 (*(volatile unsigned long *)0x3ff48484)//ADC数值寄存器
#define DAC2 (*(volatile unsigned long *)0x3ff48488)
#define LED1 25
#define LED2 26
#define pi 3.14159
#define totalStep 1000
unsigned long IOutputQueue[totalStep];
unsigned long QOutputQueue[totalStep];
unsigned long i;

void setup()
{
    delay(100);
    Serial.begin(115200);
    Serial.println("123");

    for (i = 0; i < totalStep; i++)
    {
        IOutputQueue[i] = sin(2 * pi * i / totalStep) * 127 +128;
        IOutputQueue[i] = ((IOutputQueue[i] & 0x000000ff) << (19)) | 0xc0060400;
    }
    for (i = 0; i < totalStep; i++)
    {
        QOutputQueue[i] = cos(2 * pi * i / totalStep) * 127 +128;
        QOutputQueue[i] = ((QOutputQueue[i] & 0x000000ff) << (19)) | 0xc0060400;
    }
    
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    dacWrite(LED1, 0);
    dacWrite(LED2, 0);
    Serial.printf("%x", DAC1);//在这里我们可以观察到寄存配置应该是0xc0060400 
}

void writeDac(int pin, int val)
{
  if(pin == 25)
  {
    DAC1 = ((val & 0x000000ff) << (19)) | 0xc0060400;
  }
  else if(pin == 26)
  {
    DAC2=((val & 0x000000ff) << (19)) | 0xc0060400;
  }
}
void loop()
{
  #if 0
    while (1)
    {
        i++;
        if (i >= totalStep)
            i = 0;
        DAC1 = IOutputQueue[i];
        DAC2 = QOutputQueue[i];
    }
    #endif
    writeDac(26,255);
    delayMicroseconds(2);
    writeDac(26,0);
    delayMicroseconds(2);
    
}
