#include "Channel.hpp"

//================ Orthodox Form ================//

Channel::Channel(std::string name) : _name(name), _invOnly(false), _topicOp(true), _pswrdTgle(false), _usrLmtTgl(false)
{
}

Channel::~Channel()
{
}

//================ Messaging ================//

/*------------------------------------------------------*/
/* Sends message to all users in channel except sender	*/
/*------------------------------------------------------*/
void	Channel::broadcast(const std::string& msg, Client* sender)
{
	std::cout << MAGENTA << "===========boradcast==========" WHITE << std::endl;
	std::cout << "[DEBUG] before nullptr check" << std::endl;
	std::cout << "[DEBUG] msg " << msg << std::endl;
	if (sender)
	{
		int counter = 0;
		std::cout << "[DEBUG] inside if(sender)" << std::endl;
		std::cout << "[DEBUG] sender is: " << sender->getNick() << std::endl;
		std::cout << "[DEBUG] _users.size(): " << _users.size() << std::endl;

		for (auto it = _users.begin(); it != _users.end(); it++)
		{
			std::cout << "[DEBUG] users: " << it->second->getNick() << std::endl;
			std::cout << "[DEBUG] run: " << counter++ << std::endl;
			std::cout << "[DEBUG] sender ptr: " << sender << std::endl;
			std::cout << "[DEBUG] sender socket: " << sender->getSocket() << std::endl;
			std::cout << "[DEBUG] user ptr:   " << it->second << std::endl;
			std::cout << "[DEBUG] user socket:   " << it->second->getSocket() << std::endl;
			if (it->second != sender)
			{
				std::cout << "[DEBUG]" << "_user is not equal to sender" << std::endl;
				std::string prefix = sender->getNick() + "!" + sender->getUsername() + "@" + sender->getHostname() + " PRIVMSG " + getName() + " :";
				std::cout << "[DEBUG] prefix: " << prefix << std::endl;
				it->second->sendMsg(prefix, msg);
			}
		}
	}
}

//================ Channel Operations ================//

//<<<<<<<<<<<<<<<TOPIC>>>>>>>>>>>>//
// the return value can be changed to adjust to the parsing logic
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
// gets List of all the Users that are part of the Channel and returns a string to be further processed
const std::string Channel::getJoinedUsers(void) const
{
	std::string userList;
	for (auto it = _users.rbegin(); it != _users.rend(); it++)
	{
		if (isOperator(it->second))
			userList += "@";
		userList += it->first + " ";
	}
	if (!userList.empty())
		userList.pop_back();
	return (userList);
}

//================ Adding & Removing clients ================//

//<<<<<<<<<<<<<<<JOIN>>>>>>>>>>>>//
// the return value can be changed to adjust to the parsing logic
void	Channel::addUser(Client* client, const std::string& password)
{
	if (client)
	{
		if (_usrLmtTgl && _users.size() >= _userLimit)
		{
			std::cout << "[DEBUG] user limit is set and userLimit " << _userLimit << " is reached!" << std::endl;
			return ;
		}
		if (_invOnly && !isInvited(client))
		{
			std::cout << "[DEBUG] invite Only is set and user is not invited!" << std::endl;
			return ;
		}
		if (_pswrdTgle && password != _password)
		{
			std::cout << "[DEBUG] password is set and wrong password!" << std::endl;
			return ;
		}
		auto it = _users.find(client->getNick());
		if (it == _users.end())
		{
			std::cout << "[DEBUG] Client added ! " << client->getNick() << " to " << this->getName() << std::endl;
			_users[client->getNick()] = client;
			if (_invOnly) 
				removeInvUsers(client);
			client->sendMsg("irc_custom", "available commands: JOIN, MODE, KICK, PART, QUIT, PRIVMSG/NOTICE"); // not correct format and more needs to be sent maybe outside the method
		}
		else
			std::cout << "[DEBUG] Client is already part of the Channel!" << std::endl;
		std::cout << "[DEBUG] " << getJoinedUsers() << std::endl;
	}

	// sends information about available commands
	// if successfull
	// user receives join message
	// user receives channel topic
	// user receives list of users who are on the channel including the user joining
}

void	Channel::printUsers(void) // depricated
{
	int i = 1;
	for (auto it = _users.begin(); it != _users.end(); it++)
		std::cout << GREEN "[DEBUG] [" << i++ << "] " << it->first << WHITE << std::endl;
}

//<<<<<<<<<<<<<<<INVITE>>>>>>>>>>>>//
// the return value can be changed to adjust to the parsing logic
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
void	Channel::removeUser(Client* client)
{
	if (client)
	{
		if (_users.find(client->getNick()) != _users.end())
			_users.erase(client->getNick());
		if (_operators.find(client->getNick()) != _operators.end())
			_operators.erase(client->getNick());
	}
	// check outside if Channel has 0 members now
	// if so delete the channel object
}

void	Channel::addOperator(Client* client)
{
	if (client)
	{
		if (auto it = _operators.find(client->getNick()) == _operators.end())
		{
			_operators[client->getNick()] = client;
		}
	}
}

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

void	Channel::addInvUsers(Client* client)
{
	if (client)
	{
		if (auto it = _invitedUsers.find(client->getNick()) == _invitedUsers.end())
		{
			_invitedUsers[client->getNick()] = client;
		}
	}
}

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
bool	Channel::isOperator(const Client* client) const
{
	if (client)
	{
		if (_operators.find(client->getNick()) != _operators.end())
			return (true);
	}
	return (false);
}

bool	Channel::isUser(const Client* client) const
{
	if (client)
	{
		if (_users.find(client->getNick()) != _users.end())
			return (true);
	}
	return (false);
}

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
