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
	removeFromJoinedCahnnels();
	if (_socket >= 0)
		close(_socket);
	std::cout << CYAN << "[" << _nick << "]: disconnected" WHITE << std::endl;
}

void	Client::addToJoinedChannels(Channel* channel)
{
	if (channel)
		_joinedChannels.emplace(channel->getName(), channel);
}

void	Client::removeFromJoinedCahnnels()
{
	for (auto it = _joinedChannels.begin(); it != _joinedChannels.end(); ++it)
		it->second->removeUser(this);
	_joinedChannels.clear();
}

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
	// std::cout << MAGENTA "[DEBUGG] ===== receiveMsg ======= " WHITE << std::endl;
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
			// std::cout << BOLDCYAN << "[DEBUG] buffer: " << _buffer << RESET << std::endl;
			parseHandler(_buffer, *this, *_server);
			// pseudoParser(_buffer);
			fullBuffer = fullBuffer.substr(pos + 2);;
		}
		_remainder = fullBuffer;
		// if (!fullBuffer.empty())
			// std::cout << BOLDCYAN << "[DEBUG] remainder: " << _remainder << RESET << std::endl;
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
	// std::cout << "[DEBUG] current reply: " << name << std::endl;
	// std::cout << "[DEBUG] sendMsg to " << getNick() << " socket=" << _socket << " msg=" << reply << std::endl;
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
