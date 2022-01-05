#if 0
#include <Wire.h>
#define SDA   22
#define SCL   23

#define I2C_SDA_High()      digitalWrite(SDA,1)
#define I2C_SDA_Low()       digitalWrite(SDA,0)

#define I2C_SCL_High()      digitalWrite(SCL,1)
#define I2C_SCL_Low()       digitalWrite(SCL,0)

#define SLAVE_ID         0
#define SLAVE_ADDR_WD   (0xb0|SLAVE_ID<<1)

#define I2C_Delay() delayMicroseconds(5)

typedef unsigned char u8;

void I2C_Start(void)

{
    I2C_SDA_High();     //SDA=1

    I2C_SCL_High();     //SCL=1

    I2C_Delay();

    I2C_SDA_Low();

    I2C_Delay();

    I2C_SCL_Low();

    I2C_Delay();
}

void I2C_Stop(void)
{

    I2C_SDA_Low();

    I2C_SCL_High();

    I2C_Delay();

    I2C_SDA_High();

    I2C_Delay();

}

void I2C_Ack(void)
{

    I2C_SDA_Low();

    I2C_Delay();

    I2C_SCL_High();

    I2C_Delay();

    I2C_SCL_Low();

    I2C_Delay();

    I2C_SDA_High();

}

u8 I2C_WriteByte(uint8_t Byte)
{
    uint8_t i;
    /* 先发送高位字节 */

    for(i = 0 ; i < 8 ; i++)
    {
        if(Byte & 0x80)
        {
            I2C_SDA_High();
        }
        else
        {
            I2C_SDA_Low();
        }
        I2C_Delay();
        I2C_SCL_High();
        I2C_Delay();
        I2C_SCL_Low();
        I2C_Delay();

        if(i == 7)
        {
            I2C_SDA_High();                     /* 释放SDA总线 */
        }
        Byte <<= 1;                             /* 左移一位  */
        I2C_Delay();
    }
}

void I2C_NoAck(void)
{

    I2C_SDA_High();

    I2C_Delay();

    I2C_SCL_High();

    I2C_Delay();

    I2C_SCL_Low();

    I2C_Delay();

}

uint8_t I2C_WaitToAck(void)
{

    uint8_t redata;

    I2C_SDA_High();

    I2C_Delay();

    I2C_SCL_High();

    I2C_Delay();

    if(digitalRead(SDA))
    {
        redata = 1;
    }
    else
    {
        redata = 0;

    }
    I2C_SCL_Low();

    I2C_Delay();

    return redata;
}


void Init_GP8202(void)
{
	I2C_Start();
	I2C_WriteByte(SLAVE_ADDR_WD);
	I2C_WaitToAck();
	I2C_WriteByte(0x02);
	I2C_WaitToAck();
	I2C_NoAck();
	I2C_Stop();	
}

void set_voltage(int DAC_data)
{
	I2C_Start();			// 发送开始信号
	I2C_WriteByte(SLAVE_ADDR_WD);			//1011 			000		0
							//芯片代码		地址位
	I2C_WaitToAck();  			// 读应答信号
	I2C_WriteByte(0x02);			//？？？
	I2C_WaitToAck();			// 读应答信号
	I2C_WriteByte(DAC_data%256);	//数据 低八位											
	I2C_WaitToAck();			// 读应答信号
	I2C_WriteByte(DAC_data>>8);	//数据 高四位		
	I2C_WaitToAck();			// 读应答信号
	I2C_Stop();
}

void setup()
{
    pinMode(SDA,OUTPUT);
    pinMode(SCL,OUTPUT);
    Init_GP8202();
}

void loop()
{
  for(int i = 10 ; i < 4000; i += 100)
    {
        set_voltage(i);
        delay(100);
    }
}

#else




typedef unsigned char u8;
typedef unsigned short u16;

#define SDA   21
#define SCL   22

#define I2C_SDA_H()      digitalWrite(SDA,1)
#define I2C_SDA_L()       digitalWrite(SDA,0)

#define I2C_SCL_H()      digitalWrite(SCL,1)
#define I2C_SCL_L()       digitalWrite(SCL,0)

#define SDA_DATA()       digitalRead(SDA)

#define SLAVE_ID         3
#define SLAVE_ADDR_WD   (0xb0|SLAVE_ID<<1)

