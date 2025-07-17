/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 14:12:30 by dhuss             #+#    #+#             */
/*   Updated: 2025/07/07 16:02:47 by dhuss            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "ft_irc.hpp"
#include "Client.hpp"
#include "Channel.hpp"

// subject talks about optional config file
//	currently I am assuming the server object reads the file and
//	assigns the values inside the attributes of the server object
//	makes program more flexible and allows for changes without modifying the code

class Client;
class Channel;

class Server
{
	private:
		std::string _name = "irc_custom";
		sockaddr_in	_addr;

		// uint16_t	_portNbr;
		std::string	_password;

		// store client objects inside a map
		std::unordered_map <std::string, Client* > _clientList;
		// store channel objects inside a map or unorded map because faster
		std::unordered_map <std::string, Channel* > _channelList;

		// for receiving incoming messages
		char _buffer[512]; // don't need should be in client

		// store pollfd structs inside a vector
		std::vector <pollfd> _sockets;
		// map pollfd.fd to client object
		std::unordered_map <int, Client* > _clientfd;
	public:
		Server();
		Server(std::string portNbr, std::string password);
		~Server();

		// make as much private / const / static as possible for clean code
		void	parseArgs(std::string portNbr, std::string password);
		void	initServer();
		void	serverLoop();
		void	handlePollRevents();
		void	newClient();	

		int		receiveMsg(pollfd &connection, Client *client);
		void	sendMsg(int connection, std::string reply);

		// remove later only for authentication
		void pseudoParser(std::string message, Client* client);
	};


#endif