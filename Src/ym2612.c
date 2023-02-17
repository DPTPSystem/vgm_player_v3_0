#include "ym2612.h"
//#include "sega.h"

void SN76489_SendByte(unsigned char byte)
{
	int temp = 0;
	temp = byte>>7;
	temp |= (byte>>6&0x01)<<1;
	temp |= (byte>>5&0x01)<<2;
	temp |= (byte>>4&0x01)<<3;
	temp |= (byte>>3&0x01)<<4;
	temp |= (byte>>2&0x01)<<5;
	temp |= (byte>>1&0x01)<<6;
	temp |= (byte&0x01)<<7;
	SN_PORT(temp);
	SN_WE(GPIO_PIN_RESET);
	DWT_Delay_us(11);
	SN_WE(GPIO_PIN_SET);
}

void SN76489_SendByteFast(unsigned char byte)
{
	int temp = 0;
	temp = byte>>7;
	temp |= (byte>>6&0x01)<<1;
	temp |= (byte>>5&0x01)<<2;
	temp |= (byte>>4&0x01)<<3;
	temp |= (byte>>3&0x01)<<4;
	temp |= (byte>>2&0x01)<<5;
	temp |= (byte>>1&0x01)<<6;
	temp |= (byte&0x01)<<7;

	//SN_PORT(temp);
	//GPIOC->ODR = temp;
	GPIOC->ODR = ((GPIOC->ODR & 0xFF00) | temp);	// Csak az aló 1byte-ot írja felül

	//SN_WE(GPIO_PIN_RESET);
	GPIOB->BSRR = (uint32_t)GPIO_PIN_0 << 16u;

	DWT_Delay_us(13);

	SN_WE(GPIO_PIN_SET);
	//GPIOB->BSRR = GPIO_PIN_0;
}

void PSG_Silence(void)
{
	SN76489_SendByte(0b10011111);	// 0 csatonra Hangerõ néma (0x9F)
	SN76489_SendByte(0b10111111);	// 1 csatonra Hangerõ néma (0xBF)
	SN76489_SendByte(0b11011111);	// 2 csatonra Hangerõ néma (0xDF)
	SN76489_SendByte(0b11111111);	// 3 csatonra Hangerõ néma (0xFF)
	DWT_Delay_us(25);
	YM_PORT(0x00);
	YM_A0(GPIO_PIN_RESET);
	YM_A1(GPIO_PIN_RESET);
	YM_CS(GPIO_PIN_SET);
	YM_WR(GPIO_PIN_SET);
	YM_RD(GPIO_PIN_SET);
	YM_IC(GPIO_PIN_SET);
	YM_IC(GPIO_PIN_RESET);
	DWT_Delay_us(25);
	YM_IC(GPIO_PIN_SET);
	DWT_Delay_us(25);

}
void YM2612_SendFast(unsigned char addr, unsigned char data, unsigned char CMD) //0x52 = A1 LOW, 0x53 = A1 HIGH
{
	//YM_A1(GPIO_PIN_RESET);
	GPIOA->BSRR = (uint32_t)GPIO_PIN_8 << 16u;

    //YM_A0(GPIO_PIN_RESET); 						// A0 LOW
	GPIOB->BSRR = (uint32_t)GPIO_PIN_15 << 16u;

    //YM_CS(GPIO_PIN_RESET);						// CS LOW
	GPIOB->BSRR = (uint32_t)GPIO_PIN_12 << 16u;

    //YM_PORT(addr);
	//GPIOA->ODR = addr;
	GPIOA->ODR = ((GPIOA->ODR & 0xFF00) | addr);	// Csak az aló 1byte-ot írja felül

    if(CMD==0x53){
    	//YM_A1(GPIO_PIN_SET);					// A1 HIGH
    	GPIOA->BSRR = GPIO_PIN_8;
    }
    //DWT_Delay_us(10);
    //YM_WR(GPIO_PIN_RESET);						// WR LOW
    GPIOB->BSRR = (uint32_t)GPIO_PIN_13 << 16u;

    DWT_Delay_us(2);

    //YM_WR(GPIO_PIN_SET);						// WR HIGH
    GPIOB->BSRR = YM_WR_Pin;

    //YM_CS(GPIO_PIN_SET);						// CS HIGH
    GPIOB->BSRR = GPIO_PIN_12;

    //YM_A0(GPIO_PIN_SET);						// A0 HIGH
    GPIOB->BSRR = GPIO_PIN_15;

    //YM_CS(GPIO_PIN_RESET);						// CS LOW
    GPIOB->BSRR = (uint32_t)GPIO_PIN_12 << 16u;

    //YM_PORT(data);
    //GPIOA->ODR = data;
    GPIOA->ODR = ((GPIOA->ODR & 0xFF00) | data);	// Csak az aló 1byte-ot írja felül

    //YM_WR(GPIO_PIN_RESET);						// WR LOW
    GPIOB->BSRR = (uint32_t)GPIO_PIN_13 << 16u;

    DWT_Delay_us(2);

    //YM_WR(GPIO_PIN_SET);						// WR HIGH
    GPIOB->BSRR = GPIO_PIN_13;

    //YM_CS(GPIO_PIN_SET);						// CS HIGH
    GPIOB->BSRR = GPIO_PIN_12;
}

