/*
 * untitled.c
 * 
 * Copyright 2018  <pi@raspberrypi>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

/*
 * voltage change: 10mV per degree
 * 3.021 at 28 C
 * eq: temperature = voltage/0.01-274.1
 * */
#include <stdio.h>
#include "cdevlog.h"
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <time.h>

#define VDIOD 1.687
#define VREF 3.27
#define MINSMIL 1000*60*5
#define TEST 0
#define STATLEN 25
#define DATALEN 1000

unsigned char data[5];
float calvolt=0;
long double vavr=0;
char label[10];
char result[5];
short int fail_count=0;
double lastavr=0;
double lastvolt;
int i;

double takevolt(char channel);
double takeavr(char channel);
void print_volt(char channel);
int takedat(char channel);

int main(int argc, char **argv)
{
	time_t timstr;
	timstr=time(NULL);
	printf(ctime(&timstr));
	printf("start\n");
	data[0]=0b11000000;

	if(wiringPiSPISetup(0,500000)<0) printf("setup error\n");
	printf("calibrating\n");
	calvolt=takeavr(0b11000);
	
	printf("average: %f\n",calvolt);
	printf("diff: %f\n",VDIOD-calvolt);
	
	printf("starting data collection now\n");
	
#if TEST
	creat_file();
	printf("DEBUG MODE\n");
	while(1){
		/*
		for(fail_count=0;fail_count<5;){
			for(i=0;i<1000;i++){
				wiringPiSPIDataRW(0,data,sizeof(data));
				vavr+=(double)((data[1]<<1)+(data[2]>>7))*5/1024+VREF-calvolt;		
				//data[0]=0b11001000;
				data[0]=0b11000000;
				//delay(5);
			}
			vavr/=1000;
			if(vavr-lastavr>0.003||lastavr-vavr>0.003) fail_count++;
			else fail_count=6;
			printf("%i\n",fail_count);
		}
		//printf("%f\n",vavr/0.01-21.9);
		printf("%f\n",vavr);
		lastavr=vavr;
		//printf("%f\n",vavr);
		//sprintf(result,"%f\n",((float)((data[1]<<1)+(data[2]>>7))*5/1024+VREF-calvolt)/0.01-21.9);
		//sprintf(result,"%f\n",((float)((data[1]<<1)+(data[2]>>7))*5/1024+VREF-calvolt));
		vavr=0;*/
		print_volt(0b11000);
	}
#else
	//creat_file();
	//for(i=0;i<3000;i++)print_volt(0b11001);
	while(1){
		/*for(fail_count=0;fail_count<5;){
			wiringPiSPIDataRW(0,data,sizeof(data));
			vavr=(double)((data[1]<<1)+(data[2]>>7))*5/1024+VREF-calvolt;		
			data[0]=0b11000000;
			if(vavr-lastavr>0.003||lastavr-vavr<-0.003) fail_count=6;
			else fail_count=6;
		}*/
		/*
		data[0]=0b11000000;
		wiringPiSPIDataRW(0,data,sizeof(data));
		vavr=(double)((data[1]<<2)+(data[2]>>6))*1.62/1024;		
		*/
		vavr=takeavr(0b11001);
		timstr=time(NULL);
		print_file(ctime(&timstr));
		//sprintf(result,"%f\n",vavr);
		//printf("%f\n",vavr/0.01-274.1);
		sprintf(result,"%f\n",vavr/0.01-274.1);
		print_file(result);
		//delay(100);
		delay(MINSMIL);
	}
#endif
	return 0;
}

double takevolt(char channel){
	double volt;		
	data[0]=channel;
	wiringPiSPIDataRW(0,data,sizeof(data));
	volt=(double)((data[1]<<4)+(data[2]>>4))*VREF/1024;
	
	return volt;
	
}

int takedat(char channel){
	int notvolt;		
	data[0]=channel;
	wiringPiSPIDataRW(0,data,sizeof(data));
	notvolt=((data[1]<<4)+(data[2]>>4));
	
	return notvolt;
	
}
	

double takeavr(char channel){//fval for value frequency, dval for different vals
	int j,v=0, data[DATALEN], dval[STATLEN]={0}, fval[STATLEN]={0}, mode=0,
		mmin=0,pmin=0, maxv=0;
	double tavr=0;
	
	
	for(j=0;j<DATALEN;j++){
		data[j]=takedat(channel);
		delay(1);
	}
		
	for(j=0;j<DATALEN;j++){
		while(data[j]!=dval[v]&&dval[v]!=0)v++;
		
		if(dval[v]==0){
			dval[v]=data[j];
			maxv++;
		}
		fval[v]++;
		v=0;
	}
	
	//printf("maxv:%d\n",maxv);
	for(v=0;v<maxv;v++){
		if(fval[v]>fval[mode])mode=v;
	}
	mmin=maxv;
	pmin=maxv;
	for(v=0;v<maxv;v++){
		if(dval[mode]-1==dval[v])mmin=v;
		if(dval[mode]+1==dval[v])pmin=v;
	}	
	
	/*
	printf("mode:%d\n",dval[mode]);
	printf("pmin:%d\n",dval[pmin]);
	printf("mmin:%d\n",dval[mmin]);
	printf("fmode:%d\n",fval[mode]);
	printf("fpmin:%d\n",fval[pmin]);
	printf("fmmin:%d\n",fval[mmin]);
	*/
	tavr=(double)(fval[mode]*dval[mode]+fval[pmin]*dval[pmin]+fval[mmin]*dval[mmin])
		/(fval[mode]+fval[pmin]+fval[mmin]);
	tavr=tavr*VREF/1024;
	
	return tavr;
}
	
void print_volt(char channel){
	
	printf("%f\n",takevolt(channel));
	
	return;
}
