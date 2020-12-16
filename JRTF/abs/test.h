#pragma once
#include<iostream>
#include"AB.h"
#include<time.h>
#include<NIDAQmx.h>
#include"H5Cpp.h"
#include"Global.h"
#include"Hub.h"
/*
测试每个循环的时间延时，并且进行使能控制。
当接到外部触发时，将AB中的静态变量置1，触发结束后，恢复置零或者结束线程。
*/

class test:public AB
{
public:
	test();
	~test();
	int init();
	void dowork();
	int SaveHDF5(std::string FILENAME, double* data_array, double* time_array, long data_nums);
	void ErroeCheck();
	void ShotGet();
private:
	int32 error;
	timespec time;
	double ns[1000];
	double ms[1000];
	double log_ms[1000];
	double *log_data;

	long shotNo;
	long MAX_ns;
	long MIN_ns;
	long AVE_ns;
	TaskHandle task_able;
	float64 able[1];
	int32 able_num;

};

