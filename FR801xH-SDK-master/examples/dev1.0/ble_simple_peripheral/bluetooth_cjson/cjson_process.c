#include  "cjson_process.h"
#include  "Cjson.h"
#include  "stdint.h"
#include  "co_printf.h"
#include  <stdlib.h>
#include  <stdio.h>

  //基础率
  basal_rate_information     rate_info        = {0};
  basal_rate_information     s_rate_info[48]  = {0}; /* 半个小时一段时间 */
  
  //时间同步
  sync_tim                   current_time     = {0};
  
  large_dose_information     large_dose_info  = {0};
  pack_num                   s_pack_num       = {0};
  
  //常规大剂量
  normal_large_dose          n_large_dose     = {0};
  square_wave_large_dose     s_w_large_dose   = {0};
  double_wave_large_dose     d_w_large_dose   = {0};
  
   
  //最大基础率段数
  static uint32_t max_basal_rate_num;
  
  
  //获得最大基础率段
  uint32_t get_max_basal_rate_num(void)
  {	  
	return max_basal_rate_num;   
  }
  

  /* 基础率数据指令  
   {
     "basal_rate":   
    {
     "basal_rate_num": "2",   //基础率段数，第几段
     "speed": "8.5",          //基础率速度
     "basal_rate_tim": "01:30-20:30" //基础率 起始时间 - 结束时间
    }
   }  

   {
     "basal_rate":   
    {
     "basal_rate_num": "2",
     "speed": "8.5",          
     "basal_rate_tim": "01:30-20:30" 
    }
   }      
   */
  
void basal_rate_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on) {
	
    if (len == 0 || dbg_on == 0) return;

    // 将接收的数据转为字符串
    char buffer[150] = {0};
    for (uint32_t i = 0; i < len && i < sizeof(buffer) - 1; i++) {
        buffer[i] = data[i];
    }
    buffer[len] = '\0'; // 确保字符串以 null 结尾

    // 打印接收的 JSON 数据
    co_printf("Received JSON: %s\n", buffer);

    // 解析 JSON 数据
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        co_printf("JSON parsing failed!\n");
        return;
    }

    // 创建 basal_rate_information 结构体实例
  

    // 提取 "basal_rate" 对象
    cJSON *basal_rate = cJSON_GetObjectItemCaseSensitive(root, "basal_rate");
    if (basal_rate == NULL) {
        co_printf("basal_rate object not found!\n");
        cJSON_Delete(root);
        return;
    }else{
		
	 s_pack_num.cjson_instruct_num = 1;
	
	}

    // 提取 "basal_rate_num"
    cJSON *basal_rate_num = cJSON_GetObjectItemCaseSensitive(basal_rate, "basal_rate_num");
    if (cJSON_IsString(basal_rate_num) && (basal_rate_num->valuestring != NULL)) {
        rate_info.basal_rate_num = atoi(basal_rate_num->valuestring);
    }

    // 提取 "speed"
    cJSON *speed = cJSON_GetObjectItemCaseSensitive(basal_rate, "speed");
    if (cJSON_IsString(speed) && (speed->valuestring != NULL)) {
        rate_info.basal_rate_speed = atof(speed->valuestring);
    }

    // 提取 "basal_rate_tim"
    cJSON *basal_rate_tim = cJSON_GetObjectItemCaseSensitive(basal_rate, "basal_rate_tim");
    if (cJSON_IsString(basal_rate_tim) && (basal_rate_tim->valuestring != NULL)) {
        // 分割时间段
        char start_time[6] = {0};
        char end_time[6] = {0};
        sscanf(basal_rate_tim->valuestring, "%5[^-]-%5s", start_time, end_time);

        // 分解起始时间
        sscanf(start_time, "%2hhd:%2hhd", &rate_info.basal_rate_start_tim_hh, &rate_info.basal_rate_start_tim_min);

        // 分解结束时间
        sscanf(end_time, "%2hhd:%2hhd", &rate_info.basal_rate_end_tim_hh, &rate_info.basal_rate_end_tim_min);
    }

    // 打印解析结果
    co_printf("Parsed basal_rate_information:\n");
    co_printf("  basal_rate_num: %d\n", rate_info.basal_rate_num);   
	printf("basal_rate_speed : %f\n",rate_info.basal_rate_speed);
    co_printf("  Start time: %02d:%02d\n", rate_info.basal_rate_start_tim_hh, rate_info.basal_rate_start_tim_min);
    co_printf("  End time: %02d:%02d\n", rate_info.basal_rate_end_tim_hh, rate_info.basal_rate_end_tim_min);
	
	
	//信息拷贝至对应的结构体数组中
	s_rate_info[rate_info.basal_rate_num-1] = rate_info;
	
	
