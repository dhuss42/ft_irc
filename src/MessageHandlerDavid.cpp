
#include "MessageHandler.hpp"

/*----------------------------------------------------------------------*/
/* Quit																	*/
/*	- check if client send reason										*/
/*	- loop over channels that client is part of							*/
/* 	- for every channel extract recipients excluding quitter			*/
/*	- send quit message to recipients									*/
/*	- set Disconnect bool for client									*/
/*----------------------------------------------------------------------*/
void	MessageHandler::handleQuit(void)
{
	std::string	reason;
	if (_message.params.size() == 2)
		reason = _message.params[1];

	std::unordered_set<Client*> recipients;
	std::unordered_map<std::string, Channel*> joinedChannels = _client.getJoinedChannels();
	for (auto it = joinedChannels.begin(); it != joinedChannels.end(); ++it)
	{
		std::map<std::string, Client*> users = it->second->getUsers();
		for (auto iter = users.begin(); iter != users.end(); ++iter)
		{
			if (iter->second != &_client)
				recipients.insert(iter->second);
		}
	}
	for (auto iterate = recipients.begin(); iterate != recipients.end(); ++iterate)
		(*iterate)->sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "QUIT :" + reason);
	_client.setDisconnect(true);
}

/*----------------------------------------------------------------------*/
/* Part																	*/
/*	- if there are enough parameters									*/
/*	- check if client sent reason										*/
/*	- split channel parameters separated by ","							*/
/* 	- loop over channels to part from									*/
/*	- send part message to clients in channel							*/
/*	- update all Containers												*/
/*	- if channel is now empty delete channel object						*/
/*----------------------------------------------------------------------*/
void MessageHandler::handlePart(void)
{
	if (_message.params.size() < 2)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS, "Not enough parameters");
		return ;
	}
	std::string reason;
	if (_message.params.size() > 2)
		reason = _message.params[2];

	std::regex del(",");
	std::sregex_token_iterator it(_message.params[1].begin(), _message.params[1].end(), del, -1);
	std::sregex_token_iterator end;

	while (it != end)
	{
		std::string channelName = *it;
		Channel* channel = _server.getChannel(channelName);
		if (!channel)
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL, channelName);
		else if (!_client.isJoinedChannel(channelName))
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOTONCHANNEL, channelName + " :You're not on that channel"); // this is not sending correct format
		else
		{
			_client.sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "PART " + channelName + " " + reason);
			_client.removeFromJoinedChannels(channelName);
			channel->broadcastUpdated(reason, &_client, "PART " + channelName);
			channel->removeUser(&_client);
			if (channel->getNbrUsers() == 0)
				_server.removeChannel(channel);
		}
		++it;
	}
}

/*----------------------------------------------------------------------*/
/* Kick																	*/
/*	- if there are enough parameters									*/
/*	- check if client send reason										*/
/*	- check if channel exists											*/
/*	- check if kicker is on channel										*/
/*	- check if kicker is operator										*/
/*	- check if to be kicked is on channel								*/
/* 	- loop over channels to part from									*/
/*	- send kick message to clients in channel							*/
/*	- update all Containers												*/
/*	- if channel is now empty delete channel object						*/
/*----------------------------------------------------------------------*/
void	MessageHandler::handleKick(void)
{
	if (_message.params.size() < 3)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS, "Not enough parameters");
		return ;
	}
	Client* kicked = _server.getClient(_message.params[2]);
	Channel* channel = _server.getChannel(_message.params[1]);
	if (!channel)
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL, _message.params[1]);
	else if (!_client.isJoinedChannel(_message.params[1]))
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NOTONCHANNEL, _message.params[1] + " :You're not on that channel");
	else if (!channel->isOperator(&_client))
		_client.sendError(_server.getName(), IrcErrorCode::ERR_CHANOPRIVSNEEDED, _message.params[1] + " :You're not channel operator");
	else if (!kicked || !channel->isUser(kicked))
		_client.sendError(_server.getName(), IrcErrorCode::ERR_USERNOTINCHANNEL, _message.params[2] + " " + _message.params[1] + " :They aren't on that channel");
	else
	{
		std::string reason;
		if (_message.params.size() == 4)
			reason = _message.params[3];
		channel->broadcastUpdated(reason, &_client, "KICK " + _message.params[1] + " " + kicked->getNick());
		_client.sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "KICK " + _message.params[1] + " " + kicked->getNick() + " :" + reason);
		// kicked->sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "KICK " + _message.params[1] + " " + kicked->getNick() + " :" + reason);
		channel->removeUser(kicked);
		kicked->removeFromJoinedChannels(_message.params[1]);
		if (channel->getNbrUsers() == 0)
			_server.removeChannel(channel);
	}
}


void	MessageHandler::handleInvite(void)
{
	for (auto it = _message.params.begin(); it != _message.params.end(); ++it)
		std::cout << YELLOW << "[DEBUG] params: " << *it << WHITE << std::endl;

	if (_message.params.size() < 3)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS, "Not enough parameters");
		return ;
	}
	// check params

	// check if channel exists
	// check if inviter is on channel
	// check if invite-only and inviter is operator
	// if user is on channel already
		// ERR_USERONCHANNEL 
	
	// success
	// RPL_INVITING -> command issuer
	// Invite message -> target
}
