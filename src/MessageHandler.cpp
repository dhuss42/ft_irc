/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MessageHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 16:51:48 by maustel           #+#    #+#             */
/*   Updated: 2025/09/18 16:51:48 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MessageHandler.hpp"

/*
/ Constructor
*/
MessageHandler::MessageHandler(Client& client, Message& message, Server& server):
	_client(client),
	_message(message),
	_server(server)
{}

/*
/ Destructor
*/
MessageHandler::~MessageHandler()
{
	_modeRet1.clear();
	_modeRet2.clear();
}

/*
	Builds a CAP ACK response message by combining the requested capabilities

 	constructs the server's response to a client's capability request by:
 	1. Starting with the base CAP ACK command
 	2. Iterating through all requested capabilities (starting from index 2)
 	3. Concatenating each capability to build the complete response

 	The response maintains the exact order and values of requested capabilities,
 	following the IRC specification for capability negotiation.

	CAP LS (302): Announces supported capabilities to the client
	CAP ACK: Processes accepted capabilities from the server
	CAP NAK: Handles rejected capabilities gracefully
	CAP END: Completes negotiation and proceeds with registration

	- multi-prefix: Enables enhanced user prefix handling, allowing servers to send all user
	prefixes in order of rank (e.g., @+user instead of just @user)
	- sasl: Supports Secure Authentication and Login (SASL) for secure user authentication
	- message-tags: Enables message metadata support for features like message IDs and batch tags
*/
void MessageHandler::handleCap(void)
{
	if (_message.params[1] == "LS")
		_client.sendMsg(_server.getName(), "CAP * LS :multi-prefix sasl message-tags");
	else if (_message.params[1] == "REQ")
	{
		std::string reply = "CAP * ACK ";
		for (size_t i = 2; i < _message.params.size(); i++)
		{
			reply += _message.params[i] + " ";
		}
		_client.sendMsg(_server.getName(), reply);
	}
	else if (_message.params[1] == "END")
		return ;
	else
		_client.sendError(_server.getName(), IrcErrorCode::ERR_INVALIDCAPCMD, _message.params[1] + ":Invalid or missing CAP subcommand" );
}

/*------------------------------------------------------------------------------
JOIN <channel>[,<channel>] [<key>[,<key>]]
	- Joins one or more channels in a single command
	- Each channel can optionally require a key
	- Returns error if insufficient parameters
	- Creates new channels if they don't exist
	- Adds user to all valid channels with proper keys
	- Continues processing remaining channels even if some fail
	example: JOIN #channel1,#channel2,#channel3 key1,key2
------------------------------------------------------------------------------*/
void MessageHandler::handleJoin(void)
{
	if (_message.params.size() < 2)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS,
						"Not enough parameters");
		return;
	}
	if (_message.params[1] == "")	//for capability negotiation phase
	{
		_client.sendMsg(_server.getName(), _message.params[0] + _message.params[1]);
		return ;
	}

	std::vector<std::string> channels;
	splitString(_message.params[1], ',', channels);

	std::vector<std::string> keys;
	if (_message.params.size() > 2 && _message.params[2] != "")
		splitString(_message.params[2], ',', keys);

	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string& channelName = channels[i];
		if (channelName.empty())
			continue;
		Channel* channel;
		if (_server.isChannel(channelName))
			channel = _server.getChannel(channelName);
		else
		{
			channel = _server.createChannel(channelName, &_client);
			if (!channel)
			{
				_client.sendError(_server.getName(),
						IrcErrorCode::ERR_NOSUCHCHANNEL, channelName + " :Invalid channel name");
				continue;
			}
		}
		std::string key;
		if (!keys.empty() && i < keys.size())
			key = keys[i];
		if (!channel->addUser(&_client, key))
			continue;
		sendJoinTopicUserlistMsg(channel);
	}
}

/*------------------------------------------------------------------------------
PASS command
Checks if
	- right amount of parameters
	- client is already registered
	- right password
otherwise disconnect
------------------------------------------------------------------------------*/
void MessageHandler::handlePass(void)
{
	if (_message.params.size() < 2)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS,
						"Not enough parameters");
		_client.setDisconnect(true);
		return;
	}
	if (_message.params.size() > 2)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_PASSWDMISMATCH,
						"Too many parameters");
		_client.setDisconnect(true);
		return;
	}
	if (_client.getRegistered())
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_ALREADYREGISTERED,
						"Already registered");
		return ;
	}
	if (_message.params[1].empty() || _message.params[1] != _server.getPassword())
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_PASSWDMISMATCH,
			"Wrong password! Refused!");
		_client.setDisconnect(true);
		return ;
	}
	_client.setRegistered(true);
}

