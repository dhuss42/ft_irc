/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handler.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 16:49:01 by maustel           #+#    #+#             */
/*   Updated: 2025/09/18 16:49:01 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGEHANDLER_HPP
# define MESSAGEHANDLER_HPP

#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Message.hpp"
#include "IrcErrors.hpp"
#include "IrcResponses.hpp"

class Client;
class Server;
class Message;
class MessageHandler
{
	private:
		Client& _client;
		Message& _message;
		Server& _server;

		std::string _modeRet1;
		std::string _modeRet2;

	//using reference instead of pointer because I am sure they already exist
	public:

	//Constructor
	MessageHandler(Client& client, Message& message, Server& server);

	void handleCap(void);
	void handleJoin(void);
	void handlePass(void);
	void handleNick(void);
	void handleUser(void);
	void handleMode(void);
	void handleWhois(void);
	void handlePing(void);
	void handlePrivmsg(void);

	//MODE helpers
	bool validateModeParameters(void);
	bool processModes(Channel* channel);
	bool processParameterModes(Channel* channel, char mode, bool setMode, const std::string& param, bool setModeHasChanged);	//change to void (?)
	void processNonParameterModes(Channel* channel, char mode, bool setMode, bool setModeHasChanged);
};


#endif
