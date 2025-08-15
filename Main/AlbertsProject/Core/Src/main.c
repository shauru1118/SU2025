/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "sdio.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>

#include "myLoRa.h"
#include "accelGyro.h"
#include "pressTemp.h"


//#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PACKET_SIZE sizeof(RadioData_t)

#define ROVERDATA_SIZE sizeof(RoverData_t)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t MS_Data[2];
int16_t LSM_Data[6];

int sizeOfBufferToLora = 100;
int32_t seaLvlPress;
int32_t beforAlt;
uint8_t toStart = 100;
uint32_t timeToSend = 0;
uint32_t timeToWrite = 0;
uint16_t roverFreq = 434; // !!!!!!!!---- YZNAT' CHASTOTY ----!!!!!!!!
const char filename[] = "SULOG.txt";

struct {
	uint32_t time;
	int32_t alt;
	int32_t press;
	int16_t temp;
	int16_t accel1;
	int16_t accel2;
	int16_t accel3;
	int16_t gyro1;
	int16_t gyro2;
	int16_t gyro3;
	bool flagStart;
	bool flagApag;
	bool flagLand;

} SensorsData;

#pragma pack(push, 1)
typedef struct {
	uint16_t start_of_packet; // 0xAAAA
	uint16_t team_id;
	uint32_t time;
	int16_t temperature;
	int32_t pressure;
	int16_t accelerometer_x;
	int16_t accelerometer_y;
	int16_t accelerometer_z;
	int16_t gyroscope_x;
	int16_t gyroscope_y;
	int16_t gyroscope_z;
	uint8_t checksum;

	uint8_t answer;
	uint16_t distanses[17];

} RadioData_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
	uint8_t start_of_the_pocket; // должен быть 0x13
	uint8_t answer;
	uint16_t distanses[17];
	uint8_t checksum; // XOR всех предыдущих байт
} RoverData_t;
#pragma pack(pop)

RoverData_t RoverData = { 0x13, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0 }, 0, };

enum {
	START, FLY, LAND
} mode;

FATFS FatFs;
FIL Fil;
FRESULT FR_Status;
unsigned int bytes_written;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void cheakAll(void);
void initVariables(void);
void readData(void);
void writeData(void);
void recieveTransmitData(void);

