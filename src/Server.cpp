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

/*----------------------*/
/* Constructor			*/
/*----------------------*/
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
/*----------------------*/
/* Destructor			*/
/*----------------------*/
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
/*----------------------------------------------------------------------*/
/* parse both args														*/
/*	- verify that portnbr is only digits								*/
/*	- convert to int													*/
/*	- verify that portnbr is withn valid range and usable				*/
/*	- convert to network byte order with htons							*/
/*		- done so that all machines understand the data consistantly	*/
/*----------------------------------------------------------------------*/
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

/*--------------------------------------*/
/* Creates a pollfd set to 0			*/
/*	- create pollfd						*/
/*	- memset it to 0 so no trash		*/
/*	- set to non-blocking				*/
/*--------------------------------------*/
pollfd	createPollfd()
{
	pollfd sock;

	memset(&sock, 0, sizeof(pollfd));
	// maybe distinguish with if for different types for .events
	sock.events = POLL_IN;

	try {
		if (fcntl(sock.fd, F_SETFL, O_NONBLOCK) == -1)
			throw (Errors(ErrorCode::E_FCNTL));
		// std::cout << GREEN "set sock.fd to nonblocking" WHITE << std::endl;
	}
	catch(const std::exception& e) {
		Errors::handleErrors(e);
	}
	
	return (sock);
}

/*------------------------------------------*/
/* initialise Server Object					*/
/*	- create a pollfd set to non-blocking	*/
/*	- open listening socket					*/
/*	- bin IP and port to socket				*/
/*	- listing for incoming connections		*/
/*	- add to sockets vector					*/
/*------------------------------------------*/
void	Server::initServer()
{
	pollfd sockfd = createPollfd();

	try {
		sockfd.fd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd.fd == -1)
			throw (Errors(ErrorCode::E_SCKFD));
		if (bind(sockfd.fd, (sockaddr*)&_addr, sizeof(_addr)) == -1)
			throw (Errors(ErrorCode::E_BND));
		if (listen(sockfd.fd, 32) == -1)
			throw (Errors(ErrorCode::E_LSTN));
		_sockets.push_back(sockfd);
	}
	catch(const std::exception& e) {
		Errors::handleErrors(e);
	}
}

// update pollfd() vector
// 	add new client fds
//		pushback new client fds
//	remove disconnected client fds
// 		erase client fd

// Server loop logic
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

/*--------------------------------------------------------------------------*/
/* receives messages from client											*/
/*	- recv stores msg in tmp buffer 										s*/
/*	- returns -1 on error and 0 on closed connection						*/
/*	- else returns nbr of bytes received 									*/
/*		- gets remainder from las recv call									*/
/*		- Loop: finds delimiters "\r\n" in received msg						*/
/*		- sets clients buffer to everything preceeding delimiters			*/
/*		- stores remaing message in a buffer and repeats the loop			*/
/*		- after loop the remainder is stored in clients remainder			*/
/*		- buffer is set to 0												*/
/*--------------------------------------------------------------------------*/
int	Server::receiveMsg(pollfd &connection, Client *client)
{
	std::cout << MAGENTA "[DEBUGG] ===== receiveMsg ======= " WHITE << std::endl;
	char	tmp[512] = {0};
	int		received = recv(connection.fd, tmp, sizeof(tmp), 0);

	if (received == -1)
		throw (Errors(ErrorCode::E_RCV)); // probably should not exit here
	else if (received == 0)
	{
		std::cout << CYAN << "[" << client->getNick() << "] closed their connection" << std::endl; // needs cleanup
		return (-1);
	}
	else
	{
		// std::cout << YELLOW "[DEBUGG] recv is non negative" WHITE << std::endl;
		
		std::string fullBuffer = client->getRemainder() + std::string(tmp, received);
		client->setRemainder("");
		
		std::size_t pos;
		while ((pos = fullBuffer.find("\r\n")) != std::string::npos)
		{
			client->setBuffer(fullBuffer.substr(0, pos));
			std::cout << BOLDCYAN << "[DEBUG] buffer: " << client->getBuffer() << RESET << std::endl;
			pseudoParser(client->getBuffer(), client);
			fullBuffer = fullBuffer.substr(pos + 2);;
		}
		client->setRemainder(fullBuffer);
		if (!fullBuffer.empty())
			std::cout << BOLDCYAN << "[DEBUG] remainder: " << client->getRemainder() << RESET << std::endl;
	}
	std::cout << GREEN << "[" << client->getNick() << "]" << " received: " << client->getBuffer() << WHITE << std::endl;
	client->setBufferToValue(0, client->getBufferSize());
	return (0);
}

