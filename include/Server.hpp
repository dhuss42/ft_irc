/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 14:12:30 by dhuss             #+#    #+#             */
/*   Updated: 2025/09/25 15:24:32 by dhuss            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "ft_irc.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Errors.hpp"

// subject talks about optional config file
//	currently I am assuming the server object reads the file and
//	assigns the values inside the attributes of the server object
//	makes program more flexible and allows for changes without modifying the code

class Client;
class Channel;

class Server
{
	private:
		const std::string	_name = "irc_custom";
		std::string			_password;
		sockaddr_in			_addr;

		std::unordered_map <std::string, Client* >	_clientList;
		std::unordered_map <std::string, Channel* >	_channelList;
		std::vector <pollfd> 						_sockets;
		std::unordered_map <int, Client* > 			_clientfd;

		void	removeAllClients();
		void	removeAllChannels();
		void	closeAllSockets();
		void	disconnectClient(std::vector<pollfd>::iterator &it);

		pollfd	createPollfd();
		void	parseArgs(const std::string& portNbr, const std::string& password);
		void	initServer();

		void	handlePollRevents();
		void	newClient();

		static void	handleSignal(int sig);

	public:
		Server();
		Server(std::string portNbr, std::string password);
		~Server();


		void		serverLoop();
		static void	setupSignalHandler();

		Channel*	createChannel(std::string& name, Client *client);
		void		addChannel(Channel* channel);
		void		removeChannel(Channel* channel);
		bool		isChannel(const std::string& name) const;
		Channel*	getChannel(const std::string& name);
		std::vector<std::string> getChannelVector(void);
		std::unordered_map<std::string, Channel*> getChannelUnoMap(void);

		void	uniqueNick(std::string& nick);
		bool	isClient(const std::string& name) const;
		Client*	getClient(const std::string& nick);

		const	std::string& getName(void) const;
		const	std::string& getPassword(void) const;
		void	setShouldExit(void);
		bool	getShouldExit(void);
	};


#endif