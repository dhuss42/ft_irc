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


//client pointer wird mitgeschickt, damit ich auf client und server methods zugreifen kann
void parseHandler(std::string rawMessage, Client &client, Server &server)
{
	Message message;

	// std::cout << YELLOW << "DEBUG [PARSER]: " << RESET << std::endl;
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
		handler.handleUser();
	else if (message.command == "MODE")
		handler.handleMode();
	else if (message.command == "WHOIS")
		handler.handleWhois();
	else if (message.command == "PING")
		handler.handlePing();
	else if (message.command == "PRIVMSG")
		handler.handlePrivmsg();	//still needs change to handler.

	// and other commands...

	else
		std::cout << RED << "We dont handle this command: " << message.command << "!" << RESET << std::endl;
		//send this message to client

}
