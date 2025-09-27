/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 14:41:49 by dhuss             #+#    #+#             */
/*   Updated: 2025/09/27 16:19:49 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

std::string toLower(const std::string& str)
{
	std::string lowerCase = str;
	for (unsigned long i = 0; i < str.length(); i++)
		lowerCase[i] = std::tolower(str[i]);
	return (lowerCase);
}

//==================NICK==================//

/*------------------------------------------------------------------*/
/*	used for NICK command to verify name							*/
/*		- checks if empty or too long								*/
/*		- checks for invalid first chars like channel prefix		*/
/*		- check's if all chars in name are valid					*/
/*------------------------------------------------------------------*/
bool	verifyNickName(const std::string& name)
{
	if (name.empty() || name.size() > 15)
		return (false);
	char first = name[0];
	if (first == '$' || first == '|' || first == '#' || first == '&' || first == '+' || first == '%' || first == '~' || std::isdigit(first))
		return (false);
	for (const char &c : name) {
	switch (c) {
		case '[':
		case ']':
		case '{':
		case '}':
		case '\\':
		case '|':
			continue;
		default:
			if (std::isalnum(static_cast<unsigned char>(c))) {
				continue;
			}
			std::cout << "[DEBUG] Error in verify nickname: " << c << std::endl;
			return false;
	}
}
	return (true);
}

