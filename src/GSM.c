/*
 * GSM.c
 *
 *  Created on: Feb 22, 2018
 *      Author: Jeefo
 */
/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
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
#include "board.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

#define recBuffSize 250
#define procBuffSize 170
char recBuff[recBuffSize];
char procBuff[procBuffSize];
int recBuffPointerWrite = 0;
int recBuffPointerRead = 0;
int procBuffpointer = 0;
int intFlag0 = 0;
int intFlag1 = 0;
//int gsmState = 1;		// no restart
int gsmState = 20;	//restart
SMSflags smsFlags;
SMSinfo smsInfo;
GSMinfo gsmInfo;
char SMScontent[160];
char GPRSresult[40];
char quality[10];
int gsmTimer;
int gprsTimer;
int errorTimer = 0;
int errorCounter = 0;
int restart = 1;
int restartTimer = 0;

void pec_Update(uint8_t* pec, uint8_t index)
{
	char index1;
static const  uint8_t lookup[256][3] =		//256 rows each having 3 columns
{
"21", "22", "23", "24", "25", "26", "27", "28",
"29", "2A", "2B", "2C", "2D", "2E", "2F",
"30", "31", "32", "33", "34", "35", "36", "37",
"38", "39", "3A", "3B", "3C", "3D", "3E", "3F",
"40", "41", "42", "43", "44", "45", "46", "47",
"48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
"50", "51", "52", "53", "54", "55", "56", "57",
"58", "59", "5A", "5B", "5C", "5D", "5E", "5F",
"60", "61", "62", "63", "64", "65", "66", "67",
"68", "69", "6A", "6B", "6C", "6D", "6E", "6F",
"70", "71", "72", "73", "74", "75", "76", "77",
"78", "79", "7A", "7B", "7C", "7D", "7E", "7F"

};
//pec = lookup[pec];
index1 = index - 33;

*pec = lookup[index1][0] ;
//sendByte(* pec, UART0);
pec++;
*pec = lookup[index1][1] ;
//sendByte(* pec, UART0);
pec++;
*pec = 0 ;
//strcpy(pec,lookup[index1]);
/*char temp[15];
sprintf (temp,"\nIndex:%i\r\n", index1);
sendData(temp,UART0);*/
//sendData(pec,UART0);
//sendData("\r\n",UART0);
//return pec;
}

