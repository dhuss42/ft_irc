/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/07 14:12:46 by dhuss             #+#    #+#             */
/*   Updated: 2025/09/25 15:20:59 by dhuss            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

static volatile sig_atomic_t shouldExit = 0;
// maybe better with a bool inside class and a server pointer here

//================================> Orthodox Form <================================//

/*----------------------*/
/* Constructor			*/
/*----------------------*/
Server::Server(std::string portNbr, std::string password)
{
	std::memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	parseArgs(portNbr, password);
	// could set this directly in parsing if the portNbr is only needed there
	initServer();
}

/*----------------------*/
/* Copy Constructor		*/
/*----------------------*/

/*----------------------*/
/* Assignment Overload	*/
/*----------------------*/


/*----------------------*/
/* Destructor			*/
/*----------------------*/
Server::~Server()
{
	removeAllClients();
	removeAllChannels();
	closeAllSockets();
}

//================================> private member methods <================================//

//================ initialise Data ================//

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
void	Server::parseArgs(const std::string& portNbr, const std::string& password)
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
		Errors::handleErrors(e, this);
	}
}

/*--------------------------------------*/
/* Creates a pollfd set to 0			*/
/*	- create pollfd						*/
/*	- memset it to 0 so no trash		*/
/*	- set to non-blocking				*/
/*--------------------------------------*/
pollfd	Server::createPollfd()
{
	pollfd sock;

	memset(&sock, 0, sizeof(pollfd));
	sock.events = POLL_IN;
	try {
		if (fcntl(sock.fd, F_SETFL, O_NONBLOCK) == -1)
			throw (Errors(ErrorCode::E_FCNTL));
	}
	catch(const std::exception& e) {
		Errors::handleErrors(e, this);
	}
	return (sock);
}

/*--------------------------------------------------------------*/
/* initialise Server Object										*/
/*	- create a pollfd set to non-blocking						*/
/*	- open listening socket										*/
/*	- set options so it can be reused immediately after closing */
/*	- bin IP and port to socket									*/
/*	- listing for incoming connections							*/
/*	- add to sockets vector										*/
/*--------------------------------------------------------------*/
void	Server::initServer()
{
	pollfd sockfd = createPollfd();
	int opt = 1;

	try {
		sockfd.fd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd.fd == -1)
			throw (Errors(ErrorCode::E_SCKFD));
		if (setsockopt(sockfd.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
			throw (Errors(ErrorCode::E_SCKOPT));
		if (bind(sockfd.fd, (sockaddr*)&_addr, sizeof(_addr)) == -1)
			throw (Errors(ErrorCode::E_BND));
		if (listen(sockfd.fd, SOMAXCONN) == -1)
			throw (Errors(ErrorCode::E_LSTN));
		_sockets.push_back(sockfd);
	}
	catch(const std::exception& e) {
		Errors::handleErrors(e, this);
	}
}

//================ Incoming Revents ================//

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
	pollfd newConnection = createPollfd();

	newConnection.fd = accept(_sockets[0].fd, NULL, NULL); // not sure here with NULL NULL
	if (newConnection.fd == -1)
		throw (Errors(ErrorCode::E_ACCPT));
	else
	{
		Client* client = new Client(newConnection.fd, this);
		if (client->authentication() == -1) // this has to go once JOIN PASS NICK USER are ready
		{
			delete client;
		}
		else
		{
			_clientfd[newConnection.fd] = client;
			_clientList[toLower(client->getNick())] = client;
			_sockets.push_back(newConnection);
		}
		// handle properly successfull connection and unsuccessfull connection
	}
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
		for (auto it = _sockets.begin(); it != _sockets.end();)
		{
			if (it->revents & POLL_IN)
			{
				if (_clientfd[it->fd]->receiveMsg() != -1)
				{
					_clientfd[it->fd]->sendMsg(_name, "[Server] Message received"); // needs to be updated
					if (_clientfd[it->fd]->getDisconnect())
						disconnectClient(it);
					else
						++it;
				}
				else
					disconnectClient(it);
			}
			else if (it->revents & (POLLHUP | POLLERR | POLLNVAL))
				disconnectClient(it);
			else
				++it;
		}
	}
}

