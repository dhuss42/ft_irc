#include "Server.hpp"

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
	toLower(name);
	if (isChannel(name))
	{
		std::cout << "[DEBUG] " << name << " exists already, not creating a new channel" << std::endl;
		return nullptr;
	}
	else if (!verifyChannelName(name))
	{
		std::cout << "[DEBUG] " << "invalid Channel name" << std::endl;
		return nullptr;
	}
	else
	{
		Channel* channel = new Channel(name);
		std::cout << "[DEBUG] " << " adding channel with name " << name << std::endl;
		addChannel(channel);
		channel->addOperator(client);
		return (channel);
		// not adding user to channel as this is might be better as a ordinary join functionality
	}
}