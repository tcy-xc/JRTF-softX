#pragma once
#include<iostream>
#include"AB.h"
#include<time.h>
#include<NIDAQmx.h>
#include"H5Cpp.h"
#include"Global.h"
#include"Hub.h"
/*
����ÿ��ѭ����ʱ����ʱ�����ҽ���ʹ�ܿ��ơ�
���ӵ��ⲿ����ʱ����AB�еľ�̬������1�����������󣬻ָ�������߽����̡߳�
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

