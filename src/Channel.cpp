/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 14:00:35 by dhuss             #+#    #+#             */
/*   Updated: 2025/10/03 11:06:06 by dhuss            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

//================================> Constructor and Destructor <================================//

/*----------------------*/
/* Constructor			*/
/*----------------------*/
Channel::Channel(std::string name) : _name(name)
{
}

/*----------------------*/
/* Destructor			*/
/*----------------------*/
Channel::~Channel()
{
}

//================================> public member Methods <================================//


//================ Messaging ================//

/*------------------------------------------------------*/
/* Sends message to all users in channel except sender	*/
/*------------------------------------------------------*/
void	Channel::broadcast(const std::string& content, Client* sender, const std::string& command)
{
	if (sender)
	{
		std::string msg = ":" + sender->getNick() + "!" + sender->getUsername() + "@" + sender->getHostname() + " " + command + " :" + content + "\r\n";
		for (auto it = _users.begin(); it != _users.end(); ++it)
		{
			if (it->second != sender)
				it->second->sendRaw(msg);
		}
	}
}

//================ Channel Operations ================//

/*--------------------------------------------------*/
/* returns "+" and every identifier for active mode	*/
/*--------------------------------------------------*/
std::string	Channel::getActiveChannelModes(void) const
{
	std::string activeModes = "+";

	if (_invOnly)
		activeModes += "i";
	if (_pswrdTgle)
		activeModes += "k";
	if (_usrLmtTgl)
		activeModes += "l";
	if (_topicOp)
		activeModes += "t";
	return (activeModes);
}

/*--------------------------------------------------*/
/* returns active parameters as string				*/
/*--------------------------------------------------*/
std::string	Channel::getActiveChannelParameters(void) const
{
	std::string parameters;

	if (_pswrdTgle)
		parameters += " " + _password;
	if (_usrLmtTgl)
		parameters += " " + std::to_string(_userLimit);
	return (parameters);
}

/*--------------------------------------------------*/
/* returns a string of all joined Users				*/
/*--------------------------------------------------*/
const std::string Channel::getJoinedUsers(void) const
{
	std::string userList;

	for (const auto& userPair : _users)
	{
		if (!userList.empty())
			userList += ' ';
		if (isOperator(userPair.second)) {
			userList += '@';
		}
		userList += userPair.first;
	}
	return (userList);
}

//<<<<<<<<<<<<<<<PART>>>>>>>>>>>>//
/*--------------------------------------------------*/
/* checks if channel is empty						*/
/*--------------------------------------------------*/
bool	Channel::isEmpty(void) const
{
	return (_users.empty());
}

/*--------------------------------------------------*/
/* returns amount of users in channel				*/
/*--------------------------------------------------*/
size_t Channel::getNbrUsers(void) const
{
	return (_users.size());
}

//================ Adding & Removing clients ================//

/*--------------------------------------------------*/
/* adds users to channel							*/
/* - checks for different modes						*/
/*--------------------------------------------------*/
bool	Channel::addUser(Client* client, const std::string& password)
{
	if (client)
	{
		const std::string& serverName = client->getServer()->getName();
		if (_usrLmtTgl && _users.size() >= _userLimit)
		{
			client->sendError(serverName, IrcErrorCode::ERR_CHANNELISFULL, this->getName());
			return (false);
		}
		if (_invOnly && !isInvited(client))
		{
			client->sendError(serverName, IrcErrorCode::ERR_INVITEONLYCHAN, this->getName());
			return (false);
		}
		if (_pswrdTgle && password != _password)
		{
			client->sendError(serverName, IrcErrorCode::ERR_BADCHANNELKEY, this->getName());
			return (false);
		}
		auto it = _users.find(client->getNick());
		if (it == _users.end())
		{
			_users[toLower(client->getNick())] = client;
			client->addToJoinedChannels(this);
			if (_invOnly)
				removeInvUsers(client);
		}
	}
	return (true);
}

/*--------------------------------------------------*/
/* updates nick in channel container				*/
/*--------------------------------------------------*/
void	Channel::updateNickOnChannel(const std::string& oldNick, const std::string& newNick)
{
	auto it = _users.find(oldNick);
	if (it != _users.end())
	{
		Client* client = it->second;
		_users.erase(client->getNick());
		if (client)
			_users[toLower(newNick)] = client;
	}
}

/*----------------------------------------------------------------------*/
/* removes user from Channel for KICK and PART and client disconnect	*/
/*----------------------------------------------------------------------*/
void	Channel::removeUser(Client* client)
{
	if (client)
	{
		if (_users.find(client->getNick()) != _users.end())
			_users.erase(client->getNick());
		if (_operators.find(client->getNick()) != _operators.end())
			_operators.erase(client->getNick());
	}
}

/*----------------------------------------------------------------------*/
/* Return unordered_map of all Clients in Channel						*/
/*----------------------------------------------------------------------*/
std::map<std::string, Client*> Channel::getUsers(void)
{
	return (_users);
}

