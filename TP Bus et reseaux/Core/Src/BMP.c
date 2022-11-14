/*
 * BMP.c
 *
 *  Created on: Oct 19, 2022
 *      Author: cheik
 */
#include "BMP.h"
#include "main.h"


extern I2C_HandleTypeDef hi2c1;
extern BMP280_S32_t t_fine;
extern uint16_t dig_T1;
extern int16_t dig_T2;
extern int16_t dig_T3;
extern uint16_t dig_P1;
extern int16_t dig_P2;
extern int16_t dig_P3;
extern int16_t dig_P4;
extern int16_t dig_P5;
extern int16_t dig_P6;
extern int16_t dig_P7;
extern int16_t dig_P8;
extern int16_t dig_P9;

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


// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value



BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
	BMP280_S32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((BMP280_S32_t)dig_T1<<1))) * ((BMP280_S32_t)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((BMP280_S32_t)dig_T1)) * ((adc_T>>4) - ((BMP280_S32_t)dig_T1))) >> 12) *
			((BMP280_S32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}
// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa
BMP280_U32_t bmp280_compensate_P_int32(BMP280_S32_t adc_P)
{
	BMP280_S32_t var1, var2;
	BMP280_U32_t p;
	var1 = (((BMP280_S32_t)t_fine)>>1) - (BMP280_S32_t)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((BMP280_S32_t)dig_P6);
	var2 = var2 + ((var1*((BMP280_S32_t)dig_P5))<<1);
	var2 = (var2>>2)+(((BMP280_S32_t)dig_P4)<<16);
	var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((BMP280_S32_t)dig_P2) * var1)>>1))>>18;
	var1 =((((32768+var1))*((BMP280_S32_t)dig_P1))>>15);
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = (((BMP280_U32_t)(((BMP280_S32_t)1048576)-adc_P)-(var2>>12)))*3125;
	if (p < 0x80000000)
	{
		p = (p << 1) / ((BMP280_U32_t)var1);
	}
	else
	{
		p = (p / (BMP280_U32_t)var1) * 2;
	}
	var1 = (((BMP280_S32_t)dig_P9) * ((BMP280_S32_t)(((p>>3) * (p>>3))>>13)))>>12;
	var2 = (((BMP280_S32_t)(p>>2)) * ((BMP280_S32_t)dig_P8))>>13;
	p = (BMP280_U32_t)((BMP280_S32_t)p + ((var1 + var2 + dig_P7) >> 4));
	return p;
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
	uint8_t* p = calibration_data;
	int i=0;
	if (HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &TX_Data, 1, HAL_MAX_DELAY)==HAL_OK)
	{
		if(HAL_I2C_Master_Receive(&hi2c1, DevAddress, calibration_data, Calibration_size, HAL_MAX_DELAY)==HAL_OK)
		{
			printf("Calibration data received\r\n");

			memcpy(&dig_T1, p, 2);
			p += 2;
			memcpy(&dig_T2, p, 2);
			p += 2;
			memcpy(&dig_T3, p, 2);
			p += 2;
			memcpy(&dig_P1, p, 2);
			p += 2;
			memcpy(&dig_P2, p, 2);
			p += 2;
			memcpy(&dig_P3, p, 2);
			p += 2;
			memcpy(&dig_P4, p, 2);
			p += 2;
			memcpy(&dig_P5, p, 2);
			p += 2;
			memcpy(&dig_P6, p, 2);
			p += 2;
			memcpy(&dig_P7, p, 2);
			p += 2;
			memcpy(&dig_P8, p, 2);
			p += 2;
			memcpy(&dig_P9, p, 2);


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
	BMP280_S32_t Temperature_value_compensated;
	BMP280_U32_t Pressure_value_compensated;

	if (HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &TX_Data, 1, HAL_MAX_DELAY)==HAL_OK)
	{
		if(HAL_I2C_Master_Receive(&hi2c1, DevAddress, RX_Data, DataReadout_size, HAL_MAX_DELAY)==HAL_OK)
		{

			Pressure_value=(RX_Data[0]<<12)+(RX_Data[1]<<4)+(RX_Data[2]>>4);
			Temperature_value=(RX_Data[0+3]<<12)+(RX_Data[1+3]<<4)+(RX_Data[2+3]>>4);

			printf("Pressure value = 0x%x\n\r",Pressure_value);
			printf("Temperature value = 0x%x\n\r",Temperature_value);
			Pressure_value_compensated=bmp280_compensate_P_int32(Pressure_value);
			Temperature_value_compensated=bmp280_compensate_T_int32(Temperature_value);
			printf("Pressure value compensated = %d.%d hPa\n\r",(int)(Pressure_value_compensated/100),(Pressure_value_compensated%100));
			printf("Temperature value compensated = %d.%d C\n\r",(int)(Temperature_value_compensated/100),Temperature_value_compensated%100);
		}
	}
}

BMP280_U32_t Read_Press()
{
	uint16_t DevAddress=BMP_Addr;
	uint8_t TX_Data=BMP_Reg_DataReadout;
	uint8_t RX_Data[PressReadout_size];
	uint32_t Pressure_value;
	BMP280_U32_t Pressure_value_compensated;

	if (HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &TX_Data, 1, HAL_MAX_DELAY)==HAL_OK)
	{
		if(HAL_I2C_Master_Receive(&hi2c1, DevAddress, RX_Data, PressReadout_size, HAL_MAX_DELAY)==HAL_OK)
		{
			Pressure_value=(RX_Data[0]<<12)+(RX_Data[1]<<4)+(RX_Data[2]>>4);
			Pressure_value_compensated=bmp280_compensate_P_int32(Pressure_value);
			return Pressure_value_compensated;
		}
		else
		{
			printf("Receive Error\r\n");
			return 0;
		}
	}
	else
	{
		printf("Transmit Error\r\n");
		return 0;
	}
}
BMP280_S32_t Read_Temp()
{
	uint16_t DevAddress=BMP_Addr;
	uint8_t TX_Data=BMP_Reg_DataReadout+TempReadout_size; // To read the temperature registers
	uint8_t RX_Data[TempReadout_size];
	uint32_t Temperature_value;
	BMP280_S32_t Temperature_value_compensated;

	if (HAL_I2C_Master_Transmit(&hi2c1, DevAddress, &TX_Data, 1, HAL_MAX_DELAY)==HAL_OK)
	{
		if(HAL_I2C_Master_Receive(&hi2c1, DevAddress, RX_Data, TempReadout_size, HAL_MAX_DELAY)==HAL_OK)
		{
			Temperature_value=(RX_Data[0]<<12)+(RX_Data[1]<<4)+(RX_Data[2]>>4);
			Temperature_value_compensated=bmp280_compensate_T_int32(Temperature_value);

			return Temperature_value_compensated;
		}
		else
		{
			printf("Receive Error\r\n");
			return 0;
		}
	}
	else
	{
		printf("Transmit Error\r\n");
		return 0;
	}
}




