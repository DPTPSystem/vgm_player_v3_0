#ifndef W25QXX_H
#define W25QXX_H

#ifdef __cplusplus
extern "C" {
#endif


#include "stm32f1xx_hal.h"
#include "main.h"

/*********************************************
+		Union deklar�l�sa
**********************************************/
/*- 16bit-es UNION - (SPI mem�ria c�mz�shez �s 2 b�jtos (16bit-es) adatok t�rol�s�hoz kell)*/
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

//----------- Parancsok 25D40 mem�ri�hoz -----------------------//
#define CMD_WRSR  		0x01		//St�tuszregiszter �r�sa
#define CMD_WRITE 		0x02		//�r�s a megadott c�mt�l kezd�d�en
#define CMD_READ  		0x03		//Olvas�s a megadott c�mt�l kezd�d�en
#define CMD_FREAD		0x0B		// Gyors olvas�s, WinBond 25Q64, 64byte
#define CMD_WRDI  		0x04		//Letiltja az �r�st
#define CMD_RDSR  		0x05		//St�tuszregiszter olvas�sa
#define CMD_WREN  		0x06		//Enged�lyezi az �r�st
#define CMD_Erase		0x60		// Flash t�rl�se
#define CMD_BLCK64Kb	0xD8		// 1 blokk t�rl�se (~150ms - ~2s), 1 blokk 64Kb
#define PMEMBLOCK		64
#define BufferArraySize 64

uint8_t spi_tx[5];
uint8_t spi_rx[2];

void SPI_Send(unsigned char adat);
unsigned char SPI_Get(void);
void waitForW25Qxx(void);									//V�rakoz�s a SPI mem�ri�ra
void MemWrite(union16 MemCim, unsigned char pbuf);
unsigned char MemRead(union16 MemCim);
unsigned char MemReadFast(union16 MemCim);
void MemErase(void);
void MemBlockErase64Kb(union16 MemCim);
void memWriteW25Qxx(union16 MemCim, unsigned char *pbuf);	//Mem�ria �r�sa
void memReadW25Qxx(union16 MemCim, unsigned char *pbuf);	//Meria olvas�sa
void FastmemReadW25Qxx(union16 MemCim, unsigned char *pbuf);
unsigned int memReadID();

void SPI_Transmit (uint8_t *data, int size);
void SPI_Receive (uint8_t *data, int size);

#ifdef __cplusplus
}
#endif

#endif
