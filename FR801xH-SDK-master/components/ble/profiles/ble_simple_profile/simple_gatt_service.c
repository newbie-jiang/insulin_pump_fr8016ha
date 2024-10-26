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
#include <stdio.h>
#include <string.h>
#include "co_printf.h"
#include "gap_api.h"
#include "gatt_api.h"
#include "gatt_sig_uuid.h"

#include "simple_gatt_service.h"


/*
 * MACROS (�궨��)
 */

/*
 * CONSTANTS (��������)
 */


// Simple GATT Profile Service UUID: 0xFFF0
const uint8_t sp_svc_uuid[] = UUID16_ARR(SP_SVC_UUID);

/******************************* Characteristic 1 defination *******************************/
// Characteristic 1 UUID: 0xFFF1
// Characteristic 1 data 
#define SP_CHAR1_VALUE_LEN  20
uint8_t sp_char1_value[SP_CHAR1_VALUE_LEN] = {0};
// Characteristic 1 User Description
#define SP_CHAR1_DESC_LEN   17
const uint8_t sp_char1_desc[SP_CHAR1_DESC_LEN] = "Characteristic 1";

/******************************* Characteristic 2 defination *******************************/
// Characteristic 2 UUID: 0xFFF2
// Characteristic 2 data 
#define SP_CHAR2_VALUE_LEN  20
uint8_t sp_char2_value[SP_CHAR2_VALUE_LEN] = {0};
// Characteristic 2 User Description
#define SP_CHAR2_DESC_LEN   17
const uint8_t sp_char2_desc[SP_CHAR2_DESC_LEN] = "Characteristic 2";

/******************************* Characteristic 3 defination *******************************/
// Characteristic 3 UUID: 0xFFF3
// Characteristic 3 data 
#define SP_CHAR3_VALUE_LEN  30
uint8_t sp_char3_value[SP_CHAR3_VALUE_LEN] = {0};
// Characteristic 3 User Description
#define SP_CHAR3_DESC_LEN   17
const uint8_t sp_char3_desc[SP_CHAR3_DESC_LEN] = "Characteristic 3";

/******************************* Characteristic 4 defination *******************************/
// Characteristic 4 UUID: 0xFFF4
// Characteristic 4 data 
#define SP_CHAR4_VALUE_LEN  40
uint8_t sp_char4_value[SP_CHAR4_VALUE_LEN] = {0};
// Characteristic 4 client characteristic configuration
#define SP_CHAR4_CCC_LEN   2
uint8_t sp_char4_ccc[SP_CHAR4_CCC_LEN] = {0};
// Characteristic 4 User Description
#define SP_CHAR4_DESC_LEN   17
const uint8_t sp_char4_desc[SP_CHAR4_DESC_LEN] = "Characteristic 4";

/******************************* Characteristic 5 defination *******************************/
// Characteristic 5 UUID: 0xFFF5
uint8_t sp_char5_uuid[UUID_SIZE_2] =
{ 
  LO_UINT16(SP_CHAR5_UUID), HI_UINT16(SP_CHAR5_UUID)
};
// Characteristic 5 data 
#define SP_CHAR5_VALUE_LEN  50
uint8_t sp_char5_value[SP_CHAR5_VALUE_LEN] = {0};
// Characteristic 5 User Description
#define SP_CHAR5_DESC_LEN   17
const uint8_t sp_char5_desc[SP_CHAR5_DESC_LEN] = "Characteristic 5";

/*
 * TYPEDEFS (���Ͷ���)
 */

/*
 * GLOBAL VARIABLES (ȫ�ֱ���)
 */
uint8_t sp_svc_id = 0;
uint8_t ntf_char1_enable = 0;

/*
 * LOCAL VARIABLES (���ر���)
 */
static gatt_service_t simple_profile_svc;

/*********************************************************************
 * Profile Attributes - Table
 * ÿһ���һ��attribute�Ķ��塣
 * ��һ��attributeΪService �ĵĶ��塣
 * ÿһ������ֵ(characteristic)�Ķ��壬�����ٰ�������attribute�Ķ��壻
 * 1. ����ֵ����(Characteristic Declaration)
 * 2. ����ֵ��ֵ(Characteristic value)
 * 3. ����ֵ������(Characteristic description)
 * �����notification ����indication �Ĺ��ܣ��������ĸ�attribute�Ķ��壬����ǰ�涨���������������һ������ֵ�ͻ�������(client characteristic configuration)��
 *
 */