//	//打印拷贝的数据
//	co_printf("copy rate_info info:\r\n");
//	printf("copy basal_rate_num:%d\r\n",  s_rate_info[rate_info.basal_rate_num-1].basal_rate_num);
//	printf("copy basal_rate_speed:%f\r\n",s_rate_info[rate_info.basal_rate_num-1].basal_rate_speed);
//	co_printf("  Start time: %02d:%02d\n",s_rate_info[rate_info.basal_rate_num-1].basal_rate_start_tim_hh, s_rate_info[rate_info.basal_rate_num-1].basal_rate_start_tim_min);
//    co_printf("  End time: %02d:%02d\n",  s_rate_info[rate_info.basal_rate_num-1].basal_rate_end_tim_hh,   s_rate_info[rate_info.basal_rate_num-1].basal_rate_end_tim_min);
	
	//获得最大基础率段数 一天24h 获取最后时间的段数即可知道最大多少段
	if(rate_info.basal_rate_end_tim_hh==23&&rate_info.basal_rate_end_tim_min==59)
	{
	  max_basal_rate_num = rate_info.basal_rate_num;	
      printf(" max_basal_rate_num is %d \r\n ",max_basal_rate_num);		
	}
	
    // 清理 JSON 内存
    cJSON_Delete(root);
}
   
  
  /************************************同步时间***********************************/  
/*
{
  "sync_tim": {      
    "now_tim": "2024-11-20 12:20:30"
  }
}   
*/

//void sync_tim_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on) {
//	
//    if (len == 0 || data == NULL) return;

//    // 将接收的数据转为字符串
//    char buffer[150] = {0};
//    for (uint32_t i = 0; i < len && i < sizeof(buffer) - 1; i++) {
//        buffer[i] = data[i];
//    }
//    buffer[len] = '\0'; // 确保字符串以 null 结尾

//    // 打印接收到的 JSON 数据
//    co_printf("Received JSON: %s\n", buffer);

//    // 解析 JSON 数据
//    cJSON *root = cJSON_Parse(buffer);
//    if (root == NULL) {
//        co_printf("JSON parsing failed!\n");
//        return;
//    }

//    // 创建 sync_tim 结构体实例
//    

//    // 提取 "sync_tim" 对象
//    cJSON *sync_tim_obj = cJSON_GetObjectItemCaseSensitive(root, "sync_tim");
//    if (sync_tim_obj == NULL) {
//        co_printf("sync_tim object not found!\n");
//        cJSON_Delete(root);
//        return;
//    }else{
//	
//	 s_pack_num.cjson_instruct_num = 2;
//		
//	}

//    // 提取 "now_tim"
//    cJSON *now_tim = cJSON_GetObjectItemCaseSensitive(sync_tim_obj, "now_tim");
//    if (cJSON_IsString(now_tim) && (now_tim->valuestring != NULL)) {
//        // 分解时间信息
//        sscanf(now_tim->valuestring, "%2hhd:%2hhd:%2hhd", 
//               &current_time.sync_tim_hh, 
//               &current_time.sync_tim_min, 
//               &current_time.sync_tim_s);
//    } else {
//        co_printf("now_tim not found or invalid format!\n");
//        cJSON_Delete(root);
//        return;
//    }

//    // 打印解析结果
//    co_printf("Parsed sync_tim:\n");
//    co_printf("  Hours: %02d\n", current_time.sync_tim_hh);
//    co_printf("  Minutes: %02d\n", current_time.sync_tim_min);
//    co_printf("  Seconds: %02d\n", current_time.sync_tim_s);

//    // 清理 JSON 内存
//    cJSON_Delete(root);
//}

