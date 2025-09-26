/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 14:00:35 by dhuss             #+#    #+#             */
/*   Updated: 2025/09/25 17:07:02 by dhuss            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

//================================> Orthodox Form <================================//

/*----------------------*/
/* Constructor			*/
/*----------------------*/
Channel::Channel(std::string name) : _name(name)
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
Channel::~Channel()
{
}

//================================> public member Methods <================================//


//================ Messaging ================//

/*------------------------------------------------------*/
/* Sends message to all users in channel except sender	*/
/*------------------------------------------------------*/
void	Channel::broadcast(const std::string& msg, Client* sender)
{
	if (sender)
	{
		for (auto it = _users.begin(); it != _users.end(); it++)
		{
			if (it->second != sender)
			{
				std::string prefix;
				if (isOperator(sender))
					prefix = "@";
				prefix += sender->getNick() + "!" + sender->getUsername() + "@" + sender->getHostname() + " PRIVMSG " + getName() + " :";
				it->second->sendMsg(prefix, msg);
			}
		}
	}
}

//================ Channel Operations ================//

//<<<<<<<<<<<<<<<MODE>>>>>>>>>>>>//
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

//<<<<<<<<<<<<<<<MODE>>>>>>>>>>>>//
/*--------------------------------------------------*/
/* returns active parameters as string				*/
/*--------------------------------------------------*/
std::string	Channel::getActiveChannelParameters(void) const
{
	std::string parameters = " ";

	if (_pswrdTgle)
		parameters += _password + " ";
	if (_usrLmtTgl)
		parameters += std::to_string(_userLimit);
	return (parameters);
}

//<<<<<<<<<<<<<<<TOPIC>>>>>>>>>>>>//
// the return value can be changed to adjust to the parsing logic
/*--------------------------------------------------*/
/* checks if only Operator can change topic			*/
/*	-	checks if client is Operator				*/
/* changes the topic								*/
/*--------------------------------------------------*/
void	Channel::changeTopic(const std::string& topic, const Client* client)
{
	if (_topicOp && !isOperator(client))
	{
		std::cout << "[DEBUG] only Operator can change topic and " << client->getNick() << " is not an operator for this channel" << std::endl;
		return ;
	}
	else
		setTopic(topic);
}

