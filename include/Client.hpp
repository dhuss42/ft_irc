#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "ft_irc.hpp"
#include "IrcErrors.hpp"
#include "IrcResponses.hpp"
#include "Server.hpp"	//put class includes in the end of file(?)
#include "Channel.hpp"
#include "Message.hpp"	//put class includes in the end of file(?)
#include "MessageHandler.hpp"

class Server;
class Message;
class Channel;

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
	void	sendResponse(std::string name, IrcResponseCode code, std::string reply);
	void	sendMsg(std::string name, std::string reply);
	void	sendError(std::string name, IrcErrorCode code, std::string reply);
	int		authentication();
	int		receiveMsg();

	void	setRegistered(bool state);
	void	setNickSet(bool state);
	void	setUsernameSet(bool state);
	void	setSocket(int socket);

	bool	getRegistered(void);
	bool	getNickSet(void);
	bool	getUsernameSet(void);
	int		getSocket(void);

	void	setRealname(std::string str);
	void	setHostname(std::string str);
	void	setUsername(std::string str);
	void	setNick(std::string str);

	std::string getNick() const;
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

	Server* getServer(void);

	//handler functions
	// not sure if bool or void
	// bool handleCap(Message message);
	// bool handleJoin(Message message);
	// bool handlePass(Message message);
	// bool handleNick(Message message);
	// bool handleMode(Message message);
	// bool handleWhois(Message message);
	// bool handlePing(Message message);
	bool handleUser(Message message);
	bool handlePrivmsg(Message message);
};

void	parseHandler(std::string rawMessage, Client &client, Server &server);

#endif
