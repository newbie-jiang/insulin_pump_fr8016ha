#ifndef IIC_DEMO_H
#define IIC_DEMO_H
#include <stdint.h>

#define IIC_MASTER_MODE               1
#define IIC_MAX_BUFF                  40               
#define IIC_SLVAE_ADDRESS             0x5A
#define IIC_MS_SEND_ADDRESS           (0x5A<<1)

struct iic_recv_data{
	uint8_t  start_flag;
	uint8_t  finish_flag;
	uint16_t length;
	uint16_t recv_cnt;
	uint8_t  recv_length_buff[2];
	uint8_t  buff[IIC_MAX_BUFF];
};

enum {
	IIC_SLAVE_IDLE,
	IIC_SLVAE_ONGOING,
	IIC_SLVAE_END,
};
void demo_start(void);
#endif  //_DEMO_PERIPHERAL_H
