#ifndef FT_IRC_HPP
# define FT_IRC_HPP

/*=================== Includes ==================*/

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <signal.h>

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <map>
#include <vector>
#include <regex>
#include <iomanip>
#include <string>
#include <cstring>
#include <sstream>

#include "Errors.hpp"
/*=================== Colours ==================*/

#define RESET   "\033[0m"

#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"

/*=================== utils ==================*/
bool	verifyChannelName(const std::string& name);
std::string	toLower(const std::string& str);


#endif