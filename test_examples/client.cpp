#include "test.hpp"

// creates a socket
// connects to the server

int	main()
{
	int	clientSockfd;
	sockaddr_in addr;

	clientSockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSockfd == -1)
	{
		std::cerr << RED "Error: socket clientSocketfd" WHITE << std::endl;
		exit(EXIT_FAILURE);
	}

	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addr.sin_port = htons(12345);

	if (connect(clientSockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
	{
		std::cerr << RED "Error: connect clientSockfd" WHITE << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << GREEN "connection established" WHITE << std::endl;

	if (close(clientSockfd) == -1)
	{
		std::cerr << RED "Error: close clientSockfd" WHITE << std::endl;
		exit(EXIT_FAILURE);
	}
	return (0);
}