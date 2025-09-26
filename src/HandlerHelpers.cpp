/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlerHelpers.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 08:56:53 by maustel           #+#    #+#             */
/*   Updated: 2025/09/25 08:56:53 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "MessageHandler.hpp"

// Process 'i' mode (invonly)
bool MessageHandler::processInvMode(Channel* channel, bool setMode, bool setModeHasChanged)
{
	std::cout << "[DEBUG] mode: " << setMode << " i" << std::endl;
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

// Process 't' mode (topic operator)
bool MessageHandler::processTopicOpMode(Channel* channel, bool setMode, bool setModeHasChanged)
{
	std::cout << "[DEBUG] mode: " << setMode << " t" << std::endl;
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

// Process 'k' mode (password)
bool MessageHandler::processPasswordMode(Channel* channel, size_t i, bool setMode, bool setModeHasChanged)
{
	std::cout << "[DEBUG] mode: " << setMode << " k" << std::endl;
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

// Process 'l' mode (user limit)
bool MessageHandler::processUserLimitMode(Channel* channel, size_t i, bool setMode, bool setModeHasChanged)
{
	std::cout << "[DEBUG] mode: " << setMode << " l" << std::endl;
	if (i < _message.params.size() && setMode == true)
	{
		size_t pos;
		int limit = std::stoi(_message.params[i], &pos);
		if (pos != _message.params[i].length())
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

// Process 'o' mode (operator)
bool MessageHandler::processOperatorMode(Channel* channel, size_t i, bool setMode, bool setModeHasChanged)
{
	std::cout << "[DEBUG] mode: " << setMode << " 0" << std::endl;
	std::cout << "[DEBUG] size params: " << _message.params.size() << " i = " << i << std::endl;
	if (i < _message.params.size() && setMode == true)
	{
		if (!_server.isClient(_message.params[i]))
		{
			_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHNICK, _message.params[i]);	//should go trotzdem avanti
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

// Main orchestrator function
void MessageHandler::processChannelModes(Channel* channel, char mode, bool setMode, size_t i, bool setModeHasChanged)
{
	std::cout << "[DEBUG] mode: " << mode << std::endl;

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

void MessageHandler::processModes(Channel* channel)
{
	std::cout << "[DEBUG] process mode: " << std::endl;
	const std::string allowedChars = "-+ikolt";
	bool setModeHasChanged = true;
	for(size_t i = 2; i < _message.params.size(); i++)
	{
		bool setMode = true;
		setModeHasChanged = false;
		std::string mode = _message.params[i];
		std::cout << "[DEBUG] mode: " << mode << std::endl;
		size_t j = 0;
		while (j < mode.size())
		{
			std::cout << "[DEBUG] mode: " << j << " " << mode[j] << std::endl;
			std::cout << "[DEBUG] mode: i =" << i << std::endl;
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
			else if (allowedChars.find_first_of(mode[j]) != std::string::npos)
				processChannelModes(channel, mode[j], setMode, (i + 1), setModeHasChanged);
			if (((mode[j] == 'l' || mode[j] == 'o') && setMode == true
				&& (i + 1) < _message.params.size()) || mode[j] == 'k')
					i++;
			j++;
		}
	}
}

bool MessageHandler::validateModeParameters()
{
	std::cout << "[DEBUG] validate mode: " << std::endl;
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
