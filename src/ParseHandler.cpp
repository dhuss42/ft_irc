/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 16:46:05 by maustel           #+#    #+#             */
/*   Updated: 2025/09/18 16:46:05 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

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
		return false;	//send Errormessage

	if (message.params.size() == 5)
	{
		// _client.setUsername()
		// _client.setHostname()
		// _client.setRealname()
		this->_username = message.params[1];
		this->_hostname = message.params[2];
		this->_realname = message.params[4];
		std::cout << "[DEBUG] username: " << this->_username << std::endl;
		std::cout << "[DEBUG] hostname: " << this->_hostname << std::endl; // not sure if better to have servername here or IP Address, this seems slower than doing it constructor
		std::cout << "[DEBUG] realname: " << this->_realname << std::endl;
	}
	setUsernameSet(true);
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
void parseHandler(std::string rawMessage, Client &client, Server &server)
{
	Message message;

	std::cout << YELLOW << "DEBUG [PARSER]: " << RESET << std::endl;
	message.splitMessage(rawMessage);
	MessageHandler handler(client, message, server);

	if (message.command == "CAP")
		handler.handleCap();
	else if (message.command == "JOIN")
		handler.handleJoin();
	else if (message.command == "PASS")
		handler.handlePass();
	else if (message.command == "NICK")
		handler.handleNick();
	else if (message.command == "USER")
		client.handleUser(message);	//need setter functions in client
	else if (message.command == "MODE")
		handler.handleMode();
	else if (message.command == "WHOIS")
		handler.handleWhois();
	else if (message.command == "PING")
		handler.handlePing();
	else if (message.command == "PRIVMSG")
		client.handlePrivmsg(message);	//still needs change to handler.

	// and other commands...

	else
		std::cout << RED << "We dont handle this command: " << message.command << "!" << RESET << std::endl;
		// throw (Errors(ErrorCode::E_PSSWRD));
		//send this message to client
}
