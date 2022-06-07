/*
 * ADC.c
 *
 *  Created on: Aug 7, 2018
 *      Author: Jeefo
 */
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_adc.h"
#include "r_cg_timer.h"
/* Start user code for include. Do not edit comment generated here */
#include "GSM.h"
#include "RTC.h"
#include "board.h"
#include "stdio.h"
#include "ADC.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

#define aveBuffSize 5

uint16_t adc[3];
uint16_t adc1;
uint16_t adcv;
uint16_t adca[aveBuffSize];
char adcaIndex;
uint16_t adcResult;
char result[10];

void ADC_Service(){
	//R_ADC_Set_OperationOn();
	char temp[60];
	char tempv[60];
	int len;
	R_ADC_Get_Result(adc);
	adc1 = adc[0];
	adcv = adc1*3.57;
	//adc1 = 3;
	//sprintf (temp,"ADC val: %d\r\n", adc1);
	//sendData(temp,UART0);
	//sprintf (temp,"ADC voltage: %d\r\n", adcv);
	//sendData(temp,UART0);
	adcv *= 11.01;
	ADC_Ave();
	//adcv *= 51.7;
	//adcv /= 4.7;
	//sprintf (temp,"Input voltage: %d\r\n", adcv);
	//sendData(temp,UART0);
	//sprintf (temp,"Average Input voltage: %d\r\n", adcResult);
	//sendData(temp,UART0);
	sprintf (tempv,"%d\n", adcResult);
	//sendData(tempv,UART0);
	len = strlen(tempv);
	//sprintf (temp,"ADC string length:%d\n", len);
	//sendData(temp,UART0);
	int temp1;

	for (temp1 = len; temp1 >= 0; temp1--){
		if (temp1 >= 2) result[temp1 + 1] = tempv[temp1];
		else result[temp1] = tempv[temp1];
		if (temp1 == 2) result[temp1] = ',';		//insert char

	}
	result[5] = 'V';
	result[6] = 0;	//terminating char
	//sendData(result,UART0);
	//sendData(temp,UART0);
}

void GetADC(char* data){
	float temp;
	temp = adcResult / 1000;
	//strcpy(data,result);
	int index = 0;
	while (index < 7){
		data[index] = result[index];
		index++;
	}
	//return result;
}

void ADC_Ave(){
	adcResult = 0;
	adca[adcaIndex] = adcv;
	adcaIndex++;
	if (adcaIndex >= aveBuffSize) adcaIndex = 0;
	char temp;
	for (temp = 0; temp < aveBuffSize; temp++){
		adcResult += adca[temp];
	}
	adcResult /= aveBuffSize;
}

