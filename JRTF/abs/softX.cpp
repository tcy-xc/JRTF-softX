#include"softX.h"
#include"Global.h"
#include"Hub.h"
SoftX::SoftX() :input_tensor(DT_FLOAT, TensorShape({ 1,30 }))
{
    dx_num = 1;
    SessionOptions options;
    session = std::unique_ptr<Session>(NewSession(options));
}

SoftX::~SoftX()
{
}

int SoftX::init()
{
    std::string model_path = "/home/tcy/projects/JRTF-softX/save_model/softX_model1";
 //  std::string SAVE_FILE_NAME = "/home/tcy/Data/predictions/1061333.hdf5";


/*加载模型至会话中*/
    std::cout << "Hello tensorflow!\n" << std::endl;

    tensorflow::SessionOptions session_options;
    tensorflow::RunOptions run_options;
    tensorflow::SavedModelBundle bundle;
    constexpr char kSavedModelTagServe[] = "serve";   /* 模型tag名称 */

    Status status_load = LoadSavedModel(session_options, run_options, model_path, { kSavedModelTagServe }, &bundle);
    if (!status_load.ok())
    {
        std::cerr << "Error reading graph definition from " + model_path + ": " + status.ToString() << std::endl;
    }

    session = std::move(bundle.session);


    /*离线内容，从HDF5文件中读取数据*/
/*
    std::string FILE_NAME = "/home/tcy/Data/srx_c/1061333.hdf5";
    int h5status;
    float dx[1000];
    h5status = LoadHDF5(FILE_NAME, sxrall, dx);
    if (h5status != 0) { cout << "hdf5 Load failed" << endl; exit(0); }
    else { cout << "hdf5 Load success" << endl; }
*/

/* 优化tensorflow运行*/
    for (size_t i = 0; i < 10; i++)
    {
        auto input_tensor_mapped = input_tensor.tensor<float, 2>();
        std::vector<std::pair<std::string, tensorflow::Tensor>> inputs =
        {
            {"serving_default_srx", input_tensor}                //模型输入名称接口  使用tensorflow中的的一个py脚本可以解析 saved_model_cli.py
        };
        for (size_t c = 0; c < 30; c++)
        {
            input_tensor_mapped(0, c) = 0;
        }
        status = session->Run(inputs, { "StatefulPartitionedCall" }, { }, &outputs);//模型输出名称接口
    }
/*集成模拟输出功能，硬件定时单点*/

    int32 a;
    a=DAQmxClearTask(taskAO);
    ErrorCheck(a);
    a=DAQmxCreateTask("taskAO", &taskAO);
    ErrorCheck(a);
    a=DAQmxCreateAOVoltageChan(taskAO, "PXI1Slot5/ao0", "",  -5.0, 5.0, DAQmx_Val_Volts, NULL);
    ErrorCheck(a);
    a=DAQmxCfgSampClkTiming(taskAO, "", 1000.0, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 1);
    ErrorCheck(a);
    a=DAQmxStartTask(taskAO);
    ErrorCheck(a);
    printf("taskAO start success\n");
    printf("NI DAQmx AO error = %d\n",a);

}

void SoftX::dowork()
{
    if (softX_able == 1)
    {
        ShareData* temp = g_hub.get_data_from_other_ab("pipe_softX:ai", "data");
        if (temp->share != nullptr)
        {
            sxr = temp->convert_data_array<double>();
            
            auto input_tensor_mapped = input_tensor.tensor<float, 2>();
            std::vector<std::pair<std::string, tensorflow::Tensor>> inputs =
            {
                {"serving_default_srx", input_tensor}
            };
            for (int c = 0; c < 30; c++)
            {
     
                input_tensor_mapped(0, c) = sxr[c];

            }
            status = session->Run(inputs, { "StatefulPartitionedCall" }, { }, &outputs);
            if (!status.ok())
            {
                std::cerr << status.ToString() << endl;
                exit(-1);
            }
            tensorflow::Tensor output = std::move(outputs.at(0));
            auto out_shape = output.tensor<float, 2>();
            dx_pred[0] = out_shape(0, 0);

            int a;
            a=DAQmxWriteAnalogF64(taskAO, 1, 1, 2, DAQmx_Val_GroupByChannel, dx_pred, &dx_num, NULL);
            ErrorCheck(a);
            softX_log[get_run_count()] = dx_pred[0];

        }
        delete temp->share;
    }
    else
    {
        dx_pred[0] = 0;
        DAQmxWriteAnalogF64(taskAO, 1, 1, 2, DAQmx_Val_GroupByChannel, dx_pred, &dx_num, NULL);
        count_run = -1;
    }
    
}


