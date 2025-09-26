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


// Process non-parameter modes (i, t)
void MessageHandler::processNonParameterModes(Channel* channel, char mode, bool setMode, bool setModeHasChanged)
{
	std::cout << "[DEBUG] mode without param: " << std::endl;
	switch (mode)
	{
		case 'i':
		{
			std::cout << "[DEBUG] mode: " << setMode << mode << std::endl;
			if (setMode == true && channel->getInvOnly() == false)
			{
				if (setModeHasChanged)
					this->_modeRet1 += "+";
				channel->setInvOnly(true);
				this->_modeRet1 += "i";
			}
			else if (setMode == false && channel->getInvOnly() == true)
			{
				if (setModeHasChanged)
					this->_modeRet1 += "-";
				channel->setInvOnly(false);
				this->_modeRet1 += "i";
			}
			break;
		}
		case 't':
		{
			std::cout << "[DEBUG] mode: " << setMode << mode << std::endl;
			if (setMode == true && channel->getTopicOp() == false)
			{
				if (setModeHasChanged)
					this->_modeRet1 += "+";
				channel->setTopicOp(true);
				this->_modeRet1 += "t";
			}
			else if (setMode == false && channel->getTopicOp() == true)
			{
				if (setModeHasChanged)
					this->_modeRet1 += "-";
				channel->setTopicOp(false);
				this->_modeRet1 += "t";
			}
			break;
		}
	}
}

// Process parameter-based modes (k, l, o)
bool MessageHandler::processParameterModes(Channel* channel, char mode, bool setMode, size_t i, bool setModeHasChanged)
{
	std::cout << "[DEBUG] mode param: " << _message.params[i] << std::endl;
	switch (mode)
	{
		case 'k':	//no param or param with -k -> will be ignored
		{
			std::cout << "[DEBUG] mode: " << setMode << mode << std::endl;
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
			}
			break;
		}
		case 'l': //no param with +l -> error. param with -l -> will not take param
		{
			std::cout << "[DEBUG] mode: " << setMode << mode << std::endl;
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
			}
			break;
		}

		case 'o': // no param -> always error
		{
			std::cout << "[DEBUG] mode: " << setMode << mode << std::endl;
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
			}
		}
			break;
	}
	return false;	//return tiype should be void probably
}

bool MessageHandler::processModes(Channel* channel)
{
	std::cout << "[DEBUG] process mode: " << std::endl;
	std::cout << "[DEBUG] _message.params.size() " << _message.params.size() << std::endl;
	for(size_t i = 2; i < _message.params.size(); i++)
	{
		bool setMode = true;
		bool setModeHasChanged = false;
		std::string mode = _message.params[i];
		std::cout << "[DEBUG] mode: " << mode << std::endl;
		size_t j = 0;
		while (j < mode.size())
		{
			std::cout << "[DEBUG] mode: " << j << " " << mode[j] << std::endl;
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
			else if (mode[j] == 'k' || mode[j] == 'l' || mode[j] == 'o')
			{
				processParameterModes(channel, mode[j], setMode, (i + 1), setModeHasChanged);
				if ((setMode == true && (i + 1) < _message.params.size()) || mode[j] == 'k')
					i++;
			}
			if (mode[j] == 'i' || mode[j] == 't')
				processNonParameterModes(channel, mode[j], setMode, setModeHasChanged);
			j++;
		}
	}
	return true;
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
