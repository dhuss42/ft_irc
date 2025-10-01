
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

// PART
// could split further into smaller methods
// could refactor thelogic with getting the channel at the beginning and saving the extra validation check for nullptr
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
		if (!_server.isChannel(channelName))
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL, channelName);
		else if (!_client.isJoinedChannel(channelName))
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOTONCHANNEL, channelName + ":You're not on that channel"); // this is not sending correct format
			// 10:43 -!- #test You're not on that channel || is response on IRCnet but on server page not channel
		else
		{
			_client.sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "PART " + channelName + " " + reason);
			_client.removeFromJoinedChannels(channelName);
			Channel* channel = _server.getChannel(channelName);
			if (channel)
			{
				channel->broadcastUpdated(reason, &_client, "PART " + channelName);
				channel->removeUser(&_client);
				if (channel->getNbrUsers() == 0)
					_server.removeChannel(channel);
			}
		}
		++it;
	}
}

// KICK
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
		kicked->sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "KICK " + _message.params[1] + " " + kicked->getNick() + " :" + reason);
		channel->removeUser(kicked);
		kicked->removeFromJoinedChannels(_message.params[1]);
		if (channel->getNbrUsers() == 0)
			_server.removeChannel(channel);
	}
}
