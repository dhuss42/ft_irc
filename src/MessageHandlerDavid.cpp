
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
	std::string	reason;

	std::cout << MAGENTA << "HANDLE QUIT2" WHITE << std::endl;


	if (_message.params.size() == 2)
		reason = _message.params[1];

	std::cout << "[DEBUG] message param[0]" << _message.params[0]  << std::endl;
	std::cout << "[DEBUG] message param[1]" << _message.params[1]  << std::endl;



	std::string quitMsg = ":" + _client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname() + " QUIT :" + reason + "\r\n";

	std::cout << "[DEBUG] " << quitMsg << std::endl;

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
		// for every channel get the list of joined users
		// send to the user if not the quitter
	}

	_client.sendRaw(quitMsg); // not sure if needed

	for (auto iterate = recipients.begin(); iterate != recipients.end(); ++iterate)
	{
		std::cout << "[DEBUG] sending to " << (*iterate)->getNick() << " | " << quitMsg << std::endl;
		(*iterate)->sendRaw(quitMsg);
	}
	_client.setDisconnect(true);
}

// PART
void MessageHandler::handlePart(void)
{
	std::cout << MAGENTA << "HANDLE PART" WHITE << std::endl;

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

		std::cout << "[Debug]: " << channelName << std::endl;
		if (!_server.isChannel(channelName))
		{
			std::cout << "[Debug]: Channel does not exist" << std::endl;
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL, channelName + ":No such Channel"); // problem with formating look in ir protocol
			// 10:45 -!- #dhusssibussi: No such channel || is response on IRCnet but on server page not channel
		}
		else if (!_client.isJoinedChannel(channelName))
		{
			std::cout << "[Debug]: Not on Channel:" << std::endl;
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOTONCHANNEL, channelName + ":You're not on that channel");
			// 10:43 -!- #test You're not on that channel || is response on IRCnet but on server page not channel
		}
		else
		{
			_client.sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "PART " + channelName + " " + reason);
			std::cout << "[Debug]: Removing from Channel " << channelName << std::endl;
			_client.removeFromJoinedChannels(channelName);
			Channel* channel = _server.getChannel(channelName);
			if (channel)
			{
				std::cout << "[Debug]: channel exists" << std::endl;
				channel->broadcastUpdated(reason, &_client, "PART " + channelName);
				channel->removeUser(&_client);
				if (channel->getNbrUsers() == 0)
					_server.removeChannel(channel);
			}
			else
				std::cout << "[Debug]: channel does not exists" << std::endl;
			// currently I get the status log but the ui is not changing back
		}
		++it;
	}
}
