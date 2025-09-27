/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ModeHelpers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 08:56:53 by maustel           #+#    #+#             */
/*   Updated: 2025/09/25 08:56:53 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MessageHandler.hpp"

/*------------------------------------------------------------------------------
Send active cahnnel modes and parameters into channel to user that asked for it
	- for command /mode #channel (qithout any parameters)
------------------------------------------------------------------------------*/
void MessageHandler::sendActiveChannelModes(Channel *channel)
{
	std::string prefix = _client.getNick() + "!" + _client.getUsername()
				+ "@" + _client.getHostname() + " PRIVMSG " + channel->getName() + " :";
	_client.sendResponse(prefix, IrcResponseCode::RPL_CHANNELMODEIS, "mode/" + channel->getName()
				+ " [" + channel->getActiveChannelModes() + channel->getActiveChannelParameters() + "]");
}

/*------------------------------------------------------------------------------
Send " You're not a channel operator" into channel to user that wanted to do
mode changes but has no operator privileges
------------------------------------------------------------------------------*/
void MessageHandler::sendNotChannelOpErrorMessage(Channel *channel)
{
	std::string prefix = _client.getNick() + "!" + _client.getUsername()
				+ "@" + _client.getHostname() + " PRIVMSG " + channel->getName() + " :";
	_client.sendError(prefix, IrcErrorCode::ERR_CHANOPRIVSNEEDED,
				channel->getName() + " You're not a channel operator");
}

/*------------------------------------------------------------------------------
Send all changed modes and parameters into channel to all users in that channel
------------------------------------------------------------------------------*/
void MessageHandler::sendChangedModes(std::string returnMsg, Channel *channel)
{
	std::string broadcastMsg = "mode/" + channel->getName() + " [" + returnMsg + "] by " + _client.getNick();
		channel->broadcast(broadcastMsg, &_client);
	std::string prefix = _client.getNick() + "!" + _client.getUsername() + "@" + _client.getHostname() + " PRIVMSG " + channel->getName() + " :";
		_client.sendMsg(prefix, broadcastMsg);
}

/*------------------------------------------------------------------------------
Process 'i' mode (invonly)
	- sets/removes inviteOnlyChannelFlag
	- if something changes here, it writes into return strings
------------------------------------------------------------------------------*/
bool MessageHandler::processInvMode(Channel* channel, bool setMode, bool setModeHasChanged)
{
	if (setMode == true && channel->getInvOnly() == false)
	{
		if (setModeHasChanged)
			this->_modeRet1 += "+";
		channel->setInvOnly(true);
		this->_modeRet1 += "i";
		return true;
	}
	else if (setMode == false && channel->getInvOnly() == true)
	{
		if (setModeHasChanged)
			this->_modeRet1 += "-";
		channel->setInvOnly(false);
		this->_modeRet1 += "i";
		return true;
	}
	return false;
}

/*------------------------------------------------------------------------------
Process 't' mode (topic operator)
	- sets / removes topicOperatorFlag (means that only operators can change topic)
	- if something changes here, it writes into return strings
------------------------------------------------------------------------------*/
bool MessageHandler::processTopicOpMode(Channel* channel, bool setMode, bool setModeHasChanged)
{
	if (setMode == true && channel->getTopicOp() == false)
	{
		if (setModeHasChanged)
			this->_modeRet1 += "+";
		channel->setTopicOp(true);
		this->_modeRet1 += "t";
		return true;
	}
	else if (setMode == false && channel->getTopicOp() == true)
	{
		if (setModeHasChanged)
			this->_modeRet1 += "-";
		channel->setTopicOp(false);
		this->_modeRet1 += "t";
		return true;
	}
	return false;
}

/*------------------------------------------------------------------------------
Process 'k' mode (password)
	- if no parameter is given, mode k command will be ignored
	- if something changes here, it writes into return strings
------------------------------------------------------------------------------*/
bool MessageHandler::processPasswordMode(Channel* channel, size_t i, bool setMode, bool setModeHasChanged)
{
	if (i < _message.params.size() && setMode == true)
	{
		if (setModeHasChanged)
			this->_modeRet1 += "+";
		channel->setPasswordToggle(true);
		channel->setPassword(_message.params[i]);
		this->_modeRet1 += "k";
		this->_modeRet2 += " " + _message.params[i];
		return true;
	}
	else if (setMode == false && channel->getPasswordToggle() == true)
	{
		if (setModeHasChanged)
			this->_modeRet1 += "-";
		channel->setPasswordToggle(false);
		this->_modeRet1 += "k";
		this->_modeRet2 += " *";
		return true;
	}
	return false;
}