void SvSend(uint8_t *data, int size);
void SvGet(uint8_t *data, int expected_len);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_SDIO_SD_Init();
  MX_USART1_UART_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

	cheakAll();
	initVariables();

	HAL_UART_Transmit(&huart1, (uint8_t*) "b100", 4, 1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	while (1) {

		readData();

		writeData();

//		recieveTransmitData();

		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

//		SvSend((uint8_t*) "f100", 4);

		if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE)) {
//			HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
//			HAL_Delay(500);
//			HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);

			SvGet((uint8_t*) &RoverData, ROVERDATA_SIZE);

		}

		uint8_t buffer[255];
		int recieved = 0; // = SvGet((uint8_t*) &RoverData);

		if (recieved) {
//
		}

		recieved = LORA_ReceiveData(buffer);

		if (recieved) {
			SvSend(buffer, recieved);
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void writeSD_rData(RadioData_t str) {
	f_open(&Fil, filename, FA_OPEN_ALWAYS | FA_WRITE);
	f_lseek(&Fil, f_size(&Fil));
	HAL_Delay(5);

	// ПРАВИЛЬНО: передаём адрес структуры напрямую
	f_write(&Fil, &str, sizeof(RadioData_t), &bytes_written);

	HAL_Delay(15);
	f_close(&Fil);
}

void writeSD(char *str) {
	f_open(&Fil, filename, FA_OPEN_ALWAYS | FA_WRITE);

	f_lseek(&Fil, f_size(&Fil));
	HAL_Delay(5);
	f_puts(str, &Fil);
	HAL_Delay(15);
	f_close(&Fil);
}

void readData() {
	MS_ReadData(MS_Data);
	LSM_ReadData(LSM_Data);

	beforAlt = SensorsData.alt;

	SensorsData.time = HAL_GetTick();
	SensorsData.press = MS_Data[0];
	SensorsData.alt = (int32_t) 4433000
			* (1.0f - pow((float) SensorsData.press / seaLvlPress, 0.1903)); //(int32_t) ((SensorsData.press - seaLvlPress) / 133.32239023154 * 10.5 * 100);
	SensorsData.temp = MS_Data[1];
	SensorsData.accel1 = (int16_t) (LSM_Data[3] * 0.0488);
	SensorsData.accel2 = (int16_t) (LSM_Data[4] * 0.0488);
	SensorsData.accel3 = (int16_t) (LSM_Data[5] * 0.0488);
	SensorsData.gyro1 = LSM_Data[0] * 7;
	SensorsData.gyro2 = LSM_Data[1] * 7;
	SensorsData.gyro3 = LSM_Data[2] * 7;

	if (SensorsData.alt > toStart) {
		SensorsData.flagStart = true;
		mode = FLY;
	}
	if (SensorsData.alt == beforAlt && SensorsData.flagStart) {
		SensorsData.flagApag = true;
	}

	if ((SensorsData.alt < 200 || SensorsData.alt == beforAlt)
			&& SensorsData.flagApag) {
		SensorsData.flagLand = true;
		mode = LAND;
	}

}

void writeData() {

//	struct RadioData_t r;
	RadioData_t r_data = { 0xAAAA, 0xD8, HAL_GetTick(), SensorsData.temp,
			SensorsData.press, SensorsData.accel1, SensorsData.accel2,
			SensorsData.accel3, SensorsData.gyro1, SensorsData.gyro2,
			SensorsData.gyro3, 0, RoverData.answer, { RoverData.distanses[0],
					RoverData.distanses[1], RoverData.distanses[2],
					RoverData.distanses[3], RoverData.distanses[4],
					RoverData.distanses[5], RoverData.distanses[6],
					RoverData.distanses[7], RoverData.distanses[8],
					RoverData.distanses[9], RoverData.distanses[10],
					RoverData.distanses[11], RoverData.distanses[12],
					RoverData.distanses[13], RoverData.distanses[14],
					RoverData.distanses[15], RoverData.distanses[16] } };

	r_data.checksum = r_data.start_of_packet ^ r_data.team_id ^ r_data.time
			^ r_data.temperature ^ r_data.pressure ^ r_data.accelerometer_x
			^ r_data.accelerometer_y ^ r_data.accelerometer_z
			^ r_data.gyroscope_x ^ r_data.gyroscope_y ^ r_data.gyroscope_z;

	if (HAL_GetTick() - timeToWrite >= 50) {

//		char buffer[sizeOfBufferToLora];
//
//		snprintf(buffer, sizeOfBufferToLora,
//				"%s;%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld;%ld;%d;%d;%d;\n", "SU",
//				SensorsData.time, SensorsData.alt, SensorsData.press,
//				SensorsData.temp, SensorsData.accel1, SensorsData.accel2,
//				SensorsData.accel3, SensorsData.gyro1, SensorsData.gyro2,
//				SensorsData.gyro3, SensorsData.flagStart, SensorsData.flagApag,
//				SensorsData.flagLand);

		writeSD_rData(r_data);

		timeToWrite = HAL_GetTick();
	}

	if (HAL_GetTick() - timeToSend >= 1000) {

//		int cutPacketSize = sizeOfBufferToLora / 3;
//
//		char bufferToLoRa1[cutPacketSize];
//		char bufferToLoRa2[cutPacketSize];
//		char bufferToLoRa3[cutPacketSize];
//
//		int sizeOfSnprintf1 = snprintf(bufferToLoRa1, cutPacketSize,
//				"%s;%ld;%ld;", "SU", SensorsData.time, SensorsData.alt);
//		int sizeOfSnprintf2 = snprintf(bufferToLoRa2, cutPacketSize,
//				"%ld;%ld;%ld;%ld;", SensorsData.press, SensorsData.temp,
//				SensorsData.accel1, SensorsData.accel2);
//		int sizeOfSnprintf3 = snprintf(bufferToLoRa3, cutPacketSize,
//				"%ld;%d;%d;%d;", SensorsData.accel3, SensorsData.flagStart,
//				SensorsData.flagApag, SensorsData.flagLand);
//
//		LORA_TransmitData(bufferToLoRa1, sizeOfSnprintf1);
//		LORA_TransmitData(bufferToLoRa2, sizeOfSnprintf2);
//		LORA_TransmitData(bufferToLoRa3, sizeOfSnprintf3);

//		int sizeOfSnprintf = snprintf(bufferToLoRa, sizeOfBufferToLora,
//				"%s;%ld;%ld;%ld;%ld;%ld;%ld;%ld;%d;%d;%d;\n", "SU",
//				SensorsData.time, SensorsData.alt, SensorsData.press,
//				SensorsData.temp, SensorsData.accel1, SensorsData.accel2,
//				SensorsData.accel3, SensorsData.flagStart, SensorsData.flagApag,
//				SensorsData.flagLand);
		//
//		LORA_TransmitData(buffer, sizeOfSnprintf);

		LORA_TransmitData((uint8_t*) &r_data, 26);
		timeToSend = HAL_GetTick();
	}

}

void recieveTransmitData() {
	uint8_t recieveData[10];
	uint32_t size = LORA_ReceiveData(recieveData);
	if (size) {
//		char transmitData[10];
//		for (int i = 0; i < 10; i++) {
//			transmitData[i] = (char) recieveData[i];
//		}
		LORA_ChangeFreq(roverFreq);
		LORA_TransmitData(recieveData, 10);
		LORA_ChangeFreq(433);
		writeSD("transmit to rover\n");
	}

}

void cheakAll() {

	HAL_Delay(500);

	LED2_GPIO_Port->ODR |= LED2_Pin;
	LED3_GPIO_Port->ODR |= LED3_Pin;
	LED4_GPIO_Port->ODR |= LED4_Pin;

	HAL_Delay(500);

	LED2_GPIO_Port->ODR &= ~LED2_Pin;
	LED3_GPIO_Port->ODR &= ~LED3_Pin;
	LED4_GPIO_Port->ODR &= ~LED4_Pin;

	FR_Status = f_mount(&FatFs, SDPath, 1);

	if (FR_Status != FR_OK) {
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		HAL_Delay(300);
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		HAL_Delay(300);
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		HAL_Delay(300);
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		HAL_Delay(300);
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		HAL_Delay(300);
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);

	}

	f_unlink(filename);  // удаляет старый файл
	f_open(&Fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
	f_close(&Fil);

//	writeSD("SD GOOD\n");

	if (LORA_Init(&hspi1, LORA_NSS_GPIO_Port, LORA_NSS_Pin)) {
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		HAL_Delay(1000);
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
	} else {
//		writeSD("LoRa GOOD\n");
	}

	MS_Init(&hspi1, MS_NSS_GPIO_Port, MS_NSS_Pin);

	if (LSM_Init(&hspi1, LSM_NSS_GPIO_Port, LSM_NSS_Pin)) {
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		HAL_Delay(300);
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		HAL_Delay(300);
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		HAL_Delay(300);
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
	} else {
//		writeSD("LSM GOOD\n");
	}

//	writeSD("Start LOOP\n");

//	writeSD("name;time;height;pressure;temperature;accel_x;accel_y;accel_z;gyro_x;gyro_y;gyro_z;flag_start;flag_apag;flag_land;\n");

}

void initVariables() {
	MS_ReadData(MS_Data);
	seaLvlPress = MS_Data[0];
	SensorsData.flagStart = false;
	SensorsData.flagApag = false;
	SensorsData.flagLand = false;
	mode = START;
}

void SvSend(uint8_t *data, int size) {
	HAL_UART_Transmit(&huart1, data, size, HAL_MAX_DELAY);
}
void SvGet(uint8_t *data, int expected_len) {
//	int count = 0;
//	uint32_t tick_start = HAL_GetTick();
//
//	while ((HAL_GetTick() - tick_start) < SV_TIMEOUT && count < expected_len) {
//		uint8_t byte;
//		if (HAL_UART_Receive(&huart1, &byte, 1, 10) == HAL_OK) {
//			data[count++] = byte;
//			tick_start = HAL_GetTick(); // сброс таймаута при успешном приёме байта
//		}
//	}

	HAL_UART_Receive(&huart1, (uint8_t*) &RoverData, ROVERDATA_SIZE, 1000);

//	return count;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
