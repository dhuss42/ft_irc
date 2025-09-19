#include "Client.hpp"


// it might make sense to pass pollfd, this would save me from having two unorderedmaps with clients
// I could just have the clients nick and a pointer to the client
// instead of having to containers I could also extract the value I need
Client::Client(int fd, Server* server) : _nick("default"), _username("default"), _realname("default"), _hostname(server->getName()), _server(server), _socket(fd)
{
	(void) _server;
}

Client::~Client()
{
	if (_socket >= 0)
		close(_socket);
	std::cout << CYAN << "[" << _nick << "]: disconnected" WHITE << std::endl;
}


// /*----------------------------------------------------------*/
// /* NOT FOR THE FINAL PRODUCT								*/
// /* "parses" the msgs sent by irssi							*/
// /*	- only implemented for authentication handshake to work */
// /* should be handled by the parser							*/
// /*----------------------------------------------------------*/
// void Client::pseudoParser(std::string message)
// {
// 	std::size_t pos = 0;
// 	(void) pos;

// 	std::cout << MAGENTA << "===========pseudo Parser==========" WHITE << std::endl;
// 	std::cout << message << std::endl;
// 	if (message.find("PASS") == 0)
// 	{
// 		std::cout << "[DEBUG] PASS" << std::endl;
// 		pos = message.find(" ");
// 		// compare what follows the space to the password
// 		// set registered to true
// 		_registered = true;
// 	}
// 	else if (message.find("CAP LS 302")  == 0)
// 	{
// 		sendMsg("irc_custom", "CAP * LS :"); // get the name somewhere
// 	}
// 	else if (message.find("PING")  == 0)
// 	{
// 		std::cout << " [DEBUG] PING" << std::endl;
// 		sendMsg("irc_custom", "PONG irc_custom"); // get the name somewhere
// 	}
// 	else if (message.find("NICK") == 0)
// 	{
// 		std::cout << "[DEBUG] NICK" << std::endl;
// 		_nick = message.substr(5);
// 		// copy every following space into client Nick
// 		_nickSet = true;
// 	}
// 	else if (message.find("USER") == 0)
// 	{
// 		std::cout << "[DEBUG] USER " << std::endl;
// 		std::cout << message << std::endl;

// 		std::smatch matches;
// 		std::regex pattern("^(USER) (\\S+) (\\S+) (\\S+) :(.+)$");
// 		if (std::regex_match(message, matches, pattern))
// 		{
// 			_username = matches[2];
// 			_hostname = matches[4];
// 			_realname = matches[5];
// 			std::cout << "[DEBUG] username: " << _username << std::endl;
// 			std::cout << "[DEBUG] hostname: " << _hostname << std::endl; // not sure if better to have servername here or IP Address, this seems slower than doing it constructor
// 			std::cout << "[DEBUG] realname: " << _realname << std::endl;
// 		}
// 		_usernameSet = true;
// 	}
// 	else if (message.find("JOIN") == 0)
// 	{
// 		std::cout << "[DEBUG] JOIN: " << message << std::endl;
// 		std::size_t pos = message.find('#');
// 		if (pos == std::string::npos)
// 		{

// 		}
// 		else
// 		{
// 			if (_server->isChannel(message.substr(pos)))
// 				std::cout << "channel exists already" << std::endl;
// 			Channel *channel = _server->getChannel(message.substr(pos));
// 			if (!channel)
// 			{
// 				std::cout << "[DEBUG] adding channel with name " << message.substr(pos) << std::endl;
// 				channel = new Channel(message.substr(pos));
// 				_server->addChannel(channel);
// 			}
// 			channel->addUser(this);
// 		}
// 	}
// 	else if (message.find("PRIVMSG") == 0) //PRIVMSG chan msg
// 	{
// 		// PRIVMSG #chan :hallo
// 		// currently segfaults because shitty pseudoparser
// 		std::cout << "[DEBUG] PRIVMSG" << std::endl;
// 		std::cout << message << std::endl;
// 		std::cout << message.length() << std::endl;
// 		std::size_t pos = message.find('#');
// 		std::cout << "[DEBUG] pos " << pos << std::endl;
// 		std::size_t pos2 = message.find(':');
// 		std::cout << "[DEBUG] pos2 " << pos2 << std::endl;
// 		Channel *channel = _server->getChannel(message.substr(pos, 5)); // channel name can only be 4 chars -> better extract everything up to colon
// 		std::cout << "[DEBUG] channel name " << message.substr(pos, 5) << std::endl;
// 		if (channel)
// 			std::cout << "[DEBUG] exists" << message.substr(pos, 5) << std::endl;
// 		channel->broadcast(message.substr(pos2 + 1), this); // better extract everything after colon
// 		std::cout << "[DEBUG] test2" << std::endl;
// 	}
// }

// Needs adjustment after proper Parser handling
int	Client::authentication()
{
	while(!(_nickSet && _usernameSet && _registered))
	{
		if (receiveMsg() == -1)
			return (-1);
	}
	sendMsg("irc_custom", "001 " + _nick + " :Welcome to the IRC server"); // server replies need to be handled more gracefully
	return (1);
}

//==============Messaging==============//