void sync_tim_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on) {
    if (len == 0 || data == NULL) return;

    // 将接收的数据转为字符串
    char buffer[150] = {0};
    for (uint32_t i = 0; i < len && i < sizeof(buffer) - 1; i++) {
        buffer[i] = data[i];
    }
    buffer[len] = '\0'; // 确保字符串以 null 结尾

    // 打印接收到的 JSON 数据
    if (dbg_on) {
        co_printf("Received JSON: %s\n", buffer);
    }

    // 解析 JSON 数据
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        co_printf("JSON parsing failed!\n");
        return;
    }

    // 提取 "sync_tim" 对象
    cJSON *sync_tim_obj = cJSON_GetObjectItemCaseSensitive(root, "sync_tim");
    if (sync_tim_obj == NULL) {
        co_printf("sync_tim object not found!\n");
        cJSON_Delete(root);
        return;
    }else{
	
	 s_pack_num.cjson_instruct_num = 2;
	}

    // 提取 " now_tim "
    cJSON *now_tim = cJSON_GetObjectItemCaseSensitive(sync_tim_obj, "now_tim");
    if (cJSON_IsString(now_tim) && (now_tim->valuestring != NULL)) {
        // 分解时间信息
        int result = sscanf(now_tim->valuestring, "%4hu-%2hu-%2hu %2hu:%2hu:%2hu", 
                            &current_time.sync_tim_year, 
                            &current_time.sync_tim_month, 
                            &current_time.sync_tim_day, 
                            &current_time.sync_tim_hh, 
                            &current_time.sync_tim_min, 
                            &current_time.sync_tim_s);

        if (result == 6) {
            // 打印解析结果
            if (dbg_on) {
                co_printf("Parsed sync_tim:\n");
                co_printf("  Year: %04d\n", current_time.sync_tim_year);
                co_printf("  Month: %02d\n", current_time.sync_tim_month);
                co_printf("  Day: %02d\n", current_time.sync_tim_day);
                co_printf("  Hours: %02d\n", current_time.sync_tim_hh);
                co_printf("  Minutes: %02d\n", current_time.sync_tim_min);
                co_printf("  Seconds: %02d\n", current_time.sync_tim_s);
            }

            // 可以在这里添加将 current_time 同步到系统时间的逻辑
            // update_system_time(current_time); // 示例函数
        } else {
            co_printf("Invalid time format in now_tim!\n");
        }
    } else {
        co_printf("now_tim not found or invalid format!\n");
    }
    // 清理 JSON 内存
    cJSON_Delete(root);
}











//大剂量设定
//typedef struct large_dose{
//  	char large_dose_liquid; //大剂量液量
//	char large_dose_speed;  //大剂量速度
//}large_dose;
/*
 
  {
   "large_dose": 
   {      
     "large_dose_liquid": "xxx.x",
     "large_dose_speed": "xx.x"
   }
  }

  {
   "large_dose": 
   {      
     "large_dose_liquid": "002.5",
     "large_dose_speed": "02.5"
   }
  }

*/

/* 大剂量设置 */
void large_dose_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on)
{
	 if (len == 0 || dbg_on == 0) return;

    // 将接收的数据转为字符串
    char buffer[150] = {0};
    for (uint32_t i = 0; i < len && i < sizeof(buffer) - 1; i++) {
        buffer[i] = data[i];
    }
    buffer[len] = '\0'; // 确保字符串以 null 结尾

    // 打印接收的 JSON 数据
    co_printf("Received JSON: %s\n", buffer);

    // 解析 JSON 数据
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        co_printf("JSON parsing failed!\n");
        return;
    }

    // 提取 "large_dose" 对象
    cJSON *large_dose = cJSON_GetObjectItemCaseSensitive(root, "large_dose");
    if (large_dose == NULL) {
        co_printf("large_dose object not found!\n");
        cJSON_Delete(root);
        return;
    }else{
	
	  s_pack_num.cjson_instruct_num = 3;
	}

    // 提取 "large_dose_liquid"
    cJSON *large_dose_liquid = cJSON_GetObjectItemCaseSensitive(large_dose, "large_dose_liquid");
    if (cJSON_IsString(large_dose_liquid) && (large_dose_liquid->valuestring != NULL)) {
        large_dose_info.large_dose_liquid = atof(large_dose_liquid->valuestring);
    }

    // 提取 "large_dose_speed"
    cJSON *large_dose_speed = cJSON_GetObjectItemCaseSensitive(large_dose, "large_dose_speed");
    if (cJSON_IsString(large_dose_speed) && (large_dose_speed->valuestring != NULL)) {
        large_dose_info.large_dose_speed = atof(large_dose_speed->valuestring);
    }

    // 打印解析结果
    co_printf("Parsed large_dose_information:\n");
	
	printf("  large_dose_liquid scaled=: %f\r\n", large_dose_info.large_dose_liquid);
    printf("  large_dose_speed scaled: %f\r\n", large_dose_info.large_dose_speed);

    // 清理 JSON 内存
    cJSON_Delete(root);
}


