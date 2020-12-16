#include <tensorflow/core/platform/env.h>
#include <tensorflow/core/public/session.h>
#include <iostream>
#include"tensorflow/cc/saved_model/loader.h"
#include <google/protobuf/message.h>
#include"H5Cpp.h"
#include"AB.h"
#include"NIDAQmx.h"
#include"NIAI.h"

using namespace std;
using namespace tensorflow;
using namespace H5;

class SoftX:public AB
{
public:
    SoftX();
    ~SoftX();
    int init();
    void dowork();
    int LoadHDF5(std::string FILE_NAME, float sxrall[30][900], float dx[1000]);
//    int SaveHDF5(std::string FILE_NAME, double *data_array,long data_nums);
//    float sxrall[30][900];
    void ErrorCheck(int32 error);
    ShareData* get_data(std::string name);
private:
    std::unique_ptr<Session> session;
    std::vector<tensorflow::Tensor> outputs;
    tensorflow::Status status;
    Tensor input_tensor;
    double dx_pred[1];
    int32 dx_num;
    double* sxr;
    TaskHandle taskAO = 0;
    double softX_log[1000];
};

