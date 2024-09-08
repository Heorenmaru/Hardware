/*
 *   Author: Heorenmaru
 *   License: CC-BY-SA
 */

#include "nrf24l01.h"


void nrf24_cs_high(nrf24l01Handler *handler)
{
	handler->nss.port->ODR |= handler->nss.pin;

//    HAL_GPIO_WritePin(NRF24L01P_SPI_CS_PIN_PORT, NRF24L01P_SPI_CS_PIN_NUMBER, GPIO_PIN_SET);
}

void nrf24_cs_low(nrf24l01Handler *handler)
{
	handler->nss.port->ODR &= ~handler->nss.pin;
//    HAL_GPIO_WritePin(NRF24L01P_SPI_CS_PIN_PORT, NRF24L01P_SPI_CS_PIN_NUMBER, GPIO_PIN_RESET);
}

void nrf24_ce_high(nrf24l01Handler *handler)
{
	handler->ce.port->ODR |= handler->ce.pin;
//    HAL_GPIO_WritePin(NRF24L01P_CE_PIN_PORT, NRF24L01P_CE_PIN_NUMBER, GPIO_PIN_SET);
}

void nrf24_ce_low(nrf24l01Handler *handler)
{
	handler->ce.port->ODR &= ~handler->ce.pin;
//    HAL_GPIO_WritePin(NRF24L01P_CE_PIN_PORT, NRF24L01P_CE_PIN_NUMBER, GPIO_PIN_RESET);
}

uint8_t read_register(nrf24l01Handler *handler, uint8_t reg)
{
    uint8_t command = NRF24L01P_CMD_R_REGISTER | reg;
    uint8_t status;
    uint8_t read_val;

    nrf24_cs_low(handler);
    HAL_SPI_TransmitReceive(handler->spi, &command, &status, 1, 2000);
    HAL_SPI_Receive(handler->spi, &read_val, 1, 2000);
    nrf24_cs_high(handler);

    return read_val;
}

uint8_t write_register(nrf24l01Handler *handler, uint8_t reg, uint8_t value)
{
    uint8_t command = NRF24L01P_CMD_W_REGISTER | reg;
    uint8_t status;
    uint8_t write_val = value;

    nrf24_cs_low(handler);
    HAL_SPI_TransmitReceive(handler->spi, &command, &status, 1, 2000);
    HAL_SPI_Transmit(handler->spi, &write_val, 1, 2000);
    nrf24_cs_high(handler);

    return write_val;
}

uint8_t nrf24l01p_read_rx_fifo(nrf24l01Handler *handler, uint8_t* rx_payload)
{
    uint8_t command = NRF24L01P_CMD_R_RX_PAYLOAD;
    uint8_t status;

    nrf24_cs_low(handler);
    HAL_SPI_TransmitReceive(handler->spi, &command, &status, 1, 2000);
    HAL_SPI_Receive(handler->spi, rx_payload, NRF24L01P_PAYLOAD_LENGTH, 2000);
    nrf24_cs_high(handler);

    return status;
}

uint8_t nrf24l01p_write_tx_fifo(nrf24l01Handler *handler, uint8_t* tx_payload)
{
    uint8_t command = NRF24L01P_CMD_W_TX_PAYLOAD;
    uint8_t status;

    nrf24_cs_low(handler);
    HAL_SPI_TransmitReceive(handler->spi, &command, &status, 1, 2000);
    HAL_SPI_Transmit(handler->spi, tx_payload, NRF24L01P_PAYLOAD_LENGTH, 2000);
    nrf24_cs_high(handler);

    return status;
}

void nrf24l01p_flush_rx_fifo(nrf24l01Handler *handler)
{
    uint8_t command = NRF24L01P_CMD_FLUSH_RX;
    uint8_t status;

    nrf24_cs_low(handler);
    HAL_SPI_TransmitReceive(handler->spi, &command, &status, 1, 2000);
    nrf24_cs_high(handler);
}

void nrf24l01p_flush_tx_fifo(nrf24l01Handler *handler)
{
    uint8_t command = NRF24L01P_CMD_FLUSH_TX;
    uint8_t status;

    nrf24_cs_low(handler);
    HAL_SPI_TransmitReceive(handler->spi, &command, &status, 1, 2000);
    nrf24_cs_high(handler);
}