/*--------------------------------------------------------------------------*/
/* receives messages from client											*/
/*	- recv stores msg in tmp buffer 										s*/
/*	- returns -1 on error and 0 on closed connection						*/
/*	- else returns nbr of bytes received 									*/
/*		- gets remainder from las recv call									*/
/*		- Loop: finds delimiters "\r\n" in received msg						*/
/*		- sets clients buffer to everything preceeding delimiters			*/
/*		- stores remaing message in a buffer and repeats the loop			*/
/*		- after loop the remainder is stored in clients remainder			*/
/*		- buffer is set to 0												*/
/*--------------------------------------------------------------------------*/
int	Client::receiveMsg()
{
	std::cout << MAGENTA "[DEBUGG] ===== receiveMsg ======= " WHITE << std::endl;
	char	tmp[512] = {0};
	int		received = recv(_socket, tmp, sizeof(tmp), 0);

	if (received == -1)
	{
		std::cerr << RED "Error: recv" << WHITE << std::endl;
		return (-1);
	}
	else if (received == 0)
	{
		std::cout << CYAN << "[" << _nick << "] closed their connection" << std::endl; // needs cleanup
		return (-1);
	}
	else
	{
		// std::cout << YELLOW "[DEBUGG] recv is non negative" WHITE << std::endl;

		std::string fullBuffer = _remainder + std::string(tmp, received);
		_remainder = "";

		std::size_t pos;
		while ((pos = fullBuffer.find("\r\n")) != std::string::npos)
		{
			_buffer = fullBuffer.substr(0, pos);
			std::cout << BOLDCYAN << "[DEBUG] buffer: " << _buffer << RESET << std::endl;
			parser(_buffer);
			// pseudoParser(_buffer);
			fullBuffer = fullBuffer.substr(pos + 2);;
		}
		_remainder = fullBuffer;
		if (!fullBuffer.empty())
			std::cout << BOLDCYAN << "[DEBUG] remainder: " << _remainder << RESET << std::endl;
	}
	std::cout << GREEN << "[" << _nick << "]" << " received: " << _buffer << WHITE << std::endl;
	_buffer.clear();
	return (0);
}

/*------------------------------------------------------------------*/
/* sends replies to client											*/
/*	- replies are patched together for the correct format for irssi */
/*		- every msg sent must end in \r\n							*/
/*		- :server 001 nickname :Welcome to IRC Server				*/
/*------------------------------------------------------------------*/
void	Client::sendMsg(std::string name, std::string reply)
{
	reply = ":" + name + " " + reply + "\r\n";
	std::cout << "[DEBUG] current reply: " << name << std::endl;
	std::cout << "[DEBUG] sendMsg to " << getNick() << " socket=" << _socket << " msg=" << reply << std::endl;
	if (send(_socket, reply.c_str(), reply.size(), 0) <= 0) // uncertain about the zero at the moment
	{
		throw (Errors(ErrorCode::E_SND)); // uncertain about wether it bubbles up correctly to the next catch
	}
}

//==============getters and setters==============//

void	Client::setRealname(std::string str)
{
	_realname = str;
}

void	Client::setHostname(std::string str)
{
	_hostname = str;
}

void	Client::setUsername(std::string str)
{
	_username = str;
}

bool	Client::getRegistered(void)
{
	return (_registered);
}

void	Client::setRegistered(bool state)
{
	_registered = state;
}

bool	Client::getNickSet(void)
{
	return (_nickSet);
}

void	Client::setNickSet(bool state)
{
	_nickSet = state;
}

bool	Client::getUsernameSet(void)
{
	return (_usernameSet);
}

void	Client::setUsernameSet(bool state)
{
	_usernameSet = state;
}


void	Client::setSocket(int socket)
{
	_socket = socket;
}

int Client::getSocket(void)
{
	return (_socket);
}

std::string Client::getNick(void) const
{
	return (_nick);
}

void	Client::setNick(std::string str)
{
	_nick = str;
}

std::string& Client::getBuffer(void)
{
	return (_buffer);
}

char* Client::getBufferPtr(void)
{
	return (_buffer.data());
}

size_t Client::getBufferSize(void)
{
	return (_buffer.size());
}

void	Client::setBufferToValue(uint8_t ascii, size_t len)
{
	std::memset(_buffer.data(), ascii, len);
}

void	Client::setBuffer(std::string str)
{
	_buffer = str;
}

void	Client::setRemainder(std::string str)
{
	_remainder = str;
}

std::string	Client::getRemainder(void)
{
	return(_remainder);
}

std::string	Client::getUsername()
{
	return (_username);
}

std::string	Client::getHostname()
{
	return (_hostname);
}

std::string	Client::getRealname()
{
	return (_realname);
}

Server* Client::getServer(void)
{
	return (_server);
}

// Constructor
// if a client makes a connection create Client object
// instantiate attributes
// 	socket fd
//	pointer to server
//
//  read buffer for the client
//  write buffer
//
//  a netwide unique identifier
//	nickname
//	username
//	hostname
//	modes
//	joined channels
//	registration state
//

// optional considerations
// 		away message
//		last activity timestamp
//

// Destructor
// 	close socket?
//	notify server to remove from pollfd and client list

// Buffer handling Methods
// append received data into read buffer

// check for complete lines in read buffer

// Clear Buffer when necessary
