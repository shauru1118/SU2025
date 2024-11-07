/*
 * accelGyro.c
 *
 *  Created on: Sep 1, 2024
 *      Author: sargs
 */

#include "accelGyro.h"

SPI_HandleTypeDef *_lsm_hspi1;
GPIO_TypeDef *_lsm_nssPort;
uint16_t _lsm_nssPin;

uint8_t _lsmReadReg(uint8_t reg);
void _lsmWriteReg(uint8_t reg, uint8_t data);

uint8_t LSM_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *port, uint16_t pin) {
	_lsm_hspi1 = hspi;
	_lsm_nssPort = port;
	_lsm_nssPin = pin;

	uint8_t whoAmI =_lsmReadReg(LSM_REG_WHO_AM_I);

	if (whoAmI != LSM_SET_WHO_AM_I_VAL) {
		return 1;
	}

	uint8_t xlmode = LSM_SET_CTRL1_XL_26_Hz;
	uint8_t gmode = LSM_SET_CTRL2_G_26_Hz;
	_lsmWriteReg(LSM_REG_CTRL1_XL, xlmode);
	_lsmWriteReg(LSM_REG_CTRL2_G, gmode);

	return 0;
}

void LSM_ReadData(int16_t endBufer[]) {
	uint8_t buffer[12];

	uint8_t dataReg = LSM_REG_OUTX_L_G;

	for (int i = 0; i < 12; i++) {
		buffer[i] = _lsmReadReg(dataReg+i);
	}

	endBufer[0] = (((uint16_t)buffer[1]) << 8) + buffer[0];
	endBufer[1] = (((uint16_t)buffer[3]) << 8) + buffer[2];
	endBufer[2] = (((uint16_t)buffer[5]) << 8) + buffer[4];
	endBufer[3] = (((uint16_t)buffer[7]) << 8) + buffer[6];
	endBufer[4] = (((uint16_t)buffer[9]) << 8) + buffer[8];
	endBufer[5] = (((uint16_t)buffer[11]) << 8) + buffer[10];
}

uint8_t _lsmReadReg(uint8_t reg) {
	reg = reg | (1 << 7);
	_lsm_nssPort->ODR &= ~_lsm_nssPin;
	HAL_SPI_Transmit(_lsm_hspi1, &reg, 1, 1000);
	HAL_SPI_Receive(_lsm_hspi1, &reg, 1, 1000);
	_lsm_nssPort->ODR |= _lsm_nssPin;
	return reg;
}

void _lsmWriteReg(uint8_t reg, uint8_t data) {
	_lsm_nssPort->ODR &= ~_lsm_nssPin;
	HAL_SPI_Transmit(_lsm_hspi1, &reg, 1, 1000);
	HAL_SPI_Transmit(_lsm_hspi1, &data, 1, 1000);
	_lsm_nssPort->ODR |= _lsm_nssPin;
}