/*********************************ACK*************************************/

//基础率ACK
void ack_basal_rate_cjson_process(basal_rate_information *p_basal_rate_info, char *output_buffer, uint32_t buffer_size)
{	
    if (p_basal_rate_info == NULL || output_buffer == NULL || buffer_size == 0) {
        co_printf("Invalid input parameter\n");
        return;
    }

    // 创建 JSON 根对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        co_printf("Failed to create JSON root object\n");
        return;
    }

    // 创建 "basal_rate" 对象
    cJSON *basal_rate = cJSON_CreateObject();
    if (basal_rate == NULL) {
        co_printf("Failed to create basal_rate object\n");
        cJSON_Delete(root);
        return;
    }

    // 添加 basal_rate_num
    cJSON_AddStringToObject(basal_rate, "basal_rate_num", 
                            (char[3]){p_basal_rate_info->basal_rate_num + '0', '\0'});
    
    // 添加 speed
    char speed_str[16];
    snprintf(speed_str, sizeof(speed_str), "%.1f", p_basal_rate_info->basal_rate_speed); // 正常处理速度
    cJSON_AddStringToObject(basal_rate, "speed", speed_str);

    // 添加 basal_rate_tim
    char time_range[32];
    snprintf(time_range, sizeof(time_range), 
             "%02d:%02d-%02d:%02d", 
             p_basal_rate_info->basal_rate_start_tim_hh, 
             p_basal_rate_info->basal_rate_start_tim_min, 
             p_basal_rate_info->basal_rate_end_tim_hh, 
             p_basal_rate_info->basal_rate_end_tim_min);
    cJSON_AddStringToObject(basal_rate, "basal_rate_tim", time_range);

    // 将 "basal_rate" 添加到根对象
    cJSON_AddItemToObject(root, "basal_rate", basal_rate);

    // 将 JSON 对象转换为字符串
    char *json_data = cJSON_PrintUnformatted(root);
    if (json_data == NULL) {
        co_printf("Failed to print JSON data\n");
        cJSON_Delete(root);
        return;
    }

    // 检查缓冲区大小，确保不发生溢出
    if (strlen(json_data) < buffer_size) {
        strncpy(output_buffer, json_data, buffer_size - 1);
        output_buffer[buffer_size - 1] = '\0'; // 确保字符串以 NULL 结尾
    } else {
        co_printf("Buffer size too small for JSON data\n");
    }

    // 清理 JSON 字符串和对象内存
    cJSON_free(json_data);
    cJSON_Delete(root);
}


//时间同步ack
void ack_sync_tim_cjson_process(sync_tim *p_sync_tim, char *output_buffer, uint32_t buffer_size) {
    if (p_sync_tim == NULL || output_buffer == NULL || buffer_size == 0) {
        co_printf("Invalid input parameter\n");
        return;
    }

    // 创建 JSON 根对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        co_printf("Failed to create JSON root object\n");
        return;
    }

    // 创建 "sync_tim" 对象
    cJSON *sync_tim_obj = cJSON_CreateObject();
    if (sync_tim_obj == NULL) {
        co_printf("Failed to create sync_tim object\n");
        cJSON_Delete(root);
        return;
    }

    // 将结构体数据转换为字符串格式 "YYYY-MM-DD HH:MM:SS"
    char now_tim[30] = {0};
    snprintf(now_tim, sizeof(now_tim), "%04d-%02d-%02d %02d:%02d:%02d",
             p_sync_tim->sync_tim_year,
             p_sync_tim->sync_tim_month,
             p_sync_tim->sync_tim_day,
             p_sync_tim->sync_tim_hh,
             p_sync_tim->sync_tim_min,
             p_sync_tim->sync_tim_s);

    // 添加字段到 JSON 对象
    cJSON_AddStringToObject(sync_tim_obj, "now_tim", now_tim);

    // 将 "sync_tim" 对象添加到根对象中
    cJSON_AddItemToObject(root, "sync_tim", sync_tim_obj);

    // 转换为 JSON 字符串
    char *json_str = cJSON_PrintUnformatted(root);  // 使用无格式化版本以节省空间
    if (json_str == NULL) {
        co_printf("Failed to create JSON string\n");
        cJSON_Delete(root);
        return;
    }

    // 检查缓冲区大小，确保不会溢出
    if (strlen(json_str) < buffer_size) {
        strncpy(output_buffer, json_str, buffer_size - 1);
        output_buffer[buffer_size - 1] = '\0'; // 确保字符串以 NULL 结尾
    } else {
        co_printf("Buffer size too small for JSON data\n");
    }

    // 清理 JSON 字符串和对象内存
    cJSON_free(json_str);
    cJSON_Delete(root);
}