void YM2612_Send(unsigned char addr, unsigned char data, unsigned char CMD) //0x52 = A1 LOW, 0x53 = A1 HIGH
{
	YM_A1(GPIO_PIN_RESET);
    YM_A0(GPIO_PIN_RESET); 						// A0 LOW
    YM_CS(GPIO_PIN_RESET);						// CS LOW
    YM_PORT(addr);
    if(CMD==0x53){
    	YM_A1(GPIO_PIN_SET);					// A1 HIGH
    }
    //DWT_Delay_us(10);
    YM_WR(GPIO_PIN_RESET);						// WR LOW
    DWT_Delay_us(2);
    //HAL_Delay(1);
    YM_WR(GPIO_PIN_SET);						// WR HIGH
    YM_CS(GPIO_PIN_SET);						// CS HIGH
    YM_A0(GPIO_PIN_SET);						// A0 HIGH
    YM_CS(GPIO_PIN_RESET);						// CS LOW
    YM_PORT(data);
    //DWT_Delay_us(10);
    YM_WR(GPIO_PIN_RESET);						// WR LOW
    DWT_Delay_us(2);
    //HAL_Delay(1);
    YM_WR(GPIO_PIN_SET);						// WR HIGH
    YM_CS(GPIO_PIN_SET);						// CS HIGH
}

