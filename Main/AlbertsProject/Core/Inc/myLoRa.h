/*
 * myLoRa.h
 *
 *  Created on: Jul 25, 2024
 *      Author: rw3ww
 */

#pragma once

#include "main.h"

#define LORA_REG_FIFO 0x00
#define LORA_REG_OP_MODE 0x01
#define LORA_REG_VERSION 0x42
#define LORA_REG_FR 0x06
#define LORA_REG_PA_CONFIG 0x09
#define LORA_REG_OCP 0x0B
#define LORA_REG_LNA 0x0C
#define LORA_REG_FIFO_ADDR_PTR 0x0D
#define LORA_REG_FIFO_TX 0x0E
#define LORA_REG_FIFO_RX 0x0F
#define LORA_REG_FIFO_RX_CUR 0x10
#define LORA_REG_MODEM_CON 0x1D
#define LORA_REG_MODEM_CON2 0x1E
#define LORA_REG_PREAMBLE_MSB 0x20
#define LORA_REG_PAYLOAD 0x22
#define LORA_REG_MAX_PAYLOAD 0x23
#define LORA_REG_FLAGS 0x12
#define LORA_REG_RX_NB_BYTES 0x13

#define LORA_MODE_SLEEP 0b000
#define LORA_MODE_STDBY 0b001
#define LORA_MODE_TX 0b011
#define LORA_MODE_RX_CONT 0b101
#define LORA_MODE_RX_SINGLE 0b110

#define LORA_SET_FREQ 433 // несущая частота
#define LORA_SET_MODEM_CON_BW 0x8
#define LORA_SET_MODEM_CON_COD_RATE 0b001
#define LORA_SET_MODEM_CON_HEADER_MODE_ON 0x0
#define LORA_SET_MODEM_CON2_SPEAD_FACT 0x07
#define LORA_SET_MODEM_CON2_CRC 0x00
#define LORA_SET_PREAMBLE 6
#define LORA_SET_PAYLOAD 60
#define LORA_SET_MAX_PAYLOAD 60

#define LORA_SET_LoRa 0x1 // работает LoRa
#define LORA_SET_LOW_FREQ 0x1 // режим низкой частоты
#define LORA_SET_PA_SEL 0x01 // усилитель мощности до +20 dBm
#define LORA_SET_MAX_POWER 0x7 // максимальная мощность сигнала
#define LORA_SET_OUT_POWER 0xF // выходная мощность сигнала
#define LORA_SET_OCP_ON 0b0 // защита от перегрузки по току
#define LORA_SET_OCP_TRIM 0x0b // лимит потока
#define LORA_SET_LNA_GAIN 0b001 // бесшумный усилитель
#define LORA_SET_FIFO_TX_BASE_ADDR 0xFF
#define LORA_SET_FIFO_RX_BASE_ADDR 0x00

#define LORA_FLAGS_RX_DONE (1<<6)
#define LORA_FLAGS_TX_DONE (1<<3)

#define LORA_SEMTECH_ID 0x12

uint8_t LORA_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *NSS_PORT,
		uint16_t NSS_PIN);

uint8_t LORA_ReceiveData(uint8_t *data);
void LORA_TransmitData(uint8_t* data, uint8_t size);
uint16_t LORA_ChangeFreq(uint32_t freq);

