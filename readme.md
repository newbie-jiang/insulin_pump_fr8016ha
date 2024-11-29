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

```
  { "basal_rate": { "basal_rate_num": "1", "speed": "30", "basal_rate_tim": "00:00-12:00" } } 
  { "basal_rate": { "basal_rate_num": "2", "speed": "25", "basal_rate_tim": "12:00-12:05" } } 
  { "basal_rate": { "basal_rate_num": "3", "speed": "30", "basal_rate_tim": "12:05-12:10" } } 
  { "basal_rate": { "basal_rate_num": "4", "speed": "25", "basal_rate_tim": "12:10-23:59" } } 
```

 



![image-20241128173457537](https://newbie-typora.oss-cn-shenzhen.aliyuncs.com/zhongke/image-20241128173457537.png)

![image-20241128173632668](https://newbie-typora.oss-cn-shenzhen.aliyuncs.com/zhongke/image-20241128173632668.png)



- [x] 三种大剂量模式的参数定义与数据解析实现

- 常规大剂量指令

```json
  { 
  "normal_large_dose": {        //常规大剂量
  "large_dose_liquid": "xx.x"   //液量
  }} 


-----example

{"normal_large_dose": {"large_dose_liquid": "9.5"}} 
 
```

![image-20241129145804460](https://newbie-typora.oss-cn-shenzhen.aliyuncs.com/zhongke/image-20241129145804460.png)





- 方波大剂量指令

```json
  { 
  "square_wave_large_dose": {   //方波大剂量 
  "large_dose_liquid": "xx.x",  //液量
  "tim_hh":"xx",                //时间 h
  "tim_min":"xx"                //时间 min
  }}  

-----example

{"square_wave_large_dose":{"large_dose_liquid":"10.5","tim_hh":"02","tim_min":"30"}}  

```

![image-20241129145901952](https://newbie-typora.oss-cn-shenzhen.aliyuncs.com/zhongke/image-20241129145901952.png)



- 双波大剂量指令

```json
{
 "double_wave_large_dose":{      //双波大剂量
 "all_liquid":"xx.x",            //总液量
 "proportion":"xx",              //比例
 "tim_hh":"xx",                  //时间 h
 "tim_min":"xx"                  //时间 min 
}}

-----example

{"double_wave_large_dose":{"all_liquid":"15","proportion":"50", "tim_hh":"1", "tim_min":"10"}}  

```

![image-20241129145941961](https://newbie-typora.oss-cn-shenzhen.aliyuncs.com/zhongke/image-20241129145941961.png)







