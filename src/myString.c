
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "ctype.h"

#include "mystring.h"


const char hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

short myStrSection(char *p_str,char *p_result,unsigned char p_size,char p_char,char p_pos){
	short string_index;
	short result_index;
	char tmp_pos;

	string_index=0;
	result_index=0;
	tmp_pos=0;
	p_size--;
	while(p_str[string_index]!=0){
		if(p_str[string_index]==p_char){

			if(tmp_pos==p_pos)break;
			tmp_pos++;
			result_index=0;
		}else{
			if(p_str[string_index]>=' '){
				p_result[result_index]=p_str[string_index];
				if(result_index<p_size)result_index++;
			}
		}
		string_index++;
	}
	if(tmp_pos!=p_pos){
		result_index=0;
		string_index=-1;
	}
	p_result[result_index]=0;

	return string_index;
}


void myLongStr(signed long p_val,char *p_dest,short p_size,char p_base){

	unsigned long tmp_index;
	short tmp_size;
	char tmp_val;
	char tmp_iszero;

	tmp_size=0;
	tmp_iszero=1;

	if(p_base==10){
		tmp_index=1000000000;

		if(p_val<0){
			p_val*=-1;
			p_dest[tmp_size]='-';
			tmp_size++;
		}

		while(tmp_index>0){
			tmp_val=p_val/tmp_index;
			if(tmp_val>0)tmp_iszero=0;
			if(tmp_iszero==0){
				p_dest[tmp_size]=tmp_val+0x30;
				tmp_size++;
			}
			p_val=p_val-(tmp_val*tmp_index);
			tmp_index/=10;
			if(tmp_size>=(p_size-1))break;
		}

	}
	else if(p_base==16){
		tmp_index=32;
		while(tmp_index>0){
			tmp_val=(p_val>>(tmp_index-4))&0x0f;

			if(tmp_val>0)tmp_iszero=0;
			if(tmp_iszero==0){
				if(tmp_val<=9)p_dest[tmp_size]=tmp_val+0x30;
				else if(tmp_val>=10)p_dest[tmp_size]=(tmp_val-10)+'A';
				tmp_size++;
			}

			tmp_index-=4;
			if(tmp_size>=(p_size-1))break;
		}

	}

	if(tmp_iszero==1){
		p_dest[tmp_size]='0';
		tmp_size++;
	}

	p_dest[tmp_size]=0;
}

long myStrLong(char *p_str,char p_base){
	long tmp_long;
	tmp_long=0;
	if(p_base==10){
		while(*p_str != 0){
			if((*p_str>='0')&&(*p_str<='9')){
				tmp_long*=10;
				tmp_long+=*p_str-0x30;
			}
			p_str++;
		}
	}
	else if(p_base==16){
		while(*p_str != 0){
			if(*p_str>='0'){
				tmp_long=tmp_long<<4;
				if((*p_str>='0')&&(*p_str<='9'))tmp_long|=*p_str-'0';
				else if((*p_str>='a')&&(*p_str<='f'))tmp_long|=(*p_str-'a')+10;
				else if((*p_str>='A')&&(*p_str<='F'))tmp_long|=(*p_str-'A')+10;
			}
			p_str++;
		}
	}
	return tmp_long;
}