volatile unsigned int WaveSample(void)
{
	unsigned int wait = 0;
	unsigned char Sample = 0;
	unsigned char YM_address = 0;
	unsigned char YM_data = 0;
	unsigned int i = 0;

	Sample = MemRead(MemCim);
	switch(Sample) //VGM commands
	{
		case 0x4F:
			MemCim.value++;
			SN76489_SendByte(0x06);
			SN76489_SendByte(MemRead(MemCim));
			return 0;
		case 0x50:
			MemCim.value++;
			SN76489_SendByte(MemRead(MemCim));
			return 0;
		case 0x52:
		case 0x53:
			MemCim.value++;
			YM_address = MemRead(MemCim);
			MemCim.value++;
			YM_data = MemRead(MemCim);
			YM2612_Send(YM_address, YM_data, Sample);
			return 0;
			/*case 0x58:
			case 0x59:
				// 0x58, 0x59 CDM - NeoGeo 2610
				MemCim.value++;
				YM_address = MemRead(MemCim);
				MemCim.value++;
				YM_data = MemRead(MemCim);
				YM2610_Send(YM_address, YM_data, Sample);
				return 1;*/
		case 0x61:
			wait = 0;
			for ( i = 0; i < 2; i++ )
			{
				MemCim.value++;
				wait += ( (unsigned int)(MemRead(MemCim)) << ( 8 * i ));
			}
			return wait;
		case 0x62:
			return 735;
		case 0x63:
			return 882;
		case 0x67:
			// PCM adat kezelése
			MemCim.value++;	// Skip 0x66
			MemCim.value++; // Skip data type
			for ( i = 0; i < 4; i++ )
			{
				MemCim.value++;
				PCMDataCount += ( (unsigned long)(MemRead(MemCim)) << ( 8 * i ));
			}
			pcmPosStart = MemCim.value+1;
			if(PCMDataCount > 0 && PCMDataCount <= PCMMaxSize && !PCMLoad)
			{
				for(i=0; i<PCMDataCount; i++)
				{
					MemCim.value++;
					PCMBuff[i] = MemRead(MemCim);
				}
				PCMBuffIndex = 0;
				PCMLoad = 1;
			}
			else
			{
				if(!PCMLoad)
				{
					// PCM kezdõ adat címe
					pcmPosStart = MemCim.value;
					PCMMEMADDR.value = pcmPosStart;
					MemCim.value+=PCMDataCount;
				}
			}
			/*// PCM kezdõ adat címe
			pcmPosStart = MemCim.value;
			PCMMEMADDR.value = pcmPosStart;
			MemCim.value+=PCMDataCount;*/
			return 0;
		case 0x70:
		case 0x71:
		case 0x72:
		case 0x73:
		case 0x74:
		case 0x75:
		case 0x76:
		case 0x77:
		case 0x78:
		case 0x79:
		case 0x7A:
		case 0x7B:
		case 0x7C:
		case 0x7D:
		case 0x7E:
		case 0x7F:
			return (Sample & 0x0F)+1;
		case 0x80:
		case 0x81:
		case 0x82:
		case 0x83:
		case 0x84:
		case 0x85:
		case 0x86:
		case 0x87:
		case 0x88:
		case 0x89:
		case 0x8A:
		case 0x8B:
		case 0x8C:
		case 0x8D:
		case 0x8E:
		case 0x8F:
			wait = Sample & 0x0F;
			YM_address = 0x2A;

			//PCMMEMADDR.value++;
			//YM_data = MemRead(PCMMEMADDR);

			//YM2612_Send(YM_address, YM_data, Sample);

			if(PCMLoad)
			{
				PCMBuffIndex++;
				YM_data = PCMBuff[PCMBuffIndex];
			}
			else
			{
				PCMMEMADDR.value++;
				YM_data = MemRead(PCMMEMADDR);
			}

			YM2612_Send(YM_address, YM_data, Sample);
			return wait;
		case 0xE0:
			if(PCMLoad)
			{
				PCMBuffIndex = 0;
				for (i = 0; i < 4; i++ )
				{
					MemCim.value++;
					PCMBuffIndex += ( (unsigned int)(MemRead(MemCim)) << ( 8 * i ));
				}
			}
			else
			{
				PCMMEMADDR.value = pcmPosStart;
				for (i = 0; i < 4; i++ )
				{
					MemCim.value++;
					PCMMEMADDR.value += ( (unsigned int)(MemRead(MemCim)) << ( 8 * i ));
				}
			}

			/*PCMMEMADDR.value = pcmPosStart;
			for (i = 0; i < 4; i++ )
			{
				MemCim.value++;
				PCMMEMADDR.value += ( (unsigned int)(MemRead(MemCim)) << ( 8 * i ));
			}*/
			return 0;
		case 0x66:
			MemCim.value=63;
			PCMMEMADDR.value=0;
			PCMBuffIndex=0;
			PCMDataCount=0;
			PCMLoad=0;
			return 0;
		default:
			return 0;
	}
}

