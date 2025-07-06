#ifndef SERVER_HPP
# define SERVER_HPP

#include "ft_irc.hpp"

// subject talks about optional config file
//	currently I am assuming the server object reads the file and
//	assigns the values inside the attributes of the server object
//	makes program more flexible and allows for changes without modifying the code

class Server
{
	private:
		// portnumber
		// password
		// store pollfd structs inside a vector
		// store channel objects inside a map or unorded map because faster
		// store client objects inside a map
		// map pollfd.fd to client object
	public:
		Server();
		~Server();
};


#endif