/* 大剂量ack */
void ack_large_dose_cjson_process(large_dose_information *p_large_dose_info, char *output_buffer, uint32_t buffer_size) {
    if (p_large_dose_info == NULL || output_buffer == NULL || buffer_size == 0) {
        co_printf("Invalid input parameter\n");
        return;
    }

    // 创建 JSON 根对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        co_printf("Failed to create JSON root object\n");
        return;
    }

    // 创建 "large_dose" 对象
    cJSON *large_dose = cJSON_CreateObject();
    if (large_dose == NULL) {
        co_printf("Failed to create large_dose object\n");
        cJSON_Delete(root);
        return;
    }

    // 添加 "large_dose_liquid"
    char liquid_str[16] = {0};
    snprintf(liquid_str, sizeof(liquid_str), "%.1f", p_large_dose_info->large_dose_liquid);
    cJSON_AddStringToObject(large_dose, "large_dose_liquid", liquid_str);

    // 添加 "large_dose_speed"
    char speed_str[16] = {0};
    snprintf(speed_str, sizeof(speed_str), "%.1f", p_large_dose_info->large_dose_speed);
    cJSON_AddStringToObject(large_dose, "large_dose_speed", speed_str);

    // 将 "large_dose" 添加到根对象
    cJSON_AddItemToObject(root, "large_dose", large_dose);

    // 转换为 JSON 字符串
    char *json_data = cJSON_PrintUnformatted(root);
    if (json_data == NULL) {
        co_printf("Failed to create JSON string\n");
        cJSON_Delete(root);
        return;
    }

    // 检查缓冲区大小，确保不发生溢出
    if (strlen(json_data) < buffer_size) {
        strncpy(output_buffer, json_data, buffer_size - 1);
        output_buffer[buffer_size - 1] = '\0'; // 确保字符串以 NULL 结尾
    } else {
        co_printf("Buffer size too small for JSON data\n");
    }
	

    // 清理 JSON 字符串和对象内存
    cJSON_free(json_data);
    cJSON_Delete(root);
}


/*
typedef struct normal_large_dose{	
	
   float large_dose_liquid; //大剂量液量

}normal_large_dose;

extern  normal_large_dose          n_large_dose;  


 instruction:
  
{ 
  "normal_large_dose":           //常规大剂量
  {       
  "large_dose_liquid": "xx.x"    //液量
  }
}

 -----example

{ 
  "normal_large_dose": 
  {       
  "large_dose_liquid": "9.5"   
  }
}
*/

// 正常模式大剂量
void normal_large_dose_cjson_process(uint8_t *data, uint32_t len, uint8_t dbg_on)
{
    if (len == 0 || dbg_on == 0) return;

    // 将接收的数据转为字符串
    char buffer[150] = {0};
    uint32_t copy_len = (len < sizeof(buffer) - 1) ? len : (sizeof(buffer) - 1);
    memcpy(buffer, data, copy_len);
    buffer[copy_len] = '\0'; // 确保字符串以 null 结尾

    // 打印接收的 JSON 数据
    co_printf("Received JSON: %s\n", buffer);

    // 解析 JSON 数据
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        co_printf("JSON parsing failed!\n");
        return;
    }

    // 提取 "normal_large_dose" 对象
    cJSON *normal_large_dose = cJSON_GetObjectItemCaseSensitive(root, "normal_large_dose");
    if (normal_large_dose == NULL) {
        co_printf("normal_large_dose object not found!\n");
        cJSON_Delete(root);
        return;
    } else {
        //设置相关指令号或状态
        s_pack_num.cjson_instruct_num = 4;
    }

    // 提取 "large_dose_liquid"
    cJSON *large_dose_liquid = cJSON_GetObjectItemCaseSensitive(normal_large_dose, "large_dose_liquid");
    if (cJSON_IsString(large_dose_liquid) && (large_dose_liquid->valuestring != NULL)) {
        n_large_dose.large_dose_liquid = atof(large_dose_liquid->valuestring);
    } else {
        co_printf("large_dose_liquid is missing or not a string!\n");
    }

    // 打印解析结果
    co_printf("Parsed normal_large_dose information:\n");
    printf("  large_dose_liquid: %f\n", n_large_dose.large_dose_liquid);

    // 清理 JSON 内存
    cJSON_Delete(root);
}


