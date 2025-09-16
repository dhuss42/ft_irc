#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Server.hpp"

class Server;

class Client
{
	private:
		std::string _nick;
		std::string _username;
		std::string _realname;
		std::string _hostname;

		Server*		_server;
		int 		_socket;

		std::string _buffer;
		std::string	_remainder;
		//  write buffer ?

		//	modes

		std::unordered_map<std::string, Channel*> _joinedChannels;

		// registration state
		bool	_registered = false;
		bool	_nickSet = false;
		bool	_usernameSet = false;

		// optional considerations
		// 		away message
		//		last activity timestamp

	public:
		Client(int fd, Server* server);
		~Client();

	void	pseudoParser(std::string message);
	void	sendMsg(std::string name, std::string reply);
	int		authentication();
	int		receiveMsg();

	bool	getRegistered(void);
	void	setRegistered(bool state);

	bool	getNickSet(void);
	void	setNickSet(bool state);

	bool	getUsernameSet(void);
	void	setUsernameSet(bool state);

	void	setSocket(int socket);
	int		getSocket(void);

	std::string getNick() const;
	void	setNick(std::string str);

	std::string	getUsername();
	std::string getHostname();
	std::string getRealname();


	char*	getBufferPtr(void);
	size_t	getBufferSize(void);
	std::string& getBuffer(void);
	void	setBuffer(std::string str);
	void	setBufferToValue(uint8_t ascii, size_t len);

	void	setRemainder(std::string str);
	std::string getRemainder(void);
};

#endif