/*------------------------------------------------------------------------------
NICK command
Used to give the client a nickname or change the previous one.
Sends error when
	- nickname in use
	- verify nickname fails
	- no nickname parameter given (/nick "")
	- /nick without parameter is handled by irssi
------------------------------------------------------------------------------*/
void MessageHandler::handleNick(void)
{
	if (!_client.getRegistered())
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_PASSWDMISMATCH,
			"No password! Refused!");
		_client.setDisconnect(true);
		return ;
	}
	if (_message.params.size() < 2 || _message.params[1].empty())
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_ERRONEUSNICKNAME,
						"No nickname given");
		if (!_client.getNickSet())
			_client.setDisconnect(true);
		return;
	}
	std::string newNick = _message.params[1];
	if (!verifyNickName(newNick))
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_ERRONEUSNICKNAME,
						newNick + " Erroneous Nickname");
		if (!_client.getNickSet())
			_client.setDisconnect(true);
		return;
	}
	if (_client.getNickSet() && _client.getNick() == newNick)
		return ;
	if (_server.isClient(newNick))
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_ERRONEUSNICKNAME,
						newNick + " Nickname is already in use, choose another one");
		if (!_client.getNickSet())
			_client.setDisconnect(true);
		return;
	}
	if (_client.getNickSet())
	{
		std::string oldNick = _client.getNick();
		std::string prefix = oldNick + "!" + _client.getUsername() + "@" + _client.getHostname() + " NICK " + newNick + " :";
		_client.updateNick(oldNick, newNick);
		_server.updateNickOnServer(oldNick, newNick);
		broadcastNicknameInChannels(newNick);
		_client.sendMsg(prefix, "");
	}
	_client.setNick(newNick);
	_client.setNickSet(true);
}

/*------------------------------------------------------------------------------
USER <username> <hostname> <servername> :<realname>
	Validates that at least a username is provided
	Checks if username is already registered
	Sets username from first parameter
	Sets hostname from second parameter if available, otherwise uses username
	Sets realname from fifth parameter if available, otherwise uses username
	Completes registration if all requirements are met
------------------------------------------------------------------------------*/
void MessageHandler::handleUser()
{
	if (!_client.getRegistered())
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_PASSWDMISMATCH,
			"No password! Refused!");
		_client.setDisconnect(true);
		return ;
	}
	if (!_client.getNickSet())
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NONICKNAMEGIVEN,
			"Do NICK before USER");
		return ;
	}
	if (_message.params.size() < 2 || _message.params[1].empty())
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS,
						"Not enough parameters");
		return;
	}
	if (_client.getUsernameSet())
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_ALREADYREGISTERED,
						"Already Registered");
		return;
	}
	_client.setUsername(_message.params[1]);

	if (_message.params.size() > 2)
		_client.setHostname(_message.params[2]);
	else
		_client.setHostname(_message.params[1]);

	if (_message.params.size() > 4)
		_client.setRealname(_message.params[4]);
	else
		_client.setHostname(_message.params[1]);

	_client.setUsernameSet(true);
	if (_client.getRegistered() && _client.getNickSet())
	{
		_client.sendResponse(_server.getName(), IrcResponseCode::RPL_WELCOME, "Welcome to our super nice IRC server!");
		_server.addToClientList(&_client);
	}
}

/*------------------------------------------------------------------------------
Handles mode changes in channel and also /mode nick +i at registration phase
	- can handle a row of mode changes
	- /mode #chan without parameters -> tells current active modes
	- example: /mode #chan +ilk 5 key
		-> activates inviteOnlyChannel
		-> sets Userlimit to 5
		-> sets channel key to "key"
	- sends response with changed modes
	- handles i, t, k, l, o, b
	- /mode #chan b is asked automatically by the client when joined to a channel
		(ban list) -> we dont have ban list, so we send empty response
------------------------------------------------------------------------------*/
void MessageHandler::handleMode()
{
	if (_message.params.size() < 2)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS,
						"Not enough parameters");
		return;
	}
	if (_message.params.size() == 3 && _message.params[1] == _client.getNick() && _message.params[2] == "+i")
	{
		_client.sendMsg(_server.getName(), _message.params[0] + " " + _message.params[1] + " " + _message.params[2]);
		return ; // registration phase
	}
	std::string target = _message.params[1];
	if (!_server.isChannel(target))
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL,
						target);
		return;
	}
	Channel* channel = _server.getChannel(target);
	if (_message.params.size() == 2)
	{
		sendActiveChannelModes(channel);
		return ;
	}
	if (!validateModeParameters())
		return;
	processModes(channel);
	std::string returnMsg = _modeRet1 + _modeRet2;
	if (returnMsg.size() > 0)
		sendChangedModes(returnMsg, channel);
}

