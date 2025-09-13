/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/13 14:11:50 by maustel           #+#    #+#             */
/*   Updated: 2025/09/13 14:11:50 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP


#include "ft_irc.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"

class Message
{
	public:
		std::string					command;
		std::vector<std::string>	params;

		Message();
		~Message();
		Message(const Message &other);
		Message& operator=(const Message &other);
};


#endif
