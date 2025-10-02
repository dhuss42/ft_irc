/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MessageHandlerDavid.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dhuss <dhuss@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 11:49:11 by dhuss             #+#    #+#             */
/*   Updated: 2025/10/02 11:49:23 by dhuss            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MessageHandler.hpp"

/*----------------------------------------------------------------------*/
/* Quit																	*/
/*	- check if client send reason										*/
/*	- loop over channels that client is part of							*/
/* 	- for every channel extract recipients excluding quitter			*/
/*	- send quit message to recipients									*/
/*	- set Disconnect bool for client									*/
/*----------------------------------------------------------------------*/
void	MessageHandler::handleQuit(void)
{
	std::string	reason;
	if (_message.params.size() == 2)
		reason = _message.params[1];

	std::unordered_set<Client*> recipients;
	std::unordered_map<std::string, Channel*> joinedChannels = _client.getJoinedChannels();
	for (auto it = joinedChannels.begin(); it != joinedChannels.end(); ++it)
	{
		std::map<std::string, Client*> users = it->second->getUsers();
		for (auto iter = users.begin(); iter != users.end(); ++iter)
		{
			if (iter->second != &_client)
				recipients.insert(iter->second);
		}
	}
	for (auto iterate = recipients.begin(); iterate != recipients.end(); ++iterate)
		(*iterate)->sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "QUIT :" + reason);
	_client.setDisconnect(true);
}

/*----------------------------------------------------------------------*/
/* Part																	*/
/*	- if there are enough parameters									*/
/*	- check if client sent reason										*/
/*	- split channel parameters separated by ","							*/
/* 	- loop over channels to part from									*/
/*	- send part message to clients in channel							*/
/*	- update all Containers												*/
/*	- if channel is now empty delete channel object						*/
/*----------------------------------------------------------------------*/
void MessageHandler::handlePart(void)
{
	if (_message.params.size() < 2)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS, "Not enough parameters");
		return ;
	}
	std::string reason;
	if (_message.params.size() > 2)
		reason = _message.params[2];

	std::regex del(",");
	std::sregex_token_iterator it(_message.params[1].begin(), _message.params[1].end(), del, -1);
	std::sregex_token_iterator end;

	while (it != end)
	{
		std::string channelName = *it;
		Channel* channel = _server.getChannel(channelName);
		if (!channel)
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL, channelName);
		else if (!_client.isJoinedChannel(channelName))
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOTONCHANNEL, channelName + " :You're not on that channel"); // this is not sending correct format
		else
		{
			_client.sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "PART " + channelName + " " + reason);
			_client.removeFromJoinedChannels(channelName);
			channel->broadcastUpdated(reason, &_client, "PART " + channelName);
			channel->removeUser(&_client);
			if (channel->getNbrUsers() == 0)
				_server.removeChannel(channel);
		}
		++it;
	}
}

/*----------------------------------------------------------------------*/
/* Kick																	*/
/*	- check if there are enough parameters								*/
/*	- check if client send reason										*/
/*	- check if channel exists											*/
/*	- check if kicker is on channel										*/
/*	- check if kicker is operator										*/
/*	- check if to be kicked is on channel								*/
/* 	- loop over channels to part from									*/
/*	- send kick message to clients in channel							*/
/*	- update all Containers												*/
/*	- if channel is now empty delete channel object						*/
/*----------------------------------------------------------------------*/
void	MessageHandler::handleKick(void)
{
	if (_message.params.size() < 3)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS, "Not enough parameters");
		return ;
	}
	Client* kicked = _server.getClient(_message.params[2]);
	Channel* channel = _server.getChannel(_message.params[1]);
	if (!channel)
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL, _message.params[1]);
	else if (!_client.isJoinedChannel(_message.params[1]))
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NOTONCHANNEL, _message.params[1] + " :You're not on that channel");
	else if (!channel->isOperator(&_client))
		_client.sendError(_server.getName(), IrcErrorCode::ERR_CHANOPRIVSNEEDED, _message.params[1] + " :You're not channel operator");
	else if (!kicked || !channel->isUser(kicked))
		_client.sendError(_server.getName(), IrcErrorCode::ERR_USERNOTINCHANNEL, _message.params[2] + " " + _message.params[1] + " :They aren't on that channel");
	else
	{
		std::string reason;
		if (_message.params.size() == 4)
			reason = _message.params[3];
		channel->broadcastUpdated(reason, &_client, "KICK " + _message.params[1] + " " + kicked->getNick());
		_client.sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "KICK " + _message.params[1] + " " + kicked->getNick() + " :" + reason);
		channel->removeUser(kicked);
		kicked->removeFromJoinedChannels(_message.params[1]);
		if (channel->getNbrUsers() == 0)
			_server.removeChannel(channel);
	}
}

