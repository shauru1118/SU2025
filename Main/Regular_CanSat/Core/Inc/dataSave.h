/*
 * dataSave.h
 *
 *  Created on: Jul 26, 2025
 *      Author: sargs
 */

#ifndef INC_DATASAVE_H_
#define INC_DATASAVE_H_

typedef struct {
	uint32 time;
	int16_t temp;
	uint32_t press;
	int32_t alt;
	int16_t accel[3];
	int16_t gyro[3];
} allData;

#endif /* INC_DATASAVE_H_ */