/* 

typedef struct square_wave_large_dose{
	
	 float large_dose_liquid; //大剂量液量
	 uint16_t tim_hh;    //大剂量时间 hh
	 uint16_t tim_min;   //大剂量时间 min

}square_wave_large_dose;

 extern  square_wave_large_dose     s_w_large_dose;  



 instruction:

  { 
   "square_wave_large_dose": 
   {   //方波大剂量 
   "large_dose_liquid": "xx.x",  //液量
   "tim_hh":"xx",                //时间 h
   "tim_min":"xx"                //时间 min
   }
  }   

 -----example
 { 
 "square_wave_large_dose":
   {   
   "large_dose_liquid":"10.5",
   "tim_hh":"02",              
   "tim_min":"30"                
   }
 }   

*/

// 方波模式大剂量
void square_wave_large_dose_process(uint8_t *data, uint32_t len, uint8_t dbg_on)
{
    if (len == 0 || dbg_on == 0) return;

    // 将接收的数据转为字符串
    char buffer[150] = {0};
    uint32_t copy_len = (len < sizeof(buffer) - 1) ? len : (sizeof(buffer) - 1);
    memcpy(buffer, data, copy_len);
    buffer[copy_len] = '\0'; // 确保字符串以 null 结尾

    // 打印接收的 JSON 数据
    co_printf("Received JSON: %s\n", buffer);

    // 解析 JSON 数据
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        co_printf("JSON parsing failed!\n");
        return;
    }

    // 提取 "square_wave_large_dose" 对象
    cJSON *square_wave_large_dose = cJSON_GetObjectItemCaseSensitive(root, "square_wave_large_dose");
    if (square_wave_large_dose == NULL) {
        co_printf("square_wave_large_dose object not found!\n");
        cJSON_Delete(root);
        return;
    } else {
        // 设置相关指令号或状态
        s_pack_num.cjson_instruct_num = 5; // 假设指令号为5，依据实际需求调整
    }

    // 提取 "large_dose_liquid"
    cJSON *large_dose_liquid = cJSON_GetObjectItemCaseSensitive(square_wave_large_dose, "large_dose_liquid");
    if (cJSON_IsString(large_dose_liquid) && (large_dose_liquid->valuestring != NULL)) {
        s_w_large_dose.large_dose_liquid = atof(large_dose_liquid->valuestring);
    } else {
        co_printf("large_dose_liquid is missing or not a string!\n");
    }

    // 提取 "tim_hh"
    cJSON *tim_hh = cJSON_GetObjectItemCaseSensitive(square_wave_large_dose, "tim_hh");
    if (cJSON_IsString(tim_hh) && (tim_hh->valuestring != NULL)) {
        s_w_large_dose.tim_hh = (uint16_t)atoi(tim_hh->valuestring);
    } else {
        co_printf("tim_hh is missing or not a string!\n");
    }

    // 提取 "tim_min"
    cJSON *tim_min = cJSON_GetObjectItemCaseSensitive(square_wave_large_dose, "tim_min");
    if (cJSON_IsString(tim_min) && (tim_min->valuestring != NULL)) {
        s_w_large_dose.tim_min = (uint16_t)atoi(tim_min->valuestring);
    } else {
        co_printf("tim_min is missing or not a string!\n");
    }

    // 打印解析结果
    co_printf("Parsed square_wave_large_dose information:\n");
    printf("  large_dose_liquid: %.2f\n", s_w_large_dose.large_dose_liquid);
    printf("  tim_hh: %u\n", s_w_large_dose.tim_hh);
    printf("  tim_min: %u\n", s_w_large_dose.tim_min);

    // 清理 JSON 内存
    cJSON_Delete(root);
}



