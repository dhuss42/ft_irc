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

Message Client::parser(std::string raw_message)
{
	Message message;

	std::cout << "DEBUG [PARSER]: " << std::endl;
	std::stringstream ss(raw_message);
	std::string arg;

	//add check if ' ' (not valid) or ':' (all following works including ' ' get into params[i])
	while (ss >> arg)
	{
		message.params.push_back(arg);
	}

	for (auto it = message.params.begin(); it != message.params.end(); ++it)
	{
		std::cout << YELLOW << *it << RESET << std::endl;
	}
	return (message);
}
