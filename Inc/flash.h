#ifndef FLASH_H
#define FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "main.h"

/* Microcontroller : 	STM32F103RFT6
 * Frequency : 			72MHz
 * Flash : 				768KB
 * RAM : 				96KB
 * Vendor : 			Generic
 * Bank1				0x0800 0000 - 0x0807 FFFF - 512 kbyte
 * Bank2				0x0808 0000 - 0x080B FFFF - 256 kbyte (Ezzel dolgozunk)
 * Bank2 				0x0808 0000 - 0x080F FFFF - 512 kbyte
 * Minden 1Kbyte 0x400, 26Kbyte-ot lefoglalva 0x6800
 * Meghatározott cím a flash végén: 0x080B 9800 (ettõl a címtõl kezdve van 25kbyte tárhelyünk + 1kbyte tartalék, hogy ne csúszunk túl
 */

#define FlashAddr	0x80B9800		// maximum 0x80BFBFF vagy is 25kbyte területem van


void FlashWrite(uint32_t Addr, uint32_t Data);
uint32_t FlashRead(uint32_t Addr);
void FlashErase(void);

#ifdef __cplusplus
}
#endif

#endif
