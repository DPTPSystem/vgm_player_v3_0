#ifndef W25QXX_H
#define W25QXX_H

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f1xx_hal.h"
#include "main.h"

/*********************************************
+		Union deklarálása
**********************************************/
/*- 16bit-es UNION - (SPI memória címzéshez és 2 bájtos (16bit-es) adatok tárolásához kell)*/
typedef union _union16 {
  unsigned long value;
  struct {
	unsigned char hh;
    unsigned char high;
	unsigned char low;
  };
} union16;

SPI_HandleTypeDef hspi1;
#define CS_Pin SPI1_CS_Pin
#define CS_GPIO_Port SPI1_CS_GPIO_Port
#define ChipSelect(x) HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, x)

//----------- Parancsok 25D40 memóriához -----------------------//
#define CMD_WRSR  		0x01		//Státuszregiszter írása
#define CMD_WRITE 		0x02		//Írás a megadott címtõl kezdõdõen
#define CMD_READ  		0x03		//Olvasás a megadott címtõl kezdõdõen
#define CMD_FREAD		0x0B		// Gyors olvasás, WinBond 25Q64, 64byte
#define CMD_WRDI  		0x04		//Letiltja az írást
#define CMD_RDSR  		0x05		//Státuszregiszter olvasása
#define CMD_WREN  		0x06		//Engedélyezi az írást
#define CMD_Erase		0x60		// Flash törlése
#define CMD_BLCK64Kb	0xD8		// 1 blokk törlése (~150ms - ~2s), 1 blokk 64Kb
#define PMEMBLOCK		64
#define BufferArraySize 64

uint8_t spi_tx[5];
uint8_t spi_rx[2];

void SPI_Send(unsigned char adat);
unsigned char SPI_Get(void);
void waitForW25Qxx(void);									//Várakozás a SPI memóriára
void MemWrite(union16 MemCim, unsigned char pbuf);
unsigned char MemRead(union16 MemCim);
unsigned char MemReadFast(union16 MemCim);
void MemErase(void);
void MemBlockErase64Kb(union16 MemCim);
void memWriteW25Qxx(union16 MemCim, unsigned char *pbuf);	//Memória írása
void memReadW25Qxx(union16 MemCim, unsigned char *pbuf);	//Meria olvasása
void FastmemReadW25Qxx(union16 MemCim, unsigned char *pbuf);
unsigned int memReadID();

void SPI_Transmit (uint8_t *data, int size);
void SPI_Receive (uint8_t *data, int size);

#ifdef __cplusplus
}
#endif

#endif