/*----------------------------------------------------------------------*/
/* Invite																*/
/*	- check if there are enough parameters								*/
/*	- check if channel exists											*/
/*	- check if inviter is on channel									*/
/*	- check if inviter is operator										*/
/*	- check if invited is on channel									*/
/*	- send invite message to invited 									*/
/*	- send response to inviter											*/
/*	- add invited to invite List										*/
/*----------------------------------------------------------------------*/
void	MessageHandler::handleInvite(void)
{
	if (_message.params.size() < 3)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS, "Not enough parameters");
		return ;
	}

	Client* invited = _server.getClient(_message.params[1]);
	Channel* channel = _server.getChannel(_message.params[2]);
	if (!channel)
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL, _message.params[2]);
	else if (!_client.isJoinedChannel(_message.params[2]))
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NOTONCHANNEL, _message.params[2] + " :You're not on that channel");
	else if (channel->getInvOnly() && !channel->isOperator(&_client))
		_client.sendError(_server.getName(), IrcErrorCode::ERR_CHANOPRIVSNEEDED, _message.params[2] + " :You're not channel operator");
	else if (!invited || channel->isUser(invited))
		_client.sendError(_server.getName(), IrcErrorCode::ERR_USERONCHANNEL, _message.params[1] + " " + _message.params[2] + " :is already on channel");
	else
	{
		_client.sendResponse(_server.getName(), IrcResponseCode::RPL_INVITING, invited->getNick() + " " + channel->getName());
		invited->sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "INVITE " + invited->getNick() + " " + _message.params[2]);
		channel->addInvUsers(invited);
	}
}

/*----------------------------------------------------------------------*/
/* Topic																*/
/*	- check if there are enough parameters								*/
/*	- check if second parameter is "-delete"							*/
/*		- assign channelName and topic accordingly						*/
/*	- check if channel exists											*/
/*	- check if changer is on channel									*/
/*	- check if changer is operator when only operator can change		*/
/*	- if only two args													*/
/*	- 	if topic is empty send topic not set							*/
/*	- 	else send topic													*/
/*	- if second parameter is "-delete" clear topic						*/
/*	- else change channel topic to topic								*/
/*	- send topic update message to everyone on channel					*/
/*----------------------------------------------------------------------*/
void	MessageHandler::handleTopic(void)
{
	if (_message.params.size() < 2)
	{
		_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS, "Not enough parameters");
		return ;
	}

	std::string	channelName = _message.params[1];
	std::string	topic = _message.params[2];
	if (_message.params[1] == "-delete")
	{
		channelName = _message.params[2];
		topic = _message.params[3];
	}
	else
	{
		Channel* channel = _server.getChannel(channelName);
		if (!channel)
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL, channelName);
		else if (!_client.isJoinedChannel(channelName))
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
		else if (channel->getTopicOp() && !channel->isOperator(&_client))
			_client.sendError(_server.getName(), IrcErrorCode::ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
		else
		{
			if (_message.params.size() < 3)
			{
				if (channel->getTopic().empty())
					_client.sendResponse(_server.getName(), IrcResponseCode::RPL_NOTOPIC, _client.getNick() + " " + channelName);
				else
					_client.sendResponse(_server.getName(), IrcResponseCode::RPL_TOPIC, _client.getNick() + " " + channelName);
			}
			else
			{
				if (_message.params[1] == "-delete")
					channel->setTopic("");
				else
					channel->setTopic(topic);
				channel->broadcastUpdated(topic, &_client, "TOPIC " + _message.params[1]);
				_client.sendMsg(_client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname(), "TOPIC " + _message.params[1] + " " + topic);
			}
		}
	}
}