/*----------------------------------------------------------------------*/
/* Adds Client to Operator Container									*/
/*----------------------------------------------------------------------*/
void	Channel::addOperator(Client* client)
{
	if (client)
	{
		if (_operators.find(client->getNick()) == _operators.end())
		{
			_operators[client->getNick()] = client;
		}
	}
}

/*----------------------------------------------------------------------*/
/* Removes Client from Operator Container								*/
/*----------------------------------------------------------------------*/
void	Channel::removeOperator(Client* client)
{
	if (client)
	{
		if (_operators.find(client->getNick()) != _operators.end())
		{
			_operators.erase(client->getNick());
		}
	}
}

/*----------------------------------------------------------------------*/
/* Add to Invite Container of Channel									*/
/*----------------------------------------------------------------------*/
void	Channel::addInvUsers(Client* client)
{
	if (client)
	{
		if (_invitedUsers.find(client->getNick()) == _invitedUsers.end())
		{
			_invitedUsers[client->getNick()] = client;
		}
	}
}

/*----------------------------------------------------------------------*/
/* Remove from Invite Container of Channel								*/
/*----------------------------------------------------------------------*/
void	Channel::removeInvUsers(Client *client)
{
	if (client)
	{
		if (_invitedUsers.find(client->getNick()) != _invitedUsers.end())
		{
			_invitedUsers.erase(client->getNick());
		}
	}
}

//================ verify Clients ================//
/*----------------------*/
/* Check if Operator	*/
/*----------------------*/
bool	Channel::isOperator(const Client* client) const
{
	if (client)
	{
		if (_operators.find(client->getNick()) != _operators.end())
			return (true);
	}
	return (false);
}

//
/*--------------------------*/
/* Check if User in Channel	*/
/*--------------------------*/
bool	Channel::isUser(const Client* client) const
{
	if (client)
	{
		if (_users.find(client->getNick()) != _users.end())
			return (true);
	}
	return (false);
}

/*--------------------------------------*/
/* Check if User is invited to Channel	*/
/*--------------------------------------*/
bool	Channel::isInvited(const Client* client) const
{
	if (client)
	{
		if (_invitedUsers.find(client->getNick()) != _invitedUsers.end())
			return (true);
	}
	return (false);
}

//================ getters & setters ================//

/*--------------------------------------*/
/* returns Channel Name					*/
/*--------------------------------------*/
std::string	Channel::getName(void)
{
	return (_name);
}

/*--------------------------------------*/
/* changes channel name to <name>		*/
/*--------------------------------------*/
void		Channel::setName(std::string name)
{
	_name = name;
}

/*--------------------------------------*/
/* returns channel topic				*/
/*--------------------------------------*/
std::string Channel::getTopic(void)
{
	return (_topic);
}

/*--------------------------------------*/
/* sets channel topic to <topic>		*/
/*--------------------------------------*/
void	Channel::setTopic(std::string topic)
{
	_topic = topic;
}

/*--------------------------------------*/
/* returns channel password				*/
/*--------------------------------------*/
std::string Channel::getPassword(void)
{
	return (_password);
}

/*--------------------------------------*/
/* sets channel password				*/
/*--------------------------------------*/
void	Channel::setPassword(std::string password)
{
		_password = password;
}

/*--------------------------------------*/
/* returns channel user Limit			*/
/*--------------------------------------*/
std::size_t	Channel::getUserLimit(void)
{
	return (_userLimit);
}

/*--------------------------------------*/
/* sets channel user Limit				*/
/*--------------------------------------*/
void	Channel::setUserLimit(std::size_t userLimit)
{
	_userLimit = userLimit;
}

// ======= set and get modes =======//
/*--------------------------------------*/
/* checks if Channel is invite only		*/
/*--------------------------------------*/
bool	Channel::getInvOnly(void)
{
	return (_invOnly);
}

/*----------------------------------------------*/
/* checks if only Operator can change topic		*/
/*----------------------------------------------*/
bool	Channel::getTopicOp(void)
{
	return (_topicOp);
}

/*----------------------------------------------*/
/* checks if Channel has password				*/
/*----------------------------------------------*/
bool	Channel::getPasswordToggle(void)
{
	return (_pswrdTgle);
}

/*----------------------------------------------*/
/* checks if Channel has user Limit				*/
/*----------------------------------------------*/
bool	Channel::getUserLimitToggle(void)
{
	return (_usrLmtTgl);
}

/*----------------------------------------------*/
/* sets Channel invite only flag				*/
/*----------------------------------------------*/
void	Channel::setInvOnly(bool value)
{
	_invOnly = value;
}

/*----------------------------------------------*/
/* sets only Operator can change topic flag		*/
/*----------------------------------------------*/
void	Channel::setTopicOp(bool value)
{
	_topicOp = value;
}

/*----------------------------------------------*/
/* sets Password active flag					*/
/*----------------------------------------------*/
void	Channel::setPasswordToggle(bool value)
{
	_pswrdTgle = value;
}

/*----------------------------------------------*/
/* sets User Limit flag							*/
/*----------------------------------------------*/
void	Channel::setUserLimitToggle(bool value)
{
	_usrLmtTgl = value;
}
