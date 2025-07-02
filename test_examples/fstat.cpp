#include "test.hpp"

struct stat fileStat;

int	main()
{
	int fd;

	fd = open("test.txt", O_RDONLY);
	if (fd == -1)
	{
		perror(RED "open" RESET);
		exit(EXIT_FAILURE);
	}
	if (fstat(fd, &fileStat) == -1)
	{
		perror(RED "fcntl" RESET);
		close(fd);
		exit(EXIT_FAILURE);
	}
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		perror(RED "fcntl" RESET);
		close(fd);
		exit(EXIT_FAILURE);
	}

	if (S_ISREG(fileStat.st_mode))
        std::cout << "File type: regular file" << std::endl;
    else if (S_ISDIR(fileStat.st_mode))
        std::cout << "File type: directory" << std::endl;
    else
        std::cout << "File type: other" << std::endl;

    // Print size
    std::cout << "File size: " << fileStat.st_size << " bytes" << std::endl;

    // Print permissions
    std::cout << "Permissions: ";
    std::cout << ((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    std::cout << ((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    std::cout << ((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    std::cout << std::endl;

    // Print inode
    std::cout << "Inode number: " << fileStat.st_ino << std::endl;

    // Print last modification time
    std::cout << "Last modified: " << ctime(&fileStat.st_mtime);

    close(fd);
    return 0;
}