/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdio.h>
#include <string.h>
#include "co_printf.h"
#include "sys_utils.h"
#include "driver_gpio.h"
#include "driver_iic.h"
#include "driver_uart.h"
#include "iic_demo.h"

static uint16_t send_cnt = 0;
static uint16_t send_length = 35;
#if IIC_MASTER_MODE
static uint8_t master_send_data[IIC_MAX_BUFF];
static struct iic_recv_data master_recv_data;
#else
static uint8_t slave_state;
static uint8_t slave_send_data[IIC_MAX_BUFF];
static struct iic_recv_data slave_recv_data;
#endif

#if IIC_MASTER_MODE
void iic_master_init(void)
{
    system_set_port_pull(GPIO_PD6, true);
    system_set_port_pull(GPIO_PD7, true);
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_6, PORTD6_FUNC_I2C1_CLK);
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_7, PORTD7_FUNC_I2C1_DAT);
    iic_init(IIC_CHANNEL_1,100,0);
}

uint8_t iic_dynamic_master_read_bytes(enum iic_channel_t channel, uint8_t slave_addr,struct iic_recv_data *recv_data)
{
    volatile struct iic_reg_t *iic_reg;
	uint8_t tmp;
    if(channel == IIC_CHANNEL_0)
    {
        iic_reg = IIC0_REG_BASE;
    }
    else
    {
        iic_reg = IIC1_REG_BASE;
    }

    iic_reg->data = slave_addr | IIC_TRAN_START;
    while(iic_reg->status.trans_emp != 1);
    co_delay_10us(10);
    if(iic_reg->status.no_ack == 1)
    {
        return false;
    }

    iic_reg->data = slave_addr | 0x01 | IIC_TRAN_START;
		
    while(!recv_data->finish_flag)
    {
        iic_reg->data = 0x00;
        while(iic_reg->status.trans_emp != 1);
		while(iic_reg->status.rec_emp != 1)
        {
			tmp = iic_reg->data;			
			if(!recv_data->start_flag)
			{			
				recv_data->recv_length_buff[recv_data->recv_cnt++] = tmp;
				if(recv_data->recv_cnt == 2)
				{
					recv_data->length = ((uint16_t)recv_data->recv_length_buff[0] & 0xff) |
										(((uint16_t)recv_data->recv_length_buff[1] << 8)& 0xff00);
					recv_data->start_flag = 1;
					recv_data->recv_cnt = 0;
				}
			}
			else
			{
				recv_data->buff[recv_data->recv_cnt++] = tmp;
				if(recv_data->recv_cnt >= (recv_data->length-1))
				{
					recv_data->finish_flag = 1;
				}
			}
			
        }
    }
    iic_reg->data = IIC_TRAN_STOP;
    while(iic_reg->status.bus_atv == 1);
	while(iic_reg->status.rec_emp != 1)
	{
		if(recv_data->recv_cnt < recv_data->length)
		{
			recv_data->buff[recv_data->recv_cnt++] = iic_reg->data;
		}
		else
		{
			tmp = iic_reg->data;
		}
	}
    return true;
}
#else

void iic_slave_init(void)
{
    system_set_port_pull(GPIO_PD6, true);
    system_set_port_pull(GPIO_PD7, true);
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_6, PORTD6_FUNC_I2C1_CLK);
    system_set_port_mux(GPIO_PORT_D, GPIO_BIT_7, PORTD7_FUNC_I2C1_DAT);
    iic_init(IIC_CHANNEL_1,1000,IIC_SLVAE_ADDRESS);
	iic_int_enable(IIC_CHANNEL_1,INT_SLV_DATA_REQ);
	iic_int_enable(IIC_CHANNEL_1,INT_RX_FFNE);
    NVIC_EnableIRQ(IIC1_IRQn);
}

void iic_slave_recv_handle(uint8_t c)
{
	
	if(slave_recv_data.start_flag == 0)
	{
		slave_recv_data.recv_length_buff[slave_recv_data.recv_cnt++] = c;
		if(slave_recv_data.recv_cnt == 2 )
		{
			slave_recv_data.length = ((uint16_t)slave_recv_data.recv_length_buff[0] & 0xff) |
										(((uint16_t)slave_recv_data.recv_length_buff[1] << 8)& 0xff00);
			slave_recv_data.start_flag = 1;
			slave_recv_data.recv_cnt = 0;
		}
	}
	else
	{
		slave_recv_data.buff[slave_recv_data.recv_cnt++] = c;
		if(slave_recv_data.recv_cnt >= slave_recv_data.length)
		{
			slave_recv_data.finish_flag = 1;
		}
	}
}
#endif

