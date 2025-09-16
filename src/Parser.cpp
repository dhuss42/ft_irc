/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:15:33 by maustel           #+#    #+#             */
/*   Updated: 2025/09/13 14:15:33 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

bool Client::handleCap(Message message)
{
	std::cout << "[DEBUG] CAP: " << std::endl;
	//change server to -> getServerName?
	if (message.params[1] == "LS" && message.params[2] == "302")
	{
		sendMsg("irc_custom", "CAP * LS :");
		// sendMsg("irc_custom", "FARTING\nPIPI\nKAKA"); //change to available commands
	}
	return (true);
	}
	bool Client::handleJoin(Message message)
	{
		std::cout << "[DEBUG] JOIN: " << std::endl;

	// if (message.params[1].empty())
	// {
	// 	// sendMsg("irc_custom", "join is working"); //improve this reply maybe
	// 	return (true);
	// }

	std::size_t pos = message.params[1].find('#');
	if (pos == std::string::npos)
	{

	}
	else
	{
		if (_server->isChannel(message.params[1]))
			std::cout << "channel exists already" << std::endl;
		Channel *channel = _server->getChannel(message.params[1]);
		if (!channel)
		{
			std::cout << "[DEBUG] adding channel with name " << message.params[1] << std::endl;
			channel = new Channel(message.params[1]);
			_server->addChannel(channel);
		}
		channel->addUser(this);
	}
	return (true);
}

bool Client::handlePass(Message message)
{
	std::cout << "[DEBUG] PASS: " << std::endl;
	if (message.params[1].empty() || message.params[1] != "123") //change 123 to getPassword
	{
		// sendMsg("irc_custom", "wrong password! Refused!");
		// return (false);	//do not connect
	}
	this->_registered + true;
	return (true);
}

bool Client::handleNick(Message message)
{
	std::cout << "[DEBUG] NICK: " << std::endl;
	//error if nickname in use, otherwise accept
	if (message.params[1].empty())
		return false;	//throw exception
	this->_nick = message.params[1];
	this->_nickSet = true;
	return (true);
}

/*
	- USER <username> <hostname> <servername> :<realname> -> server: sends welcome message
*/
bool Client::handleUser(Message message)
{
	std::cout << "[DEBUG] USER: " << std::endl;
	if (message.params[1].empty())
		return false;	//throw exception
	// this->_username = message.params[1];
	this->_usernameSet = true;
	sendMsg("irc_custom", "Welcome to our super nice IRC-Server, " + this->_nick + "! <3");
	return (true);
}

bool Client::handleMode(Message message)
{
	std::cout << "[DEBUG] MODE: " << std::endl;
	//to do
	return (true);
}

bool Client::handleWhois(Message message)
{
	std::cout << "[DEBUG] WHOIS: " << std::endl;
	return (true);
}

bool Client::handlePing(Message message)
{
	std::cout << "[DEBUG] PING: " << std::endl;
	if (message.params[1].empty())
		return false;	//throw exception
	sendMsg("irc_custom", "PONG " + message.params[1]);
	return (true);
}


Message Client::parser(std::string rawMessage)
{
	Message message;

	std::cout << YELLOW << "DEBUG [PARSER]: " << std::endl;
	message.splitMessage(rawMessage);

	// std::cout << "Command: " << message.command << std::endl;
	// for (auto it = message.params.begin(); it != message.params.end(); ++it)
	// {
	// 	std::cout << YELLOW << *it << RESET << std::endl;
	// }

	// do try and catch here

	if (message.command == "CAP")
		handleCap(message);
	else if (message.command == "JOIN")
		handleJoin(message);
	else if (message.command == "PASS")
		handlePass(message);
	else if (message.command == "NICK")
		handleNick(message);
	else if (message.command == "USER")
		handleUser(message);
	else if (message.command == "MODE")
		handleMode(message);
	else if (message.command == "WHOIS")
		handleWhois(message);
	else if (message.command == "PING")
		handlePing(message);

		// and other commands...

	else
		std::cout << "We dont handle this command: " << message.command << "!" << std::endl;

	return (message);	//probably other return type would make more sense
}
