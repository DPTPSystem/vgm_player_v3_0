/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * STM VGM Player 2023-01-17
  *
  * Tóth Péter - don_peter@freemail.hu
  * DPTP System
  *
  * Microcontroller : 	STM32F103RFT6
  * Frequency : 		72MHz
  * Flash : 			768KB
  * RAM : 				96KB
  * Vendor : 			Generic
  *
  *                             www.neo-geo.hu
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dwt_stm32_delay.h"
#include "w25qxx.h"
#include "usbd_cdc_if.h"
#include "ym2612.h"

//#include "fatfs_sd.h"

#include "fonts.h"
#include "ssd1306.h"

//#include "flash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BufferArraySize 64
//#define PCMJump
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

SD_HandleTypeDef hsd;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim6;

/* USER CODE BEGIN PV */
uint8_t BuffRX_a[BufferArraySize];
uint8_t BuffRX_b[BufferArraySize];
volatile uint32_t DataSize;
volatile uint16_t BufferI;
volatile uint16_t BufferSize;
volatile uint8_t bUffEr = 1;
volatile uint16_t PackCount=0;
volatile unsigned int waitSamples = 0;
volatile uint8_t SampleNext = 1;
volatile uint8_t PCMLoad;

FATFS FatFs;
FIL MyFile;      // file objects
BYTE buffer[512]; // file copy buffer
FRESULT res;         // FatFs function common result code
UINT br, bw;         // File R/W count
UINT bytes_written;
DIR dirs;
FILINFO finfo;
char path[16]={""};
//FRESULT fresult;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C2_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
struct VGMStreamHeader {
	// StreamInit 0x90
	uint8_t StreamID;	// Stream ID lekérése, nem kell ( 0x00 )
	uint8_t ChipType;	// Chip típusa, nem kell
	uint8_t PCMPort;	// Port	( 0x00 )
	uint8_t PCMCMD;		// Parancs mentése ( 0x2A )

	// StreamData 0x91
	uint8_t DataBankID;	// Data Bank ID (nincs tömörítve) ( 0x00 )
	uint8_t StepSize;	// Lépésméret, hány adatot hagyjon ki minden írás után (álltalában 1-et)
	uint8_t DataShift;	// Adat eltolás hozzáadva a start offset-hez

	// StreamFreq 0x92
	uint32_t Freq;		// Frekencia 4 byte

	// StreamStart 0x93
	uint32_t DataStartOffset;	// Data Start offset 4byte
	uint8_t LenghtMode;			// Length Mode
	uint32_t DataLength;		// Data Length 4 byte
	uint32_t PCMStartAdr;		// PCM adat kezdõ címe
};
volatile struct VGMStreamHeader VGMStream;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/********************************************************************************
 * - 2023-01-20
 * Timer6 407-es elõtöltéssel 22.67uS / megzsakítás szkóppal kimérve.
 * A 22.67uS idõ még mindig nem elégséges, bár ez lehet már csak optimalizálás
 * kérdése, de 23-24uS ideig tart egy PCM-et nem tartalmazó VGM adafolyam lejátszása.
 * Továbbá ebben a megszakítási idõben, legalább kétszeres a lejátszási sebesség, ezért
 * 22.67uS-rõl 45.34uS idõzítésre állítom, így a VGM zene a normális tempóban játsza le.
 * Ebben az új 816-os beállíátás mellet vagy is a 45.34uS megszakítássba viszonylag
 * stabil 46-47uS / megszakítással folyamatos PCM adat nélkül a lejátszás. Fõprogram
 * ebben az esetben 5-20mS közt elérhetõ.
 * Egy kisebb PCM-et tartalmazó fájl esetén 45-90uS-re nõt a megszakítások ideje
 * ez még ugyan memória olvasással mért adat.
 * Buffert alkalmazva kisebb PCM adat folyammal is viszonylag stabil 45-48uS alatt történik
 * a megszakítás kiszolgálása, ami majdnem teljesen bele fér az alap 45.34uS megszakításba.
 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	char CMDTX[1];

	uint16_t i = 0;
	uint8_t EraseBlock = 0;
	uint32_t DataSizePC = 0;
	uint16_t Maradek=0, Pack=0;
	uint8_t Upload;
	uint16_t temp = 0;
	PCMLoad = 0;
	uint8_t StrTemp[19] = {0};
	uint8_t Khz = 44;
	uint32_t FSize = 0;

	/*unsigned long pcmBufferPositionStart = 0;
	unsigned int PCMDataSize = 0;
	unsigned int wait = 0;
	unsigned char Sample = 0;
	unsigned char YM_address = 0;
	unsigned char YM_data = 0;

	const float sampleRate = 44100; //0.00002676
	const float WAIT60TH = 1000 / (sampleRate/735);
	const float WAIT50TH = 1000 / (sampleRate/882);
	float cachedWaitTime = 0;
	unsigned char Sample = 0;*/
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
  MX_USB_DEVICE_Init();
  MX_I2C2_Init();
  MX_SDIO_SD_Init();
  MX_TIM6_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  DWT_Delay_Init();
  SSD1306_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  PSG_Silence();
  //MemErase();