void GSM_Service(){
	char count[15];

	sprintf(count, "%d", gsmTimer);
	sprintf(count, "%d", gsmState);
	//sendData(count,UART0);
	//sendData("\r\n",UART0);
	/*if (smsFlags.context == 1) gprsTimer++;
	if (gprsTimer >= 10) {

		sprintf(count, "GPRS:%d\r\n", gprsTimer);
		sendData(count,UART0);
		if ((smsFlags.gprsPending == 0)&&(smsFlags.gprsActive == 0)&&(smsFlags.context == 1)){
			sendData("Open context\r\n",UART0);
			gsmState = 8;	//open context
		}
		//else gsmState = 9;		//close conext
		gprsTimer = 0;
	}*/

	/*if (gsmTimer >= 10){

		gsmTimer = 0;

		gsmState = 5;
	}*/
	/*if (restart == 1){
		sprintf(count, "Restart-%d", restartTimer);
		sendData(count,UART0);
		sendData("\r\n",UART0);

		if (restartTimer <= 4) restartTimer++;
		else restart = 0;
		GSM_ON = 1;
	}
	else{
		GSM_ON = 0;
		//gsmState = 1;
	}*/
	errorTimer++;
	switch (gsmState){

	case 1:
		//smsFlags.send = 1;	//test sms function
		sendData("Search\r\n",UART0);
		sendData("AT+CREG?\r\n",UART1);

		break;
	case 2:
		sendData("SMS config\r\n",UART0);
		sendData("AT+CMGF=1\r\n",UART1);
		smsFlags.config = 1;
		smsFlags.signal = 1;
		gsmState = 3;
		break;
	case 3:
		sendData("AT+CGDCONT=1,\"IP\",\"INTERNET\"\r\n",UART1);
		strcpy(gsmInfo.url,"www.google.com");
		strcpy(gsmInfo.urlport,"80");
		gsmState = 4;
		break;

	case 4:
		sendData("AT+CGDCONT?\r\n",UART1);
		gsmState = 5;
		break;
	case 5:
		sendData("AT+CMGL=\"ALL\"\r\n",UART1);
		smsInfo.count = 0;
		gsmState = 13;		//check signal
		break;
	case 6:
		sendSMS(smsInfo.recMSISDN, smsInfo.build);
		//sendSMS("0720631005", "TEST");
		gsmState = 0;
		break;
	case 7:
		sendData("Insert SMS text\r\n",UART0);
		sendData(SMScontent,UART1);
		//sendByte('1A', UART1);
		sendData("\032\r", UART1);		//1A HEX
		smsFlags.reply = 0;
		gsmState = 0;
		break;
	case 8:
		sendData("AT+MIPCALL=1\r\n",UART1);
		smsFlags.gprsPending = 1;
		gsmInfo.socket = 0;
		gsmState = 0;
		//gsmState = 10;
		break;
	case 9:
		sendData("AT+MIPCALL=0\r\n",UART1);
		smsFlags.gprsActive = 0;
		gsmState = 0;
		break;
	case 10:
		//sendData("AT+MIPOPEN=1,10,\"139.130.4.5\",80,0\r\n",UART1);		//connect to remote server
		if (gsmInfo.socket == 0) sendData("AT+MIPOPEN=1,10,\"216.58.223.4\",80,0\r\n",UART1);		//connect to remote server
		else sendData("Socket open\r\n",UART1);
		//sendData("AT+MIPOPEN=?\r\n",UART1);
		//gsmState = 11;
		gsmState = 16;
		gsmState = 0;
		break;
	case 11:
		sendData("AT+MIPCLOSE=1\r\n",UART1);
		gsmState = 0;
		break;
	case 12:
		smsFlags.send = 1;
		sendData("AT+CMGD=",UART1);
		sendData(smsInfo.index,UART1);
		//sendData("4",UART1);
		sendData("\r\n",UART1);
		smsFlags.del = 0;
		gsmState = 0;
		break;
	case 13:
		sendData("AT+CSQ?\r\n",UART1);
		if (smsFlags.gprsActive == 0) gsmState = 8;
		else gsmState = 10;
		//buildInfo();
		break;
	case 14:
		sendData("AT+MIPOPEN=1,1100,\"0.0.0.0\",0,1\r\n",UART1);	//listen for incoming connections
		//sendData("AT+MIPOPEN=?\r\n",UART1);
		gsmState = 0;
		break;
	case 15:
		sendData("AT+MIPSETS=1,200\r\n",UART1);	//listen for incoming connections
		//sendData("AT+MIPOPEN=?\r\n",UART1);
		gsmState = 16;
		break;
	case 16:
		sendData("AT+MIPOPEN?\r\n",UART1);	//listen for incoming connections
		//sendData("AT+MIPOPEN=?\r\n",UART1);
		gsmState = 0;
		break;
	case 17:
		//sendData("AT+MIPSEND=1,\"HEAD / HTTP/1.1\"\r\n",UART1);	//listen for incoming connections
		//sendData("AT+MIPSEND=1,\"444456\"\r\n",UART1);	//listen for incoming connections

		//char temp[60];
		//strcpy(sendGPRS,"AT+MIPSEND=1,\"");
		//strcat(sendGPRS,(char*)GPRSresult);
		//strcat(sendGPRS,"\"\r\n");
		//sendData(sendGPRS,UART1);
		sendData("AT+MIPSEND=1,\"",UART1);
		//sendData(GPRSresult,UART1);
		GPRS_Send("GET / HTTP/1.1");
		sendData("\"\r\n",UART1);
		//sendData("AT+MIPOPEN=?\r\n",UART1);
		gsmState = 18;
		break;
	case 18:
		sendData("AT+MIPPUSH=1\r\n",UART1);	//listen for incoming connections
		//sendData("AT+MIPOPEN=?\r\n",UART1);
		gsmState = 0;
		break;
	case 20:
		sendData("GSM power off\r\n",UART0);
		GSM_PWR = 0;
		GSM_ON = 1;
		smsFlags.config = 0;
		sprintf(count, "Restart-%d", restartTimer);
		sendData(count,UART0);
		sendData("\r\n",UART0);
		restartTimer++;
		if (restartTimer >= 4){
			restartTimer = 0;
			gsmState++;
		}
		break;
	case 21:
		sendData("GSM power on\r\n",UART0);
		GSM_PWR = 1;
		gsmState++;
		smsFlags.restartCount++;
		break;
	case 22:
		sendData("GSM enable\r\n",UART0);
		restartTimer++;
		if (restartTimer >= 3){
			gsmState = 1;
			GSM_ON = 0;
		}
		break;
	default:

		break;
	}
	//if (gsmInfo.socket == 1) gsmState = 17;		//if context is open send gprs data

	if (smsFlags.prompt == 1){

		smsFlags.prompt = 0;
	}
	if ((gsmTimer < 20)&&(smsFlags.signal == 1)){			//restarts cycle
		gsmTimer++;
	}
	else {
		gsmTimer = 0;
		if (smsFlags.config == 1) gsmState = 5;
	}
	if (errorTimer >= 55){
		errorTimer = 0;
		//strcpy(smsInfo.recMSISDN,"0720631005");
		//buildInfo();
		//smsFlags.send = 1;
		//restart = 1;
		//restartTimer = 0;
		gsmState = 20;
	}
}

