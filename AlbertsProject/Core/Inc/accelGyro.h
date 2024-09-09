/*
 * accelGyro.h
 *
 *  Created on: Sep 1, 2024
 *      Author: sargs
 */

#pragma once

#include "main.h"

#define LSM_WHO_AM_I 0x0F
#define LSM_WHO_AM_I_VAL 0x69



void LSM_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *port, uint16_t pin);
void LSM_ReadData(uint32_t endBufer[]);
