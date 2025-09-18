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

void MessageHandler::handleCap(void)
{
	std::cout << "[DEBUG] CAP: " << std::endl;
	//change server to -> getServerName?
	if (_message.params[1] == "LS" && _message.params[2] == "302")
	{
		_client.sendMsg("irc_custom", "CAP * LS :");
		// sendMsg("irc_custom", "FARTING\nPIPI\nKAKA"); //change to available commands
	}
}

void MessageHandler::handleJoin(void)
{
	std::cout << "[DEBUG] JOIN: " << std::endl;
		Channel* channel = _server.createChannel(_message.params[1], &_client);
		if (channel)
			channel->addUser(&_client, _message.params[2]);
}

void MessageHandler::handlePass(void)
{
	std::cout << "[DEBUG] PASS: " << std::endl;
	if (_message.params[1].empty() || _message.params[1] != "123") //change 123 to getPassword
	{
		_client.sendError("irc_custom", IrcErrorCode::ERR_PASSWDMISMATCH, "wrong password! Refused!");
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


void MessageHandler::handleMode()
{
	std::cout << "[DEBUG] MODE: " << std::endl;
	//to do
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
