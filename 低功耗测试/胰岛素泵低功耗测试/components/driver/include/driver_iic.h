/**
 * Copyright (c) 2019, Freqchip
 *
 * All rights reserved.
 *
 *
 */

#ifndef _DRIVER_IIC_H
#define _DRIVER_IIC_H

#include <stdint.h>           // standard integer functions
#include <stdbool.h>

/*
 * MACROS 
 */
#define IIC_TRAN_START      0x100
#define IIC_TRAN_STOP       0x200

#define IIC0_REG_BASE           ((volatile struct iic_reg_t *)I2C0_BASE)
#define IIC1_REG_BASE           ((volatile struct iic_reg_t *)I2C1_BASE)

#define IIC_STATUS_TRANS_DONE                (1<<0)
#define IIC_STATUS_ARB_FAIL                  (1<<1)
#define IIC_STATUS_NO_ACK                    (1<<2)
#define IIC_STATUS_MS_DATA_REQ               (1<<3)
#define IIC_STATUS_SLV_DATA_REQ              (1<<4)
#define IIC_STATUS_BUS_ACT                   (1<<5)
#define IIC_STATUS_SCL_ST                    (1<<6)
#define IIC_STATUS_SDA_ST                    (1<<7)
#define IIC_STATUS_MS_RX_FFF                 (1<<8)
#define IIC_STATUS_MS_RX_FFE                 (1<<9)
#define IIC_STATUS_MS_TX_FFF                 (1<<10)
#define IIC_STATUS_MS_TX_FFE                 (1<<11)
#define IIC_STATUS_SLV_TX_FFF                (1<<12)
#define IIC_STATUS_SLV_TX_FFE                (1<<13)

/*
 * TYPEDEFS 
 */
struct iic_data_t
{
    uint32_t data:8;
    uint32_t start:1;
    uint32_t stop:1;
    uint32_t unused:22;
};

struct iic_status_t
{
    uint32_t trans_done:1;
    uint32_t arb_fail:1;
    uint32_t no_ack:1;
    uint32_t data_req_mst:1;
    uint32_t data_req_slv:1;
    uint32_t bus_atv:1;
    uint32_t sts_scl:1;
    uint32_t sts_sda:1;
    uint32_t rec_full:1;
    uint32_t rec_emp:1;
    uint32_t trans_ful:1;
    uint32_t trans_emp:1;
    uint32_t slv_trans_ful:1;
    uint32_t slv_trans_emp:1;
    uint32_t unused:18;
};

struct iic_control_t
{
    uint32_t trans_done_ie:1;
    uint32_t arb_fail_ie:1;
    uint32_t no_ack_ie:1;
    uint32_t req_mst_ie:1;
    uint32_t req_slv_ie:1;
    uint32_t rec_full_ie:1;
    uint32_t rec_noemp_ie:1;
    uint32_t trans_noful_ie:1;
    uint32_t soft_reset:1;
    uint32_t seven_bit:1;
    uint32_t slv_noful_ie:1;
    uint32_t unused:21;
};

struct iic_clkdiv_t
{
    uint32_t clk_div:9;
    uint32_t unused:23;
};

struct iic_address_t
{
    uint32_t slv_addr:10;
    uint32_t unused:22;
};

struct iic_rxlevel_t
{
    uint32_t rx_level:3;
    uint32_t unused:29;
};

struct iic_txlevel_t
{
    uint32_t tx_level:3;
    uint32_t unused:29;
};

struct iic_rxbytecnt_t
{
    uint32_t rx_byte_cnt:16;
    uint32_t unused:16;
};

struct iic_txbytecnt_t
{
    uint32_t tx_byte_cnt:16;
    uint32_t unused:16;
};

struct iic_slavedata_t
{
    uint32_t slave_data:8;
    uint32_t unused:24;
};

struct iic_reg_t
{
    uint32_t data;
    struct iic_status_t status;
    struct iic_control_t control;
    struct iic_clkdiv_t clkdiv;
    struct iic_address_t address;
    struct iic_rxlevel_t rxlevel;
    struct iic_txlevel_t txlevel;
    struct iic_rxbytecnt_t rx_bytecnt;
    struct iic_txbytecnt_t tx_bytecnt;
    struct iic_slavedata_t slavedata;
};

enum iic_channel_t
{
    IIC_CHANNEL_0,
    IIC_CHANNEL_1,
    IIC_CHANNEL_MAX,
};


typedef enum
{
	 INT_TRANS_DONE                   = 0x00000001,
	 INT_ARB_FAIL                     = 0x00000002,
	 INT_NO_ACK                       = 0x00000004,
	 INT_MS_DATA_REQ                  = 0x00000008,
	 INT_SLV_DATA_REQ                 = 0x00000010,
	 INT_RX_FFF                       = 0x00000020,
	 INT_RX_FFNE                      = 0x00000040,
	 INT_MS_TX_FFNF                   = 0x00000080,
	 INT_SW_RST                       = 0x00000100,
	 INT_ADD_TYPE_SEL                 = 0x00000200,
	 INT_SLV_TX_FFNF                  = 0x00000400,
}enum_iic_int_indx_t;

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
void iic_int_enable(enum iic_channel_t channel,enum_iic_int_indx_t fe_INT_indx);

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
void iic_int_disable(enum iic_channel_t channel,enum_iic_int_indx_t fe_INT_indx);

/*********************************************************************
 * @fn      iic_get_status
 *
 * @brief   get iic status.
 *
 * @param   channel     - IIC_CHANNEL_0 or IIC_CHANNEL_1.
 *                      
 *
 * @return  iic status.
 */
uint32_t iic_get_status(enum iic_channel_t channel);

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
uint8_t iic_write_byte(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, uint8_t data);

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
uint8_t iic_write_bytes(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, uint8_t *buffer, uint16_t length);

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
 uint8_t iic_write_bytes_no_regaddr(enum iic_channel_t channel, uint8_t slave_addr, uint8_t *buffer, uint16_t length);

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
uint8_t iic_read_byte(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, uint8_t *buffer);

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
uint8_t iic_read_bytes(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, uint8_t *buffer, uint16_t length);
uint8_t iic_read_bytes_no_regaddr(enum iic_channel_t channel, uint8_t slave_addr, uint8_t *buffer, uint16_t length);
uint8_t iic_write_bytes_imp(enum iic_channel_t channel, uint8_t slave_addr, uint8_t reg_addr, 
									uint8_t *buffer, uint16_t length);

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
void iic_init(enum iic_channel_t channel, uint16_t speed, uint16_t slave_addr);

#endif

