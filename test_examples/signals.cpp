#include "test.hpp"


/*------------------------------------------------------------------*/
/* forks processes													*/
/* parent stops child												*/
/* waits for input from user										*/
/* if input is bigger than 0										*/
/* child is started which prints message							*/
/* after t child is stopped and loop start from beginning			*/
/* else parent terminates child process and exits					*/
/*------------------------------------------------------------------*/
int	main(int argc, char **argv)
{
	int pid = fork();
	if (pid == -1)
		return 1;
	if (pid == 0) // child
	{
		while(1)
		{
			printf("some output\n");
			usleep(50000);
		}
	}
	else
	{
		kill(pid, SIGSTOP); // stop child
		int t;
		do {
			printf("time in seconds for execution: ");
			scanf("%d", &t);
			if (t > 0)
			{
				kill(pid, SIGCONT); // start child
				sleep(t);
				kill(pid, SIGSTOP);
			}
		} while (t > 0);

		kill(pid, SIGKILL); // terminate child process
		wait(NULL);
	}
	return (0);
}