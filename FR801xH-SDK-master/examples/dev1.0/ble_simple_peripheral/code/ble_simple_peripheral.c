/**
 * Copyright (c) 2019, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */
 
/*
 * INCLUDES (����ͷ�ļ�)
 */
#include <stdbool.h>
#include "gap_api.h"
#include "gatt_api.h"
#include "driver_gpio.h"
#include "driver_pmu.h"
#include "button.h"
#include "os_timer.h"
#include "speaker_service.h"
#include "simple_gatt_service.h"
#include "ble_simple_peripheral.h"

#include "sys_utils.h"
#include "lcd.h"
#include "capb18-001.h"
#include "sht3x.h"
#include "decoder.h"
#include "gyro_alg.h"
#include "flash_usage_config.h"
#include "user_task.h"


/*
 * MACROS
 */

/*
 * CONSTANTS 
 */
const unsigned	char * lcd_show_workmode[MODE_MAX] = {"PICTURE_UPDATE","SENSOR_DATA","SPEAKER_FROM_FLASH","CODEC_TEST"};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
// GAP-�㲥��������,�31���ֽ�.��һ������ݿ��Խ�ʡ�㲥ʱ��ϵͳ����.
static uint8_t adv_data[] =
{
  // service UUID, to notify central devices what services are included
  // in this peripheral. ����central������ʲô����, ��������ֻ��һ����Ҫ��.
  0x03,   // length of this data
  GAP_ADVTYPE_16BIT_MORE,      // some of the UUID's, but not all
  0xFF,
  0xFE,
};

// GAP - Scan response data (max size = 31 bytes, though this is
// best kept short to conserve power while advertisting)
// GAP-Scan response����,�31���ֽ�.��һ������ݿ��Խ�ʡ�㲥ʱ��ϵͳ����.
static uint8_t scan_rsp_data[] =
{
  // complete name �豸����
  0x12,   // length of this data
  GAP_ADVTYPE_LOCAL_NAME_COMPLETE,
  'S','i','m','p','l','e',' ','P','e','r','i','p','h','e','r','a','l',

  // Tx power level ���书��
  0x02,   // length of this data
  GAP_ADVTYPE_POWER_LEVEL,
  0,	   // 0dBm
};

/*
 * TYPEDEFS 
 */

/*
 * GLOBAL VARIABLES 
 */

os_timer_t timer_refresh;//
os_timer_t motor_task;// 
os_timer_t beep_task;

uint8_t App_Mode = PICTURE_UPDATE;//

/*
 * LOCAL VARIABLES 
 */
 
/*
 * LOCAL FUNCTIONS
 */
static void sp_start_adv(void);

/*
 * EXTERN FUNCTIONS
 */
uint8_t CAPB18_data_get(float *temperature,float *air_press);
uint8_t demo_CAPB18_APP(void);

/*
 * PUBLIC FUNCTIONS
 */

/** @function group ble peripheral device APIs (ble������ص�API)
 * @{
 */

/*********************************************************************
 * @fn      app_gap_evt_cb
 *
 * @brief   Application layer GAP event callback function. Handles GAP evnets.
 *
 * @param   p_event - GAP events from BLE stack.
 *       
 *
 * @return  None.
 */
