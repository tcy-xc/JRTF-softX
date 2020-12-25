#include"test.h"
#include<fcntl.h>
#include"H5Cpp.h"
#include <mdslib.h>
#include <mdsshr.h>
#include<string.h>
using namespace H5;
using namespace std;

test::test()
{
	able[0] = 0;
	ab = 1;
}

test::~test()
{
}

int test::init()
{
	int err;
	int latency_target_fd = 0;
	int32_t latency_target_value = 0;

	latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
	if (latency_target_fd == -1) {
		std::cout<<"WARN: open /dev/cpu_dma_latency"<< std::endl;
		return -1;
	}
	err = write(latency_target_fd, &latency_target_value, 4);
	if (err < 1) {
		std::cout << "# error setting cpu_dma_latency to %d!"<< latency_target_value << std::endl;
		close(latency_target_fd);
		return -1;
	}
	printf("# /dev/cpu_dma_latency set to %dus\n", latency_target_value);
	std::cout << "init OK!" << std::endl;

	error = DAQmxClearTask(task_able);
	ErroeCheck();
	error = DAQmxCreateTask("taskABLE", &task_able);
	ErroeCheck();
	error = DAQmxCreateAIVoltageChan(task_able, "PXI1Slot3/ai23", "", DAQmx_Val_Diff, -10.0, 10.0, DAQmx_Val_Volts, NULL);
	//	error = DAQmxCreateAIVoltageChan(taskHandle, "PXI1Slot3/ai0", "", DAQmx_Val_Diff, -10.0, 10.0, DAQmx_Val_Volts, NULL);
	ErroeCheck();
	error = DAQmxCfgSampClkTiming(task_able, "", 1000.0, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 1000);
	ErroeCheck();
	error = DAQmxStartTask(task_able);

	return 0;
}

void test::dowork()
{
	DAQmxReadAnalogF64(task_able, 1, 10.0, DAQmx_Val_GroupByChannel, able, 1, &able_num, NULL);
	if (able[0]>=2.5)//	if(ab==1)
	{
		softX_able = 1;
		if (get_run_count() < 1000)
		{


			clock_gettime(CLOCK_MONOTONIC, &time);
			ns[get_run_count()] = double(time.tv_nsec + time.tv_sec * 1000000000);
			ms[get_run_count()] = ns[get_run_count()] / 1000000;
			
		}
		else
		{
//			ab = 0;
//			for (size_t i = 800; i < 820 - 1; i++)
//			{
//				std::cout << double(ns[i + 1] - ns[i]) / 1000 << std::endl;
//
//			}
			printf("the count >1000,abort!");
			pthread_exit(0);
		}



	}
	else
	{
		if (softX_able == 1)
		{
			log_ms[0] = 200;
			for (size_t i = 1; i <count_run; i++)
			{
				log_ms[i] = ms[i] - ms[0]+ log_ms[0];
			}
			ShareData* temp = g_hub.get_data_from_other_ab("pipe_softX:softX", "log");
			if (temp->share != nullptr)
			{
				log_data= temp->convert_data_array<double>();
			}

			ShotGet();
			if (shotNo != 0)
			{


				std::string FILE_WAY = "/home/tcy/Data/softx/";
				printf("FILE_WAY == %s\n", FILE_WAY.c_str());
				printf("shotNo == %d\n", shotNo);
				std::string FILE_WAY2 = FILE_WAY.append(std::to_string(shotNo));
				std::string FILE_SUFFIX = ".hdf5";
				std::string FILE_NAME = FILE_WAY2.append(FILE_SUFFIX);
				printf("FILE_NAME == %s\n", FILE_NAME.c_str());
				SaveHDF5(FILE_NAME, log_data, log_ms, count_run);
				printf("JRTF-softX is waiting for trigger\n");
				//			std::cout << "MAX = " << MAX_ns << endl;
				//			std::cout << "MIN = " << MIN_ns << endl;
				//			std::cout << "AVE = " << AVE_ns << endl;
			}
		}
	    softX_able = 0;
		count_run = -1;                   //执行完后加一,不是在执行前
//		pthread_exit(0);
	}
}


int test::SaveHDF5(std::string FILENAME, double* data_array,double *time_array, long data_nums)
{

	const std::string DATASET_NAME_TIMA = "time";
	const std::string DATASET_NAME_DATA = "data";
	const std::string GROUP_NAME_DX = "predic_dx";

	Exception::dontPrint();
	H5File Wfile(FILENAME, H5F_ACC_TRUNC);
	long data_numss = data_nums - 1;
	hsize_t rim[2] = { data_numss,1 };
	Group* group = new Group(Wfile.createGroup(GROUP_NAME_DX));
	DataSpace dataspace(1, rim);
	DataSet* dataset1 = new DataSet(group->createDataSet(DATASET_NAME_DATA, PredType::NATIVE_DOUBLE, dataspace));
	dataset1->write(data_array, PredType::NATIVE_DOUBLE);
	DataSet* dataset2 = new DataSet(group->createDataSet(DATASET_NAME_TIMA, PredType::NATIVE_DOUBLE, dataspace));
	dataset2->write(time_array, PredType::NATIVE_DOUBLE);

	dataset1->close();
	dataset2->close();
	dataspace.close();
	group->close();
	Wfile.close();

}



void test::ErroeCheck()
{
	if (DAQmxFailed(error))
	{
		char        errBuff[2048] = { '\0' };
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
		printf("DAQmx Error: %s\n", errBuff);
		pthread_exit(0);
	}
}


void test::ShotGet()
{
	int status;
	int shot = 0;
	int null = 0;
	int dtypelong = DTYPE_LONG;
	status = MdsConnect("222.20.94.136");
	printf("status = %d\n", status);
	if (status == -1) {
		fprintf(stderr, "Error connecting to MyServer.\n");
		
		shotNo = 0;
		return;
	}
	status = MdsOpen("jtext", &shot);
	if (!((status) & 1))
	{
		fprintf(stderr, "Error opening tree for shot %d: %s.\n", shot, MdsGetMsg(status));
		
		shotNo = 0;
		return;
	}
	long* data;
	int size = 1;
	int retSize;
	int shotDesc = descr(&dtypelong, data, &size, &null);
	status = MdsValue("\SHOT", &shotDesc, &null, &retSize);
	printf("shot = %d\n", data[0]);
	if (!((status) & 1))
	{
		fprintf(stderr, "Error retrieving signal: %s\n", MdsGetMsg(status));
		
		shotNo = 0;
		return;
	}
	shotNo = data[0];
}



