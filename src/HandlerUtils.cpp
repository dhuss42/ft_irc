/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlerUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 18:15:25 by maustel           #+#    #+#             */
/*   Updated: 2025/09/30 18:15:25 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MessageHandler.hpp"

void MessageHandler::broadcastNicknameInChannels(std::string &newNick)
{
	std::unordered_map<std::string, Channel*> joinedChannels = _client.getJoinedChannels();
	for (auto it = joinedChannels.begin(); it != joinedChannels.end(); ++it)
	{
		it->second->broadcast("", &_client, " NICK " + newNick);
	}
}


/*------------------------------------------------------------------------------
// Send message to channel that <nick> has joined
// if topic send topic
// Send channel user list to client
------------------------------------------------------------------------------*/
void MessageHandler::sendJoinTopicUserlistMsg(Channel *channel)
{
	std::string joinMsg = _client.getNick() + " [~" +
						_client.getUsername() + "@" +
						_client.getHostname() + "] has joined " + channel->getName() + "\n";
	channel->broadcast(joinMsg, &_client, "JOIN " + channel->getName());
	std::string prefix = _client.getNick() + "!" + _client.getUsername() + "@"
						+ _client.getHostname() + " JOIN " + channel->getName() + " :";
	_client.sendMsg(prefix, joinMsg);

	std::string topic = channel->getTopic();
	if (!topic.empty())
	{
		std::string prefix = _client.getNick() + "!" + _client.getUsername()
						+ "@" + _client.getHostname();
		_client.sendResponse(prefix, IrcResponseCode::RPL_TOPIC,
							channel->getName() + " :" + topic);
	}

	std::string users = channel->getJoinedUsers();
	if (!users.empty())
	{
		std::string prefix = _client.getNick() + "!@" + _client.getHostname();
		_client.sendResponse(prefix, IrcResponseCode::RPL_NAMREPLY,
							"* " + channel->getName() + " :" + users);
		_client.sendResponse(prefix, IrcResponseCode::RPL_ENDOFNAMES,
							channel->getName() + " :End of /NAMES list.");
	}
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