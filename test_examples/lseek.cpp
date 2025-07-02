#include "test.hpp"

#define BUFF 11

int	main()
{
	int fd;
	int offset;
	int bytes;
	std::string buffer;
	buffer.resize(BUFF);

	fd = open("test.txt", O_RDONLY);
	if (fd == -1)
	{
		perror( RED "open" WHITE);
		exit(EXIT_FAILURE);
	}
	// advance position in file descriptor to third line
	offset = lseek(fd, BUFF * 2, SEEK_SET);
	if (offset == -1)
	{
		perror( RED "lseek" WHITE);
		exit(EXIT_FAILURE);
	}
	else
	{
		bytes = read(fd, &buffer[0], BUFF);
		if (bytes == -1)
		{
			perror( RED "read" WHITE);
			exit(EXIT_FAILURE);
		}
		else
		{
			buffer.resize(bytes -1);
			std::cout << buffer << std::endl;
		}
	}
	return (0);
}