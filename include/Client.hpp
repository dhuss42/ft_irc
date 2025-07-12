#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "ft_irc.hpp"

class Client
{
	private:
		std::string _nick;
		// 	socket fd
		//	pointer to server

		//  read buffer for the client
		//  write buffer
		std::string _buffer;

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

	std::string getNick();

	char*	getBufferPtr(void);
	size_t	getBufferSize(void);
	std::string getBuffer(void);
	void	setBuffer(uint8_t ascii, size_t len);
};

#endif