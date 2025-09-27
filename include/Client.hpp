/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 14:42:38 by dhuss             #+#    #+#             */
/*   Updated: 2025/09/27 13:01:02 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "ft_irc.hpp"
#include "IrcErrors.hpp"
#include "IrcResponses.hpp"
#include "Server.hpp"	//put class includes in the end of file(?)
#include "Channel.hpp"
#include "Message.hpp"	//put class includes in the end of file(?)
#include "MessageHandler.hpp"

class Server;
class Message;
class Channel;

class Client
{
	private:
		std::string _nick;
		std::string _username;
		std::string _realname;
		std::string _hostname;

		Server*		_server;
		int 		_socket;

		std::string _buffer;
		std::string	_remainder;
		//  write buffer ?

		// registration state
		bool	_registered = false;
		bool	_nickSet = false;
		bool	_usernameSet = false;

		bool	_disconnect = false;

		std::unordered_map<std::string, Channel*> _joinedChannels;

	public:
		Client() = delete;
		Client(const Client&) = delete;
		Client& operator=(const Client&) = delete;
		Client(int fd, Server* server);
		~Client();

	void	addToJoinedChannels(Channel* channel);
	// void	removeFromJoinedChannels(Channel* channel);
	void	removeFromAllJoinedChannels();

	void	sendResponse(std::string name, IrcResponseCode code, std::string reply);
	void	sendMsg(std::string name, std::string reply);
	void	sendError(std::string name, IrcErrorCode code, std::string reply);
	int		authentication(); // can be deleted once authentication is updated
	int		receiveMsg();

	//------------can be deleted once authentication is updated------------//
	void	setRegistered(bool state);
	void	setNickSet(bool state);
	void	setUsernameSet(bool state);
	bool	getRegistered(void);
	bool	getNickSet(void);
	bool	getUsernameSet(void);
	//---------------------------------------------------------------------//

	int		getSocket(void); // [DEBUGGING]

	void	setRealname(const std::string& str);
	void	setHostname(const std::string& str);
	void	setUsername(const std::string& str);
	void	setNick(const std::string& str);

	std::string getNick() const;
	std::string	getUsername();
	std::string getHostname();
	std::string getRealname();

	void	setDisconnect(bool value);
	bool	getDisconnect(void);

	Server* getServer(void);

	//handler functions
	// not sure if bool or void
	// bool handleCap(Message message);
	// bool handleJoin(Message message);
	// bool handlePass(Message message);
	// bool handleNick(Message message);
	// bool handleMode(Message message);
	// bool handleWhois(Message message);
	// bool handlePing(Message message);
	bool handleUser(Message message);
	bool handlePrivmsg(Message message);
};

void	parseHandler(std::string rawMessage, Client &client, Server &server);

#endif
