FR8016HA  胰岛素泵

project :  FR801xH-SDK-master\examples\dev1.0\ble_simple_peripheral\keil





外设控制

- [x] 电机     PD5   

- [x] 蜂鸣器 2khz   PA4

- [x] LED_R     LED2

- [x] 开关按键   PA5

- [x] 蓝牙

- [x] vbat 读电压   VBAT

- [x] adc 电位器检测 中值滤波








- [x] 多段基础率设置成功 初步测试通过

开机启动时，时间为12:00:00，蓝牙连接输入以下指令，将按照不同时间段不同速度运行

  { "basal_rate": { "basal_rate_num": "1", "speed": "30", "basal_rate_tim": "00:00-12:00" } } 
  { "basal_rate": { "basal_rate_num": "2", "speed": "25", "basal_rate_tim": "12:00-12:05" } } 
  { "basal_rate": { "basal_rate_num": "3", "speed": "30", "basal_rate_tim": "12:05-12:10" } } 
  { "basal_rate": { "basal_rate_num": "4", "speed": "25", "basal_rate_tim": "12:10-23:59" } } 



![image-20241128173457537](https://newbie-typora.oss-cn-shenzhen.aliyuncs.com/zhongke/image-20241128173457537.png)

![image-20241128173632668](https://newbie-typora.oss-cn-shenzhen.aliyuncs.com/zhongke/image-20241128173632668.png)