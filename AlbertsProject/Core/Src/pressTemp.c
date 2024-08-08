/*
 * pressTemp.c
 *
 *  Created on: Aug 3, 2024
 *      Author: User
 */

#include "pressTemp.h"

uint8_t ADRS[8] = { 0xA0, 0xA2, 0xA4, 0xA6, 0xA8, 0xAA, 0xAC, 0xAE };

SPI_HandleTypeDef *_ms_hspi1;
GPIO_TypeDef *_ms_nssPort;
uint16_t _ms_nssPin;

uint16_t _calibrCoeff[7] = {0x00};

void _msReadAdc(uint32_t bufForPresTemp[]);
void _msReadProm(uint16_t bufForCalibrCoef[]);
void _calculate(uint32_t dataWithPressTemp[], int32_t bufer[]);

void _msSendCmd(uint8_t cmd);
void _msSendCmdGetData16(uint8_t cmd, uint16_t bufer[], uint8_t i);
void _msSendCmdGetData32(uint8_t cmd, uint32_t bufer[]);

//real

void MS_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *port, uint16_t pin) {

	_ms_hspi1 = hspi;
	_ms_nssPort = port;
	_ms_nssPin = pin;

	_msSendCmd(MS_RESET);

	_msReadProm(_calibrCoeff);

}

void MS_ReadData(int32_t endBufer[]) {
	uint32_t startPressTemp[2];

	_msReadAdc(startPressTemp);
	_calculate(startPressTemp, endBufer);

}

void _msReadAdc(uint32_t bufForPresTemp[]) {
	_msSendCmdGetData32(MS_ADC_READ, bufForPresTemp);
}

void _msReadProm(uint16_t bufForCalibrCoef[]) {
	_msSendCmdGetData16(ADRS[1], bufForCalibrCoef, 1);
	_msSendCmdGetData16(ADRS[2], bufForCalibrCoef, 2);
	_msSendCmdGetData16(ADRS[3], bufForCalibrCoef, 3);
	_msSendCmdGetData16(ADRS[4], bufForCalibrCoef, 4);
	_msSendCmdGetData16(ADRS[5], bufForCalibrCoef, 5);
	_msSendCmdGetData16(ADRS[6], bufForCalibrCoef, 6);
}

void _calculate(uint32_t dataWithPressTemp[], int32_t bufer[]) {
	uint32_t D1 = dataWithPressTemp[0];
	uint32_t D2 = dataWithPressTemp[1];

	int32_t dT = D2 - (_calibrCoeff[5] * (2 << 7));
	int32_t TEMP = 2000 + dT * _calibrCoeff[6] / (2 << 22);

	int64_t OFF = _calibrCoeff[2] * (2 << 15)
			+ (_calibrCoeff[4] * dT) / (2 << 6);

	int64_t SENS = _calibrCoeff[1] * (2 << 14)
			+ (_calibrCoeff[3] * dT) / (2 << 7);

	int32_t PRES = (D1 * SENS / (2 << 20) - OFF) / (2 << 14);

	bufer[0] = PRES;
	bufer[1] = TEMP;
}

void _msSendCmd(uint8_t cmd) {
	_ms_nssPort->ODR &= ~_ms_nssPin;
	HAL_SPI_Transmit(_ms_hspi1, &cmd, 1, 1000);
	_ms_nssPort->ODR |= _ms_nssPin;
}

void _msSendCmdGetData16(uint8_t cmd, uint16_t bufer[], uint8_t i) {
	uint8_t data1, data2;

	_ms_nssPort->ODR &= ~_ms_nssPin;

	HAL_SPI_Transmit(_ms_hspi1, &cmd, 1, 1000);
	HAL_SPI_Receive(_ms_hspi1, &data1, 1, 1000);
	HAL_SPI_Receive(_ms_hspi1, &data2, 1, 1000);

	_ms_nssPort->ODR |= _ms_nssPin;

	uint16_t receiveData = data1;
	receiveData = receiveData << 8;
	receiveData |= data2;

	bufer[i] = receiveData;
}
void _msSendCmdGetData32(uint8_t cmd, uint32_t bufer[]) {
	uint8_t presData[3];
	uint8_t tempData[3];

	_msSendCmd(MS_D1_4096);
	HAL_Delay(ADC_DELAY);

	_ms_nssPort->ODR &= ~_ms_nssPin;
	HAL_SPI_Transmit(_ms_hspi1, &cmd, 1, 1000);
	HAL_SPI_Receive(_ms_hspi1, presData, 3, 1000);
	_ms_nssPort->ODR |= _ms_nssPin;

	_msSendCmd(MS_D2_4096);
	HAL_Delay(ADC_DELAY);

	_ms_nssPort->ODR &= ~_ms_nssPin;
	HAL_SPI_Transmit(_ms_hspi1, &cmd, 1, 1000);
	HAL_SPI_Receive(_ms_hspi1, tempData, 3, 1000);
	_ms_nssPort->ODR |= _ms_nssPin;

	uint32_t pres = presData[0];
	pres = pres << 8;
	pres |= presData[1];
	pres = pres << 8;
	pres |= presData[2];

	uint32_t temp = tempData[0];
	temp = temp << 8;
	temp |= tempData[1];
	temp = temp << 8;
	temp |= tempData[2];


	bufer[0] = pres;
	bufer[1] = temp;
}