void app_gap_evt_cb(gap_event_t *p_event)
{
    switch(p_event->type)
    {
        case GAP_EVT_ADV_END:
        {
            co_printf("adv_end,status:0x%02x\r\n",p_event->param.adv_end.status);
            //gap_start_advertising(0);
        }
        break;
        
        case GAP_EVT_ALL_SVC_ADDED:
        {
            co_printf("All service added\r\n");
            sp_start_adv();
#ifdef USER_MEM_API_ENABLE
            //show_mem_list();
            //show_msg_list();
            //show_ke_malloc();
#endif
        }
        break;

        case GAP_EVT_SLAVE_CONNECT:
        {
            co_printf("slave[%d],connect. link_num:%d\r\n",p_event->param.slave_connect.conidx,gap_get_connect_num());
			gatt_mtu_exchange_req(p_event->param.slave_connect.conidx);
            gap_conn_param_update(p_event->param.slave_connect.conidx, 6, 6, 0, 500);
        }
        break;

        case GAP_EVT_DISCONNECT:
        {
            co_printf("Link[%d] disconnect,reason:0x%02X\r\n",p_event->param.disconnect.conidx
                      ,p_event->param.disconnect.reason);
            sp_start_adv();
#ifdef USER_MEM_API_ENABLE
            show_mem_list();
            //show_msg_list();
            show_ke_malloc();
#endif
        }
        break;

        case GAP_EVT_LINK_PARAM_REJECT:
            co_printf("Link[%d]param reject,status:0x%02x\r\n"
                      ,p_event->param.link_reject.conidx,p_event->param.link_reject.status);
            break;

        case GAP_EVT_LINK_PARAM_UPDATE:
            co_printf("Link[%d]param update,interval:%d,latency:%d,timeout:%d\r\n",p_event->param.link_update.conidx
                      ,p_event->param.link_update.con_interval,p_event->param.link_update.con_latency,p_event->param.link_update.sup_to);
            break;

        case GAP_EVT_PEER_FEATURE:
            co_printf("peer[%d] feats ind\r\n",p_event->param.peer_feature.conidx);
            show_reg((uint8_t *)&(p_event->param.peer_feature.features),8,1);
            break;

        case GAP_EVT_MTU:
            co_printf("mtu update,conidx=%d,mtu=%d\r\n"
                      ,p_event->param.mtu.conidx,p_event->param.mtu.value);
            break;
        
        case GAP_EVT_LINK_RSSI:
            co_printf("link rssi %d\r\n",p_event->param.link_rssi);
            break;
                
        case GAP_SEC_EVT_SLAVE_ENCRYPT:
            co_printf("slave[%d]_encrypted\r\n",p_event->param.slave_encrypt_conidx);
            break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      sp_start_adv
 *
 * @brief   Set advertising data & scan response & advertising parameters and start advertising
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
static void sp_start_adv(void)
{
    // Set advertising parameters
    gap_adv_param_t adv_param;
    adv_param.adv_mode = GAP_ADV_MODE_UNDIRECT;
    adv_param.adv_addr_type = GAP_ADDR_TYPE_PUBLIC;
    adv_param.adv_chnl_map = GAP_ADV_CHAN_ALL;
    adv_param.adv_filt_policy = GAP_ADV_ALLOW_SCAN_ANY_CON_ANY;
    adv_param.adv_intv_min = 300;
    adv_param.adv_intv_max = 300;
        
    gap_set_advertising_param(&adv_param);
    
    // Set advertising data & scan response data
	gap_set_advertising_data(adv_data, sizeof(adv_data));
	gap_set_advertising_rsp_data(scan_rsp_data, sizeof(scan_rsp_data));
    // Start advertising
	co_printf("Start advertising...\r\n");
	gap_start_advertising(0);
}



void pmu_pwm_start(enum system_port_t port, enum system_port_bit_t bit,bool repeat_flag,bool reverse_flag);



uint32_t speed_set_flag = 0;

void motor_run_fun1(void)
{
     speed_set_flag = 1 ;
  os_timer_start(&motor_task,10,1);	
 
}

void motor_run_fun2(void)
{
     speed_set_flag = 2 ;	  
  os_timer_start(&motor_task,10,1);		
 
}

void motor_run_fun3(void)
{
  speed_set_flag = 3 ;	  
  os_timer_start(&motor_task,10,1);	
 	  
}


void pmu_pwm_stop(enum system_port_t port, enum system_port_bit_t bit);

extern void pmu_set_led1_value(uint8_t value);

void motor_stop_fun(void)
{ 
  os_timer_stop(&motor_task);
  gpio_set_pin_value(GPIO_PORT_D,GPIO_BIT_5,0);
}
  


void io_init(void)
{   
    system_set_port_mux(GPIO_PORT_D,GPIO_BIT_5,PORTD5_FUNC_D5);
  
    gpio_set_dir(GPIO_PORT_D, GPIO_BIT_5, GPIO_DIR_OUT);
}
 











void ble_init(void)
{
	    // set local device name
	uint8_t local_name[] = "Simple Peripheral";
	gap_set_dev_name(local_name, sizeof(local_name));

	// Initialize security related settings.
	gap_security_param_t param =
	{
	    .mitm = false,
	    .ble_secure_conn = false,
	    .io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
	    .pair_init_mode = GAP_PAIRING_MODE_WAIT_FOR_REQ,
	    .bond_auth = true,
	    .password = 0,
	};

	gap_security_param_init(&param);

	gap_set_cb_func(app_gap_evt_cb);

	gap_bond_manager_init(BLE_BONDING_INFO_SAVE_ADDR, BLE_REMOTE_SERVICE_SAVE_ADDR, 8, true);
	gap_bond_manager_delete_all();

	mac_addr_t addr;
	gap_address_get(&addr);
	co_printf("Local BDADDR: 0x%2X%2X%2X%2X%2X%2X\r\n", addr.addr[0], addr.addr[1], addr.addr[2], addr.addr[3], addr.addr[4], addr.addr[5]);

	// Adding services to database
    sp_gatt_add_service();
	speaker_gatt_add_service();	
}

//PD5
void motor_io_init(void)
{
  system_set_port_mux(GPIO_PORT_D,GPIO_BIT_5,PORTD5_FUNC_D5);
  
  gpio_set_dir(GPIO_PORT_D, GPIO_BIT_5, GPIO_DIR_OUT);
}

//PA4
void beep_init(void)
{
	
	

}

 

void bsp_init(void)
{
   motor_io_init();
	


}

/*********************************************************************
 * @fn      simple_peripheral_init
 *
 * @brief   Initialize simple peripheral profile, BLE related parameters.
 *
 * @param   None. 
 *       
 *
 * @return  None.
 */
void simple_peripheral_init(void)
{	
	co_printf("BLE Peripheral\r\n");
	ble_init();
	  	
//	pmu_set_pin_pull(GPIO_PORT_D, (1<<GPIO_BIT_6), true);
//	pmu_set_pin_pull(GPIO_PORT_C, (1<<GPIO_BIT_5), true);
//	pmu_port_wakeup_func_set(GPIO_PD6|GPIO_PC5);
//	button_init(GPIO_PD6|GPIO_PC5);

//	demo_LCD_APP();							            
//	demo_CAPB18_APP();						            
//	demo_SHT3x_APP();						           
//	gyro_dev_init();	

//  pmu_set_pin_dir(GPIO_PORT_D,(1<<GPIO_BIT_4),1);
	
//	demo_pmu_pwm();
//	io_init();
	//OS Timer
//	os_timer_init(&timer_refresh,timer_refresh_fun,NULL);
//	os_timer_start(&timer_refresh,1000,1);
			
	bsp_init();
	
	/* motor task */
    os_timer_init(&motor_task,motor_task_fun,NULL);
	// os_timer_start(&motor_task,1000,1); 	
	
	/* beep task */
	os_timer_init(&beep_task,beep_task_fun,NULL);
	os_timer_start(&beep_task,1000,1);


}


