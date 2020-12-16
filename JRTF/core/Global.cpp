#include<iostream>
#include"Global.h"
#include"Hub.h"
#include"Console.h"
#include"ThreadSync.h"

// 定义全局变量
const char* g_god = "ZhengGuozhen";
std::string g_home_path = "../";


									 // if path not exist, this logger will not work
Hub g_hub;
Console g_console;
ThreadSync g_threadsync;