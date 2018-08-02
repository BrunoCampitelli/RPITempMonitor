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
 * 0.498/0.488 at 28.1 C
 * eq: temperature = voltage/0.01-21.9
 * */
#include <stdio.h>
#include "cdevlog.h"
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <time.h>

#define VREF 1.623
#define MINSMIL 1000*60*1
#define TEST 0

unsigned char data[5];
float calvolt=0;
long double vavr=0;
char label[10];
char result[5];
int i;

int main(int argc, char **argv)
{
	time_t timstr;
	timstr=time(NULL);
	printf(ctime(&timstr));
	printf("start\n");
	data[0]=0b11000000;

	if(wiringPiSPISetup(0,3600000)<0) printf("setup error\n");
	printf("calibrating\n");
	for(i=0;i<10;i++){
		wiringPiSPIDataRW(0,data,sizeof(data));
		data[0]=0b11000000;
		calvolt+=(float)((data[1]<<1)+(data[2]>>7))*5/1024;
		delay(500);
	}
	
	calvolt/=10;
	printf("average: %f\n",calvolt);
	printf("diff: %f\n",VREF-calvolt);
	
	printf("done\n");
	data[0]=0b11001000;
	printf("starting data collection now\n");
	
#if TEST
	creat_file();
	while(1){
		for(i=0;i<1000;i++){
			wiringPiSPIDataRW(0,data,sizeof(data));
			vavr+=(double)((data[1]<<1)+(data[2]>>7))*5/1024+VREF-calvolt;		
			data[0]=0b11001000;
			delay(5);
		}
		vavr/=1000;
		printf("%f\n",vavr/0.01-21.9);
		//printf("%f\n",vavr);
		//sprintf(result,"%f\n",((float)((data[1]<<1)+(data[2]>>7))*5/1024+VREF-calvolt)/0.01-21.9);
		//sprintf(result,"%f\n",((float)((data[1]<<1)+(data[2]>>7))*5/1024+VREF-calvolt));
		vavr=0;
	}
#else
	while(1){
		
		for(i=0;i<1000;i++){
			wiringPiSPIDataRW(0,data,sizeof(data));
			vavr+=(double)((data[1]<<1)+(data[2]>>7))*5/1024+VREF-calvolt;		
			data[0]=0b11001000;
			delay(5);
		}
		vavr/=1000;
		timstr=time(NULL);
		print_file(ctime(&timstr));
		sprintf(result,"%f\n",vavr/0.01-21.9);
		print_file(result);
		vavr=0;
		delay(MINSMIL);
	}
#endif
	return 0;
}

