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
		if (fcntl(sock.fd, F_SETFL, O_NONBLOCK) == -1)
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
int	Server::receiveMsg(pollfd &connection, Client *client)
{
	std::cout << MAGENTA "[DEBUGG] ===== receiveMsg ======= " WHITE << std::endl;
	char	tmp[512] = {0};
	int		received = recv(connection.fd, tmp, sizeof(tmp), 0);

	if (received == -1)
		throw (Errors(ErrorCode::E_RCV)); // probably should not exit here
	else if (received == 0)
	{
		std::cout << CYAN << "[" << client->getNick() << "] closed their connection" << std::endl;
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

// uncertain about the throw, wether they bubble up correctly to the next catch
// send messages
void	Server::sendMsg(int connection, std::string reply)
{
	reply = ":" + _name + " " + reply + "\r\n";
	if (send(connection, reply.c_str(), reply.size(), 0) <= 0) // uncertain about the zero at the moment
	{
		throw (Errors(ErrorCode::E_SND));
	}
}

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

// adds new client if requested connection is accepted
// new connection is added to _sockets vector
// also newConnection.fd and client object are mapped together
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
		Client* client = new Client();
		client->setSocket(&newConnection.fd);
		
		// authentication process
		// for irssi multiple messages are sent by the client first
		// 1. checks for capabilities
		// 	-> requires server to send capabilities
		// 		:ircserv CAP * LS :
		// 2. premature JOIN message
		// 3. CAP END
		// 4. PASS hallo
		// 5. NICK irssitest
		// 6. USER david david 127.0.0.1 :David
		while(!(client->getNickSet() && client->getUsernameSet() && client->getRegistered()))
		{
			if (receiveMsg(newConnection, client) == -1)
				return ;
		}
		sendMsg(newConnection.fd, "001 " + client->getNick() + " :Welcome to the IRC server");



		// also add nick and username after pw

		// for (int counter = 2; counter > -1; counter--)
		// {
		// 	if (receiveMsg(newConnection, client) == -1)
		// 		return ;
		// 	std::cout << YELLOW "[DEBUGG] after recev MSG" WHITE << std::endl;
		// 	if (std::strcmp(client->getBufferPtr(), _password.c_str()) == 0)
		// 	{
		// 		std::cout << GREEN "[DEBUGG] password is correct" WHITE << std::endl;
		// 		sendMsg(newConnection, GREEN "password is correct" WHITE);
				_clientfd[newConnection.fd] = client;
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
	if (_sockets[0].revents & POLL_IN) // could change to [0] because I know that this position in the array is filled and it is faster than .at because no checking
	{
		newClient();
	}
	else
	{
		std::cout << YELLOW "[DEBUGG] entered else in handlePollRevents()" WHITE << std::endl;
		for (auto it = _sockets.begin(); it != _sockets.end();)
		{
			std::cout << YELLOW "[DEBUGG] entered for in handlePollRevents()" WHITE << std::endl;
			if (it->revents & POLL_IN)
			{
				if (receiveMsg(*it, _clientfd[it->fd]) != -1)
				{
					sendMsg(it->fd, "[Server] Message received");
					++it;
				}
				else
				{
					close(it->fd);
					_clientfd.erase(it->fd);
					it = _sockets.erase(it);
				}
			}
			else if (it->revents & (POLLHUP | POLLERR | POLLNVAL)) 
			{
				close(it->fd);
				_clientfd.erase(it->fd);
				it = _sockets.erase(it);
			}
			else
				++it;
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
