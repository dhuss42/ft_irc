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
	switch (mode)
	{
		case 'i':
		{
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
bool MessageHandler::processParameterModes(Channel* channel, char mode, bool setMode, const std::string& param, bool setModeHasChanged)
{
	switch (mode)
	{
		case 'k':	//no param or param with -k -> will be ignored
		{
			if (!param.empty() && setMode == true)
			{
				if (setModeHasChanged)
					this->_modeRet1 += "+";
				channel->setPasswordToggle(true);
				channel->setPassword(param);
				this->_modeRet1 += "k";
				this->_modeRet2 += " " + param;
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
			if (!param.empty() && setMode == true)
			{
				size_t pos;
				int limit = std::stoi(param, &pos);
				if (pos != param.length())
					return false;
				if (setModeHasChanged)
					this->_modeRet1 += "+";
				channel->setUserLimitToggle(true);
				channel->setUserLimit(limit);
				this->_modeRet1 += "l";
				this->_modeRet2 += " " + param;
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
			if (!param.empty() && setMode == true)
			{
				if (!_server.isClient(param))
				{
					_client.sendError(_server.getName(), IrcErrorCode::ERR_NOSUCHCHANNEL, param);	//goes trotzdem avanti
					return false;
				}
				if (setModeHasChanged)
					this->_modeRet1 += "+";
				channel->addOperator(_server.getClient(param));
				this->_modeRet1 += "o";
				this->_modeRet2 += " " + param;
				return true;
			}
			else if (setMode == false)
			{
				if (setModeHasChanged)
					this->_modeRet1 += "-";
				channel->removeOperator(_server.getClient(param));
				this->_modeRet1 += "o";
				this->_modeRet2 += param;
			}
		}
			break;
	}
	return false;	//return tiype should be void probably
}

bool MessageHandler::processModes(Channel* channel)
{
	for(size_t i = 3; i < _message.params.size(); i++)
	{
		bool setMode = true;
		bool setModeHasChanged = false;
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
			else if (mode[j] == 'k' || mode[j] == 'l' || mode[j] == 'o')
			{
				processParameterModes(channel, mode[j], setMode, _message.params[i + 1], setModeHasChanged);
				if ((setMode == true && !_message.params[i + 1].empty()) || mode[j] == 'k')
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
	const std::string allowedChars = "-+ikolt";
	for(size_t i = 3; i < _message.params.size(); i++)
	{
		std::string mode = _message.params[i];
		bool setMode = true;
		size_t j = 0;
		while (j < mode.size())
		{
			if (allowedChars.find_first_of(mode[j]) == std::string::npos)
			{
				_client.sendError(_server.getName(), IrcErrorCode::ERR_UNKNOWNMODE, "Unknown mode character " + mode[j]);
				return false;
			}
			if (mode[j] == '-')
				setMode = false;
			else if (mode[j] == '+')
				setMode = true;
			if ((mode[j] =='l' && setMode == true) || mode[j] == 'o')
			{
				if ( _message.params[i + 1].empty())
				{
					_client.sendError(_server.getName(), IrcErrorCode::ERR_NEEDMOREPARAMS,
							"Parameter required for mode " + mode[j]);
					return false;
				}
				i++;
			}
			j++;
		}
	}
	return true;
}
