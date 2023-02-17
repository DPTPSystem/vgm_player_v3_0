#ifndef YM2612_H
#define YM2612_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "dwt_stm32_delay.h"
#include "w25qxx.h"
#include "main.h"

// SN76489AN
#define SN_WE(x)		HAL_GPIO_WritePin(SN_WE_GPIO_Port, SN_WE_Pin, x)
#define SN_PORT(data) 	GPIOC->ODR = data;
//#define SN_PORT(data) 	GPIOB->ODR = (GPIOB->ODR & ~0b0001000011111011) | (((data & 0b00000100) << 10) | (data & 0b11111011));
//#define SN_OE(x)		HAL_GPIO_WritePin(SN_OE_GPIO_Port, SN_OE_Pin, x)

// YM2612
#define YM_IC(x)		HAL_GPIO_WritePin(YM_IC_GPIO_Port, YM_IC_Pin, x)
#define YM_CS(x)		HAL_GPIO_WritePin(YM_CS_GPIO_Port, YM_CS_Pin, x)
#define YM_WR(x)		HAL_GPIO_WritePin(YM_WR_GPIO_Port, YM_WR_Pin, x)
#define YM_RD(x)		HAL_GPIO_WritePin(YM_RD_GPIO_Port, YM_RD_Pin, x)
#define YM_A0(x)		HAL_GPIO_WritePin(YM_A0_GPIO_Port, YM_A0_Pin, x)
#define YM_A1(x)		HAL_GPIO_WritePin(YM_A1_GPIO_Port, YM_A1_Pin, x)
#define YM_PORT(data) 	YM_D0_GPIO_Port->ODR = data;

#define PCMMaxSize 0x8000

volatile union16 MemCim, PCMMEMADDR;					//16bit-es struct változók
volatile uint32_t MEMADDR;
volatile uint32_t pcmPosStart;
volatile uint32_t PCMDataCount;
volatile uint8_t PCMBuff[PCMMaxSize];
volatile uint8_t PCMLoad;
volatile uint16_t PCMBuffIndex;
volatile uint16_t VGMVersion;

// PCM táblázat használatához
volatile uint16_t JumpTableE0[1024];			// PCM ugró tábla
volatile uint16_t PCMJumpIndex;


void SN76489_SendByte(unsigned char byte);
void SN76489_SendByteFast(unsigned char byte);
void PSG_Silence(void);
void YM2612_Send(unsigned char addr, unsigned char data, unsigned char CMD);
void YM2612_SendFast(unsigned char addr, unsigned char data, unsigned char CMD);
volatile unsigned int WaveSample(void);
volatile unsigned int WaveSampleIf(void);
void PCMJumpSave(void);

#ifdef __cplusplus
}
#endif

#endif