void demo_start(void)
{
#if IIC_MASTER_MODE
	//配置一个普通IO口用于判断从机是否准备好数据
	system_set_port_pull(GPIO_PA4, true);
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_4, PORTA4_FUNC_A4);
	gpio_set_dir(GPIO_PORT_A,GPIO_BIT_4,GPIO_DIR_IN);
	
	//iic主机初始化
	iic_master_init();
	
	//设置发送数据
	master_send_data[0] = (uint8_t)(send_length&0xff);
	master_send_data[1] = (uint8_t)((send_length&0xff00) >> 8);
	for(uint8_t i = 0 ; i < send_length; i++)
	{
		master_send_data[i+2] = i;
	}
	show_reg(master_send_data,send_length+2,1);
	master_recv_data.finish_flag = 0;
	master_recv_data.start_flag = 0;
	master_recv_data.recv_cnt = 0;
	
	while(1)
	{
		//主机数据发送
		while(iic_write_bytes_no_regaddr(IIC_CHANNEL_1,IIC_MS_SEND_ADDRESS,master_send_data,send_length+2) == 0);
//		//等待从机准备好数据
		while(gpio_get_pin_value(GPIO_PORT_A,GPIO_BIT_4));
		//接收从机数据
		iic_dynamic_master_read_bytes(IIC_CHANNEL_1,IIC_MS_SEND_ADDRESS,&master_recv_data);
		while(!master_recv_data.finish_flag);
		if(master_recv_data.finish_flag)
		{
			master_recv_data.finish_flag = 0;
			master_recv_data.start_flag = 0;
			master_recv_data.recv_cnt = 0;
			printf("master recv:%d\r\n",master_recv_data.length);
			show_reg(master_recv_data.buff,master_recv_data.length,1);
		}

		co_delay_100us(10000);
	}
#else
	//配置一个普通IO口用于从机需要发送消息时告诉主机接收
    system_set_port_mux(GPIO_PORT_A, GPIO_BIT_4, PORTA4_FUNC_A4);
	gpio_set_dir(GPIO_PORT_A,GPIO_BIT_4,GPIO_DIR_OUT);
	gpio_set_pin_value(GPIO_PORT_A,GPIO_BIT_4,1);
	//iic 从机初始化
	iic_slave_init();
	slave_state = IIC_SLAVE_IDLE;
	send_cnt = 0;
	//设置从机需要发射的数据（length + payload）
	slave_send_data[0] = (uint8_t)(send_length&0xff);
	slave_send_data[1] = (uint8_t)((send_length&0xff00) >> 8);
	slave_recv_data.start_flag = 0;
	slave_recv_data.finish_flag = 0;
	slave_recv_data.recv_cnt = 0;
	for(uint8_t i = 0 ; i < send_length; i++)
	{
		slave_send_data[i+2] = i+2;
	}
	show_reg(slave_send_data,send_length+2,1);
	while(1)
	{
		while(!slave_recv_data.finish_flag)
		{
			if(slave_state == IIC_SLVAE_END)
			{
				send_cnt = 0;
				slave_state = IIC_SLAVE_IDLE;
			}
		}
		if(slave_recv_data.finish_flag)
		{
			slave_recv_data.start_flag = 0;
			slave_recv_data.finish_flag = 0;
			slave_recv_data.recv_cnt = 0;
			iic_int_enable(IIC_CHANNEL_1,INT_SLV_DATA_REQ);
			printf("slave recv:%d\r\n",slave_recv_data.length);
			show_reg(slave_recv_data.buff,slave_recv_data.length,1);
			//告知主机接收数据
			gpio_set_pin_value(GPIO_PORT_A,GPIO_BIT_4,0); 
		}
	}
#endif
}

//i2c 中断处理函数
__attribute__((section("ram_code")))void iic1_isr_ram(void)
{
   volatile struct iic_reg_t *iic_reg = IIC1_REG_BASE;
#if IIC_MASTER_MODE
	
#else
	//主机开启请求从机发送数据
   if(iic_reg->status.data_req_slv)
   {
	   if((send_cnt == 0) && (slave_state == IIC_SLAVE_IDLE))
	   {
			gpio_set_pin_value(GPIO_PORT_A,GPIO_BIT_4,1);
			iic_reg->slavedata.slave_data = slave_send_data[send_cnt++];
			slave_state = IIC_SLVAE_ONGOING;
	   }
	   else
	   {
			if(slave_state == IIC_SLVAE_ONGOING)
			{
				if(send_cnt < (send_length+2))
				{
					iic_reg->slavedata.slave_data = slave_send_data[send_cnt++];
				}
				else
				{
					slave_state = IIC_SLVAE_END;
					iic_reg->slavedata.slave_data = 0;
				}
			}
			else if(slave_state == IIC_SLVAE_END)
			{
				iic_reg->slavedata.slave_data = 0;
			}
	   }
   }
   //从机获取I2C数据
   if(!iic_reg->status.rec_emp)
   {
	   while(!iic_reg->status.rec_emp)
	   {
		   volatile uint8_t data = iic_reg->data;
		   iic_slave_recv_handle(data);
	   }
   }
#endif
}

