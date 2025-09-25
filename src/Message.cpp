/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:48:54 by maustel           #+#    #+#             */
/*   Updated: 2025/09/13 14:48:54 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"

Message::Message(): command(), params()
{}

Message::~Message()
{}

Message::Message(const Message &other)

{
	*this = other;
}

Message& Message::operator=(const Message& other)
{
	if (this == &other)
		return (*this);
	this->command = other.command;
	this->params = other.params;
	return (*this);
}

/*------------------------------------------------------------
Split message from client into command (/join) and parameters (#channel)
white spaces are already handled by irssi. Too many of them are eliminated
after ':'  Get everything after the colon in the original message
------------------------------------------------------------*/
void Message::splitMessage(std::string rawMessage)
{
	std::stringstream ss(rawMessage);
	std::string arg;

	while (ss >> arg)
	{
		if (arg[0] == ' ')
		{
			//white spaces are actually already handled by irssi
			std::cout << RED << "Error whitespace in parameter!" << RESET << std::endl;
			return ;
		}
		if (arg[0] == ':')
		{
			// Get everything after the colon in the original message
			// npos means not found
			size_t colonPos = rawMessage.find(':');
			if (colonPos != std::string::npos)
			{
				this->params.push_back(rawMessage.substr(colonPos + 1));
			}
			break;
		}
		this->params.push_back(arg);
	}
	this->command = *this->params.begin();
}

