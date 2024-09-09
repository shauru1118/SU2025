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


void LSM_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *port, uint16_t pin) {
	_lsm_hspi1 = hspi;
	_lsm_nssPort = port;
	_lsm_nssPin = pin;

}

