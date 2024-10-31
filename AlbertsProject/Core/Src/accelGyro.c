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

	return 0;
}

void LSM_ReadData(int16_t endBufer[]) {
	uint8_t bufer[6];

	uint8_t dataReg = LSM_REG_OUTX_L_XL;

	for (int i = 0; i < 6; i++) {
		bufer[i] = _lsmReadReg(dataReg+i);
	}
	for (int i=0; i<3; i++) {
		endBufer[i] = (((int16_t)bufer[2*i+1]) << 8) + bufer[2*i];
	}
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
