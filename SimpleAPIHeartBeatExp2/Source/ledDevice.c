#include "smartHome.h"
#include "sapi.h"
#include "hal_led.h"
#include "rain.h"
#include "Mydht11.h"
#include "adc.h"
//定义LED灯的编号：我是不同的终端固定不同的LED编号
/*#if 1
  #define NUM_LED_1 1
  #define NUM_LED_2 2
#elif 0*/
#define NUM_LED_1 1
#define NUM_LED_2 2
/*#else
  #define NUM_LED_1 5
  #define NUM_LED_2 6
#endif*/

#define LEDNUM  2 //定义led数量
uint8 ledIdList[LEDNUM]={NUM_LED_1,NUM_LED_2};//定义灯列表

#define NUM_IN_CMD_LEDDEVICE 1 //定义输入命令的数量
#define NUM_OUT_CMD_LEDDEVICE 2 //定义输出命令的数量
const cId_t ledDeviceInputCommandList[NUM_IN_CMD_LEDDEVICE]=
                                {TOGGLE_LED_CMD_ID}; //定义输入命令号 
                                //TOGGLE_LED_CMD_ID：接收协调器发来控制灯状态的信息
const cId_t ledDeviceOutputCommandList[NUM_OUT_CMD_LEDDEVICE]=
                                {LEDJOINNET_CMD_ID,HEART_BEAT_CMD_ID};//定义输出命令号
                                //LEDJOINNET_CMD_ID：发送灯号的命令 
                                //HEART_BEAT_CMD_ID：发送传感器数据的命令

//为simple API定义简单描述符                                
const SimpleDescriptionFormat_t zb_SimpleDesc=
{
  ENDPOINT_ID_SMARTHOME,//Endpoint 终端节点
  PROFILE_ID_SMARTHOME,//Profile ID 个人资料ID
  DEVICE_ID_LEDDEVICE,//Device ID 设备编号
  DEVIDE_VERSION_ID,//Device Version 设备版本
  0,//  Reserved 预留的
  NUM_IN_CMD_LEDDEVICE,//Number of Input Commands 输入的命令数量
  (cId_t*)ledDeviceInputCommandList,//  Input Command List 输入的命令列表
  NUM_OUT_CMD_LEDDEVICE,//  Number of Output Commands 输出的命令数量
  (cId_t*)ledDeviceOutputCommandList  //  Output Command List 输出的命令列表
};
/***********
执行时机：发送的数据包被接收方收到时被调用
handle:包的编号；
status:ZSUCCESS表示成功接收
************/
void zb_SendDataConfirm( uint8 handle, uint8 status )//handle:数据包编号；status:ZSUCCESS表示发送成功；
{
  
}
/***********
执行时机：接收到的数据包被调用
************/
//接收信息，改变灯的状态
void zb_ReceiveDataIndication( uint16 source, uint16 command, 
                              uint16 len, uint8 *pData  )//资源 命令 长度 数据 
{
  if(command==TOGGLE_LED_CMD_ID)
  {
    uint8 led=pData[0];
    if(pData[0]==1)
    {
      HalLedSet(HAL_LED_1,HAL_LED_MODE_ON);//当接收的数据为1：灯亮；若为0：灯灭
    }
    if(pData[0]==0)
    {
      HalLedSet(HAL_LED_1,HAL_LED_MODE_OFF);
    }
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

//void zb_SendDataRequest ( uint16 destination, uint16 commandId, uint8 len,
//  uint8 *pData, uint8 handle, uint8 ack, uint8 radius );
//当构建网络或加入网络成功时被调用
//加入网络后，将led灯号发向协调器
void zb_StartConfirm( uint8 status )
{
  if(status==ZSUCCESS)
  {
    //可把节点所包含的led灯的ID号发送过去
    zb_SendDataRequest(0X0,LEDJOINNET_CMD_ID,
                       LEDNUM,ledIdList,0,FALSE,AF_DEFAULT_RADIUS);
                       //单播 绑定的命令ID 数据长度 数据
    //zb_SendDataRequest发送函数
    osal_start_timerEx(sapi_TaskID,TIMER_TIMEOUT_EVT,2000);//定时器
  }
}
//用来处理用户的自定义事件
//定义各类事件：发送灯号；发送DHT11数据；发送雨滴传感器数据；
void zb_HandleOsalEvent( uint16 event )
{
  if(event&TIMER_TIMEOUT_EVT)//触发定时器
  {
    osal_start_timerEx(sapi_TaskID,TIMER_TIMEOUT_EVT,2000);//定时器
    zb_SendDataRequest(0X0,HEART_BEAT_CMD_ID, 0,NULL,0,FALSE,AF_DEFAULT_RADIUS);
    //单播 绑定的命令ID(发送命令，告诉协调器终端掉线)，
    //发送终端号
    char buff[]="1:";
    zb_SendDataRequest(0X0,0x7,osal_strlen(buff),buff,0,FALSE,AF_DEFAULT_RADIUS);
    //P0SEL &= 0xbf;//？
    //发送DHT11数据
    uint8 buf[2];
    readDHT11();//读DHT11数据的函数
    buf[0] = dht11Temp;
    buf[1] = dht11Humidity;
    zb_SendDataRequest(0X0,0x5,2,buf,0,FALSE,AF_DEFAULT_RADIUS);//发送DHT11数据
    //0x0：单播 0x5：发送DHT11数据的命令ID 数据长度 数据 
    //发送雨滴传感器数据
    uint8 adc[10];
    readAdc(&adc[0],&adc[1],5);//adc[0]:低位 adc[1]:高位
    zb_SendDataRequest(0X0,0x6,osal_strlen(adc),adc,0,FALSE,AF_DEFAULT_RADIUS);
    //0x0：单播 0x6：发送雨滴传感器数据 数据长度 数据 
  }
}
//用户任务通过zb_FindDeviceRequest通过节点的物理地址获取节点的网络地址时的调用 
void zb_FindDeviceConfirm( uint8 searchType, 
                          uint8 *searchKey, uint8 *result )
{
  
}