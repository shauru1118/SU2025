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

uint16_t _calibrCoeff[7] = { 0x00 };
struct {
	double Tref;
	double TempSens;
	double OffT1;
	double Tco;
	double SensT1;
	double Tcs;
} _realCalibrCoeff;

void _msReadAdc(uint32_t bufForPresTemp[]);
void _msReadProm(uint16_t bufForCalibrCoef[]);
void _calculate(uint32_t dataWithPressTemp[], uint32_t bufer[]);

void _msSendCmd(uint8_t cmd);
void _msSendCmdGetData16(uint8_t cmd, uint16_t bufer[], uint8_t i);
void _msSendCmdGetData32(uint8_t cmd, uint32_t bufer[]);

//real

void MS_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *port, uint16_t pin) {

	_ms_hspi1 = hspi;
	_ms_nssPort = port;
	_ms_nssPin = pin;

	_msSendCmd(MS_RESET);
	HAL_Delay(100);

	_msReadProm(_calibrCoeff);

}

void MS_ReadData(uint32_t endBufer[]) {
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

	_realCalibrCoeff.Tref = (_calibrCoeff[5] * (2 << 7));
	_realCalibrCoeff.TempSens = _calibrCoeff[6] / (2 << 22);
	_realCalibrCoeff.OffT1 = _calibrCoeff[2] * (2 << 15);
	_realCalibrCoeff.Tco = (_calibrCoeff[4]) / (2 << 6);
	_realCalibrCoeff.SensT1 = _calibrCoeff[1] * (2 << 14);
	_realCalibrCoeff.Tcs = (_calibrCoeff[3]) / (2 << 7);
}

void _calculate(uint32_t dataWithPressTemp[], uint32_t bufer[]) {
	uint32_t D1 = dataWithPressTemp[0];
	uint32_t D2 = dataWithPressTemp[1];

	float dT = D2 - _realCalibrCoeff.Tref;
	float TEMP = 2000 + dT * _realCalibrCoeff.TempSens;

	float OFF = _realCalibrCoeff.OffT1 + _realCalibrCoeff.Tco * dT;

	float SENS = _realCalibrCoeff.SensT1 + _realCalibrCoeff.Tcs * dT;

	if (TEMP < 2000) {
		float T2 = (dT * dT) / (2 << 30);
		float OFF2 = 5 * (TEMP - 2000) * (TEMP - 2000) / 2;
		float SENS2 = 5 * (TEMP - 2000) * (TEMP - 2000) / (2 * 2);

		if (TEMP < -1500) {
			OFF2 = OFF2 + 7 * (TEMP + 1500) * (TEMP + 1500);
			SENS2 = SENS2 + 11 * (TEMP + 1500) * (TEMP + 1500) / 2;
		}

		TEMP -= T2;
		OFF -= OFF2;
		SENS -= SENS2;
	}

	float PRES = (D1 * SENS / (2 << 20) - OFF) / (2 << 14);

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
	HAL_Delay(10);
	HAL_SPI_Receive(_ms_hspi1, &data1, 1, 1000);
	HAL_SPI_Receive(_ms_hspi1, &data2, 1, 1000);

	_ms_nssPort->ODR |= _ms_nssPin;

	uint16_t receiveData = data1;
	receiveData = receiveData << 8;
	receiveData |= data2;

	bufer[i] = receiveData;
	HAL_Delay(20);

}
void _msSendCmdGetData32(uint8_t cmd, uint32_t bufer[]) {
	uint8_t presData[3];
	uint8_t tempData[3];

	_msSendCmd(MS_D1);
	HAL_Delay(ADC_DELAY);

	_ms_nssPort->ODR &= ~_ms_nssPin;
	HAL_SPI_Transmit(_ms_hspi1, &cmd, 1, 1000);
	HAL_SPI_Receive(_ms_hspi1, presData, 3, 1000);
	_ms_nssPort->ODR |= _ms_nssPin;

	_msSendCmd(MS_D2);
	HAL_Delay(ADC_DELAY);

	_ms_nssPort->ODR &= ~_ms_nssPin;
	HAL_SPI_Transmit(_ms_hspi1, &cmd, 1, 1000);
	HAL_SPI_Receive(_ms_hspi1, tempData, 3, 1000);
	_ms_nssPort->ODR |= _ms_nssPin;

	int32_t pres = presData[0];
	pres = pres << 8;
	pres |= presData[1];
	pres = pres << 8;
	pres |= presData[2];

	int32_t temp = tempData[0];
	temp = temp << 8;
	temp |= tempData[1];
	temp = temp << 8;
	temp |= tempData[2];

	bufer[0] = pres;
	bufer[1] = temp;
}