/*------------------------------------------------------------------------------
Process 'l' mode (user limit)
	- checks if limit is an integer and >0
		-> if not, it will be ignored
	- if something changes here, it writes into return strings
------------------------------------------------------------------------------*/
bool MessageHandler::processUserLimitMode(Channel* channel, size_t i, bool setMode, bool setModeHasChanged)
{
	if (i < _message.params.size() && setMode == true)
	{
		size_t pos;
		int limit = std::stoi(_message.params[i], &pos);
		if (pos != _message.params[i].length() || limit <= 0)
			return false;
		if (setModeHasChanged)
			this->_modeRet1 += "+";
		channel->setUserLimitToggle(true);
		channel->setUserLimit(limit);
		this->_modeRet1 += "l";
		this->_modeRet2 += " " + _message.params[i];
		return true;
	}
	else if (setMode == false && channel->getUserLimitToggle() == true)
	{
		if (setModeHasChanged)
			this->_modeRet1 += "-";
		channel->setUserLimitToggle(false);
		this->_modeRet1 += "l";
		this->_modeRet2 += " *";
		return true;
	}
	return false;
}

/*------------------------------------------------------------------------------
Process 'o' mode (operator)
	- checks if nick which should get operator exists
	- if something changes here, it writes into return strings
------------------------------------------------------------------------------*/
bool MessageHandler::processOperatorMode(Channel* channel, size_t i, bool setMode, bool setModeHasChanged)
{
	if (i < _message.params.size() && setMode == true)
	{
		if (!_server.isClient(_message.params[i]))
		{
			//problem if _message.params[i] == this channel -> irssi window closes
			// solution is adding emptyspace for now
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHNICK, " " + _message.params[i]);	//should go trotzdem avanti
			return false;
		}
		if (setModeHasChanged)
			this->_modeRet1 += "+";
		channel->addOperator(_server.getClient(_message.params[i]));
		this->_modeRet1 += "o";
		this->_modeRet2 += " " + _message.params[i];
		return true;
	}
	else if (setMode == false)
	{
		if (setModeHasChanged)
			this->_modeRet1 += "-";
		channel->removeOperator(_server.getClient(_message.params[i]));
		this->_modeRet1 += "o";
		this->_modeRet2 += _message.params[i];
		return true;
	}
	return false;
}

/*------------------------------------------------------------------------------
Main orchestrator function to devide in explicit modes
------------------------------------------------------------------------------*/
void MessageHandler::processChannelModes(Channel* channel, char mode, bool setMode, size_t i, bool setModeHasChanged)
{
	switch (mode)
	{
		case 'i':
			processInvMode(channel, setMode, setModeHasChanged);
			break;
		case 't':
			processTopicOpMode(channel, setMode, setModeHasChanged);
			break;
		case 'k':
			processPasswordMode(channel, i, setMode, setModeHasChanged);
			break;
		case 'l':
			processUserLimitMode(channel, i, setMode, setModeHasChanged);
			break;
		case 'o':
			processOperatorMode(channel, i, setMode, setModeHasChanged);
			break;
	}
}

/*------------------------------------------------------------------------------
iterates through parameter and mode characters and calls corresponding modeFunction
	- i
	- t
	- k
	- l
	- o
------------------------------------------------------------------------------*/
void MessageHandler::processModes(Channel* channel)
{
	const std::string modeChars = "ikolt";
	bool setMode = true;
	bool setModeHasChanged = true;
	for(size_t i = 2; i < _message.params.size(); i++)
	{
		std::string mode = _message.params[i];
		size_t j = 0;
		while (j < mode.size())
		{
			if (mode[j] == '-' && setMode == true)
			{
				setMode = false;
				setModeHasChanged = true;
			}
			else if (mode[j] == '+' && setMode == false)
			{
				setMode = true;
				setModeHasChanged = true;
			}
			else if (modeChars.find_first_of(mode[j]) != std::string::npos)
			{
				processChannelModes(channel, mode[j], setMode, (i + 1), setModeHasChanged);
				setModeHasChanged = false;
			}
			if (((mode[j] == 'l' || mode[j] == 'o') && setMode == true
				&& (i + 1) < _message.params.size()) || mode[j] == 'k')
					i++;
			j++;
		}
	}
}

/*------------------------------------------------------------------------------
Checks for enough parameters and unknown mode characters
	- if error ocuurs, will send error message to client
	- handles each mode character of first parameter
	- if mode needs parameter, paramter will increment
	- if all mode parameters are handled, goes to next parameter and
		handles each mode character of that parameter
------------------------------------------------------------------------------*/
bool MessageHandler::validateModeParameters()
{
	const std::string allowedChars = "-+ikolt";
	for(size_t i = 2; i < _message.params.size(); i++)
	{
		std::string mode = _message.params[i];
		bool setMode = true;
		size_t j = 0;
		while (j < mode.size())
		{
			if (allowedChars.find_first_of(mode[j]) == std::string::npos)
			{
				_client.sendError(_server.getName(), IrcErrorCode::ERR_UNKNOWNMODE, "Unknown mode character " + std::string(1, mode[j]));
				return false;
			}
			else if (mode[j] == '-')
				setMode = false;
			else if (mode[j] == '+')
				setMode = true;
			else if ((mode[j] =='l' && setMode == true) || mode[j] == 'o')
			{
				if ((i + 1) >= _message.params.size() || _message.params[i + 1].empty())
				{
					_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS,
							"MODE Not enough parameters");
					return false;
				}
				i++;
			}
			else if (mode[j] =='k')
				i++;
			j++;
		}
	}
	return true;
}
