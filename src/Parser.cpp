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
		Channel* channel = _server->createChannel(message.params[1], this);
		if (channel)
			channel->addUser(this, message.params[2]);
	return (true);
}

bool Client::handlePass(Message message)
{
	std::cout << "[DEBUG] PASS: " << std::endl;
	if (message.params[1].empty() || message.params[1] != "123") //change 123 to getPassword
	{

		sendError("irc_custom", IrcErrorCode::ERR_PASSWDMISMATCH, "wrong password! Refused!");
		return (false);	//do not connect
	}
	this->_registered = true;
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
	- USER <username> <hostname> <servername> :<realname>
		-> server: sends welcome message when all flags are set
	ERR_NEEDMOREPARAMS
	ERR_ALREADYREGISTRED: If the client tries to send another USER message after registration
		-> "You may not reregister"
*/
bool Client::handleUser(Message message)
{
	std::cout << "[DEBUG] USER: " << std::endl;
	if (message.params[1].empty())
		return false;	//throw exception

	if (message.params.size() == 5)
	{
		this->_username = message.params[1];
		this->_hostname = message.params[2];
		this->_realname = message.params[4];
		std::cout << "[DEBUG] username: " << this->_username << std::endl;
		std::cout << "[DEBUG] hostname: " << this->_hostname << std::endl; // not sure if better to have servername here or IP Address, this seems slower than doing it constructor
		std::cout << "[DEBUG] realname: " << this->_realname << std::endl;
	}
	this->_usernameSet = true;
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

bool Client::handlePrivmsg(Message message)
{
	// PRIVMSG #chan :hallo
	// currently segfaults because shitty pseudoparser
	std::cout << "[DEBUG] PRIVMSG" << std::endl;

	Channel *channel = _server->getChannel(message.params[1]); // channel name can only be 4 chars -> better extract everything up to colon
	std::cout << "[DEBUG] channel name " << message.params[1] << std::endl;
	if (channel)
		std::cout << "[DEBUG] exists" << message.params[1] << std::endl;
	channel->broadcast(message.params[2], this);

	return (true);
}

//client pointer wird mitgeschickt, damit ich auf client und server methods zugreifen kann
Message Client::parser(std::string rawMessage)
{
	Message message;

	std::cout << YELLOW << "DEBUG [PARSER]: " << RESET << std::endl;
	message.splitMessage(rawMessage);

	try
	{
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
		else if (message.command == "PRIVMSG")
			handlePrivmsg(message);

		// and other commands...

		else
			std::cout << RED << "We dont handle this command: " << message.command << "!" << RESET << std::endl;
			// throw (Errors(ErrorCode::E_PSSWRD));
			//send this message to client
	}
	catch(const std::exception& e)
	{
		Errors::handleErrors(e);
	}




	return (message);	//probably other return type would make more sense
}