/*------------------------------------------------------------------------------
Handles WHO command to list channel members with their details.
 Cient automatically asks for it when joined to a channel
 	- Format: WHO <channel>
 	- Returns RPL_WHOREPLY for each user with format:
 	- :server channel username hostname server nick mode :realname
	- the 0 indicates that its the local server
 	- Ends with RPL_ENDOFWHO to mark completion.
------------------------------------------------------------------------------*/
void MessageHandler::handleWho()
{
	if (_message.params.size() < 2)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS,
						"Not enough parameters");
		return;
	}
	std::string mask = _message.params[1];
	Channel *channel;

	if (_server.isChannel(mask))
	{
		channel = _server.getChannel(mask);
		const std::string users = channel->getJoinedUsers();

		std::istringstream iss(users);
		std::string nick;

		while (iss >> nick)
		{
			std::string cleanNick = nick;
			if (cleanNick[0] == '@')
				cleanNick = cleanNick.substr(1);

			Client* client = _server.getClient(cleanNick);	//not working anymore ?!
			if (client != nullptr)
			{
				std::string reply = channel->getName() + " " +
					client->getUsername() + " " +
					client->getHostname() + " " +
					_server.getName() + " " +
					client->getNick() + " " +
					(channel->isOperator(client) ? "H@" : "H") + " " +
					":1 " +
					client->getRealname();

				_client.sendResponse(_server.getName(),
					IrcResponseCode::RPL_WHOREPLY, reply);
			}
		}
	}
	_client.sendResponse(_server.getName(),
				IrcResponseCode::RPL_ENDOFWHO,
				mask);
}

/*------------------------------------------------------------------------------
The PING command is sent by clients to check the other side of
the connection is still connected and/or to check for connection latency,
at the application layer.
Servers must reply to it with a PONG message with the same <token> value.
------------------------------------------------------------------------------*/
void MessageHandler::handlePing()
{
	if (_message.params.size() < 2 || _message.params[1].empty())
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS,
						"Not enough parameters");
		return;
	}

	_client.sendMsg(_server.getName(), "PONG " + _message.params[1]);
}

/*------------------------------------------------------------------------------
PRIVMSG <target> :<message>
This command handles private messaging in IRC, allowing clients to send messages
to either individual users or channels. The implementation follows the standard
IRC message format chi.cs.uchicago.edu, where:

<target>: The recipient (either a nickname or channel name)
<message>: The message content (must be preceded by a colon)
The function processes messages through a logical flow:

Validates message format
Determines message type (private or channel)
Routes message appropriately
Handles errors for invalid targets
------------------------------------------------------------------------------*/
void MessageHandler::handlePrivmsg()
{
	if (_message.params.size() < 3)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS,
						"Not enough parameters");
		return;
	}
	const std::string& target = _message.params[1];
	const std::string& message = _message.params[2];
	if (_server.isClient(target))
	{
		Client* recipient = _server.getClient(target);
		if (recipient)
		{
			std::string prefix = _client.getNick() + "!" + _client.getUsername() + "@"
						+ _client.getHostname() + " PRIVMSG " + recipient->getNick() + " :";
			recipient->sendMsg(prefix, message);
		}
		else
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NORECIPIENT,
						"No recipient given");
	}
	else if (_server.isChannel(target))
	{
		Channel* channel = _server.getChannel(target);
		if (channel && channel->getJoinedUsers().find(_client.getNick()) != std::string::npos)
		{
			channel->broadcast(message, &_client, "PRIVMSG " + channel->getName());
		}
		else
			_client.sendError(_server.getName(), IrcErrorCode::ERR_CANNOTSENDTOCHAN,
							target + " Cannot send to channel");
	}
	else
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHNICK, target);
	}
}

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
			channel->broadcast(reason, &_client, "PART " + channelName);
			channel->removeUser(&_client);
			if (channel->getNbrUsers() == 0)
				_server.removeChannel(channel);
		}
		++it;
	}
}

