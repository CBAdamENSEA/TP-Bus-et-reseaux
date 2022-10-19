/*
 * BMP.h
 *
 *  Created on: Oct 19, 2022
 *      Author: cheik
 */

#ifndef INC_BMP_H_
#define INC_BMP_H_

#include "stdio.h"



#define BMP_Addr 0x77<<1



#define BMP_Reg_AddrCtrlMeas 0xF4
#define BMP_Reg_ID 0xD0
#define BMP_Reg_calib 0x88
#define BMP_Reg_DataReadout 0xF7



#define Calibration_size 26
#define DataReadout_size 6




#define SLEEP_MODE 0X00
#define FORCED_MODE 0X01
#define MODE_NORMAL 0x03


#define OverSamplingSkipped 0x00
#define OverSampling1 0x01
#define OverSampling2 0x02
#define OverSampling4 0x03
#define OverSampling8 0x04
#define OverSampling16 0x05

#define MaskOsrsT 0xE0
#define MaskOsrsP 0x1C
#define MaskMode 0x03
#define MaskXLSB 0xF0


typedef struct BMP280{
	uint8_t mode;
	uint8_t pressure_oversampling;
	uint8_t temperature_oversampling;
} BMP280;

typedef uint32_t BMP280_U32_t;
typedef int32_t BMP280_S32_t;

uint8_t BMP_Read_Register_ID();
char BMP_Verify_Id();
void BMP_Config(BMP280 bmp);
void Calibration(uint8_t calibration_data[Calibration_size]);
void Read_Temp_Press(BMP280 bmp);
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);
BMP280_U32_t bmp280_compensate_P_int32(BMP280_S32_t adc_P);




#endif /* INC_BMP_H_ */
