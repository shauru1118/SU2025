/*
 * pressTemp.h
 *
 *  Created on: Aug 4, 2024
 *      Author: User
 */

#pragma once

#include "main.h"

#define MS_ADC_READ 0x00
#define MS_RESET 0x1E

#define MS_D1_256 0x40
#define MS_D1_512 0x42
#define MS_D1_1024 0x44
#define MS_D1_2048 0x46
#define MS_D1_4096 0x48

#define MS_D2_256 0x50
#define MS_D2_512 0x52
#define MS_D2_1024 0x54
#define MS_D2_2048 0x56
#define MS_D2_4096 0x58

#define ADC_DELAY 12

void MS_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *port, uint16_t pin);
void MS_ReadData(int32_t endBufer[]);
