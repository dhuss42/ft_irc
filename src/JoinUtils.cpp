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
	channel->broadcastUpdated(joinMsg, &_client, "JOIN " + channel->getName());
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
							"* " + channel->getName() + " :" + users);
		_client.sendResponse(prefix, IrcResponseCode::RPL_ENDOFNAMES,
							channel->getName() + " :End of /NAMES list.");
	}
	//maybe send also creation time
}

void MessageHandler::splitString(const std::string& str, char delimiter,
					std::vector<std::string>& tokens)
{
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimiter))
	{
		tokens.push_back(token);
	}
}
