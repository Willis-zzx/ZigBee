#include "smartHome.h"
#include "sapi.h"
#include "hal_led.h"
#include "osal.h"
#include "hal_uart.h"
#include "stdio.h"
#define NUM_IN_CMD_COORINATOR 2  //定义输入命令的数量
#define NUM_OUT_CMD_COORINATOR 1 //定义输出命令的数量
void *alloceLedDeviceNode(uint8 lednum);
void uart_receive(uint8 port,uint8 event);//定义回调函数
void UartSendNum( uint8 x );//定义发送DHT11数据到esp32函数
const cId_t coordinatorInputCommandList[NUM_IN_CMD_COORINATOR]=
                                {LEDJOINNET_CMD_ID,HEART_BEAT_CMD_ID};
                                //定义协调器接收终端信息的命令号
                                //LEDJOINNET_CMD_ID：接收灯号的命令  0x1
                                //HEART_BEAT_CMD_ID：当终端掉线时接收到的命令 0x3
const cId_t coordinatorOutputCommandList[NUM_OUT_CMD_COORINATOR]=
                                {TOGGLE_LED_CMD_ID};//定义协调器向终端发送消息的命令号 0x3
                                //0x5：接收DHT11数据  
                                //0x6：接收雨滴传感器数据
                                //0x7：接收终端号
//TIMER_TIMEOUT_EVT 自己定义的定时器触发事件 0x01
struct led_device_node //定义led的结构体
{
  struct led_device_node *next; //
  uint8 shortAddr[2];  //led的地址
  uint8 lostHeartCount;//
  uint8 ledNum;//led灯的数量
  uint8 ledId[1];//led的id
};

static struct led_device_node ledDeviceHeader={NULL};//定义灯的头节点

void *alloceLedDeviceNode(uint8 lednum)
{
  return osal_mem_alloc(sizeof(struct led_device_node)-1+lednum);//分配空间
}
//为simple API定义简单描述符 
const SimpleDescriptionFormat_t zb_SimpleDesc=
{
  ENDPOINT_ID_SMARTHOME,//Endpoint 终端节点 0x01
  PROFILE_ID_SMARTHOME,//Profile ID 个人资料ID 0x401
  DEVICE_ID_COORDINATOR,//Device ID 设备编号 0x01
  DEVIDE_VERSION_ID,//Device Version 设备版本 0x0
  0,//  Reserved 预留的
  NUM_IN_CMD_COORINATOR,//Number of Input Commands 输入的命令数量
  (cId_t*)coordinatorInputCommandList,//Input Command List 输入命令列表
  NUM_OUT_CMD_COORINATOR,//  Number of Output Commands 输出的命令数量
  (cId_t*)coordinatorOutputCommandList  //  Output Command List  输出的命令列表
};
/***********
执行时机：发送的数据包被接收方收到时被调用
handle:包的编号；
status:ZSUCCESS表示成功接收
************/
void zb_SendDataConfirm( uint8 handle, uint8 status )//handle:数据包编号；status:ZSUCCESS表示发送成功；
{
  
}
//发送DHT11到esp32串口的函数 
void UartSendNum( uint8 x )
{
  uint8 buf[10];
  buf[0] = x/100+'0';
  buf[1] = x%100/10+'0';
  buf[2] = x%10+'0';
  HalUARTWrite(1,buf,3);
  //HalUARTWrite(1,wenshidu,osal_strlen(wenshidu));
}
/***********
执行时机：接收到数据包时被调用
************/
void zb_ReceiveDataIndication( uint16 source, uint16 command, 
                              uint16 len, uint8 *pData  )//资源 命令 长度 数据
{
  if(command==LEDJOINNET_CMD_ID)//当终端加入网络后，接收灯号的命令
  {
    char buf[100];
    int i;
    struct led_device_node *p=ledDeviceHeader.next;
    while(p!=NULL)
    {
      if( osal_memcmp(pData,p->ledId,len)==TRUE)//
        break;
      else
      {
        p=p->next;
      }
    }
    if(p==NULL)//新节点加入
    {
      struct led_device_node *np=(struct led_device_node *)alloceLedDeviceNode(len);//分配空间
      osal_memcpy(np->shortAddr,&source,2);
      np->ledNum=len;
      osal_memcpy(np->ledId,pData,len); 
      np->next=ledDeviceHeader.next;//头插
      ledDeviceHeader.next=np;
      p=np;
    } 
    else
    {
      osal_memcpy(p->shortAddr,&source,2);
    }
    //向串口发送灯号
    sprintf(buf,"Led device come on!,shortAddr=%u,ledId:",(uint16)p->shortAddr);//向串口发送灯号
    HalUARTWrite(1,buf,osal_strlen(buf));
    for(i=0;i<p->ledNum;i++)
    {
      sprintf(buf,"%u  ",p->ledId[i]);
      HalUARTWrite(1,buf,osal_strlen(buf));
    }
    //HalUARTWrite(1,"\r\n",2);
  }
  else if(command==HEART_BEAT_CMD_ID)//心跳包命令号
  {
    struct led_device_node *p=ledDeviceHeader.next;
    while(p!=NULL)
    {
      if( osal_memcmp(&source,p->shortAddr,2)==TRUE)
        break;
      else
      {
        p=p->next;
      }
    } 
    if(p!=NULL)
    {
      p->lostHeartCount=HEART_BEAT_MAX_COUNT;//HEART_BEAT_MAX_COUNT：3
    }
  }

else if(command==0x7)//接收终端号
{
  char buf[100];
  osal_memcpy(buf,pData,len);
  HalUARTWrite(1,buf,osal_strlen(buf));
}
  else if(command==0x5)//发送DHT11数据
  {
    char buf[100];
    //osal_memcpy(buf,pData,len);
    buf[0]=pData[0];//温度
    buf[1]=pData[1];//湿度
    HalUARTWrite(1,"T:",osal_strlen("T:"));
    UartSendNum(buf[0]);//温度
    HalUARTWrite(1," H:",osal_strlen(" H:"));
    UartSendNum(buf[1]);//湿度
  }
   else if(command==0x6)//发送雨滴传感器数据
  {
    char buf[100];
    unsigned int adc;
    adc=pData[0];
    adc|=((unsigned int)pData[1])<<8;
    adc>>2;
    sprintf(buf,"G:%d",adc);
    HalUARTWrite(1,buf,osal_strlen(buf));
  }
  
}

