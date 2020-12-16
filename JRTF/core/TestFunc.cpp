#include"TestFunc.h"
/*
void run_test()
{
	Pipeline * pipe1 = new Pipeline("pipe1", "interval");
	pipe1->set_period(20000000);
	Pipeline * pipe2 = new Pipeline("pipe2", "interval");
	pipe2->set_period(20000000);
	g_hub.add_pipe(pipe1);
	g_hub.add_pipe(pipe2);
	AB* ab1 = new AbNiAi();
	ab1->set_name("ai");
	AB* ab2 = new TestInput();
	ab2->set_name("test_ai");
	pipe1->add_ab(ab1);
	pipe2->add_ab(ab2);
	pipe1->start();
	pipe2->start();
	sleep(3);		// 创建线程耗时 保证在发出信号的时候 线程已经创建好并且已经阻塞
	pipe1->threadsync.condsignal();
	pipe2->threadsync.condsignal();
}
*/
std::vector<Pipeline *> test_create_pipe_ab()
{
	//////////////////////  start create cmd //////////////////////////////////

	

	//////////////////////  end create pipe  //////////////////////////////////

	//////////////////////  start create pipe //////////////////////////////////

	//pipeNode each
	//std::cout << COLOR_PIPE << std::endl;
	cout << "################ pipeline created ################" << endl;
	std::string pipe_name, pipe_mode;
	std::string pipe_period = "1000";
	std::string pipe_policy = "fifo";
	std::string pipe_priority = "80";
	std::string pipe_affinity = "0";
	
	pipe_name = "pipe_softX";
	pipe_mode = "interval";
	pipe_period = "1000";
	pipe_policy = "fifo";
	pipe_priority = "91";
	pipe_affinity = "2";
	Pipeline *temp_pipe = new Pipeline(pipe_name, pipe_mode);
	pipe_vector.push_back(temp_pipe);

	temp_pipe->set_period(strtoul(pipe_period.c_str(), NULL, 0));
	temp_pipe->set_policy(pipe_policy);
	temp_pipe->set_priority(atoi(pipe_priority.c_str()));
	//temp_pipe->set_affinity(atol(pipe_affinity.c_str()));


	////////////////////  start create AB ////////////////////

	//std::cout << COLOR_AB << std::endl;
	cout << "=========== AB created 1 ===========" << endl;
	std::string ab_name = "time";
	AB* temp_ab = new test();
	temp_ab->set_name(ab_name);
	temp_ab->set_hub(&g_hub);

	temp_pipe->add_ab(temp_ab);
	ab_vector.push_back(temp_ab);

	// print things in AB, not here
	//cout<<"set_data \""<<p1<<"\" = \""<<p2<<"\""<< endl;;
	cout << "==================================";
	std::cout << COLOR_PIPE << std::endl;
	cout << endl;

	//////////////////  stop create AB //////////////////////////

	cout << "=========== AB created 2 ===========" << endl;
	ab_name = "ai";
	temp_ab = new NIAI();
	temp_ab->set_name(ab_name);
	temp_ab->set_hub(&g_hub);

	temp_pipe->add_ab(temp_ab);
	ab_vector.push_back(temp_ab);

	// print things in AB, not here
	//cout<<"set_data \""<<p1<<"\" = \""<<p2<<"\""<< endl;;
	cout << "==================================";
	std::cout << COLOR_PIPE << std::endl;
	cout << endl;

	////////////////////  start create AB ////////////////////

	//std::cout << COLOR_AB << std::endl;
	cout << "=========== AB created 3 ===========" << endl;
	ab_name = "softX";
	temp_ab = new SoftX();
	temp_ab->set_name(ab_name);
	temp_ab->set_hub(&g_hub);

	temp_pipe->add_ab(temp_ab);
	ab_vector.push_back(temp_ab);

	// print things in AB, not here
	//cout<<"set_data \""<<p1<<"\" = \""<<p2<<"\""<< endl;;
	cout << "==================================";
	std::cout << COLOR_PIPE << std::endl;
	cout << endl;

	//////////////////  stop create AB //////////////////////////

	//////////////////  stop create AB //////////////////////////


	cout << "##################################################";
	std::cout << COLOR_DEF;
	cout << endl << endl;

	//////////////////////  stop create pipe //////////////////////////////////


	std::vector<Pipeline *>::iterator iter = pipe_vector.begin();
	for (; iter<pipe_vector.end(); iter++)
	{
		g_hub.add_pipe(*iter);
	}

	// init_after
	std::vector<AB *>::iterator iter1 = ab_vector.begin();
	for (; iter1<ab_vector.end(); iter1++)
	{
		(*iter1)->init();//init after the g_hub has all the informations!
	}


	/*
	* create pipe threads, pipe thread will wait signal!
	*/
	for (std::vector<Pipeline *>::iterator iter = pipe_vector.begin(); iter != pipe_vector.end(); iter++)
	{
		(*iter)->start();
	}

	/*send signal to start begin the threads*/
	for (std::vector<Pipeline *>::iterator iter = pipe_vector.begin(); iter != pipe_vector.end(); iter++)
	{
		sleep(3);
		(*iter)->threadsync.condsignal();
	}

	return pipe_vector;
}
