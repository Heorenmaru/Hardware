/*
 *   Author: Heorenmaru
 *   License: CC-BY-SA
 */

#ifndef INC_NRF24L01_H_
#define INC_NRF24L01_H_

#include "handlers.h"

typedef struct {
	SPI_HandleTypeDef* spi;
	Gpio nss;
	Gpio ce;
	Gpio irq;
} nrf24l01Handler;


#define NRF24L01P_PAYLOAD_LENGTH          8     // 1 - 32bytes


/* nRF24L01+ typedefs */
typedef uint8_t count;
typedef uint8_t widths;
typedef uint8_t length;
typedef uint16_t delay;
typedef uint16_t channel;

typedef enum
{
    _250kbps = 2,
    _1Mbps   = 0,
    _2Mbps   = 1
} air_data_rate;

typedef enum
{
    _0dBm  = 3,
    _6dBm  = 2,
    _12dBm = 1,
    _18dBm = 0
} output_power;




uint8_t read_register(nrf24l01Handler *handler, uint8_t reg);
uint8_t write_register(nrf24l01Handler *handler, uint8_t reg, uint8_t value);
uint8_t nrf24l01p_read_rx_fifo(nrf24l01Handler *handler, uint8_t* rx_payload);
uint8_t nrf24l01p_write_tx_fifo(nrf24l01Handler *handler, uint8_t* tx_payload);
/* nRF24L01+ Main Functions */
void nrf24l01p_rx_init(nrf24l01Handler *handler, channel MHz, air_data_rate bps);
void nrf24l01p_tx_init(nrf24l01Handler *handler, channel MHz, air_data_rate bps);
void nrf24l01p_rx_receive(nrf24l01Handler *handler, uint8_t* rx_payload);
void nrf24l01p_tx_transmit(nrf24l01Handler *handler, uint8_t* tx_payload);
void nrf24l01p_tx_irq(nrf24l01Handler *handler);
/* nRF24L01+ Sub Functions */
void nrf24l01p_reset(nrf24l01Handler *handler);
void nrf24l01p_prx_mode(nrf24l01Handler *handler);
void nrf24l01p_ptx_mode(nrf24l01Handler *handler);
void nrf24l01p_flush_rx_fifo(nrf24l01Handler *handler);
void nrf24l01p_flush_tx_fifo(nrf24l01Handler *handler);
uint8_t nrf24l01p_get_status(nrf24l01Handler *handler);
uint8_t nrf24l01p_get_fifo_status(nrf24l01Handler *handler);
void nrf24l01p_rx_set_payload_widths(nrf24l01Handler *handler, widths bytes);
void nrf24l01p_clear_rx_dr(nrf24l01Handler *handler);
void nrf24l01p_clear_tx_ds(nrf24l01Handler *handler);
void nrf24l01p_clear_max_rt(nrf24l01Handler *handler);
void nrf24l01p_power_up(nrf24l01Handler *handler);
void nrf24l01p_power_down(nrf24l01Handler *handler);
void nrf24l01p_set_crc_length(nrf24l01Handler *handler, length bytes);
void nrf24l01p_set_address_widths(nrf24l01Handler *handler, widths bytes);
void nrf24l01p_auto_retransmit_count(nrf24l01Handler *handler, count cnt);
void nrf24l01p_auto_retransmit_delay(nrf24l01Handler *handler, delay us);
void nrf24l01p_set_rf_channel(nrf24l01Handler *handler, channel MHz);
void nrf24l01p_set_rf_tx_output_power(nrf24l01Handler *handler, output_power dBm);
void nrf24l01p_set_rf_air_data_rate(nrf24l01Handler *handler, air_data_rate bps);

/* nRF24L01+ Commands */
#define NRF24L01P_CMD_R_REGISTER                  0b00000000
#define NRF24L01P_CMD_W_REGISTER                  0b00100000
#define NRF24L01P_CMD_R_RX_PAYLOAD                0b01100001
#define NRF24L01P_CMD_W_TX_PAYLOAD                0b10100000
#define NRF24L01P_CMD_FLUSH_TX                    0b11100001
#define NRF24L01P_CMD_FLUSH_RX                    0b11100010
#define NRF24L01P_CMD_REUSE_TX_PL                 0b11100011
#define NRF24L01P_CMD_R_RX_PL_WID                 0b01100000
#define NRF24L01P_CMD_W_ACK_PAYLOAD               0b10101000
#define NRF24L01P_CMD_W_TX_PAYLOAD_NOACK          0b10110000
#define NRF24L01P_CMD_NOP                         0b11111111

/* nRF24L01+ Registers */
#define NRF24L01P_REG_CONFIG            0x00
#define NRF24L01P_REG_EN_AA             0x01
#define NRF24L01P_REG_EN_RXADDR         0x02
#define NRF24L01P_REG_SETUP_AW          0x03
#define NRF24L01P_REG_SETUP_RETR        0x04
#define NRF24L01P_REG_RF_CH             0x05
#define NRF24L01P_REG_RF_SETUP          0x06
#define NRF24L01P_REG_STATUS            0x07
#define NRF24L01P_REG_OBSERVE_TX        0x08    // Read-Only
#define NRF24L01P_REG_RPD               0x09    // Read-Only
#define NRF24L01P_REG_RX_ADDR_P0        0x0A
#define NRF24L01P_REG_RX_ADDR_P1        0x0B
#define NRF24L01P_REG_RX_ADDR_P2        0x0C
#define NRF24L01P_REG_RX_ADDR_P3        0x0D
#define NRF24L01P_REG_RX_ADDR_P4        0x0E
#define NRF24L01P_REG_RX_ADDR_P5        0x0F
#define NRF24L01P_REG_TX_ADDR           0x10
#define NRF24L01P_REG_RX_PW_P0          0x11
#define NRF24L01P_REG_RX_PW_P1          0x12
#define NRF24L01P_REG_RX_PW_P2          0x13
#define NRF24L01P_REG_RX_PW_P3          0x14
#define NRF24L01P_REG_RX_PW_P4          0x15
#define NRF24L01P_REG_RX_PW_P5          0x16
#define NRF24L01P_REG_FIFO_STATUS       0x17
#define NRF24L01P_REG_DYNPD             0x1C
#define NRF24L01P_REG_FEATURE           0x1D




#endif /* INC_NRF24L01_H_ */
