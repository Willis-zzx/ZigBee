# ZigBee
利用ZigBee协议构建组网，终端带有DHT11以及雨滴传感器，协调器借有esp32WIFI模块，终端采集数据传向协调器，协调器通过esp32串口通信，传向MQTT服务器，移动端安卓app上可以实时查看终端传感器数据；另一面，移动端可以通过发布消息到MQTT服务器，esp32订阅相同的主题以接收到信息，然后将将此信息写入协调器，协调器再传入终端，以此来控制终端灯的亮灭。数据一来一回，一收一发
