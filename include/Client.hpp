/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 14:42:38 by dhuss             #+#    #+#             */
/*   Updated: 2025/10/03 11:10:51 by dhuss            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "ft_irc.hpp"
#include "IrcErrors.hpp"
#include "IrcResponses.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Message.hpp"
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

	Channel*	getJoinedChannel(const std::string& name);
	bool	isJoinedChannel(const std::string& name);
	void	addToJoinedChannels(Channel* channel);
	void	removeFromJoinedChannels(const std::string& name);
	void	removeFromAllJoinedChannels();
	void	updateNick(const std::string& oldNick, const std::string& newNick);
	std::unordered_map<std::string, Channel*> getJoinedChannels(void);

	void	sendResponse(std::string name, IrcResponseCode code, std::string reply);
	void	sendMsg(std::string name, std::string reply);
	void	sendRaw(std::string msg);
	void	sendError(std::string name, IrcErrorCode code, std::string reply);
	int		receiveMsg();

	void	setRegistered(bool state);
	void	setNickSet(bool state);
	void	setUsernameSet(bool state);
	bool	getRegistered(void);
	bool	getNickSet(void);
	bool	getUsernameSet(void);

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
};

void	parseHandler(std::string rawMessage, Client &client, Server &server);

#endif