/*

typedef struct double_wave_large_dose{
	
	 float all_liquid;  //总的大剂量
	
	 uint8_t proportion;           //输注比例
	
	 uint16_t tim_hh;     //第二段方波时间 hh
	 uint16_t tim_min;    //第二段方波时间 min
	
}double_wave_large_dose;


 extern  double_wave_large_dose     d_w_large_dose;   


  instruction:

{
  "double_wave_large_dose":       //双波大剂量
  {     
  "all_liquid":"xx.x",            //总液量
  "proportion":"xx",              //比例
  "tim_hh":"xx",                  //双波时间 h
  "tim_min":"xx"                  //双波时间 min 
  }
}

 -----example

{
  "double_wave_large_dose":       
  {     
  "all_liquid":"15",            
  "proportion":"50",              
  "tim_hh":"1",                  
  "tim_min":"10"                  
  }
}

*/

// 双波模式大剂量
void double_wave_large_dose_process(uint8_t *data, uint32_t len, uint8_t dbg_on)
{
    if (len == 0 || dbg_on == 0) return;

    // 将接收的数据转为字符串
    char buffer[150] = {0};
    uint32_t copy_len = (len < sizeof(buffer) - 1) ? len : (sizeof(buffer) - 1);
    memcpy(buffer, data, copy_len);
    buffer[copy_len] = '\0'; // 确保字符串以 null 结尾

    // 打印接收的 JSON 数据
    co_printf("Received JSON: %s\n", buffer);

    // 解析 JSON 数据
    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        co_printf("JSON parsing failed!\n");
        return;
    }

    // 提取 "double_wave_large_dose" 对象
    cJSON *double_wave_large_dose = cJSON_GetObjectItemCaseSensitive(root, "double_wave_large_dose");
    if (double_wave_large_dose == NULL) {
        co_printf("double_wave_large_dose object not found!\n");
        cJSON_Delete(root);
        return;
    } else {
        // 设置相关指令号或状态
        s_pack_num.cjson_instruct_num = 6; // 假设指令号为6，依据实际需求调整
    }

    // 提取 "all_liquid"
    cJSON *all_liquid = cJSON_GetObjectItemCaseSensitive(double_wave_large_dose, "all_liquid");
    if (cJSON_IsString(all_liquid) && (all_liquid->valuestring != NULL)) {
        d_w_large_dose.all_liquid = atof(all_liquid->valuestring);
    } else {
        co_printf("all_liquid is missing or not a string!\n");
    }

    // 提取 "proportion"
    cJSON *proportion = cJSON_GetObjectItemCaseSensitive(double_wave_large_dose, "proportion");
    if (cJSON_IsString(proportion) && (proportion->valuestring != NULL)) {
        d_w_large_dose.proportion = (uint8_t)atoi(proportion->valuestring);
    } else {
        co_printf("proportion is missing or not a string!\n");
    }

    // 提取 "tim_hh"
    cJSON *tim_hh = cJSON_GetObjectItemCaseSensitive(double_wave_large_dose, "tim_hh");
    if (cJSON_IsString(tim_hh) && (tim_hh->valuestring != NULL)) {
        d_w_large_dose.tim_hh = (uint16_t)atoi(tim_hh->valuestring);
    } else {
        co_printf("tim_hh is missing or not a string!\n");
    }

    // 提取 "tim_min"
    cJSON *tim_min = cJSON_GetObjectItemCaseSensitive(double_wave_large_dose, "tim_min");
    if (cJSON_IsString(tim_min) && (tim_min->valuestring != NULL)) {
        d_w_large_dose.tim_min = (uint16_t)atoi(tim_min->valuestring);
    } else {
        co_printf("tim_min is missing or not a string!\n");
    }

    // 打印解析结果
    co_printf("Parsed double_wave_large_dose information:\n");
    printf("  all_liquid: %.2f\n", d_w_large_dose.all_liquid);
    printf("  proportion: %u\n", d_w_large_dose.proportion);
    printf("  tim_hh: %u\n", d_w_large_dose.tim_hh);
    printf("  tim_min: %u\n", d_w_large_dose.tim_min);

    // 清理 JSON 内存
    cJSON_Delete(root);
}

















