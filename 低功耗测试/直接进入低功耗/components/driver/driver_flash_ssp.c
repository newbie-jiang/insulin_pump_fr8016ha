
/**
 ****************************************************************************************
 *
 * @file flash.c
 *
 * @brief Flash memory driver.
 *
 * Copyright (C) Freqchip 2019-2021
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup FLASH
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>        // standard integer definition
#include <string.h>        // string manipulation

#include "driver_flash_ssp.h"     // flash definition
#include "driver_ssp.h"
#include "driver_plf.h"
#include "driver_gpio.h"

/*
 * DEFINES
 ****************************************************************************************
 */
#define FLASH_SSP_CS_LOW           gpio_porta_write(gpio_porta_read() & ~(1<<5))   // (*(volatile uint32_t *)(0x50064004)) &= ~(1<<5) //
#define FLASH_SSP_CS_HIGH          gpio_porta_write(gpio_porta_read() | (1<<5))    // (*(volatile uint32_t *)(0x50064004)) |=  (1<<5) // 
/*
 * LOCAL FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */
void ssp_put_byte(uint32_t ssp_addr,const uint16_t c);
void ssp_reconfigure(uint32_t ssp_addr,uint32_t bit_rate, uint8_t clk_rate, uint8_t data_size);
void ssp_put_byte(uint32_t ssp_addr,const uint16_t c);
void ssp_put_data(uint32_t ssp_addr,const uint8_t *d, uint32_t size);
void ssp_get_data(uint32_t ssp_addr,uint8_t* buf, uint32_t size);
void ssp_enable(uint32_t ssp_addr);
void ssp_disable(uint32_t ssp_addr);
void ssp_wait_busy_bit(uint32_t ssp_addr);
void ssp_clear_rx_fifo(uint32_t ssp_addr);
void ssp_write_then_read(uint32_t ssp_addr,uint8_t* tx_buffer, uint32_t n_tx, uint8_t* rx_buffer, uint32_t n_rx);

static uint8_t ssp_flash_read_status_reg(uint32_t ssp_addr)
{
    uint8_t buffer[2] = {0x00, 0x00};

    GLOBAL_INT_DISABLE();
    FLASH_SSP_CS_LOW;
    ssp_put_byte(ssp_addr,FLASH_READ_STATUS_REG_OPCODE);
    ssp_put_byte(ssp_addr,0xff);
    ssp_enable(ssp_addr);
    ssp_get_data(ssp_addr,buffer, 2);
    ssp_wait_busy_bit(ssp_addr);
    ssp_disable(ssp_addr);
    FLASH_SSP_CS_HIGH;
    GLOBAL_INT_RESTORE();
    
    return buffer[1];
}
static uint8_t ssp_flash_read_status_reg2(uint32_t ssp_addr)
{
    uint8_t buffer[2] = {0x00, 0x00};

    GLOBAL_INT_DISABLE();
    FLASH_SSP_CS_LOW;
    ssp_put_byte(ssp_addr,FLASH_READ_STATUS_REG2_OPCODE);
    ssp_put_byte(ssp_addr,0xff);
    ssp_enable(ssp_addr);
    ssp_get_data(ssp_addr,buffer, 2);
    ssp_wait_busy_bit(ssp_addr);
    ssp_disable(ssp_addr);
    FLASH_SSP_CS_HIGH;
    GLOBAL_INT_RESTORE();
    
    return buffer[1];
}

static void ssp_flash_write_status_reg(uint32_t ssp_addr,uint8_t status)
{
    GLOBAL_INT_DISABLE();
    FLASH_SSP_CS_LOW;
    ssp_put_byte(ssp_addr,FLASH_WRITE_STATUS_REG_OPCODE);
    ssp_put_byte(ssp_addr,status);
    ssp_enable(ssp_addr);
    ssp_wait_busy_bit(ssp_addr);
    ssp_disable(ssp_addr);
    FLASH_SSP_CS_HIGH;
    GLOBAL_INT_RESTORE();
}

static void ssp_flash_write_status_reg2(uint32_t ssp_addr,uint8_t status1,uint8_t status2)
{
    GLOBAL_INT_DISABLE();
    FLASH_SSP_CS_LOW;
    ssp_put_byte(ssp_addr,FLASH_WRITE_STATUS_REG_OPCODE);
    ssp_put_byte(ssp_addr,status1);
	ssp_put_byte(ssp_addr,status2);
    ssp_enable(ssp_addr);
    ssp_wait_busy_bit(ssp_addr);
    ssp_disable(ssp_addr);
    FLASH_SSP_CS_HIGH;
    GLOBAL_INT_RESTORE();
}

