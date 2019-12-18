#ifndef __DHT11_H__
#define __DHT11_H__
#include "ZComDef.h"
#include <stdio.h>
#include <ioCC2530.h>
#include "OnBoard.h"

#define uchar unsigned char
extern void Delay_ms1(unsigned int xms);	//��ʱ����
extern void COM(void);                  // ��ʪд��
extern void DHT11(void);                //��ʪ��������

extern uchar temp[2]; 
extern uchar temp1[5];
extern uchar humidity[2];
extern uchar humidity1[9];
extern uchar shidu_shi,shidu_ge,wendu_shi,wendu_ge;
char a[2]={'a','b'};
char b[2]={'a','b'};

#define EVENT_FLAG_TIMEOUT (1<<0)
#define TIMEOUTVALUE 1000

//typedef unsigned char uchar;
typedef unsigned int  uint;

#define DATA_PIN P0_7

void Delay_us1(void);
void Delay_10us1(void);
void Delay_ms1(uint Time);
void COM(void);
void DHT11(void);

//��ʪ�ȶ���
uchar ucharFLAG,uchartemp;
uchar shidu_shi,shidu_ge,wendu_shi,wendu_ge=4;
uchar ucharT_data_H,ucharT_data_L,ucharRH_data_H,ucharRH_data_L,ucharcheckdata;
uchar ucharT_data_H_temp,ucharT_data_L_temp,ucharRH_data_H_temp,ucharRH_data_L_temp,ucharcheckdata_temp;
uchar ucharcomdata;


//��ʱ����
void Delay_us1(void) //1 us��ʱ
{
    MicroWait(1);   
}

void Delay_10us1(void) //10 us��ʱ
{
   MicroWait(10);
}

void Delay_ms1(uint Time)//n ms��ʱ
{
  unsigned char i;
  while(Time--)
  {
    for(i=0;i<100;i++)
     Delay_10us1();
  }
}

//��ʪ�ȴ���
void COM(void)    // ��ʪд��
{     
    uchar i;         
    for(i=0;i<8;i++)    
    {
        ucharFLAG=2; 
        while((!DATA_PIN)&&ucharFLAG++);
        Delay_10us1();
        Delay_10us1();
        Delay_10us1();
        uchartemp=0;
        if(DATA_PIN)uchartemp=1;
        ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++);   
        if(ucharFLAG==1)break;    
        ucharcomdata<<=1;
        ucharcomdata|=uchartemp; 
    }    
}

void DHT11(void)   //��ʪ��������
{
    DATA_PIN=0;
    Delay_ms1(20);  //>18MS
    DATA_PIN=1; 
    P0DIR &= ~0x80; //��������IO�ڷ���
    Delay_10us1();
    Delay_10us1();                        
    Delay_10us1();
    Delay_10us1();  
    if(!DATA_PIN) 
    {
        ucharFLAG=2; 
        while((!DATA_PIN)&&ucharFLAG++);
        ucharFLAG=2;
        while((DATA_PIN)&&ucharFLAG++); 
        COM();
        ucharRH_data_H_temp=ucharcomdata;
        COM();
        ucharRH_data_L_temp=ucharcomdata;
        COM();
        ucharT_data_H_temp=ucharcomdata;
        COM();
        ucharT_data_L_temp=ucharcomdata;
        COM();
        ucharcheckdata_temp=ucharcomdata;
        DATA_PIN=1; 
        uchartemp=(ucharT_data_H_temp+ucharT_data_L_temp+ucharRH_data_H_temp+ucharRH_data_L_temp);
        if(uchartemp==ucharcheckdata_temp)
        {
            ucharRH_data_H=ucharRH_data_H_temp;
            ucharRH_data_L=ucharRH_data_L_temp;
            ucharT_data_H=ucharT_data_H_temp;
            ucharT_data_L=ucharT_data_L_temp;
            ucharcheckdata=ucharcheckdata_temp;
        }
        wendu_shi=ucharT_data_H/10; 
        wendu_ge=ucharT_data_H%10;
        
        shidu_shi=ucharRH_data_H/10; 
        shidu_ge=ucharRH_data_H%10;        
    } 
    else //û�óɹ���ȡ������0
    {
        wendu_shi=0; 
        wendu_ge=0;
        
        shidu_shi=0; 
        shidu_ge=0;  
    } 
    
    P0DIR |= 0x80; //IO����Ҫ�������� 
}


#endif