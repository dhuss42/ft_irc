/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 14:42:13 by dhuss             #+#    #+#             */
/*   Updated: 2025/10/03 10:57:02 by dhuss            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

//================================> Constructor and Destructor <================================//

/*----------------------*/
/* Constructor			*/
/*----------------------*/
Client::Client(int fd, Server* server) : _server(server), _socket(fd)
{
	std::cout << GREEN << "new connection" WHITE << std::endl;
}

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

Channel* Client::getJoinedChannel(const std::string& name)
{
	auto it = _joinedChannels.find(name);
	if (it != _joinedChannels.end())
		return (it->second);
	return (nullptr);
}

bool	Client::isJoinedChannel(const std::string& name)
{
	if (_joinedChannels.find(name) != _joinedChannels.end())
		return (true);
	return (false);
}

/*------------------------------------------------------------------------------*/
/* add the Channel to the container storing all the Channels user is part of	*/
/*------------------------------------------------------------------------------*/
void	Client::addToJoinedChannels(Channel* channel)
{
	if (_joinedChannels.find(channel->getName()) == _joinedChannels.end())
		_joinedChannels[channel->getName()] = channel;
}

/*----------------------------------------------------------------------------------*/
/* remove the Channel from the container storing all the Channels user is part of	*/
/*----------------------------------------------------------------------------------*/
void	Client::removeFromJoinedChannels(const std::string& name)
{
	if (_joinedChannels.find(name) != _joinedChannels.end())
			_joinedChannels.erase(name);
}

/*----------------------------------------------------------------------------------*/
/* remove User from all Channels he has joined										*/
/* remove all Channels from container storing all the Channels user is part of		*/
/*----------------------------------------------------------------------------------*/
void	Client::removeFromAllJoinedChannels()
{
	for (auto it = _joinedChannels.begin(); it != _joinedChannels.end(); ++it)
	{
		it->second->removeUser(this);
		if (it->second->getNbrUsers() == 0)
			_server->removeChannel(it->second);
	}
	_joinedChannels.clear();
}

/*----------------------------------------------------------------------------------*/
/* update Nick in Client container and channel container							*/
/*----------------------------------------------------------------------------------*/
void	Client::updateNick(const std::string& oldNick, const std::string& newNick)
{
	std::map<std::string, Client*> channelUsers;
	for (auto it = _joinedChannels.begin(); it != _joinedChannels.end(); ++it)
	{
		channelUsers = it->second->getUsers();
		auto iter = channelUsers.find(oldNick);
		if (iter != channelUsers.end())
		{
			Client* clientPtr = iter->second;
			channelUsers.erase(_nick);
			channelUsers[toLower(newNick)] = clientPtr;
			it->second->updateNickOnChannel(oldNick, newNick);
		}
	}
}

/*----------------------------------------------------------------------------------*/
/* return unordered map with all channels client is part of							*/
/*----------------------------------------------------------------------------------*/
std::unordered_map<std::string, Channel*> Client::getJoinedChannels(void)
{
	return (_joinedChannels);
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
	if (received <= 0)
		return (-1);
	else
	{
		std::string rec = std::string(tmp, received);
		std::string fullBuffer = _remainder + rec;
		_remainder = "";
		std::size_t pos;

		// std::cout << "[" << _nick << "] received: " << rec;
		// if (fullBuffer.back() != '\n')
		// 	std::cout << " ";

		while ((pos = fullBuffer.find("\r\n")) != std::string::npos)
		{
			_buffer = fullBuffer.substr(0, pos);
			parseHandler(_buffer, *this, *_server);
			fullBuffer = fullBuffer.substr(pos + 2);;
		}
		_remainder = fullBuffer;
	}
	_buffer.clear();
	return (0);
}

/*------------------------------------------------------------------*/
/* sends Raw replies to client										*/
/*------------------------------------------------------------------*/
void	Client::sendRaw(std::string msg) // what about this
{
	ssize_t sent = send(_socket, msg.c_str(), msg.size(), 0);
	if (sent <= 0)
		throw (Errors(ErrorCode::E_SND));
}

/*------------------------------------------------------------------*/
/* sends replies to client											*/
/*	- replies are patched together for the correct format for irssi */
/*		- every msg sent must end in \r\n							*/
/*------------------------------------------------------------------*/
void	Client::sendMsg(std::string name, std::string reply)
{
	reply = ":" + name + " " + reply + "\r\n";
	if (send(_socket, reply.c_str(), reply.size(), 0) <= 0)
		throw (Errors(ErrorCode::E_SND));
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

	sendResponse(_server->getName(), IrcResponseCode::RPL_WELCOME, "WelcomeMessage");
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
