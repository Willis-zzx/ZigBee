#include "adc.h"
#include "OnBoard.h"
#include "ioCC2530.h"
#include <stdio.h>
#define ADC_CHNN  0x05  //选用通道5 P0_5
#define uchar unsigned char 
#define uint unsigned int
void Get_ADCValue(unsigned char* AvgValue);
/********************************************************
 * @brief   读取ADC的值
********************************************************/
void readAdc(uint8 *D,uint8 *G,uint8 channal)
{
          
         
          APCFG |= 1 << channal ; // 模拟信号设置
          ADCIF = 0 ;
         
          ADCCON3 = channal;          
          while ( !ADCIF ) ;  //等待转换完成
          
          *D = ADCL ;
          *D |= ADCH ;
          
          //return value; 
}