/*------------------------------------------------------------------*/
/* sends replies to client											*/
/*	- replies are patched together for the correct format for irssi */
/*		- every msg sent must end in \r\n							*/
/*------------------------------------------------------------------*/
void	Server::sendMsg(int connection, std::string reply)
{
	reply = ":" + _name + " " + reply + "\r\n";
	if (send(connection, reply.c_str(), reply.size(), 0) <= 0) // uncertain about the zero at the moment
	{
		throw (Errors(ErrorCode::E_SND)); // uncertain about wether it bubbles up correctly to the next catch
	}
}


/*----------------------------------------------------------*/
/* NOT FOR THE FINAL PRODUCT								*/
/* "parses" the msgs sent by irssi							*/
/*	- only implemented for authentication handshake to work */
/* should be handled by the parser							*/
/*----------------------------------------------------------*/
void Server::pseudoParser(std::string message, Client* client)
{
	std::size_t pos = 0;
	(void) pos;

	if (message.find("PASS") == 0)
	{
		pos = message.find(" ");
		// compare what follows the space to the password
		// set registered to true
		client->setRegistered(true);
	}
	else if (message.find("CAP LS 302")  == 0)
	{
		sendMsg(client->getSocket(), "CAP * LS :");
	}
	else if (message.find("NICK") == 0)
	{
		client->setNick(message.substr(6));
		// copy every following space into client Nick
		client->setNickSet(true);
	}
	else if (message.find("USER") == 0)
	{
		// copy every following space into client User
		client->setUsernameSet(true);
	}
}

/*------------------------------------------------------------------*/
/* Adds new client to _sockets vector if authentication successful	*/
/*	- accepts incoming connection									*/
/*	- created new Client object										*/
/*																	*/
/* runns through irrsi authentication handshake						*/
/*  	1. [irssi] inquires capabilities							*/
/*			-> [ircserv] sends capabilties							*/
/*		2. [irssi] sends premature join msg							*/
/*		3. [irssi] sends CAP END									*/
/*		4. [irssi] sends PASS <password>							*/
/*		5. [irssi] sends NICK <nickname>							*/
/*		6. [irssi] sends USER <username>							*/
/*			-> [ircerv] sends Welcome message						*/
/*																	*/
/* needs work on success and failure handling of authentication		*/
/*	- on successfull authentication									*/
/*		- client.fd and client object are mapped (seems redundant)	*/
/*		- client fd is added to _sockets vector						*/
/*	- on failure													*/
/*		- fd should be closed and client object deleted				*/
/*------------------------------------------------------------------*/
void	Server::newClient()
{
	std::cout << "[DEBUGG] listening socket has revents & POLLIN" << std::endl;
	pollfd newConnection = createPollfd();

	newConnection.fd = accept(_sockets[0].fd, NULL, NULL); // not sure here with NULL NULL
	if (newConnection.fd == -1)
		throw (Errors(ErrorCode::E_ACCPT));
	else
	{
		std::cout << GREEN "[DEBUGG] created accepted Socket" WHITE << std::endl;
		Client* client = new Client(&newConnection.fd);
		
		while(!(client->getNickSet() && client->getUsernameSet() && client->getRegistered()))
		{
			if (receiveMsg(newConnection, client) == -1)
				return ;
		}
		sendMsg(newConnection.fd, "001 " + client->getNick() + " :Welcome to the IRC server"); // server replies need to be handled more gracefully

		// handle properly successfull connection and unsuccessfull connection

		// for (int counter = 2; counter > -1; counter--)
		// {
		// 	if (receiveMsg(newConnection, client) == -1)
		// 		return ;
		// 	std::cout << YELLOW "[DEBUGG] after recev MSG" WHITE << std::endl;
		// 	if (std::strcmp(client->getBufferPtr(), _password.c_str()) == 0)
		// 	{
		// 		std::cout << GREEN "[DEBUGG] password is correct" WHITE << std::endl;
		// 		sendMsg(newConnection, GREEN "password is correct" WHITE);
				_clientfd[newConnection.fd] = client; // seems redundant if fd is passed into constructor, server only needs list of clients then
				_sockets.push_back(newConnection);
		// 		break ;
		// 	}
		// 	else
		// 	{
		// 		if (counter == 0)
		// 		{
		// 			sendMsg(newConnection, RED "entered passord incorrect 3 times" WHITE);
		// 			std::cout << "[DEBUGG] entered passord incorrect 3 times" WHITE << std::endl;
		// 			close(newConnection.fd);
		// 			delete client;
		// 			break ;
		// 		}
		// 		else
		// 		{
		// 			std::string msg = RED "password is incorrect. Remaining tries: " WHITE;
		// 			msg += std::to_string(counter);
		// 			std::cout << "[DEBUGG] " << msg <<  WHITE << std::endl;
		// 			sendMsg(newConnection, msg);
				// }
			// }
		// }

	}
	std::cout << YELLOW "[DEBUGG] exiting newClient()" WHITE << std::endl;
}

