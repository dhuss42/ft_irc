
#include "MessageHandler.hpp"

// Parse Handler if Check
// into MessageHandler

// when pushing delete method declaration & parse Handler check


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


void	MessageHandler::handleQuit2(void)
{
	std::string	reason;


	if (_message.params.size() == 2)
		reason = _message.params[1];

	std::cout << "[DEBUG] message param[0]" << _message.params[0]  << std::endl;
	std::cout << "[DEBUG] message param[1]" << _message.params[1]  << std::endl;

	

	std::string quitMsg = ":" + _client.getNick() + "!" +
                          _client.getUsername() + "@" +
                          _client.getHostname() +
                          " QUIT :" + reason + "\r\n";

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
	// need to type # before channel name otherwise irssi interprets it as reason for leaving
	// if no channel is given the second parameter is the current channel
	// max three parameters 1. PART 2. List of Channels to part from 3. reason

	if (_message.params.size() < 2)
	{
		// _client.sendError(); // -> ERR_NEEDMOREPARAMS 461
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS, "Not enough parameters");
		std::cout << "[Debug]: not enough params" << std::endl;	
		return ;
	}
	
	for (auto it = _message.params.begin(); it != _message.params.end(); ++it)
	{
		std::cout << "[Debug]: channels Parting from: " << *it << std::endl;
	}

	std::regex del(",");

	std::sregex_token_iterator it(_message.params[1].begin(), _message.params[1].end(), del, -1);
	std::sregex_token_iterator end;

	while (it != end)
	{
		std::string channelName = *it;

		std::cout << "[Debug]: " << channelName << std::endl;
		if (!_server.isChannel(channelName))
		{
			std::cout << "[Debug]: Channel does not exist" << std::endl; // -> ERR_NOSUCHCHANNEL 403
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL, channelName + ":No such Channel"); // problem with formating look in ir protocol
		}
		else if (!_client.isJoinedChannel(channelName))
		{
			std::cout << "[Debug]: Not on Channel:" << std::endl; // -> ERR_NOTONCHANNEL 442
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOTONCHANNEL, channelName + ":You're not on that channel");
		}
		else
		{
			_client.sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "PART " + channelName);
			std::cout << "[Debug]: Removing from Channel " << channelName << std::endl;
			_client.removeFromJoinedChannels(channelName);
			Channel* channel = _client.getJoinedChannel(channelName);
			if (channel)
				channel->removeUser(&_client);
			// currently I get the status log but the ui is not changing back
		}
		++it;
		// also check if the channel is empty now
	}
	// check if at least two parameters
	// takes list of channels to Part
	// last argument is reason for parting

	// check if client is joined to channel
		// -> ERR_NOTONCHANNEL 442
	// check if there is such as channel
		// -> ERR_NOSUCHCHANNEL 403

	
	// remove from one Channel at a time
	// remove from Channel Container
	// remove from usersJoinedChannels
	// always append reason

}

// 12:33 -!- paikka [~david@ip-005-146-193-175.um05.pools.vodafone-ip.de] has left
//           #dhuss [bye]