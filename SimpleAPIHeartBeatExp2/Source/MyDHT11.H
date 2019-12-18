#ifndef _DHT11_H_
#define _DHT11_H_


#define DHT_PIN P0_6//DHT11 P0_6

extern uint8 dht11Temp,dht11Humidity;
extern uint8 dht11TempDec,dht11HumidityDec;
extern void readDHT11(void);   //声明读DHT11的函数


#endif 