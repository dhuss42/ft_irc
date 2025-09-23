/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 14:12:30 by dhuss             #+#    #+#             */
/*   Updated: 2025/09/23 11:27:45 by dhuss            ###   ########.fr       */
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
		std::string	_password;

		sockaddr_in	_addr;

		// uint16_t	_portNbr;

		std::unordered_map <std::string, Client* > _clientList;
		std::unordered_map <std::string, Channel* > _channelList;
		std::vector <pollfd> _sockets;
		// map pollfd.fd to client object
		std::unordered_map <int, Client* > _clientfd;

	public:
		Server();
		Server(std::string portNbr, std::string password);
		~Server();

		// signals
		static void handleSignal(int sig);
		static void setupSignalHandler();

		void	closedConnection(std::vector<pollfd>::iterator &it);

		// make as much private / const / static as possible for clean code
		void	parseArgs(const std::string& portNbr, const std::string& password);
		void	initServer();
		void	serverLoop();
		void	handlePollRevents();
		void	newClient();

		//======= verify =======//
		bool	isChannel(const std::string& name) const;
		bool	isClient(const std::string& name) const;

		pollfd	createPollfd();

		//======= Add Channels and Clients =======//
		void	addChannel(Channel* channel);
		void	removeChannel(Channel* channel);
		Channel*	getChannel(std::string name);
		Channel*	createChannel(std::string& name, Client *client);
		std::unordered_map<std::string, Channel*> getChannelUnoMap(void);
		std::vector<std::string> getChannelVector(void);

		void	addClient(Client* client);
		void	removeClient(Client* client);
		Client*	getClient(const std::string& nick);

		//======= getters & setters =======//
		const	std::string& getName(void) const;
		const	std::string& getPassword(void) const;

		void	setShouldExit(void);
		bool	getShouldExit(void);
	};


#endif