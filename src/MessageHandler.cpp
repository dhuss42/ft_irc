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
instead of _message.params[].empty() -> check for params size < x
*/

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
	std::cout << "[DEBUG] CAP: " << std::endl;

	if (_message.params[1] == "LS")
	{
		std::cout << "[DEBUG] LS: " << std::endl;
		_client.sendMsg(_server.getName(), "CAP * LS :multi-prefix sasl message-tags");
	}
	else if (_message.params[1] == "REQ")
	{
		std::cout << "[DEBUG] REQ: " << std::endl;
		std::string reply = "CAP * ACK ";
		bool hasMultiPrefix = false;	//do i need this??
		for (size_t i = 2; i < _message.params.size(); i++)
		{
			if (_message.params[i] == "multi-prefix")
				hasMultiPrefix = true;
			reply += _message.params[i] + " ";
		}
		_client.sendMsg(_server.getName(), reply);
	}
	else if (_message.params[1] == "END")
	{
		std::cout << "[DEBUG] Capability negotiation completed" << std::endl;
		return ;
	}
	else
		_client.sendError(_server.getName(), IrcErrorCode::ERR_INVALIDCAPCMD, _message.params[1] + ":Invalid or missing CAP subcommand" );
}

void MessageHandler::handleJoin(void)
{
	std::cout << "[DEBUG] JOIN: " << std::endl;

	if (_message.params[1].empty())	//for capability negotiation phase	-> change maybe to if _message.params[1] == ""
			return ;

	// //leaving channels (problem: irssi still opens a chatwindow called #0)
	// if (_message.params[1][1] == '0')	//irssi does not handle this
	// {
	// 	std::cout << "[DEBUG]: entered if condition 0" << std::endl;
	// 	std::unordered_map <std::string, Channel* > chanList = _server.getChannelUnoMap();
	// 	for (auto it = chanList.begin(); it != chanList.end(); it++)
	// 	{
	// 		it->second->removeUser(&_client);	//not working
	// 		std::string joinMsg = _client.getNick() + " [~" +
	// 					_client.getUsername() + "@" + _client.getHostname() +
	// 					"] has left " + _message.params[1] + "\n";
	// 	}
	// 	return;
	// }

	if (_message.params.size() < 2)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS,
						"Not enough parameters");
		return;
	}

	Channel* channel = _server.createChannel(_message.params[1], &_client);
	if (!channel)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL,
						_message.params[1]);
		return;
	}

	if (!channel->addUser(&_client, _message.params[2]))
		return ;
	// all checks if client may join (password, userlimit, inviteOnly) is done in addUSer
	// but need to add:
		// _client.sendError(_server.getName(), IrcErrorCode::ERR_BADCHANNELKEY, "Bad channel key");
		// _client.sendError(_server.getName(), IrcErrorCode::ERR_CHANNELISFULL, "Channel is full");
		// _client.sendError(_server.getName(), IrcErrorCode::ERR_INVITEONLYCHAN, "Invite-only channel");

	// Send message to channel that <nick> has joined
	std::string joinMsg = _client.getNick() + " [~" +
						_client.getUsername() + "@" + _client.getHostname() +
						"] has joined " + _message.params[1] + "\n";
	channel->broadcast(joinMsg, &_client);
	std::string prefix = _client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname() + " PRIVMSG " + channel->getName() + " :";
	_client.sendMsg(prefix, joinMsg);

	std::string topic = channel->getTopic();
	if (!topic.empty())
	{
		std::string prefix = _client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname() + " PRIVMSG " + channel->getName() + " :";
		_client.sendResponse(prefix, IrcResponseCode::RPL_TOPIC,
							"Topic for " + _message.params[1] + " :" + topic);
	}

	// Send channel user list to client
	std::string users = channel->getJoinedUsers();
	std::cout << "[DEBUG]: get joined users:" << users.size() << " " << users << std::endl;
	if (!users.empty())
	{
		std::cout << "[DEBUG]: entered if condition !users.empty" << std::endl;
		//docu says to use response codes, but not sure if it works as it should here
		std::string prefix = _client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname() + " PRIVMSG " + channel->getName() + " :";
		_client.sendResponse(prefix, IrcResponseCode::RPL_NAMREPLY,
							"Users " + _message.params[1] + ": " + users);
		_client.sendResponse(prefix, IrcResponseCode::RPL_ENDOFNAMES,
							_message.params[1] + " :End of /NAMES list.");
	}
	//maybe send also creation time
}

