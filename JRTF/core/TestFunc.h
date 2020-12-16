#include <iostream>
#include "Global.h"
#include "Hub.h"
#include "Pipeline.h"
#include "test.h"
#include "Console.h"
#include"softX.h"
#include"NIAI.h"



static std::vector<Pipeline *> pipe_vector;
static std::vector<AB *> ab_vector;

void run_test();

vector<Pipeline *> test_create_pipe_ab();