#include "Server.hpp"

// constructor
// create server object
// 	init attributes
// 		port
// 		password
// 		containers for clients and channels

// setup server object
// open socket
// 		socket()
// set to non blocking
// 		fctnl()
// bind IP and port to socket
// 		bind()
// listen for incoming connections
// 		listen()

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
//			add to pollfd 
//	if client fd has .revents & POLLIN
// 		receive incoming messages recv()
// 			more complex steps with accumulating partial reads into a buffer
// 				read into client specific buffer
//				each call to recv() appends to the buffer
//				check if buffer contains one or more complete messages defined by \r\n
// 					create substring up to end of message
//					pass to parser
//					remove from buffer / remining bytes in buffer kept for next time (like get_next_line)
//			parse commands
//			carry out commands
// 				update server/client/channel state
// 				send replies
//	if client fd has .revents & (POLLHUB | POLLERR | POLLNVAL); means client called close() or exit, error occured on the socket(), invalid fd
// 		clean up client object	
// 		remove from pollfd
