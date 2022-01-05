typedef unsigned short u16;
typedef unsigned char u8;

#define DATA_PIN  23
#define CLK_PIN   22

#define I2C_SDA_High()      digitalWrite(DATA_PIN,1)
#define I2C_SDA_Low()       digitalWrite(DATA_PIN,0)

#define I2C_SCL_High()      digitalWrite(CLK_PIN,1)
#define I2C_SCL_Low()       digitalWrite(CLK_PIN,0)

#define I2C_SDA_DATA()      digitalRead(DATA_PIN)

#define LED_BUILTIN 2

#define DEVICE_ID  3
#define SL_ADDR (0xb0|(DEVICE_ID<<1))

u16 DAC_data;

//-------------------------------------------------
// 函数名称：Delay5US()
// 函数功能：5微秒延时,如果自己的主频有变，请自行修改
// 入口参数：无
// 出口参数：无
//-------------------------------------------------
void Delay5US(void)    //@240.0592MHz
{
  //u8 i;
  //i = 250;
  //while (--i);
  delayMicroseconds(5);
}
//-------------------------------------------------
// 函数名称：IIC_Start()
// 函数功能：IIC起动
// 入口参数：无
// 出口参数：无
//-------------------------------------------------
void IIC_Start(void)
{
  I2C_SDA_High();
  Delay5US();
  I2C_SCL_High();
  Delay5US();
  I2C_SDA_Low();
  Delay5US();
}
//-------------------------------------------------
// 函数名称：IIC_Stop()
// 函数功能：IIC停止
// 入口参数：无
// 出口参数：无
//-------------------------------------------------
void IIC_Stop(void)                     
{
  I2C_SDA_Low();
  Delay5US();
  I2C_SCL_High();
  Delay5US();
  I2C_SDA_High();
}
//-------------------------------------------------
// 函数名称：IIC_Ack()
// 函数功能：IIC应答
// 入口参数：无
// 出口参数：无
//-------------------------------------------------
void IIC_Ack(void)                      
{ 
  I2C_SCL_Low();        // 为产生脉冲准备
  I2C_SDA_Low();        // 产生应答信号
  Delay5US();     
  I2C_SCL_High();
  Delay5US();  
  I2C_SCL_Low();
  Delay5US();         // 产生高脉冲 
  I2C_SDA_High();        // 释放总线
}
//-------------------------------------------------
// 函数名称：IIC_RdAck()
// 函数功能：读IIC应答
// 入口参数：无
// 出口参数：是否应答真值
//--------------------------------------------------
u8 IIC_RdAck(void)                      
{ 
  u8 AckFlag;
  u8 uiVal = 0;
  I2C_SCL_Low();
  Delay5US();    
  I2C_SDA_High();
  I2C_SCL_High();
  Delay5US();
  while((1 == I2C_SDA_DATA()) && (uiVal < 255))
  {
    uiVal ++;
    AckFlag = I2C_SDA_DATA();
  }
  digitalWrite(LED_BUILTIN,AckFlag);
  I2C_SCL_Low();  
  return AckFlag;   // 应答返回：0;不应答返回：1
}
//-----------------------------------------------------
// 函数名称：IIC_Nack()
// 函数功能：IIC不应答
// 入口参数：无
// 出口参数：无
//-----------------------------------------------------
void IIC_Nack(void)                    
{   
  I2C_SDA_High();
  I2C_SCL_Low();
  Delay5US();      
  I2C_SCL_High();
  Delay5US();      
  I2C_SCL_Low();
}
//---------------------------------------------------------
// 函数名称：R_Byte1()
// 函数功能：从IIC器件中读出一个字节
// 入口参数：无
// 出口参数：读出的一个字节（uByteVal）
//---------------------------------------------------------
u8 R_Byte1(void)        
{
  u8 uByteVal = 0;
  u8 iCount;
  I2C_SDA_High();
  for (iCount = 0;iCount < 8;iCount++)
  {
    I2C_SCL_Low();
    Delay5US();
    I2C_SCL_High();
    Delay5US(); 
    uByteVal <<= 1;
    if(I2C_SDA_DATA())
      uByteVal |= 0x01;
  }
  I2C_SCL_Low();
  return(uByteVal);
}
//-------------------------------------------------------
// 函数名称：W_Byte1()
// 函数功能：向IIC器件写入一个字节
// 入口参数：待写入的一个字节（uByteVal）
// 出口参数：无
//-------------------------------------------------------
void W_Byte1(u8 uByteVal)
{
    u8 iCount;
  for(iCount = 0;iCount < 8;iCount++)
  {
    I2C_SCL_Low();
    Delay5US();     
    digitalWrite(DATA_PIN, ((uByteVal & 0x80) >> 7));
    Delay5US();         
    I2C_SCL_High();
    Delay5US();
    uByteVal <<= 1;
    } 
  I2C_SCL_Low();
}
//-------------------------------------------------------
void Init_GP8202(void)
{
  IIC_Start();
  W_Byte1(SL_ADDR);
  IIC_RdAck();
  W_Byte1(0x02);
  IIC_RdAck();
  IIC_Nack();
  IIC_Stop(); 
}
//----------------------------------------------------------
void change_data(u16 DAC_data)
{
  IIC_Start();      // 发送开始信号
  W_Byte1(SL_ADDR);      //1011      000   0
              //芯片代码    地址位
  IIC_RdAck();        // 读应答信号
  W_Byte1(0x02);      //？？？
  IIC_RdAck();      // 读应答信号
  W_Byte1(DAC_data%256);  //数据 低八位                      
  IIC_RdAck();      // 读应答信号
  W_Byte1(DAC_data>>8); //数据 高四位    
  IIC_RdAck();      // 读应答信号
  IIC_Stop();
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  Init_GP8202(); //每次启动进行的初始化芯片
  delay(200);
  DAC_data=0x0FFF;

}

int leds=1;

void loop()
{
    if(DAC_data==0x0000)DAC_data=0x0FFF;
    DAC_data--;
    change_data(DAC_data);
    digitalWrite(LED_BUILTIN,leds);
    delay(1000);
    leds = !leds;
}
/*
#include  "GP8202.h" 
//-------------------主函数---------------------------
void main(void)
{
  GPIO_config();
//  GUHUA_GP8202();//用于固话芯片的一些数据，每个芯片只在第一次写入
  Init_GP8202(); //每次启动进行的初始化芯片
  delay_ms(200);
  DAC_data=0x0FFF;
  while(1)
  {   
    if(DAC_data==0x0000)DAC_data=0x0FFF;
    DAC_data--;
    change_data(DAC_data);
    LED=1;
  }
}
*/