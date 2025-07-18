#include "Client.hpp"

// Thoughts on Inheritance
// 	user inhertis from client
//  service inherits from client
//  moderator inherits from user

Client::Client(int fd) : _nick("default"), _socket(fd)
{
}

Client::~Client()
{
	if (_socket)
		close(_socket);
	std::cout << CYAN << "[" << _nick << "]: disconnected" WHITE << std::endl;
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

std::string Client::getNick(void)
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
