#include "Channel.hpp"

//================ Orthodox Form ================//

Channel::Channel(std::string name) : _name(name), _invOnly(false), _topicOp(false), _pswrdTgle(false), _usrLmtTgl(false)
{
}

Channel::~Channel()
{
}

// ======= verfication =======//
bool	Channel::verifyChannelName(std::string name)
{
	if (name.size() >= 50)
		return (false);
	if (name[0] == '#' || name[0] == '&' || name[0] == '+' || name[0] == '!')
	{
		if (name.find(' ') != std::string::npos ||
			name.find(',') != std::string::npos ||
			name.find(7) != std::string::npos)
			return (false);
	}
	return (false);
}

//================ Messaging ================//

/*------------------------------------------------------*/
/* Sends message to all users in channel except sender	*/
/*------------------------------------------------------*/
void	Channel::broadcast(const std::string& msg, Client* sender)
{
	if (sender)
	{
		for (auto it = _users.begin(); it != _users.end(); ++it)
		{
			if (it->second != sender)
				it->second->sendMsg("irc_custom", msg); // need to change for access to server name
		}
	}
}

//================ Adding & Removing clients ================//

void	Channel::addUser(Client* client)
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
		if (auto it = _users.find(client->getNick()) == _users.end())
		{
			std::cout << "[DEBUG] Client added to Channel!" << std::endl;
			_users[client->getNick()] = client;
			client->sendMsg("irc_custom", "available commands: JOIN, MODE, KICK, PART, QUIT, PRIVMSG/NOTICE"); // use  irssi client as a reference
			removeInvUsers(client);
		}
		else
			std::cout << "[DEBUG] Client is already part of the Channel!" << std::endl;
		
	}
	// if the client exists
	// add to map
	// sends information about available commands
	// if successfull
	// user receives join message
	// user receives channel topic
	// user receives list of users who are on the channel including the user joining
}

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
bool	Channel::isOperator(const Client* client)
{
	if (client)
	{
		if (_operators.find(client->getNick()) != _operators.end())
			return (true);
	}
	return (false);
}

bool	Channel::isUser(const Client* client)
{
	if (client)
	{
		if (_users.find(client->getNick()) != _users.end())
			return (true);
	}
	return (false);
}

bool	Channel::isInvited(const Client* client)
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