uint8_t nrf24l01p_get_status(nrf24l01Handler *handler)
{
    uint8_t command = NRF24L01P_CMD_NOP;
    uint8_t status;

    nrf24_cs_low(handler);
    HAL_SPI_TransmitReceive(handler->spi, &command, &status, 1, 2000);
    nrf24_cs_high(handler);

    return status;
}
/* nRF24L01+ Main Functions */
void nrf24l01p_rx_init(nrf24l01Handler *handler, channel MHz, air_data_rate bps)
{
    nrf24l01p_reset(handler);

    nrf24l01p_prx_mode(handler);
    nrf24l01p_power_up(handler);

    nrf24l01p_rx_set_payload_widths(handler, NRF24L01P_PAYLOAD_LENGTH);

    nrf24l01p_set_rf_channel(handler, MHz);
    nrf24l01p_set_rf_air_data_rate(handler, bps);
    nrf24l01p_set_rf_tx_output_power(handler, _0dBm);

    nrf24l01p_set_crc_length(handler, 1);
    nrf24l01p_set_address_widths(handler, 5);

    nrf24l01p_auto_retransmit_count(handler, 3);
    nrf24l01p_auto_retransmit_delay(handler, 250);

    nrf24_ce_high(handler);
}

void nrf24l01p_tx_init(nrf24l01Handler *handler, channel MHz, air_data_rate bps)
{
    nrf24l01p_reset(handler);

    nrf24l01p_ptx_mode(handler);
    nrf24l01p_power_up(handler);

    nrf24l01p_set_rf_channel(handler, MHz);
    nrf24l01p_set_rf_air_data_rate(handler, bps);
    nrf24l01p_set_rf_tx_output_power(handler, _0dBm);

    nrf24l01p_set_crc_length(handler, 1);
    nrf24l01p_set_address_widths(handler, 5);

    nrf24l01p_auto_retransmit_count(handler, 3);
    nrf24l01p_auto_retransmit_delay(handler, 250);

    nrf24_ce_high(handler);

}