void sendData(char* str,Comms d){
	//int length;
	//length = strlen(str);
	//R_UART1_Send(str, length);
	while (* str != 0){
		sendByte(* str, d);
		str++;
	}
}
void tx0Done(){
	intFlag0 = 1;
}

void tx1Done(){
	intFlag1 = 1;
}

void sendByte(char send, Comms in){
	if (in == UART0){
		TXD0 = send;
		while (intFlag0 != 1);
		intFlag0 = 0;
	}
	else if (in == UART1){
		TXD1 = send;
		while (intFlag1 != 1);
		intFlag1 = 0;
	}
}

void sendSMS(char* num, char* msg){
	if (smsFlags.signal == 0) return;
	if (smsFlags.config == 0) return;
	//char sms
	sendData("Send SMS\r\n",UART0);
	sendData("AT+CMGS=\"",UART1);
	sendData(num,UART1);
	sendData("\"\r\n",UART1);
	//sendData("AT+CMGS=\"0720631005\"\r\n",UART1);
	strcpy(SMScontent,msg);
	smsFlags.send = 0;
}

void recData(){
	P1_bit.no0 ^= 1;
	while (recBuffPointerWrite != recBuffPointerRead){
		if (recBuffPointerWrite != recBuffPointerRead){
			procBuff[procBuffpointer] = recBuff[recBuffPointerRead];
			recBuffPointerRead++;
			if (recBuffPointerRead > recBuffSize) recBuffPointerRead = 0;

			procBuffpointer++;
			if (procBuffpointer > procBuffSize) procBuffpointer = 0;

			if (procBuff[procBuffpointer - 1] == '>') gsmState = 7;

			if (procBuff[procBuffpointer - 1] == 0x0A){
				procBuff[procBuffpointer - 1] = 0;
				procBuffpointer = 0;
				//sendData(procBuff,UART0);
				//sendData("\r\n",UART0);
				//sendData("0x0A\r\n",UART0);
			}
			if (procBuff[procBuffpointer - 1] == 0x0D){
				procBuff[procBuffpointer - 1] = 0;
				procBuffpointer = 0;
				sendData(procBuff,UART0);
				sendData("\r\n",UART0);
				procData();
				//sendData("0x0D\r\n",UART0);
				//sendData("0x0D\r\n",UART0);
			}
		}
	}
}

void procData(){		//process line
	if (smsFlags.content == 1){
		smsFlags.content = 0;
		smsFlags.del = 1;
		//if(strncmp((char*)smsInfo.index,"1",2)==0){
			sendData("Got index: ",UART0);
			sendData(smsInfo.index,UART0);
			sendData("\r\n",UART0);
			strcpy(smsInfo.content1,procBuff);	//store SMS content
			procSMS();
		/*}
		if(strncmp((char*)smsInfo.index,"2",2)==0){
			sendData("Got index2\r\n",UART0);
			strcpy(smsInfo.content2,procBuff);
			procSMS();
		}*/
		//sendData("SMS conent:",UART0);
		//sendData(smsInfo.content,UART0);
		//sendData("\r\n",UART0);
		sendData("Got SMS content\r\n",UART0);
	}
	if(strncmp((char*)procBuff,"+CREG: 0,1",10)==0)gsmState = 2;
	else if(strncmp((char*)procBuff,"+CSQ",4)==0)getSignal();
	else if(strncmp((char*)procBuff,"OK",3)==0)	OK();
	else if(strncmp((char*)procBuff,"+MIPCALL",8)==0)Context();
	else if(strncmp((char*)procBuff,"+MIPOPEN",8)==0)Socket1();
	else if(strncmp((char*)procBuff,"+MIPSTAT",8)==0)DataStatus();
	else if(strncmp((char*)procBuff,"6",1)==0)DataDecrypt();
	else if(strncmp(procBuff,"+SIM READY",10)==0)sendData("SIM is ready!!!\r\n",UART0);
	else if(strncmp((char*)procBuff,"+CMGL",5)==0)	if (smsFlags.reply == 0) GotSMS();


	else if(strncmp((char*)procBuff,"+CGDCONT",8)==0)checkGPRS();
	else if(strncmp((char*)procBuff,"ERROR",5)==0)gsmState = 20;	//restart

	//sendData(procBuff,UART0);
	//sendData("\r\n",UART0);
	//else if(strncmp((char*)procBuff,"+SIM READY1",8)==0);		//unsolicited
}

