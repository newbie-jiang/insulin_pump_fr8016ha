/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

/*
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h> // for rand()

#include "driver_iic.h"
#include "driver_plf.h"
#include "driver_system.h"
#include "sys_utils.h"

/*
 * LOCAL VARIABLES 
 */
/// how many 10us will be taken on IIC bus for one byte
static uint16_t iic_byte_period[IIC_CHANNEL_MAX] = {10, 10};
/*********************************************************************
 * @fn      iic_int_enable
 *
 * @brief   iic intrrupt enable
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *                      - fe_INT_indx intrrupt indx
 *
 * @return  None.
 */
void iic_int_enable(enum iic_channel_t channel,enum_iic_int_indx_t fe_INT_indx)
{
    if(channel == IIC_CHANNEL_0)
    {
		*(volatile uint32_t *)(0x50030008) |= fe_INT_indx;
    }
    else
    {
		*(volatile uint32_t *)(0x50038008) |= fe_INT_indx;
    }
	
}

/*********************************************************************
 * @fn      iic_int_disable
 *
 * @brief   iic intrrupt disable
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *                      - fe_INT_indx intrrupt indx
 *
 * @return  None.
 */
void iic_int_disable(enum iic_channel_t channel,enum_iic_int_indx_t fe_INT_indx)
{
    if(channel == IIC_CHANNEL_0)
    {
		*(volatile uint32_t *)(0x50030008) &= (~fe_INT_indx);
    }
    else
    {
		*(volatile uint32_t *)(0x50038008) &= (~fe_INT_indx);
    }
	
}
/*********************************************************************
 * @fn      iic_get_status
 *
 * @brief   get iic status.
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *
 * @return  iic status.
 */
__attribute__((section("ram_code")))  uint32_t iic_get_status(enum iic_channel_t channel)
{
	uint32_t status;
    if(channel == IIC_CHANNEL_0)
    {
		status = *(volatile uint32_t *)(0x50030004);
		
    }
    else
    {
		status = *(volatile uint32_t *)(0x50038004);
    }
	return status;	
}
/*********************************************************************
 * @fn      iic_write_byte
 *
 * @brief   write one byte to slave.
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *          slave_addr  - slave address
 *          reg_addr    - which register to be writen
 *          data        - data to be writen
 *
 * @return  None.
 */
