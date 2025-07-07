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
		sockaddr_in	_addr;

		// uint16_t	_portNbr;
		std::string	_password;

		// store client objects inside a map
		std::unordered_map <std::string, Client* > _clientList;
		// store channel objects inside a map or unorded map because faster
		std::unordered_map <std::string, Channel* > _channelList;

		// store pollfd structs inside a vector
		std::vector <pollfd> _sockets;
		// map pollfd.fd to client object
		std::unordered_map <int, Client* > _clientfd;
	public:
		Server();
		Server(std::string portNbr, std::string password);
		~Server();

		void	parseArgs(std::string portNbr, std::string password);
		void	initServer();
		void	addToPollFd(pollfd* toAdd);
};


#endif