#ifdef PCMJump
  MemCim.value = 63;
  PCMJumpSave();
#endif

	/*MemCim.value = 0;
	for(int t=0; t<100; ++t)
	{
		temp = memReadID();
		Sample = MemRead(MemCim);
		MemCim.value++;
	}*/

  /*for(;;)	// Lefutás ideje, szkóppal mérve: 45uS/2=22.5uS
  {
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    if(!(GOMB_Pin_GPIO_Port->IDR & GOMB_Pin_Pin))
    	  {
    		  i=0;
    		  while(!(GOMB_Pin_GPIO_Port->IDR & GOMB_Pin_Pin));
    		  HAL_RCC_DeInit();
    		  HAL_Delay(100);
    		  SystemClock_Config_128MHz();
    		  HAL_Delay(100);
    	  }
    //DWT_Delay_us(22);
  }*/
  	// Kezdõ képernyõ
  	SSD1306_GotoXY (0,0);
	SSD1306_Puts ("DPTP System - 2023 ", &Font_7x10, 1);
	SSD1306_GotoXY (0,9);
	SSD1306_Puts ("VGM Player v3.0 ", &Font_7x10, 1);
	SSD1306_GotoXY (0,20);
	SSD1306_Puts ("System Ready ", &Font_7x10, 1);
	SSD1306_UpdateScreen();
	HAL_Delay(2000);

	/*SSD1306_ScrollRight(0,7);
	HAL_Delay(3000);
	SSD1306_ScrollLeft(1,7);
	HAL_Delay(3000);
	SSD1306_Stopscroll();
	//SSD1306_Clear();*/

  /*MemCim.value = 64;
  if(MemRead(MemCim) == 0x52 || MemRead(MemCim) == 0x67)
  {
	  MemCim.value = 63;
	  HAL_TIM_Base_Start_IT(&htim6);
  }
  else
	  MemCim.value = 128;

  if(MemRead(MemCim) == 0x52 || MemRead(MemCim) == 0x67)
  {
  	  MemCim.value = 127;
  	  HAL_TIM_Base_Start_IT(&htim6);
    }*/
	// Listázó (file)
	unsigned char FCount = 0, FCount2 = 0;
	BYTE FileName[200][32] = {0};
	BYTE FileSortName[200][12] = {0};
	char FnameCharCount = 0;

	#if _USE_LFN
		static char lfn[_MAX_LFN + 1];
		finfo.lfname = lfn;
		finfo.lfsize = sizeof lfn;
	#endif
	if (f_mount(&FatFs,"/",(BYTE)1) == FR_OK)
	{
	   //f_mount(&FatFs[0],"/",(BYTE)1);
	   if (f_opendir(&dirs, path) == FR_OK)
	   {
		 while (f_readdir(&dirs, &finfo) == FR_OK)
		 {
		   if (finfo.fattrib & AM_ARC)
		   {
			 if(!finfo.fname[0]) break;
			 FnameCharCount = strlen(finfo.lfname) > 32 ? 32 : strlen(finfo.lfname);
			 //finfo.fname
			 if(FnameCharCount>1)
			 {
				 memcpy(FileName[FCount++], finfo.lfname, FnameCharCount+1);
				 memcpy(FileSortName[FCount2++], finfo.fname, strlen(finfo.fname)+1);
			 }
			 else
			 {
				 memcpy(FileName[FCount++], finfo.fname, strlen(finfo.fname)+1);
				 memcpy(FileSortName[FCount2++], finfo.fname, strlen(finfo.fname)+1);
			 }	//finfo.lfname
			 // Fájl méretének megadása (3byte)
			 FileName[FCount-1][29] = finfo.fsize>>16;
			 FileName[FCount-1][30] = finfo.fsize>>8;
			 FileName[FCount-1][31] = finfo.fsize;
			 f_close(&MyFile);
		   }
		 }
	   }
	   FCount=0;
	   //FCount2=0;
	}
	else
	{
		// Hiba
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, SET);
	}

	// Timer2 - 44.1KHz
	// Ellenõrizük, hogy adott VGM fájl hol kezdõdik, 0x40 vagy 0x80-nál
	MemCim.value = 8;
	VGMVersion = MemRead(MemCim);
	MemCim.value++;
	VGMVersion |= ((uint16_t)MemRead(MemCim))<<8;
	if(VGMVersion==0x151)
	{
	  MemCim.value = 127;
	  HAL_TIM_Base_Start_IT(&htim6);
	}
	else
	{
	  MemCim.value = 63;
	  HAL_TIM_Base_Start_IT(&htim6);
	}

  while (1)	// 1uS
  {
	  // Gomb kezelése
	  if(!(GOMB_Pin_GPIO_Port->IDR & GOMB_Pin_Pin))
	  {
		  HAL_TIM_Base_Stop_IT(&htim6);
		  i=0;
		  while(!(GOMB_Pin_GPIO_Port->IDR & GOMB_Pin_Pin))
		  {
			  SSD1306_GotoXY (0,0);
			  SSD1306_Puts ("                   ", &Font_7x10, 1);
			  SSD1306_GotoXY (0,0);
			  if(i<=10)
			  {
				  SSD1306_Puts ("File step..", &Font_7x10, 1);
			  }
			  else if(i>10 && i<=30)
			  {
				  sprintf(StrTemp, "Step KHz: %02iKHz", Khz);
				  SSD1306_Puts (StrTemp, &Font_7x10, 1);
			  }
			  else if(i>30 && i<=40)
			  {
				  sprintf(StrTemp, "Step freq. %03iMHz", (uint32_t)HAL_RCC_GetHCLKFreq()/1000000);
				  SSD1306_Puts (StrTemp, &Font_7x10, 1);
			  }
			  else if(i>40)
			  {
				  SSD1306_Puts ("File upload!", &Font_7x10, 1);
			  }
			  SSD1306_UpdateScreen();
			  HAL_Delay(100);
			  i++;

			  if(i>80) i=0;
		  }
			SSD1306_GotoXY (0,0);
			SSD1306_Puts ("VGM Player v3.0 ", &Font_7x10, 1);
			SSD1306_GotoXY (0,20);
			SSD1306_Puts ("System Ready ", &Font_7x10, 1);
			SSD1306_UpdateScreen();
		  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, RESET);

		  if(i>30 && i<=40)	// 3-4s
		  {
			if((uint32_t)HAL_RCC_GetHCLKFreq() < 128000000)
			{
				HAL_RCC_DeInit();
				HAL_Delay(100);
				SystemClock_Config_128MHz();
			}
			else
			{
				HAL_RCC_DeInit();
				HAL_Delay(100);
				SystemClock_Config();
			}
			HAL_Delay(100);
		  }
		  else if(i>10 && i<=30) // Ha 2 és 3 másodpercet nyomtuk
		  {
			if((uint32_t)HAL_RCC_GetHCLKFreq() > 72000000)
			{
				if(htim6.Init.Period!=666)
				{
					htim6.Init.Period = 666;	// 48KHz
					HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, SET);
					Khz = 48;
				}
				else
				{
					htim6.Init.Period = 725; // Alapértelmezett 44.1KHz
					HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, RESET);
					Khz = 44;
				}
			}
			else
			{
				if(htim6.Init.Period!=749)
				{
					htim6.Init.Period = 749;	// 48KHz
					HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, SET);
					Khz = 48;
				}
				else
				{
					htim6.Init.Period = 816; // Alapértelmezett 44.1KHz
					HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, RESET);
					Khz = 44;
				}
			}

			HAL_TIM_Base_Init(&htim6);
		  }

		  if(i<=10)
		  {
				//SSD1306_Clear();
			  if(FCount>FCount2) FCount = 0;
				SSD1306_GotoXY (0,9);
				SSD1306_Puts ("                   ", &Font_7x10, 1);
				SSD1306_GotoXY (0,9);
				SSD1306_Puts (FileName[FCount++], &Font_7x10, 1);
				//SSD1306_ScrollRight(1,1);
				SSD1306_UpdateScreen();
			}
		  else if(i>10 && i<=40) // 3-4s
		  {
			//SSD1306_Clear();
			SSD1306_GotoXY (0,0);
			sprintf(StrTemp, "CP: %03iMHz F: %02iKHz", (uint32_t)HAL_RCC_GetHCLKFreq()/1000000, Khz);
			SSD1306_Puts (StrTemp, &Font_7x10, 1);
			SSD1306_UpdateScreen();
		  }
		  else if(i>=40)	// -5s
		  {
			  PSG_Silence();
			  // MD program beírása ROM-ba
				f_mount(&FatFs,"/",(BYTE)1);
				res = f_open(&MyFile, FileName[FCount-1], FA_OPEN_EXISTING | FA_READ);
				memset(buffer, '\0', sizeof(buffer));
				FSize = MyFile.fsize;
				if(res==FR_OK){
					// Flash törlése
					EraseBlock = (FSize/0xFFFF)>0 ? ((FSize/0xFFFF)+1) : 1;
					MemCim.value = 0;
					for(i=0; i<=EraseBlock; ++i)
					{
						MemBlockErase64Kb(MemCim);
						MemCim.value += 0xFFFF;
					}
					MemCim.value = 0;
				}
				do{
				  res = f_read(&MyFile, buffer, sizeof(buffer), &br);
				  for(i=0; i<(br/BufferArraySize); i++){
					//FSMC_NOR_WriteHalfWord(SramAddr, buffer[i]);
					  //memcpy(BuffRX_a, buffer[i*BufferArraySize], BufferArraySize);
					  for(int s=0; s<BufferArraySize; s++) BuffRX_a[s] = buffer[(i*BufferArraySize)+s];
					  memWriteW25Qxx(MemCim, BuffRX_a);
					  memset(BuffRX_a, '\0', sizeof(BuffRX_a));
					  MemCim.value+=64;
				  }
				  FSize -= br;

					SSD1306_GotoXY (0,20);
					SSD1306_Puts ("                   ", &Font_7x10, 1);
					SSD1306_GotoXY (0,20);
					sprintf(StrTemp, "FSize:%06ibyte", FSize);
					SSD1306_Puts (StrTemp, &Font_7x10, 1);
					SSD1306_UpdateScreen();

				  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
				}while(FSize && res == FR_OK);
				f_close(&MyFile);
				HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, RESET);
				MemCim.value = 8;
				VGMVersion = MemRead(MemCim);
				MemCim.value++;
				VGMVersion |= ((uint16_t)MemRead(MemCim))<<8;
				if(VGMVersion==0x151) MemCim.value = 127;
				else MemCim.value = 63;

				SSD1306_GotoXY (0,20);
				SSD1306_Puts ("                   ", &Font_7x10, 1);
				SSD1306_GotoXY (0,20);
				SSD1306_Puts ("Upload success!", &Font_7x10, 1);
				SSD1306_UpdateScreen();
		  }

		  HAL_TIM_Base_Start_IT(&htim6);
	  }

	  if(BuffRX_a[0] == 'd') {
		HAL_TIM_Base_Stop_IT(&htim6);
		BuffRX_a[0] = '\0';

		PSG_Silence();

		DataSize = 0;
		BufferSize = 0;
		BufferI=0;

		HAL_Delay(1);
		CMDTX[0] = 0x10;
		CDC_Transmit_FS((uint8_t *)CMDTX, 1);	// Kiküldjük az OK visszajelzést az upload kódot

		while(DataSize<3);	// Amig nem jön meg a 3 adat
		// PC-s adat hossza
		DataSizePC = BuffRX_a[2];
		DataSizePC = DataSizePC<<8 | BuffRX_a[1];
		DataSizePC = DataSizePC<<8 | BuffRX_a[0];
		Maradek = DataSizePC%BufferArraySize;	// Hossz vége, ha nem pontosan egyelõ BufferArraySize
		Pack = DataSizePC/BufferArraySize;		// Adat csomagok száma

		memset(BuffRX_a, '\0', sizeof(BuffRX_a));	//Törõljük a buffer tartalmát
		memset(BuffRX_b, '\0', sizeof(BuffRX_b));	//Törõljük a buffer tartalmát
		Upload=1;
		bUffEr=1;
		DataSize = 0;
		BufferSize = 0;
		BufferI=0;
		PackCount=0;
		MemCim.value = 0;

		EraseBlock = (DataSizePC/0xFFFF)>0 ? ((DataSizePC/0xFFFF)+1) : 1;

		//CMDTX[0] = EraseBlock;
		//CDC_Transmit_FS((uint8_t *)CMDTX, 1;

		for(i=0; i<=EraseBlock; ++i)
		{
			MemBlockErase64Kb(MemCim);
			MemCim.value += 0xFFFF;
		}

		MemCim.value = 0;

		CMDTX[0] = 0xF0;
		CDC_Transmit_FS((uint8_t *)CMDTX, 1);

		DataSize = 0;

		while(Upload){

			if(BufferSize==BufferArraySize || (PackCount == Pack && Maradek == BufferSize)){		// Ha minden rendben van

				if((PackCount == Pack && Maradek == BufferSize))
				{
					temp = Maradek;
					DataSize -= BufferArraySize;
					//DataSize += temp;
				}
				else
				{
					temp = BufferArraySize;
				}
				// Beírjuk az új adatokat a memóriába

				////// Két buffer töltésének írányítása, szervezése /////
				bUffEr = !bUffEr;
				BufferSize=0;
				BufferI=0;

				if(PackCount<=Pack){
					CMDTX[0] = 0x02;
					CDC_Transmit_FS((uint8_t *)CMDTX, 1); //strlen(CMDTX)-1);	// kiküldjük a tovább kódot
				}
				PackCount++;

				if(!bUffEr){
					memWriteW25Qxx(MemCim, BuffRX_a);
				}else{
					memWriteW25Qxx(MemCim, BuffRX_b);
				}

				// Adat kiírása AddresCounter,
				MemCim.value += temp;
				//DataSizePC == DataSize &&
				if(DataSizePC == DataSize && PackCount >= Pack)
				{
					Upload = 0;
					bUffEr = 1; 				// Alapértelmezett buffer használata
					BufferSize=0;
					BufferI=0;
					CMDTX[0] = 0x02;
					CDC_Transmit_FS((uint8_t *)CMDTX, 1);//strlen(CMDTX)-1);	// kiküldjuk a tovább kódot

					MemCim.value = 0;
					memset(BuffRX_a, '\0', sizeof(BuffRX_a));	//Törõljük a buffer tartalmát
					memset(BuffRX_b, '\0', sizeof(BuffRX_b));	//Törõljük a buffer tartalmát
				}// Kilépünk írásmódból
				HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
			}

		}//while
		// Ellenõrizük, hogy adott VGM fájl hol kezdõdik, 0x40 vagy 0x80-nál
		MemCim.value = 8;
		VGMVersion = MemRead(MemCim);
		MemCim.value++;
		VGMVersion |= ((uint16_t)MemRead(MemCim))<<8;
		if(VGMVersion==0x151) MemCim.value = 127;
		else MemCim.value = 63;

#ifdef PCMJump
		// Ugró tábla
		memset(JumpTableE0, '\0', sizeof(JumpTableE0));
		PCMJumpSave();
		MemCim.value = 63;
#endif

		//Reset();
		//EepromWrite(0x00);
		PCMLoad = 0;
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);
		i=0;

		HAL_TIM_Base_Start_IT(&htim6);
	}

	  // 1uS idõ alatt ér ide a program
	  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	  HAL_Delay(100);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 4;
  /* USER CODE BEGIN SDIO_Init 2 */
  // Ha az SD kezelése FR_DISK_ERROR-ra futna, akkor a hsd.Init.ClockDiv-nt növelni kell.
  /* USER CODE END SDIO_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */
  // 72M / Pre / Post (Ez kell 0,0000226)
  // ((((72000000/2) / (1 / (1 / 44100)) + 0,5) - 1) = 815
  // ((((72000000/2) / (1 / (1 / 48000)) + 0,5) - 1) = 749
  // ((((64000000/2) / (1 / (1 / 44100)) + 0,5) - 1) = 725	// Ha 125MHz-en ját az MCU, akkor sajnos a ABP2 64MHz-re csökken
  // ((((64000000/2) / (1 / (1 / 48000)) + 0,5) - 1) = 666
  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 816;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SN_D0_Pin|SN_D1_Pin|SN_D2_Pin|SN_D3_Pin 
                          |SN_D4_Pin|SN_D5_Pin|SN_D6_Pin|SN_D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, YM_D0_Pin|YM_D1_Pin|YM_D2_Pin|YM_D3_Pin 
                          |YM_D4_Pin|YM_D5_Pin|YM_D6_Pin|YM_D7_Pin 
                          |YM_A1_Pin|LED1_Pin|LED2_Pin|LED3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SN_WE_Pin|YM_IC_Pin|YM_CS_Pin|YM_WR_Pin 
                          |YM_RD_Pin|YM_A0_Pin|SPI1_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SN_OE_GPIO_Port, SN_OE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SN_D0_Pin SN_D1_Pin SN_D2_Pin SN_D3_Pin 
                           SN_D4_Pin SN_D5_Pin SN_D6_Pin SN_D7_Pin */
  GPIO_InitStruct.Pin = SN_D0_Pin|SN_D1_Pin|SN_D2_Pin|SN_D3_Pin 
                          |SN_D4_Pin|SN_D5_Pin|SN_D6_Pin|SN_D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : YM_D0_Pin YM_D1_Pin YM_D2_Pin YM_D3_Pin 
                           YM_D4_Pin YM_D5_Pin YM_D6_Pin YM_D7_Pin 
                           YM_A1_Pin LED1_Pin LED2_Pin LED3_Pin */
  GPIO_InitStruct.Pin = YM_D0_Pin|YM_D1_Pin|YM_D2_Pin|YM_D3_Pin 
                          |YM_D4_Pin|YM_D5_Pin|YM_D6_Pin|YM_D7_Pin 
                          |YM_A1_Pin|LED1_Pin|LED2_Pin|LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SN_WE_Pin SN_OE_Pin YM_IC_Pin YM_CS_Pin 
                           YM_WR_Pin YM_RD_Pin YM_A0_Pin SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SN_WE_Pin|SN_OE_Pin|YM_IC_Pin|YM_CS_Pin 
                          |YM_WR_Pin|YM_RD_Pin|YM_A0_Pin|SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : GOMB_Pin_Pin */
  GPIO_InitStruct.Pin = GOMB_Pin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GOMB_Pin_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
// ((((72000000/2) / (1 / (1 / 44100)) + 0,5) - 1)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)	// Ha érkezik timer megszakítás
	{
		if(waitSamples>0)
			waitSamples--;

		if(waitSamples==0 && SampleNext == 1)
		{
			SampleNext=0;
			MemCim.value++;

			//HAL_TIM_Base_Stop_IT(&htim6);
			//waitSamples = WaveSample();
			waitSamples = WaveSampleIf();
			//HAL_TIM_Base_Start_IT(&htim6);

			SampleNext=1;
		}
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
	}
}

// USB not working
void SystemClock_Config_128MHz(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	/** Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
								|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
}

void SystemClock_Config_Normal(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	//Initializes the CPU, AHB and APB busses clocks

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	//Initializes the CPU, AHB and APB busses clocks

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							   |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