//================ Cleanup ================//

/*--------------------------------------*/
/* removes all clients					*/
/*--------------------------------------*/
void	Server::removeAllClients()
{
	for (auto iti = _clientfd.begin(); iti != _clientfd.end(); iti++)
	{
		if (iti->second)
			delete(iti->second);
	}
	_clientfd.clear();
	_clientList.clear();
}

/*--------------------------------------*/
/* removes all channels					*/
/*--------------------------------------*/
void	Server::removeAllChannels()
{
	for (auto it = _channelList.begin(); it != _channelList.end(); ++it)
	{
		if (it->second)
			delete(it->second);
	}
	_channelList.clear();
}

/*--------------------------------------*/
/* close all Sockets					*/
/*--------------------------------------*/
void	Server::closeAllSockets()
{
	for (auto it = _sockets.rbegin(); it != _sockets.rend(); it++)
	{
		if (it->fd >= 0)
			close(it->fd);
	}
	_sockets.clear();
}

/*------------------------------------------------------------------*/
/* handles cleanup for closing connections							*/
/*------------------------------------------------------------------*/
void	Server::disconnectClient(std::vector<pollfd>::iterator &it)
{
	Client* client = _clientfd[it->fd];

	if (client)
	{
		_clientList.erase(client->getNick());
		delete client;
		_clientfd.erase(it->fd);
	}
	it = _sockets.erase(it);
}

//================================> member methods <================================//

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

	while (!shouldExit)
	{
		try {
			if (!_sockets.empty())
			{
				ret = poll(_sockets.data(), _sockets.size(), 100);
				if (ret == -1)
				{
					if (shouldExit)
						return (setShouldExit());
					throw (Errors(ErrorCode::E_PLL));
				}
				else
					handlePollRevents();
			}
			else
				throw (Errors(ErrorCode::E_SCKEMPTY));
		}
		catch(const std::exception& e) {
			Errors::handleErrors(e, this);
		}
	}
}

//================ Signals ================//

/*-------------------------------------*/
/* handles events for incoming signals */
/*-------------------------------------*/
void	Server::handleSignal(int sig)
{
	switch (sig)
	{
	case SIGINT:
		std::cout << BLUE << "received SIGINT" << WHITE << std::endl;
		shouldExit = 1;
		break;
	case SIGKILL:
		std::cout << BLUE << "received SIGTERM" << WHITE << std::endl;
		shouldExit = 1;
		break;
	case SIGQUIT:
		std::cout << BLUE << "received SIGQUIT" << WHITE << std::endl;
		shouldExit = 1;
		break;
	default:
		std::cout << BLUE << "unhandled Signal: " << sig << WHITE << std::endl;
		break;
	}
}

/*--------------------------------------------------*/
/* handles signals like ctrl +c						*/
/*	- when ctrl + c is pressed calls handle signal	*/
/*--------------------------------------------------*/
void	Server::setupSignalHandler()
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handleSignal;
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, nullptr);
	sigaction(SIGTERM, &sa, nullptr);
	sigaction(SIGQUIT, &sa, nullptr);
	// add more signals here with sigaction
}

//================ channel management ================//

/*------------------------------------------------------------------*/
/* Creates a new Channel											*/
/*	- checks if the channel already exists (case insensitive)		*/
/*	- verifies the new channel will have a valid name				*/
/*	- creates a new Channel											*/
/*	- adds channel to map of known channels							*/
/*	- sets User who created channel as operator						*/
/*------------------------------------------------------------------*/
Channel*	Server::createChannel(std::string& name, Client *client) // can be altered to return values for sending replies or adding users afterwards
{
	std::string lcName = toLower(name);
	if (isChannel(lcName))
		return (getChannel(lcName)); // more of a patch at the moment
	else if (!verifyChannelName(lcName))
		return (nullptr);
	else
	{
		Channel* channel = new Channel(lcName);
		addChannel(channel);
		channel->addOperator(client);
		return (channel);
	}
}

