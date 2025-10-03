/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 16:46:05 by maustel           #+#    #+#             */
/*   Updated: 2025/09/18 16:46:05 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/*------------------------------------------------------------------------------
Parse input into command and parameter
and call corresponding handler functions
------------------------------------------------------------------------------*/
void parseHandler(std::string rawMessage, Client &client, Server &server)
{
	if (rawMessage.empty() || rawMessage.find("\n") == 0)
		return ;

	Message message;
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
	else if (message.command == "WHO")
		handler.handleWho();
	else if (message.command == "PING")
		handler.handlePing();
	else if (message.command == "PRIVMSG")
		handler.handlePrivmsg();
	else if (message.command == "PART")
		handler.handlePart();
	else if (message.command == "QUIT")
		handler.handleQuit();
	else if (message.command == "KICK")
		handler.handleKick();
	else if (message.command == "TOPIC")
		handler.handleTopic();
	else if (message.command == "INVITE")
		handler.handleInvite();
	else
		client.sendError(server.getName(), IrcErrorCode::ERR_UNKNOWNCOMMAND, "Unknown command: " + message.command);
}
