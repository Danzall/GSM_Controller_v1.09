/*
 * RTC.c
 *
 *  Created on: Mar 1, 2018
 *      Author: Jeefo
 */

#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
//#include "r_cg_adc.h"
#include "r_cg_timer.h"
#include "r_cg_wdt.h"
/* Start user code for include. Do not edit comment generated here */
#include "GSM.h"
#include "stdio.h"
#include "RTC.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

unsigned int seconds;
unsigned int minutes;
unsigned int hours;
unsigned int days;
unsigned int weeks;
unsigned int years;
rtc uptime;

void incSeconds(){
	char time[60];
	uptime.seconds++;
	if((uptime.seconds == 20)||(uptime.seconds == 40)||(uptime.seconds == 58)){

	}
	if (uptime.seconds >= 60){
		uptime.seconds = 0;
		uptime.minutes++;
		if((uptime.minutes == 2)||(uptime.minutes == 5)||(uptime.minutes == 10)||(uptime.minutes == 20)||(uptime.minutes == 30)||(uptime.minutes == 40)||(uptime.minutes == 50)||(uptime.minutes == 0)){

		}

	}
	if (uptime.minutes >= 60){
		uptime.minutes = 0;
		uptime.hours++;
		char sms[30];
		//sprintf(sms, "GSM module uptime-%dd:%dh:%dm:%ds", uptime.days,uptime.hours, uptime.minutes, uptime.seconds);
		//strcpy(sms,)
		//sendSMS("0720631005", sms);
		//buildInfo();
	}
	if (uptime.hours >= 24){
		uptime.hours = 0;
		uptime.days++;
	}
	if (uptime.days >= 7){
		uptime.days = 0;
		uptime.weeks++;
	}
	if (uptime.weeks >= 52){
		uptime.weeks = 0;
		uptime.years++;
	}
	//sprintf(time, "years:%.2d weeks:%d days:%d hours:%d minutes:%d seconds:%d", uptime.years, uptime.weeks, uptime.days, uptime.hours, uptime.minutes, uptime.seconds);
	//sendData(time,UART0);
	//sendData("\r\n",UART0);
}

rtc getUptime(){
	return uptime;
}