//其它节点绑定到该节点时调用 source:发送绑定请求设备的网络地址
void zb_AllowBindConfirm( uint16 source )
{
}
//用来处理按键消息
void zb_HandleKeys( uint8 shift, uint8 keys )
{
  
}
//用户任务的绑定成功后被调用 commandId :绑定的命令号；status:ZSUCCESS表示成功
void zb_BindConfirm( uint16 commandId, uint8 status )
{
}

//当构建网络或加入网络成功时被调用
void zb_StartConfirm( uint8 status )
{

  if(status==ZSUCCESS)
  {
    //初始化串口
  halUARTCfg_t uartcfg;//halUARTCfg_t：串口结构体
  uartcfg.baudRate=HAL_UART_BR_115200;//定义波特率
  uartcfg.flowControl=FALSE;//关闭流控
  uartcfg.callBackFunc=uart_receive;//定义回调函数
  //uartcfg.callBackFunc=NULL;
  HalUARTOpen(1,&uartcfg);//打开串口
    char buf[]="Coordinator is created successfully!";//当构建网络成功了向串口发送构建成功的信息
    HalUARTWrite(1,buf,osal_strlen(buf));
    osal_start_timerEx(sapi_TaskID,TIMER_TIMEOUT_EVT,2000);//定时器
  }  
}
//用来处理用户的自定义事件
void zb_HandleOsalEvent( uint16 event )
{
  if(event&TIMER_TIMEOUT_EVT)//触发定时器
  {
    struct led_device_node *p=ledDeviceHeader.next;//定义结构体变量
    struct led_device_node *pre=ledDeviceHeader.next;
    osal_start_timerEx(sapi_TaskID,TIMER_TIMEOUT_EVT,2000);//不断触发？
    while(p!=NULL)
    {
      p->lostHeartCount--;
      if(p->lostHeartCount<=0)//当灯的头节点<=0时进入循环
      {
          char buf[100];
         struct led_device_node *pTmp=p;
         pre->next=p->next;  
         p=p->next;
         sprintf(buf,"endpoint:shortAddr=%u is off-line！",(uint16)pTmp->shortAddr);//打印掉线终端的地址
         HalUARTWrite(1,buf,osal_strlen(buf));
         continue;
      }
      pre=p;//遍历
      p=p->next;
    }    
  }
}
//用户任务通过zb_FindDeviceRequest通过节点的物理地址获取节点的网络地址时的调用 
void zb_FindDeviceConfirm( uint8 searchType, 
                          uint8 *searchKey, uint8 *result )
{
  
}
//声明接收信息函数
void uart_receive(uint8 port,uint8 event)
{
  uint16 dstAddr;
  if(event& (HAL_UART_RX_FULL|HAL_UART_RX_ABOUT_FULL|HAL_UART_RX_TIMEOUT))
  {
    struct led_device_node *p=ledDeviceHeader.next;
    struct led_device_node *pre=ledDeviceHeader.next;
    char buff[20];
    HalUARTRead(1,buff,2);//读串口发来的数据
    while(p!=NULL)
    {
      char id[1];
      sprintf(id,"%d",(char)p->ledId[0]);//将灯id号复制到新定义命名为id的变量中
      if(id[0]==buff[0])//将灯的ID号与接收到的数据的第一位相比对；
                        // 接收的数据为两位数，第一位是灯的编号，第二位是控制灯的状态
      {
        //HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);
        uint8 buf[1];//新定义一个buf变量
        if(buff[1]=='1')//若是接收的数据buff的第二位为1，则将buf[0]=1；若不为1，则buf[0]=0
        {
          buf[0]=1;
        }
        else{buf[0]=0;}
        struct led_device_node *pTmp=p;
        pre->next=p->next;//遍历节点
        p=p->next;
        osal_memcpy(&dstAddr,pTmp->shortAddr,2);//寻得终端的地址
        zb_SendDataRequest(dstAddr,TOGGLE_LED_CMD_ID,
                       1,buf,0,FALSE,AF_DEFAULT_RADIUS);
                       //单播地址 绑定的命令ID 数据长度 数据 
                       //将buf发送到终端，buf[0]=1：灯亮；buf[0]！=1：灯灭
                       //TOGGLE_LED_CMD_ID：协调器向终端发送信息的命令号
        continue;
      }
      pre=p;
      p=p->next;
    }
  }  
}