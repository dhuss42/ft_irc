#ifndef TEST_HPP
# define TEST_HPP

#include <sys/socket.h> // for socket(), bind, accept, listen
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h> // for sockaddr_in
#include <arpa/inet.h> // inet_addr
#include <poll.h>
#include <fstream>
#include <signal.h>


#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[1;37m"
#define RESET "\033[0m"

#endif