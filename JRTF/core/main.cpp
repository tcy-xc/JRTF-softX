#include"TestFunc.h"

static int shutdown_main = 0;
static void sighand(int sig)
{
	shutdown_main = 1;
}

int main(int argc, char *argv[]) {
	//run_test();
	test_create_pipe_ab();
	g_console.start();

	
	// set signal handle
	signal(SIGINT, sighand);
	signal(SIGTERM, sighand);
	signal(SIGUSR1, sighand);
	while (!shutdown_main) {
		pause();
	}
}
