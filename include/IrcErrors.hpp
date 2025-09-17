/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcErrors.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maustel <maustel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 15:51:31 by maustel           #+#    #+#             */
/*   Updated: 2025/09/17 15:51:31 by maustel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCERRORS_HPP
# define IRCERRORS_HPP

#include "ft_irc.hpp"

enum class IrcErrorCode
{
	ERR_NOSUCHNICK = "401";			// No such nick/channel
	ERR_NOSUCHSERVER = "402";		// No such server
	ERR_NOSUCHCHANNEL = "403";		// No such channel
	ERR_CANNOTSENDTOCHAN = "404";	// Cannot send to channel
	ERR_TOOMANYCHANNELS = "405";	// Too many channels
	ERR_WASNOSUCHNICK = "406";		// There was no such nick
	ERR_TOOMANYTARGETS = "407";		// Too many targets
	ERR_NOORIGIN = "409";			// No origin specified
	ERR_INVALIDCAPCMD = "410";		// Invalid or missing CAP subcommand
	ERR_NORECIPIENT = "411";		// No recipient given
	ERR_NOTEXTTOSEND = "412";		// No text to send
	ERR_NOTOPLEVEL = "413";			// No top level domain
	ERR_WILDTOPLEVEL = "414";		// Wild card top level domain
	ERR_BADMASK = "415";			// Invalid mask
	ERR_UNKNOWNCOMMAND = "421";		// Unknown command
	ERR_NOMOTD = "422";				// MOTD file missing
	ERR_NOADMININFO = "423";		// No admin information available
	ERR_FILEERROR = "424";			// File error
	ERR_NONICKNAMEGIVEN = "431";	// No nickname given
	ERR_ERRONEUSNICKNAME = "432";	// Erroneous nickname
	ERR_NICKNAMEINUSE = "433";		// Nickname is already in use
	ERR_NICKCOLLISION = "436";		// Nickname collision
	ERR_UNAVAILRESOURCE = "437";	// Nick/channel is temporarily unavailable
	ERR_USERNOTINCHANNEL = "441";	// They aren't on that channel
	ERR_NOTONCHANNEL = "442";		// You're not on that channel
	ERR_USERONCHANNEL = "443";		// User is already on channel
	ERR_NOLOGIN = "444";			// User not logged in
	ERR_SUMMONDISABLED = "445";		// SUMMON command disabled
	ERR_USERSDISABLED = "446";		// USERS command disabled
	ERR_NOTREGISTERED = "451";		// You have not registered
	ERR_NEEDMOREPARAMS = "461";		// Not enough parameters --
	ERR_ALREADYREGISTRED = "462";	// Already registered
	ERR_NOPERMFORHOST = "463";		// No permission for operation
	ERR_PASSWDMISMATCH = "464";		// Password mismatch
	ERR_YOUREBANNEDCREEP = "465";	// You're banned
	ERR_KEYSET = "467";				// Channel key already set
	ERR_CHANNELISFULL = "471";		// Channel is full
	ERR_UNKNOWNMODE = "472";		// Unknown mode
	ERR_INVITEONLYCHAN = "473";		// Invite-only channel
	ERR_BANNEDFROMCHAN = "474";		// Banned from channel
	ERR_BADCHANNELKEY = "475";		// Bad channel key
	ERR_BADCHANMASK = "476";		// Bad channel mask
	ERR_NOCHANMODES = "477";		// Channel modes can't be changed
	ERR_BANLISTFULL = "478";		// Ban list is full
	ERR_NOPRIVILEGES = "481";		// No privileges
	ERR_CHANOPRIVSNEEDED = "482";	// You're not channel operator
	ERR_CANTKILLSERVER = "483";		// Can't kill server
	ERR_RESTRICTED = "484";			// Restricted command
	ERR_UNIQOPPRIVSNEEDED = "485";	// You need uniqop privileges
	ERR_NOOPERHOST = "491";			// No O-line for your host
	ERR_UMODEUNKNOWNFLAG = "501";	// Unknown MODE flag
	ERR_USERSDONTMATCH = "502";		// Cannot change or set mode for other
};
