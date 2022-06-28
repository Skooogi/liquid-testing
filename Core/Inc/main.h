/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define EN_3V0_Pin GPIO_PIN_4
#define EN_3V0_GPIO_Port GPIOA
#define DEMOD_RESET_Pin GPIO_PIN_5
#define DEMOD_RESET_GPIO_Port GPIOA
#define DEMOD_CS_Pin GPIO_PIN_0
#define DEMOD_CS_GPIO_Port GPIOB
#define DEMOD_RXEN_Pin GPIO_PIN_1
#define DEMOD_RXEN_GPIO_Port GPIOB
#define DEMOD_TXEN_Pin GPIO_PIN_2
#define DEMOD_TXEN_GPIO_Port GPIOB
#define CANLED_Pin GPIO_PIN_5
#define CANLED_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */


/* GPIO macros to reduce writing and potential for mistakes */
#define GPIOW(pin, v) HAL_GPIO_WritePin(pin ## _GPIO_Port, pin ## _Pin, v)
#define GPIOR(pin) HAL_GPIO_ReadPin(pin ## _GPIO_Port, pin ## _Pin)

/* Extern the SPI handle type for receiver.c */
extern SPI_HandleTypeDef hspi2;
#define RXSPI (&hspi2)

/* Extern the ADC hanle types for adc.c */
extern ADC_HandleTypeDef hadc1;
#define ADCI (&hadc1)			// ADC responsible for I data

extern ADC_HandleTypeDef hadc2;
#define ADCQ (&hadc2)			// ADC responsible for Q data

extern ADC_HandleTypeDef hadc3;
#define ADCT (&hadc3)			// ADC responsible for MCU temperature data

/* Extern the TIM handle types for adc.c */
extern TIM_HandleTypeDef htim1;
#define TIM_1 (&htim1)			// Underscore to differentiate it from the instance name defined in main.c


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
