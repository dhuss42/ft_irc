/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 14:12:46 by dhuss             #+#    #+#             */
/*   Updated: 2025/07/07 16:03:54 by dhuss            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::string portNbr, std::string password)
{
	std::memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	// currently set to localhost
	_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	parseArgs(portNbr, password);
	// could set this directly in parsing if the portNbr is only needed there
	initServer();
}

// should make a separate file for freeing all resources
// maybe better to handle delete and filedescriptors in client class!!
Server::~Server()
{
	// frees allocated client objects
	for (auto iti = _clientfd.begin(); iti != _clientfd.end(); iti++)
	{
		if (iti->second)
			delete(iti->second);
	}
	// closes all open file descriptors
	for (auto it = _sockets.rbegin(); it != _sockets.rend(); it++)
	{
		if (it->fd >= 0)
			close(it->fd);
	}
}

// additional considerations to make regarding to safe password policy (could be toggled on and off inside config file)
	// min length, special chars, not allowed chars, uppecase/lowercase, digit required?
// also max length of the password?
// also empty string = no password or error?
void	Server::parseArgs(std::string portNbr, std::string password)
{
	try {
		if (all_of(portNbr.begin(), portNbr.end(), ::isdigit))
		{
			int port = std::stoi(portNbr);
			if (port >= 1024 && port <= 65535)
				_addr.sin_port = htons(port);
			else
				throw (Errors(ErrorCode::E_PRTNBR));
		}
		else
			throw (Errors(ErrorCode::E_PRT));
		if (all_of(password.begin(), password.end(), ::isprint))
			_password = password;
		else
			throw (Errors(ErrorCode::E_PSSWRD));
	}
	catch (const std::exception& e) {
		Errors::handleErrors(e);
	}
}

pollfd	createPollfd()
{
	pollfd sock;

	memset(&sock, 0, sizeof(pollfd));
	// maybe distinguish with if for different types for .events
	sock.events = POLL_IN;

	try {
		if (fcntl(sock.fd, F_SETFL, O_NONBLOCK) == -1) // not sure if I can place this here or after socket()
			throw (Errors(ErrorCode::E_FCNTL));
		std::cout << GREEN "set sock.fd to nonblocking" WHITE << std::endl;
	}
	catch(const std::exception& e) {
		Errors::handleErrors(e);
	}
	
	return (sock);
}

// setup server object
// open socket
// 		socket()
// set to non blocking
// 		fctnl()
// bind IP and port to socket
// 		bind()
// listen for incoming connections
// 		listen()
void	Server::initServer()
{
	pollfd sockfd = createPollfd();

	try {
		sockfd.fd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd.fd == -1)
			throw (Errors(ErrorCode::E_SCKFD));
		std::cout << GREEN "created sockfd" WHITE << std::endl;
		if (bind(sockfd.fd, (sockaddr*)&_addr, sizeof(_addr)) == -1)
			throw (Errors(ErrorCode::E_BND));
		std::cout << GREEN "bind IP address and port to socket" WHITE << std::endl;
		if (listen(sockfd.fd, 32) == -1) // now listens for 32 connections
			throw (Errors(ErrorCode::E_LSTN));
		std::cout << GREEN "listening on socket" WHITE << std::endl;
		_sockets.push_back(sockfd);
	}
	catch(const std::exception& e) {
		Errors::handleErrors(e);
	}
}

// create pollfd() vector
// 		1. listening socket()
//			.events = POLLIN for readable events
//		2-n. client FDs when they connect
// 			.events = POLLIN


// update pollfd() vector
// 	add new client fds
//		pushback new client fds
//	remove disconnected client fds
// 		erase client fd


// Server loop
// monitor socketfd and connected clients fd with poll()
// 	if socket fd has .revents & POLLIN a new client wants to connect
// 		accept() waiting clients
//			check for password
//			create client object
//			set to nonblocking fctnl()
//			add to pollfd (_sockets)
//	if client fd has .revents & POLLIN
// 		receive incoming messages recv()
// 			more complex steps with accumulating partial reads into a buffer
// 				read into client specific buffer
//				each call to recv() appends to the buffer
//				check if buffer contains one or more complete messages defined by \r\n
// 					create substring up to end of message
//					pass to parser
//					remove from buffer / remining bytes in buffer kept for next time (like get_next_line)
//			parse commands (strings)
//				tokenise and store (container/array of strings)
//			handle commands
//				check permissions of client
//				carry out commands
// 				update server/client/channel state
// 				send replies
//	if client fd has .revents & (POLLHUB | POLLERR | POLLNVAL); means client called close() or exit, error occured on the socket(), invalid fd
// 		clean up client object
// 		remove from pollfd

// receive messages
void	Server::receiveMsg(pollfd &connection, Client *client)
{
	int received = 0;

	client->setBuffer(0, 512); // not sure about reseting buffer here, could be problematic when multiple /r/n sequences in one message
	bool	loop = true;
	int		last = 0;
	int		tmp = 0;
	while (loop)
	{
		received = recv(connection.fd, client->getBufferPtr(), client->getBufferSize(), 0);
		if (received == -1)
			throw (Errors(ErrorCode::E_RCV)); // probably should not exit here
		else if (received == 0)
		{
			std::cout << CYAN << "[" << client->getNick() << "] closed their connection" << std::endl;
			loop = false;
		}
		else
		{
			tmp = received + last;
			last = tmp;
			for (int i = 0; i < tmp; i++)
			{
				if (client->getBufferPtr()[i] == '\r' && client->getBuffer()[i + 1] == '\n') // change to actual value $'\r\n'
				{
					loop = false;
					break ;
				}
			}
		}
		// loop backwards over the string and locate the two signs
		// if the two chars are found next to each other break the loop
	}
	std::cout << GREEN << "[" << client->getNick() << "]" << " received: " << client->getBuffer() << WHITE << std::endl;
}

// uncertain about the throw, wether they bubble up correctly to the next catch
// send messages
void	Server::sendMsg(pollfd &connection, std::string reply)
{
	if (send(connection.fd, reply.c_str(), std::strlen(reply.c_str()), 0) <= 0) // uncertain about the zero at the moment
	{
		throw (Errors(ErrorCode::E_SND));
	}
}

// adds new client if requested connection is accepted
// new connection is added to _sockets vector
// also newConnection.fd and client object are mapped together
void	Server::newClient()
{
	std::cout << "[DEBUGG] listening socket has revents & POLLIN" << std::endl;
	pollfd newConnection = createPollfd();

	newConnection.fd = accept(_sockets.at(0).fd, NULL, NULL); // not sure here with NULL NULL
	if (newConnection.fd == -1)
		throw (Errors(ErrorCode::E_ACCPT));
	else
	{
		std::cout << GREEN "[DEBUGG] created accepted Socket" WHITE << std::endl;
		Client* client = new Client();
		receiveMsg(newConnection, client);
		
		if (std::strcmp(client->getBufferPtr(), _password.c_str()) == 0)
		{
			std::cout << GREEN "[DEBUGG] password is correct" WHITE << std::endl;
			sendMsg(newConnection, GREEN "password is correct" WHITE);
	
			_clientfd[newConnection.fd] = client;
			_sockets.push_back(newConnection);
		}
		else
		{
			std::cout << RED "[DEBUGG] password is incorrect" WHITE << std::endl;
			sendMsg(newConnection, RED "password is incorrect" WHITE);
		}
	}
	std::cout << YELLOW "[DEBUGG] exiting newClient()" WHITE << std::endl;
}

// handles filedescriptor poll.revents()
// _socket[0] is always listening socket
// if its revents is POLL_IN
// 	a new client is added
// the other pollfds inside _socket are clients
// if they send a message their revents is set to POLL_IN
// 	server needs to handle receiving the message and also replying 
// if their revents is POLLHUB POLLERR or POLLINVAL
// 	connection is somehow lost and server needs to handle that + cleanup
void	Server::handlePollRevents()
{
	if (_sockets.at(0).revents & POLL_IN) // could change to [0] because I know that this position in the array is filled and it is faster than .at because no checking
	{
		newClient();
	}
	else
	{
		std::cout << YELLOW "[DEBUGG] entered else in handlePollRevents()" WHITE << std::endl;
		for (auto it = _sockets.begin(); it != _sockets.end(); it++)
		{
			std::cout << YELLOW "[DEBUGG] entered for in handlePollRevents()" WHITE << std::endl;
			if (it->revents & POLL_IN)
			{
				receiveMsg(*it, _clientfd[it->fd]);
				sendMsg(*it, "[Server] Message receivedt");
			}
			else if (it->revents & (POLLHUP | POLLERR | POLLNVAL)) 
			{
				std::cout << "connection lost" << std::endl;
			}
		}
	}
	std::cout << YELLOW "[DEBUGG] exiting handlePollRevents()" WHITE << std::endl;
}

// handles poll return values
// if non zero it calls handlePollRevents()
void	Server::serverLoop()
{
	int ret = 0;

	while (true)
	{
		try {
			if (!_sockets.empty())
			{
				// Returns a pointer to the first element of the vector's internal array
				ret = poll(_sockets.data(), _sockets.size(), 0);
				if (ret == -1)
					throw (Errors(ErrorCode::E_PLL));
				else if (ret == 0)
					sleep(1);
				else
				{
					std::cout << MAGENTA "[DEBUGG] poll() has returned a positive value" WHITE << std::endl;
					handlePollRevents();
				}
			}
			else
				throw (Errors(ErrorCode::E_SCKEMPTY));
		}
		catch(const std::exception& e) {
			Errors::handleErrors(e);
		}
	}
}