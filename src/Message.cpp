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
