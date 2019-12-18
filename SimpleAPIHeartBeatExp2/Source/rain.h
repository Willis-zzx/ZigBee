#include <ioCC2530.h>

#define LED1 P1_0    //定义P10口为demo1控制端
#define RAIN_PIN P2_0    

extern void ledInit(void);
extern void rainInit( void );
void ledInit(void)
{
  P1DIR |= 0x01;     //P1_0定义为输出
  P1INP |= 0X01;     //打开下拉
}

void rainInit( void )
{
  P2SEL &= ~0X01;     //设置P20为普通IO口  
  P2DIR &= ~0X01;    // 在P20口，设置为输入模式 
  P2INP &=  ~0x01;   //打开P20上拉电阻,不影响
}