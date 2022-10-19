/*
 * BMP.c
 *
 *  Created on: Oct 19, 2022
 *      Author: cheik
 */
#include "BMP.h"
#include "main.h"


extern I2C_HandleTypeDef hi2c1;

uint8_t BMP_Read_Register_ID()
{
	uint16_t DevAddress=BMP_Addr;
	uint8_t TX_Data=BMP_Reg_ID;
	uint8_t RX_Data;
	if (HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &TX_Data, 1, HAL_MAX_DELAY)==HAL_OK)
	{
		if(HAL_I2C_Master_Receive(&hi2c1, DevAddress, &RX_Data, 1, HAL_MAX_DELAY)==HAL_OK)
		{
		}
		else
		{
			printf("Receive Error\n\r");
			return 0;
		}
	}
	else
	{
		printf("Transmit Error\n\r");
		return 0;
	}
	return RX_Data;
}

char BMP_Verify_Id()
{
	if (BMP_Read_Register_ID()==0x58)
	{
		printf("The sensor used is BMP-280\n\r");
		return 1;
	}
	else
	{
		printf("The sensor used is not BMP-280\n\r");
		return 0;
	}
}

void BMP_Config(BMP280 bmp)
{
	uint16_t DevAddress=BMP_Addr;
	uint8_t TX_Data[2];
	uint8_t RX_Data;
	TX_Data[0]=BMP_Reg_AddrCtrlMeas;
	TX_Data[1]=bmp.mode+(bmp.pressure_oversampling<<2)+(bmp.temperature_oversampling<<5);
	if(HAL_I2C_Master_Transmit(&hi2c1, DevAddress, TX_Data, 2, HAL_MAX_DELAY)==HAL_OK)
	{
		if (HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &TX_Data[0], 2, HAL_MAX_DELAY)==HAL_OK)
		{
			if(HAL_I2C_Master_Receive(&hi2c1, DevAddress, &RX_Data, 1, HAL_MAX_DELAY)==HAL_OK)
			{
				if ((RX_Data & MaskMode==bmp.mode)&(((RX_Data&MaskOsrsP)>>2)==bmp.pressure_oversampling)&(((RX_Data&MaskOsrsT)>>5)==bmp.temperature_oversampling))
				{
					printf("BMP280 is configured successfully\n\r");
				}
			}
		}
	}

}
void Calibration(uint8_t calibration_data[Calibration_size])
{
	uint16_t DevAddress=BMP_Addr;
	uint8_t TX_Data=BMP_Reg_calib;
	int i=0;
	if (HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &TX_Data, 1, HAL_MAX_DELAY)==HAL_OK)
	{
		if(HAL_I2C_Master_Receive(&hi2c1, DevAddress, calibration_data, Calibration_size, HAL_MAX_DELAY)==HAL_OK)
		{
			printf("Calibration data received\n\r");

			/*
			for (i=0;i<Calibration_size;i++)
			{
				printf("calib %d = 0x%x\n\r",i,calibration_data[i]);
			}
			 */

		}
	}
}
void Read_Temp_Press(BMP280 bmp)
{
	uint16_t DevAddress=BMP_Addr;
	uint8_t TX_Data=BMP_Reg_DataReadout;
	uint8_t RX_Data[DataReadout_size];
	uint32_t Temperature_value;
	uint32_t Pressure_value;
	//uint8_t Temperature_resolution=Resolution(bmp.temperature_oversampling);
	//uint8_t Pressure_resolution=Resolution(bmp.pressure_oversampling);

	if (HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &TX_Data, 1, HAL_MAX_DELAY)==HAL_OK)
	{
		if(HAL_I2C_Master_Receive(&hi2c1, DevAddress, RX_Data, DataReadout_size, HAL_MAX_DELAY)==HAL_OK)
		{

			Pressure_value=(RX_Data[0]<<12)+(RX_Data[1]<<4)+(RX_Data[2]>>4);
			Temperature_value=(RX_Data[0+3]<<12)+(RX_Data[1+3]<<4)+(RX_Data[2+3]>>4);
			printf("Pressure value = 0x%x\n\r",Pressure_value);
			printf("Temperature value = 0x%x\n\r",Temperature_value);
		}
	}
}
uint8_t Resolution(uint8_t oversampling)
{
	switch (oversampling)
	{
	case OverSampling1:
		return 16;
		break;
	case OverSampling2:
		return 17;
		break;
	case OverSampling4:
		return 18;
		break;
	case OverSampling8:
		return 19;
		break;
	case OverSampling16:
		return 20;
		break;
	}
}





