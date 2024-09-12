/*
 * accelGyro.h
 *
 *  Created on: Sep 1, 2024
 *      Author: sargs
 */

#pragma once

#include "main.h"

#define LSM_REG_FUNC_CFG_ACCESS 0x01
#define LSM_REG_SENSOR_SYNC_TIME_FRAME 0x04
#define LSM_REG_FIFO_CTRL1 0x06
#define LSM_REG_FIFO_CTRL2 0x07
#define LSM_REG_FIFO_CTRL3 0x08
#define LSM_REG_FIFO_CTRL4 0x09
#define LSM_REG_FIFO_CTRL5 0x0A
#define LSM_REG_ORIENT_CFG_G 0x0B
#define LSM_REG_INT1_CTRL 0x0D
#define LSM_REG_INT2_CTRL 0x0E
#define LSM_REG_WHO_AM_I 0x0F
#define LSM_REG_WHO_AM_I_VAL 0x69
#define LSM_REG_CTRL1_XL 0x10
#define LSM_REG_CTRL2_G 0x11
#define LSM_REG_CTRL3_C 0x12
#define LSM_REG_CTRL4_C 0x13
#define LSM_REG_CTRL5_C 0x14
#define LSM_REG_CTRL6_C 0x15
#define LSM_REG_CTRL7_G 0x16
#define LSM_REG_CTRL8_XL 0x17
#define LSM_REG_CTRL9_XL 0x18
#define LSM_REG_CTRL10_C 0x19
#define LSM_REG_MASTER_CONFIG 0x1A
#define LSM_REG_WAKE_UP_SRC 0x1B
#define LSM_REG_TAP_SRC 0x1C
#define LSM_REG_D6D_SRC 0x1D
#define LSM_REG_STATUS_REG 0x1E
#define LSM_REG_OUT_TEMP_L 0x20
#define LSM_REG_OUT_TEMP_H 0x21
#define LSM_REG_OUT_TEMP_L 0x20
#define LSM_REG_
#define LSM_REG_
#define LSM_REG_
#define LSM_REG_




void LSM_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *port, uint16_t pin);
void LSM_ReadData(uint32_t endBufer[]);
