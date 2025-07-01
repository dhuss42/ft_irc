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
		close(clientSockfd);
		exit(EXIT_FAILURE);
	}
	else
		std::cout << GREEN "connection established" WHITE << std::endl;
	
	bool loop = true;
	while (loop)
	{
		char buffer[200];
		std::cout << "enter message: ";
		std::cin.getline(buffer, 200);
		if (strcmp(buffer, "EXIT") == 0)
			break ;
		else if (send(clientSockfd, buffer, strlen(buffer), 0) <= 0)
		{
			perror( RED "send" RESET);
			break ;
		}
	
		std::memset(buffer, 0, 200);
		size_t received = recv(clientSockfd, buffer, 17, 0);
		if (received <= 0)
		{
			perror( RED "recv" RESET);
			break ;
		}
		buffer[received] = '\0';
		std::cout << buffer << std::endl;

	}

	std::cout << GREEN "Exiting..." RESET << std::endl;
	if (close(clientSockfd) == -1)
	{
		std::cerr << RED "Error: close clientSockfd" WHITE << std::endl;
		exit(EXIT_FAILURE);
	}
	return (0);
}