const gatt_attribute_t simple_profile_att_table[SP_IDX_NB] =
{
    // Simple gatt Service Declaration
    [SP_IDX_SERVICE]                        =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_PRIMARY_SERVICE_UUID) },     /* UUID */
                                                    GATT_PROP_READ,                                             /* Permissions */
                                                    UUID_SIZE_2,                                                /* Max size of the value */     /* Service UUID size in service declaration */
                                                    (uint8_t*)sp_svc_uuid,                                      /* Value of the attribute */    /* Service UUID value in service declaration */
                                                },

        // Characteristic 1 Declaration           
        [SP_IDX_CHAR1_DECLARATION]          =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
                                                    GATT_PROP_READ,                                             /* Permissions */
                                                    0,                                                          /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */
                                                },
        // Characteristic 1 Value                  
        [SP_IDX_CHAR1_VALUE]                =   {
                                                    { UUID_SIZE_16, SP_CHAR1_TX_UUID },                 /* UUID */
                                                    GATT_PROP_READ | GATT_PROP_NOTI,                           /* Permissions */
                                                    SP_CHAR1_VALUE_LEN,                                         /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
                                                },        

        // Characteristic 4 client characteristic configuration
        [SP_IDX_CHAR1_CFG]                  =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID) },     /* UUID */
                                                    GATT_PROP_READ | GATT_PROP_WRITE,                           /* Permissions */
                                                    2,                                           /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
                                                }, 
																								
        // Characteristic 1 User Description
        [SP_IDX_CHAR1_USER_DESCRIPTION]     =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },      /* UUID */
                                                    GATT_PROP_READ,                                             /* Permissions */
                                                    SP_CHAR1_DESC_LEN,                                          /* Max size of the value */
                                                    (uint8_t *)sp_char1_desc,                                   /* Value of the attribute */
                                                },


        // Characteristic 2 Declaration
        [SP_IDX_CHAR2_DECLARATION]          =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
                                                    GATT_PROP_READ,                                             /* Permissions */
                                                    0,                                                          /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */
                                                },
        // Characteristic 2 Value   
        [SP_IDX_CHAR2_VALUE]                =   {
                                                    { UUID_SIZE_16, SP_CHAR2_RX_UUID },                 /* UUID */
                                                    GATT_PROP_READ | GATT_PROP_WRITE,                                             /* Permissions */
                                                    SP_CHAR2_VALUE_LEN,                                         /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */	/* Can assign a buffer here, or can be assigned in the application by user */
                                                },   
        // Characteristic 2 User Description
        [SP_IDX_CHAR2_USER_DESCRIPTION]     =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },       /* UUID */
                                                    GATT_PROP_READ,                                             /* Permissions */
                                                    SP_CHAR2_DESC_LEN,                                          /* Max size of the value */
                                                    (uint8_t *)sp_char2_desc,                                   /* Value of the attribute */
                                                },


        // Characteristic 3 Declaration
        [SP_IDX_CHAR3_DECLARATION]          =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
                                                    GATT_PROP_READ,                                             /* Permissions */
                                                    0,                                                          /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */
                                                },
        // Characteristic 3 Value
        [SP_IDX_CHAR3_VALUE]                =   {
                                                    { UUID_SIZE_2, UUID16_ARR(SP_CHAR3_UUID) },                 /* UUID */
                                                    GATT_PROP_WRITE,                                            /* Permissions */
                                                    SP_CHAR3_VALUE_LEN,                                         /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
                                                },
        // Characteristic 3 User Description
        [SP_IDX_CHAR3_USER_DESCRIPTION]     =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },      /* UUID */
                                                    GATT_PROP_READ,                                             /* Permissions */
                                                    SP_CHAR3_DESC_LEN,                                          /* Max size of the value */
                                                    (uint8_t *)sp_char3_desc,                                   /* Value of the attribute */
                                                },


        // Characteristic 4 Declaration
        [SP_IDX_CHAR4_DECLARATION]          =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
                                                    GATT_PROP_READ,                                             /* Permissions */
                                                    0,                                                          /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */
                                                },
        // Characteristic 4 Value
        [SP_IDX_CHAR4_VALUE]                =   {
                                                    { UUID_SIZE_2, UUID16_ARR(SP_CHAR4_UUID) },                 /* UUID */
                                                    GATT_PROP_WRITE | GATT_PROP_NOTI,                           /* Permissions */
                                                    SP_CHAR4_VALUE_LEN,                                         /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
                                                },  
        // Characteristic 4 client characteristic configuration
        [SP_IDX_CHAR4_CFG]                  =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CLIENT_CHAR_CFG_UUID) },     /* UUID */
                                                    GATT_PROP_READ | GATT_PROP_WRITE,                           /* Permissions */
                                                    SP_CHAR4_CCC_LEN,                                           /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
                                                }, 
        // Characteristic 4 User Description
        [SP_IDX_CHAR4_USER_DESCRIPTION]     =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },      /* UUID */
                                                    GATT_PROP_READ,                                             /* Permissions */
                                                    SP_CHAR4_DESC_LEN,                                          /* Max size of the value */
                                                    (uint8_t *)sp_char4_desc,                                   /* Value of the attribute */
                                                },


        // Characteristic 5 Declaration 
        [SP_IDX_CHAR5_DECLARATION]          =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHARACTER_UUID) },           /* UUID */
                                                    GATT_PROP_READ,                                             /* Permissions */
                                                    0,                                                          /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */
                                                },
        // Characteristic 5 Value
        [SP_IDX_CHAR5_VALUE]                =   {
                                                    { UUID_SIZE_2, UUID16_ARR(SP_CHAR5_UUID) },                 /* UUID */
                                                    GATT_PROP_WRITE | GATT_PROP_READ,             /* Permissions */
                                                    SP_CHAR5_VALUE_LEN,                                         /* Max size of the value */
                                                    NULL,                                                       /* Value of the attribute */    /* Can assign a buffer here, or can be assigned in the application by user */
                                                },
        // Characteristic 5 User Description
        [SP_IDX_CHAR5_USER_DESCRIPTION]     =   {
                                                    { UUID_SIZE_2, UUID16_ARR(GATT_CHAR_USER_DESC_UUID) },      /* UUID */
                                                    GATT_PROP_READ,                                             /* Permissions */
                                                    SP_CHAR5_DESC_LEN,                                          /* Max size of the value */
                                                    (uint8_t *)sp_char5_desc,                                   /* Value of the attribute */
                                                },
};

