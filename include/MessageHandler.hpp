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
	void handleJoin(void);	//done
	void handlePass(void);	//re-check
	void handleNick(void);	//re-check
	void handleUser(void);	//re-check
	void handleMode(void);	//done
	void handleWho(void);	//done
	void handlePing(void);	//done
	void handlePrivmsg(void);	//done
	void handlePart(void);	//will do david
	void handleQuit(void);	//will do david
	void handleKick(void);
	void handleInvite(void);
	void handleTopic(void);
	void handleNotice(void);

	//MODE Utils

	bool validateModeParameters(void);
	void processModes(Channel* channel);
	bool processInvMode(Channel* channel, bool setMode, bool setModeHasChanged);
	bool processTopicOpMode(Channel* channel, bool setMode, bool setModeHasChanged);
	bool processPasswordMode(Channel* channel, size_t i, bool setMode, bool setModeHasChanged);
	bool processUserLimitMode(Channel* channel, size_t i, bool setMode, bool setModeHasChanged);
	bool processOperatorMode(Channel* channel, size_t i, bool setMode, bool setModeHasChanged);
	void processChannelModes(Channel* channel, char mode, bool setMode, size_t i, bool setModeHasChanged);
	void processBanlistMode(Channel* channel);
	void sendActiveChannelModes(Channel* channel);
	void sendNotChannelOpErrorMessage(Channel* channel);
	void sendChangedModes(std::string returnMsg, Channel* channel);

	//JOIN utils

	void sendJoinTopicUserlistMsg(Channel* channel);
	void splitString(const std::string& str, char delimiter, std::vector<std::string>& tokens);

	//Nick utils
	void broadcastNicknameInChannels(std::string& newNick);
};


#endif