void Error(){
	errorCounter++;
	if (errorCounter >= 3){
		errorCounter = 0;
		gsmState = 20;
	}
}

void getSignal(){
	myStrSection(procBuff, quality,3,' ',1);
	char temp1[25];
	int temp2;
	int temp3;
	temp2 = atoi(quality);
	temp2 *= 2;
	temp3 = 113 - temp2;
	sprintf (quality," -%ddBm", temp3);
	//sendData("Signal ",UART0);
	//sendData(quality,UART0);
	//sendData("\r\n",UART0);

	//sendData("\r\nSignal-",UART0);
	//sendData(quality,UART0);
	//sendData("\r\n",UART0);
	/*rtc temp;
	temp = getUptime();

	sprintf (temp1,"Uptime minutes: %.2dd %.2dh %.2dm", temp.days, temp.hours, temp.minutes);
	sendData(temp1,UART0);
	sendData("\r\n",UART0);*/
}

void procSMS(){
	sendData("Process SMS\r\n",UART0);
	//caps((char*)smsInfo.content1);		//convert to capital letters
	if(strncmp((char*)smsInfo.content1,"info",4)==0){
		buildInfo();
		RELAY = 1;
	}
	if(strncmp((char*)smsInfo.content1,"test",4)==0){
		buildInfo();
		RELAY = 0;
	}

	gsmState = 12;			//del sms
}

void caps(char* str){
	while (*str != 0){
		if ((*str >= 97) && (*str <= 122)){
			*str =- 32;
			str++;
		}
	}
}

void buildInfo(){
	sendData("Build INFO SMS\r\n",UART0);
	//strcpy(SMScontent,"Powerbox is active\n ");
	strcpy(SMScontent,"Unit is active\n");
	//sendData(quality,UART0);
	//sendData("\r\n",UART0);

	char temps[20];
	char adcv[10];
	GetADC(adcv);
	sprintf(temps,"Voltage: %s\n",adcv);
	strcat(SMScontent,temps);
	sprintf(temps,"IP: %s\n",smsInfo.IP);
	strcat(SMScontent,temps);
	sprintf(temps,"Restarts: %i\n",smsFlags.restartCount);
	strcat(SMScontent,temps);
	quality[0] = 0x20;
	sprintf(temps,"Signal:%s\n",quality);
	strcat(SMScontent,temps);
	sprintf(temps,"URL:%s:%s\n",gsmInfo.url, gsmInfo.urlport);
	strcat(SMScontent,temps);
	//strcat(SMScontent,quality);
	//strcat(SMScontent,"\n");
	rtc time;
	time = getUptime();
	char temp1[30];
	sprintf (temp1,"Uptime: %.2dw %.2dd %.2dh %.2dm %.2ds\n", time.weeks, time.days, time.hours, time.minutes, time.seconds);
	strcat(SMScontent,temp1);
	strcpy (smsInfo.build, SMScontent);
	//sendData("\r\nSMS content:",UART0);
	//sendData(SMScontent,UART0);
	//sendData("\r\n",UART0);
	sendData("\r\nSMSinfo build:",UART0);
	sendData(smsInfo.build,UART0);
	sendData("\r\n",UART0);
	smsFlags.reply = 1;			//generates reply sms
	strcpy(smsInfo.recMSISDN,"0720631005");	//debug for sms from rtc
}

void checkGPRS(){
	char section[15];
	myStrSection(procBuff, smsInfo.IP,10,'"',5);
	if(strncmp((char*)procBuff,"0.",2)==0)smsFlags.gprsActive = 0;
	else smsFlags.gprsActive = 1;
	/*if (strlen(smsInfo.IP) > 3){

		smsFlags.gprsActive = 1;
	}*/
}

