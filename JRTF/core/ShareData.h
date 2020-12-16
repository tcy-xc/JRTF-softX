#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <typeinfo>
#include <limits.h>
#include <map>

typedef unsigned char BYTE;

#define DATATYPE_NONE 0
#define DATATYPE_INT 10
#define DATATYPE_UNSIGNED_INT 11
#define DATATYPE_LONG 20
#define DATATYPE_UNSIGNED_LONG 21
#define DATATYPE_FLOAT 30
#define DATATYPE_DOUBLE 40
#define DATATYPE_INT_ARRAY 50
#define DATATYPE_LONG_ARRAY 60
#define DATATYPE_FLOAT_ARRAY 70
#define DATATYPE_DOUBLE_ARRAY 80

class ShareData {
public:
	ShareData() {};
	ShareData(int type);
	ShareData(void* local, int type, unsigned int size);
	~ShareData();

	template<typename T>
	T* convert_data()
	{
		if (type == DATATYPE_INT || type == DATATYPE_FLOAT || type == DATATYPE_LONG || type == DATATYPE_DOUBLE)
		{
			return new T((T*)share);
		}
		else
		{
			return nullptr;
		}
	}

	template<typename T>
	T* convert_data_array()
	{
		if (type == DATATYPE_INT_ARRAY || type == DATATYPE_FLOAT_ARRAY || type == DATATYPE_LONG_ARRAY || type == DATATYPE_DOUBLE_ARRAY)
		{
			int number;
			switch (type)
			{
			case DATATYPE_INT_ARRAY:
				number = size / sizeof(int);
				return resolve_data_array<int, T>(number);
			case DATATYPE_FLOAT_ARRAY:
				number = size / sizeof(float);
				return resolve_data_array<float, T>(number);
			case DATATYPE_LONG_ARRAY:
				number = size / sizeof(long);
				return resolve_data_array<long, T>(number);
			case DATATYPE_DOUBLE_ARRAY:
				number = size / sizeof(double);
				return resolve_data_array<double, T>(number);
			}
		}
		else
		{
			return nullptr;
		}
	}

	template<typename T1, typename T2>
	T2* resolve_data_array(int number)
	{
		T2* dst_temp = new T2[number];
		T1* src_temp = (T1*)share;
		for (size_t i = 0; i < number; i++)
		{
			dst_temp[i] = (T2)src_temp[i];
		}
		return dst_temp;
	}

	void* share;
	int type;
	unsigned int size;

private:

};
