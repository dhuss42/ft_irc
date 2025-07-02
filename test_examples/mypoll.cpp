#include "test.hpp"

#define BUFFER_SIZE 1024

int	main()
{
	char name[255];
	int counter = 0;

	// init mypoll
	pollfd mypoll;
	memset(&mypoll, 0, sizeof(mypoll));
	mypoll.fd = 0;
	mypoll.events = POLLIN;

	std::cout << "type your name: " << std::endl;
	while (true)
	{
		// now read is nonblocking
		int ret = poll(&mypoll, 1, 100);
		if (ret == -1)
		{
			perror("poll");
			return (1);
		}
		else if (ret == 0)
		{
			counter++;
		}
		else if (mypoll.revents & POLLIN)
		{
			ssize_t bytesRead = read(0, name, sizeof(name));
			if (bytesRead <= 0)
			{
				perror("read");
				return 1;
			}
			name[bytesRead] = '\0';
			std::cout << "Hello, " << name;
			break;
		}
	}
	std::cout << "it took you " << 100 * counter << " milliseconds to type your name " << std::endl;
	return (0);
}
