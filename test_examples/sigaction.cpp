#include "test.hpp"

void	handle_sigstp(int sig)
{
	printf("Stop is not allowed\n");
}

/*--------------------------------------------------------------*/
/* changing ctrl z so that it prints msg from handle_sigstop	*/
/*--------------------------------------------------------------*/
int	main(int argc, char** argv)
{
	struct sigaction sa;
	sa.sa_handler = &handle_sigstp;
	sa.sa_flags = SA_RESTART;
	sigaction(SIGTSTP, &sa, NULL);

	int x;
	printf("input number: ");
	scanf("%d", &x);
	printf("Result %d * 5 = %d\n", x, x*5);
	return (0);
}