volatile unsigned int WaveSampleIf(void)
{
	unsigned int wait = 0;
	unsigned char Sample = 0;
	unsigned char YM_address = 0;
	unsigned char YM_data = 0;
	unsigned int i = 0;

	Sample = MemReadFast(MemCim);
	if(Sample == 0x4F || Sample == 0x50)
	{
		MemCim.value++;
		if(Sample == 0x4F) SN76489_SendByteFast(0x06);
		SN76489_SendByteFast(MemReadFast(MemCim));
		return  0;
	}
	else if(Sample == 0x52 || Sample == 0x53)
	{
		MemCim.value++;
		YM_address = MemReadFast(MemCim);
		MemCim.value++;
		YM_data = MemReadFast(MemCim);
		YM2612_SendFast(YM_address, YM_data, Sample);
		return  0;
	}
	/*case 0x58:
	case 0x59:
		// 0x58, 0x59 CDM - NeoGeo 2610
		MemCim.value++;
		YM_address = MemReadFast(MemCim);
		MemCim.value++;
		YM_data = MemReadFast(MemCim);
		YM2610_Send(YM_address, YM_data, Sample);
		return  1;*/
	else if(Sample == 0x61)
	{
		wait = 0;
		for ( i = 0; i < 2; i++ )
		{
			MemCim.value++;
			wait += ( (unsigned int)(MemReadFast(MemCim)) << ( 8 * i ));
		}

		//if(VGMStream.DataLength == 0)	// Normál idõzítés
		//{
			return  wait;
		/*}
		else	// Van PCM Stream
		{
			if(VGMStream.DataLength > wait)
			{
				 // PCM hosszabb mint a várakozás
				//x = VGMStream.DataLength - wait;
			}
			else
			{
				// itt a várakozási idõ nagyobb mint a PCM hossza
				x = wait - VGMStream.DataLength;
			}
			PCMMEMADDR.value = VGMStream.PCMStartAdr;
			Time = (double)(1000000/VGMStream.Freq) / 1.5;
			for(i=0; i < x; i++)
			{
				YM_address = VGMStream.PCMCMD;
				YM_data = MemReadFast(PCMMEMADDR);
				PCMMEMADDR.value++;
				YM2612_Send(YM_address, YM_data, Sample);
				// Idõzítés ide jön.
				DWT_Delay_us(Time);	// 1*1.5uS
			}
			VGMStream.PCMStartAdr += PCMMEMADDR.value;
			return  0;
		}*/
	}
	else if(Sample == 0x62)
	{
		return  735;
	}
	else if(Sample == 0x63)
	{
		return  882;
	}
	else if(Sample == 0x67)
	{
		// PCM adat kezelése
		MemCim.value++;	// Skip 0x66
		MemCim.value++; // Skip data type
		for ( i = 0; i < 4; i++ )
		{
			MemCim.value++;
			PCMDataCount += ( (unsigned long)(MemReadFast(MemCim)) << ( 8 * i ));
		}
		pcmPosStart = MemCim.value+1;
		if(PCMDataCount > 0 && PCMDataCount <= PCMMaxSize && !PCMLoad)
		{
			for(i=0; i<PCMDataCount; i++)
			{
				MemCim.value++;
				PCMBuff[i] = MemReadFast(MemCim);
			}
			PCMBuffIndex = 0;
			PCMLoad = 1;
			PCMJumpIndex = 0;
		}
		else
		{
			if(!PCMLoad)
			{
				// PCM kezdõ adat címe
				pcmPosStart = MemCim.value;
				PCMMEMADDR.value = pcmPosStart;
				MemCim.value+=PCMDataCount;
			}
		}
		return  0;
	}
	else if(Sample >= 0x70 && Sample <= 0x7F)
	{
		return  (Sample & 0x0F)+1;
	}
	else if(Sample >= 0x80 && Sample <= 0x8F)
	{
		wait = Sample & 0x0F;
		YM_address = 0x2A;

		if(PCMLoad)
		{
			PCMBuffIndex++;
			YM_data = PCMBuff[PCMBuffIndex];
		}
		else
		{
			PCMMEMADDR.value++;
			YM_data = MemReadFast(PCMMEMADDR);
		}

		YM2612_SendFast(YM_address, YM_data, Sample);
		return  wait;
	}
	/*else if(Sample == 0x93)	// Stream start
	{
		//       ss aa aa aa aa mm ll ll ll ll
		// 0x93: 00 00 00 00 00 01 CB 07 00 00 | 61 B0 52
		// 0x93: 00 DB 07 00 00 01 98 07 00 00 | 61 B0 52
		MemCim.value++;		// Stream ID

		// Data Start offset 4byte
		VGMStream.DataStartOffset = 0;
		for ( i = 0; i < 4; i++ )
		{
			MemCim.value++;
			VGMStream.DataStartOffset += ( (unsigned long)(MemReadFast(MemCim)) << ( 8 * i ));
		}
		MemCim.value++;
		// Length Mode (ha 1 van akkor oké, ha más akkor nem jó, hossza eltér a byte-tól)
		VGMStream.LenghtMode = MemReadFast(MemCim);

		// Data Length 4 byte
		VGMStream.DataLength = 0;
		for ( i = 0; i < 4; i++ )
		{
			MemCim.value++;
			VGMStream.DataLength += ( (unsigned long)(MemReadFast(MemCim)) << ( 8 * i ));
		}
		// DataLength*4
		VGMStream.DataLength = VGMStream.DataLength;
		// PCM kezdõ címe + Data Start offset
		VGMStream.PCMStartAdr = pcmPosStart + VGMStream.DataStartOffset;

		return  0;
	}
	else if(Sample == 0x90)	// inicializáljuk a PCM stream-et
	{
		MemCim.value++;		// Stream ID lekérése, nem kell ( 0x00 )
		VGMStream.StreamID = MemReadFast(MemCim);
		MemCim.value++;		// Chip típusa, ( 0x02 - YM2612 )
		VGMStream.ChipType = MemReadFast(MemCim);
		MemCim.value++;		// Port	( 0x00 )
		VGMStream.PCMPort = MemReadFast(MemCim);
		MemCim.value++;		// Parancs mentése ( 0x2A )
		VGMStream.PCMCMD = MemReadFast(MemCim);
		return  0;
	}
	else if(Sample == 0x91)	// Beállítjuk a stream adatot
	{
		MemCim.value++;		// Stream ID ( 0x00 )
		MemCim.value++;		// Data Bank ID (nincs tömörítve) ( 0x00 )
		VGMStream.DataBankID = MemReadFast(MemCim);
		MemCim.value++;		// Lépésméret, hány adatot hagyjon ki minden írás után (álltalában 1-et)
		VGMStream.StepSize = MemReadFast(MemCim);
		MemCim.value++;		// Adat eltolás hozzáadva a start offset-hez
		VGMStream.DataShift = MemReadFast(MemCim);
		return  0;
	}
	else if(Sample == 0x92)	// Beállítjuk a stream frekvenciát
	{
		MemCim.value++;		// Stream ID ( 0x00 )
		// Frekencia 4 byte
		VGMStream.Freq = 0;
		for ( i = 0; i < 4; i++ )
		{
			MemCim.value++;
			VGMStream.Freq += ( (unsigned long)(MemReadFast(MemCim)) << ( 8 * i ));
		}
		return  0;
	}*/
	else if(Sample == 0xE0)
	{
#ifdef PCMJump
		MemCim.value+=4;
		if(PCMLoad)
		{
			PCMBuffIndex = JumpTableE0[PCMJumpIndex++];
		}
		else
		{
			PCMMEMADDR.value = pcmPosStart;
			PCMMEMADDR.value += JumpTableE0[PCMJumpIndex++];
		}
#else
		if(PCMLoad)
		{
			PCMBuffIndex = 0;
			for (i = 0; i < 4; i++ )
			{
				MemCim.value++;
				PCMBuffIndex += ( (unsigned int)(MemReadFast(MemCim)) << ( 8 * i ));
			}
		}
		else
		{
			PCMMEMADDR.value = pcmPosStart;
			for (i = 0; i < 4; i++ )
			{
				MemCim.value++;
				PCMMEMADDR.value += ( (unsigned int)(MemReadFast(MemCim)) << ( 8 * i ));
			}
		}
#endif
		return  0;
	}
	else if(Sample == 0x66)
	{
		MemCim.value = 63;
		if(VGMVersion==0x151) MemCim.value = 127;
		PCMMEMADDR.value=0;
		PCMBuffIndex=0;
		PCMDataCount=0;
		PCMJumpIndex=0;
		PCMLoad=0;
		return  0;
	}
	else
	{
		return  0;
	}
}

