#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "ft_irc.hpp"

class Server;

class Client
{
	private:
		std::string _nick;
		std::string _username;
		// Server		*server;
		// 	socket fd
		int _socket;
		//	pointer to server

		//  read buffer for the client
		//  write buffer
		std::string _buffer;
		std::string	_remainder;

		//  a netwide unique identifier
		//	nickname
		//	username
		//	hostname
		//	modes
		//	joined channels map(channel name, *channels)
		//	registration state
		bool	_registered = false;
		bool	_nickSet = false;
		bool	_usernameSet = false;


		// optional considerations
		// 		away message
		//		last activity timestamp

	public:
		Client(int fd);
		~Client();

	bool	getRegistered(void);
	void	setRegistered(bool state);

	bool	getNickSet(void);
	void	setNickSet(bool state);

	bool	getUsernameSet(void);
	void	setUsernameSet(bool state);

	void	setSocket(int socket);
	int		getSocket(void);

	std::string getNick();
	void	setNick(std::string str);

	char*	getBufferPtr(void);
	size_t	getBufferSize(void);
	std::string& getBuffer(void);
	void	setBuffer(std::string str);
	void	setBufferToValue(uint8_t ascii, size_t len);

	void	setRemainder(std::string str);
	std::string getRemainder(void);
};

#endif