/*----------------------------------------------------------------------------------*/
/* handles pollfd poll.revents() 													*/
/*	- checks if a new connection to the listening socket has been made				*/
/*		- if so it calls newClient, which adds new connections to _sockets vec		*/
/*	- else it iterates over _sockets vector											*/
/* 		- if the current pollfd in _sockets has revents POLL_IN -> data to read		*/
/*			- receives msg of that client and sends reply -> reply needs work still */
/*			- if receiving msg fails then connection is closed						*/
/*		- if current revents POLLHUB POLLERR or POLLINVAL							*/
/*			- connection is lost and resources cleaned up							*/
/*----------------------------------------------------------------------------------*/
void	Server::handlePollRevents()
{
	if (_sockets[0].revents & POLL_IN)
		newClient();
	else
	{
		// std::cout << YELLOW "[DEBUGG] entered else in handlePollRevents()" WHITE << std::endl;
		for (auto it = _sockets.begin(); it != _sockets.end();)
		{
			// std::cout << YELLOW "[DEBUGG] entered for in handlePollRevents()" WHITE << std::endl;
			if (it->revents & POLL_IN)
			{
				if (receiveMsg(*it, _clientfd[it->fd]) != -1)
				{
					sendMsg(it->fd, "[Server] Message received"); // needs to be updated
					++it;
				}
				else
				{
					close(it->fd); // handle closing fds inside client destructor
					_clientfd.erase(it->fd);
					it = _sockets.erase(it);
				}
			}
			else if (it->revents & (POLLHUP | POLLERR | POLLNVAL)) 
			{
				close(it->fd); // handle closing fds inside client destructor
				_clientfd.erase(it->fd);
				it = _sockets.erase(it);
			}
			else
				++it;
		}
	}
	// std::cout << YELLOW "[DEBUGG] exiting handlePollRevents()" WHITE << std::endl;
}


/*------------------------------------------------------------------------------*/
/* Loop that keeps Server running 												*/
/*	- Verifies that the servers listening socket is present at _sockets[0]		*/
/*	- Calls Poll to wait on multiple fd at the same time						*/
/*		- if -1 there is an error -> currently exits							*/
/*		- if 0 system timeout -> currently just sleeps (Placeholder)			*/
/*		- non negative															*/
/*			- nbr of elements whos revents have been set to non zero			*/
/*			- calls handle Pollevents for further processing					*/
/*------------------------------------------------------------------------------*/
void	Server::serverLoop()
{
	int ret = 0;

	while (true)
	{
		try {
			if (!_sockets.empty())
			{
				ret = poll(_sockets.data(), _sockets.size(), 0);
				if (ret == -1)
					throw (Errors(ErrorCode::E_PLL));
				else if (ret == 0)
					sleep(1);
				else
				{
					// std::cout << MAGENTA "[DEBUGG] poll() has returned a positive value" WHITE << std::endl;
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
