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
	//using reference instead of pointer because I am sure they already exist
	public:
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
};

#endif
