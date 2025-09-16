/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:15:33 by maustel           #+#    #+#             */
/*   Updated: 2025/09/13 14:15:33 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Message Client::parser(std::string rawMessage)
{
	Message message;

	std::cout << YELLOW << "DEBUG [PARSER]: " << std::endl;
	message.splitMessage(rawMessage);

	std::cout << "Command: " << message.command << std::endl;
	for (auto it = message.params.begin(); it != message.params.end(); ++it)
	{
		std::cout << YELLOW << *it << RESET << std::endl;
	}
	return (message);
}
