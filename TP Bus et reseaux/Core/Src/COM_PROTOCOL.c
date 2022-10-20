/*
 * COM_PROTOCOL.c
 *
 *  Created on: 20 oct. 2022
 *      Author: cheik
 */


#include "COM_PROTOCOL.h"

BMP280_U32_t Pressure_value;
BMP280_S32_t Temperature_value;

extern UART_HandleTypeDef huart3;
extern int K;


void comm_protocol(char RX_Pi_buffer[BUFFER_SIZE])
{
	if (strncmp(RX_Pi_buffer,"GET_T",strlen("GET_T"))==0)
	{
		Temperature_value=Read_Temp();
		if (Temperature_value>=0)
		{
			printf("T=+%d.%d_C\r\n",(int)(Temperature_value/100),Temperature_value%100);
		}
		else
		{
			printf("T=%d.%d_C\r\n",(int)(Temperature_value/100),-Temperature_value%100);
		}
	}
	else if (strncmp(RX_Pi_buffer,"GET_P",strlen("GET_P"))==0)
	{
		Pressure_value=Read_Press();
		printf("P=%dPa\r\n",Pressure_value);
	}
	else if (strncmp(RX_Pi_buffer,"SET_K",strlen("SET_K"))==0)
	{
		printf("SET_K Received\r\n");
		if (strncmp(RX_Pi_buffer+strlen("SET_K"),"=",strlen("="))==0)
		{
			K=atoi(RX_Pi_buffer+strlen("SET_K")+strlen("="));
		}
		else
			{
				printf("Command not found\r\n"
						"Please try this syntax\r\n"
						"SET_K=xxxx: to set the K coefficient on xxxx value (in 1/100e)\r\n");
			}
	}
	else if (strncmp(RX_Pi_buffer,"GET_K",strlen("GET_K"))==0)
	{
		printf("GET_K Received\r\n");
	}
	else if (strncmp(RX_Pi_buffer,"GET_A",strlen("GET_A"))==0)
	{
		printf("GET_A Received\r\n");
	}
	else
	{
		printf("Command not found\r\n"
				"Try one of these commands:\r\n"
				"GET_T: to get the temperature value compensated\r\n"
				"GET_P: to get the pressure value compensated\r\n"
				"SET_K=xxxx: to set the K coefficient on xxxx value (in 1/100e)\r\n"
				"GET_K: to get the K coefficient\r\n"
				"GET_A: to get the angle\r\n");
	}
}
