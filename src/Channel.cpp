#include "Channel.hpp"

//================ Orthodox Form ================//

Channel::Channel(std::string name) : _name(name), _invOnly(false), _topicOp(false), _pswrdTgle(false), _usrLmtTgl(false)
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
		// check for userlimit
		// check if user is in invite list if invOnly is set
		_users[client->getNick()] = client;
		client->sendMsg("irc_custom", "available commands: JOIN, MODE, KICK, PART, QUIT, PRIVMSG/NOTICE"); // use  irssi client as a reference
		
	}
	// if the client exists
	// add to map
	// sends information about available commands
	// if successfull
	// user receives join message
	// user receives channel topic
	// user receives list of users who are on the channel including the user joining

	// 
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
		// check if client is already in list
		_operators[client->getNick()] = client;
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

// add to inv list
// remove from inv list

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
