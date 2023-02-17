/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f1xx_hal.h"

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
void SystemClock_Config_128MHz(void);
void SystemClock_Config_Normal(void);
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SN_D0_Pin GPIO_PIN_0
#define SN_D0_GPIO_Port GPIOC
#define SN_D1_Pin GPIO_PIN_1
#define SN_D1_GPIO_Port GPIOC
#define SN_D2_Pin GPIO_PIN_2
#define SN_D2_GPIO_Port GPIOC
#define SN_D3_Pin GPIO_PIN_3
#define SN_D3_GPIO_Port GPIOC
#define YM_D0_Pin GPIO_PIN_0
#define YM_D0_GPIO_Port GPIOA
#define YM_D1_Pin GPIO_PIN_1
#define YM_D1_GPIO_Port GPIOA
#define YM_D2_Pin GPIO_PIN_2
#define YM_D2_GPIO_Port GPIOA
#define YM_D3_Pin GPIO_PIN_3
#define YM_D3_GPIO_Port GPIOA
#define YM_D4_Pin GPIO_PIN_4
#define YM_D4_GPIO_Port GPIOA
#define YM_D5_Pin GPIO_PIN_5
#define YM_D5_GPIO_Port GPIOA
#define YM_D6_Pin GPIO_PIN_6
#define YM_D6_GPIO_Port GPIOA
#define YM_D7_Pin GPIO_PIN_7
#define YM_D7_GPIO_Port GPIOA
#define SN_D4_Pin GPIO_PIN_4
#define SN_D4_GPIO_Port GPIOC
#define SN_D5_Pin GPIO_PIN_5
#define SN_D5_GPIO_Port GPIOC
#define SN_WE_Pin GPIO_PIN_0
#define SN_WE_GPIO_Port GPIOB
#define SN_OE_Pin GPIO_PIN_1
#define SN_OE_GPIO_Port GPIOB
#define YM_IC_Pin GPIO_PIN_2
#define YM_IC_GPIO_Port GPIOB
#define YM_CS_Pin GPIO_PIN_12
#define YM_CS_GPIO_Port GPIOB
#define YM_WR_Pin GPIO_PIN_13
#define YM_WR_GPIO_Port GPIOB
#define YM_RD_Pin GPIO_PIN_14
#define YM_RD_GPIO_Port GPIOB
#define YM_A0_Pin GPIO_PIN_15
#define YM_A0_GPIO_Port GPIOB
#define SN_D6_Pin GPIO_PIN_6
#define SN_D6_GPIO_Port GPIOC
#define SN_D7_Pin GPIO_PIN_7
#define SN_D7_GPIO_Port GPIOC
#define YM_A1_Pin GPIO_PIN_8
#define YM_A1_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_9
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_10
#define LED2_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_15
#define LED3_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_6
#define SPI1_CS_GPIO_Port GPIOB
#define GOMB_Pin_Pin GPIO_PIN_7
#define GOMB_Pin_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
