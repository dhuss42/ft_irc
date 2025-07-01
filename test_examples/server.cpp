# include "test.hpp"
// create a socket
// bind the socket
// listen on the socket
// accept connection
int	main()
{
	int sockfd;
	int	acceptfd;
	sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cerr << RED "Error: socket" WHITE << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << GREEN "created socket" WHITE << std::endl;

	int opt = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) 
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	// structs don't auto initialise, better to set to 0 before being used by functions
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	// Inside sin_addr, the actual IP address is stored in s_addr
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (addr.sin_addr.s_addr < 0)
	{
		perror(RED "Error: inet_addr" WHITE);
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	addr.sin_port = htons(12345);
	// need to cast to sockaddr since bind does not take sockaddr_in
	if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) == -1)
	{
		perror(RED "Error: bind");
		std::cerr << RED "Error: bind" WHITE << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	else
		std::cout << GREEN "bind IP address and port to socket" WHITE << std::endl;
	if (listen(sockfd, 1) == -1)
	{
		perror(RED "Error: listen");
		std::cerr << RED "Error: listen" WHITE << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	else
		std::cout << GREEN "listening on socket" WHITE << std::endl;

	acceptfd = accept(sockfd, NULL, NULL);
	if (acceptfd == -1)
	{
		std::cerr << RED "Error: accept" WHITE << std::endl;
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	else
		std::cout << GREEN "created accepted Socket" WHITE << std::endl;
	
	bool loop = true;
	while (loop)
	{

		char buffer[200];
		size_t received;
		received = recv(acceptfd, buffer, sizeof(buffer), 0);
		if (received <= 0)
		{
			perror( RED "recv" RESET);
			break ;
		}
		else
		{
			std::cout << "number of chars" << received << std::endl;
			buffer[received] = '\0';
			if (strcmp(buffer, "EXIT") == 0)
				break ;
			else
				std::cout << "Received: " << buffer << std::endl;
	
			const char* reply = "Message Received";
			if (send(acceptfd, reply, strlen(reply), 0) <= 0)
			{
				perror( RED "send" RESET);
				break ;
			}
		}
	}
	std::cout << GREEN "Exiting..." RESET << std::endl;
	if (close(sockfd) == -1)
		std::cerr << RED "Error: close sockfd" WHITE << std::endl;
	if (close(acceptfd) == -1)
		std::cerr << RED "Error: close acceptfd" WHITE << std::endl;
	return (0);
}