static void show_reg(uint8_t *data,uint32_t len,uint8_t dbg_on)
{
    uint32_t i=0;
    if(len == 0 || (dbg_on==0)) return;
    for(; i<len; i++)
    {
        co_printf("0x%02X,",data[i]);
    }
    co_printf("\r\n");
}
void ntf_data(uint8_t con_idx,uint8_t att_idx,uint8_t *data,uint16_t len)
{
		gatt_ntf_t ntf_att;
		ntf_att.att_idx = att_idx;
		ntf_att.conidx = con_idx;
		ntf_att.svc_id = sp_svc_id;
		ntf_att.data_len = len;
		ntf_att.p_data = data;
		gatt_notification(ntf_att);
}




extern void motor_run_fun1(void);
extern void motor_run_fun2(void);
extern void motor_run_fun3(void);



extern void motor_stop_fun(void);




void bluetooth_process(uint8_t *data,uint32_t len,uint8_t dbg_on)
{
	 char start_speed5_buffer[] = "START5END";
     char start_speed50_buffer[] = "START50END";
     char start_speed500_buffer[] = "START500END";
     char stop_buffer[] =  "STOP";
	 char home_buffer[] =  "ZK-HOME";	
	
	 char  speed_desc[3]  =  {0};
	 char  volume_desc[3] =  {0};
	 char  which_mode[3]  =  {0};
	 
	 static int int_which_mode;
	
	 uint32_t i=0;
	 uint8_t buffer[50]={0};
	 	 
     if(len == 0 || (dbg_on==0)) return;
     for(; i<len; i++)
    {	   
		buffer[i]=data[i];
//        co_printf("0x%02X,",data[i]); //????????????????????
    }
		
	if (strncmp((char*)start_speed5_buffer, (char*)buffer, strlen((char*)start_speed5_buffer)) == 0) {

		co_printf("motor run fun1\r\n");    
        motor_run_fun1();	

    } else if (strncmp((char*)start_speed50_buffer, (char*)buffer, strlen((char*)start_speed50_buffer)) == 0) {

        co_printf("motor run fun2\r\n");	
        motor_run_fun2();	
       
     
    }else if (strncmp((char*)start_speed500_buffer, (char*)buffer, strlen((char*)start_speed500_buffer)) == 0) {

        motor_run_fun3();		
        co_printf("motor run fun3\r\n");

    }else if (strncmp((char*)stop_buffer, (char*)buffer, strlen((char*)stop_buffer)) == 0){
		
        co_printf("motor stop fun\r\n");
		motor_stop_fun();
		
	}else
	{
	  co_printf("not instruct\r\n");	     
	}
	
   }





/*********************************************************************
 * @fn      sp_gatt_msg_handler
 *
 * @brief   Simple Profile callback funtion for GATT messages. GATT read/write
 *			operations are handeled here.
 *
 * @param   p_msg       - GATT messages from GATT layer.
 *
 * @return  uint16_t    - Length of handled message.
 */