#define LED_BUILTIN 2
u16 DAC_data;
//-------------------------------------------------
// 函数名称：Delay5US()
// 函数功能：5微秒延时,如果自己的主频有变，请自行修改
// 入口参数：无
// 出口参数：无
//-------------------------------------------------
void Delay5US(void)		//@11.0592MHz
{
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
	I2C_SDA_H();
	Delay5US();
	I2C_SCL_H();
	Delay5US();
	I2C_SDA_L();
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
	I2C_SDA_L();
	Delay5US();
	I2C_SCL_H();
	Delay5US();
	I2C_SDA_H();
}
//-------------------------------------------------
// 函数名称：IIC_Ack()
// 函数功能：IIC应答
// 入口参数：无
// 出口参数：无
//-------------------------------------------------
void IIC_Ack(void)                      
{ 
	I2C_SCL_L();				// 为产生脉冲准备
	I2C_SDA_L();				// 产生应答信号
	Delay5US();			
	I2C_SCL_H();
	Delay5US();  
	I2C_SCL_L();
	Delay5US();					// 产生高脉冲 
	I2C_SDA_H();				// 释放总线
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
	I2C_SCL_L();
	Delay5US();    
	I2C_SDA_H();
	I2C_SCL_H();
	Delay5US();
	while((1 == SDA_DATA()) && (uiVal < 255))
	{
		uiVal ++;
		AckFlag = SDA_DATA();
	}
	digitalWrite(LED_BUILTIN,AckFlag); 
	I2C_SCL_L();  
	return AckFlag;		// 应答返回：0;不应答返回：1
}
//-----------------------------------------------------
// 函数名称：IIC_Nack()
// 函数功能：IIC不应答
// 入口参数：无
// 出口参数：无
//-----------------------------------------------------
void IIC_Nack(void)                    
{   
	I2C_SDA_H();
	I2C_SCL_L();
	Delay5US();
	I2C_SCL_H();
	Delay5US(); 
	I2C_SCL_L();
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
	I2C_SDA_H();
	for (iCount = 0;iCount < 8;iCount++)
	{
		I2C_SCL_L();
		Delay5US();
		I2C_SCL_H();
		Delay5US(); 
		uByteVal <<= 1;
		if(SDA_DATA())
		  uByteVal |= 0x01;
	}
	I2C_SCL_L();
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
    u8 val= 0 ;
		I2C_SCL_L();
		Delay5US();			
		val = (uByteVal & 0x80) >> 7;
    digitalWrite(SDA,val);
		Delay5US();         
		I2C_SCL_H();
		Delay5US();
		uByteVal <<= 1;
    }
	I2C_SCL_L();
}
//-------------------------------------------------------
void Init_GP8202(void)
{
	IIC_Start();	
	W_Byte1(SLAVE_ADDR_WD);
	IIC_RdAck();
	W_Byte1(0x02);
	IIC_RdAck();
	IIC_Nack();
	IIC_Stop();	
}
//----------------------------------------------------------
void change_data(u16 DAC_data)
{
	IIC_Start();			// 发送开始信号
	W_Byte1(SLAVE_ADDR_WD);			//1011 			000		0
							//芯片代码		地址位
	IIC_RdAck();  			// 读应答信号
	W_Byte1(0x02);			//？？？
	IIC_RdAck();			// 读应答信号
	W_Byte1(DAC_data%256);	//数据 低八位											
	IIC_RdAck();			// 读应答信号
	W_Byte1(DAC_data>>8);	//数据 高四位		
	IIC_RdAck();			// 读应答信号
	IIC_Stop();
}

void Init_GP8202_dev(u8 dev)
{
	IIC_Start();	
	W_Byte1(dev);
	IIC_RdAck();
	W_Byte1(0x02);
	IIC_RdAck();
	IIC_Nack();
	IIC_Stop();	
}
void set_dev(u16 DAC_data, u8 addr)
{
	IIC_Start();			// 发送开始信号
	W_Byte1(addr);			//1011 			000		0
							//芯片代码		地址位
	IIC_RdAck();  			// 读应答信号
	W_Byte1(0x02);			//？？？
	IIC_RdAck();			// 读应答信号
	W_Byte1(DAC_data%256);	//数据 低八位											
	IIC_RdAck();			// 读应答信号
	W_Byte1(DAC_data>>8);	//数据 高四位		
	IIC_RdAck();			// 读应答信号
	IIC_Stop();
}

u8 IIC_wait_ack(void)                      
{ 
	u8 AckFlag;
	int uiVal = 0;
	I2C_SCL_L();
	Delay5US();    
	I2C_SDA_H();
	I2C_SCL_H();
	Delay5US();
	while((1 == SDA_DATA()) && (uiVal < 0xfffff))
	{
		uiVal++;
		AckFlag = SDA_DATA();
	}
	digitalWrite(LED_BUILTIN,AckFlag); 
	I2C_SCL_L();
	return AckFlag;		// 应答返回：0;不应答返回：1
}

void find_i2c_dev()
{
    u8 dev = 0;
    for(int i = 0 ; i < 8;i++)
    {
        dev = 0xb0| i<<1;
        IIC_Start();
        W_Byte1(dev);
        u8 val1 = IIC_wait_ack();
        IIC_Stop();
        delay(100);
        Serial.printf("dev:%d val:%d\n\r",i,val1);
    }

}

u16 test_data[8]={0xff,0x1f,0x1ff,0x222,0x333,0xfff,0x123,0x111};

void test()
{
    u8 dev = 0;
    for(int i = 0 ; i < 8;i++)
    {
        dev = 0xb0| i<<1;
        Init_GP8202_dev(dev);
        set_dev(test_data[i],dev);
        delay(1000);
        Serial.printf("%08x\n\r",dev);
    }
}

//test protocol
void test_protocol()
{
    IIC_Start();
    W_Byte1(1<<1);//address last 0 means read
    IIC_RdAck();
	  W_Byte1(0x0);//write
    IIC_RdAck();
    W_Byte1(0x22);
    IIC_Stop();
}
void setup()
{
    Serial.begin(115200);
    pinMode(SDA,OUTPUT);
    pinMode(SCL,OUTPUT);
    pinMode(LED_BUILTIN,OUTPUT);
    I2C_SCL_H();
    I2C_SDA_H();
    //delay(1000);
    //Serial.printf("%08x\n\r",SLAVE_ADDR_WD);
    //Init_GP8202();
    //delay(200);
}

u8 val = 0;
void loop()
{
    test_protocol();
    delay(1000);

}

#endif
