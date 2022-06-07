/*
 * GSM.h
 *
 *  Created on: Feb 22, 2018
 *      Author: Jeefo
 */

#ifndef GSM_H_
#define GSM_H_

//#define GSM_ON P2_bit.no0
//#define GSM_PWR P3_bit.no0
#define PIN P_bit.no7

typedef enum{
	UART0 = 0,
	UART1
}Comms;

typedef struct{
	int signal;
	int config;
	int send;
	int prompt;
	int content;
	int context;
	int gprsPending;
	int gprsActive;
	int del;
	int reply;
	int restartCount;
	int socket;
}SMSflags;

typedef struct{
	char index[4];
	char recMSISDN[25];
	int date[25];
	int prompt;
	char content1[30];
	char content2[30];
	char IP[30];
	int count;
	char build[160];
}SMSinfo;

typedef struct{
	char url[30];
	char urlport[4];
	int socket1;
	int socket2;
	int socket3;
	int socket4;
	int socket;
}GSMinfo;

void recByte(char data);
void sendData(char* str,Comms d);
void sendByte(char send, Comms in);
void tx0Done();
void tx2Done();
void recData();
void GSM_Service();
void sendSMS(char* num, char* msg);
void GotSMS();
void procData();
void OK();
void Context();
void checkGPRS();
void procSMS();
void buildInfo();
void buildTest();
void getSignal();
void caps(char* str);
void Error();
void pec_Update(uint8_t* pec, uint8_t index);
void GPRS_Send(char* gprs);
void Socket1();
void DataStatus();
void DataDecrypt();
int Getvalue(char in);
#endif /* GSM_H_ */