void MessageHandler::handlePass(void)
{
	std::cout << "[DEBUG] PASS: " << std::endl;
	if (_message.params[1].empty() || _message.params[1] != _server.getPassword())
	{
		_client.sendError("irc_custom", IrcErrorCode::ERR_PASSWDMISMATCH, "wrong password! Refused!");
		return ;
	}
	_client.setRegistered(true);
}

void MessageHandler::handleNick(void)
{
	std::cout << "[DEBUG] NICK: " << std::endl;
	//error if nickname in use, otherwise accept
	if (_message.params[1].empty())
		return ;	//send error message
	_client.setNick(_message.params[1]);
	_client.setNickSet(true);
}

/*
	- USER <username> <hostname> <servername> :<realname>
		-> server: sends welcome message when all flags are set
	ERR_NEEDMOREPARAMS
	ERR_ALREADYREGISTRED: If the client tries to send another USER message after registration
		-> "You may not reregister"
*/
void MessageHandler::handleUser()
{
	std::cout << "[DEBUG] USER: " << std::endl;
	if (_message.params[1].empty())
		return ;	//send Errormessage

	if (_message.params.size() == 5)
	{
		_client.setUsername(_message.params[1]);
		_client.setHostname(_message.params[2]);
		_client.setRealname(_message.params[4]);

		std::cout << "[DEBUG] username: " << _client.getUsername() << std::endl;
		std::cout << "[DEBUG] hostname: " << _client.getHostname() << std::endl; // not sure if better to have servername here or IP Address, this seems slower than doing it constructor
		std::cout << "[DEBUG] realname: " << _client.getRealname() << std::endl;

		_client.setUsernameSet(true);
	}
}

void MessageHandler::handleMode()
{
	std::cout << "[DEBUG] MODE: " << std::endl;

	if (_message.params.size() < 2)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS,
						"Not enough parameters");
		return;
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
		std::string prefix = _client.getNick() + "!" + _client.getUsername()
				+ "@" + _client.getHostname() + " PRIVMSG " + channel->getName() + " :";
		_client.sendResponse(prefix, IrcResponseCode::RPL_CHANNELMODEIS, "mode/" + channel->getName() + " [" + channel->getActiveChannelModes() + "]");
		return ;
	}
	if (!channel->isOperator(&_client))
	{
		std::string prefix = _client.getNick() + "!" + _client.getUsername()
				+ "@" + _client.getHostname() + " PRIVMSG " + channel->getName() + " :";
		_client.sendError(prefix, IrcErrorCode::ERR_CHANOPRIVSNEEDED,
						channel->getName() + " You're not a channel operator");
		return;
	}

	if (!validateModeParameters())
		return;

	processModes(channel);

	std::string returnMsg = _modeRet1 + _modeRet2;
	channel->broadcast(returnMsg, &_client);
	std::string prefix = _client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname() + " PRIVMSG " + channel->getName() + " :";
	_client.sendMsg(prefix, returnMsg);
}

void MessageHandler::handleWhois()
{
	std::cout << "[DEBUG] WHOIS: " << std::endl;
	//todo
}

void MessageHandler::handlePing()
{
	std::cout << "[DEBUG] PING: " << std::endl;
	if (_message.params[1].empty())
		return ;	//Send error message
	_client.sendMsg("irc_custom", "PONG " + _message.params[1]);
}

void MessageHandler::handlePrivmsg()
{
	std::cout << "[DEBUG] PRIVMSG: " << std::endl;

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
			channel->broadcast(message, &_client);
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