static void ssp_flash_write_enable(uint32_t ssp_addr)
{
    GLOBAL_INT_DISABLE();
    FLASH_SSP_CS_LOW;
    ssp_put_byte(ssp_addr,FLASH_WRITE_ENABLE_OPCODE);
    ssp_enable(ssp_addr);
    ssp_wait_busy_bit(ssp_addr);
    ssp_disable(ssp_addr);
    FLASH_SSP_CS_HIGH;
    GLOBAL_INT_RESTORE();
}

static void ssp_flash_write_disable(uint32_t ssp_addr)
{
    GLOBAL_INT_DISABLE();
    FLASH_SSP_CS_LOW;
    ssp_put_byte(ssp_addr,FLASH_WRITE_DISABLE_OPCODE);
    ssp_enable(ssp_addr);
    ssp_wait_busy_bit(ssp_addr);
    ssp_disable(ssp_addr);
    FLASH_SSP_CS_HIGH;
    GLOBAL_INT_RESTORE();
}

static void flash_poll_busy_bit(uint32_t ssp_addr)
{
    volatile uint16_t i;

    while(ssp_flash_read_status_reg(ssp_addr)&0x03)
    {
        //delay
        for(i=0; i<1000; i++);
    }
}

/*
 * EXPORTED FUNCTIONS DEFINITIONS
 ****************************************************************************************
 */

uint32_t ssp_flash_init(uint32_t ssp_addr)
{
    uint32_t id;

    ssp_flash_identify(ssp_addr,(uint8_t *)&id);

    return (id & 0xFFFFFF);
}

uint8_t ssp_flash_identify(uint32_t ssp_addr, uint8_t* id)
{
    GLOBAL_INT_DISABLE();
    FLASH_SSP_CS_LOW;
    ssp_put_byte(ssp_addr,FLASH_READ_IDENTIFICATION);//FLASH_READ_IDENTIFICATION);//;
    ssp_put_byte(ssp_addr,0x00);
    ssp_put_byte(ssp_addr,0x00);
    ssp_put_byte(ssp_addr,0x00);
    ssp_enable(ssp_addr);
    ssp_get_data(ssp_addr,id, 4);
    //ssp_get_data(ssp_addr,&id[1], 3);
    ssp_wait_busy_bit(ssp_addr);
    ssp_disable(ssp_addr);
    FLASH_SSP_CS_HIGH;
    GLOBAL_INT_RESTORE();

    return 0;
}

uint8_t ssp_flash_erase(uint32_t offset, uint32_t size)
{
    uint32_t addr;
    uint32_t ssp_addr = SSP0_BASE;

    addr = offset & 0xFFFFF000;

    ssp_flash_write_enable(ssp_addr);

    GLOBAL_INT_DISABLE();
    FLASH_SSP_CS_LOW;
    ssp_put_byte(ssp_addr,FLASH_SECTORE_ERASE_OPCODE);
    ssp_put_byte(ssp_addr,addr >> 16);
    ssp_put_byte(ssp_addr,addr >> 8);
    ssp_put_byte(ssp_addr,addr);
    ssp_enable(ssp_addr);
    ssp_wait_busy_bit(ssp_addr);
    ssp_disable(ssp_addr);
    ssp_clear_rx_fifo(ssp_addr);
    FLASH_SSP_CS_HIGH;
    GLOBAL_INT_RESTORE();
    
    flash_poll_busy_bit(ssp_addr);

    return 0;
}

uint8_t ssp_flash_chip_erase(void)
{
    uint32_t ssp_addr = SSP0_BASE;
    ssp_flash_write_enable(ssp_addr);

    GLOBAL_INT_DISABLE();
    FLASH_SSP_CS_LOW;
    ssp_put_byte(ssp_addr,FLASH_CHIP_ERASE_OPCODE);
    ssp_enable(ssp_addr);
    ssp_wait_busy_bit(ssp_addr);
    ssp_disable(ssp_addr);
    ssp_clear_rx_fifo(ssp_addr);
    FLASH_SSP_CS_HIGH;
    GLOBAL_INT_RESTORE();
    
    flash_poll_busy_bit(ssp_addr);

    return 0;
}
uint8_t ssp_flash_write(uint32_t offset, uint32_t length, uint8_t *buffer)
{
    unsigned char page_count;
    uint32_t i;

    page_count = length >> 8;
    if((offset&0x000000ff)|(length&0x000000ff))
    {
        if((256-(offset&0x000000ff))>=(length&0x000000ff))
            page_count++;
        else
            page_count += 2;
    }

    if(length > 0)
    {
        for( i = 0; page_count > 0; page_count--)
        {
            ssp_flash_write_enable(SSP0_BASE);
            
            GLOBAL_INT_DISABLE();
            FLASH_SSP_CS_LOW;
            ssp_put_byte(SSP0_BASE,FLASH_PAGE_PROGRAM_OPCODE);
            ssp_put_byte(SSP0_BASE,offset >> 16);
            ssp_put_byte(SSP0_BASE,offset >> 8);
            ssp_put_byte(SSP0_BASE,offset);
            ssp_enable(SSP0_BASE);

            for(; i < length; )
            {
                ssp_put_byte(SSP0_BASE,buffer[i]);
                offset++;
                i++;
                if(!(offset<<24))
                    break;
            }
            ssp_wait_busy_bit(SSP0_BASE);
            ssp_disable(SSP0_BASE);
            ssp_clear_rx_fifo(SSP0_BASE);
            FLASH_SSP_CS_HIGH;
            GLOBAL_INT_RESTORE();
            
            flash_poll_busy_bit(SSP0_BASE);
        }
    }

    return 0;
}

