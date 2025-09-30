
#include "MessageHandler.hpp"

// Parse Handler if Check
// into MessageHandler

	// void handleQuit(void);
	// void handleQuit2(void);
	// void handlePart(void);

// when pushing delete method declaration & parse Handler check
	// else if (message.command == "QUIT")
	// 	handler.handleQuit();
	// else if (message.command == "PART")
	// 	handler.handlePart();

void MessageHandler::handleQuit(void)
{
	std::cout << "[DEBUG] QUIT: " << std::endl;

	if (_message.params.size() > 2)
		return ;
	std::string msg = _client.getNick() + " [~" + _client.getUsername() + "@" + _client.getHostname() + "] has quit ";
	if (_message.params.size() == 2)
		msg += "[\"" + _message.params[1] + "\"]";

	std::unordered_map<std::string, Channel*> joinedChannels = _client.getJoinedChannels();
	for (auto it = joinedChannels.begin(); it != joinedChannels.end(); ++it)
		it->second->broadcast(msg, &_client);
	_client.setDisconnect(true);
}

// 12:34 -!- paikka [~david@ip-005-146-193-175.um05.pools.vodafone-ip.de] has quit
//       ["ciao"]

// I channel is never added to _joined channels
void	MessageHandler::handleQuit2(void)
{
	std::cout << MAGENTA << "HANDLE QUIT2" WHITE << std::endl;
	std::string	reason;
	if (_message.params.size() == 2)
		reason = _message.params[1];

	std::unordered_set<Client*> recipients;
	std::unordered_map<std::string, Channel*> joinedChannels = _client.getJoinedChannels();
	// loop over al joined channels and send message to all clients in those channels except when it's the client himself
	for (auto it = joinedChannels.begin(); it != joinedChannels.end(); ++it)
	{
		std::cout << "[DEBUG] user is part of Channel: " << it->second->getName() << std::endl;
		std::map<std::string, Client*> users = it->second->getUsers();
		for (auto iter = users.begin(); iter != users.end(); ++iter)
		{
			if (iter->second != &_client)
			{
				std::cout << "[DEBUG] For Channel: " << it->second->getName() << " Found user: " << iter->second->getNick() << std::endl;
				recipients.insert(iter->second);
			}
		}

	}
	for (auto iterate = recipients.begin(); iterate != recipients.end(); ++iterate)
	{
		(*iterate)->sendMsg((*iterate)->getNick() + "!" + (*iterate)->getUsername() + "@" + (*iterate)->getHostname() + " ", "QUIT :" + reason);
		// not sure if this is entirely correct but when I send it with the quitting clients info irssi does not respond have to double check
	}
	_client.setDisconnect(true);
}

// PART
// could split further into smaller methods
// could refactor thelogic with getting the channel at the beginning and saving the extra validation check for nullptr
// in join names are in CYAN but in PART username@hostname are not in CYAN ask leni
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
	// two separte calls when two users separate by comma
	for (auto it = _message.params.begin(); it != _message.params.end(); ++it)
		std::cout << YELLOW << "[DEBUG] params: " << *it << WHITE << std::endl;


	if (_message.params.size() < 3)
	{
		std::cout << RED << "[DEBUG] not enough params" WHITE << std::endl;
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS, "Not enough parameters");
		return ;
	}

	Client* kicked = _server.getClient(_message.params[2]);
	Channel* channel = _server.getChannel(_message.params[1]);
	if (!channel)
	{
		std::cout << RED "[DEBUG] no such channel" << WHITE << std::endl;
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL, _message.params[1]); // -> works fine
	}
	else if (!_client.isJoinedChannel(_message.params[1]))
	{
		std::cout << RED "[DEBUG] kicker not on channel" << WHITE << std::endl;
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NOTONCHANNEL, _message.params[1] + ":You're not on that channel"); // this is not sending correct format
		// 15:24 -!- #test:You're not on that channel 
		// nothing is printed in bold
	}
	else if (!channel->isOperator(&_client))
	{
		std::cout << RED "[DEBUG] not operator" << WHITE << std::endl;
		_client.sendError(_server.getName(), IrcErrorCode::ERR_CHANOPRIVSNEEDED, _message.params[1]); // check what needs to be send -> only prints test right now
	}
	else if (!kicked || !channel->isUser(kicked))
	{
		std::cout << RED "[DEBUG] kicked not on channel" << WHITE << std::endl;
		_client.sendError(_server.getName(), IrcErrorCode::ERR_USERNOTINCHANNEL, _message.params[1]); // check what needs to be send -> only prints test right now
	}
	else
	{
		std::string reason;
		if (_message.params.size() == 4)
			reason = _message.params[3];

		// std::string kickMsg = ":" + _client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname() + " KICK " + _message.params[1] + " " + kicked->getNick();

		channel->broadcastUpdated(reason, &_client, "KICK " + _message.params[1] + " " + kicked->getNick());
		// :maustel!david@david KICK #test david :bye bye
		_client.sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "KICK " + _message.params[1] + " " + kicked->getNick() + " :" + reason);
		std::cout << RED "[DEBUG] removing :" << kicked->getNick() << WHITE << std::endl;
		channel->kickUser(&_client, _message.params[2]);
		kicked->removeFromJoinedChannels(_message.params[1]);
	}
	// kick user from Channel and check if kicker has priviledges to do so
	// 	void	Channel::kickUser(Client* kicker, std::string kicked)
}