void nrf24l01p_rx_receive(nrf24l01Handler *handler, uint8_t* rx_payload)
{
    nrf24l01p_read_rx_fifo(handler, rx_payload);
    nrf24l01p_clear_rx_dr(handler);

//    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}

void nrf24l01p_tx_transmit(nrf24l01Handler *handler, uint8_t* tx_payload)
{
    nrf24l01p_write_tx_fifo(handler, tx_payload);
}

void nrf24l01p_tx_irq(nrf24l01Handler *handler)
{
    uint8_t tx_ds = nrf24l01p_get_status(handler);
    tx_ds &= 0x20;

    if(tx_ds)
    {
        // TX_DS
//        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        nrf24l01p_clear_tx_ds(handler);
    }

    else
    {
        // MAX_RT
//        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, SET);
        nrf24l01p_clear_max_rt(handler);
    }
}

/* nRF24L01+ Sub Functions */
void nrf24l01p_reset(nrf24l01Handler *handler)
{
    // Reset pins
	nrf24_cs_high(handler);
	nrf24_ce_low(handler);

    // Reset registers
    write_register(handler, NRF24L01P_REG_CONFIG, 0x08);
    write_register(handler, NRF24L01P_REG_EN_AA, 0x3F);
    write_register(handler, NRF24L01P_REG_EN_RXADDR, 0x03);
    write_register(handler, NRF24L01P_REG_SETUP_AW, 0x03);
    write_register(handler, NRF24L01P_REG_SETUP_RETR, 0x03);
    write_register(handler, NRF24L01P_REG_RF_CH, 0x02);
    write_register(handler, NRF24L01P_REG_RF_SETUP, 0x07);
    write_register(handler, NRF24L01P_REG_STATUS, 0x7E);
    write_register(handler, NRF24L01P_REG_RX_PW_P0, 0x00);
    write_register(handler, NRF24L01P_REG_RX_PW_P0, 0x00);
    write_register(handler, NRF24L01P_REG_RX_PW_P1, 0x00);
    write_register(handler, NRF24L01P_REG_RX_PW_P2, 0x00);
    write_register(handler, NRF24L01P_REG_RX_PW_P3, 0x00);
    write_register(handler, NRF24L01P_REG_RX_PW_P4, 0x00);
    write_register(handler, NRF24L01P_REG_RX_PW_P5, 0x00);
    write_register(handler, NRF24L01P_REG_FIFO_STATUS, 0x11);
    write_register(handler, NRF24L01P_REG_DYNPD, 0x00);
    write_register(handler, NRF24L01P_REG_FEATURE, 0x00);

    // Reset FIFO
    nrf24l01p_flush_rx_fifo(handler);
    nrf24l01p_flush_tx_fifo(handler);
}

void nrf24l01p_prx_mode(nrf24l01Handler *handler)
{
    uint8_t new_config = read_register(handler, NRF24L01P_REG_CONFIG);
    new_config |= 1 << 0;

    write_register(handler, NRF24L01P_REG_CONFIG, new_config);
}

void nrf24l01p_ptx_mode(nrf24l01Handler *handler)
{
    uint8_t new_config = read_register(handler, NRF24L01P_REG_CONFIG);
    new_config &= 0xFE;

    write_register(handler, NRF24L01P_REG_CONFIG, new_config);
}

uint8_t nrf24l01p_get_fifo_status(nrf24l01Handler *handler)
{
    return read_register(handler, NRF24L01P_REG_FIFO_STATUS);
}

void nrf24l01p_rx_set_payload_widths(nrf24l01Handler *handler, widths bytes)
{
    write_register(handler, NRF24L01P_REG_RX_PW_P0, bytes);
}

void nrf24l01p_clear_rx_dr(nrf24l01Handler *handler)
{
    uint8_t new_status = nrf24l01p_get_status(handler);
    new_status |= 0x40;

    write_register(handler, NRF24L01P_REG_STATUS, new_status);
}

void nrf24l01p_clear_tx_ds(nrf24l01Handler *handler)
{
    uint8_t new_status = nrf24l01p_get_status(handler);
    new_status |= 0x20;

    write_register(handler, NRF24L01P_REG_STATUS, new_status);
}

void nrf24l01p_clear_max_rt(nrf24l01Handler *handler)
{
    uint8_t new_status = nrf24l01p_get_status(handler);
    new_status |= 0x10;

    write_register(handler, NRF24L01P_REG_STATUS, new_status);
}

void nrf24l01p_power_up(nrf24l01Handler *handler)
{
    uint8_t new_config = read_register(handler, NRF24L01P_REG_CONFIG);
    new_config |= 1 << 1;

    write_register(handler, NRF24L01P_REG_CONFIG, new_config);
}

void nrf24l01p_power_down(nrf24l01Handler *handler)
{
    uint8_t new_config = read_register(handler, NRF24L01P_REG_CONFIG);
    new_config &= 0xFD;

    write_register(handler, NRF24L01P_REG_CONFIG, new_config);
}

void nrf24l01p_set_crc_length(nrf24l01Handler *handler, length bytes)
{
    uint8_t new_config = read_register(handler, NRF24L01P_REG_CONFIG);

    switch(bytes)
    {
        // CRCO bit in CONFIG resiger set 0
        case 1:
            new_config &= 0xFB;
            break;
        // CRCO bit in CONFIG resiger set 1
        case 2:
            new_config |= 1 << 2;
            break;
    }

    write_register(handler, NRF24L01P_REG_CONFIG, new_config);
}

void nrf24l01p_set_address_widths(nrf24l01Handler *handler, widths bytes)
{
    write_register(handler, NRF24L01P_REG_SETUP_AW, bytes - 2);
}

void nrf24l01p_auto_retransmit_count(nrf24l01Handler *handler, count cnt)
{
    uint8_t new_setup_retr = read_register(handler, NRF24L01P_REG_SETUP_RETR);

    // Reset ARC register 0
    new_setup_retr |= 0xF0;
    new_setup_retr |= cnt;
    write_register(handler, NRF24L01P_REG_SETUP_RETR, new_setup_retr);
}

void nrf24l01p_auto_retransmit_delay(nrf24l01Handler *handler, delay us)
{
    uint8_t new_setup_retr = read_register(handler, NRF24L01P_REG_SETUP_RETR);

    // Reset ARD register 0
    new_setup_retr |= 0x0F;
    new_setup_retr |= ((us / 250) - 1) << 4;
    write_register(handler, NRF24L01P_REG_SETUP_RETR, new_setup_retr);
}

void nrf24l01p_set_rf_channel(nrf24l01Handler *handler, channel MHz)
{
	uint16_t new_rf_ch = MHz - 2400;
    write_register(handler, NRF24L01P_REG_RF_CH, new_rf_ch);
}

void nrf24l01p_set_rf_tx_output_power(nrf24l01Handler *handler, output_power dBm)
{
    uint8_t new_rf_setup = read_register(handler, NRF24L01P_REG_RF_SETUP) & 0xF9;
    new_rf_setup |= (dBm << 1);

    write_register(handler, NRF24L01P_REG_RF_SETUP, new_rf_setup);
}

void nrf24l01p_set_rf_air_data_rate(nrf24l01Handler *handler, air_data_rate bps)
{
    // Set value to 0
    uint8_t new_rf_setup = read_register(handler, NRF24L01P_REG_RF_SETUP) & 0xD7;

    switch(bps)
    {
        case _1Mbps:
            break;
        case _2Mbps:
            new_rf_setup |= 1 << 3;
            break;
        case _250kbps:
            new_rf_setup |= 1 << 5;
            break;
    }
    write_register(handler, NRF24L01P_REG_RF_SETUP, new_rf_setup);
}
