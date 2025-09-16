#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Server.hpp"	//put class includes in the end of file(?)
#include "Message.hpp"	//put class includes in the end of file(?)

class Server;
class Message;

class Client
{
	private:
		std::string _nick;
		std::string _username;
		//  a netwide unique identifier
		//	hostname

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

	Message parser(std::string rawMessage);
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

	char*	getBufferPtr(void);
	size_t	getBufferSize(void);
	std::string& getBuffer(void);
	void	setBuffer(std::string str);
	void	setBufferToValue(uint8_t ascii, size_t len);

	void	setRemainder(std::string str);
	std::string getRemainder(void);

	//handler functions
	// not sure if bool or void
	bool handleCap(Message message);
	bool handleJoin(Message message);
	bool handlePass(Message message);
	bool handleNick(Message message);
	bool handleUser(Message message);
	bool handleMode(Message message);
	bool handleWhois(Message message);
	bool handlePing(Message message);
};

#endif