void PCMJumpSave(void)
{
	unsigned char Sample = 0;
	unsigned int i = 0;
	unsigned char Flag = 1;

	while(Flag)
	{
		MemCim.value++;
		Sample = MemReadFast(MemCim);
		switch(Sample) //VGM commands
		{
			case 0x4F:
				MemCim.value++;
				break;
			case 0x50:
				MemCim.value++;
				break;
			case 0x52:
			case 0x53:
				MemCim.value++;
				MemCim.value++;
				break;
			case 0x61:
				MemCim.value++;
				MemCim.value++;
				break;
			case 0x62:
			case 0x63:
				break;
			case 0x67:
				// PCM adat kezelése
				MemCim.value++;	// Skip 0x66
				MemCim.value++; // Skip data type
				for ( i = 0; i < 4; i++ )
				{
					MemCim.value++;
					PCMDataCount += ( (unsigned long)(MemReadFast(MemCim)) << ( 8 * i ));
				}
				MemCim.value+=PCMDataCount;
				break;
			case 0x70:
			case 0x71:
			case 0x72:
			case 0x73:
			case 0x74:
			case 0x75:
			case 0x76:
			case 0x77:
			case 0x78:
			case 0x79:
			case 0x7A:
			case 0x7B:
			case 0x7C:
			case 0x7D:
			case 0x7E:
			case 0x7F:
				break;
			case 0x80:
			case 0x81:
			case 0x82:
			case 0x83:
			case 0x84:
			case 0x85:
			case 0x86:
			case 0x87:
			case 0x88:
			case 0x89:
			case 0x8A:
			case 0x8B:
			case 0x8C:
			case 0x8D:
			case 0x8E:
			case 0x8F:
				break;
			case 0xE0:
				for (i = 0; i < 4; i++ )
				{
					MemCim.value++;
					JumpTableE0[PCMBuffIndex] += ( (unsigned int)(MemReadFast(MemCim)) << ( 8 * i ));
				}
				PCMBuffIndex++;
				break;
			case 0x66:
				MemCim.value=64;
				PCMDataCount=0;
				PCMBuffIndex=0;
				Flag = 0;
				break;
			default:
				break;
		}
	}
}
