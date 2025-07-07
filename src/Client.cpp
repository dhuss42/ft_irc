#include "Client.hpp"

// Thoughts on Inheritance
// 	user inhertis from client
//  service inherits from client
//  moderator inherits from user

Client::Client()
{
}

Client::~Client()
{

}

// Constructor
// if a client makes a connection create Client object
// instantiate attributes
// 	socket fd
//	pointer to server
//
//  read buffer for the client
//  write buffer
//
//  a netwide unique identifier
//	nickname
//	username
//	hostname
//	modes
//	joined channels
//	registration state
//

// optional considerations
// 		away message
//		last activity timestamp
//

// Destructor
// 	close socket?
//	notify server to remove from pollfd and client list

// Buffer handling Methods
// append received data into read buffer

// check for complete lines in read buffer

// Clear Buffer when necessary
