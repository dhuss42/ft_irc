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
	std::string chan = _message.params[1];
	std::string key;
	if (_message.params.size() > 2)
		key = _message.params[2];
	Channel* channel;
	if (_server.isChannel(chan))
		channel = _server.getChannel(chan);
	else
	{
		channel = _server.createChannel(chan, &_client);
		if (!channel)
		{
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL,
							chan);
			return;
		}
	}
	if (!channel->addUser(&_client, key))
		return ;

	sendJoinTopicUserlistMsg(channel);
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
		_client.sendError(_server.getName(), IrcErrorCode::ERR_ALREADYREGISTERED,
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

	if (_message.params.size() < 2 || _message.params[1].empty())
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_ERRONEUSNICKNAME,
						"No nickname given");
		if (!_client.getNickSet())
			_client.setDisconnect(true); //only at first call
		return;
	}
	std::string newNick = _message.params[1];
	if (!verifyNickName(newNick))
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_ERRONEUSNICKNAME,
						newNick + " Erroneous Nickname");
		if (!_client.getNickSet())
			_client.setDisconnect(true); //only at first call
		return;
	}
	if (_client.getNickSet() && _client.getNick() == newNick)
		return ;
	if (_server.isClient(newNick))	//not working -> david
	{
		std::cout << "[DEBUG] nick is already in use! " << newNick << std::endl;
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NICKNAMEINUSE,
						"Nickname is already in use, choose another one");
		if (!_client.getNickSet())
		_client.setDisconnect(true); //only at first call
		return;
	}
	if (!_server.isClient(newNick))
		std::cout << "[DEBUG] nick is not yet in use! " << newNick << std::endl;

	if (_client.getNickSet())//send something to client so he knows the new nickname?
	{
		std::cout << "[DEBUG] NICK change nickname " << newNick << std::endl;
		std::string oldNick = _client.getNick();
		std::string prefix = oldNick + "!" + _client.getUsername() + "@" + _client.getHostname() + " NICK " + newNick + " :";
		// updateNicknameInChannels(client, oldNick, newNick, nickChangeMsg);
		// server.broadcastMessage(client, "", nickChangeMsg);	//??
		_client.sendMsg(prefix, "");
	}

	std::cout << "[DEBUG] NICK sets nickname: " << newNick << std::endl;

	_client.setNick(newNick);
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
USER <username> <hostname> <servername> :<realname>
------------------------------------------------------------------------------*/
void MessageHandler::handleUser()
{
	std::cout << "[DEBUG] USER: " << std::endl;
	std::cout << "[DEBUG] size params: " << _message.params.size() << std::endl;
	std::cout << "[DEBUG] first param: " << _message.params[1] << std::endl;

	if (_message.params.size() < 2 || _message.params[1].empty())
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS,
						"Not enough parameters");
		_client.setDisconnect(true);
		return;
	}
	if (_client.getUsernameSet())
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_ALREADYREGISTERED,
						"AlreadyRegistered");
		_client.setDisconnect(true);
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
	if (_client.getRegistered())
		_client.sendResponse(_server.getName(), IrcResponseCode::RPL_WELCOME, "Welcome to our super nice IRC server!");
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
void MessageHandler::handleWho()
{
	std::cout << "[DEBUG] WHO: " << std::endl;

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

		// Split the users string by spaces
		std::istringstream iss(users);
		std::string nick;

		std::cout << "[DEBUG] users = : " << users << std::endl;

		while (iss >> nick)
		{
			std::string cleanNick = nick;
			if (cleanNick[0] == '@')
				cleanNick = cleanNick.substr(1);

			Client* client = _server.getClient(cleanNick);	//not working anymore ?!
			std::cout << "[DEBUG] cleannick = '" << cleanNick << "'" << std::endl;
			if (client != nullptr)
			{
				std::string reply = channel->getName() + " " +
					client->getUsername() + " " +
					client->getHostname() + " " +
					_server.getName() + " " +
					client->getNick() + " " +
					(channel->isOperator(client) ? "H@" : "H") + " " +
					"1 " +
					client->getRealname();

				std::cout << "[DEBUG] reply = : " << reply << std::endl;

				client->sendResponse(_server.getName(),
					IrcResponseCode::RPL_WHOREPLY, reply);
					// channel->getName() + " " +
					// client->getUsername() + " " +
					// client->getHostname() + " " +
					// _server.getName() + " " +
					// client->getNick() + " " +
					// (channel->isOperator(client) ? "H@" : "H") + " " +
					// "1 " +
					// client->getRealname());
			}
		}
	}

	// if (_server.isChannel(mask))
	// {
		// _client.sendResponse(_server.getName(),
		// 		IrcResponseCode::RPL_WHOREPLY,
		// 		channel->getName() + " " + _client.getUsername() + " " + _client.getHostname()
		// 		+ " " + _server.getName() + " " + _client.getNick() + " something"  " :1 realname");
		// 		//"something" should be: H@ - User modes (H = here, @ = channel operator)
	// }
	_client.sendResponse(_server.getName(),
				IrcResponseCode::RPL_ENDOFWHO,
				mask);
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

