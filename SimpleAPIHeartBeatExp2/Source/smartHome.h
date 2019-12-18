#ifndef __SMARTHOME_H_
#define __SMARTHOME_H_
#define ENDPOINT_ID_SMARTHOME 0X01//终端节点
#define PROFILE_ID_SMARTHOME 0X401//个人资料ID
#define DEVICE_ID_COORDINATOR 0X01//设备编号
#define DEVICE_ID_LEDDEVICE 0X02
#define DEVIDE_VERSION_ID 0X0//设备版本

#define LEDJOINNET_CMD_ID 0X1 //接收灯号的命令
#define TOGGLE_LED_CMD_ID  0X2//定义协调器向终端发送消息的命令号
#define HEART_BEAT_CMD_ID  0X3//当终端掉线时接收到的命令号
#define  HEART_BEAT_MAX_COUNT 3


#define TIMER_TIMEOUT_EVT 0X01 //自己定义的定时器触发事件
#endif