/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JoinUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 11:34:48 by maustel           #+#    #+#             */
/*   Updated: 2025/09/30 11:34:48 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MessageHandler.hpp"


/*------------------------------------------------------------------------------
// Send message to channel that <nick> has joined
// if topic send topic
// Send channel user list to client
------------------------------------------------------------------------------*/
void MessageHandler::sendJoinTopicUserlistMsg(Channel *channel)
{
	// Send message to channel that <nick> has joined
	std::string joinMsg = _client.getNick() + " [~" +
						_client.getUsername() + "@" +
						_client.getHostname() + "] has joined " + channel->getName() + "\n";
	channel->broadcast(joinMsg, &_client);
	std::string prefix = _client.getNick() + "!" + _client.getUsername() + "@"
						+ _client.getHostname() + " JOIN " + channel->getName() + " :";
	_client.sendMsg(prefix, joinMsg);

	//send topic if it exists
	std::string topic = channel->getTopic();
	if (!topic.empty())
	{
		std::string prefix = _client.getNick() + "!" + _client.getUsername()
						+ "@" + _client.getHostname();
		_client.sendResponse(prefix, IrcResponseCode::RPL_TOPIC,
							channel->getName() + " :" + topic);
	}

	// Send channel user list to client
	std::string users = channel->getJoinedUsers();
	if (!users.empty())
	{
		std::string prefix = _client.getNick() + "!@" + _client.getHostname();
		_client.sendResponse(prefix, IrcResponseCode::RPL_NAMREPLY,
							"* " + _message.params[1] + " " + users);
		_client.sendResponse(prefix, IrcResponseCode::RPL_ENDOFNAMES,
							_message.params[1] + " :End of /NAMES list.");
	}
	//maybe send also creation time
}