//<<<<<<<<<<<<<<<JOIN>>>>>>>>>>>>//
/*--------------------------------------------------*/
/* returns a string of all joined Users				*/
/*--------------------------------------------------*/
const std::string Channel::getJoinedUsers(void) const
{
	std::string userList;
	for (auto it = _users.rbegin(); it != _users.rend(); it++)
	{
		if (isOperator(it->second))
			userList += "@";
		else
			userList += " ";
		userList += it->first + " ";
	}
	if (!userList.empty())
		userList.pop_back();
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


//<<<<<<<<<<<<<<<NOT SURE IF NEEDED>>>>>>>>>>>>//
/*--------------------------------------------------*/
/* returns amount of users in channel				*/
/*--------------------------------------------------*/
size_t Channel::getNbrUsers(void) const
{
	return (_users.size());
}

//================ Adding & Removing clients ================//

//<<<<<<<<<<<<<<<JOIN>>>>>>>>>>>>//
// the return value can be changed to adjust to the parsing logic
/*--------------------------------------------------*/
/* adds users to channel							*/
/* - checks for different modes						*/
/*--------------------------------------------------*/
bool	Channel::addUser(Client* client, const std::string& password)
{
	if (client)
	{
		if (_usrLmtTgl && _users.size() >= _userLimit)
		{
			std::cout << "[DEBUG] user limit is set and userLimit " << _userLimit << " is reached!" << std::endl;
			return (false);
		}
		if (_invOnly && !isInvited(client))
		{
			std::cout << "[DEBUG] invite Only is set and user is not invited!" << std::endl;
			return (false);
		}
		if (_pswrdTgle && password != _password)
		{
			std::cout << "[DEBUG] password is set and wrong password!" << std::endl;
			return (false);
		}
		auto it = _users.find(client->getNick());
		if (it == _users.end())
		{
			std::cout << "[DEBUG] Client added ! " << client->getNick() << " to " << this->getName() << std::endl;
			_users[client->getNick()] = client;
			client->addToJoinedChannels(this);
			if (_invOnly)
				removeInvUsers(client);
			client->sendMsg("irc_custom", "available commands: JOIN, MODE, KICK, PART, QUIT, PRIVMSG/NOTICE"); // not correct format and more needs to be sent maybe outside the method
		}
		else
			std::cout << "[DEBUG] Client is already part of the Channel!" << std::endl;
		std::cout << "[DEBUG] " << getJoinedUsers() << std::endl;
	}
	return (true);
}

//<<<<<<<<<<<<<<<INVITE>>>>>>>>>>>>//
// the return value can be changed to adjust to the parsing logic
/*--------------------------------------------------*/
/* invite user to channel							*/
/*--------------------------------------------------*/
void	Channel::inviteUser(Client* inviter, Client* invited)
{
	if (!isOperator(inviter))
	{
		std::cout << GREEN "[DEBUG] " << inviter->getNick() << "[inviter] is not a channel operator"  WHITE << std::endl;
		return ;
	}
	if (!inviter->getServer()->isClient(invited->getNick()))
	{
		std::cout << GREEN "[DEBUG] " << invited->getNick() << "[invited] is not on the server"  WHITE << std::endl;
		return ;
	}
	if (!isUser(inviter))
	{
		std::cout << GREEN "[DEBUG] " << invited->getNick() << "[invited] is not in the channel"  WHITE << std::endl;
		return ;
	}
	auto it = _users.find(invited->getNick());
	if (it == _users.end())
	{
		std::cout << GREEN "[DEBUG] " << invited->getNick() << " is not in channel. Inviting now..." WHITE << std::endl;
		addInvUsers(invited);
	}
}

//<<<<<<<<<<<<<<<KICK>>>>>>>>>>>>//
// the return value can be changed to adjust to the parsing logic
/*--------------------------------------------------*/
/* kicks User out of channel						*/
/*--------------------------------------------------*/
void	Channel::kickUser(Client* kicker, const std::string& kicked)
{
	auto it = _users.find(kicked);
	if (it == _users.end())
	{
		std::cout << GREEN "[DEBUG] " << kicked << " is not in channel " << std::endl;
		return ;
	}
	if (!isOperator(kicker))
	{
		std::cout << GREEN "[DEBUG] " << kicker->getNick() << " is not an operator" << std::endl;
		return ;
	}
	else
	{
		std::cout << GREEN "[DEBUG] " << kicker->getNick() << " is kicking " << kicked << std::endl;
		removeUser(_users[kicked]);
	}
}

//<<<<<<<<<<<<<<<PART & JOIN & KICK>>>>>>>>>>>>//
/*----------------------------------------------------------------------*/
/* removes user from Channel for KICK and PART and client disconnect	*/
/*----------------------------------------------------------------------*/
void	Channel::removeUser(Client* client)
{
	if (client)
	{
		std::cout << "[DEBUG] Removing client with nick: " << client->getNick() << std::endl;
		std::cout << "[DEBUG] Before removing users_size: " << _users.size() << std::endl;
		std::cout << "[DEBUG] Before removing operators_size: " << _operators.size() << std::endl;
		if (_users.find(client->getNick()) != _users.end())
		{
			// client->removeFromJoinedChannels(this);
			_users.erase(client->getNick());
			std::cout << GREEN "[DEBUG] Removed client with nick: " RESET << client->getNick() << std::endl;
		}
		if (_operators.find(client->getNick()) != _operators.end())
			_operators.erase(client->getNick());
		std::cout << "[DEBUG] After removing users_size: " << _users.size() << std::endl;
		std::cout << "[DEBUG] [DEBUG] After removing users_size: " << _operators.size() << std::endl;
	}
	// check outside if Channel has 0 members now
	// if so delete the channel object
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

std::string	Channel::getName(void)
{
	return (_name);
}

void		Channel::setName(std::string name)
{
	_name = name;
}

std::string Channel::getTopic(void)
{
	return (_topic);
}

void	Channel::setTopic(std::string topic)
{
	_topic = topic;
}

std::string Channel::getPassword(void)
{
	return (_password);
}

void	Channel::setPassword(std::string password)
{
		_password = password;
}

std::size_t	Channel::getUserLimit(void)
{
	return (_userLimit);
}

void	Channel::setUserLimit(std::size_t userLimit)
{
	_userLimit = userLimit;
}

// ======= set and get modes =======//
bool	Channel::getInvOnly(void)
{
	return (_invOnly);
}

bool	Channel::getTopicOp(void)
{
	return (_topicOp);
}

bool	Channel::getPasswordToggle(void)
{
	return (_pswrdTgle);
}

bool	Channel::getUserLimitToggle(void)
{
	return (_usrLmtTgl);
}

void	Channel::setInvOnly(bool value)
{
	_invOnly = value;
}

void	Channel::setTopicOp(bool value)
{
	_topicOp = value;
}

void	Channel::setPasswordToggle(bool value)
{
	_pswrdTgle = value;
}

void	Channel::setUserLimitToggle(bool value)
{
	_usrLmtTgl = value;
}
