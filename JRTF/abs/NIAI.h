#pragma once
#include"NIDAQmx.h"
#include<stdio.h>
#include<AB.h>

class NIAI:public AB
{
public:
	NIAI();
	~NIAI();
	int init();
	void dowork();
	ShareData* get_data(std::string name);
	void ErroeCheck();
private:
	int32 error;
	TaskHandle taskHandle_1 = 0;
	TaskHandle taskHandle_2 = 0;
	float64 data_1[15];
	float64 data_2[15]; 
	float64 data[30];
	int32 read;
	int errorEnable;
};

