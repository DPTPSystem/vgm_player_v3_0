/*
 * flash.c
 *
 *  Created on: 2023. jan. 17.
 *      Author: DPTP
 */
#include "flash.h"

void FlashErase(void)
{
	HAL_FLASH_Unlock();
	FLASH_PageErase(FlashAddr);
	CLEAR_REG(FLASH->CR);
	SET_BIT(FLASH->CR, 0x80);	// CR regiszter valamiért nem áll vissza 0x80-ra, ami a flash memória blokkolásáért felelõs (FLASH_CR_LOCK).
	HAL_FLASH_Lock();
}

void FlashWrite(uint32_t Addr, uint32_t Data)
{
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Addr, Data);
	HAL_FLASH_Lock();
}

uint32_t FlashRead(uint32_t Addr)
{
	uint32_t Data = 0;
	Data = *(uint16_t *)Addr;
	return Data;
}

