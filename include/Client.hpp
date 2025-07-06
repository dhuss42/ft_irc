#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "ft_irc.hpp"

class Client
{
	private:
		// 	socket fd
		//	pointer to server

		//  read buffer for the client
		//  write buffer

		//  a netwide unique identifier
		//	nickname
		//	username
		//	hostname
		//	modes
		//	joined channels map(channel name, *channels)
		//	registration state

		// optional considerations
		// 		away message 
		//		last activity timestamp

	public:
		Client();
		~Client();
};



#endif