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

void MessageHandler::handlePrivmsg()
{
	// PRIVMSG #chan :hallo
	// currently segfaults because shitty pseudoparser
	std::cout << "[DEBUG] PRIVMSG" << std::endl;

	Channel *channel = _server.getChannel(_message.params[1]); // channel name can only be 4 chars -> better extract everything up to colon
	std::cout << "[DEBUG] channel name " << _message.params[1] << std::endl;
	if (channel)
		std::cout << "[DEBUG] exists" << _message.params[1] << std::endl;
	channel->broadcast(_message.params[2], &_client);
}