void Context(){
	char section[15];
	myStrSection(procBuff, smsInfo.IP,15,' ',1);
	if (strlen(smsInfo.IP) > 3){
		smsFlags.gprsActive = 1;		//if gotten IP
		smsFlags.gprsPending = 0;
		gsmState = 10;
		sendData(smsInfo.IP,UART0);
		sendData("\r\n",UART0);
		sendData("Context opened\r\n",UART0);
	}
	else{
		smsFlags.gprsActive = 0;
		sendData("Context closed\r\n",UART0);
	}
}

/*void Socket(){
	char temp[10];
	char temp1[5];
	int index = 0;
	gsmInfo.socket1 = 1;
	gsmInfo.socket2 = 1;
	gsmInfo.socket3 = 1;
	gsmInfo.socket4 = 1;
	gsmInfo.socket = 0;
	myStrSection(procBuff, temp ,15,' ',1);
	for (index = 0; index < 4; index++){
		myStrSection(temp, temp1 ,15,',',index);
		if (strncmp((char*)temp1,"1",1)==0) gsmInfo.socket1 = 0;
		if (strncmp((char*)temp1,"2",1)==0) gsmInfo.socket2 = 0;
		if (strncmp((char*)temp1,"3",1)==0) gsmInfo.socket3 = 0;
		if (strncmp((char*)temp1,"4",1)==0) gsmInfo.socket4 = 0;
	}
	char sock[20];
	sprintf(sock,"Socket status: 1:%d 2:%d 3:%d 4:%d \r\n",gsmInfo.socket1, gsmInfo.socket2, gsmInfo.socket3, gsmInfo.socket4);
	sendData(sock,UART0);
	if ((gsmInfo.socket1 == 1)||(gsmInfo.socket2 == 1)||(gsmInfo.socket3 == 1)||(gsmInfo.socket4 == 1)){
		gsmInfo.socket = 1;
		gsmState = 17;
	}
	else gsmInfo.socket = 0;
}*/

void Socket1(){
	char temp[10];
	char temp1[5];
	char temp2[5];
	int index = 0;
	gsmInfo.socket1 = 0;
	gsmInfo.socket2 = 0;
	gsmInfo.socket3 = 0;
	gsmInfo.socket4 = 0;
	gsmInfo.socket = 0;
	myStrSection(procBuff, temp ,15,' ',1);		//1,1
	myStrSection(temp, temp1 ,15,',',0);		//socket
	myStrSection(temp, temp2 ,15,',',1);		//socket status
	if (strncmp((char*)temp2,"1",1)==0){
		if (strncmp((char*)temp1,"1",1)==0) gsmInfo.socket1 = 1;
		if (strncmp((char*)temp1,"2",1)==0) gsmInfo.socket2 = 1;
		if (strncmp((char*)temp1,"3",1)==0) gsmInfo.socket3 = 1;
		if (strncmp((char*)temp1,"4",1)==0) gsmInfo.socket4 = 1;
	}
	char sock[20];
	sprintf(sock,"Socket status: 1:%d 2:%d 3:%d 4:%d \r\n",gsmInfo.socket1, gsmInfo.socket2, gsmInfo.socket3, gsmInfo.socket4);
	sendData(sock,UART0);
	if ((gsmInfo.socket1 == 1)||(gsmInfo.socket2 == 1)||(gsmInfo.socket3 == 1)||(gsmInfo.socket4 == 1)){
		gsmInfo.socket = 1;
		gsmState = 17;			//send data
	}
	else gsmInfo.socket = 0;
}

void GotSMS(){
	char section[15];
	char tempData[90];
	strcpy(tempData,procBuff);
	//sendData("SMS info:\r\n",UART0);
	myStrSection(procBuff, section,10,',',0);
	myStrSection(section, smsInfo.index,3,' ',1);
	myStrSection(procBuff, smsInfo.recMSISDN,20,'"',3);
	strcpy(smsInfo.recMSISDN,"0720631005");					//return SMS to default number
	//sendData(smsInfo.recMSISDN,UART0);
	sendData(smsInfo.index,UART0);
	sendData("\r\n",UART0);
	//sendData(smsInfo.index,UART0);
	//sendData("\r\n",UART0);
	smsFlags.content = 1;
}

void OK(){
	char count[20];
	sendData("Got OK\r\n",UART0);
	errorTimer = 0;
	if (smsFlags.reply == 1){
	//if (smsFlags.send == 1){
		if (smsFlags.config == 1) gsmState = 6;
		sendData("Send OK\r\n",UART0);
	}
	if (gsmState == 5) smsFlags.context = 1;
	if (smsFlags.del == 1) gsmState = 12;
	//if (gsmState == 12) gsmState = 6;
	//sprintf(count, "GSM state=%d", gsmState);
	//sendData(count,UART0);
	//sendData("\r\n",UART0);
}

