/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 14:42:13 by dhuss             #+#    #+#             */
/*   Updated: 2025/09/25 16:46:14 by dhuss            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

//================================> Orthodox Form <================================//

// it might make sense to pass pollfd, this would save me from having two unorderedmaps with clients
// I could just have the clients nick and a pointer to the client
// instead of having to containers I could also extract the value I need

/*----------------------*/
/* Constructor			*/
/*----------------------*/
Client::Client(int fd, Server* server) : _nick("default"), _username("default"), _realname("default"), _hostname(server->getName()), _server(server), _socket(fd)
{
}

/*----------------------*/
/* Copy Constructor		*/
/*----------------------*/

/*----------------------*/
/* Assignment Overload	*/
/*----------------------*/

/*----------------------*/
/* Destructor			*/
/*----------------------*/
Client::~Client()
{
	removeFromAllJoinedChannels();
	if (_socket >= 0)
		close(_socket);
	std::cout << CYAN << "[" << _nick << "]: disconnected" WHITE << std::endl;
}


//============== Channel Data ==============//

/*------------------------------------------------------------------------------*/
/* add the Channel to the container storing all the Channels user is part of	*/
/*------------------------------------------------------------------------------*/
void	Client::addToJoinedChannels(Channel* channel)
{
	if (channel)
		_joinedChannels.emplace(channel->getName(), channel);
}

// RESULTED IN SEGFAULTS BECAUSE I CODED WHEN I SHOULD HAVE STOPPED ALREADY
// /*----------------------------------------------------------------------------------*/
// /* remove the Channel from the container storing all the Channels user is part of	*/
// /*----------------------------------------------------------------------------------*/
// void	Client::removeFromJoinedChannels(Channel* channel)
// {
// 	if (channel)
// 	{
// 		if (_joinedChannels.find(channel->getName()) != _joinedChannels.end())
// 			_joinedChannels.erase(channel->getName());
// 	}
// }

/*----------------------------------------------------------------------------------*/
/* remove User from all Channels he has joined										*/
/* remove all Channels from container storing all the Channels user is part of		*/
/*----------------------------------------------------------------------------------*/
void	Client::removeFromAllJoinedChannels()
{
	for (auto it = _joinedChannels.begin(); it != _joinedChannels.end(); ++it)
		it->second->removeUser(this);
	_joinedChannels.clear();
}

//============== Messaging ==============//

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
	char	tmp[512] = {0};
	int		received = recv(_socket, tmp, sizeof(tmp), 0);

	if (received == -1)
	{
		std::cerr << RED "Error: recv" << WHITE << std::endl;
		return (-1);
	}
	else if (received == 0)
	{
		std::cout << CYAN << "[" << _nick << "] closed their connection" << std::endl; // needs cleanup -> maybe change to setRegisfailed?
		return (-1);
	}
	else
	{
		std::string fullBuffer = _remainder + std::string(tmp, received);
		_remainder = "";

		std::size_t pos;
		while ((pos = fullBuffer.find("\r\n")) != std::string::npos)
		{
			_buffer = fullBuffer.substr(0, pos);
			parseHandler(_buffer, *this, *_server);
			fullBuffer = fullBuffer.substr(pos + 2);;
		}
		_remainder = fullBuffer;
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
	if (send(_socket, reply.c_str(), reply.size(), 0) <= 0) // uncertain about the zero at the moment
	{
		throw (Errors(ErrorCode::E_SND)); // uncertain about wether it bubbles up correctly to the next catch
	}
}

//============== getters and setters ==============//

int	Client::getSocket(void) // [DEBUGGING]
{
	return (_socket);
}

/*--------------------------------------*/
/* set _realname to str					*/
/*--------------------------------------*/
void	Client::setRealname(const std::string& str)
{
	_realname = str;
}

/*--------------------------------------*/
/* set _hostname to str					*/
/*--------------------------------------*/
void	Client::setHostname(const std::string& str)
{
	_hostname = str;
}

/*--------------------------------------*/
/* set _username to str					*/
/*--------------------------------------*/
void	Client::setUsername(const std::string& str)
{
	_username = str;
}

/*--------------------------------------*/
/* set _nick to str						*/
/*--------------------------------------*/
void	Client::setNick(const std::string& str)
{
	_nick = str;
}

/*--------------------------------------*/
/* set _regisFailed to value			*/
/*--------------------------------------*/
void	Client::setDisconnect(bool value)
{
	_disconnect = value;
}

/*--------------------------------------*/
/* returns _realname					*/
/*--------------------------------------*/
std::string	Client::getRealname()
{
	return (_realname);
}

/*--------------------------------------*/
/* returns _hostname					*/
/*--------------------------------------*/
std::string	Client::getHostname()
{
	return (_hostname);
}

/*--------------------------------------*/
/* returns _username					*/
/*--------------------------------------*/
std::string	Client::getUsername()
{
	return (_username);
}

/*--------------------------------------*/
/* returns _nick						*/
/*--------------------------------------*/
std::string Client::getNick(void) const
{
	return (_nick);
}

/*--------------------------------------*/
/* returns _regisFailed					*/
/*--------------------------------------*/
bool	Client::getDisconnect(void)
{
	return (_disconnect);
}

/*--------------------------------------*/
/* returns server pointer				*/
/*--------------------------------------*/
Server* Client::getServer(void)
{
	return (_server);
}



//------------can be deleted once authentication is updated------------//
int	Client::authentication()
{
	while(!(_nickSet && _usernameSet && _registered))
	{
		if (receiveMsg() == -1)
			return (-1);
	}
	sendMsg("irc_custom", "001 " + _nick + " :Welcome to the IRC server");
	return (1);
}

//------------can be deleted once authentication is updated------------//
bool	Client::getRegistered(void)
{
	return (_registered);
}
//------------can be deleted once authentication is updated------------//
void	Client::setRegistered(bool state)
{
	_registered = state;
}
//------------can be deleted once authentication is updated------------//
bool	Client::getNickSet(void)
{
	return (_nickSet);
}
//------------can be deleted once authentication is updated------------//
void	Client::setNickSet(bool state)
{
	_nickSet = state;
}
//------------can be deleted once authentication is updated------------//
bool	Client::getUsernameSet(void)
{
	return (_usernameSet);
}
//------------can be deleted once authentication is updated------------//
void	Client::setUsernameSet(bool state)
{
	_usernameSet = state;
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
