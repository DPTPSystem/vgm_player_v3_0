#include "w25qxx.h"
//#include "dwt_stm32_delay.h"

void SPI_Transmit (uint8_t *data, int size)
{

	/************** STEPS TO FOLLOW *****************
	1. Wait for the TXE bit to set in the Status Register
	2. Write the data to the Data Register
	3. After the data has been transmitted, wait for the BSY bit to reset in Status Register
	4. Clear the Overrun flag by reading DR and SR
	************************************************/
	uint8_t temp;
	int i=0;
	while (i<size)
	{
	   while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	   SPI1->DR = data[i];  // load the data into the Data Register
	   i++;
	}


	/*During discontinuous communications, there is a 2 APB clock period delay between the
	write operation to the SPI_DR register and BSY bit setting. As a consequence it is
	mandatory to wait first until TXE is set and then until BSY is cleared after writing the last
	data.
	*/
	while (!((SPI1->SR)&(1<<1))) {};  // wait for TXE bit to set -> This will indicate that the buffer is empty
	while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication

	//  Clear the Overrun flag by reading DR and SR
	temp = SPI1->DR;
	temp = SPI1->SR;

}

void SPI_Receive (uint8_t *data, int size)
{
	/************** STEPS TO FOLLOW *****************
	1. Wait for the BSY bit to reset in Status Register
	2. Send some Dummy data before reading the DATA
	3. Wait for the RXNE bit to Set in the status Register
	4. Read data from Data Register
	************************************************/

	while (size)
	{
		while (((SPI1->SR)&(1<<7))) {};  // wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
		SPI1->DR = 0;  // send dummy data
		while (!((SPI1->SR) &(1<<0))){};  // Wait for RXNE to set -> This will indicate that the Rx buffer is not empty
		*data++ = (SPI1->DR);
		size--;
	}
}

void SPI_Send(unsigned char adat)
{
	spi_tx[0] = adat;
	HAL_SPI_Transmit(&hspi1, spi_tx, 1, 1000);
}

unsigned char SPI_Get(void)
{
	HAL_SPI_Receive(&hspi1, spi_rx, 1, 1000);
	return spi_rx[0];
}

void waitForW25Qxx(void)
{
	unsigned int flag;
	do {
		ChipSelect(GPIO_PIN_RESET);                      	 //kiadjuk a Chip Enable jelet
		SPI_Send(CMD_RDSR);                       //Státuszregiszter olvasása parancs
		flag = SPI_Get();                    //Olvasunk, de valamit akkor is küldeni kell!
		ChipSelect(GPIO_PIN_SET);                   		 //megszüntetjük a Chip Enable jelet
	} while (flag & 0x01);
}

void MemWrite(union16 MemCim, unsigned char pbuf)
{
	waitForW25Qxx();
	ChipSelect(GPIO_PIN_RESET);
	SPI_Send(CMD_WREN);
	ChipSelect(GPIO_PIN_SET);
	HAL_Delay(1);
	ChipSelect(GPIO_PIN_RESET);
	HAL_Delay(1);
	spi_tx[0] = CMD_WRITE;
	spi_tx[1] = MemCim.low;
	spi_tx[2] = MemCim.high;
	spi_tx[3] = MemCim.hh;
	spi_tx[4] = pbuf;
	HAL_SPI_Transmit(&hspi1, spi_tx, 5, 1000);
	HAL_Delay(1);
	ChipSelect(GPIO_PIN_SET);
	HAL_Delay(1);
}

unsigned int memReadID()
{
	unsigned int temp = 0;
	waitForW25Qxx();
	ChipSelect(GPIO_PIN_RESET);
	HAL_Delay(1);
	spi_tx[0] = 0x4b;
	spi_tx[1] = 0;
	spi_tx[2] = 0;
	spi_tx[3] = 0;
	HAL_SPI_Transmit(&hspi1, spi_tx, 4, 1000);
	HAL_Delay(1);
	temp = SPI_Get();
	temp = (temp<<8) | SPI_Get();
	HAL_Delay(1);
	ChipSelect(GPIO_PIN_SET);
	return temp;
}

