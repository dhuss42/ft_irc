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
		Message &_message;
		Server& _server;

		std::string _modeRet1;
		std::string _modeRet2;

	//using reference instead of pointer because I am sure they already exist
	public:

	//Constructor
	MessageHandler(Client& client, Message &message, Server& server);
	//Destructor
	~MessageHandler();

	void handleCap(void);	//done
	void handleJoin(void);	//not yet ready
	void handlePass(void);
	void handleNick(void);
	void handleUser(void);
	void handleMode(void);	//done
	void handleWhois(void);
	void handlePing(void);
	void handlePrivmsg(void);	//done
	void handlePart(void);	//will do david
	void handleQuit(void);	//will do david

	//MODE helpers

	bool validateModeParameters(void);
	void processModes(Channel* channel);
	bool processInvMode(Channel* channel, bool setMode, bool setModeHasChanged);
	bool processTopicOpMode(Channel* channel, bool setMode, bool setModeHasChanged);
	bool processPasswordMode(Channel* channel, size_t i, bool setMode, bool setModeHasChanged);
	bool processUserLimitMode(Channel* channel, size_t i, bool setMode, bool setModeHasChanged);
	bool processOperatorMode(Channel* channel, size_t i, bool setMode, bool setModeHasChanged);
	void processChannelModes(Channel* channel, char mode, bool setMode, size_t i, bool setModeHasChanged);
	void sendActiveChannelModes(Channel* channel);
	void sendNotChannelOpErrorMessage(Channel* channel);
	void sendChangedModes(std::string returnMsg, Channel* channel);

	// void processParameterModes(Channel* channel, char mode, bool setMode, size_t i, bool setModeHasChanged);	//change to void (?)
	// void processNonParameterModes(Channel* channel, char mode, bool setMode, bool setModeHasChanged);
};


#endif
