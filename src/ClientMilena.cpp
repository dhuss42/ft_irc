/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientMilena.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 09:53:10 by maustel           #+#    #+#             */
/*   Updated: 2025/09/18 09:53:10 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/*------------------------------------------------------------------*/
/* sends error replies to client									*/
/*	- replies are patched together for the correct format for irssi */
/*		- every msg sent must end in \r\n							*/
/*		- :server 462 nickname :You may not reregister				*/
/*																	*/
/* think about using getIrcErrorResponse, but probably not useful	*/
/*------------------------------------------------------------------*/
void	Client::sendError(std::string name, IrcErrorCode code, std::string reply)
{
	reply = ":" + name + " " + getIrcErrorCodeString(code) + " " + getNick() + " :" + reply + "\r\n";
	std::cout << "[DEBUG] current error reply: " << reply << std::endl;
	std::cout << "[DEBUG] sendMsg to " << getNick() << " socket=" << _socket << " msg=" << reply << std::endl;
	if (send(_socket, reply.c_str(), reply.size(), 0) <= 0) // uncertain about the zero at the moment
	{
		throw (Errors(ErrorCode::E_SND)); // uncertain about wether it bubbles up correctly to the next catch
	}
}

/*------------------------------------------------------------------*/
/* sends replies to client											*/
/*	- replies are patched together for the correct format for irssi */
/*		- every msg sent must end in \r\n							*/
/*		- :server 001 nickname :Welcome to IRC Server				*/
/*																	*/
/*------------------------------------------------------------------*/
void	Client::sendResponse(std::string name, IrcResponseCode code, std::string reply)
{
	reply = ":" + name + " " + getIrcResponseCodeString(code) + " " + getNick() + " :" + reply + "\r\n";
	std::cout << "[DEBUG] current reply: " << name << std::endl;
	std::cout << "[DEBUG] sendMsg to " << getNick() << " socket=" << _socket << " msg=" << reply << std::endl;
	if (send(_socket, reply.c_str(), reply.size(), 0) <= 0) // uncertain about the zero at the moment
	{
		throw (Errors(ErrorCode::E_SND)); // uncertain about wether it bubbles up correctly to the next catch
	}
}