unsigned char MemReadFast(union16 MemCim)
{
		//ChipSelect(GPIO_PIN_RESET);
		GPIOB->BSRR = (uint32_t)GPIO_PIN_6 << 16u;

		spi_tx[0] = CMD_READ;
		spi_tx[1] = MemCim.low;
		spi_tx[2] = MemCim.high;
		spi_tx[3] = MemCim.hh;
		//HAL_SPI_Transmit(&hspi1, spi_tx, 4, 1000);
		//HAL_SPI_Receive(&hspi1, spi_rx, 1, 1000);
		SPI_Transmit (spi_tx, 4);
		SPI_Receive (spi_rx, 1);

		//ChipSelect(GPIO_PIN_SET);
		GPIOB->BSRR = GPIO_PIN_6;

		return spi_rx[0];
}

void FastmemReadW25Qxx(union16 MemCim, unsigned char *pbuf) {
	//waitForW25Qxx();                         //Várunk, ha az EEPROM elfoglalt
	//ChipSelect(GPIO_PIN_RESET);
	GPIOB->BSRR = (uint32_t)GPIO_PIN_6 << 16u;
	spi_tx[0] = CMD_FREAD;
	spi_tx[1] = MemCim.low;
	spi_tx[2] = MemCim.high;
	spi_tx[3] = MemCim.hh;
	HAL_SPI_Transmit(&hspi1, spi_tx, 4, 1000);
	//SPI_Get();
	HAL_SPI_Receive(&hspi1, pbuf, PMEMBLOCK, 1000);
	//ChipSelect(GPIO_PIN_SET);
	GPIOB->BSRR = GPIO_PIN_6;
}

unsigned char MemRead(union16 MemCim)
{
	unsigned char temp = 0;
	waitForW25Qxx();
	ChipSelect(GPIO_PIN_RESET);
	//DWT_Delay_us(1);
	spi_tx[0] = CMD_READ;
	spi_tx[1] = MemCim.low;
	spi_tx[2] = MemCim.high;
	spi_tx[3] = MemCim.hh;
	HAL_SPI_Transmit(&hspi1, spi_tx, 4, 10000);
	temp = SPI_Get();
	ChipSelect(GPIO_PIN_SET);
	return temp;
}

void MemErase(void){
	waitForW25Qxx();
	ChipSelect(GPIO_PIN_RESET);
	SPI_Send(CMD_WREN);
	ChipSelect(GPIO_PIN_SET);
	//HAL_Delay(1);
	ChipSelect(GPIO_PIN_RESET);
	SPI_Send(CMD_Erase);
	ChipSelect(GPIO_PIN_SET);
	waitForW25Qxx();
}
void MemBlockErase64Kb(union16 MemCim)
{
	waitForW25Qxx();
	ChipSelect(GPIO_PIN_RESET);
	SPI_Send(CMD_WREN);
	ChipSelect(GPIO_PIN_SET);
	//HAL_Delay(1);
	ChipSelect(GPIO_PIN_RESET);
	spi_tx[0] = CMD_BLCK64Kb;
	spi_tx[1] = MemCim.low;
	spi_tx[2] = MemCim.high;
	spi_tx[3] = MemCim.hh;
	HAL_SPI_Transmit(&hspi1, spi_tx, 4, 1000);
	ChipSelect(GPIO_PIN_SET);
	waitForW25Qxx();
}

void memWriteW25Qxx(union16 MemCim, unsigned char *pbuf) {
	waitForW25Qxx();                         //Várunk, ha az EEPROM elfoglalt
	ChipSelect(GPIO_PIN_RESET);
	SPI_Send(CMD_WREN);                       //Írás újraengedélyezése
	ChipSelect(GPIO_PIN_SET);
	HAL_Delay(1);
	ChipSelect(GPIO_PIN_RESET);
	spi_tx[0] = CMD_WRITE;
	spi_tx[1] = MemCim.low;
	spi_tx[2] = MemCim.high;
	spi_tx[3] = MemCim.hh;
	HAL_SPI_Transmit(&hspi1, spi_tx, 4, 1000);
	HAL_SPI_Transmit(&hspi1, pbuf, PMEMBLOCK, 1000);
	ChipSelect(GPIO_PIN_SET);
}

void memReadW25Qxx(union16 MemCim, unsigned char *pbuf) {
	waitForW25Qxx();                         //Várunk, ha az EEPROM elfoglalt
	ChipSelect(GPIO_PIN_RESET);
	spi_tx[0] = CMD_READ;
	spi_tx[1] = MemCim.low;
	spi_tx[2] = MemCim.high;
	spi_tx[3] = MemCim.hh;
	HAL_SPI_Transmit(&hspi1, spi_tx, 4, 1000);
	HAL_SPI_Receive(&hspi1, pbuf, PMEMBLOCK, 1000);
	ChipSelect(GPIO_PIN_SET);
}

