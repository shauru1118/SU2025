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
void _lsmReadRegs(uint8_t reg, uint8_t *buf, uint8_t len);

uint8_t LSM_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *port, uint16_t pin) {
	_lsm_hspi1 = hspi;
	_lsm_nssPort = port;
	_lsm_nssPin = pin;

	uint8_t whoAmI = _lsmReadReg(LSM_REG_WHO_AM_I);

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
	uint8_t tempreg = LSM_REG_OUTX_L_G;
	_lsmReadRegs(tempreg, buffer, 12);

	endBufer[0] = (int16_t) ((buffer[1] << 8) | buffer[0]);   // Gyro X
	endBufer[1] = (int16_t) ((buffer[3] << 8) | buffer[2]);   // Gyro Y
	endBufer[2] = (int16_t) ((buffer[5] << 8) | buffer[4]);   // Gyro Z
	endBufer[3] = (int16_t) ((buffer[7] << 8) | buffer[6]);   // Accel X
	endBufer[4] = (int16_t) ((buffer[9] << 8) | buffer[8]);   // Accel Y
	endBufer[5] = (int16_t) ((buffer[11] << 8) | buffer[10]); // Accel Z
}

void _lsmReadRegs(uint8_t reg, uint8_t *buf, uint8_t len) {
	reg |= 0x80;  // auto-increment
	_lsm_nssPort->ODR &= ~_lsm_nssPin;
	HAL_SPI_Transmit(_lsm_hspi1, &reg, 1, 1000);
	HAL_SPI_Receive(_lsm_hspi1, buf, len, 1000);
	_lsm_nssPort->ODR |= _lsm_nssPin;
}

uint8_t _lsmReadReg(uint8_t reg) {
	reg |= 0x80;
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