ShareData* SoftX::get_data(std::string name)
{

    if (name == "log")
    {
        return new ShareData(softX_log, DATATYPE_DOUBLE_ARRAY, 1000);
    }

}


int SoftX::LoadHDF5(std::string FILE_NAME, float sxrall[30][900], float dx[1000])
{
    int sxr_no[30];
    std::string GROUP_NAME_SXR = "sxr_cc_0";
    const std::string DATASET_NAME_TIMA = "time";
    const std::string DATASET_NAME_DATA = "data";
    const std::string GROUP_NAME_DX = "dx";

    try
    {
        Exception::dontPrint();

        H5File file(FILE_NAME, H5F_ACC_RDONLY);
        std::string GROUP_NAME_SXR = "sxr_cc_0";

        for (int i = 33; i < 63; i++)
        {
            GROUP_NAME_SXR = "sxr_cc_0";
            float* sxr = new float[450000];
            int a = i - 33;
            sxr_no[a] = i;

            std::string GROUP_NAME_SXR_NO = GROUP_NAME_SXR.append(to_string(sxr_no[a]));

            cout << GROUP_NAME_SXR_NO << "   ";

            Group* group = new Group(file.openGroup(GROUP_NAME_SXR_NO));
            DataSet* dataset = new DataSet(group->openDataSet(DATASET_NAME_DATA));


            dataset->read(sxr, PredType::NATIVE_FLOAT);

            for (long b = 0; b < 900; b++)
            {
                int c = b * 500;
                sxrall[a][b] = sxr[c];
            }

            group->close();
            dataset->close();
            delete[]sxr;
            delete group;
            delete dataset;

        }
        float dxData[10000];
        Group* group = new Group(file.openGroup(GROUP_NAME_DX));
        DataSet* dataset = new DataSet(group->openDataSet(DATASET_NAME_DATA));
        dataset->read(dxData, PredType::NATIVE_FLOAT);
        for (short d = 0; d < 1000; d++)
        {
            short c = d * 10;
            dx[d] = dxData[c];
        }

        cout << " " << endl;
        cout << "sxr036  2000 = " << sxrall[3][200] << endl;
        group->close();
        dataset->close();
        file.close();
        delete group;
        delete dataset;
    }




    catch (FileIException error)
    {
        error.printErrorStack();
        return -1;
    }

    // catch failure caused by the DataSet operations
    catch (DataSetIException error)
    {
        error.printErrorStack();
        return -1;
    }

    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error)
    {
        error.printErrorStack();
        return -1;
    }

    // catch failure caused by the DataSpace operations
    catch (DataTypeIException error)
    {
        error.printErrorStack();
        return -1;
    }

    return 0;

}




void SoftX::ErrorCheck(int32 error)
{
    if (DAQmxFailed(error))
    {
        char        errBuff[2048] = { '\0' };
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        printf("DAQmx Error: %s\n", errBuff);
    }
}

/*
int SoftX::SaveHDF5(std::string FILE_NAME, double* data_array, long data_nums)
{
    
    const std::string DATASET_NAME_TIMA = "time";
    const std::string DATASET_NAME_DATA = "data";
    const std::string GROUP_NAME_DX = "predic_dx";

    Exception::dontPrint();
    H5File Wfile(FILE_NAME, H5F_ACC_RDWR);

    hsize_t rim[2] = { data_nums,1 };
    Group* group = new Group(Wfile.openGroup(GROUP_NAME_DX));
    DataSpace dataspace(1, rim);
    DataSet* dataset = new DataSet(group->createDataSet(DATASET_NAME_DATA, PredType::NATIVE_FLOAT, dataspace));
    dataset->write(data_array, PredType::NATIVE_DOUBLE);

    dataset->close();
    dataspace.close();
    group->close();
    Wfile.close();

}
*/