#define FLASH_READ_SINGLE_PACKET_LEN       (SSP_FIFO_SIZE-5)
#if 1
uint8_t ssp_flash_read(uint32_t offset, uint32_t length, uint8_t *buffer)
{
    uint8_t write_buf[5];
    uint32_t read_times;
    uint8_t last_bytes;
    uint32_t i;
	uint32_t ssp_addr = SSP0_BASE;

    read_times = length/FLASH_READ_SINGLE_PACKET_LEN;
    last_bytes = length%FLASH_READ_SINGLE_PACKET_LEN;

    //ssp_reconfigure(ssp_addr,8000000, FLASH_SSP_CLK, FLASH_DATA_SIZE);
    ssp_clear_rx_fifo(ssp_addr);

    for(i=0; i<read_times; i++)
    {
        GLOBAL_INT_DISABLE();
        FLASH_SSP_CS_LOW;
        write_buf[0] = FLASH_FAST_READ_OPCODE;
        write_buf[1] = offset >> 16;
        write_buf[2] = offset >> 8;
        write_buf[3] = offset ;
        write_buf[4] = 0xFF;
        ssp_write_then_read(ssp_addr,write_buf, 5, buffer+(i*FLASH_READ_SINGLE_PACKET_LEN), FLASH_READ_SINGLE_PACKET_LEN);
        FLASH_SSP_CS_HIGH;
        GLOBAL_INT_RESTORE();
        offset += FLASH_READ_SINGLE_PACKET_LEN;
    }
    if(last_bytes != 0)
    {
        GLOBAL_INT_DISABLE();
        FLASH_SSP_CS_LOW;
        write_buf[0] = FLASH_FAST_READ_OPCODE;
        write_buf[1] = offset >> 16;
        write_buf[2] = offset >> 8;
        write_buf[3] = offset ;
        write_buf[4] = 0xFF;
        ssp_write_then_read(ssp_addr,write_buf, 5, buffer+(read_times*FLASH_READ_SINGLE_PACKET_LEN), last_bytes);
        FLASH_SSP_CS_HIGH;
        GLOBAL_INT_RESTORE();
    }
	ssp_enable(ssp_addr);
    return 0;
}

#else
uint8_t ssp_flash_read(uint32_t offset, uint32_t length, uint8_t *buffer)
{
    uint8_t write_buf[5];
    uint32_t read_times;
    uint8_t last_bytes;
    uint32_t i;

    read_times = length/FLASH_READ_SINGLE_PACKET_LEN;
    last_bytes = length%FLASH_READ_SINGLE_PACKET_LEN;

    for(i=0; i<read_times; i++)
    {
        GLOBAL_INT_DISABLE();
        FLASH_SSP_CS_LOW;
        write_buf[0] = FLASH_FAST_READ_OPCODE;
        write_buf[1] = offset >> 16;
        write_buf[2] = offset >> 8;
        write_buf[3] = offset ;
        write_buf[4] = 0xFF;
        ssp_send_then_recv(write_buf, 5, buffer+(i*FLASH_READ_SINGLE_PACKET_LEN), FLASH_READ_SINGLE_PACKET_LEN);
        FLASH_SSP_CS_HIGH;
        GLOBAL_INT_RESTORE();
        offset += FLASH_READ_SINGLE_PACKET_LEN;
    }
    if(last_bytes != 0)
    {
        GLOBAL_INT_DISABLE();
        FLASH_SSP_CS_LOW;
        write_buf[0] = FLASH_FAST_READ_OPCODE;
        write_buf[1] = offset >> 16;
        write_buf[2] = offset >> 8;
        write_buf[3] = offset ;
        write_buf[4] = 0xFF;
        ssp_send_then_recv(write_buf, 5, buffer+(read_times*FLASH_READ_SINGLE_PACKET_LEN), last_bytes);
        FLASH_SSP_CS_HIGH;
        GLOBAL_INT_RESTORE();
    }

    return 0;
}
#endif
/// @} FLASH