static uint16_t sp_gatt_msg_handler(gatt_msg_t *p_msg)
{
    switch(p_msg->msg_evt)
    {
        case GATTC_MSG_READ_REQ:
				{
/*********************************************************************
 * @brief   Simple Profile user application handles read request in this callback.
 *			Ӧ�ò�������ص��������洦����������
 *
 * @param   p_msg->param.msg.p_msg_data  - the pointer to read buffer. NOTE: It's just a pointer from lower layer, please create the buffer in application layer.
 *					  ָ�����������ָ�롣 ��ע����ֻ��һ��ָ�룬����Ӧ�ó����з��仺����. Ϊ�������, ���Ϊָ���ָ��.
 *          p_msg->param.msg.msg_len     - the pointer to the length of read buffer. Application to assign it.
 *                    ���������ĳ��ȣ��û�Ӧ�ó���ȥ������ֵ.
 *          p_msg->att_idx - index of the attribute value in it's attribute table.
 *					  Attribute��ƫ����.
 *
 * @return  ������ĳ���.
 */					
					
					if(p_msg->att_idx == SP_IDX_CHAR1_VALUE)
					{
						memcpy(p_msg->param.msg.p_msg_data, "CHAR1_VALUE", strlen("CHAR1_VALUE"));
						return strlen("CHAR1_VALUE");
					}
					else if(p_msg->att_idx == SP_IDX_CHAR2_VALUE)
					{
						memcpy(p_msg->param.msg.p_msg_data, "CHAR2_VALUE", strlen("CHAR2_VALUE"));
					
						return strlen("CHAR2_VALUE");
						
	
					}
					else if(p_msg->att_idx == SP_IDX_CHAR4_CFG)
					{
						memcpy(p_msg->param.msg.p_msg_data, sp_char4_ccc, 2);
						return 2;
					}					
					else if(p_msg->att_idx == SP_IDX_CHAR5_VALUE)
					{
						memcpy(p_msg->param.msg.p_msg_data, "CHAR5_VALUE", strlen("CHAR5_VALUE"));
						return strlen("CHAR5_VALUE");
					}
				}
            break;
        
        case GATTC_MSG_WRITE_REQ:
				{
/*********************************************************************
 * @brief   Simple Profile user application handles write request in this callback.
 *			Ӧ�ò�������ص��������洦��д������
 *
 * @param   p_msg->param.msg.p_msg_data   - the buffer for write
 *			              д����������.
 *					  
 *          p_msg->param.msg.msg_len      - the length of write buffer.
 *                        д�������ĳ���.
 *          att_idx     - index of the attribute value in it's attribute table.
 *					      Attribute��ƫ����.
 *
 * @return  д����ĳ���.
 */				
					if (p_msg->att_idx == SP_IDX_CHAR1_VALUE)
					{
						co_printf("char1_recv:");
						show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
					}
					else if (p_msg->att_idx == SP_IDX_CHAR2_VALUE)
					{
						co_printf("char2_recv:");
						
						
						bluetooth_process(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);			
						
						show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
						
						
					}
					else if (p_msg->att_idx == SP_IDX_CHAR3_VALUE)
					{
						co_printf("char3_recv:");
						show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
					}
					else if (p_msg->att_idx == SP_IDX_CHAR5_VALUE)
					{
						co_printf("char5_recv:");
						show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
					}
					else if (p_msg->att_idx == SP_IDX_CHAR1_CFG)
					{
						co_printf("char1_ntf_enable:");
						show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
						if(p_msg->param.msg.p_msg_data[0] & 0x1)
							ntf_char1_enable = 1;
						if(ntf_char1_enable)
							ntf_data(p_msg->conn_idx,SP_IDX_CHAR1_VALUE,(uint8_t *)"char1_ntf_data",strlen("char1_ntf_data"));
					}
					else if (p_msg->att_idx == SP_IDX_CHAR4_CFG)
					{
						co_printf("char4_ntf_enable:");
						show_reg(p_msg->param.msg.p_msg_data,p_msg->param.msg.msg_len,1);
						memcpy(sp_char4_ccc,p_msg->param.msg.p_msg_data,2);
					}
				}
            break;
        case GATTC_MSG_LINK_CREATE:
						co_printf("link_created\r\n");
            break;
        case GATTC_MSG_LINK_LOST:
						co_printf("link_lost\r\n");
						ntf_char1_enable = 0;
            break;    
        default:
            break;
    }
    return p_msg->param.msg.msg_len;
}

/*********************************************************************
 * @fn      sp_gatt_add_service
 *
 * @brief   Simple Profile add GATT service function.
 *			����GATT service��ATT�����ݿ����档
 *
 * @param   None. 
 *        
 *
 * @return  None.
 */
void sp_gatt_add_service(void)
{
	simple_profile_svc.p_att_tb = simple_profile_att_table;
	simple_profile_svc.att_nb = SP_IDX_NB;
	simple_profile_svc.gatt_msg_handler = sp_gatt_msg_handler;
	
	sp_svc_id = gatt_add_service(&simple_profile_svc);
}