uint8_t iic_write_byte(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{
    volatile struct iic_reg_t *iic_reg;

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
    co_delay_10us(iic_byte_period[channel]);
    if(iic_reg->status.no_ack == 1)
    {
        return false;
    }

    iic_reg->data = reg_addr & 0xff;
    iic_reg->data = data | IIC_TRAN_STOP;

    while(iic_reg->status.bus_atv == 1);

    return true;
}

/*********************************************************************
 * @fn      iic_write_bytes
 *
 * @brief   write multi-bytes to slave.
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *          slave_addr  - slave address
 *          reg_addr    - which register to be writen
 *          buffer      - pointer to data buffer
 *          length      - how many bytes to be written
 *
 * @return  None.
 */
uint8_t iic_write_bytes(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, uint8_t *buffer, uint16_t length)
{
    volatile struct iic_reg_t *iic_reg;

    if(length == 0)
    {
        return true;
    }
    length--;

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
    co_delay_10us(iic_byte_period[channel]);
    if(iic_reg->status.no_ack == 1)
    {
        return false;
    }

    iic_reg->data = reg_addr & 0xff;

    while(length)
    {
        while(iic_reg->status.trans_ful == 1);
        iic_reg->data = *buffer++;
        length--;
    }

    while(iic_reg->status.trans_ful == 1);
    iic_reg->data = *buffer | IIC_TRAN_STOP;

    while(iic_reg->status.bus_atv == 1);

    return true;
}
/*********************************************************************
 * @fn      iic_write_bytes_no_regaddr
 *
 * @brief   write multi-bytes to slave.
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *          slave_addr  - slave address
 *          buffer      - pointer to data buffer
 *          length      - how many bytes to be written
 *
 * @return  None.
 */
uint8_t iic_write_bytes_no_regaddr(enum iic_channel_t channel, uint8_t slave_addr, uint8_t *buffer, uint16_t length)
{
    volatile struct iic_reg_t *iic_reg;
    if(length == 0)
    {
        return true;
    }
    length--;

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
    co_delay_10us(iic_byte_period[channel]);
    if(iic_reg->status.no_ack == 1)
    {
        return false;
    }
	
    while(length)
    {
        while(iic_reg->status.trans_ful == 1);
        iic_reg->data = *buffer++;
        length--;
    }

    while(iic_reg->status.trans_ful == 1);
    iic_reg->data = *buffer | IIC_TRAN_STOP;

    while(iic_reg->status.bus_atv == 1);

    return true;
}

/*********************************************************************
 * @fn      iic_read_byte
 *
 * @brief   read one byte frome slave.
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *          slave_addr  - slave address
 *          reg_addr    - which register to be written
 *          buffer      - store data to buffer
 *
 * @return  None.
 */
uint8_t iic_read_byte(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, uint8_t *buffer)
{
    volatile struct iic_reg_t *iic_reg;

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
    co_delay_10us(iic_byte_period[channel]);
    if(iic_reg->status.no_ack == 1)
    {
        return false;
    }

    iic_reg->data = reg_addr & 0xff;
    iic_reg->data = slave_addr | 0x01 | IIC_TRAN_START;
    iic_reg->data = IIC_TRAN_STOP;

    while(iic_reg->status.bus_atv == 1);

    *buffer = iic_reg->data&0xff;

    return true;
}

/*********************************************************************
 * @fn      iic_read_bytes
 *
 * @brief   read multi-bytes frome slave.
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *          slave_addr  - slave address
 *          reg_addr    - which register to be written
 *          buffer      - buffer pointer to be written
 *          length      - how many bytes to be read
 *
 * @return  None.
 */
uint8_t iic_read_bytes(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, uint8_t *buffer, uint16_t length)
{
    volatile struct iic_reg_t *iic_reg;

    if(length == 0)
    {
        return true;
    }

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
    co_delay_10us(iic_byte_period[channel]);
    if(iic_reg->status.no_ack == 1)
    {
        return false;
    }

    iic_reg->data = reg_addr & 0xff;
    iic_reg->data = slave_addr | 0x01 | IIC_TRAN_START;
	
    while(length > 1)
    {
        iic_reg->data = 0x00;
        while(iic_reg->status.rec_emp != 1)
        {
            *buffer++ = iic_reg->data;
            length--;
        }
        while(iic_reg->status.trans_emp != 1);
    }

    iic_reg->data = IIC_TRAN_STOP;

    while(iic_reg->status.bus_atv == 1);

    while(length)
    {
        *buffer++ = iic_reg->data;
        length--;
    }

    *buffer = iic_reg->data&0xff;

    return true;
}

uint8_t iic_read_bytes_no_regaddr(enum iic_channel_t channel, uint8_t slave_addr, uint8_t *buffer, uint16_t length)
{
    volatile struct iic_reg_t *iic_reg;

    if(length == 0)
    {
        return true;
    }

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
    co_delay_10us(iic_byte_period[channel]);
    if(iic_reg->status.no_ack == 1)
    {
        return false;
    }
    iic_reg->data = slave_addr | 0x01 | IIC_TRAN_START;
	
    while(length > 1)
    {
        iic_reg->data = 0x00;
        while(iic_reg->status.rec_emp != 1)
        {
            *buffer++ = iic_reg->data;
            length--;
        }
        while(iic_reg->status.trans_emp != 1);
    }

    iic_reg->data = IIC_TRAN_STOP;

    while(iic_reg->status.bus_atv == 1);

    while(length)
    {
        *buffer++ = iic_reg->data;
        length--;
    }

    *buffer = iic_reg->data&0xff;

    return true;
}
uint8_t iic_write_bytes_imp(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, 
									uint8_t *buffer, uint16_t length)
{
    volatile struct iic_reg_t *iic_reg;

    if(length == 0)
    {
        return true;
    }
    length--;

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

    iic_reg->data = reg_addr & 0xff;

    while(length)
    {
        while(iic_reg->status.trans_ful == 1);
        iic_reg->data = *buffer++;
        length--;
    }

    while(iic_reg->status.trans_ful == 1);
    iic_reg->data = *buffer | IIC_TRAN_STOP;

	co_delay_100us(1);
	
	return true;
	
    //while(iic_reg->status.bus_atv == 1);

    //return true;
}

/*********************************************************************
 * @fn      iic_init
 *
 * @brief   Initialize iic instance.
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *          speed       - SCL speed when working as master, N * 1000
 *          slave_addr  - local address when working as slave
 *
 * @return  None.
 */
void iic_init(enum iic_channel_t channel, uint16_t speed, uint16_t slave_addr)
{
    volatile struct iic_reg_t *iic_reg;

    if(channel == IIC_CHANNEL_0)
    {
        iic_reg = IIC0_REG_BASE;
    }
    else
    {
        iic_reg = IIC1_REG_BASE;
    }

    iic_reg->clkdiv.clk_div = (system_get_pclk_config()*1000/speed-10)/2;
    iic_reg->control.soft_reset = 1;
    iic_reg->control.seven_bit = 1;
    iic_reg->address.slv_addr = slave_addr;

    iic_byte_period[channel] = 1000/speed + 1;
}

__attribute__((weak)) __attribute__((section("ram_code"))) void iic0_isr_ram(void)
{
   
}

__attribute__((weak)) __attribute__((section("ram_code"))) void iic1_isr_ram(void)	
{
	
}