/*--------------------------------------*/
/* add one channel to server			*/
/*--------------------------------------*/
void	Server::addChannel(Channel* channel)
{
	if (channel && !isChannel(channel->getName()))
		_channelList[channel->getName()] = channel;
}

/*--------------------------------------*/
/* remove one channel from server		*/
/*--------------------------------------*/
void	Server::removeChannel(Channel* channel)
{
	if (channel && isChannel(channel->getName()))
	{
		delete channel;
		_channelList.erase(channel->getName());
	}
}

/*--------------------------------------*/
/* return a Channel with <name>			*/
/*--------------------------------------*/
Channel*	Server::getChannel(const std::string& name)
{
	auto it = _channelList.find(toLower(name));
	if (it != _channelList.end())
		return (it->second);
	return (nullptr);
}

/*--------------------------------------*/
/* return _channelList as unordered map	*/
/*--------------------------------------*/
std::unordered_map<std::string, Channel*> Server::getChannelUnoMap(void)
{
	return (_channelList);
}

/*--------------------------------------*/
/* return _channelList as vector		*/
/*--------------------------------------*/
std::vector<std::string> Server::getChannelVector(void)
{
	std::vector<std::string> channelVec;
	for (auto it = _channelList.begin(); it != _channelList.end(); it++)
	{
		channelVec.push_back(it->first);
	}
	return (channelVec);
}

/*--------------------------------------*/
/* checks if Channel exists				*/
/*--------------------------------------*/
bool	Server::isChannel(const std::string& name) const
{
	std::string lcName = toLower(name);
	return (_channelList.find(lcName) != _channelList.end());
}

//================ Client management ================//
//<<<<<<<<<<<<<<<NICK>>>>>>>>>>>>//
/*------------------------------------------------------------------*/
/* Checks if the passed nick already exists on the server			*/
/*	- case insensitive (Dan, dan)									*/
/*------------------------------------------------------------------*/
bool	Server::isClient(const std::string& name) const
{
	std::string lcName = toLower(name);
	return (_clientList.find(lcName) != _clientList.end());
}

//<<<<<<<<<<<<<<<NICK>>>>>>>>>>>>//
/*------------------------------------------------------------------*/
/* Checks if the passed nick already exists on the server			*/
/*	- case insensitive (Dan, dan)									*/
/*	- if exists it adds an underscore and checks again				*/
/*------------------------------------------------------------------*/
void	Server::uniqueNick(std::string& nick)
{
	while (_clientList.find(toLower(nick)) != _clientList.end())
		nick += "_";
}

/*--------------------------------------*/
/* return a Client with <nick>			*/
/*--------------------------------------*/
Client*	Server::getClient(const std::string& nick)
{
	auto it = _clientList.find(toLower(nick));
	if (it != _clientList.end())
		return (it->second);
	return (nullptr);
}

//================ getters and setters ================//

/*--------------------------------------*/
/* return server name					*/
/*--------------------------------------*/
const std::string& Server::getName(void) const
{
	return (_name);
}

//<<<<<<<<<<<<<<<PASS>>>>>>>>>>>>//
/*--------------------------------------*/
/* return server password					*/
/*--------------------------------------*/
const std::string& Server::getPassword() const
{
	return (_password);
}

/*--------------------------------------*/
/* set value for sever to exit			*/
/*--------------------------------------*/
void	Server::setShouldExit(void)
{
	shouldExit = 1;
}

/*--------------------------------------*/
/* get value for sever to exit			*/
/*--------------------------------------*/
bool	Server::getShouldExit(void)
{
	return (shouldExit);
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