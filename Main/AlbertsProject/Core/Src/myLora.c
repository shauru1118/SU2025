/*
 * myLora.c
 *
 *  Created on: Jul 25, 2024
 *      Author: rw3ww
 */

#include "myLoRa.h"

SPI_HandleTypeDef *_hspi;
GPIO_TypeDef *_nssPort;
uint16_t _nssPin;

uint32_t curFreq = 433;

uint8_t _loraReadReg(uint8_t reg);
void _loraWriteReg(uint8_t reg, uint8_t data);

uint8_t LORA_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *NSS_PORT,
		uint16_t NSS_PIN) {

	_hspi = hspi;
	_nssPort = NSS_PORT;
	_nssPin = NSS_PIN;

	if (_loraReadReg(LORA_REG_VERSION) != LORA_SEMTECH_ID)
		return 1;

	_loraWriteReg(LORA_REG_OP_MODE, LORA_MODE_SLEEP);

	_loraWriteReg(LORA_REG_OP_MODE,
			(LORA_SET_LoRa << 7) | (LORA_SET_LOW_FREQ << 3));

	uint32_t _lora_Frf = (curFreq << 19) / 32;

	_loraWriteReg(LORA_REG_FR, _lora_Frf >> 16);
	_loraWriteReg(LORA_REG_FR + 1, _lora_Frf >> 8);
	_loraWriteReg(LORA_REG_FR + 2, _lora_Frf);

	_loraWriteReg(LORA_REG_PA_CONFIG,
			(LORA_SET_PA_SEL << 7) | (LORA_SET_MAX_POWER << 4)
					| (LORA_SET_OUT_POWER));

	_loraWriteReg(LORA_REG_OCP, (LORA_SET_OCP_ON << 5) | LORA_SET_OCP_TRIM);

	_loraWriteReg(LORA_REG_LNA, LORA_SET_LNA_GAIN << 5);

	_loraWriteReg(LORA_REG_FIFO_TX, LORA_SET_FIFO_TX_BASE_ADDR);
	_loraWriteReg(LORA_REG_FIFO_RX, LORA_SET_FIFO_RX_BASE_ADDR);

	_loraWriteReg(LORA_REG_MODEM_CON,
			(LORA_SET_MODEM_CON_BW << 4) | (LORA_SET_MODEM_CON_COD_RATE << 1)
					| LORA_SET_MODEM_CON_HEADER_MODE_ON);

	_loraWriteReg(LORA_REG_MODEM_CON2,
			(LORA_SET_MODEM_CON2_SPEAD_FACT << 4)
					| (LORA_SET_MODEM_CON2_CRC << 2));

	_loraWriteReg(LORA_REG_PREAMBLE_MSB, LORA_SET_PREAMBLE >> 8);
	_loraWriteReg(LORA_REG_PREAMBLE_MSB + 1, LORA_SET_PREAMBLE);

	_loraWriteReg(LORA_REG_PAYLOAD, LORA_SET_PAYLOAD);
	_loraWriteReg(LORA_REG_MAX_PAYLOAD, LORA_SET_MAX_PAYLOAD);

	_loraWriteReg(LORA_REG_OP_MODE, LORA_MODE_RX_CONT);

	return 0;
}

uint8_t _loraReadReg(uint8_t reg) {
	_nssPort->ODR &= ~_nssPin; // nss 0
	HAL_SPI_Transmit(_hspi, &reg, 1, 1000);
	HAL_SPI_Receive(_hspi, &reg, 1, 1000);
	_nssPort->ODR |= _nssPin; // nss 1
	return reg;
}

void _loraWriteReg(uint8_t reg, uint8_t data) {
	reg |= 1 << 7;
	_nssPort->ODR &= ~_nssPin; // nss 0
	HAL_SPI_Transmit(_hspi, &reg, 1, 1000);
	HAL_SPI_Transmit(_hspi, &data, 1, 1000);
	_nssPort->ODR |= _nssPin; // nss 1
}

uint8_t LORA_ReceiveData(uint8_t *data) {

	if (_loraReadReg(LORA_REG_FLAGS) & (LORA_FLAGS_RX_DONE)) {
		_loraWriteReg(LORA_REG_FLAGS, LORA_FLAGS_RX_DONE);

		uint8_t packetSize = _loraReadReg(LORA_REG_RX_NB_BYTES);
		uint8_t curAddr = _loraReadReg(LORA_REG_FIFO_RX_CUR);
		uint8_t regFifo = LORA_REG_FIFO;

		_loraWriteReg(LORA_REG_FIFO_ADDR_PTR, curAddr);

		uint8_t spaceLeft = 255 - curAddr;
		if (spaceLeft >= packetSize) {
			_nssPort->ODR &= ~_nssPin; // nss 0
			HAL_SPI_Transmit(_hspi, &regFifo, 1, 1000);
			HAL_SPI_Receive(_hspi, data, packetSize, 1000);
			_nssPort->ODR |= _nssPin; // nss 1
		} else {
			_nssPort->ODR &= ~_nssPin; // nss 0
			HAL_SPI_Transmit(_hspi, &regFifo, 1, 1000);
			HAL_SPI_Receive(_hspi, data, spaceLeft, 1000);
			_nssPort->ODR |= _nssPin; // nss 1

			_loraWriteReg(LORA_REG_FIFO_ADDR_PTR, 0);

			_nssPort->ODR &= ~_nssPin; // nss 0
			HAL_SPI_Transmit(_hspi, &regFifo, 1, 1000);
			HAL_SPI_Receive(_hspi, data, packetSize - spaceLeft, 1000);
			_nssPort->ODR |= _nssPin; // nss 1
		}
		return packetSize;
	}
	return 0;
}

void LORA_TransmitData(uint8_t *data, uint8_t size) {
//	uint8_t dataToLora[size];

//	for (int i = 0; i < size; i++) {
//		dataToLora[i] = (uint8_t) data[i];
//	}

	_loraWriteReg(LORA_REG_FIFO_ADDR_PTR, LORA_SET_FIFO_TX_BASE_ADDR);
	_loraWriteReg(LORA_REG_PAYLOAD, size);

	uint8_t regFifo = LORA_REG_FIFO | (1 << 7);
	_nssPort->ODR &= ~_nssPin; // nss 0
	HAL_SPI_Transmit(_hspi, &regFifo, 1, 1000);
	HAL_SPI_Transmit(_hspi, data, size, 1000);
	_nssPort->ODR |= _nssPin; // nss 1

	_loraWriteReg(LORA_REG_OP_MODE, LORA_MODE_TX);
	while (!(_loraReadReg(LORA_REG_FLAGS) & LORA_FLAGS_TX_DONE))
		;
	_loraWriteReg(LORA_REG_FLAGS, LORA_FLAGS_TX_DONE);

	_loraWriteReg(LORA_REG_OP_MODE, LORA_MODE_RX_CONT);

}

uint16_t LORA_ChangeFreq(uint32_t freq) {
	curFreq = freq;
	uint32_t _lora_Frf = (curFreq << 19) / 32;

	_loraWriteReg(LORA_REG_FR, _lora_Frf >> 16);
	_loraWriteReg(LORA_REG_FR + 1, _lora_Frf >> 8);
	_loraWriteReg(LORA_REG_FR + 2, _lora_Frf);

	return curFreq;
}