/*----------------------------------------------------------------------*/
/* Kick																	*/
/*	- check if there are enough parameters								*/
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
		channel->broadcast(reason, &_client, "KICK " + _message.params[1] + " " + kicked->getNick());
		_client.sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "KICK " + _message.params[1] + " " + kicked->getNick() + " :" + reason);
		channel->removeUser(kicked);
		kicked->removeFromJoinedChannels(_message.params[1]);
		if (channel->getNbrUsers() == 0)
			_server.removeChannel(channel);
	}
}

/*----------------------------------------------------------------------*/
/* Invite																*/
/*	- check if there are enough parameters								*/
/*	- check if channel exists											*/
/*	- check if inviter is on channel									*/
/*	- check if inviter is operator										*/
/*	- check if invited is on channel									*/
/*	- send invite message to invited 									*/
/*	- send response to inviter											*/
/*	- add invited to invite List										*/
/*----------------------------------------------------------------------*/
void	MessageHandler::handleInvite(void)
{
	if (_message.params.size() < 3)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS, "Not enough parameters");
		return ;
	}

	Client* invited = _server.getClient(_message.params[1]);
	Channel* channel = _server.getChannel(_message.params[2]);
	if (!channel)
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL, _message.params[2]);
	else if (!_client.isJoinedChannel(_message.params[2]))
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NOTONCHANNEL, _message.params[2] + " :You're not on that channel");
	else if (channel->getInvOnly() && !channel->isOperator(&_client))
		_client.sendError(_server.getName(), IrcErrorCode::ERR_CHANOPRIVSNEEDED, _message.params[2] + " :You're not channel operator");
	else if (!invited || channel->isUser(invited))
		_client.sendError(_server.getName(), IrcErrorCode::ERR_USERONCHANNEL, _message.params[1] + " " + _message.params[2] + " :is already on channel");
	else
	{
		_client.sendResponse(_server.getName(), IrcResponseCode::RPL_INVITING, invited->getNick() + " " + channel->getName());
		invited->sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "INVITE " + invited->getNick() + " " + _message.params[2]);
		channel->addInvUsers(invited);
	}
}

/*----------------------------------------------------------------------*/
/* Topic																*/
/*	- check if there are enough parameters								*/
/*	- check if second parameter is "-delete"							*/
/*		- assign channelName and topic accordingly						*/
/*	- check if channel exists											*/
/*	- check if changer is on channel									*/
/*	- check if changer is operator when only operator can change		*/
/*	- if only two args													*/
/*	- 	if topic is empty send topic not set							*/
/*	- 	else send topic													*/
/*	- if second parameter is "-delete" clear topic						*/
/*	- else change channel topic to topic								*/
/*	- send topic update message to everyone on channel					*/
/*----------------------------------------------------------------------*/
void	MessageHandler::handleTopic(void)
{
	if (_message.params.size() < 2)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS, "Not enough parameters");
		return ;
	}

	std::string	channelName = _message.params[1];
	std::string	topic = _message.params[2];
	if (_message.params[1] == "-delete")
	{
		channelName = _message.params[2];
		topic = _message.params[3];
	}
	else
	{
		Channel* channel = _server.getChannel(channelName);
		if (!channel)
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL, channelName);
		else if (!_client.isJoinedChannel(channelName))
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
		else if (channel->getTopicOp() && !channel->isOperator(&_client))
			_client.sendError(_server.getName(), IrcErrorCode::ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
		else
		{
			if (_message.params.size() < 3)
			{
				if (channel->getTopic().empty())
					_client.sendResponse(_server.getName(), IrcResponseCode::RPL_NOTOPIC, _client.getNick() + " " + channelName);
				else
					_client.sendResponse(_server.getName(), IrcResponseCode::RPL_TOPIC, _client.getNick() + " " + channelName);
			}
			else
			{
				if (_message.params[1] == "-delete")
					channel->setTopic("");
				else
					channel->setTopic(topic);
				channel->broadcast(topic, &_client, "TOPIC " + _message.params[1]);
				_client.sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "TOPIC " + _message.params[1] + " " + topic);
			}
		}
	}
}
