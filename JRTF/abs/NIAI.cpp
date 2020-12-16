#include"NIAI.h"



NIAI::NIAI()
{
	error = 0;
	read = 0;
	errorEnable = 0;
}

NIAI::~NIAI()
{
}

ShareData* NIAI::get_data(std::string name)
{
													
	if (name == "data")
	{
		return new ShareData(data, DATATYPE_DOUBLE_ARRAY, read);
	}
	
}

int NIAI::init()
{
	error = DAQmxClearTask(taskHandle);
	ErroeCheck();
	error = DAQmxCreateTask("taskAI", &taskHandle);
	ErroeCheck();
	error = DAQmxCreateAIVoltageChan(taskHandle, "PXI1Slot3/ai0:7,PXI1Slot3/ai16:22,PXI1Slot5/ai0:7,PXI1Slot5/ai16:22", "", DAQmx_Val_Diff, -10.0, 10.0, DAQmx_Val_Volts, NULL);
//	error = DAQmxCreateAIVoltageChan(taskHandle, "PXI1Slot3/ai0", "", DAQmx_Val_Diff, -10.0, 10.0, DAQmx_Val_Volts, NULL);
	ErroeCheck();
	error = DAQmxCfgSampClkTiming(taskHandle, "", 1000.0, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 1000);
	ErroeCheck();
	error = DAQmxStartTask(taskHandle);
	ErroeCheck();
	printf("taskAI start success\n");
	printf("NI DAQmx AI error = %d\n", error);
}


void NIAI::dowork()
{
	error = DAQmxReadAnalogF64(taskHandle, 1, 10.0, DAQmx_Val_GroupByChannel, data, 30, &read, NULL);
	if (read!=1&&errorEnable==0)
	{
		printf("NI DAQmx read error\n");
		printf("NI DAQmx read = %d\n",read);
		printf("NI DAQmx error = %d\n", error);
		ErroeCheck();
	}
	if (error!=0 && errorEnable == 0)
	{
		printf("NI DAQmx function error\n");
		printf("NI DAQmx read = %d\n", read);
		printf("NI DAQmx error = %d\n", error);
		errorEnable ++;
		ErroeCheck();
	}
}


void NIAI::ErroeCheck()   //It can be writed to the class AB;
{
	if (DAQmxFailed(error))
	{
		char        errBuff[2048] = { '\0' };
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
		printf("DAQmx Error: %s\n", errBuff);
		pthread_exit(0);
	}
}