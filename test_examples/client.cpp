#include "test.hpp"

void	receiveMsg(int &connection)
{
	char buffer[512];
	size_t received = 0;
	memset(buffer, 0, sizeof(buffer)); // could this lead to dataraces? when many connections

	bool	loop = true;
	int		last = 0;
	int		tmp = 0;
	while (loop)
	{
		received = recv(connection, buffer, sizeof(buffer), 0);
		if (received <= 0)
		{
			perror("recv");
			exit(EXIT_FAILURE);
		}
		else
		{
			buffer[received] = '\0';
			tmp = received + last;
			last = tmp;
			for (int i = 0; i < tmp; i++)
			{
				if (buffer[i] == '\r' && buffer[i + 1] == '\n')
				{
					loop = false;
					break ;
				}
			}
		}
		std::cout << buffer << std::endl;
		// loop backwards over the string and locate the two signs
		// if the two chars are found next to each other break the loop
	}
}


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
		std::string buffer(512, 0);
		std::cout << "enter message: ";
		std::getline(std::cin, buffer);
		buffer += "\r\n";
		if (strcmp(buffer.c_str(), "EXIT") == 0)
			break ;
		else if (send(clientSockfd, buffer.c_str(), strlen(buffer.c_str()), 0) <= 0)
		{
			perror( RED "send" RESET);
			break ;
		}

		receiveMsg(clientSockfd);
		// std::memset(buffer, 0, 200);
		// size_t received = recv(clientSockfd, buffer, 17, 0);
		// if (received <= 0)
		// {
		// 	perror( RED "recv" RESET);
		// 	break ;
		// }
		// buffer[received] = '\0';
		// std::cout << buffer << std::endl;

	}

	std::cout << GREEN "Exiting..." RESET << std::endl;
	if (close(clientSockfd) == -1)
	{
		std::cerr << RED "Error: close clientSockfd" WHITE << std::endl;
		exit(EXIT_FAILURE);
	}
	return (0);
}