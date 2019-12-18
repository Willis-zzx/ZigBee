#include "hal_mcu.h"
#include "Mydht11.h"
#include "OnBoard.h"


//温湿度定义
uint8 uint8FLAG,uint8temp;
uint8 uint8T_data_H,uint8T_data_L,uint8RH_data_H,uint8RH_data_L,uint8checkdata;
uint8 uint8T_data_H_temp,uint8T_data_L_temp,uint8RH_data_H_temp,uint8RH_data_L_temp,uint8checkdata_temp;
uint8 uint8comdata;
uint8 dht11Temp,dht11Humidity;
uint8 dht11TempDec,dht11HumidityDec;

void Delay_us(void);
void Delay_10us(void);
void Delay_ms(uint16 Time);
void COM(void);
void readDHT11(void);

/****************************
        延时函数
*****************************/
void Delay_us(void) //1 us延时
{
    MicroWait(1);   
}

void Delay_10us(void) //10 us延时
{
  MicroWait(10);   
}

void Delay_ms(uint16 Time)//n ms延时
{
  unsigned char i;
  while(Time--)
  {
    for(i=0;i<100;i++)
     Delay_10us();
  }
}


/***********************
   温湿度传感
***********************/
void COM(void)	// 温湿写入
{     
  uint8 i;         
  for(i=0;i<8;i++)    
  {
    uint8FLAG=2; 
    while((!DHT_PIN)&&uint8FLAG++);
    Delay_10us();
    Delay_10us();
    Delay_10us();
    uint8temp=0;
    if(DHT_PIN)uint8temp=1;
    uint8FLAG=2;
    while((DHT_PIN)&&uint8FLAG++);   
    if(uint8FLAG==1)break;    
    uint8comdata<<=1;
    uint8comdata|=uint8temp; 
  }    
}

void readDHT11(void)   //温湿传感启动
{
  P0DIR |= 0x40; //IO口需要重新配置 
  DHT_PIN=0;
  Delay_ms(19);  //>18MS
  DHT_PIN=1; 
  P0DIR &= ~0x40; //重新配置IO口方向
  Delay_10us();
  Delay_10us();						
  Delay_10us();
  Delay_10us();  
  
  if(!DHT_PIN) //DHT11串口不为空
  {
    uint8FLAG=2; 
    while((!DHT_PIN)&&uint8FLAG++);
    uint8FLAG=2;
    while((DHT_PIN)&&uint8FLAG++); 
    COM();
    uint8RH_data_H_temp=uint8comdata;
    COM();
    uint8RH_data_L_temp=uint8comdata;
    COM();
    uint8T_data_H_temp=uint8comdata;
    COM();
    uint8T_data_L_temp=uint8comdata;
    COM();
    uint8checkdata_temp=uint8comdata;
    DHT_PIN=1; 
    uint8temp=(uint8T_data_H_temp+uint8T_data_L_temp+uint8RH_data_H_temp+uint8RH_data_L_temp);
    if(uint8temp==uint8checkdata_temp)
    {
      uint8RH_data_H=uint8RH_data_H_temp;
      uint8RH_data_L=uint8RH_data_L_temp;
      uint8T_data_H=uint8T_data_H_temp;
      uint8T_data_L=uint8T_data_L_temp;
      uint8checkdata=uint8checkdata_temp;
    }
    dht11Temp = uint8T_data_H; 
    dht11TempDec = uint8T_data_L;
    dht11Humidity = uint8RH_data_H;     
    dht11HumidityDec = uint8RH_data_L; 
  } 
  else //没有成功读取，返回0
  {
    dht11Temp = 0; 
    dht11TempDec = 0;
    dht11Humidity = 0;
    dht11HumidityDec = 0;
  } 
}
