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

/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
void MessageHandler::handleJoin(void)
{
	std::cout << "[DEBUG] JOIN: " << std::endl;

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
	if (_message.params[1] == "")	//for capability negotiation phase
	{
		_client.sendMsg(_server.getName(), _message.params[0] + _message.params[1]);
		return ;
	}
	Channel* channel;
	if (_server.isChannel(_message.params[1]))
		channel = _server.getChannel(_message.params[1]);
	else
	{
		channel = _server.createChannel(_message.params[1], &_client);
		if (!channel)
		{
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL,
							_message.params[1]);
			return;
		}
	}
	if (!channel->addUser(&_client, _message.params[2]))
		return ;

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
	std::cout << "[DEBUG] PASS: " << std::endl;
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
	if (_client.getRegistered())	//create flag if client is already registered
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_ALREADYREGISTRED,
						"Already registered");
		_client.setDisconnect(true);
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
	std::cout << "[DEBUG] NICK: " << std::endl;
	//error if nickname in use, otherwise accept
	if (_message.params.size() < 2 || _message.params[1].empty())
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_ERRONEUSNICKNAME,
						"No nickname given");
		if (!_client.getNickSet())
			_client.setDisconnect(true); //only at first call
		return;
	}
	if (!verifyNickName(_message.params[1]))
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_ERRONEUSNICKNAME,
						_message.params[1] + " Erroneous Nickname");
		if (!_client.getNickSet())
			_client.setDisconnect(true); //only at first call
		return;
	}
	if (_server.isClient(_message.params[1]))
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NICKNAMEINUSE,
						"Nickname is already in use, choose another one");
		if (!_client.getNickSet())
			_client.setDisconnect(true); //only at first call
		return;
	}

	std::cout << "[DEBUG] NICK sets nickname: " << _message.params[1] << std::endl;
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
/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
void MessageHandler::handleMode()
{
	std::cout << "[DEBUG] MODE: " << std::endl;
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
	if (!channel->isOperator(&_client))
	{
		sendNotChannelOpErrorMessage(channel);
		return;
	}
	if (!validateModeParameters())
		return;
	processModes(channel);
	std::string returnMsg = _modeRet1 + _modeRet2;
	if (returnMsg.size() > 0)
		sendChangedModes(returnMsg, channel);
}

/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
void MessageHandler::handleWhois()
{
	std::cout << "[DEBUG] WHOIS: " << std::endl;
	//todo
}

/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
void MessageHandler::handlePing()
{
	std::cout << "[DEBUG] PING: " << std::endl;
	if (_message.params[1].empty())
		return ;	//Send error message
	_client.sendMsg("irc_custom", "PONG " + _message.params[1]);
}

/*------------------------------------------------------------------------------

------------------------------------------------------------------------------*/
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