void GPRS_Send(char* gprs){
	int len;
	len = strlen(gprs);
	char temp[30];
	sprintf (temp,"Len: %d\r\n", len);
	sendData(temp,UART0);
	int ind = 0;

	uint8_t byt[5];
	strcpy((char*)GPRSresult,"");
	while (len > 0){
		pec_Update(byt,*gprs);
		gprs++;
		strcat(GPRSresult,byt);
		len--;
		ind++;
	}
	strcat(GPRSresult,"0A0D");
	sendData(GPRSresult,UART1);
	//sendData("\r\n",UART1);
	sendData(GPRSresult,UART0);
	sendData("\r\n",UART0);
	GPRSresult[0] = 0;
}

void DataStatus(){
	char temp[10];
	char temp1[5];
	char temp2[5];
	myStrSection(procBuff, temp ,15,' ',1);		//1,1
	myStrSection(temp, temp1 ,15,',',0);		//socket
	myStrSection(temp, temp2 ,15,',',1);		//socket data status
	if (strncmp((char*)temp2,"0",1)==0){

			sendData("ACK indication\r\n",UART0);
		}
	if (strncmp((char*)temp2,"1",1)==0){
		gsmInfo.socket = 0;
		sendData("Broken protocol stack\r\n",UART0);
		gsmState = 11;  		//close socket
	}
}

void DataDecrypt(){
	//procBuff
	int value;
	char test[20];
	char test1[10];
	//strcpy(procBuff,"696E672F");
	int len;
	int index = 0;
	char hex[4];
	int hexIndex = 0;
	int count = 0;
	len = strlen(procBuff);
	//len = strlen(test);
	char receive[50];
	int recIndex = 0;
	while (index < len){
		/*sendData("D\r\n",UART0);
		sprintf(test1, "i:%d\r\n", index);
		sendData(test1,UART0);
		sprintf(test1, "h:%d\r\n", hexIndex);
		sendData(test1,UART0);*/

		if (hexIndex < 2){
			//hex[hexIndex] = test[index];
			hex[hexIndex] = procBuff[index];
			hexIndex++;

		}
		else {			//hex index == 2
			hexIndex = 0;
			hex[2] = 0;
			/*sendData("2\r\n",UART0);
			sendByte(hex[0], UART0);
			sendByte(hex[1], UART0);*/
			value = Getvalue(hex[0]) * 16;
			value += Getvalue(hex[1]);
			receive[recIndex] = value;
			recIndex++;
			//sprintf(test1, "VALUE:%d\r\n", value);
			//sendData(test1,UART0);
			//if (value == 105) sendData("First byte 105\r\n",UART0);
			//hex[hexIndex] = test[index];
			hex[hexIndex] = procBuff[index];
			hexIndex++;
		}
		if (index == 7){
			hex[2] = 0;
			/*sendData("3\r\n",UART0);
			sendByte(hex[0], UART0);
			sendByte(hex[1], UART0);*/
			value = Getvalue(hex[0]) * 16;
			value += Getvalue(hex[1]);
			receive[recIndex] = value;
			recIndex++;
			//if (strncmp((char*)hex,"6",1)==0) sendData("First nibble 6\r\n",UART0);
		}
		index++;
	}
	recIndex++;
	receive[recIndex] = 0;
	sendData(receive,UART0);
	sendData("\r\n",UART0);
}

int Getvalue(char in){
	int count = 0;

	int compare = 0x30;
	compare = 0x30;
	if ((in >= 0x30) && (in <= 0x39)){
		 //sendData("Number\r\n",UART0);
		 while(compare != in){
			 //sendByte(compare, UART0);
			 compare++;
			 count++;
		 }
	 }
	compare = 0x41;
	 if ((in >= 0x41) && (in <= 0x46)){
		 //sendData("Letter\r\n",UART0);
		 while(compare != in){
			 //sendByte(compare, UART0);
			 compare++;
			 count++;
		 }
		 count += 10;
	 }
	 //char testS[5];
	 //sprintf(testS, "c:%d\r\n", count);
	 //sendData(testS,UART0);
	return count;
}


void recByte(char data){
	recBuff[recBuffPointerWrite] = data;
	recBuffPointerWrite++;
	if (recBuffPointerWrite > recBuffSize) recBuffPointerWrite = 0;
	//sendByte(data, UART0);
}

